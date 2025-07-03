#ifndef EEZ_LVGL_UI_VARS_H
#define EEZ_LVGL_UI_VARS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// enum declarations

typedef enum {
    sirine_wail = 0,
    sirine_yield = 1,
    sirine_piercer = 2
} sirine;

// Flow global variables

enum FlowGlobalVariables {
    FLOW_GLOBAL_VARIABLE_ADDRESS = 0,
    FLOW_GLOBAL_VARIABLE_ICON_GEAR = 1,
    FLOW_GLOBAL_VARIABLE_ICON_HOME = 2,
    FLOW_GLOBAL_VARIABLE_ICON_PEAKER = 3,
    FLOW_GLOBAL_VARIABLE_LOAD = 4,
    FLOW_GLOBAL_VARIABLE_MODE = 5,
    FLOW_GLOBAL_VARIABLE_MSG_BOX_SIRINE = 6,
    FLOW_GLOBAL_VARIABLE_MSGBOX = 7,
    FLOW_GLOBAL_VARIABLE_SCREEN = 8,
    FLOW_GLOBAL_VARIABLE_SIREN_MODE_1 = 9,
    FLOW_GLOBAL_VARIABLE_SIREN_TONE = 10,
    FLOW_GLOBAL_VARIABLE_VOLUME = 11,
    FLOW_GLOBAL_VARIABLE_VOLUME_PERSENTAGE = 12,
    FLOW_GLOBAL_VARIABLE_NOTIFICATION = 13,
    FLOW_GLOBAL_VARIABLE_NOTIF_HIDDEN = 14,
    FLOW_GLOBAL_VARIABLE_ADDRESS2 = 15,
    FLOW_GLOBAL_VARIABLE_DEVICE1 = 16,
    FLOW_GLOBAL_VARIABLE_DEVICE2 = 17,
    FLOW_GLOBAL_VARIABLE_CONNECTED = 18,
    FLOW_GLOBAL_VARIABLE_STATUS_CONNECTED = 19
};

// Native global variables

extern const char *get_var_address();
extern void set_var_address(const char *value);
extern const char *get_var_icon_gear();
extern void set_var_icon_gear(const char *value);
extern const char *get_var_icon_home();
extern void set_var_icon_home(const char *value);
extern const char *get_var_icon_peaker();
extern void set_var_icon_peaker(const char *value);
extern int32_t get_var_load();
extern void set_var_load(int32_t value);
extern int32_t get_var_mode();
extern void set_var_mode(int32_t value);
extern bool get_var_msg_box_sirine();
extern void set_var_msg_box_sirine(bool value);
extern bool get_var_msgbox();
extern void set_var_msgbox(bool value);
extern int32_t get_var_screen();
extern void set_var_screen(int32_t value);
extern bool get_var_siren_mode_1();
extern void set_var_siren_mode_1(bool value);
extern int32_t get_var_siren_tone();
extern void set_var_siren_tone(int32_t value);
extern int32_t get_var_volume();
extern void set_var_volume(int32_t value);
extern const char *get_var_volume_persentage();
extern void set_var_volume_persentage(const char *value);
extern const char *get_var_notification();
extern void set_var_notification(const char *value);
extern bool get_var_notif_hidden();
extern void set_var_notif_hidden(bool value);
extern const char *get_var_address2();
extern void set_var_address2(const char *value);
extern const char *get_var_device1();
extern void set_var_device1(const char *value);
extern const char *get_var_device2();
extern void set_var_device2(const char *value);
extern const char *get_var_connected();
extern void set_var_connected(const char *value);
extern bool get_var_status_connected();
extern void set_var_status_connected(bool value);


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_VARS_H*/