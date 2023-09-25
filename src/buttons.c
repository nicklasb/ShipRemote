
#include "buttons.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

#include <button.h>

#include <robusto_logging.h>
#include <communication.h>
#include <robusto_system.h>

static char *button_log_prefix;
static button_event_t ev;
static QueueHandle_t button_events;


e_action_t poll_button_queue() {

    if (xQueueReceive(button_events, &ev, 1000 / portTICK_PERIOD_MS))
    {


        if ((ev.pin == CONFIG_BUTTON_LEFT_GPIO) && (ev.event == BUTTON_DOWN))
        {
            ROB_LOGI(button_log_prefix, "ACTION_AP_LEFT_1");
            return ACTION_AP_LEFT_1;
        }
        else if ((ev.pin == CONFIG_BUTTON_RIGHT_GPIO) && (ev.event == BUTTON_DOWN))
        {
            ROB_LOGI(button_log_prefix, "ACTION_AP_RIGHT_1");
            return ACTION_AP_RIGHT_1;
        }
        else if ((ev.pin == CONFIG_BUTTON_LEFT_GPIO) && (ev.event == BUTTON_HELD))
        {
            ROB_LOGI(button_log_prefix, "ACTION_AP_LEFT_10");
            return ACTION_AP_LEFT_10;
        }
        else if ((ev.pin == CONFIG_BUTTON_RIGHT_GPIO) && (ev.event == BUTTON_HELD))
        {
            ROB_LOGI(button_log_prefix, "ACTION_AP_RIGHT_10");
            return ACTION_AP_RIGHT_10;
        }
        
    } 
    return ACTION_NONE;
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
    button_events = pulled_button_init(PIN_BIT(CONFIG_BUTTON_LEFT_GPIO) | PIN_BIT(CONFIG_BUTTON_RIGHT_GPIO), GPIO_PULLUP_ONLY);
    ROB_LOGI(button_log_prefix, "Buttons initiated.");

}