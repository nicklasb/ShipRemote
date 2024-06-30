
#include <robusto_logging.h>
#include <loop.h>
#include <ap.h>
#include <communication.h>
#include <robusto_init.h>
#include <screen.h>
#include <buttons.h>
#include "nav_screen.h"
#ifdef CONFIG_ROBUSTO_INPUT_ADC_MONITOR 
#ifdef USE_ESPIDF
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#endif
#endif

char *remote_log_prefix;

void app_main()
{
    init_robusto();
    remote_log_prefix = "ShipRemote";
#ifdef CONFIG_ROBUSTO_INPUT_ADC_MONITOR 
    gpio_pulldown_dis(GPIO_NUM_36);
    gpio_pullup_dis(GPIO_NUM_36);
    gpio_set_direction(GPIO_NUM_36, GPIO_MODE_INPUT);
    //gpio_set_level(GPIO_NUM_39, 1);
    gpio_set_pull_mode(GPIO_NUM_36, GPIO_FLOATING);
    
#else
    init_communication(remote_log_prefix);
    init_loop(remote_log_prefix);
    init_ap(remote_log_prefix);

    init_screen(remote_log_prefix);
    init_nav_screen(remote_log_prefix);
    start_nav_screen();
    start_communication();
    
    start_ap();
    r_delay(3000);

    init_buttons(remote_log_prefix);
//    robusto_print_peers();
    start_loop();
#endif
}
