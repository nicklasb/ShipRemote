#include "ap.h"
#include "communication.h"
#include <robusto_peer.h>
#include <robusto_logging.h>
#include <robusto_concurrency.h>
#include <string.h>
#include <screen.h>
#include <robusto_pubsub_client.h>
#include <inttypes.h>
#include "nav_screen.h"

static int32_t curr_target_heading = -1;
static int32_t curr_heading = -1;
static char *ap_log_prefix;
#define COLUMN_2 8

static const uint32_t set_evo_course = 126208;


// TODO: Just do positions here instead?

char pubsub_status[] = "    ";

static subscribed_topic_t *nmea_ap_topic = NULL;

rob_ret_val_t send_course_correction(int32_t degrees)
{
    if ((nmea_ap_topic) && (nmea_ap_topic->topic_hash > 0))
    {
        ROB_LOGW(ap_log_prefix, "Create course correction message for PGN %lu, HDG %li, CHG %li!", set_evo_course, curr_target_heading, degrees);
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
        return;
    }
    // TODO: Do no changes until an initial magnetic direction or target magnetic is available. To not steer up on a cliff.
    int32_t change = 0;
    if (action == ACTION_AP_LEFT_1)
    {
        set_target_heading(" < ");
        change = -1;
    }
    else if (action == ACTION_AP_RIGHT_1)
    {
        set_target_heading(" > ");
        change = 1;
    }
    else if (action == ACTION_AP_LEFT_10)
    {
        set_target_heading("<<<");
        change = -10;
    }
    else if (action == ACTION_AP_RIGHT_10)
    {
        set_target_heading(">>>");
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
#ifdef CONFIG_ROBUSTO_UI
            pubsub_status[PUBSUB_HDG_OFFSET] = '*';
            set_subscription_states(&pubsub_status);
#endif
        }
        else
        {
#ifdef CONFIG_ROBUSTO_UI
            pubsub_status[PUBSUB_HDG_OFFSET] = '!';
            set_subscription_states(&pubsub_status);
#endif
        };
    }
};

void pubsub_nmea_ap_out_cb(subscribed_topic_t *topic, uint8_t *data, uint16_t data_length)
{
    ROB_LOGI(ap_log_prefix, "In pubsub_nmea_ap_out_cb, peer %s ", topic->peer->name);
    rob_log_bit_mesh(ROB_LOG_INFO, ap_log_prefix, data, data_length);
    if (*(uint32_t *)data == PILOT_STATE)
    {
#ifdef CONFIG_ROBUSTO_UI
    char pilot_state_row[15];
    uint32_t state = *(int32_t *)(data + sizeof(int32_t));
    switch (state) {
        case PILOT_MODE_STANDBY:
            sprintf(&pilot_state_row, "Stby");
            break;
        case PILOT_MODE_AUTO:
            sprintf(&pilot_state_row, "Auto");
            break;
        case PILOT_MODE_WIND:
            sprintf(&pilot_state_row, "Wind");
            break;
        case PILOT_MODE_TRACK:
            sprintf(&pilot_state_row, "Track");
            break;
    };

    set_pilot_state(pilot_state_row);
#endif
    }
    else
    {
        ROB_LOGE(ap_log_prefix, "Got pubsub data from %s i don't understand PGN %lu", topic->peer->name, *(uint32_t *)data);
    }
}

