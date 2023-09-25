#include "loop.h"

#include <robusto_logging.h>
#include "buttons.h"
#include "actions.h"
#include "ap.h"
#include <robusto_qos.h>
#include <robusto_screen_minimal.h>


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

void on_state_change(robusto_peer_t * peer, robusto_media_t *info, e_media_type media_type, e_media_state media_state, e_media_problem problem) {
    if (media_state == media_state_working) {
        #ifdef CONFIG_ROBUSTO_UI_MINIMAL
            char err_row[4];
            sprintf(&err_row, "<->");
            robusto_screen_minimal_write(err_row, 0, 0);  

        #endif
    } else
    if (media_state == media_state_problem) {
        #ifdef CONFIG_ROBUSTO_UI_MINIMAL
            char err_row[4];
            sprintf(&err_row, "<!>");
            robusto_screen_minimal_write(err_row, 0, 0);  

        #endif
    } else
    if (media_state == media_state_recovering) {
        #ifdef CONFIG_ROBUSTO_UI_MINIMAL
            char err_row[4];
            sprintf(&err_row, "<R>");
            robusto_screen_minimal_write(err_row, 0, 0);  

        #endif
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
    robusto_qos_register_on_state_change(on_state_change);
};