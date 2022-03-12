#include "RadioReceiver.hpp"

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

VaultSignal::RadioReceiver::RadioReceiver()
{
    this->connectionsReceived = 0;
}

VaultSignal::RadioReceiver::~RadioReceiver()
{
    this->connectionsReceived = 0;
}
