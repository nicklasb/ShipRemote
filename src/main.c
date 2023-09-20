
#include <robusto_logging.h>
#include <button_handling.h>
#include <communication.h>
#include <robusto_init.h>
char * remote_log_prefix;

void app_main() {
    remote_log_prefix = "ShipRemote";
    init_communication(remote_log_prefix);
    init_robusto();
    start_communication();
    init_button_handling(remote_log_prefix);
}
