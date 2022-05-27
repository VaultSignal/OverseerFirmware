/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * VaultSignal Overseer Firmware - Firmware for Overseer
 * Copyright (C) 2022  VaultSignal
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "NetworkClient.hpp"
#include "WatcherController.hpp"
#include "esp_log.h"
#include "esp_websocket_client.h"
#include <ctime>
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"
#include <cstring>
#include <string>
#include <cstdlib>
static const int NO_DATA_TIMEOUT = 10;

IPAddress VaultSignal::NetworkClient::apIP = IPAddress(8, 8, 4, 4);

VaultSignal::NetworkClient::NetworkClient(const char *ssid, const char *password)
{
    esp_log_level_set(TAG, ESP_LOG_INFO);
    ESP_LOGI(TAG, "Initialising NetworkClient.");
    // Initialise the connection.
    this->eventsQueue = xQueueCreate(NETWORK_EVENT_QUEUE_LIMIT, sizeof(DeviceEvent));
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        ESP_LOGW(TAG, "Connection Failed...");
        WatcherController::blinkLED(125, 125, LedPin::WIFI_PIN);
    }
    ESP_LOGI(TAG, "WiFi Connected.");
    WatcherController::setLEDState(LedPin::WIFI_PIN, LedState::ON);
    this->client = esp_websocket_client_init(&WS_CONFIG);
    ESP_LOGI(TAG, "Websocket Client is Initialised");
}

VaultSignal::NetworkClient::~NetworkClient()
{
    esp_websocket_client_close(this->client, portMAX_DELAY);
    esp_websocket_client_destroy(this->client);
}

void VaultSignal::NetworkClient::queueForUpload(std::unique_ptr<DeviceEvent> event)
{
    // Try to queue the event, if the queue fails, the package is 'dropped'.
    xQueueSend(this->eventsQueue, event.get(), 0);
}

void VaultSignal::NetworkClient::postToServer(const DeviceEvent &event)
{
    ESP_LOGI(TAG, "Sending event.");
    std::time_t currentTime = std::time(0);
    static char data[10000];
    int len = sprintf(data, "{\"station_id\": \"%X\","
                            "\"device_id\": \"%i\","
                            "\"timestamp\": %li,"
                            "\"has_moved\": %i,"
                            "\"has_opened\": %i,"
                            "\"has_light\": %i,"
                            "\"accelerometer_data\": [%f, %f, %f],"
                            "\"light_sensor_data\": [%i, %i, %i]}",
                      WatcherController::deviceID,
                      event.deviceID,
                      currentTime,
                      event.hasMoved,
                      event.hasOpened,
                      event.hasLight,
                      event.xAccel, event.yAccel, event.zAccel,
                      event.ldrOne, event.ldrTwo, event.ldrThree);
    esp_websocket_client_send_text(this->client, data, len, portMAX_DELAY);
}

void VaultSignal::NetworkClient::connectClient(void)
{
    ESP_LOGI(TAG, "Connecting to WebSocket.");
    esp_websocket_client_start(this->client);
    while (!esp_websocket_client_is_connected(this->client))
    {
        WatcherController::blinkLED(125, 125, LedPin::WEBSOCKET_PIN);
    }
    ESP_LOGI(TAG, "WebSocket Client Connected.");
    WatcherController::setLEDState(LedPin::WEBSOCKET_PIN, LedState::ON);
}

void VaultSignal::NetworkClient::sendEvents(void)
{
    this->connectClient();
    ESP_LOGI(TAG, "Waiting for events...");
    while (true)
    {
        // Within the event loop.
        DeviceEvent currentEvent = DeviceEvent();
        BaseType_t receivedEvent = xQueueReceive(this->eventsQueue, &currentEvent, 60);
        // One possible thing to do here is to switch of wifi when
        // No event is received.
        if (!receivedEvent)
        {
            continue;
        }
        else
        {
            ESP_LOGI(TAG, "Event received.");
            this->postToServer(currentEvent);
        }
    }
}

// This portion is modified from https://iotespresso.com/create-captive-portal-using-esp32/
WebServer *VaultSignal::NetworkClient::setUpCaptivePortal(VaultSignal::NetworkCredentials *credentials, std::string captivePortal)
{
    auto *server = new WebServer(80);
    server->onNotFound([server, captivePortal]()
                       {
      server->send(200, "text/html", captivePortal.c_str()); 
      Serial.println("Client Connected"); });
    server->on("/get", [server, credentials]()
               {
  
      if (server->hasArg("ssid")) {
          credentials->ssid = server->arg("ssid").c_str();
      }

      if (server->hasArg("password")) {
          credentials->password = server->arg("password").c_str();
      }
      server->send(200, "text/plain", "Credentials accepted."); });
    return server;
}

const std::string VaultSignal::NetworkClient::generateCaptivePortalPage()
{
    const int numberOfNetworks = WiFi.scanNetworks();
    ESP_LOGI(APTAG, "%d APs found.", numberOfNetworks);
    std::string networks("");
    for (int i = 0; i < numberOfNetworks; i++)
    {
        const std::string networkSSID(WiFi.SSID(i).c_str());
        networks += "<option value=\"" + networkSSID + "\">" + networkSSID + "</option>\n";
    }
    ESP_LOGI(APTAG, "%s", networks.c_str());
    std::string inputForm(index_html);
    // Find the replace place.
    const std::string replacePattern("{options}");
    const int replaceLocation = inputForm.find(replacePattern);
    // Replace the options.
    inputForm.replace(replaceLocation, replacePattern.length(), networks);
    return inputForm;
}

const VaultSignal::NetworkCredentials VaultSignal::NetworkClient::initializeWifiProvisioning(const char *apName)
{
    NetworkCredentials credentials = {"", ""};
    esp_log_level_set(APTAG, ESP_LOG_INFO);
    WiFi.mode(WIFI_AP_STA);
    const std::string captivePortal = generateCaptivePortalPage();
    WiFi.softAP(apName);
    // As per https://github.com/espressif/arduino-esp32/blob/master/libraries/DNSServer/examples/CaptivePortal/CaptivePortal.ino
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    ESP_LOGI(APTAG, "Set up softAP mode, server is at: ");
    auto *server = NetworkClient::setUpCaptivePortal(&credentials, captivePortal);
    ESP_LOGI(APTAG, "Initialised tag");
    DNSServer dnsServer;
    // Redirect all network traffic to the SoftAP IP.
    dnsServer.start(53, "*", apIP);
    ESP_LOGI(APTAG, "Set up DNS Server.");
    server->begin();
    ESP_LOGI(APTAG, "Server initialised.");
    WatcherController::litAll();
    // From https://github.com/espressif/esp-idf/issues/1646
    // We are okay with the CPU spending time here.
    while (credentials.ssid == "" || credentials.password == "")
    {
        // Loop until we get all credentials
        dnsServer.processNextRequest();
        server->handleClient();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    ESP_LOGI(APTAG, "SSID is %s", (credentials.ssid.c_str()));
    WiFi.softAPdisconnect();
    server->stop();
    server->close();
    delete server;
    WatcherController::unlitAll();
    return credentials;
}