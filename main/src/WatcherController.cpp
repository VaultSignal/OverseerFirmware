#include "WatcherController.hpp"
#include "esp_mac.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include <chrono>
#include <thread>
#include <cstring>
#include <string>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

uint8_t VaultSignal::WatcherController::deviceID = 0;

constexpr static const int gpioInputPin = 14;
constexpr static const gpio_num_t GPIO_INPUT_IO_0 = GPIO_NUM_14;
static int s_retry_num = 0;
std::unique_ptr<VaultSignal::RadioReceiver> VaultSignal::WatcherController::receiver = nullptr;

/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

static const char *TAG = "wifi station";
static QueueHandle_t gpio_evt_queue = NULL;

void IRAM_ATTR VaultSignal::WatcherController::gpio_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t)arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

void VaultSignal::WatcherController::radio_receiver_event_wrapper(void *arg)
{
}

void VaultSignal::WatcherController::setupRadioReceiverInterrupt(std::unique_ptr<VaultSignal::RadioReceiver> radioReceiver)
{
    static bool interruptSet = false;
    receiver = std::move(radioReceiver);
    interruptSet = true;
    gpio_config_t io_conf = {};

    // Set the interrupt to happen on negative edge
    // Because that's how the radio receiver informs the IRQ pin.
    io_conf.intr_type = GPIO_INTR_NEGEDGE;
    io_conf.pin_bit_mask = 14;
    io_conf.mode = GPIO_MODE_INPUT;
    gpio_config(&io_conf);
    gpio_set_intr_type(GPIO_INPUT_IO_0, GPIO_INTR_ANYEDGE);
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    xTaskCreate(&radio_receiver_event_wrapper, "gpio_task_example", 2048, NULL, 10, NULL);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(GPIO_INPUT_IO_0, gpio_isr_handler, (void *)GPIO_INPUT_IO_0);
}

void VaultSignal::WatcherController::blinkLED(time_t secondsOn, time_t secondsOff, gpio_num_t ledPin)
{
    gpio_set_level(ledPin, 0);
    std::this_thread::sleep_for(std::chrono::seconds(secondsOn));
    gpio_set_level(ledPin, 1);
    std::this_thread::sleep_for(std::chrono::seconds(secondsOn));
}

void VaultSignal::WatcherController::setLEDState(gpio_num_t ledPin, LedState state)
{
    gpio_set_level(ledPin, state);
}

void VaultSignal::WatcherController::initialiseWatcher()
{
    // Verbatim from the https://docs.espressif.com
    const uart_port_t uart_num = UART_NUM_2;
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS,
        .rx_flow_ctrl_thresh = 122,
    };
    // Configure UART parameters
    ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));
    gpio_reset_pin(LedPin::WIFI_PIN);
    gpio_reset_pin(LedPin::WEBSOCKET_PIN);
    esp_read_mac(&WatcherController::deviceID, ESP_MAC_WIFI_STA);
}

void VaultSignal::WatcherController::wifiEventHandler(void *arg, esp_event_base_t event_base,
                                                      int32_t event_id, void *event_data)
{
    constexpr static const int ESP_MAXIMUM_ENTRY = 3;
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (s_retry_num < ESP_MAXIMUM_ENTRY)
        {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        }
        else
        {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG, "connect to the AP fail");
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

void VaultSignal::WatcherController::initialiseWifi(std::string ssid, std::string password)
{
    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &VaultSignal::WatcherController::wifiEventHandler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &VaultSignal::WatcherController::wifiEventHandler,
                                                        NULL,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = {};
    // Comes from https://www.esp32.com/viewtopic.php?t=18652
    std::strncpy((char *)wifi_config.sta.ssid, ssid.c_str(), ssid.size());
    std::strncpy((char *)wifi_config.sta.password, password.c_str(), password.size());
    wifi_config.sta.threshold.authmode = WIFI_AUTH_OPEN;
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_sta finished.");
}

void VaultSignal::WatcherController::tearDownRadio(void)
{
    gpio_isr_handler_remove(GPIO_INPUT_IO_0);
}
