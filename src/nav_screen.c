
#include "nav_screen.h"
#include "screen.h"
#include "lvgl.h"
#include <robusto_logging.h>
#include <robusto_system.h>
static lv_obj_t *screen = NULL;

static lv_obj_t *heading_magnetic;
static lv_obj_t *target_heading;
static lv_obj_t *sog;
static lv_obj_t *stw;
static lv_obj_t *media_states;
static lv_obj_t *subscription_states;

static lv_style_t *large_style;
static lv_style_t *small_style_l;
static lv_style_t *small_style_r;
static lv_style_t *tiny_style_l;

static char *nav_screen_log_prefix;


void set_heading_magnetic(const char *txt)
{
    label_set_text(heading_magnetic, txt);
}

void set_target_heading(const char *txt)
{
    label_set_text(target_heading, txt);
}

void set_sog(const char *txt)
{
    label_set_text(sog, txt);
}

void set_stw(const char *txt)
{
    label_set_text(stw, txt);
}

void set_media_states(const char *txt)
{
    label_set_text(media_states, txt);
}

void set_subscription_states(const char *txt)
{
    label_set_text(subscription_states, txt);
}

void start_nav_screen()
{
    // Initiate all the
    if (!screen)
    {
        ROB_LOGE(nav_screen_log_prefix, "Nav screen initializing");
        screen = get_current_screen();

        target_heading = lv_label_create(screen);
        lv_obj_set_width(target_heading, 64);
        lv_obj_add_style(target_heading, large_style, LV_STATE_DEFAULT);
        lv_obj_align(target_heading, LV_ALIGN_TOP_MID, 0, 0);
        label_set_text(target_heading, "000");

        heading_magnetic = lv_label_create(screen);
        lv_obj_set_width(heading_magnetic, 32);
        lv_obj_add_style(heading_magnetic, small_style_r, LV_STATE_DEFAULT);
        lv_obj_align(heading_magnetic, LV_ALIGN_TOP_RIGHT, 0, 0);
        label_set_text(heading_magnetic, "000");

        sog = lv_label_create(screen);
        lv_obj_set_width(sog, 32);
        lv_obj_align(sog, LV_ALIGN_TOP_LEFT, 0, 0);
        lv_obj_add_style(sog, small_style_l, LV_STATE_DEFAULT);
        label_set_text(sog, "0");

        stw = lv_label_create(screen);
        lv_obj_set_width(stw, 32);
        lv_obj_align(stw, LV_ALIGN_TOP_LEFT, 0, 16);
        lv_obj_add_style(stw, small_style_l, LV_STATE_DEFAULT);
        label_set_text(stw, "0");


        media_states = lv_label_create(screen);
        lv_obj_set_width(media_states, 24);
        lv_obj_align(media_states, LV_ALIGN_BOTTOM_LEFT, 0, 0);
        lv_obj_add_style(media_states, small_style_l, LV_STATE_DEFAULT);
        label_set_text(media_states, "  ");

        lv_obj_t *media_label = lv_label_create(screen);
        lv_obj_set_width(media_label, 24);
        lv_obj_align(media_label, LV_ALIGN_BOTTOM_LEFT, 0, -14);
        lv_obj_add_style(media_label, tiny_style_l, LV_STATE_DEFAULT);
        label_set_text(media_label, "E I L");

        subscription_states = lv_label_create(screen);
        lv_obj_set_width(subscription_states, 48);
        lv_obj_align(subscription_states, LV_ALIGN_BOTTOM_LEFT, 24, 0);
        lv_obj_add_style(subscription_states, small_style_l, LV_STATE_DEFAULT);
        label_set_text(subscription_states, "    ");

        lv_obj_t *subscription_label = lv_label_create(screen);
        lv_obj_set_width(subscription_label, 48);
        lv_obj_align(subscription_label, LV_ALIGN_BOTTOM_LEFT, 24, -14);
        lv_obj_add_style(subscription_label, tiny_style_l, LV_STATE_DEFAULT);
        label_set_text(subscription_label, "H M S A");

    }
    ROB_LOGE(nav_screen_log_prefix, "Nav screen initialized");
}

void init_nav_screen(char *_log_prefix)
{
    nav_screen_log_prefix = _log_prefix;

    large_style = robusto_malloc(sizeof(lv_style_t));
    lv_style_init(large_style);
    lv_style_set_text_font(large_style, &lv_font_montserrat_32);
    lv_style_set_text_align(large_style, LV_TEXT_ALIGN_CENTER);

    small_style_l = robusto_malloc(sizeof(lv_style_t));
    lv_style_init(small_style_l);
    lv_style_set_text_align(small_style_l, LV_TEXT_ALIGN_LEFT);

    small_style_r = robusto_malloc(sizeof(lv_style_t));
    lv_style_init(small_style_r);
    lv_style_set_text_align(small_style_r, LV_TEXT_ALIGN_RIGHT);

    tiny_style_l = robusto_malloc(sizeof(lv_style_t));
    lv_style_init(tiny_style_l);
    lv_style_set_text_font(tiny_style_l, &lv_font_montserrat_8);
    lv_style_set_text_align(tiny_style_l, LV_TEXT_ALIGN_LEFT);
    // TODO: Probably these should be centered
}