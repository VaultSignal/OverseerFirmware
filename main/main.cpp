#include "Arduino.h"
#include "WatcherController.hpp"
#include "NetworkClient.hpp"

extern "C" void app_main()
{
    initArduino();
    watcher::WatcherController::initialiseWatcher();
    watcher::NetworkClient client("Superbox_Wifi_9538", "DearLordIFinallyHaveInternet");
    while (true)
    {
        watcher::WatcherController::blinkBuiltinLED(250, 250);
    }
}