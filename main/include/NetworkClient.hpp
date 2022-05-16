#ifndef NETWORK_CLIENT_HPP
#define NETWORK_CLIENT_HPP

#include "Arduino.h"
#include "WiFi.h"
#include "DeviceEvent.hpp"
#include <memory>
#include "esp_websocket_client.h"

/**
 * @brief Maximum number of events that can be queued.
 */
const int NETWORK_EVENT_QUEUE_LIMIT = 20;
constexpr const char *HOST = "ws://192.168.1.3:8000/";
static constexpr esp_websocket_client_config_t WS_CONFIG = {
    .uri = HOST,
};

namespace VaultSignal
{

    typedef struct NetworkCredentials
    {
        const char *ssid;
        const char *password;
    } NetworkCredentials;

    class NetworkClient
    {
    private:
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
        void queueForUpload(DeviceEvent *event);

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
        static const NetworkCredentials &initializeWifiProvisioning(const char *apName);
    };
};

#endif // NETWORK_CLIENT_HPP