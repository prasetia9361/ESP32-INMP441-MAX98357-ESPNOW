#ifndef EEZ_LVGL_UI_EVENTS_H
#define EEZ_LVGL_UI_EVENTS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void action_mode_binding(lv_event_t * e);
extern void action_sending_data_espnow(lv_event_t * e);
extern void action_delete_mac_address(lv_event_t * e);
extern void action_main_page(lv_event_t * e);
extern void action_setting_page(lv_event_t * e);
extern void action_load_setting(lv_event_t * e);
extern void action_piring(lv_event_t * e);
extern void action_close_modal(lv_event_t * e);


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_EVENTS_H*/