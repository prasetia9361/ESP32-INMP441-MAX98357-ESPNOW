// #pragma once
#ifndef AUDIO_H
#define AUDIO_H

#include <driver/i2s.h>
// #include <freertos/FreeRTOS.h>

class audio
{
private:
    //speaker
    int16_t *m_frames;
    //mic
    int16_t *m_raw_samples;
    int m_raw_samples_size;
    int m_read_head;

    i2s_pin_config_t m_i2s_pins;
    i2s_port_t m_i2s_port = I2S_NUM_0;
public:
    audio(i2s_port_t i2s_port, i2s_pin_config_t &i2s_pins, int size);
    // sound();
    ~audio();
    // speaker
    void startSpeaker(uint16_t sample_rate);
    void stopAudio();
    int16_t process_sample(int16_t sample){return sample;}
    void write(int16_t *samples, int count);
    //mic
    void startMic(uint16_t sample_rate);
    // void stopMic();
    int read(int16_t *samples, int count);


};
#endif
