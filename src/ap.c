#include "ap.h"
#include "communication.h"
#include <robusto_peer.h>
#include <robusto_logging.h>
#include <robusto_concurrency.h>
#include <string.h>
#include <robusto_screen_minimal.h>
#include <robusto_pubsub_client.h>
#include <inttypes.h>

static int32_t curr_target_heading = -1;
static int32_t curr_heading = -1;
static char *ap_log_prefix;
#define COLUMN_2 8

static const uint32_t set_evo_course = 126208;

#define SPEED_THROUGH_WATER_PGN 128259UL
#define TARGET_HEADING_MAGNETIC 65360UL
#define HEADING_MAGNETIC 65359UL

static robusto_peer_t *nmea_gateway = NULL;
static subscribed_topic_t *nmea_speed_topic = NULL;
static subscribed_topic_t *nmea_ap_topic = NULL;
static subscribed_topic_t *nmea_hdg_topic = NULL;

rob_ret_val_t send_course_correction(int32_t degrees)
{

    if ((nmea_ap_topic) && (nmea_ap_topic->topic_hash > 0))
    {
        ROB_LOGE(ap_log_prefix, "Create course correction message for PGN %lu, HDG %li, CHG %li!", set_evo_course, curr_target_heading, degrees);
        uint8_t *data = robusto_malloc(12);
        memcpy(data, &set_evo_course, sizeof(set_evo_course));
        memcpy(data + 4, &curr_target_heading, sizeof(int32_t));
        memcpy(data + 8, &degrees, sizeof(int32_t));

        robusto_pubsub_client_publish(nmea_ap_topic, data, 12);
        robusto_free(data);
        return ROB_OK;
    }
    else
    {
        ROB_LOGE(ap_log_prefix, "No subscription hash, refreshing subscriptions!");
        refresh_subscription();
        return ROB_FAIL;
    }
}

void perform_ap_actions(e_action_t action)
{
    if (curr_target_heading < 0)
    {
        // We first need to have gotten a heading before we can allow AP commands (0 would turn the boat).
        return;
    }
    // TODO: Do no changes until an initial magnetic direction or target magnetic is available. To not steer up on a cliff.
    int32_t change = 0;
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
            if (curr_target_heading + change < 0)
            {
                curr_target_heading = curr_target_heading + change + 360;
            }
            else if (curr_target_heading + change > 359)
            {
                curr_target_heading = curr_target_heading + change - 360;
            }
            curr_target_heading = curr_target_heading + change;
            robusto_screen_minimal_write("*", 12, 0);
        }
        else
        {
#ifdef CONFIG_ROBUSTO_UI_MINIMAL
            char ap_row[15];
            sprintf(&ap_row, "HDG <!>");
            robusto_screen_minimal_write(ap_row, 0, 3);
            robusto_screen_minimal_write("P", 12, 0);
#endif
        };
    }
};
void pubsub_nmea_speed_cb(subscribed_topic_t *topic, uint8_t *data, uint16_t data_length)
{
    ROB_LOGI(ap_log_prefix, "In pubsub_nmea_speed_cb, peer %s ", topic->peer->name);
    rob_log_bit_mesh(ROB_LOG_INFO, ap_log_prefix, data, data_length);
    if (*(uint32_t *)data == SPEED_THROUGH_WATER_PGN)
    {
#ifdef CONFIG_ROBUSTO_UI_MINIMAL
        char ap_row[15];
        sprintf(&ap_row, "STW %-2.1f", (double)(*(uint16_t *)(data + sizeof(uint32_t))) / 1000);
        robusto_screen_minimal_write(ap_row, COLUMN_2, 2);
#endif
    }
    else
    {
        ROB_LOGE(ap_log_prefix, "Got pubsub data from %s i don't understand PGN %lu", topic->peer->name, *(uint32_t *)data);
    }
}

