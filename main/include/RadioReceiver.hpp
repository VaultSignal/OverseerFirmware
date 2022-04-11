#ifndef RADIO_RECEIVER_HPP
#define RADIO_RECEIVER_HPP
#include <memory>
#include "NetworkClient.hpp"
#include "DeviceEvent.hpp"

namespace VaultSignal
{
    class RadioReceiver
    {
    private:
        NetworkClient &client;
        int connectionsReceived;
        std::unique_ptr<DeviceEvent> encodeDeviceEvent(uint8_t *payload);

    public:
        void receiveMessages(NetworkClient &client);
        void receiveMessage()
        RadioReceiver(NetworkClient &client);
        ~RadioReceiver();
    };
};

#endif // RADIO_RECEIVER_HPP
