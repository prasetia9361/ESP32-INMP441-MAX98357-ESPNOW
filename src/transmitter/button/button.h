#pragma once

#include "OneButton.h"
#include <driver/gpio.h>


class button
{
private:
    //banjar (kabel kuning)
    const int pin1 = GPIO_NUM_33;
    const int pin2 = GPIO_NUM_25;
    const int pin3 = GPIO_NUM_26;
    ////baris (kabel biru)
    const int pin4 = GPIO_NUM_15;
    const int pin5 = GPIO_NUM_4;
    const int pin6 = GPIO_NUM_22;
    const int pin7 = GPIO_NUM_23;
    int massage;

    const int rowPins[3];
    const int colPins[4];

    char getKey(int row, int col, const int modeArr[7]);

public:
    button()
        : rowPins{pin1,pin2,pin3}, colPins{pin4,pin5,pin6,pin7} { 
            massage = 0;
    }

    inline void begin();
    inline void checkKey(const int modeArr[7]);
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

// Membuat fungsi getKey dengan parameter tambahan untuk mode 1 sampai 8
// Fungsi getKey yang lebih efektif dengan input array int
inline char button::getKey(int row, int col, const int modeArr[7]){
    int keys[3][4] = {
        {modeArr[0], modeArr[1], modeArr[2], modeArr[3]},
        {modeArr[4], modeArr[5], modeArr[6], modeArr[7]},
        {9, 10, 11, 12}
    };
    return keys[row][col];
}

inline void button::checkKey(const int modeArr[7]){
    for (int col = 0; col < 4; col++)
    {
        digitalWrite(colPins[col], LOW);
        for (int row = 0; row < 3; row++)
        {
          if (digitalRead(rowPins[row]) == LOW)
            {
                massage = getKey(row, col, modeArr);
                // Serial.print("button:");
                // Serial.println(massage);
                // massage = 0;
            }
            
        }
        // massage = 0;
        digitalWrite(colPins[col], HIGH);
    }
}
