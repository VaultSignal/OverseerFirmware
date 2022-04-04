#include "RadioReceiver.hpp"
#include "NetworkClient.hpp"
#include "WatcherController.hpp"

static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

void VaultSignal::RadioReceiver::receiveMessages(VaultSignal::NetworkClient &client)
{
    VaultSignal::DeviceEvent *event = new VaultSignal::DeviceEvent();
    event->deviceID = 23;
    event->eventID = 34;
    client.queueForUpload(*event);
    return;
    while (true)
    {
        // uint8_t[] payload = ...;
        // std::unique_ptr<DeviceEvent> event = this->encodeDeviceEvent(payload);
        // Send the encoded device event to the client.
        client.queueForUpload(*event);
    }
}

VaultSignal::RadioReceiver::RadioReceiver(VaultSignal::NetworkClient &client)
{
    this->connectionsReceived = 0;
    this->client = client;
}

VaultSignal::RadioReceiver::~RadioReceiver()
{
    gpio_isr_handler_remove(GPIO_INPUT_IO_0);
    this->connectionsReceived = 0;
}
