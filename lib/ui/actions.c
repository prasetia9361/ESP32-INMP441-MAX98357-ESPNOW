#include <stdbool.h>
#include "actions.h"

lv_event_t g_event_sending = {};
lv_event_t g_event_binding = {};
lv_event_t g_event_delete = {};
bool g_binding = false;
bool g_sending = false;
bool g_delete = false;

void action_mode_binding(lv_event_t *e)
{
    g_event_binding = *e;
    g_binding = true;
}

void action_sending_data_espnow(lv_event_t *e)
{
    g_event_sending = *e;
    g_sending = true;
}

void action_delete_mac_address(lv_event_t * e)
{
    g_event_delete = *e;
    g_delete = true;
}