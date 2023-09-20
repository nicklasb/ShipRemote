
#include "communication.h"

#include <robusto_server_init.h>
#include <robusto_network_init.h>

#include <robusto_message.h>
#include <robusto_peer.h>
#include <robusto_incoming.h>
#include <robusto_queue.h>
#include <string.h>

char * comm_log_prefix;

float curr_heading = 0.5;

robusto_peer_t * nmea_gateway = NULL;

#define NMEA_SERVICE_ID 1959
uint32_t set_evo_course = 126208;

rob_ret_val_t send_course_correction(int degrees) {
    if ((nmea_gateway != NULL) && (nmea_gateway->state != PEER_UNKNOWN)) {
        uint8_t data[12];
        memcpy(&data, &set_evo_course, sizeof(set_evo_course));
        memcpy(&data[4], &curr_heading, sizeof(float));
        memcpy(&data[8], &degrees, sizeof(int));

        queue_state* q_state = robusto_malloc(sizeof(queue_state));
    
        rob_ret_val_t ret_val_flag;
        send_message_binary(nmea_gateway, NMEA_SERVICE_ID, 0, &data, sizeof(data), q_state);
        if (!robusto_waitfor_queue_state(q_state, 1000, &ret_val_flag)) {
            ROB_LOGE(comm_log_prefix, "Failed sending course, queue state %hhu , reason code: %hi", *(uint8_t*)q_state[0], ret_val_flag);
        } else {
            ROB_LOGW(comm_log_prefix, "Successfully sent message and got a receipt.");
        }
        q_state = robusto_free_queue_state(q_state);
        return ROB_OK;
    } else {
        ROB_LOGW(comm_log_prefix, "Could not send, peer unknown.");
        return ROB_FAIL;
        
    }
    
}

void start_communication() {

    nmea_gateway = add_peer_by_mac_address("NMEA_Gateway", kconfig_mac_to_6_bytes(0x58bf250541e0), robusto_mt_espnow);
    //robusto_peer_t *peer = add_peer_by_mac_address("NMEA_Gateway", kconfig_mac_to_6_bytes(0x1097bdd3f6f4), robusto_mt_espnow);
}

void init_communication(char * _log_prefix) {
    comm_log_prefix = _log_prefix;
    register_network_service();
    register_server_service();
}