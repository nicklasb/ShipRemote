#pragma once

#include "nav_screen.h"
#include "screen.h"
#include "lvgl.h"
#include <robusto_logging.h>
static lv_obj_t *screen = NULL;

static lv_obj_t *heading_magnetic;
static lv_obj_t *target_heading;
static lv_obj_t *sog;
static lv_obj_t *stw;
static lv_obj_t *media_states;
static lv_obj_t *subscription_states;

static char *nav_screen_log_prefix;

void set_heading_magnetic(const char *txt)
{
    label_set_text(heading_magnetic, txt);
    ROB_LOGE(nav_screen_log_prefix, "Heading magneting set to %s", txt);
}

void set_target_heading()
{
}

void set_sog()
{
}

void set_stw()
{
}

void set_media_states()
{
}

void set_subscription_states()
{
}

void start_nav_screen()
{
    // Initiate all the
    if (!screen)
    {
        ROB_LOGE(nav_screen_log_prefix, "Nav screen initializing");
        screen = get_current_screen();
        heading_magnetic = lv_label_create(screen);
        lv_obj_set_width(heading_magnetic, 32);
        // lv_obj_set_style_text_font(heading_magnetic);
        lv_obj_align(heading_magnetic, LV_ALIGN_TOP_MID, 0, 0);
    }
    ROB_LOGE(nav_screen_log_prefix, "Nav screen initialized");
}

void init_nav_screen(char *_log_prefix)
{
    nav_screen_log_prefix = _log_prefix;
    /*
        static lv_style_t large_font;
        lv_style_init(&large_font);
        lv_style_set_text_font(&large_font, LV_STATE_DEFAULT, &arial70px);
        */
}