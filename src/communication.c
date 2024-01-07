#include "communication.h"
#include "ap.h"

#include <robusto_server_init.h>
#include <robusto_network_init.h>

#include <robusto_message.h>

#include <robusto_incoming.h>
#include <robusto_queue.h>

#include <robusto_screen_minimal.h>
#include <robusto_qos.h>
#include <math.h>

char *comm_log_prefix;

static robusto_peer_t *nmea_gateway = NULL;

robusto_peer_t *get_nmea_peer()
{
    return nmea_gateway;
}

/* TODO: Parametrize sending to gateways and servers */

void print_state(uint8_t offset, e_media_state state, e_media_type media_type)
{

    char *state_char;
    switch (state)
    {
    case media_state_problem:
        state_char = "!";
        break;
    case media_state_recovering:
        state_char = "R";
        break;
    case media_state_working:
        state_char = "*";
        break;
    case media_state_initiating:
        state_char = "I";
        break;
    default:
        state_char = "?";
    }

    char *media_char;
    switch (media_type)
    {
    case robusto_mt_espnow:
        media_char = "E";
        break;
    case robusto_mt_i2c:
        media_char = "I";
        break;
    case robusto_mt_lora:
        media_char = "L";
        break;
    default:
        media_char = "?";
    }

#ifdef CONFIG_ROBUSTO_UI_MINIMAL

    robusto_screen_minimal_write(media_char, offset, 0);
    robusto_screen_minimal_write(state_char, offset, 1);
#endif
}

void on_state_change(robusto_peer_t *peer, robusto_media_t *info, e_media_type media_type, e_media_state media_state, e_media_problem problem)
{
    print_state((uint8_t)log2(media_type), media_state, media_type);
}

void start_communication()
{
    /* The NMEA gateway peer */
    // TTGO-LORA32
    // nmea_gateway = add_peer_by_mac_address("NMEA_Gateway", kconfig_mac_to_6_bytes(0x58bf250541e0), robusto_mt_espnow);
    // Sail hat
    // nmea_gateway = add_peer_by_mac_address("NMEA_Gateway", kconfig_mac_to_6_bytes(0x1097bdd3f6f4), robusto_mt_espnow);
    // TTGO T-Beam
    ROB_LOGI(comm_log_prefix, "Add NMEA Gateway peer.");
    nmea_gateway = add_peer_by_mac_address("NMEA_Gateway", kconfig_mac_to_6_bytes(0x08b61fc0d660), robusto_mt_lora);

    // DevKit V4
    // nmea_gateway = add_peer_by_mac_address("NMEA_Gateway", kconfig_mac_to_6_bytes(0x30c6f70407c4), robusto_mt_espnow);
    while (nmea_gateway->state < PEER_KNOWN_INSECURE)
    {
        robusto_screen_minimal_write("Fetching info", 0, 2);
        robusto_screen_minimal_write("from gateway ", 0, 3);
        if (!robusto_waitfor_byte(&nmea_gateway->state, PEER_KNOWN_INSECURE, 3000))
        {
            ROB_LOGE(comm_log_prefix, "Failed connecting to NMEA Gateway");
#ifdef CONFIG_ROBUSTO_UI_MINIMAL
            robusto_screen_minimal_write("No info from ", 0, 2);
            robusto_screen_minimal_write("gateway      ", 0, 3);
#endif
        }
        r_delay(2000);
    }
    robusto_screen_minimal_write("             ", 0, 2);
    robusto_screen_minimal_write("             ", 0, 3);
    ROB_LOGI(comm_log_prefix, "Connnected and presented.");
    // TODO: It would appear that something should happen around here
    /* A Robusto server peer */
    // TTGO-LORA32
    // robusto_server = add_peer_by_mac_address("NMEA_Gateway", kconfig_mac_to_6_bytes(0x58bf250541e0), robusto_mt_espnow);
}

void init_communication(char *_log_prefix)
{
    comm_log_prefix = _log_prefix;
    robusto_qos_register_on_state_change(&on_state_change);
}