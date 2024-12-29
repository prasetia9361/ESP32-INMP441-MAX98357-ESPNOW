#pragma once

#include "OneButton.h"
// #include "config.h"

class button
{
private:
    bool mode;
    bool removeData;
    bool buttonAudio;
    char massage;
    OneButton bindingButton;
    OneButton button1;
    OneButton button2;
    OneButton button3;
    OneButton button4;
    static button* instance;
    const int rowPins[3];
    const int colPins[4];

    char getKey(int row, int col);
    // void checkKey();

    // void button_1();
    // void button_2();
    // void button_3();
    // void button_4();
    // void button_5();
    void onDelete();
    void onBinding();
    // static void button_1_wrapper();
    // static void button_2_wrapper();
    // static void button_3_wrapper();
    // static void button_4_wrapper();
    // static void button_5_wrapper();
    // static void doubleClick();
    // static void longPress();

public:
    button(int pin1, int pin2, int pin3, int pin4, int pin5, int pin6, int pin7)
        : rowPins{pin1,pin2,pin3}, colPins{pin4,pin5,pin6,pin7} { 
            massage = '\0';
            mode = false;
            removeData = false;
            buttonAudio = false;
            instance = this;
    }

    void begin();
    void checkKey();
    const char getButton() { return massage; }
    void setButton() { massage = '\0'; }

    bool getAudio() {return buttonAudio;}

    bool getMode() { return mode; }
    bool getRemove() { return removeData; }
    bool setMode(bool value) {
        mode = value;
        return mode;
    }
    bool setRemove(bool value) {
        removeData = value;
        return removeData;
    }
    // void ticks();
};

button* button::instance = nullptr;

void button::begin(){
    for (int i = 0; i < 3; i++)
    {
        pinMode(rowPins[i], INPUT_PULLUP);
    }
    
    for (int i = 0; i < 4; i++)
    {
        pinMode(colPins[i], OUTPUT);
        digitalWrite(colPins[i], HIGH);
    }
}

char button::getKey(int row, int col){
    char keys[3][4] = {
        {'1', '2', '3', '4'},
        {'5', '6', '7', '8'},
        {'A', 'B', 'C', 'D'}
    };
    return keys[row][col];
}

void button::checkKey(){
    for (int col = 0; col < 4; col++)
    {
        digitalWrite(colPins[col], LOW);
        for (int row = 0; row < 3; row++)
        {

          if (digitalRead(rowPins[row]) == LOW)
                {
                    massage = getKey(row, col);
                    // Serial.println(massage);
            }

            // if (row == 2 && col == 2) {
            //     if (digitalRead(rowPins[row]) == LOW) {
            //         buttonAudio = true; 
            //         // Serial.println(buttonAudio);
            //     } else {
            //         buttonAudio = false; 
            //         // Serial.println(buttonAudio);
            //     }
                
            // }else {
            //     if (digitalRead(rowPins[row]) == LOW)
            //     {
            //         massage = getKey(row, col);
            //         // Serial.println(massage);
            //     }
                
            //     // Serial.print(row);
            //     // Serial.print(",");
            //     // Serial.println(col);
            // }
        }
        digitalWrite(colPins[col], HIGH);
    }
    
}

void button::onBinding(){
    if (massage = 'C')
    {
        mode = true;
    }
}

void button::onDelete(){
    if (massage == 'B')
    {
        removeData = true;
    }
}