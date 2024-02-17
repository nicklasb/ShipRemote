
#include "buttons.h"

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <robusto_logging.h>
#include <robusto_input.h>
#include <robusto_concurrency.h>
#include <robusto_system.h>

resistance_mapping_t resistances[6] = {
    {.resistance = 176684, .adc_voltage = 2678, .adc_stdev = 4}, //This is the total resistance, or base source_voltage value
    {.resistance = 92228, .adc_voltage = 2218, .adc_stdev = 5},
    {.resistance = 42452, .adc_voltage = 2525, .adc_stdev = 3},
    {.resistance = 18530, .adc_voltage = 2618, .adc_stdev = 4},
    {.resistance = 7745, .adc_voltage = 2653, .adc_stdev = 2},
    {.resistance = 3437, .adc_voltage = 2668, .adc_stdev = 3}, 
};

resistor_monitor_t *monitor = NULL;
uint32_t change_count = 0;

#define BUTTON_OK 0x01
#define BUTTON_UP 0x02
#define BUTTON_LEFT 0x04
#define BUTTON_DOWN 0x08
#define BUTTON_RIGHT 0x10

static char *button_log_prefix;

static e_action_t curr_action = ACTION_NONE;

e_action_t poll_button_queue() {
    e_action_t ret_action = curr_action;
    curr_action = ACTION_NONE;
    return ret_action;
}

void callback_buttons_press(uint32_t buttons)
{

    char bit_string[33]; // 32 bits + null terminator
    for (int i = 0; i < 33; i++)
    {
        bit_string[i] = (buttons & (1 << i)) ? '0' + i : '_';
    }
    bit_string[32] = '\0'; // Null-terminate the string

    ROB_LOGI("LADDER BUTTONS", "Change: %3.lu - Buttons: %s - Value: %lu.", change_count++, bit_string, buttons);

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
    monitor->R1 = 41200;
    monitor->R2_check_resistor = 2200;
    monitor->source_voltage = 3300; //TODO:  3206;
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