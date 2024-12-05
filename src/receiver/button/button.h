#pragma once

#include "OneButton.h"
// #include "config.h"

class button
{
private:
    bool mode;
    bool removeData;
    int _pin;
    OneButton bindingButton;
    static button* instance;
    static void doubleClick();
    static void longPress();
public:
    button(int pin):_pin(pin),bindingButton(_pin, true){
        mode = false;
        removeData = false;
        instance = this;
    }
    void begin();
    void onDoubleClick();
    void onLongPress();
    bool getMode(){return mode;}
    bool setMode(bool value){
        mode = value;
        return mode;
    }
    bool getRemove(){return removeData;}
    bool setRemove(bool value){
        removeData = value;
        return removeData;
    }
    void tick();
};

button* button::instance = nullptr;

void button::begin(){
    bindingButton.attachDoubleClick(doubleClick);
    bindingButton.attachLongPressStop(longPress);
    pinMode(_pin,INPUT_PULLUP);
}

void button::doubleClick(){
    if (instance)
    {
        instance->onDoubleClick();
    }
}
void button::onDoubleClick(){
    mode = true;
}

void button::longPress(){
    if (instance)
    {
        instance->onLongPress();
    }
}

void button::onLongPress(){
    removeData = true;
}

void button::tick(){
    bindingButton.tick();
}

