#ifndef NETWORK_CLIENT_HPP
#define NETWORK_CLIENT_HPP

#include "Arduino.h"
#include "WiFi.h"
#include "DeviceEvent.hpp"
#include <memory>

/**
 * @brief Maximum number of events that can be queued.
 */
const int NETWORK_EVENT_QUEUE_LIMIT = 20;

namespace watcher
{
    class NetworkClient
    {
    private:
        QueueHandle_t eventsQueue;
        std::unique_ptr<WiFiClient> client;

        /**
         * @brief Post a single event to the server.
         *
         * @param event Event to be posted.
         */
        void postToServer(const listener::DeviceEvent &event);

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
        void queueForUpload(const listener::DeviceEvent &event);

        /**
         * @brief Sends device events to the server.
         *
         * Send the device events to the server.
         */
        void sendEvents(void);
    };
};

#endif // NETWORK_CLIENT_HPP