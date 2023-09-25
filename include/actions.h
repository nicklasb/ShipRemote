#pragma once

#include <robconfig.h>




typedef enum e_actions {
    ACTION_NONE, /* No Action*/

    /* Autopilot actions */
    ACTION_AP_LEFT_1, /* Tell Autopilot to turn left 1 degree */ 
    ACTION_AP_LEFT_10, /* Tell Autopilot to turn left 10 degrees */ 
    ACTION_AP_RIGHT_1, /* Tell Autopilot to turn left 1 degree */ 
    ACTION_AP_RIGHT_10, /* Tell Autopilot to turn left 10 degrees */ 
    ACTION_AP_STANDBY, /* Disable Autopilot */ 
    ACTION_AP_ENABLE, /* Enable Autopilot */ 
    /* NMEA data */
    ACTION_UPDATE_HEADING, /* Update heading */
    ACTION_UPDATE_SOG, /* Update Speed over ground */
    ACTION_UPDATE_STW, /* Update Speed through water */
    ACTION_UPDATE_TRUE_WIND_ANGLE /* Update Speed through water */

} e_action_t;