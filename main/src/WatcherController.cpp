#include "WatcherController.hpp"
#include "Arduino.h"
#include "esp_mac.h"

uint8_t VaultSignal::WatcherController::deviceID = 0;

void VaultSignal::WatcherController::blinkLED(time_t secondsOn, time_t secondsOff, LedPin ledPin)
{
    digitalWrite(ledPin, LOW);
    delay((uint32_t)secondsOn);
    digitalWrite(ledPin, HIGH);
    delay((uint32_t)secondsOff);
}

void VaultSignal::WatcherController::setLEDState(LedPin ledPin, LedState state)
{
    digitalWrite(ledPin, state);
}

void VaultSignal::WatcherController::initialiseWatcher()
{
    Serial.begin(115200);
    pinMode(LedPin::WIFI_PIN, OUTPUT);
    pinMode(LedPin::WEBSOCKET_PIN, OUTPUT);
    esp_read_mac(&WatcherController::deviceID, ESP_MAC_WIFI_STA);
}