
#include "buttons.h"

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <robusto_logging.h>
#include <robusto_input.h>
#include <robusto_concurrency.h>
#include <robusto_system.h>

#ifdef USE_ESPIDF
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#endif

#define BUTTON_RIGHT 0x01
#define BUTTON_UP 0x02
#define BUTTON_LEFT 0x04
#define BUTTON_DOWN 0x08
#define BUTTON_OK 0x10

resistance_mapping_t resistances[6] = {
    {.resistance = 205638.8, .adc_voltage = 2717.0, .adc_stdev = 4.0}, //This is the total resistance, or base source_voltage value
    {.resistance = 117509.1, .adc_voltage = 2220.2, .adc_stdev = 3.7},
    {.resistance = 55542.6, .adc_voltage = 2559.2, .adc_stdev = 2.0},
    {.resistance = 26789.1, .adc_voltage = 2651.8, .adc_stdev = 3.0},
    {.resistance = 11020.8, .adc_voltage = 2692.6, .adc_stdev = 2.5},
    {.resistance = 5443.3, .adc_voltage = 2704.7, .adc_stdev = 1.0},
};





resistor_monitor_t *monitor = NULL;
uint32_t change_count = 0;


static char *button_log_prefix;

static e_action_t curr_action = ACTION_NONE;

e_action_t poll_button_queue() {
    e_action_t ret_action = curr_action;
    curr_action = ACTION_NONE;
    return ret_action;
}

void callback_buttons_press(uint32_t buttons, float voltage)
{

    char bit_string[33]; // 32 bits + null terminator
    for (int i = 0; i < 33; i++)
    {
        bit_string[i] = (buttons & (1 << i)) ? '0' + i : '_';
    }
    bit_string[32] = '\0'; // Null-terminate the string

    ROB_LOGW(button_log_prefix, "Buttons: %s - Value: %lu, voltage: %.1f V. Change: %lu.", 
        bit_string, buttons, voltage, change_count++);

    if (buttons == BUTTON_LEFT)
    {
        ROB_LOGI(button_log_prefix, "ACTION_AP_LEFT_1");
        curr_action = ACTION_AP_LEFT_1;
    }
    else if (buttons == BUTTON_RIGHT)
    {
        ROB_LOGI(button_log_prefix, "ACTION_AP_RIGHT_1");
        curr_action = ACTION_AP_RIGHT_1;
    } 
    else if (buttons == BUTTON_UP)
    {
        ROB_LOGI(button_log_prefix, "ACTION_AP_RIGHT_10");
        curr_action = ACTION_AP_RIGHT_10;
    } 
    else if (buttons == BUTTON_DOWN)
    {
        ROB_LOGI(button_log_prefix, "ACTION_AP_LEFT_10");
        curr_action = ACTION_AP_LEFT_10;
    } 
    else if (buttons == BUTTON_OK)
    {
        ROB_LOGW(button_log_prefix, "ACTION_OK (unassigned)");
        curr_action = ACTION_OK;
    } 
};

void init_resistance_mappings()
{
    if (monitor)
    {
        return;
    }
    monitor = robusto_malloc(sizeof(resistor_monitor_t));
    monitor->mappings = &resistances;
    monitor->mapping_count = 6;
    monitor->callback = &callback_buttons_press;
    monitor->R1 = 41100; // Nom 41200
    monitor->R2_check_resistor = 2160; // Nom 2200
    monitor->source_voltage = 3260; // Nom 3300
    monitor->GPIO = 36; // Usually OK.
    monitor->name = "Ladder monitor";
    monitor->ladder_decode = true;
    monitor->ladder_exclude_count = 0;

    if (robusto_input_add_resistor_monitor(monitor) != ROB_OK)
    {
        ROB_LOGE(button_log_prefix, "Failed to initialize buttons!");
    } else {
        ROB_LOGI(button_log_prefix, "Buttons initialized!");
    }
}

void init_buttons(char *_log_prefix)
{
    /* TODO: Implementen a robusto debounce variant that:
     * * posts Robusto UI events to the UI event queue directly.
     * * is cross platform
     * * is in Robusto UI
     */
    button_log_prefix = _log_prefix;
    ROB_LOGI(button_log_prefix, "Init button");
    gpio_pulldown_dis(GPIO_NUM_36);
    gpio_pullup_dis(GPIO_NUM_36);
    gpio_set_direction(GPIO_NUM_36, GPIO_MODE_INPUT);
    //gpio_set_level(GPIO_NUM_39, 1);
    gpio_set_pull_mode(GPIO_NUM_36, GPIO_FLOATING);

    init_resistance_mappings();
    ROB_LOGI(button_log_prefix, "Buttons initiated.");
}