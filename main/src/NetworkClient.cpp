#include "NetworkClient.hpp"
#include "WatcherController.hpp"
#include "utility.hpp"
#include "esp_log.h"
#include "esp_websocket_client.h"
#include <ctime>
static const char *TAG = "WEBSOCKET";
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
        delay(250);
    }
    ESP_LOGI(TAG, "WiFi Connected.");
    this->client = esp_websocket_client_init(&WS_CONFIG);
    ESP_LOGI(TAG, "Websocket Client Connected");
}

VaultSignal::NetworkClient::~NetworkClient()
{
    esp_websocket_client_close(this->client, portMAX_DELAY);
    esp_websocket_client_destroy(this->client);
}

void VaultSignal::NetworkClient::queueForUpload(const DeviceEvent &event)
{
    // Try to queue the event, if the queue fails, the package is 'dropped'.
    xQueueSend(this->eventsQueue, &event, 0);
}

void VaultSignal::NetworkClient::postToServer(const DeviceEvent &event)
{
    std::time_t currentTime = std::time(0);
    static char data[10000];
    int len = sprintf(data, "{\"station_id\": \"%X\","
                            "\"device_id\": \"%i\","
                            "\"event_id\": %i,"
                            "\"timestamp\": %li,"
                            "\"has_moved\": %i,"
                            "\"has_opened\": %i,"
                            "\"has_light\": %i,"
                            "\"accelerometer_data\": [%f, %f, %f],"
                            "\"light_sensor_data\": [%f, %f, %f]}",
                      WatcherController::deviceID,
                      event.deviceID,
                      event.eventID,
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
    esp_websocket_client_start(this->client);
    while (!esp_websocket_client_is_connected(this->client))
    {
        delay(250);
    }
}

void VaultSignal::NetworkClient::sendEvents(void)
{
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
            this->postToServer(currentEvent);
        }
    }
}