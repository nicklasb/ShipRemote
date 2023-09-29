
#include <robusto_logging.h>
#include <loop.h>
#include <ap.h>
#include <communication.h>
#include <robusto_init.h>
#include <screen.h>
#include <buttons.h>
char * remote_log_prefix;

void app_main() {
    remote_log_prefix = "ShipRemote";
    init_communication(remote_log_prefix);
    init_robusto();
    init_ap(remote_log_prefix);
    init_loop(remote_log_prefix);
    
    start_communication();
    start_ap();

    init_screen(remote_log_prefix);
    init_buttons(remote_log_prefix);
    
    start_loop();

}
