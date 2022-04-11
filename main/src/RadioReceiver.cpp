#include "RadioReceiver.hpp"
#include "NetworkClient.hpp"
#include "WatcherController.hpp"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

// Parts of this code is from https://github.com/espressif/esp-idf/tree/faed2a44aa/examples/peripherals/gpio/generic_gpio

VaultSignal::RadioReceiver::RadioReceiver(std::shared_ptr<VaultSignal::NetworkClient> client)
{
    this->connectionsReceived = 0;
    this->client = client;
}

VaultSignal::RadioReceiver::~RadioReceiver()
{
    this->connectionsReceived = 0;
}
