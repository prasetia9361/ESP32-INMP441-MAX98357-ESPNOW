#include "vars.h"

#include <stdio.h>
#include <string.h>

int32_t vol = 15;
int32_t load_persentage = 0;
char volume[5] = "15";
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