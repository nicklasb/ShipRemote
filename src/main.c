
#include <robusto_logging.h>
#include <loop.h>
#include <ap.h>
#include <communication.h>
#include <robusto_init.h>
#include <screen.h>
#include <buttons.h>
#include "nav_screen.h"
char *remote_log_prefix;

void app_main()
{
    
    init_robusto();
    remote_log_prefix = "ShipRemote";
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