void pubsub_nmea_heading_cb(subscribed_topic_t *topic, uint8_t *data, uint16_t data_length)
{
    ROB_LOGI(ap_log_prefix, "In pubsub_nmea_heading_cb, peer %s ", topic->peer->name);
    rob_log_bit_mesh(ROB_LOG_INFO, ap_log_prefix, data, data_length);
    if (*(uint32_t *)data == TARGET_HEADING_MAGNETIC)
    {
        curr_target_heading = *(int32_t *)(data + sizeof(uint32_t));
#ifdef CONFIG_ROBUSTO_UI_MINIMAL
        char ap_row[15];
        sprintf(&ap_row, "THM %3li ", curr_target_heading);
        robusto_screen_minimal_write(ap_row, 0, 2);
#endif
    }
    else if (*(uint32_t *)data == HEADING_MAGNETIC)
    {
        curr_heading = *(int32_t *)(data + sizeof(int32_t));
        char ap_row[20];
        sprintf(&ap_row, "HDG %3li", curr_heading);
        robusto_screen_minimal_write(ap_row, 0, 3);
        robusto_screen_minimal_write("*", 14, 0);
    }
    else
    {
        ROB_LOGE(ap_log_prefix, "Got pubsub data from %s i don't understand PGN %lu", topic->peer->name, *(uint32_t *)data);
    }
}


void refresh_subscription()
{
    if ((nmea_gateway->state == PEER_UNKNOWN) || (nmea_gateway->state == PEER_PRESENTING))
    {
        ROB_LOGE(ap_log_prefix, "Peer unknown/presenting, not creating subscription.");
        return;
    }
    ROB_LOGI(ap_log_prefix, "Creating subscriptions.");
    robusto_pubsub_remove_topic(nmea_speed_topic);
    nmea_speed_topic = robusto_pubsub_client_get_topic(nmea_gateway, "NMEA.speed", &pubsub_nmea_speed_cb);
    if (!nmea_speed_topic)
    {
        robusto_screen_minimal_write("S", 13, 0);
    }
    else
    {
        robusto_screen_minimal_write("*", 13, 0);
    }
    robusto_pubsub_remove_topic(nmea_hdg_topic);
    nmea_hdg_topic = robusto_pubsub_client_get_topic(nmea_gateway, "NMEA.hdg", &pubsub_nmea_heading_cb);
    if (!nmea_hdg_topic)
    {
        robusto_screen_minimal_write("H", 14, 0);
    }
    else
    {
        robusto_screen_minimal_write("*", 14, 0);
    }
    robusto_pubsub_remove_topic(nmea_ap_topic);
    nmea_ap_topic = robusto_pubsub_client_get_topic(nmea_gateway, "NMEA.ap", NULL);
    if (!nmea_ap_topic)
    {
        robusto_screen_minimal_write("A", 15, 0);
    }
    else
    {
        robusto_screen_minimal_write("*", 15, 0);
    }
}

void start_ap()
{

    ROB_LOGI(ap_log_prefix, "Starting the AP functionality");

    /* The NMEA gateway peer */
    // TTGO-LORA32
    // nmea_gateway = add_peer_by_mac_address("NMEA_Gateway", kconfig_mac_to_6_bytes(0x58bf250541e0), robusto_mt_espnow);
    // Sail hat
    // nmea_gateway = add_peer_by_mac_address("NMEA_Gateway", kconfig_mac_to_6_bytes(0x1097bdd3f6f4), robusto_mt_espnow);
    // TTGO T-Beam
    nmea_gateway = add_peer_by_mac_address("NMEA_Gateway", kconfig_mac_to_6_bytes(0x08b61fc0d660), robusto_mt_espnow);

    // DevKit V4
    // nmea_gateway = add_peer_by_mac_address("NMEA_Gateway", kconfig_mac_to_6_bytes(0x30c6f70407c4), robusto_mt_espnow);
    if (!robusto_waitfor_byte(&nmea_gateway->state, PEER_KNOWN_INSECURE, 1000))
    {
        ROB_LOGE(ap_log_prefix, "Failed connecting to NMEA Gateway");
#ifdef CONFIG_ROBUSTO_UI_MINIMAL
        robusto_screen_minimal_write("AP conn. failure!", 0, 3);
#endif
        return;
    }
    else
    {
        refresh_subscription();
    }
}
void init_ap(char *_log_prefix)
{
    ap_log_prefix = _log_prefix;
    // Start the pubsub client
    robusto_pubsub_client_init(ap_log_prefix);
    robusto_pubsub_client_start();
}