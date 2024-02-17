
#include <robusto_logging.h>
#include <loop.h>
#include <ap.h>
#include <communication.h>
#include <robusto_init.h>
#include <screen.h>
#include <buttons.h>
#include "nav_screen.h"

#ifdef USE_ESPIDF
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#endif
char *remote_log_prefix;

void app_main()
{

    gpio_pulldown_dis(GPIO_NUM_38);
    gpio_pullup_dis(GPIO_NUM_38);
    gpio_set_direction(GPIO_NUM_38, GPIO_MODE_INPUT);
    //gpio_set_level(GPIO_NUM_39, 1);
    gpio_set_pull_mode(GPIO_NUM_38, GPIO_FLOATING);
    init_robusto();
    remote_log_prefix = "ShipRemote";
    
/*
resistance_mapping_t bm[6] = {
    {.resistance = 179514, .adc_voltage = 2684, .adc_stdev = 9}, //This is the total resistance, or base source_voltage value
    {.resistance = 95636, .adc_voltage = 2213, .adc_stdev = 8},
    {.resistance = 44372, .adc_voltage = 2529, .adc_stdev = 7},
    {.resistance = 20184, .adc_voltage = 2622, .adc_stdev = 3},
    {.resistance = 9596, .adc_voltage = 2656, .adc_stdev = 3},
    {.resistance = 4562, .adc_voltage = 2671, .adc_stdev = 7},
}

*/


   
    init_communication(remote_log_prefix);
    init_loop(remote_log_prefix);
    init_ap(remote_log_prefix);

    init_screen(remote_log_prefix);
    init_nav_screen(remote_log_prefix);
    start_nav_screen();
    start_communication();
    
    start_ap();

    init_buttons(remote_log_prefix);
//    robusto_print_peers();
    start_loop();
    
}
