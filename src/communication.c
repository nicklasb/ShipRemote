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

/* TODO: Parametrize sending to gateways and servers */

void print_state(uint8_t offset, e_media_state state, e_media_type media_type)
{

    char *state_char;
    switch (state)
    {
    case media_state_down:
        state_char = "_";
        break;
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

    char * media_char;
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