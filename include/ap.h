#pragma once

#include <robconfig.h>
#include "actions.h"

void refresh_subscription();
void perform_ap_actions(e_action_t action);
void start_ap();
void init_ap(char * _log_prefix);

