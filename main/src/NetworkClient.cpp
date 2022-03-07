#include "NetworkClient.hpp"
#include "WatcherController.hpp"

watcher::NetworkClient::NetworkClient(const char *ssid, const char *password)
{
    // Initialise the connection.
    this->eventsQueue = xQueueCreate(NETWORK_EVENT_QUEUE_LIMIT, sizeof(DeviceEvent));
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        WatcherController::blinkBuiltinLED(250, 250);
    }
    Serial.printf("WiFi Connected, IP Address:");
    Serial.print(WiFi.localIP());
}

void watcher::NetworkClient::queueForUpload(const listener::DeviceEvent &event)
{
    // Try to queue the event, if the queue fails, the package is 'dropped'.
    xQueueSend(this->eventsQueue, event, 0);
}

void watcher::NetworkClient::postToServer(const listener::DeviceEvent &event)
{
    ;
}

void watcher::NetworkClient::sendEvents(void)
{
    while (true)
    {
        // Within the event loop.
        listener::DeviceEvent currentEvent = listener::DeviceEvent();
        BaseType_t receivedEvent = xQueueReceive(this->eventsQueue, &currentEvent, 60);
        // One possible thing to do here is to switch of wifi when
        // No event is received.
        if (!receivedEvent)
        {
            continue;
        }
        else
        {
            this->postToServer(&currentEvent);
        }
    }
}