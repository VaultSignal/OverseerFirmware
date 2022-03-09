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
        uint8_t event_id;
        time_t event_timestamp;
        bool has_moved;
        bool has_opened;
        bool has_light;
        float xAccel;
        float yAccel;
        float zAccel;
        float ldrOne;
        float ldrTwo;
        float ldrThree;
    };

};

#endif // DEVICE_PAYLOAD_HPP