#include "WatcherController.hpp"
#include "Arduino.h"
#include "esp_mac.h"
#include "hal/spi_types.h"
#include "driver/spi_common.h"

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
    ESP_LOGI(SYSTAG, "Initialising the pins.");
    pinMode(LedPin::WIFI_PIN, OUTPUT);
    pinMode(LedPin::WEBSOCKET_PIN, OUTPUT);
    pinMode(LedPin::RADIO_PIN, OUTPUT);
    esp_read_mac(&WatcherController::deviceID, ESP_MAC_WIFI_STA);
    // Initialise the SPI bus.
    // As per https://github.com/ReDFoX43rus/nRF24L01p-ESP32-Library
    ESP_LOGI(SYSTAG, "Initialising the SPI.");
    spi_bus_config_t buscfg;
    memset(&buscfg, 0, sizeof(buscfg));
    buscfg.miso_io_num = RF_SPI_MISO;
    buscfg.mosi_io_num = RF_SPI_MOSI;
    buscfg.sclk_io_num = RF_SPI_SCLK;
    buscfg.quadhd_io_num = -1;
    buscfg.quadwp_io_num = -1;
    buscfg.max_transfer_sz = 4096;
    esp_err_t err = spi_bus_initialize(HSPI_HOST, &buscfg, 1);
    assert(err == ESP_OK);
    ESP_LOGI(SYSTAG, "Device init complete.");
}