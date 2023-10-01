#include "ap.h"
#include "communication.h"
#include <robusto_peer.h>
#include <robusto_logging.h>
#include <robusto_concurrency.h>
#include <string.h>
#include <robusto_screen_minimal.h>
#include <robusto_pubsub_client.h>

static float curr_heading;
static char *ap_log_prefix;
#define PUBSUB_SERVICE_ID 1958
static const uint32_t set_evo_course = 126208;
#define SPEED_THROUGH_WATER_PGN 128259L
#define TARGET_HEADING_MAGNETIC 65360L
static robusto_peer_t *nmea_gateway = NULL;
static subscribed_topic_t *nmea_speed_topic = NULL;
static subscribed_topic_t *nmea_ap_topic = NULL;
static subscribed_topic_t *nmea_hdg_topic = NULL;
rob_ret_val_t send_course_correction(int degrees)
{

    if ((nmea_ap_topic) && (nmea_ap_topic->topic_hash > 0))
    {
        ROB_LOGE(ap_log_prefix, "Create course correction message for PGN %lu, HDG %f, CHG %i!",set_evo_course, curr_heading, degrees);
        uint8_t data[12];
        memcpy(&data, &set_evo_course, sizeof(set_evo_course));
        memcpy(&data[4], &curr_heading, sizeof(float));
        memcpy(&data[8], &degrees, sizeof(int));
        
        robusto_pubsub_client_publish(nmea_ap_topic, &data, sizeof(data));
        return ROB_OK;
    }
    else
    {
        ROB_LOGE(ap_log_prefix, "No subscription hash!");
        return ROB_FAIL;
    }
}

void perform_ap_actions(e_action_t action)
{
    int change = 0;
    if (action == ACTION_AP_LEFT_1)
    {
        change = -1;
    }
    else if (action == ACTION_AP_RIGHT_1)
    {
        change = 1;
    }
    else if (action == ACTION_AP_LEFT_10)
    {
        change = -10;
    }
    else if (action == ACTION_AP_RIGHT_10)
    {
        change = 10;
    }
    else
    {
        ROB_LOGE(ap_log_prefix, "Invalid Action in perform_ap_actions.");
        return;
    }
    if ((change > 0) || (change < 0))
    {

        if (send_course_correction(change) == ROB_OK)
        {
            if (curr_heading + change < 0)
            {
                curr_heading = curr_heading + 360;
            }
            else if (curr_heading + change > 360)
            {
                curr_heading = curr_heading - 360;
            }
            curr_heading = curr_heading + change;
#ifdef CONFIG_ROBUSTO_UI_MINIMAL

            char ap_row[20];
            sprintf(&ap_row, "AP H:%-3.f", curr_heading);
            robusto_screen_minimal_write(ap_row, 0, 3);

#endif
        }
        else
        {
#ifdef CONFIG_ROBUSTO_UI_MINIMAL
            char ap_row[15];
            sprintf(&ap_row, "AP H:<!>");
            robusto_screen_minimal_write(ap_row, 0, 3);

#endif
        };
    }
};
void pubsub_nmea_speed_cb(subscribed_topic_t *topic, uint8_t *data, uint16_t data_length)
{
    ROB_LOGI(ap_log_prefix, "In pubsub_nmea_speed_cb, peer %s ", topic->peer->name);
    rob_log_bit_mesh(ROB_LOG_INFO, ap_log_prefix, data, data_length);
    if (*(uint32_t *)data == SPEED_THROUGH_WATER_PGN) {
    #ifdef CONFIG_ROBUSTO_UI_MINIMAL
        char ap_row[15];
        sprintf(&ap_row, "STW %-2.1f", (double)(*(uint16_t *)(data + sizeof(uint32_t))) / 1000);
        robusto_screen_minimal_write(ap_row, 0, 2);
    #endif
    } else {
        ROB_LOGE(ap_log_prefix, "Got pubsub data from %s i don't understand PGN %lu", topic->peer->name, *(uint32_t *)data); 
    }

}

void pubsub_nmea_heading_cb(subscribed_topic_t *topic, uint8_t *data, uint16_t data_length)
{
    ROB_LOGI(ap_log_prefix, "In pubsub_nmea_heading_cb, peer %s ", topic->peer->name);
    rob_log_bit_mesh(ROB_LOG_INFO, ap_log_prefix, data, data_length);
    if (*(uint32_t *)data == TARGET_HEADING_MAGNETIC) {
        curr_heading = *(float *)(data + sizeof(uint32_t));
    #ifdef CONFIG_ROBUSTO_UI_MINIMAL
        char ap_row[15];
        sprintf(&ap_row, "THM %-3.1f", curr_heading);
        robusto_screen_minimal_write(ap_row, 7, 2);
    #endif
    } else {
        ROB_LOGE(ap_log_prefix, "Got pubsub data from %s i don't understand PGN %lu", topic->peer->name, *(uint32_t *)data); 
    }

}


void start_ap(char *_log_prefix)
{
    // Start the pubsub client
    robusto_pubsub_client_init(ap_log_prefix);
    robusto_pubsub_client_start();
    /* The NMEA gateway peer */
    // TTGO-LORA32
    // nmea_gateway = add_peer_by_mac_address("NMEA_Gateway", kconfig_mac_to_6_bytes(0x58bf250541e0), robusto_mt_espnow);
    // Sail hat
    //nmea_gateway = add_peer_by_mac_address("NMEA_Gateway", kconfig_mac_to_6_bytes(0x1097bdd3f6f4), robusto_mt_espnow);
    // TTGO T-Beam
    nmea_gateway = add_peer_by_mac_address("NMEA_Gateway", kconfig_mac_to_6_bytes(0x08b61fc0d660), robusto_mt_espnow);
    
    // DevKit V4
    //nmea_gateway = add_peer_by_mac_address("NMEA_Gateway", kconfig_mac_to_6_bytes(0x30c6f70407c4), robusto_mt_espnow);
    if (!robusto_waitfor_byte(&nmea_gateway->state, PEER_KNOWN_INSECURE, 1000))
    {
        ROB_LOGE(ap_log_prefix, "Failed connecting to NMEA Gateway");
#ifdef CONFIG_ROBUSTO_UI_MINIMAL
        robusto_screen_minimal_write("AP conn. failure!", 0, 3);
#endif
        return;
    }

    ROB_LOGI(ap_log_prefix, "Creating subscription");

    nmea_speed_topic = robusto_pubsub_client_get_topic(nmea_gateway, "NMEA.speed", &pubsub_nmea_speed_cb);
    nmea_hdg_topic = robusto_pubsub_client_get_topic(nmea_gateway, "NMEA.hdg", &pubsub_nmea_heading_cb);
    nmea_ap_topic = robusto_pubsub_client_get_topic(nmea_gateway, "NMEA.ap", NULL);

}
void init_ap(char *_log_prefix)
{

    ap_log_prefix = _log_prefix;
}