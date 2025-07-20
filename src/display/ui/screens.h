#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *main;
    lv_obj_t *home;
    lv_obj_t *setting;
    lv_obj_t *load_setting;
    lv_obj_t *setting_siren_page;
    lv_obj_t *back_to_setting_1;
    lv_obj_t *box_message;
    lv_obj_t *box_message__approve;
    lv_obj_t *box_message__button_device1;
    lv_obj_t *box_message__button_piring_1;
    lv_obj_t *box_message__button_piring_2;
    lv_obj_t *box_message__close;
    lv_obj_t *box_message__delete_address;
    lv_obj_t *box_message__obj11;
    lv_obj_t *box_message__obj12;
    lv_obj_t *box_message__obj13;
    lv_obj_t *box_message__obj14;
    lv_obj_t *button_about;
    lv_obj_t *button_piring;
    lv_obj_t *data_from_receiver_1;
    lv_obj_t *mode_10;
    lv_obj_t *mode_11;
    lv_obj_t *mode_12;
    lv_obj_t *mode_7;
    lv_obj_t *mode_8;
    lv_obj_t *mode_9;
    lv_obj_t *mode1_1;
    lv_obj_t *mode2_1;
    lv_obj_t *mode3_1;
    lv_obj_t *mode4_1;
    lv_obj_t *mode5_1;
    lv_obj_t *mode6_1;
    lv_obj_t *mode7_1;
    lv_obj_t *mode8_1;
    lv_obj_t *obj0;
    lv_obj_t *obj1;
    lv_obj_t *obj10;
    lv_obj_t *obj15;
    lv_obj_t *obj16;
    lv_obj_t *obj17;
    lv_obj_t *obj18;
    lv_obj_t *obj19;
    lv_obj_t *obj2;
    lv_obj_t *obj20;
    lv_obj_t *obj21;
    lv_obj_t *obj22;
    lv_obj_t *obj22__approve_1;
    lv_obj_t *obj22__close_1;
    lv_obj_t *obj22__obj23;
    lv_obj_t *obj22__testing;
    lv_obj_t *obj3;
    lv_obj_t *obj4;
    lv_obj_t *obj5;
    lv_obj_t *obj6;
    lv_obj_t *obj7;
    lv_obj_t *obj8;
    lv_obj_t *obj9;
    lv_obj_t *panel_load_1;
    lv_obj_t *perentase_1;
    lv_obj_t *top_bar_home;
    lv_obj_t *top_bar_load_setting;
    lv_obj_t *top_bar_setting;
    lv_obj_t *volume_1;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_MAIN = 1,
    SCREEN_ID_HOME = 2,
    SCREEN_ID_SETTING = 3,
    SCREEN_ID_LOAD_SETTING = 4,
    SCREEN_ID_SETTING_SIREN_PAGE = 5,
    SCREEN_ID_BOX_MESSAGE = 6,
    SCREEN_ID_BOX_MESSAGE_SIRINE = 7,
};

void create_screen_main();
void tick_screen_main();

void create_screen_home();
void tick_screen_home();

void create_screen_setting();
void tick_screen_setting();

void create_screen_load_setting();
void tick_screen_load_setting();

void create_screen_setting_siren_page();
void tick_screen_setting_siren_page();

void create_user_widget_box_message(lv_obj_t *parent_obj, int startWidgetIndex);
void tick_user_widget_box_message(int startWidgetIndex);

void create_user_widget_box_message_sirine(lv_obj_t *parent_obj, int startWidgetIndex);
void tick_user_widget_box_message_sirine(int startWidgetIndex);

void create_screens();
void tick_screen(int screen_index);


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/