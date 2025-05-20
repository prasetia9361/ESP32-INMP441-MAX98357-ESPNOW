#include <stdbool.h>
#include "actions.h"
#include "vars.h"

lv_event_t g_event_sending = {};
lv_event_t g_event_binding = {};
lv_event_t g_event_delete = {};
bool gBinding = false;
bool gSending = false;
bool gDelete = false;
bool gMain = false;
bool gSetting = false;
bool loadSetting = false;
bool modal = false;
bool close_modal = false;
bool box = false;

void action_mode_binding(lv_event_t *e)
{
    g_event_binding = *e;
    gBinding = true;
}

void action_sending_data_espnow(lv_event_t *e)
{
    g_event_sending = *e;
    gSending = true;
}

void action_delete_mac_address(lv_event_t * e)
{
    g_event_delete = *e;
    gDelete = true;
}

extern void action_main_page(lv_event_t * e){
    gMain = true;
}
extern void action_setting_page(lv_event_t * e){
    gSetting = true;
}

void action_load_setting(lv_event_t * e){
    loadSetting = true;
}

void action_piring(lv_event_t * e){
    // modal = true;
    box = true;
    set_var_msgbox(box);
}

void action_close_modal(lv_event_t * e){
    // close_modal = true;
    box = false;
    set_var_msgbox(box);
    // msg_box = false;
}