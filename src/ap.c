#include "ap.h"
#include "communication.h"
#include <robusto_peer.h>
#include <string.h>

static int curr_heading;
static char * ap_log_prefix;
robusto_peer_t * nmea_gateway = NULL;
#define NMEA_SERVICE_ID 1959
uint32_t set_evo_course = 126208;

rob_ret_val_t send_course_correction(int degrees) {
    
    
        uint8_t data[12];
        memcpy(&data, &set_evo_course, sizeof(set_evo_course));
        memcpy(&data[4], &curr_heading, sizeof(float));
        memcpy(&data[8], &degrees, sizeof(int));
        send_to_peer(nmea_gateway, NMEA_SERVICE_ID, data, 12);
        set_evo_course = set_evo_course + degrees;
        return ROB_OK;

    
}


void perform_ap_actions(e_action_t action) {
    int change = 0;
    if (action == ACTION_AP_LEFT_1) {
        change = -1;
    } else 
    if (action == ACTION_AP_RIGHT_1) {
        change = 1;
    } else 
    if (action == ACTION_AP_LEFT_10) {
        change = -10;
    } else 
    if (action == ACTION_AP_RIGHT_10) {
        change = 10;
    } else {
        ROB_LOGE(ap_log_prefix, "Invalid Action in perform_ap_actions.");
        return;
    }
    if (change != 0) {
        if (curr_heading + change < 0) {
            curr_heading += 365;
        } else if (curr_heading + change > 365) {
            curr_heading -= 365;
        }

    }
    send_course_correction(change);


#ifdef ROBUSTO_UI_MINIMAL

    char ap_row[20];
    sprintf(&ap_row, "AP|D%-3d", curr_heading);
    robusto_ui_minimal_write(ap_row, 0, 3);  

#endif


};

void start_ap(char * _log_prefix) {
   /* The NMEA gateway peer */
    // TTGO-LORA32
    //nmea_gateway = add_peer_by_mac_address("NMEA_Gateway", kconfig_mac_to_6_bytes(0x58bf250541e0), robusto_mt_espnow);
    // Sail hat
    //nmea_gateway = add_peer_by_mac_address("NMEA_Gateway", kconfig_mac_to_6_bytes(0x1097bdd3f6f4), robusto_mt_espnow);
    // DevKit V4
    nmea_gateway = add_peer_by_mac_address("NMEA_Gateway", kconfig_mac_to_6_bytes(0x30c6f70407c4), robusto_mt_espnow);

}
void init_ap(char * _log_prefix) {

    ap_log_prefix = _log_prefix;
}