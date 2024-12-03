#include <stdio.h> //Provides standard input/output functionality for C programming, such as printf() for debugging and logging.

//for FreeRTOS, the real-time operating system running on the ESP32.
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>

#include <esp_log.h> //logging functionality with levels and tags
#include <rom/ets_sys.h> //Provides low-level timing utilities, such as ets_delay_us(), which can pause execution for microsecond-level delays.
#include <inttypes.h> // Add this to use PRIi64

//Wi-Fi Management
#include "esp_wifi.h" //Contains functions to initialize Wi-Fi, connect to networks, and handle Wi-Fi events.
#include "esp_event.h" //Event handling system for Wi-Fi, TCP/IP stack, and other peripherals.
#include "esp_netif.h" //Simplifies network interface management for both Wi-Fi and Ethernet connections.
#include "nvs_flash.h" //Manages non-volatile storage to save data that persists across resets, such as Wi-Fi credentials.

#include <driver/gpio.h> // control and read the state of GPIO pins on the ESP32
#include "driver/i2c.h" // Manages communication with I2C devices, such as temperature sensors (DHT11) or OLED displays.

//Camera Driver
#include "esp_camera.h" //Provides functionality for initializing and interacting with camera modules like the OV5640 or OV2640. Used for image capture and configuration of the camera hardware.

// PWM (Pulse Width Modulation) Driver
// #include "driver/mcpwm.h" //Enables PWM functionality for controlling motors, LEDs, or other devices.
// #include "soc/mcpwm_periph.h" //Contains low-level details about MCPWM (Motor Control PWM) hardware peripherals.
#include "driver/ledc.h"

#include <esp_http_client.h>  // for uploading the image

#include <esp_http_server.h> //for reaading the open door request

#define HTTPD_MAX_REQ_HDR_LEN 4096

/* Max supported HTTP request URI length */
#define HTTPD_MAX_URI_LEN 1024

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

#define BUTTON_PIN 38 // GPIO pin for the button
#define SERVO_PIN GPIO_NUM_45  // GPIO pin for the servo motor

#define XCLK_GPIO_NUM    4  // Example GPIO for external clock (XC on module)
#define PCLK_GPIO_NUM    7 // Pixel clock pin (PC on module)
#define VSYNC_GPIO_NUM   6 // Vertical sync pin (VS on module)
#define HREF_GPIO_NUM    5 // Horizontal reference pin (HS on module)
#define SIOD_GPIO_NUM    35 // I2C SDA pin
#define SIOC_GPIO_NUM    36 // I2C SCL pin
#define PWDN_GPIO_NUM    10 // Power down pin, set to -1 if not used
#define RESET_GPIO_NUM   11 // Reset pin, set to -1 if not used
#define D0_GPIO_NUM      19 //D2 on module 
#define D1_GPIO_NUM      20 //D3 on module 
#define D2_GPIO_NUM      21 //D4 on module 
#define D3_GPIO_NUM      26 //D5 on module 
#define D4_GPIO_NUM      48 //D6 on module 
#define D5_GPIO_NUM      47 //D7 on module 
#define D6_GPIO_NUM      33 //D8 on module 
#define D7_GPIO_NUM      34 //D9 on module 

// Servo parameters for continuous rotation
#define SERVO_STOP_PULSEWIDTH_US 1500  // Pulse width for no movement (neutral position)
#define SERVO_FORWARD_PULSEWIDTH_US 2500 // Pulse width for forward movement
#define SERVO_REVERSE_PULSEWIDTH_US 500  // Pulse width for reverse movement

#define SERVER_URL "http://b-ring.vercel.app"  // Replace with actual server IP

// Global boolean flag set by ISR
volatile bool button_pressed = false;

// tags for debugging
static const char* TAG_WIFI = "wifi_station";
static const char* TAG_BUTTON = "button";
static const char* TAG_HTTP = "http_client";
static const char* TAG_CAMERA = "camera";
static const char* TAG_MOTOR = "motor";
static const char* TAG_SERVER = "http_server";

static EventGroupHandle_t s_wifi_event_group;

//wifi setup stuff

static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        ESP_LOGI(TAG_WIFI, "Failed to connect.");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
        ESP_LOGI(TAG_WIFI, "Connected with IP address: " IPSTR, IP2STR(&event->ip_info.ip));
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
        ESP_LOGI(TAG_WIFI, "Successfully connected to SSID: %s", wifi_config.sta.ssid);
    } else {
        ESP_LOGI(TAG_WIFI, "Failed to connect to SSID: %s", wifi_config.sta.ssid);
    }
}

//button set up stuff

// Button ISR to set the global boolean flag
static void IRAM_ATTR button_isr_handler(void* arg) {
    button_pressed = true;  // Set the flag to true when the button is pressed
}

