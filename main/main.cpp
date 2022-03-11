#include "Arduino.h"
#include "WatcherController.hpp"
#include "NetworkClient.hpp"
#include <thread>

/**
 * @brief Controls the networking.
 *
 * Sends events to the WebSocket server as they are pushed to
 *  the event queue of the network client.
 * @param client Network client used by the event loop.
 */
void networkEventLoop(VaultSignal::NetworkClient &client)
{
    client.sendEvents();
}

extern "C" void app_main()
{
    initArduino();
    VaultSignal::WatcherController::initialiseWatcher();
    VaultSignal::NetworkClient client("Superbox_Wifi_9538", "DearLordIFinallyHaveInternet");
    std::thread networkThread(networkEventLoop, std::ref(client));
}