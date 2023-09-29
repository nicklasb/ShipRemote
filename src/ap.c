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
static robusto_peer_t *nmea_gateway = NULL;
#define PUBSUB_SERVICE_ID 1958
static const uint32_t set_evo_course = 126208;

#define PUBSUB_SUBSCRIBE 0
#define PUBSUB_UNSUBSCRIBE 1
#define PUBSUB_PUBLISH 2

static subscribed_topic_t *nmea_in_topic = NULL;

rob_ret_val_t send_course_correction(int degrees)
{

    if ((nmea_in_topic) && (nmea_in_topic->topic_hash > 0))
    {
        uint8_t data[12];
        memcpy(&data, &set_evo_course, sizeof(set_evo_course));
        memcpy(&data[4], &curr_heading, sizeof(float));
        memcpy(&data[8], &degrees, sizeof(int));
        robusto_pubsub_client_publish(nmea_in_topic, &data, sizeof(data));
        return ROB_OK;
    }
    else
    {
        ROB_LOGI(ap_log_prefix, "No subscription hash!");
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
    ROB_LOGI(ap_log_prefix, "Got pubsub data from %s peer", topic->peer->name);
    rob_log_bit_mesh(ROB_LOG_INFO, ap_log_prefix, data, data_length);
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
    // nmea_gateway = add_peer_by_mac_address("NMEA_Gateway", kconfig_mac_to_6_bytes(0x1097bdd3f6f4), robusto_mt_espnow);
    // DevKit V4
    nmea_gateway = add_peer_by_mac_address("NMEA_Gateway", kconfig_mac_to_6_bytes(0x30c6f70407c4), robusto_mt_espnow);
    if (!robusto_waitfor_byte(&nmea_gateway->state, PEER_KNOWN_INSECURE, 1000))
    {
        ROB_LOGE(ap_log_prefix, "Failed connecting to NMEA Gateway");
#ifdef CONFIG_ROBUSTO_UI_MINIMAL
        robusto_screen_minimal_write("AP conn. failure!", 0, 3);
#endif
        return;
    }

    ROB_LOGI(ap_log_prefix, "Creating subscription");

    nmea_in_topic = robusto_pubsub_client_subscribe(nmea_gateway, "NMEA.speed", pubsub_nmea_speed_cb);
}
void init_ap(char *_log_prefix)
{

    ap_log_prefix = _log_prefix;
}