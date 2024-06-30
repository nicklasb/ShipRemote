#include "communication.h"
#include "ap.h"

#include <robusto_server_init.h>
#include <robusto_network_init.h>
#include <screen.h>
#include <robusto_message.h>

#include <robusto_incoming.h>
#include <robusto_queue.h>

#include <nav_screen.h>
#include <robusto_qos.h>
#include <math.h>

char *comm_log_prefix;

static robusto_peer_t *nmea_gateway = NULL;

char comm_status[4] = "   ";

robusto_peer_t *get_nmea_peer()
{
    return nmea_gateway;
}

/* TODO: Parametrize sending to gateways and servers */

void print_state(uint8_t offset, e_media_state state, e_media_type media_type)
{

    char state_char;
    switch (state)
    {
    case media_state_problem:
        state_char = '!';
        break;
    case media_state_recovering:
        state_char = 'R';
        break;
    case media_state_working:
        state_char = '*';
        break;
    case media_state_initiating:
        state_char = 'I';
        break;
    default:
        state_char = '?';
    }

    uint8_t media_offset;
    switch (media_type)
    {
    case robusto_mt_espnow:
        media_offset = 0;
        break;
    case robusto_mt_i2c:
        media_offset = 1;
        break;
    case robusto_mt_lora:
        media_offset = 2;
        break;
    default:
        ROB_LOGE(comm_log_prefix, "Unhandled mediatype %s", media_type_to_str(media_type));
        return;
    }
    comm_status[media_offset] = state_char;
#ifdef CONFIG_ROBUSTO_UI
    set_media_states(&comm_status);
#endif
}
void on_send_activity(media_queue_item_t *queue_item, e_media_type media_type)
{
    char activity[4] = "   ";
    switch (media_type)
    {
    case robusto_mt_espnow:
        strcpy(&activity, "_  ");
        break;
    case robusto_mt_i2c:
        strcpy(&activity, " _ ");
        break;
    case robusto_mt_lora:
        strcpy(&activity, "  _");
        break;
    default:
        break;
    }
    set_activity(&activity);
}
void on_state_change(robusto_peer_t *peer, robusto_media_t *info, e_media_type media_type, e_media_state media_state, e_media_problem problem)
{
    print_state((uint8_t)log2(media_type), media_state, media_type);
}

void on_presentation(robusto_peer_t *peer, e_presentation_reason reason)
{

    ROB_LOGE(comm_log_prefix, "Got a presentation request from the gateway, reason %hx", reason);
    if (reason < presentation_update)
    {
        r_delay(2000);
        ROB_LOGE(comm_log_prefix, "Refreshing subscriptions");
        set_subscription_states("RRRRR");
        refresh_subscription();
    }
}
void register_presentation_callback() {
    nmea_gateway->on_presentation = &on_presentation;
}

void start_communication()
{
    /* The NMEA gateway peer */
    // TTGO-LORA32
    // nmea_gateway = add_peer_by_mac_address("NMEA_Gateway", kconfig_mac_to_6_bytes(0x58bf250541e0), robusto_mt_espnow);
    // Sail hat
    // nmea_gateway = add_peer_by_mac_address("NMEA_Gateway", kconfig_mac_to_6_bytes(0x1097bdd3f6f4), robusto_mt_espnow);
    // TTGO T-Beam
    ROB_LOGE(comm_log_prefix, "Add NMEA Gateway peer.");
    set_target_heading("*  ");
    // DevKit V4
    // nmea_gateway = add_peer_by_mac_address("NMEA_Gateway", kconfig_mac_to_6_bytes(0x30c6f70407c4), robusto_mt_espnow);
    // T-Beam LoRa 32
    //nmea_gateway = add_peer_by_mac_address("NMEA_Gateway", kconfig_mac_to_6_bytes(0x08b61fc0d660), robusto_mt_lora | robusto_mt_espnow);
    nmea_gateway = add_peer_by_mac_address("NMEA_Gateway", kconfig_mac_to_6_bytes(0x1097bdd3f6f4), robusto_mt_espnow);
    // This will be set later
    nmea_gateway->on_presentation = NULL;
    
    while (nmea_gateway->state < PEER_KNOWN_INSECURE)
    {
        set_target_heading("** ");
        if (!robusto_waitfor_byte(&nmea_gateway->state, PEER_KNOWN_INSECURE, 3000))
        {
            ROB_LOGE(comm_log_prefix, "Failed connecting to NMEA Gateway");
        }
        r_delay(2000);
    }

    set_target_heading("***");
    ROB_LOGW(comm_log_prefix, "Connnected and presented.");

    robusto_message_sending_register_on_activity(&on_send_activity);
}

void init_communication(char *_log_prefix)
{
    comm_log_prefix = _log_prefix;
    robusto_qos_register_on_state_change(&on_state_change);
}