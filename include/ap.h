#pragma once

#include <robconfig.h>
#include "actions.h"
#include <robusto_peer.h>

void refresh_subscription();
void perform_ap_actions(e_action_t action);
robusto_peer_t * get_nmea_peer();

void start_ap();
void init_ap(char * _log_prefix);

