
#include "button_handling.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

#include <button.h>
#include <robusto_logging.h>
#include <communication.h>

char * button_log_prefix;

void init_button_handling(char * _log_prefix) {

    button_log_prefix = _log_prefix;

    button_event_t ev;
    ROB_LOGI(button_log_prefix, "Init button");
    QueueHandle_t button_events = pulled_button_init(PIN_BIT(CONFIG_BUTTON_LEFT_GPIO), GPIO_PULLDOWN_ONLY);
    ROB_LOGI(button_log_prefix, "Starting button loop.");
    while (true) {
        if (xQueueReceive(button_events, &ev, 1000/portTICK_PERIOD_MS)) {
            if ((ev.pin == CONFIG_BUTTON_LEFT_GPIO) && (ev.event == BUTTON_DOWN)) {
                //ROB_LOGI(button_log_prefix, "1");
                send_course_correction(-1);
            } else
            if ((ev.pin == CONFIG_BUTTON_RIGHT_GPIO) && (ev.event == BUTTON_DOWN)) {
                ROB_LOGI(button_log_prefix, "2");
                //send_course_correction(1);
            } else
            if ((ev.pin == CONFIG_BUTTON_LEFT_GPIO) && (ev.event == BUTTON_HELD)) {
                ROB_LOGI(button_log_prefix, "3");
                //send_course_correction(-2);
            } else
            if ((ev.pin == CONFIG_BUTTON_RIGHT_GPIO) && (ev.event == BUTTON_HELD)) {
                ROB_LOGI(button_log_prefix, "4");
                //send_course_correction(2);
            }
        }
    }

}