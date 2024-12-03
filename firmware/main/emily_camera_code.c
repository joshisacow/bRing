#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <driver/i2c.h>
#include <driver/gpio.h>
#include <driver/ledc.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_http_server.h>
#include <esp_log.h>
#include <nvs_flash.h>

#include "esp_http_client.h"

//Wi-Fi Management
#include "esp_wifi.h" //Contains functions to initialize Wi-Fi, connect to networks, and handle Wi-Fi events.
#include "esp_event.h" //Event handling system for Wi-Fi, TCP/IP stack, and other peripherals.
#include "esp_netif.h" //Simplifies network interface management for both Wi-Fi and Ethernet connections.
#include "nvs_flash.h" //Manages non-volatile storage to save data that persists across resets, such as Wi-Fi credentials.

// GPIO definitions for OV7670
#define SDA_GPIO GPIO_NUM_35
#define SCL_GPIO GPIO_NUM_36
#define D0_GPIO GPIO_NUM_19
#define D1_GPIO GPIO_NUM_20
#define D2_GPIO GPIO_NUM_21
#define D3_GPIO GPIO_NUM_26
#define D4_GPIO GPIO_NUM_48
#define D5_GPIO GPIO_NUM_47
#define D6_GPIO GPIO_NUM_33
#define D7_GPIO GPIO_NUM_34
#define VSYNC_GPIO GPIO_NUM_6
#define HREF_GPIO GPIO_NUM_5
#define PCLK_GPIO GPIO_NUM_7
#define XCLK_GPIO GPIO_NUM_4

#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_FREQ_HZ 100000
#define OV7670_ADDR 0x21

#define IMAGE_WIDTH  320
#define IMAGE_HEIGHT 240
#define IMAGE_SIZE   (IMAGE_WIDTH * IMAGE_HEIGHT)
#define MAX_WAIT_CYCLES 1000000 

#define WIFI_SSID "DukeVisitor"
#define WIFI_PASS ""
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_PSK
#define ESP_WIFI_SAE_MODE WPA3_SAE_PWE_HUNT_AND_PECK
#define EXAMPLE_H2E_IDENTIFIER ""    

#define SERVER_URL "http://172.28.116.119"

static uint8_t image_buffer[IMAGE_SIZE];
static const char *TAG = "WiFiCamera";

// Function to write to OV7670 registers
esp_err_t ov7670_write_register(uint8_t reg, uint8_t value) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (OV7670_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    i2c_master_write_byte(cmd, value, true);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

// Function to initialize I2C
void i2c_master_init() {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = SDA_GPIO,
        .scl_io_num = SCL_GPIO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    i2c_param_config(I2C_MASTER_NUM, &conf);
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
}

// Function to initialize OV7670
void ov7670_init() {
    // Reset the camera
    ov7670_write_register(0x12, 0x80); // Reset
    vTaskDelay(100 / portTICK_PERIOD_MS);

    // Configure basic settings
    ov7670_write_register(0x12, 0x14); // Set QVGA, RGB
    ov7670_write_register(0x11, 0x01); // Set clock prescaler
    ov7670_write_register(0x6B, 0x4A); // PLL settings
    ov7670_write_register(0x0C, 0x04); // Enable RGB
    ov7670_write_register(0x40, 0xD0); // RGB565 format
}

// Function to initialize GPIOs
void gpio_init() {
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = ((1ULL << D0_GPIO) | (1ULL << D1_GPIO) |
                         (1ULL << D2_GPIO) | (1ULL << D3_GPIO) |
                         (1ULL << D4_GPIO) | (1ULL << D5_GPIO) |
                         (1ULL << D6_GPIO) | (1ULL << D7_GPIO) |
                         (1ULL << VSYNC_GPIO) | (1ULL << HREF_GPIO) |
                         (1ULL << PCLK_GPIO)),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE
    };
    gpio_config(&io_conf);

    gpio_set_direction(XCLK_GPIO, GPIO_MODE_OUTPUT);
}

// Function to generate XCLK signal
void xclk_init() {
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_1_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 24000000, // 24 MHz
        .clk_cfg = LEDC_AUTO_CLK
    };

    esp_err_t timer_ret = ledc_timer_config(&ledc_timer);
    if (timer_ret != ESP_OK) {
        printf("Error initializing LEDC timer: %s\n", esp_err_to_name(timer_ret));
        return; // Exit if the timer configuration failed
    }

    ledc_channel_config_t ledc_channel = {
        .gpio_num = XCLK_GPIO,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 1, // 50% duty cycle
        .hpoint = 0
    };
    
    esp_err_t channel_ret = ledc_channel_config(&ledc_channel);
    if (channel_ret != ESP_OK) {
        printf("Error configuring LEDC channel: %s\n", esp_err_to_name(channel_ret));
        return; // Exit if the channel configuration failed
    }
}

