#include "myButton.h"

#define PIN_0 0
OneButton button0(PIN_0,true);

myButton* myButton::instance = nullptr;

myButton::myButton(){}

void myButton::setup(){pinMode(PIN_0,INPUT_PULLUP);}

void myButton::button_0_wrapper(){instance->button_0();}

void myButton::attachs(){
    button0.attachDoubleClick(button_0_wrapper);
}

void myButton::ticks(){
    button0.tick();
}

void myButton::button_0(){bindingState = true;}