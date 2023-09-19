
#include "communication.h"
#include <robusto_retval.h>
#include <robusto_message.h>
#include <robusto_peer.h>
#include <robusto_incoming.h>
#include <robusto_queue.h>
#include <string.h>

char * comm_log_prefix;

double curr_course = 0;

robusto_peer_t * nmea_gateway = NULL;

#define NMEA_SERVICE_ID 1959
uint32_t set_evo_course = 126208;

rob_ret_val_t send_course_correction(float degrees) {
    if (nmea_gateway && nmea_gateway->state != PEER_UNKNOWN) {
        uint8_t data[7];
        memcpy(&data, &set_evo_course, 3);
        memcpy(&data[3], &degrees, 4);
        queue_state* q_state = robusto_malloc(sizeof(queue_state));
    
        rob_ret_val_t ret_val_flag;
        send_message_binary(nmea_gateway, NMEA_SERVICE_ID, 0, data, 7, q_state);
        if (!robusto_waitfor_queue_state(q_state, 1000, &ret_val_flag)) {
            ROB_LOGE(comm_log_prefix, "Failed sending course, queue state %hhu , reason code: %hi", *(uint8_t*)q_state[0], ret_val_flag);
        } else {
            ROB_LOGW(comm_log_prefix, "Successfully queued hello message (not waiting for receipt)!");
        }
        q_state = robusto_free_queue_state(q_state);
        return ROB_OK;
    } else {
        return ROB_FAIL;
    }
    
}

void init_communication(char * _log_prefix) {
    comm_log_prefix = _log_prefix;





}