// Function to capture image
void capture_image() {
    bool vsync_flag = false;
    int wait_cycles = 0;

    // Wait for VSYNC
    while (!vsync_flag) {
        if (gpio_get_level(VSYNC_GPIO) == 1) {
            vsync_flag = true;
        }
        if (++wait_cycles > MAX_WAIT_CYCLES) {
            printf("Error: VSYNC timeout\n");
            return;
        }
    }

    // Reset wait cycle counter
    wait_cycles = 0;

    // Read pixel data
    for (int i = 0; i < IMAGE_SIZE; i++) {
        // Wait for PCLK high
        while (gpio_get_level(PCLK_GPIO) == 0) {
            if (++wait_cycles > MAX_WAIT_CYCLES) {
                printf("Error: PCLK timeout (high)\n");
                return;
            }
        }

        // Read data
        image_buffer[i] = (gpio_get_level(D7_GPIO) << 7) |
                          (gpio_get_level(D6_GPIO) << 6) |
                          (gpio_get_level(D5_GPIO) << 5) |
                          (gpio_get_level(D4_GPIO) << 4) |
                          (gpio_get_level(D3_GPIO) << 3) |
                          (gpio_get_level(D2_GPIO) << 2) |
                          (gpio_get_level(D1_GPIO) << 1) |
                          gpio_get_level(D0_GPIO);

        // Wait for PCLK low
        while (gpio_get_level(PCLK_GPIO) == 1) {
            if (++wait_cycles > MAX_WAIT_CYCLES) {
                printf("Error: PCLK timeout (low)\n");
                return;
            }
        }
    }
}

void test_vsync() {
    printf("Testing VSYNC signal...\n");
    while (1) {
        int vsync_level = gpio_get_level(VSYNC_GPIO);
        printf("VSYNC: %d\n", vsync_level);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

static EventGroupHandle_t s_wifi_event_group;

//wifi setup stuff

static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        ESP_LOGI(TAG, "Failed to connect.");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
        ESP_LOGI(TAG, "Connected with IP address: " IPSTR, IP2STR(&event->ip_info.ip));
    }
}

void wifi_init_sta(void) {
    s_wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, NULL));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = "DukeVisitor",  // Open network SSID
            .password = "",         // No password for open network
            .threshold.authmode = WIFI_AUTH_OPEN  // Set auth mode to openg
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT, pdFALSE, pdFALSE, portMAX_DELAY);
    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "Successfully connected to SSID: %s", wifi_config.sta.ssid);
    } else {
        ESP_LOGI(TAG, "Failed to connect to SSID: %s", wifi_config.sta.ssid);
    }
}

// void wifi_init() {
//     ESP_ERROR_CHECK(nvs_flash_init());
//     ESP_ERROR_CHECK(esp_netif_init());
//     ESP_ERROR_CHECK(esp_event_loop_create_default());
//     esp_netif_create_default_wifi_sta();

//     wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
//     ESP_ERROR_CHECK(esp_wifi_init(&cfg));

//     wifi_config_t wifi_config = {
//         .sta = {
//             .ssid = WIFI_SSID,
//             .threshold.authmode = WIFI_AUTH_OPEN,
//             .sae_pwe_h2e = ESP_WIFI_SAE_MODE,
//             .sae_h2e_identifier = EXAMPLE_H2E_IDENTIFIER,
//         },
//     };

//     ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
//     ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
//     ESP_ERROR_CHECK(esp_wifi_start());

//     ESP_LOGI(TAG, "Connecting to Wi-Fi...");
//     ESP_ERROR_CHECK(esp_wifi_connect());
// }

// HTTP POST request to upload the image
void upload_picture() {
    uint8_t bmp_header[54] = {
        0x42, 0x4D, 0x36, 0x6C, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x36, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x40, 0x01,
        0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x01, 0x00, 0x18, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x6C, 0x0C, 0x00, 0xC4, 0x0E,
        0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00
    };

    size_t bmp_file_size = sizeof(bmp_header) + IMAGE_SIZE;
    uint8_t *bmp_data = malloc(bmp_file_size);
    if (!bmp_data) {
        ESP_LOGE(TAG, "Failed to allocate memory for BMP file");
        return;
    }

    memcpy(bmp_data, bmp_header, sizeof(bmp_header));
    memcpy(bmp_data + sizeof(bmp_header), image_buffer, IMAGE_SIZE);

    esp_http_client_config_t config = {
        .url = SERVER_URL "/image",
        .cert_pem = NULL,  // Disables certificate validation
        .skip_cert_common_name_check = true,  // Skip server hostname verification
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);

    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_header(client, "Content-Type", "image/bmp");
    esp_http_client_set_post_field(client, (const char *)bmp_data, bmp_file_size);

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP POST Status = %d", esp_http_client_get_status_code(client));
    } else {
        ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
    free(bmp_data);
}

esp_err_t image_handler(httpd_req_t *req) {
    httpd_resp_set_type(req, "image/bmp");

    // BMP Header for a 320x240 RGB565 image
    uint8_t bmp_header[54] = {
        0x42, 0x4D, 0x36, 0x6C, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x36, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x40, 0x01,
        0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x01, 0x00, 0x18, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x6C, 0x0C, 0x00, 0xC4, 0x0E,
        0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00
    };

    httpd_resp_send_chunk(req, (const char *)bmp_header, sizeof(bmp_header));

    httpd_resp_send_chunk(req, (const char *)image_buffer, IMAGE_SIZE);

    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}


void start_http_server() {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    httpd_handle_t server = NULL;
    ESP_ERROR_CHECK(httpd_start(&server, &config));

    httpd_uri_t uri_handler = {
        .uri = "/image",
        .method = HTTP_GET,
        .handler = image_handler,
        .user_ctx = NULL,
    };

    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &uri_handler));
    ESP_LOGI(TAG, "HTTP server started. Access the image at http://172.28.69.113/image");

}

// Main application
void app_main() {
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    wifi_init_sta();
    start_http_server();

    i2c_master_init();
    gpio_init();
    xclk_init();

    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 1);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);

    ov7670_init();

    vTaskDelay(1000 / portTICK_PERIOD_MS);


    printf("Starting image capture...\n");
    // while (1) {
    capture_image();
    printf("Image captured\n");

    upload_picture();
    // Add image processing or transmission here
    vTaskDelay(1000 / portTICK_PERIOD_MS); // Delay for demo purposes
    }