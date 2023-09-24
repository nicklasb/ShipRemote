
#include <robusto_logging.h>
#include <loop.h>
#include <communication.h>
#include <robusto_init.h>
#include <screen.h>
#include <buttons.h>
char * remote_log_prefix;

void app_main() {
    remote_log_prefix = "ShipRemote";
    init_communication(remote_log_prefix);
    init_robusto();
    start_communication();
    init_screen(remote_log_prefix);
    init_buttons(remote_log_prefix);
}
