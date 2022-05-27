#include "Arduino.h"
#include "OverseerController.hpp"
#include "NetworkClient.hpp"
#include "RadioReceiver.hpp"
#include <thread>
#include "esp_log.h"

/**
 * @brief Controls the networking.
 *
 * Sends events to the WebSocket server as they are pushed to
 *  the event queue of the network client.
 * @param client Network client used by the event loop.
 */
void networkEventLoop(VaultSignal::NetworkClient &client)
{
    ESP_LOGI(VaultSignal::TAG, "Starting network thread.");
    client.sendEvents();
}

extern "C" void app_main()
{
    initArduino();
    VaultSignal::OverseerController::initialiseWatcher();
    auto credentials = VaultSignal::NetworkClient::initializeWifiProvisioning("VaultSignal Overseer");
    VaultSignal::NetworkClient client(credentials.ssid.c_str(), credentials.password.c_str());
    std::thread networkThread(networkEventLoop, std::ref(client));
    VaultSignal::RadioReceiver receiver;
    receiver.receiveMessages(client);
    networkThread.join();
}