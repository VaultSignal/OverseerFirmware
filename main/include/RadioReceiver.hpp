#ifndef RADIO_RECEIVER_HPP
#define RADIO_RECEIVER_HPP
#include <memory>

namespace watcher
{
    class RadioReceiver
    {
    private:
        static std::unique_ptr<DeviceEvent> encodeDeviceEvent(uint8_t *payload);

    public:
        static void receiveMessages(NetworkClient client);
    }
};

#endif // RADIO_RECEIVER_HPP
