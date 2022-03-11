#include "WatcherController.hpp"
#include "Arduino.h"
#include "esp_mac.h"

uint8_t VaultSignal::WatcherController::deviceID = 0;

void VaultSignal::WatcherController::blinkBuiltinLED(time_t seconds_on, time_t seconds_off)
{
    digitalWrite(BUILTIN_LED_PIN, LOW);
    delay((uint32_t)seconds_on);
    digitalWrite(BUILTIN_LED_PIN, HIGH);
    delay((uint32_t)seconds_off);
}

void VaultSignal::WatcherController::initialiseWatcher()
{
    Serial.begin(115200);
    pinMode(BUILTIN_LED_PIN, OUTPUT);
    esp_read_mac(&WatcherController::deviceID, ESP_MAC_WIFI_STA);
}