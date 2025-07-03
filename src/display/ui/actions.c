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
bool gSirine = false;
bool loadSetting = false;
bool modal = false;
bool close_modal = false;
bool box = false;
bool saveTone = false;
bool button1 = false;
bool button2 = false;
bool button3 = false;
bool button4 = false;
bool button5 = false;
bool button6 = false;
bool button7 = false;
bool button8 = false;
const char *nameDevice;
int32_t toneSelected[8];



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

extern void action_mode_sirine(lv_event_t * e){
    gSirine = true;
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

void action_siren_mode_1(lv_event_t * e){
    set_var_siren_mode_1(true);
    set_var_siren_tone(toneSelected[0]);
    button1 = true;
    button2 = false;
    button3 = false;
    button4 = false;
    button5 = false;
    button6 = false;
    button7 = false;
    button8 = false;
}

void action_siren_mode_2(lv_event_t * e){
    set_var_siren_mode_1(true);
    set_var_siren_tone(toneSelected[1]);
    button1 = false;
    button2 = true;
    button3 = false;
    button4 = false;
    button5 = false;
    button6 = false;
    button7 = false;
    button8 = false;
}

void action_siren_mode_3(lv_event_t * e){
    set_var_siren_mode_1(true);
    set_var_siren_tone(toneSelected[2]);
    button1 = false;
    button2 = false;
    button3 = true;
    button4 = false;
    button5 = false;
    button6 = false;
    button7 = false;
    button8 = false;
}

void action_siren_mode_4(lv_event_t * e){
    set_var_siren_mode_1(true);
    set_var_siren_tone(toneSelected[3]);
    button1 = false;
    button2 = false;
    button3 = false;
    button4 = true;
    button5 = false;
    button6 = false;
    button7 = false;
    button8 = false;
}

void action_siren_mode_5(lv_event_t * e){
    set_var_siren_mode_1(true);
    set_var_siren_tone(toneSelected[4]);
    button1 = false;
    button2 = false;
    button3 = false;
    button4 = false;
    button5 = true;
    button6 = false;
    button7 = false;
    button8 = false;
}

void action_siren_mode_6(lv_event_t * e){
    set_var_siren_mode_1(true);
    set_var_siren_tone(toneSelected[5]);
    button1 = false;
    button2 = false;
    button3 = false;
    button4 = false;
    button5 = false;
    button6 = true;
    button7 = false;
    button8 = false;
}

void action_siren_mode_7(lv_event_t * e){
    set_var_siren_mode_1(true);
    set_var_siren_tone(toneSelected[6]);
    button1 = false;
    button2 = false;
    button3 = false;
    button4 = false;
    button5 = false;
    button6 = false;
    button7 = true;
    button8 = false;
}

void action_siren_mode_8(lv_event_t * e){
    set_var_siren_mode_1(true);
    set_var_siren_tone(toneSelected[7]);
    button1 = false;
    button2 = false;
    button3 = false;
    button4 = false;
    button5 = false;
    button6 = false;
    button7 = false;
    button8 = true;
}

void action_close_modal_sirine(lv_event_t * e){
    set_var_siren_mode_1(false);
}

void action_save_sirine(lv_event_t * e) {
    // Hanya update elemen toneSelected yang sesuai dengan button yang aktif, yang lain tetap
    if (button1) {
        toneSelected[0] = get_var_siren_tone();
    } else if (button2) {
        toneSelected[1] = get_var_siren_tone();
    } else if (button3) {
        toneSelected[2] = get_var_siren_tone();
    } else if (button4) {
        toneSelected[3] = get_var_siren_tone();
    } else if (button5) {
        toneSelected[4] = get_var_siren_tone();
    } else if (button6) {
        toneSelected[5] = get_var_siren_tone();
    } else if (button7) {
        toneSelected[6] = get_var_siren_tone();
    } else if (button8) {
        toneSelected[7] = get_var_siren_tone();
    }
    saveTone = true;
}


void action_device1(lv_event_t * e){
    
    nameDevice = "address0";
}
void action_device2(lv_event_t * e){
    nameDevice = "address1";
}
