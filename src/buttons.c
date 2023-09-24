
#include "buttons.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

#include <button.h>
#include <robusto_logging.h>
#include <communication.h>
#include <robusto_system.h>

char *button_log_prefix;




void poll_button_queue() {
    
}

void init_buttons(char *_log_prefix)
{

    button_log_prefix = _log_prefix;

    button_event_t ev;
    ROB_LOGI(button_log_prefix, "Init button");

    robusto_gpio_set_direction(CONFIG_BUTTON_LEFT_GPIO, false);
    if (robusto_gpio_get_level(CONFIG_BUTTON_LEFT_GPIO))
    {
        ROB_LOGE(button_log_prefix, "BUTTON IS HIGH");
    }
    QueueHandle_t button_events = button_init(PIN_BIT(CONFIG_BUTTON_LEFT_GPIO));
    ROB_LOGI(button_log_prefix, "Starting event loop.");
    while (true)
    {
        if (xQueueReceive(button_events, &ev, 1000 / portTICK_PERIOD_MS))
        {
            if (robusto_gpio_get_level(CONFIG_BUTTON_LEFT_GPIO))
            {
                ROB_LOGE(button_log_prefix, "BUTTON IS HIGH");
            }
            else
            {
                ROB_LOGE(button_log_prefix, "BUTTON IS LOW");
            }

            if ((ev.pin == CONFIG_BUTTON_LEFT_GPIO) && (ev.event == BUTTON_DOWN))
            {
                // ROB_LOGI(button_log_prefix, "1");
                send_course_correction(-1);
            }
            else if ((ev.pin == CONFIG_BUTTON_RIGHT_GPIO) && (ev.event == BUTTON_DOWN))
            {
                ROB_LOGI(button_log_prefix, "2");
                send_course_correction(1);
            }
            else if ((ev.pin == CONFIG_BUTTON_LEFT_GPIO) && (ev.event == BUTTON_HELD))
            {
                ROB_LOGI(button_log_prefix, "3");

                // send_course_correction(-2);
            }
            else if ((ev.pin == CONFIG_BUTTON_RIGHT_GPIO) && (ev.event == BUTTON_HELD))
            {
                ROB_LOGI(button_log_prefix, "4");
                // send_course_correction(2);
            }
            
        }
    }
}