void button_init() {
    // Button configuration
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BUTTON_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .intr_type = GPIO_INTR_NEGEDGE  // Trigger interrupt on button press
    };
    gpio_config(&io_conf);

    // Install ISR service and hook the button handler
    gpio_install_isr_service(0);
    gpio_isr_handler_add(BUTTON_PIN, button_isr_handler, NULL);
}

void take_picture() {
    ESP_LOGI(TAG_CAMERA, "Capturing picture...");
    camera_fb_t *pic = esp_camera_fb_get(); // Capture a frame
    if (!pic) {
        ESP_LOGE(TAG_CAMERA, "Failed to capture picture");
        return;
    }
    
    ESP_LOGI(TAG_CAMERA, "Picture taken! Size: %zu bytes", pic->len);
    
    // Process the image here (e.g., send it to a server or save to SD card)
    // send_picture_to_server(pic->buf, pic->len);

    esp_camera_fb_return(pic); // Return the frame buffer
}

// Function to set the servo pulse width
void set_servo_pulsewidth(uint32_t pulse_width) {
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, pulse_width);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}

void open_door() {
     for(int i=0; i<5; i++) {
        // Spin forward for 3 full rotations (approx. 3 seconds per rotation)
        set_servo_pulsewidth(SERVO_FORWARD_PULSEWIDTH_US);
        vTaskDelay(pdMS_TO_TICKS(5000)); // 9 seconds for 3 rotations
        set_servo_pulsewidth(SERVO_STOP_PULSEWIDTH_US); // Stop
        vTaskDelay(pdMS_TO_TICKS(500)); // Pause for 1 second

        // Spin backward for 3 full rotations
        set_servo_pulsewidth(SERVO_REVERSE_PULSEWIDTH_US);
        vTaskDelay(pdMS_TO_TICKS(5000)); // 9 seconds for 3 rotations
        set_servo_pulsewidth(SERVO_STOP_PULSEWIDTH_US); // Stop
        vTaskDelay(pdMS_TO_TICKS(500)); // Pause for 1 second
    }
}