void pubsub_nmea_speed_cb(subscribed_topic_t *topic, uint8_t *data, uint16_t data_length)
{
    ROB_LOGI(ap_log_prefix, "In pubsub_nmea_speed_cb, peer %s ", topic->peer->name);
    rob_log_bit_mesh(ROB_LOG_INFO, ap_log_prefix, data, data_length);
    if (*(uint32_t *)data == SPEED_THROUGH_WATER_PGN)
    {
#ifdef CONFIG_ROBUSTO_UI
        char ap_row[15];
        sprintf(&ap_row, "%-2.1f", (double)(*(uint16_t *)(data + sizeof(uint32_t))) / 1000);
        set_stw(ap_row);
#endif
        // TODO: Add SOG
    }
    else if (*(uint32_t *)data == SPEED_COURSE_OVER_GROUND)
    {
#ifdef CONFIG_ROBUSTO_UI
        set_sog("?");
        // char ap_row[15];
        // sprintf(&ap_row, "%-2.1f", (double)(*(uint16_t *)(data + sizeof(uint32_t))) / 1000);
        // set_spg(ap_row);
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
    ROB_LOGI(ap_log_prefix, "In pubsub_nmea_heading_cb, peer %s ", topic->peer->name);
    rob_log_bit_mesh(ROB_LOG_DEBUG, ap_log_prefix, data, data_length);
    if (*(uint32_t *)data == TARGET_HEADING_MAGNETIC)
    {
        curr_target_heading = *(int32_t *)(data + sizeof(uint32_t));
#ifdef CONFIG_ROBUSTO_UI
        uint8_t before = 0;
        uint8_t after = 0;
        if (curr_target_heading < 100)
        {
            before = (curr_target_heading < 10) ? 1 : 0;
            after = 1;
        }
        char *thg = robusto_malloc(4);
        sprintf(thg, "%.*s%li%.*s", before, "  ", curr_target_heading, after, " ");
        set_target_heading(thg);
        robusto_free(thg);
        pubsub_status[PUBSUB_HDG_OFFSET] = '*';
        set_subscription_states(&pubsub_status);

#endif
    }
    else if (*(uint32_t *)data == HEADING_MAGNETIC)
    {
        curr_heading = *(int32_t *)(data + sizeof(int32_t));
        char hm[20];
        sprintf(&hm, "%3li", curr_heading);
        set_heading_magnetic(&hm);
        pubsub_status[PUBSUB_HDG_MAG_OFFSET] = '*';
        set_subscription_states(&pubsub_status);
    }
    else
    {
        ROB_LOGE(ap_log_prefix, "Got pubsub data from %s i don't understand PGN %lu", topic->peer->name, *(uint32_t *)data);
    }
}

void refresh_subscription()
{

    robusto_pubsub_client_get_topic(get_nmea_peer(), "NMEA.hdg", &pubsub_nmea_heading_cb, PUBSUB_HDG_OFFSET);
    r_delay(200);
    robusto_pubsub_client_get_topic(get_nmea_peer(), "NMEA.speed", &pubsub_nmea_speed_cb, PUBSUB_SPEED_OFFSET);
    r_delay(200);
    robusto_pubsub_client_get_topic(get_nmea_peer(), "NMEA.ap_out", &pubsub_nmea_ap_out_cb, PUBSUB_AP_STATE_OFFSET);
    r_delay(200);
    nmea_ap_topic = robusto_pubsub_client_get_topic(get_nmea_peer(), "NMEA.ap_in", NULL, PUBSUB_AP_OFFSET);
}

void start_ap()
{

    ROB_LOGW(ap_log_prefix, "Starting the AP functionality");
    robusto_pubsub_client_start();
    ROB_LOGW(ap_log_prefix, "Refreshing subscriptions.");
    refresh_subscription();
}

void topic_state_callback(subscribed_topic_t *topic)
{
    char state_char = ' ';
    switch (topic->state)
    {
    case TOPIC_STATE_STALE:
        state_char = '-';
        break;
    case TOPIC_STATE_ACTIVE:
        state_char = '*';
        break;
    case TOPIC_STATE_INACTIVE:
        state_char = 'I';
        break;
    case TOPIC_STATE_PROBLEM:
        state_char = '!';
        break;
    case TOPIC_STATE_REMOVING:
        state_char = 'R';
        break;
    case TOPIC_STATE_PUBLISHED:
        // TODO: Any UI point to discern between sending and receiving
        state_char = '*';
        break;
    default:
        state_char = '?';
        break;
    }
    pubsub_status[topic->display_offset] = state_char;
    set_subscription_states(&pubsub_status);
}

void init_ap(char *_log_prefix)
{
    ap_log_prefix = _log_prefix;
    // Start the pubsub client
    robusto_pubsub_client_init(ap_log_prefix, &topic_state_callback);
}