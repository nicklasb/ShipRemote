
#include "buttons.h"

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <robusto_logging.h>
#include <robusto_input.h>
#include <robusto_concurrency.h>
#include <robusto_system.h>

resistance_mapping_t resistances[6] = {
    {.resistance = 180956, .adc_voltage = 2688, .adc_stdev = 6}, //This is the total resistance, or base source_voltage value
    {.resistance = 96268, .adc_voltage = 2220, .adc_stdev = 7}, // Right 
    {.resistance = 44894, .adc_voltage = 2533, .adc_stdev = 8}, // Up
    {.resistance = 20435, .adc_voltage = 2626, .adc_stdev = 7}, // Left
    {.resistance = 9719, .adc_voltage = 2660, .adc_stdev = 4}, // Down 150
    {.resistance = 4968, .adc_voltage = 2674, .adc_stdev = 5}, // OK
};

resistor_monitor_t *monitor = NULL;
uint32_t change_count = 0;

#define BUTTON_OK 0x16
#define BUTTON_UP 0x02
#define BUTTON_LEFT 0x04
#define BUTTON_DOWN 0x08
#define BUTTON_RIGHT 0x1

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

    ROB_LOGI("LADDER BUTTONS", "Buttons: %s - Value: %lu, voltage: %.1f V. Change: %lu.", 
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
    monitor->source_voltage = 3270; // Nom 3300
    monitor->GPIO = 38; // Usually OK.
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
    init_resistance_mappings();
    ROB_LOGI(button_log_prefix, "Buttons initiated.");
}