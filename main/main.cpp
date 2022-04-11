#include "WatcherController.hpp"
#include "NetworkClient.hpp"
#include "RadioReceiver.hpp"
#include <thread>
#include <memory>
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"

/**
 * @brief Controls the networking.
 *
 * Sends events to the WebSocket server as they are pushed to
 *  the event queue of the network client.
 * @param client Network client used by the event loop.
 */
void networkEventLoop(std::shared_ptr<VaultSignal::NetworkClient> client)
{
    ESP_LOGI(VaultSignal::TAG, "Starting network thread.");
    client->sendEvents();
}

extern "C" void app_main()
{
    nvs_flash_init();
    VaultSignal::WatcherController::initialiseWatcher();
    auto client = std::make_shared<VaultSignal::NetworkClient>("Superbox_Wifi_9538", "DearLordIFinallyHaveInternet");
    VaultSignal::RadioReceiver receiver(client);
    std::thread networkThread(networkEventLoop, client);
    networkThread.join();
}