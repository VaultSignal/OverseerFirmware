#include "NetworkClient.hpp"
#include "WatcherController.hpp"
#include "utility.hpp"
#include "esp_log.h"
#include "esp_websocket_client.h"
#include "CaptiveRequestHandler.hpp"
#include <ctime>
#include <cstring>
static const int NO_DATA_TIMEOUT = 10;

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
AsyncWebServer *setUpCaptivePortal(NetworkCredentials *credentials)
{
    auto *server = new AsyncWebServer(80);
    server->on("/", HTTP_GET, [](AsyncWebServerRequest *request)
               {
      request->send_P(200, "text/html", index_html); 
      Serial.println("Client Connected"); });

    server->on("/get", HTTP_GET, [credentials](AsyncWebServerRequest *request)
               {
  
      if (request->hasParam("ssid")) {
        credentials->ssid = request->getParam("ssid")->value();
      }

      if (request->hasParam("password")) {
        credentials->password = request->getParam("password")->value();
      }
      request->send(200, "text/html", "The values entered by you have been successfully sent to the device <br><a href=\"/\">Return to Home Page</a>"); });
    return server;
}

static const NetworkCredentials &initializeWifiProvisioning(const char *apName)
{
    NetworkCredentials credentials("", "");
    esp_log_level_set(APTAG, ESP_LOG_INFO);
    Wifi.mode(WIFI_AP);
    WiFi.softAP(apName);
    ESP_LOGI(APTAG, "Set up softAP mode.");
    auto *server = NetworkClient::setUpCaptivePortal(&credentials);
    ESP_LOGI(APTAG, "Initialised tag");
    DNSServer dnsServer;
    // Redirect all network traffic to the SoftAP IP.
    dnsServer.start(53, '*', WiFi.softAPIP());
    ESP_LOGI(APTAG, "Set up DNS Server.");
    server.addHandler(new CaptiveRequestHandler());
    ESP_LOGI(APTAG, "Server initialised.");
    while (credentials.ssid[0] == '\0' || credentials == '\0')
    {
        // Loop until we get all credentials
        dnsServer.processNextRequest();
    }
}