#include "vars.h"
int32_t vol;
void set_var_volume (int32_t value) {
    vol = value;
}

int32_t get_var_volume(){
    return vol;
}