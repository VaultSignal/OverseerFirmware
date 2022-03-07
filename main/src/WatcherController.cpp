#include "WatcherController.hpp"
#include "Arduino.h"

void watcher::WatcherController::blinkBuiltinLED(time_t seconds_on, time_t seconds_off)
{
    digitalWrite(BUILTIN_LED_PIN, LOW);
    delay((uint32_t)seconds_on);
    digitalWrite(BUILTIN_LED_PIN, HIGH);
    delay((uint32_t)seconds_off);
}

void watcher::WatcherController::initialiseWatcher(void)
{
    pinMode(BUILTIN_LED_PIN, OUTPUT);
}