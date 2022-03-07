#ifndef WATCHER_CONTROLLER_HPP
#define WATCHER_CONTROLLER_HPP
#include <time.h>

const int BUILTIN_LED_PIN = 33;

namespace watcher
{
    class WatcherController
    {
    public:
        /**
         * @brief Blink the built-in led of the device once.
         *
         * @param seconds_on Seconds it will stay lit.
         * @param seconds_off seconds it will stay unlit.
         */
        static void blinkBuiltinLED(time_t seconds_on, time_t seconds_off);
        /**
         * @brief Initialise the watcher device.
         *
         * Initialise the watcher device's state.
         */
        static void initialiseWatcher(void);
    };

};

#endif // WATCHER_CONTROLLER_HPP