void button_task(void *pvParameter) {
    while (1) {
        if (gpio_get_level(BUTTON_PIN) == 0) {
            ESP_LOGI(TAG_BUTTON, "Button pressed, taking picture");
            take_picture();
            ESP_LOGI(TAG_MOTOR, "Button pressed, moving motor");
            open_door();
            vTaskDelay(1000 / portTICK_PERIOD_MS);  // Debounce delay
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

// Initialize Camera
void camera_init() {
    camera_config_t config = {
        .ledc_channel = LEDC_CHANNEL_0,
        .ledc_timer = LEDC_TIMER_0,
        .pin_d0 = D0_GPIO_NUM,
        .pin_d1 = D1_GPIO_NUM,
        .pin_d2 = D2_GPIO_NUM,
        .pin_d3 = D3_GPIO_NUM,
        .pin_d4 = D4_GPIO_NUM,
        .pin_d5 = D5_GPIO_NUM,
        .pin_d6 = D6_GPIO_NUM,
        .pin_d7 = D7_GPIO_NUM,
        .pin_xclk = XCLK_GPIO_NUM,
        .pin_pclk = PCLK_GPIO_NUM,
        .pin_vsync = VSYNC_GPIO_NUM,
        .pin_href = HREF_GPIO_NUM, 
        .pin_sccb_sda = SIOD_GPIO_NUM,
        .pin_sccb_scl = SIOC_GPIO_NUM,
        .pin_pwdn = PWDN_GPIO_NUM,
        .pin_reset = RESET_GPIO_NUM,
        .xclk_freq_hz = 20000000, // 20 MHz clock frequency
        .pixel_format = PIXFORMAT_JPEG, // Use JPEG format for images
        .frame_size = FRAMESIZE_QVGA,  // Resolution (e.g., QVGA, VGA)
        .jpeg_quality = 12,           // JPEG quality (1-63, lower is better quality)
        .fb_count = 1,                // Framebuffers count
        .fb_location = CAMERA_FB_IN_DRAM, 
        .grab_mode = CAMERA_GRAB_LATEST,
    };

    gpio_set_pull_mode(SIOD_GPIO_NUM, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(SIOC_GPIO_NUM, GPIO_PULLUP_ONLY);

    ESP_LOGI(TAG_CAMERA, "Available PSRAM: %d bytes", heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
    ESP_LOGI(TAG_CAMERA, "Available Default SRAM: %d bytes", heap_caps_get_free_size(MALLOC_CAP_INTERNAL));

    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_CAMERA, "Camera init failed with error 0x%x", err);
        return;
    } else {
        ESP_LOGI(TAG_CAMERA, "Camera successfully initialized");
    }
}

// Upload picture to server
void upload_picture(camera_fb_t *pic) {
    esp_http_client_config_t config = {
        .url = SERVER_URL "/guest-verification",
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);

    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_header(client, "Content-Type", "image/jpeg");
    esp_http_client_set_post_field(client, (const char *)pic->buf, pic->len);

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG_HTTP, "HTTP POST Status = %d, content_length = %" PRIi64,
         esp_http_client_get_status_code(client),
         esp_http_client_get_content_length(client));
    } else {
        ESP_LOGE(TAG_HTTP, "HTTP POST request failed: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
}

// // Function to set the servo pulse width
// void set_servo_pulsewidth(uint32_t pulse_width) {
//     ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, pulse_width);
//     ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
// }

// void open_door() {
//      for(int i=0; i<5; i++) {
//         // Spin forward for 3 full rotations (approx. 3 seconds per rotation)
//         set_servo_pulsewidth(SERVO_FORWARD_PULSEWIDTH_US);
//         vTaskDelay(pdMS_TO_TICKS(5000)); // 9 seconds for 3 rotations
//         set_servo_pulsewidth(SERVO_STOP_PULSEWIDTH_US); // Stop
//         vTaskDelay(pdMS_TO_TICKS(500)); // Pause for 1 second

//         // Spin backward for 3 full rotations
//         set_servo_pulsewidth(SERVO_REVERSE_PULSEWIDTH_US);
//         vTaskDelay(pdMS_TO_TICKS(5000)); // 9 seconds for 3 rotations
//         set_servo_pulsewidth(SERVO_STOP_PULSEWIDTH_US); // Stop
//         vTaskDelay(pdMS_TO_TICKS(500)); // Pause for 1 second
//     }
// }

esp_err_t door_open_handler(httpd_req_t *req) {
    // Logic to open the door
    ESP_LOGI("HTTP_SERVER", "Door open request received");

    // Add your logic to open the door (e.g., call `open_door()` function)
    open_door();
    
    // Respond to the HTTP client
    const char *resp = "Door is now open!";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

void start_http_server() {
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    // Start the HTTP server
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set up the URI handler
        httpd_uri_t door_open_uri = {
            .uri       = "/open-door",
            .method    = HTTP_GET,
            .handler   = door_open_handler, 
            .user_ctx  = NULL
        };
        httpd_register_uri_handler(server, &door_open_uri);
    }
}

void app_main() {
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Initialize button
    button_init();

    // Initialize Camera
    camera_init();

    // Capture and upload picture periodically (example task) 
    // for testing purposes 

    // while (1) {
    //     camera_fb_t *pic = esp_camera_fb_get();
    //     ESP_LOGI(TAG_CAMERA, "*** took picture? ***");
    //     if (pic) {
    //         ESP_LOGI(TAG_CAMERA, "maybe took a picture yay");
    //         // upload_picture(pic); // Upload captured image
    //         esp_camera_fb_return(pic);
    //     }
    //     vTaskDelay(pdMS_TO_TICKS(10000)); // Delay 10 seconds between captures
    // }

    // Configure the LEDC peripheral
    ledc_timer_config_t ledc_timer = {
        .duty_resolution = LEDC_TIMER_14_BIT, // Resolution of PWM duty
        .freq_hz = 50,                        // Frequency in Hertz (50Hz for servo)
        .speed_mode = LEDC_LOW_SPEED_MODE,    // Timer mode
        .timer_num = LEDC_TIMER_0             // Timer index
    };
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel = {
        .channel    = LEDC_CHANNEL_0,
        .duty       = 0,
        .gpio_num   = SERVO_PIN,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_sel  = LEDC_TIMER_0
    };
    ledc_channel_config(&ledc_channel);

    // for(int i=0; i<5; i++) {
    //     // Spin forward for 3 full rotations (approx. 3 seconds per rotation)
    //     set_servo_pulsewidth(SERVO_FORWARD_PULSEWIDTH_US);
    //     vTaskDelay(pdMS_TO_TICKS(5000)); // 9 seconds for 3 rotations
    //     set_servo_pulsewidth(SERVO_STOP_PULSEWIDTH_US); // Stop
    //     vTaskDelay(pdMS_TO_TICKS(500)); // Pause for 1 second

    //     // Spin backward for 3 full rotations
    //     set_servo_pulsewidth(SERVO_REVERSE_PULSEWIDTH_US);
    //     vTaskDelay(pdMS_TO_TICKS(5000)); // 9 seconds for 3 rotations
    //     set_servo_pulsewidth(SERVO_STOP_PULSEWIDTH_US); // Stop
    //     vTaskDelay(pdMS_TO_TICKS(500)); // Pause for 1 second
    // }

    // Initialize WiFi
    wifi_init_sta();

    // Start HTTP server
    start_http_server();

    // Create button task to run concurrently with the main function
    xTaskCreate(&button_task, "button_task", 4096, NULL, 5, NULL);
}


