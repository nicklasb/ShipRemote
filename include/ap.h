#pragma once

#include <robconfig.h>
#include "actions.h"
#include <robusto_peer.h>
// Raymarine specific
// AP commands
#define SET_EVO_PILOT_COURSE 126208UL
// AP output
#define TARGET_HEADING_TRUE 65360UL
#define TARGET_HEADING_MAGNETIC 653601UL // Own differentiator
#define HEADING_MAGNETIC 65359UL
#define HEADING_TRUE 653591UL // Own differentiator
#define SPEED_THROUGH_WATER_PGN 128259UL
#define SPEED_COURSE_OVER_GROUND 129026UL
#define PILOT_STATE 65379UL

#define PUBSUB_HDG_OFFSET 0
#define PUBSUB_HDG_MAG_OFFSET 1
#define PUBSUB_SPEED_OFFSET 2
#define PUBSUB_AP_OFFSET 3
#define PUBSUB_AP_STATE_OFFSET 4

enum RaymarinePilotModes
{
PILOT_MODE_STANDBY = 1,
PILOT_MODE_AUTO = 2,
PILOT_MODE_WIND = 3,
PILOT_MODE_TRACK = 4
};

void refresh_subscription();
void perform_ap_actions(e_action_t action);


void start_ap();
void init_ap(char * _log_prefix);

