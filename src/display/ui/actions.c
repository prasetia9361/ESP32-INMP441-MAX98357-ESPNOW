#include <stdbool.h>
#include "actions.h"

lv_event_t g_event_sending = {};
lv_event_t g_event_binding = {};
lv_event_t g_event_delete = {};
bool gBinding = false;
bool gSending = false;
bool gDelete = false;
bool gMain = false;
bool gSetting = false;

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
