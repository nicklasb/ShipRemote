#include "loop.h"


static char * loop_log_prefix;

void start_loop(char * _log_prefix) {
    while (1) {
        /* Poll inputs */
        // Poll buttons

        // Poll communications


        /* Do actions */
        
        // Send data

        // Update UI





    }


};

void init_loop(char * _log_prefix) {
    loop_log_prefix = _log_prefix;
};