#ifndef DEVICE_PAYLOAD_HPP
#define DEVICE_PAYLOAD_HPP

namespace VaultSignal
{
    /**
     * @brief Represents an event emitted from a device in the
     * fleet.
     */
    struct DeviceEvent
    {
        uint16_t deviceID;
        uint8_t eventID;
        time_t eventTimestamp;
        bool hasMoved;
        bool hasOpened;
        bool hasLight;
        float xAccel;
        float yAccel;
        float zAccel;
        float ldrOne;
        float ldrTwo;
        float ldrThree;
    };

};

#endif // DEVICE_PAYLOAD_HPP