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
        uint8_t deviceID;
        uint8_t hasOpened;
        uint8_t hasMoved;
        uint8_t hasLight;
        float xAccel;
        float yAccel;
        float zAccel;
        uint32_t ldrOne;
        uint32_t ldrTwo;
        uint32_t ldrThree;
    };

};

#endif // DEVICE_PAYLOAD_HPP