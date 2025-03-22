#pragma once

#include "OneButton.h"

class button
{
private:
    int massage;

    const int rowPins[3];
    const int colPins[4];

    char getKey(int row, int col);

public:
    button(int pin1, int pin2, int pin3, int pin4, int pin5, int pin6, int pin7)
        : rowPins{pin1,pin2,pin3}, colPins{pin4,pin5,pin6,pin7} { 
            massage = 0;
    }

    inline void begin();
    inline void checkKey();
    int getButton() { return massage; }
    void setButton() { massage = 0; }
};

inline void button::begin(){
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

inline char button::getKey(int row, int col){
    int keys[3][4] = {
        {1, 2, 3, 4},
        {5, 6, 7, 8},
        {9, 10, 11, 12}
    };
    return keys[row][col];
}

inline void button::checkKey(){
    for (int col = 0; col < 4; col++)
    {
        digitalWrite(colPins[col], LOW);
        for (int row = 0; row < 3; row++)
        {
          if (digitalRead(rowPins[row]) == LOW)
            {
                massage = getKey(row, col);
                // massage = 0;
            }
            
        }
        // massage = 0;
        digitalWrite(colPins[col], HIGH);
    }
}
