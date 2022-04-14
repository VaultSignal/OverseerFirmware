#ifndef WATCHER_CONTROLLER_HPP
#define WATCHER_CONTROLLER_HPP
#include "Arduino.h"
#include <time.h>

#define RF_SPI_SCLK 18
#define RF_SPI_MISO 19
#define RF_SPI_MOSI 23

namespace VaultSignal
{
    constexpr const char *TAG = "overseer_network";
    constexpr const char *RFTAG = "overseer_radio";
    constexpr const char *SYSTAG = "overseer_system";

    typedef enum LedPin : uint8_t
    {
        WIFI_PIN = 33,
        WEBSOCKET_PIN = 32,
        RADIO_PIN = 25
    } LedPin;

    typedef enum LedState : uint8_t
    {
        ON = HIGH,
        OFF = LOW
    } LedState;

    class WatcherController
    {
    public:
        /**
         * @brief Blink the built-in led of the device once.
         *
         * @param seconds_on Seconds it will stay lit.
         * @param seconds_off seconds it will stay unlit.
         */
        static void blinkLED(time_t secondsOn, time_t secondsOff, LedPin ledPin);
        /**
         * @brief Set the state of th LED.
         *
         * @param ledPin Pin connected to the LED.
         * @param state State of the LED to set.
         */
        static void setLEDState(LedPin ledPin, LedState state);
        /**
         * @brief Initialise the watcher device.
         *
         * Initialise the watcher device's state.
         */
        static void initialiseWatcher(void);
        static uint8_t deviceID;
    };

};

#endif // WATCHER_CONTROLLER_HPP