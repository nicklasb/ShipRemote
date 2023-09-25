
#include "communication.h"

#include <robusto_server_init.h>
#include <robusto_network_init.h>

#include <robusto_message.h>

#include <robusto_incoming.h>
#include <robusto_queue.h>


char * comm_log_prefix;

/* TODO: Parametrize sending to gateways and servers */

rob_ret_val_t send_to_peer(robusto_peer_t * peer, uint16_t service_id, uint8_t * data, uint8_t data_length) {

    if ((peer != NULL) && (peer->state != PEER_UNKNOWN)) {

        queue_state* q_state = robusto_malloc(sizeof(queue_state));

        rob_ret_val_t ret_val_flag;
        send_message_binary(peer, service_id, 0, data, data_length, q_state);
        if (!robusto_waitfor_queue_state(q_state, 1000, &ret_val_flag)) {
            ROB_LOGE(comm_log_prefix, "Failed sending to %s, queue state %hhu , reason code: %hi", peer->name, (uint8_t)q_state[0], ret_val_flag);
        } else {
            ROB_LOGW(comm_log_prefix, "Successfully sent message to NMEA gateway and got a receipt.");
        }
        q_state = robusto_free_queue_state(q_state);
        return ROB_OK;
    } else {
        ROB_LOGW(comm_log_prefix, "Could not send to NMEA gateway, not initiated or peer still unknown.");
        return ROB_FAIL;
    }
}

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