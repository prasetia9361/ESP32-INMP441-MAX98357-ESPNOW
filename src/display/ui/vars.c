#include "vars.h"

#include <stdio.h>
#include <string.h>

int32_t vol = 15;
int32_t load_persentage = 0;
char volume[5] = "15";
const char gear[4] = "\xEF\x80\x93";
const char home[4] = "\xEF\x80\x95";
const char speaker[4] = "\xEF\x80\xA8";
const char *address;
bool msg_box;
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

extern void set_var_load(int32_t value){
    load_persentage = value;
}

const char *get_var_icon_home(){
    return home;
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

bool get_var_msgbox(){
    return !msg_box;
}

void set_var_msgbox(bool value){
    msg_box = value;
}