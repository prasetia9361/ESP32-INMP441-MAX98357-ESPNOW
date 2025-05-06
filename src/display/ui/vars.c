#include "vars.h"

#include <stdio.h>
#include <string.h>

int32_t vol = 15;
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