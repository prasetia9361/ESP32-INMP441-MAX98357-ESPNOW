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
    lv_obj_t *data_from_receiver;
    lv_obj_t *data_from_receiver_1;
    lv_obj_t *mode_10;
    lv_obj_t *mode_11;
    lv_obj_t *mode_12;
    lv_obj_t *mode_7;
    lv_obj_t *mode_8;
    lv_obj_t *mode_9;
    lv_obj_t *obj0;
    lv_obj_t *obj1;
    lv_obj_t *obj10;
    lv_obj_t *obj11;
    lv_obj_t *obj12;
    lv_obj_t *obj13;
    lv_obj_t *obj14;
    lv_obj_t *obj15;
    lv_obj_t *obj16;
    lv_obj_t *obj17;
    lv_obj_t *obj2;
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
};

void create_screen_main();
void tick_screen_main();

void create_screen_home();
void tick_screen_home();

void create_screen_setting();
void tick_screen_setting();

void create_screen_load_setting();
void tick_screen_load_setting();

void create_screens();
void tick_screen(int screen_index);


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/