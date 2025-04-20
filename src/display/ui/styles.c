#include "styles.h"
#include "images.h"
#include "fonts.h"

void apply_style_topbar(lv_obj_t *obj) {
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff32324d), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(obj, lv_color_hex(0xff1e1a1a), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_line_color(obj, lv_color_hex(0xff1e1a1a), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_color(obj, lv_color_hex(0xff1e1a1a), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(obj, LV_GRAD_DIR_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(obj, lv_color_hex(0xff1e1a1a), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_recolor(obj, lv_color_hex(0xff1e1a1a), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(obj, lv_color_hex(0xff1e1a1a), LV_PART_MAIN | LV_STATE_DEFAULT);
};
void apply_style_homepress(lv_obj_t *obj) {
    apply_style_topbar(obj);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff181829), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(obj, lv_color_hex(0xff1e1a1a), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_recolor(obj, lv_color_hex(0xff1e1a1a), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(obj, lv_color_hex(0xff1e1a1a), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_line_color(obj, lv_color_hex(0xff1e1a1a), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_color(obj, lv_color_hex(0xff1e1a1a), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(obj, lv_color_hex(0xff1e1a1a), LV_PART_MAIN | LV_STATE_DEFAULT);
};

