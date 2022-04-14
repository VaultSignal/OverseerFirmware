#include "NetworkClient.hpp"
#include "WatcherController.hpp"
#include "utility.hpp"
#include "esp_log.h"
#include "esp_websocket_client.h"
#include <ctime>
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

void VaultSignal::NetworkClient::queueForUpload(DeviceEvent *event)
{
    // Try to queue the event, if the queue fails, the package is 'dropped'.
    xQueueSend(this->eventsQueue, event, 0);
    free(event);
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