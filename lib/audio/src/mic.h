#pragma once

#include <Arduino.h>
#include <driver/i2s.h>

// I2S Microphone Settings
#define I2S_MIC_CHANNEL I2S_CHANNEL_FMT_ONLY_LEFT
#define I2S_MIC_SERIAL_CLOCK 18
#define I2S_MIC_LEFT_RIGHT_CLOCK 19
#define I2S_MIC_SERIAL_DATA 21

class mic
{
private:
    //mic
    int16_t *rawSamples;
    int rawSamplesSize;
    int micReadHead;

    i2s_pin_config_t i2s_mic_pins = {
        .bck_io_num = I2S_MIC_SERIAL_CLOCK,
        .ws_io_num = I2S_MIC_LEFT_RIGHT_CLOCK,
        .data_out_num = I2S_PIN_NO_CHANGE,
        .data_in_num = I2S_MIC_SERIAL_DATA
    };

    i2s_port_t i2sPort = I2S_NUM_0;

    
public:
    mic(int size);
    ~mic();
    //mic
    void startMic(uint16_t sampleRate);
    int read(int16_t *samples, int count);
    void stopAudio();
};