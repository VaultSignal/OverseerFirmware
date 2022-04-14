#include "RadioReceiver.hpp"
#include "WatcherController.hpp"
#include "freertos/FreeRTOS.h"
#include <cstring>
#include "freertos/task.h"

void VaultSignal::RadioReceiver::receiveMessages(VaultSignal::NetworkClient &client)
{
    this->receiver->begin();
    ESP_LOGI(RFTAG, "Message loop started.");
    WatcherController::blinkLED(100, 100, LedPin::RADIO_PIN);
    WatcherController::setLEDState(LedPin::RADIO_PIN, LedState::OFF);
    uint8_t buffer[28] = {0};
    this->receiver->openReadingPipe(0, this->deviceAddress);
    this->receiver->setPALevel(RF24_PA_MAX);
    this->receiver->startListening();
    while (true)
    {
        if (this->receiver->available())
        {
            ESP_LOGI(RFTAG, "Message Received");
            WatcherController::setLEDState(LedPin::RADIO_PIN, LedState::ON);
            this->receiver->read(buffer, 28);
            ESP_LOGI(RFTAG, "%i%i%i%i", buffer[0], buffer[1], buffer[2], buffer[3]);
            uint8_t *payload = (uint8_t *)malloc(sizeof(uint8_t) * 28);
            payload = (uint8_t *)std::memcpy(payload, buffer, 28);
            client.queueForUpload(reinterpret_cast<DeviceEvent *>(payload));
            WatcherController::setLEDState(LedPin::RADIO_PIN, LedState::OFF);
        }
        vTaskDelay(pdMS_TO_TICKS(250));
    }
}

VaultSignal::RadioReceiver::RadioReceiver()
{
    ESP_LOGI(RFTAG, "Initialising the radio receiver.");
    this->receiver = new RF24(4, 5);
    ESP_LOGI(RFTAG, "Radio receiver initialised.");
}

VaultSignal::RadioReceiver::~RadioReceiver()
{
    delete this->receiver;
    this->connectionsReceived = 0;
}
