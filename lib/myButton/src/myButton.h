#ifndef MYBUTTON_H
#define MYBUTTON_H

#include "EspNowTransport.h"
#include <OneButton.h>
#define PIN_1 33
#define PIN_2 25
#define PIN_3 26
#define PIN_4 27
#define BINDING_BUTTON 0

OneButton button1(PIN_1, true);
OneButton button2(PIN_2, true);
OneButton button3(PIN_3, true);
OneButton button4(PIN_4, true);
OneButton bindingButton(BINDING_BUTTON, true);

class Transport;
class myButton {
   private:
    bool mode;
    bool removeData;
    int _pin;
    void button_1();
    void button_2();
    void button_3();
    void button_4();
    void button_5();
    static void button_1_wrapper();
    static void button_2_wrapper();
    static void button_3_wrapper();
    static void button_4_wrapper();
    static void button_5_wrapper();
    static void doubleClick();
    static void longPress();
    Transport *mTransport;
    static myButton *instance;

   public:
    myButton(Transport *m_transport);
    void setup();
    // void attachs();
    void ticks();
};

#endif  // onebutton_h