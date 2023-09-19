
#include <robusto_logging.h>
#include <button_handling.h>

char * remote_log_prefix;

void app_main() {
    remote_log_prefix = "ShipRemote";
    init_button_handling(remote_log_prefix);
}
