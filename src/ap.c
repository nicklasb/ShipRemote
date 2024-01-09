#include "ap.h"
#include "communication.h"
#include <robusto_peer.h>
#include <robusto_logging.h>
#include <robusto_concurrency.h>
#include <string.h>
#include <screen.h>
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

#define PUBSUB_OFFSET 4
#define PUBSUB_HDG_OFFSET 0
#define PUBSUB_SPEED_OFFSET 1
#define PUBSUB_AP_OFFSET 2

// TODO: Just do positions here instead? 

static subscribed_topic_t *nmea_ap_topic = NULL;

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
        ROB_LOGW(ap_log_prefix, "Cannot perform any heading changes before we have actual heading data");
#ifdef CONFIG_ROBUSTO_UI_MINIMAL
            robusto_screen_minimal_write("A", PUBSUB_OFFSET, 0);
#endif
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
            #ifdef CONFIG_ROBUSTO_UI_MINIMAL
            robusto_screen_minimal_write_small("*", PUBSUB_OFFSET, 1);
            #endif
        }
        else
        {
#ifdef CONFIG_ROBUSTO_UI_MINIMAL
            robusto_screen_minimal_write_small("!", PUBSUB_OFFSET, 1);
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
//        robusto_screen_minimal_write(ap_row, COLUMN_2, 3);
#endif
    // TODO: Add SOG
    }
    else
    {
        ROB_LOGE(ap_log_prefix, "Got pubsub data from %s i don't understand PGN %lu", topic->peer->name, *(uint32_t *)data);
    }
}

void pubsub_nmea_heading_cb(subscribed_topic_t *topic, uint8_t *data, uint16_t data_length)
{
    ROB_LOGD(ap_log_prefix, "In pubsub_nmea_heading_cb, peer %s ", topic->peer->name);
    rob_log_bit_mesh(ROB_LOG_DEBUG, ap_log_prefix, data, data_length);
    if (*(uint32_t *)data == TARGET_HEADING_MAGNETIC)
    {
        curr_target_heading = *(int32_t *)(data + sizeof(uint32_t));
#ifdef CONFIG_ROBUSTO_UI_MINIMAL

        uint8_t before = 0;
        uint8_t after = 0;
        if (curr_target_heading < 100) {
            before = (curr_target_heading < 10) ? 1:0;
            after = 1;
        }
        char* thg = robusto_malloc(4);
        sprintf(thg, "%.*s%li%.*s",before, "  ", curr_target_heading,after," ");
        ROB_LOGE("sdfs", "|%s|", thg);
        robusto_screen_minimal_write_xy(thg, 32, 2, FONT_LARGE);
        robusto_free(thg);
        robusto_screen_minimal_write("<", 3, 1);
#endif
    }
    else if (*(uint32_t *)data == HEADING_MAGNETIC)
    {
        curr_heading = *(int32_t *)(data + sizeof(int32_t));
        char hm[20];
        sprintf(&hm, " HM %3li", curr_heading);
        robusto_screen_minimal_write(&hm, COLUMN_2, 3);
    }
    else
    {
        ROB_LOGE(ap_log_prefix, "Got pubsub data from %s i don't understand PGN %lu", topic->peer->name, *(uint32_t *)data);
    }
}

void refresh_subscription()
{

    robusto_screen_minimal_write_small("HSA", PUBSUB_OFFSET, 0);

    robusto_pubsub_client_get_topic(get_nmea_peer(), "NMEA.hdg", &pubsub_nmea_heading_cb, PUBSUB_HDG_OFFSET);
    robusto_pubsub_client_get_topic(get_nmea_peer(), "NMEA.speed", &pubsub_nmea_speed_cb, PUBSUB_SPEED_OFFSET);
    nmea_ap_topic = robusto_pubsub_client_get_topic(get_nmea_peer(), "NMEA.ap", NULL, PUBSUB_AP_OFFSET);
}

void start_ap()
{

    ROB_LOGI(ap_log_prefix, "Starting the AP functionality");
    robusto_pubsub_client_start();
    ROB_LOGW(ap_log_prefix, "Refreshing subscriptions.");
    refresh_subscription();
}

void topic_state_callback(subscribed_topic_t * topic) {
    switch (topic->state) {
        case TOPIC_STATE_STALE:
            robusto_screen_minimal_write_small("-", PUBSUB_OFFSET + topic->display_offset, 1);
            break;
        case TOPIC_STATE_ACTIVE:
            robusto_screen_minimal_write_small("*", PUBSUB_OFFSET + topic->display_offset, 1);
            break;
        case TOPIC_STATE_INACTIVE:
            robusto_screen_minimal_write_small("I", PUBSUB_OFFSET + topic->display_offset, 1);
            break;
        case TOPIC_STATE_PROBLEM:
            robusto_screen_minimal_write_small("!", PUBSUB_OFFSET + topic->display_offset, 1);
            break;
        case TOPIC_STATE_REMOVING:
            robusto_screen_minimal_write_small("R", PUBSUB_OFFSET + topic->display_offset, 1);
            break;
        case TOPIC_STATE_PUBLISHED:
            // TODO: Any UI point to discern between sending and receiving
            robusto_screen_minimal_write_small("*", PUBSUB_OFFSET + topic->display_offset, 1);
            break;
        default:
            robusto_screen_minimal_write_small("?", PUBSUB_OFFSET + topic->display_offset, 1);
            break;   
    }
}


void init_ap(char *_log_prefix)
{
    ap_log_prefix = _log_prefix;
    // Start the pubsub client
    robusto_pubsub_client_init(ap_log_prefix, &topic_state_callback);

}