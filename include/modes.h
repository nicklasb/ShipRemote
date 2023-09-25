
#pragma once
#include "robconfig.h"

typedef enum e_modes {
    MODE_HELM /* In this mode, the focus is on helming a vessel */
    #if 0
    MODE_CONTROL, /* Control mode: Controlling systems */
    MODE_MONITOR, /* Monitor mode: Monitoring systems */
    MODE_AWAY, /* Away mode: Monitoring, forwarding */
    MODE_DISTRESS, /* Rescue mode: Facilitating a rescue */
    #endif
}