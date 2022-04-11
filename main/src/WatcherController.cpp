#include "WatcherController.hpp"
#include "Arduino.h"
#include "esp_mac.h"

uint8_t VaultSignal::WatcherController::deviceID = 0;

constexpr int gpioInputPin = 14

void VaultSignal::WatcherController::setupRadioReceiverInterrupt(void) {
    static bool interruptSet = false;
    interruptSet = true;
        //zero-initialize the config structure.
    gpio_config_t io_conf = {};

    //interrupt of rising edge
    io_conf.intr_type = GPIO_INTR_LOW;
    //bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = 14;
    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //enable pull-up mode
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    //change gpio interrupt type for one pin
    gpio_set_intr_type(GPIO_INPUT_IO_0, GPIO_INTR_ANYEDGE);

    //create a queue to handle gpio event from isr
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    //start gpio task
    xTaskCreate(gpio_task_example, "gpio_task_example", 2048, NULL, 10, NULL);

    //install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(GPIO_INPUT_IO_0, gpio_isr_handler, (void*) GPIO_INPUT_IO_0);
    //hook isr handler for specific gpio pin
}

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