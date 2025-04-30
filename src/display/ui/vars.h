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
    FLOW_GLOBAL_VARIABLE_MODE = 1
};

// Native global variables

extern int32_t get_var_volume();
extern void set_var_volume(int32_t value);
extern int32_t get_var_mode();
extern void set_var_mode(int32_t value);


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_VARS_H*/