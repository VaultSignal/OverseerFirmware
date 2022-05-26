#include "NetworkClient.hpp"
#include "WatcherController.hpp"
#include "utility.hpp"
#include "esp_log.h"
#include "esp_websocket_client.h"
#include <ctime>
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"
#include <cstring>
#include <cstdlib>
static const int NO_DATA_TIMEOUT = 10;

IPAddress VaultSignal::NetworkClient::apIP = IPAddress(8, 8, 4, 4);

VaultSignal::NetworkClient::NetworkClient(const char *ssid, const char *password)
{
    esp_log_level_set(TAG, ESP_LOG_INFO);
    ESP_LOGI(TAG, "Initialising NetworkClient.");
    // Initialise the connection.
    this->eventsQueue = xQueueCreate(NETWORK_EVENT_QUEUE_LIMIT, sizeof(DeviceEvent));
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
WebServer *VaultSignal::NetworkClient::setUpCaptivePortal(VaultSignal::NetworkCredentials *credentials)
{
    auto *server = new WebServer(80);
    server->onNotFound([server]()
                       {
      server->send(200, "text/html", index_html); 
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

const VaultSignal::NetworkCredentials VaultSignal::NetworkClient::initializeWifiProvisioning(const char *apName)
{
    NetworkCredentials credentials = {"", ""};
    esp_log_level_set(APTAG, ESP_LOG_INFO);
    WiFi.mode(WIFI_AP);
    WiFi.softAP(apName);
    // As per https://github.com/espressif/arduino-esp32/blob/master/libraries/DNSServer/examples/CaptivePortal/CaptivePortal.ino
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    ESP_LOGI(APTAG, "Set up softAP mode, server is at: ");
    auto *server = NetworkClient::setUpCaptivePortal(&credentials);
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