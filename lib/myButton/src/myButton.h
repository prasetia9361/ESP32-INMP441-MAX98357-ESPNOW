#ifndef MYBUTTON_H
#define MYBUTTON_H

#include <Arduino.h>
#include "OneButton.h"

class myButton
{
private:
    /* data */
    void button_0();
    static void button_0_wrapper();
    static myButton *instance;
    bool bindingState;
public:
    myButton(/* args */);

    void setup();
    void attachs();
    void ticks();
    bool getButton(){return bindingState;}

};

#endif