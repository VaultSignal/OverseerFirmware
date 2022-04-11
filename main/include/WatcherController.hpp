#ifndef WATCHER_CONTROLLER_HPP
#define WATCHER_CONTROLLER_HPP
#include <time.h>
#include <memory>
#include "esp_event.h"
#include "driver/gpio.h"
#include "RadioReceiver.hpp"

namespace VaultSignal
{
    constexpr const char *TAG = "vault_signal";
    namespace LedPin
    {
        constexpr const gpio_num_t WIFI_PIN = GPIO_NUM_33;
        constexpr const gpio_num_t WEBSOCKET_PIN = GPIO_NUM_32;
    };
    typedef enum LedState : uint8_t
    {
        ON = 1,
        OFF = 0
    } LedState;

    class WatcherController
    {
    private:
        /** This has to be private as it is triggered from interrupts */
        static std::unique_ptr<VaultSignal::RadioReceiver> receiver;
        /**
         * @brief Setup the radio receiver interrupt pins.
         *
         */
        static void setupRadioReceiverInterrupt(std::unique_ptr<VaultSignal::RadioReceiver> radioReceiver);
        /**
         * @brief The event handler for the Wifi events.
         *
         */
        static void wifiEventHandler(void *arg, esp_event_base_t event_base,
                                     int32_t event_id, void *event_data);
        static void gpio_isr_handler(void *arg);
        /**
         * @brief Event handler that takes an interrupt event and calls the radio receiver.
         *
         * @param arg
         */
        static void radio_receiver_event_wrapper(void *arg);

    public:
        /**
         * @brief Blink the built-in led of the device once.
         *
         * @param seconds_on Seconds it will stay lit.
         * @param seconds_off seconds it will stay unlit.
         */
        static void blinkLED(time_t secondsOn, time_t secondsOff, gpio_num_t ledPin);
        /**
         * @brief Set the state of th LED.
         *
         * @param ledPin Pin connected to the LED.
         * @param state State of the LED to set.
         */
        static void setLEDState(gpio_num_t ledPin, LedState state);
        /**
         * @brief Initialise the watcher device.
         *
         * Initialise the watcher device's state.
         */
        static void initialiseWatcher(void);
        /**
         * @brief Initalise the Wifi driver of the device with the given params.
         *
         * @param ssid Access point SSID
         * @param password Password to the access point.
         */
        static void initialiseWifi(std::string ssid, std::string password);
        /**
         * @brief Shutdown radio connections.
         *
         */
        static void tearDownRadio(void);
        static uint8_t deviceID;
    };
};

#endif // WATCHER_CONTROLLER_HPP