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
        std::shared_ptr<NetworkClient> client;
        int connectionsReceived;
        std::unique_ptr<DeviceEvent> encodeDeviceEvent(uint8_t *payload);

    public:
        RadioReceiver(std::shared_ptr<VaultSignal::NetworkClient> client);
        ~RadioReceiver();
        void receiveMessage(void);
    };
};

#endif // RADIO_RECEIVER_HPP
