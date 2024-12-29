#include "myButton.h"
// OneButton button1(PIN_1, true);
// OneButton button2(PIN_2, true);
// OneButton button3(PIN_3, true);
// OneButton button4(PIN_4, true);

myButton* myButton::instance = nullptr;

myButton::myButton(Transport *m_transport) { 
    mTransport = m_transport;
    instance = this; 
    }

// void myButton::setup() {}

void myButton::button_1_wrapper() { instance->button_1(); }

void myButton::button_2_wrapper() { instance->button_2(); }

void myButton::button_3_wrapper() { instance->button_3(); }

void myButton::button_4_wrapper() { instance->button_4(); }

void myButton::button_5_wrapper() { instance->button_5(); }

// void myButton::button_1() { _esp.sendMessage(1); }
// void myButton::button_2() { _esp.sendMessage(2); }
// void myButton::button_3() { _esp.sendMessage(3); }
// void myButton::button_4() { _esp.sendMessage(4); }
// void myButton::button_5() { _esp.sendMessage(5); }
void myButton::setup() {
    button1.attachClick(button_1_wrapper);
    button2.attachClick(button_2_wrapper);
    button3.attachClick(button_3_wrapper);
    button4.attachClick(button_4_wrapper);
    button4.attachDoubleClick(button_5_wrapper);
}

void myButton::ticks() {
    button1.tick();
    button2.tick();
    button3.tick();
    button4.tick();
}

void myButton::button_1() { mTransport->sendChar("mode1"); }

void myButton::button_2() { mTransport->sendChar("mode2"); }

void myButton::button_3() { mTransport->sendChar("mode3"); }

void myButton::button_4() { mTransport->sendChar("mode4"); }

void myButton::button_5() { mTransport->sendChar("mode5"); }
