
#include "communication.h"

#include <robusto_server_init.h>
#include <robusto_network_init.h>

#include <robusto_message.h>

#include <robusto_incoming.h>
#include <robusto_queue.h>


char * comm_log_prefix;

/* TODO: Parametrize sending to gateways and servers */



void start_communication() {

    // TODO: It would appear that something should happen around here
    /* A Robusto server peer */
    // TTGO-LORA32
    //robusto_server = add_peer_by_mac_address("NMEA_Gateway", kconfig_mac_to_6_bytes(0x58bf250541e0), robusto_mt_espnow);

}

void init_communication(char * _log_prefix) {
    comm_log_prefix = _log_prefix;
    register_network_service();
    register_server_service();
    
}