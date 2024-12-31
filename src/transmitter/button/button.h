#pragma once

#include "OneButton.h"

class button
{
private:
    bool mode;
    bool removeData;
    bool buttonAudio;
    char massage;

    static button* instance;
    const int rowPins[3];
    const int colPins[4];

    char getKey(int row, int col);

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
    char getButton() { return massage; }
    void setButton() { massage = '\0'; }

    bool getAudio() {return buttonAudio;}
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
        }
        digitalWrite(colPins[col], HIGH);
    }
    
}
