#include "RadioReceiver.hpp"
#include "DeviceEvent.hpp"
#include "NetworkClient.hpp"

static void VaultSignal::RadioReceiver::receiveMessages(NetworkClient client)
{
    DeviceEvent event = new DeviceEvent();
    event->deviceID = 23;
    event->eventID = 34;
    client->queueForUpload(event);
    return;
    while (true)
    {
        // uint8_t[] payload = ...;
        // std::unique_ptr<DeviceEvent> event = this->encodeDeviceEvent(payload);
        // Send the encoded device event to the client.
        client->queueForUpload(event);
    }
}
