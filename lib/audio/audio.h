// #pragma once
#ifndef AUDIO_H
#define AUDIO_H

#include <Arduino.h>
#include <driver/i2s.h>
#include <freertos/FreeRTOS.h>

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
    int m_mic_read_head;

    // outputBuffer
    uint8_t *m_buffer;
    SemaphoreHandle_t m_semaphore;
    int m_number_samples_to_buffer;
    int m_read_head;
    int m_write_head;
    int m_available_samples;
    int m_buffer_size;
    bool m_buffering;

    //tone controll
    uint8_t vol = 100;       // Volume 0-100%
    uint8_t mode;      
    uint8_t envelope = 100;  // Envelope volume untuk Mode 4
    uint32_t phase_accumulator = 0;

    // Wave table dan phase step
    int16_t wave_table[WAVE_TABLE_SIZE];
    int16_t combine_table[WAVE_TABLE_SIZE];
    int16_t airhorn_table[WAVE_TABLE_SIZE];
    int16_t tone_table[WAVE_TABLE_SIZE];
    volatile uint32_t phase_step = 0;

    i2s_pin_config_t m_i2s_pins;
    i2s_port_t m_i2s_port = I2S_NUM_0;

    const uint8_t modeTableMap(int mode);
    
public:
    audio(i2s_port_t i2s_port, i2s_pin_config_t &i2s_pins, int size);
    ~audio();

    // speaker
    void startSpeaker(uint16_t sample_rate);
    void stopAudio();
    int16_t processSample(int16_t sample){return sample;}
    void write(int16_t *samples, int count);

    // outputBuffer
    void startBuffering(int number_samples_to_buffer);
    void addBuffer(const uint8_t *samples, int count);
    void removeBuffer(int16_t *samples, int count);
    bool getBuffer(){return m_buffering;}

    //mic
    void startMic(uint16_t sample_rate);
    int read(int16_t *samples, int count);

    //Sirine
    void i2sTone(uint8_t _mode);
    void generateSineWave();
};
#endif
