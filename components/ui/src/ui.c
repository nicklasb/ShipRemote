
#include "ui.h"
#include <robusto_ui_init.h>
static char * ui_log_prefix;

void init_ui(char * _log_prefix) {
    ui_log_prefix = _log_prefix;
    robusto_init_ui(ui_log_prefix);
}