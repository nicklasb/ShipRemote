
#include "button_handling.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

#include <button.h>
#include <robusto_logging.h>

char * button_log_prefix;

void init_button_handling(char * _log_prefix) {

    button_log_prefix = _log_prefix;

    button_event_t ev;
    QueueHandle_t button_events = button_init(PIN_BIT(CONFIG_BUTTON_LEFT_GPIO) | PIN_BIT(CONFIG_BUTTON_RIGHT_GPIO));
    while (true) {
        if (xQueueReceive(button_events, &ev, 1000/portTICK_PERIOD_MS)) {
            if ((ev.pin == CONFIG_BUTTON_LEFT_GPIO) && (ev.event == BUTTON_DOWN)) {
                ROB_LOGI(button_log_prefix, "Left click");
            } else
            if ((ev.pin == CONFIG_BUTTON_RIGHT_GPIO) && (ev.event == BUTTON_DOWN)) {
                ROB_LOGI(button_log_prefix,"Right click");
            } else
            if ((ev.pin == CONFIG_BUTTON_LEFT_GPIO) && (ev.event == BUTTON_HELD)) {
                ROB_LOGI(button_log_prefix,"Left long click");
            } else
            if ((ev.pin == CONFIG_BUTTON_RIGHT_GPIO) && (ev.event == BUTTON_HELD)) {
                ROB_LOGI(button_log_prefix,"Right long click");
            }
        }
    }

}