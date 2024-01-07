#pragma once
#include <robconfig.h>
#include <robusto_retval.h>
#include <robusto_peer.h>

rob_ret_val_t send_to_peer(robusto_peer_t * peer, uint16_t service_id, uint8_t * data, uint8_t data_length);
robusto_peer_t * get_nmea_peer();

void start_communication();
void init_communication(char * _log_prefix);