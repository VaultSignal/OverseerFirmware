#ifndef RADIO_RECEIVER_HPP
#define RADIO_RECEIVER_HPP
#include <memory>
#include "NetworkClient.hpp"
#include "DeviceEvent.hpp"
#include "RF24.h"

/* How to determine sender and receiver */
#define NRF_MODE_SENDER 1
#define NRF_MODE_RECEIVER 2

namespace VaultSignal
{
    class RadioReceiver
    {
    private:
        RF24 *receiver;
        int connectionsReceived;
        std::unique_ptr<DeviceEvent> encodeDeviceEvent(uint8_t *payload);
        constexpr static const uint64_t deviceAddress = 0;
        /**
         * @brief Print the payload to the logger.
         *
         * @param payload Payload to print.
         * @param payloadSize Size of the payload in bytes.
         */
        void logPayload(uint8_t *payload, int payloadSize);

    public:
        /**
         * @brief Receive a group of messages and send them to the network client.
         *
         * @param client Client to send the messages to.
         */
        void receiveMessages(NetworkClient &client);
        RadioReceiver();
        ~RadioReceiver();
    };
};

#endif // RADIO_RECEIVER_HPP
