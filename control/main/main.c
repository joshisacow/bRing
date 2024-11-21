// #include <stdio.h>
// #include <freertos/FreeRTOS.h>
// #include <freertos/task.h>
// #include <driver/gpio.h>
// #include <esp_timer.h>
// #include "driver/ledc.h"
// #include "esp_err.h"


// #define RED_LED     45 // GPIO6
// #define BLUE_LED    42 // GPIO7

// #define LEDC_CHANNEL       LEDC_CHANNEL_0
// #define LEDC_CHANNEL_1     LEDC_CHANNEL_1
// #define LEDC_TIMER         LEDC_TIMER_0
// #define LEDC_MODE          LEDC_LOW_SPEED_MODE
// #define LEDC_DUTY_RES      LEDC_TIMER_13_BIT
// #define LEDC_FREQUENCY     (5000) // 5 kHz
// #define MAX_DUTY           100


// void app_main(void) {

//     // gpio_config_t red_conf = {
//     //     .pin_bit_mask = (1ULL << RED_LED),
//     //     .mode = GPIO_MODE_OUTPUT,
//     // };
//     // gpio_config_t blue_conf = {
//     //     .pin_bit_mask = (1ULL << BLUE_LED),
//     //     .mode = GPIO_MODE_OUTPUT,
//     // };
//     // gpio_config(&red_conf);
//     // gpio_config(&blue_conf);


//     ledc_timer_config_t ledc_timer = {
//         .duty_resolution = LEDC_TIMER_13_BIT, // resolution of PWM duty
//         .freq_hz = LEDC_FREQUENCY,            // frequency of PWM signal
//         .speed_mode = LEDC_MODE,           // timer mode
//         .timer_num = LEDC_TIMER,            // timer index
//         .clk_cfg = LEDC_AUTO_CLK,              // Auto select the source clock
//     };

//     ledc_timer_config(&ledc_timer);

//     ledc_channel_config_t ledc_channel = {
//         .channel    = LEDC_CHANNEL,
//         .duty       = 0,
//         .gpio_num   = RED_LED,
//         .speed_mode = LEDC_MODE,
//         .hpoint     = 0,
//         .timer_sel  = LEDC_TIMER
//     };

//     ledc_channel_config(&ledc_channel);

//     ledc_channel_config_t ledc_channel_b = {
//         .channel    = LEDC_CHANNEL_1,
//         .duty       = 0,
//         .gpio_num   = BLUE_LED,
//         .speed_mode = LEDC_MODE,
//         .hpoint     = 0,
//         .timer_sel  = LEDC_TIMER
//     };

//     ledc_channel_config(&ledc_channel_b);





//     while (1) {
//         for (int duty = 0; duty <= MAX_DUTY; duty=duty+2) {
//             ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty);
//             ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
//             ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_1, MAX_DUTY-duty);
//             ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_1);
//             vTaskDelay(10 / portTICK_PERIOD_MS);
//         }
        
//         for (int duty = MAX_DUTY; duty >= 0; duty=duty-2) {
//             ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty);
//             ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
//             ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_1, MAX_DUTY-duty);
//             ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_1);
//             vTaskDelay(10 / portTICK_PERIOD_MS);
//         }
//     }




// }


#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"

// Define the GPIO pin connected to the servo signal wire
#define SERVO_PIN GPIO_NUM_45

// Servo parameters for continuous rotation
#define SERVO_STOP_PULSEWIDTH_US 1500  // Pulse width for no movement (neutral position)
#define SERVO_FORWARD_PULSEWIDTH_US 2500 // Pulse width for forward movement
#define SERVO_REVERSE_PULSEWIDTH_US 500  // Pulse width for reverse movement

// Function to set the servo pulse width
void set_servo_pulsewidth(uint32_t pulse_width) {
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, pulse_width);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}

void app_main(void) {
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