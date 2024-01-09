#include "screen.h"
#include <robusto_screen.h>
#include <lvgl.h>
static char * ui_log_prefix;

void robusto_screen_minimal_write_xy(char * txt, uint16_t x, uint16_t y, uint8_t height) {

}
void robusto_screen_minimal_write(char * txt, uint8_t col, uint8_t row) {
    robusto_screen_minimal_write_xy(txt, col * 8, row * 16, 16);
}

void robusto_screen_minimal_write_small(char * txt, uint8_t col, uint8_t row) {
    robusto_screen_minimal_write_xy(txt, col * 8, row * 12, 12);
}




void init_screen(char * _log_prefix) {
    ui_log_prefix = _log_prefix;
    robusto_screen_init(ui_log_prefix);

    lv_disp_t * display = robusto_screen_lvgl_get_active_display();
    if (robusto_screen_lvgl_port_lock(0))
    {

        lv_obj_t *scr = lv_disp_get_scr_act(display);
        lv_obj_t *label = lv_label_create(scr);
        lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR); /* Circular scroll */
        lv_label_set_text(label, "Robusto framework example. This should look good on the screen...");
        /* Size of the screen (if you use rotation 90 or 270, please set disp->driver->ver_res) */
        lv_obj_set_width(label, display->driver->hor_res);
        lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);

        robusto_screen_lvgl_port_unlock();
    }

}