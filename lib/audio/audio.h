// #pragma once
#ifndef AUDIO_H
#define AUDIO_H

#include <Arduino.h>
#include <driver/i2s.h>

// #include <freertos/FreeRTOS.h>
// #define I2SDAC_I2S_NUMBER       (0)
// #define I2SDAC_SAMPLE_RATE      (4000)

// #define I2SDAC_DMA_BUF_COUNT    (2)
#define I2SDAC_DMA_BUF_LEN      (64)
// #define I2SDAC_DMA_BUF_TOTAL    (I2SDAC_DMA_BUF_COUNT * I2SDAC_DMA_BUF_LEN)

// #define I2SDAC_SAMPLE_MIN       (-32768)

// #define I2SDAC_TONE_MIN         (200)
// #define I2SDAC_TONE_BUF_LEN     (I2SDAC_SAMPLE_RATE/I2SDAC_TONE_MIN)

// #define I2SDAC_UP_DOWN_STEP     (256)

#define SAMPLE_RATE 44100
#define WAVE_TABLE_SIZE 256
#define N_VALUE 0.4f
#define M_VALUE 0.4f


class audio
{
private:
    //speaker

    int16_t *m_frames;
    //mic
    int16_t *m_raw_samples;
    int m_raw_samples_size;
    int m_read_head;

    float t = 0;
    float freq = 0;
    float periode = 0;
    float quarterWave = 0;
    float sample = 0; 
    int16_t toneSamples[I2SDAC_DMA_BUF_LEN * 2];

    //tone controll
    uint8_t vol = 100;       // Volume 0-100%
    uint8_t mode;      
    uint8_t envelope = 100;  // Envelope volume untuk Mode 4
    uint32_t phase_accumulator = 0;

// Wave table dan phase step
    int16_t wave_table[WAVE_TABLE_SIZE];
    int16_t combine_table[WAVE_TABLE_SIZE];
    int16_t airhorn_table[WAVE_TABLE_SIZE];
    // int16_t sinus_table[WAVE_TABLE_SIZE];
    int16_t tone_table[WAVE_TABLE_SIZE];
    volatile uint32_t phase_step = 0;

    i2s_pin_config_t m_i2s_pins;
    i2s_port_t m_i2s_port = I2S_NUM_0;
    
    // Tambahkan variabel untuk sinwave
    float m_phase = 0.0f;

    const uint8_t modeTableMap(int mode);
    
public:
    audio(i2s_port_t i2s_port, i2s_pin_config_t &i2s_pins, int size);
    // sound();
    ~audio();
    // speaker
    void startSpeaker(uint16_t sample_rate);
    void stopAudio();
    int16_t processSample(int16_t sample){return sample;}
    void write(int16_t *samples, int count);
    //mic
    void startMic(uint16_t sample_rate);
    // void stopMic();
    int read(int16_t *samples, int count);
    void i2sStartTone();
    void i2sStopTone();
    void i2sTone(uint8_t _mode);
    size_t i2sWriteTone (int16_t sample);
    
    // Tambahkan method untuk generate sinwave
    void generateSineWave();
};
#endif
