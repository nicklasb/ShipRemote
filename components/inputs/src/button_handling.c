
#include "../include/button_handling.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

#include <button.h>
#include <robusto_logging.h>

void init_button_handling(void) {

    button_event_t ev;
    QueueHandle_t button_events = button_init(PIN_BIT(CONFIG_BUTTON_LEFT_GPIO));
    while (true) {
        if (xQueueReceive(button_events, &ev, 1000/portTICK_PERIOD_MS)) {
            if ((ev.pin == CONFIG_BUTTON_LEFT_GPIO) && (ev.event == BUTTON_DOWN)) {
                ROB_LOGI("sdfsdf", "1");
            }
            if ((ev.pin == CONFIG_BUTTON_LEFT_GPIO) && (ev.event == BUTTON_HELD)) {
                ROB_LOGI("sdfsdf", "2");
            }
        }
    }

}