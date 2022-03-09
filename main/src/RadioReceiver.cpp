#include "RadioReceiver.hpp"
#include "DeviceEvent.hpp"
#include "NetworkClient.hpp"

static void watcher::RadioReceiver::receiveMessages(NetworkClient client)
{
    while (true)
    {
        uint8_t[] payload = ...;
        std::unique_ptr<DeviceEvent> event = this->encodeDeviceEvent(payload);
        // Send the encoded device event to the client.
        client->queueForUpload(event);
    }
}
