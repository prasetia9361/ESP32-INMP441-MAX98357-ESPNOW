#ifndef EEZ_LVGL_UI_EVENTS_H
#define EEZ_LVGL_UI_EVENTS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void action_mode_binding(lv_event_t * e);
extern void action_siren_mode_1(lv_event_t * e);
extern void action_save_sirine(lv_event_t * e);
extern void action_close_modal_sirine(lv_event_t * e);
extern void action_mode_sirine(lv_event_t * e);
extern void action_piring(lv_event_t * e);
extern void action_close_modal(lv_event_t * e);
extern void action_load_setting(lv_event_t * e);
extern void action_setting_page(lv_event_t * e);
extern void action_main_page(lv_event_t * e);
extern void action_delete_mac_address(lv_event_t * e);
extern void action_sending_data_espnow(lv_event_t * e);
extern void action_siren_mode_2(lv_event_t * e);
extern void action_siren_mode_3(lv_event_t * e);
extern void action_siren_mode_4(lv_event_t * e);
extern void action_siren_mode_5(lv_event_t * e);
extern void action_siren_mode_6(lv_event_t * e);
extern void action_siren_mode_7(lv_event_t * e);
extern void action_siren_mode_8(lv_event_t * e);
extern void action_select_button_1(lv_event_t * e);
extern void action_device1(lv_event_t * e);
extern void action_device2(lv_event_t * e);
extern void action_test_sirine(lv_event_t * e);


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_EVENTS_H*/