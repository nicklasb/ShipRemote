#include "loop.h"

#include <robusto_logging.h>
#include "buttons.h"
#include "actions.h"
#include "ap.h"


static char * loop_log_prefix;

void perform_actions(e_action_t action){
    if (action == ACTION_NONE) {
        return;
    } else
    if (action < ACTION_UPDATE_HEADING) {
        perform_ap_actions(action);
    } else {
        ROB_LOGE(loop_log_prefix, "Error in perform_actions(), unhandled action.");
    }

}

void start_loop() {
    
    while (1) {
        /* Poll inputs */
        // Poll buttons
        perform_actions(poll_button_queue());
        
        // Poll communications


        /* Do actions */
        
        // Send data

        // Update UI





    }


};

void init_loop(char * _log_prefix) {
    loop_log_prefix = _log_prefix;
};