#ifndef DEVICE_PAYLOAD_HPP
#define DEVICE_PAYLOAD_HPP

namespace listener
{
    /**
     * @brief Represents an event emitted from a device in the
     * fleet.
     */
    struct DeviceEvent
    {
        uint16_t device_id;
        time_t event_timestamp;
        bool has_moved;
        bool has_opened;
    };

};

#endif // DEVICE_PAYLOAD_HPP