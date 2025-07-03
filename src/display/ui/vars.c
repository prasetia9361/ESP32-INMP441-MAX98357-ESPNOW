#include "vars.h"

#include <stdio.h>
#include <string.h>

int32_t vol = 0;
int32_t load_persentage = 0;
char volume[5] = "0";
const char gear[4] = "\xEF\x80\x93";
const char home[4] = "\xEF\x80\x95";
const char speaker[4] = "\xEF\x80\xA8";
const char connected[4] = "\xEF\x87\xAB";
const char *address;
const char *address2;
const char *device1;
const char *device2;
bool msg_box;
bool siren;
char* message_notification;
bool notif_hidden;
int32_t tone = sirine_wail;
void set_var_volume (int32_t value) {
    vol = value;
    snprintf(volume, sizeof(volume), "%d", vol); 
}

int32_t get_var_volume(){
    return vol;
}

const char *get_var_volume_persentage(){
    return volume;
}

int32_t get_var_load(){
    return load_persentage;
}

// extern void set_var_load(int32_t value){
//     load_persentage = value;
// }

void set_var_load(int32_t value){
    load_persentage = value;
}

const char *get_var_icon_home(){
    return home;
}

const char *get_var_connected(){
    return connected;
}

const char *get_var_icon_gear(){
    return gear;
}

const char *get_var_icon_peaker(){
    return speaker;
}

const char *get_var_address(){
    return address;
}

const char *get_var_device1(){
    return device1;
}

const char *get_var_device2(){
    return device2;
}

const char *get_var_address2(){
    return address2;
}

void set_var_address(const char *value){
    address = value;
}

void set_var_address2(const char *value){
    address2 = value;
}

void set_var_device1(const char *value){
    device1 = value;
}

void set_var_device2(const char *value){
    device2 = value;
}

bool get_var_msgbox(){
    return !msg_box;
}

void set_var_msgbox(bool value){
    msg_box = value;
}

bool get_var_siren_mode_1(){
    return !siren;
}

void set_var_siren_mode_1(bool value){
    siren = value;
}

int32_t get_var_siren_tone(){
    return tone;
}

void set_var_siren_tone(int32_t value){
    tone = value;
}

const char *get_var_notification(){
    return message_notification;
}

// void set_var_notification(const char *value){
//     message_notification = value;
// }

bool get_var_notif_hidden(){
    return !notif_hidden;
}

// void set_var_notif_hidden(bool value){
//     notif_hidden = value;
// }


