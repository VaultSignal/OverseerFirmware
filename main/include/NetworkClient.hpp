#ifndef NETWORK_CLIENT_HPP
#define NETWORK_CLIENT_HPP

#include "Arduino.h"
#include "WiFi.h"
#include "DeviceEvent.hpp"
#include <memory>
#include "esp_websocket_client.h"
#include <DNSServer.h>
#include <AsyncTCP.h>
#include <WebServer.h>
#include <string.h>
/**
 * @brief Maximum number of events that can be queued.
 */
const int NETWORK_EVENT_QUEUE_LIMIT = 20;
constexpr const char *HOST = "wss://overseer-websocket-simple.herokuapp.com";
static constexpr esp_websocket_client_config_t WS_CONFIG = {
    .uri = HOST};
extern const char index_html[] asm("_binary_getpassword_html_start");

namespace VaultSignal
{

    typedef struct NetworkCredentials
    {
        std::string ssid;
        std::string password;
    } NetworkCredentials;

    class NetworkClient
    {
    private:
        // As per https://github.com/espressif/arduino-esp32/blob/master/libraries/DNSServer/examples/CaptivePortal/CaptivePortal.ino
        static IPAddress apIP;
        QueueHandle_t eventsQueue;
        esp_websocket_client_handle_t client;
        /**
         * @brief Post a single event to the server.
         *
         * @param event Event to be posted.
         */
        void postToServer(const DeviceEvent &event);

        /**
         * @brief Connect to the websocket server.
         *
         */
        void connectClient(void);

        /**
         * Set up the captive request portal pages, return the set up asyncWebServer.
         */
        static WebServer *setUpCaptivePortal(NetworkCredentials *credentials, std::string captivePortal);

        /**
         * @brief Generate the page for captive portal.
         *
         * Generate HTML page for the captive portal by scanning for any WiFi
         * networks.
         *
         * @return std::string HTML page of the captive portal.
         */
        const static std::string generateCaptivePortalPage();

    public:
        /**
         * @brief Construct a new Network Client object
         *
         * Create a new NetworkClient object to connect to a Wireless network.
         *
         * @param ssid The WiFi SSID.
         * @param password Password for the network.
         */
        NetworkClient(const char *ssid, const char *password);
        /**
         * @brief Destroy the Network Client object
         *
         */
        ~NetworkClient();
        /**
         * @brief Queue a received device event for upload to server.
         *
         * Queue a device event for the server to be uploaded the server
         *  when the time permits.
         * @param event Event to upload.
         */
        void queueForUpload(std::unique_ptr<DeviceEvent> event);

        /**
         * @brief Sends device events to the server.
         *
         * Send the device events to the server.
         */
        void sendEvents(void);

        /**
         * @brief Create a Wifi Access point to get wifi information.
         *
         * @param apName Name of the access point.
         * @return  the object holding network credentials
         */
        static const NetworkCredentials initializeWifiProvisioning(const char *apName);
    };
};

#endif // NETWORK_CLIENT_HPP