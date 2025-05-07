#ifndef EEZ_LVGL_UI_VARS_H
#define EEZ_LVGL_UI_VARS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// enum declarations



// Flow global variables

enum FlowGlobalVariables {
    FLOW_GLOBAL_VARIABLE_VOLUME = 0,
    FLOW_GLOBAL_VARIABLE_MODE = 1,
    FLOW_GLOBAL_VARIABLE_VOLUME_PERSENTAGE = 2,
    FLOW_GLOBAL_VARIABLE_LOAD = 3,
    FLOW_GLOBAL_VARIABLE_SCREEN = 4,
    FLOW_GLOBAL_VARIABLE_ICON_HOME = 5,
    FLOW_GLOBAL_VARIABLE_ICON_GEAR = 6,
    FLOW_GLOBAL_VARIABLE_ICON_PEAKER = 7
};

// Native global variables

extern int32_t get_var_volume();
extern void set_var_volume(int32_t value);
extern int32_t get_var_mode();
extern void set_var_mode(int32_t value);
extern const char *get_var_volume_persentage();
extern void set_var_volume_persentage(const char *value);
extern int32_t get_var_load();
extern void set_var_load(int32_t value);
extern int32_t get_var_screen();
extern void set_var_screen(int32_t value);
extern const char *get_var_icon_home();
extern void set_var_icon_home(const char *value);
extern const char *get_var_icon_gear();
extern void set_var_icon_gear(const char *value);
extern const char *get_var_icon_peaker();
extern void set_var_icon_peaker(const char *value);


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_VARS_H*/