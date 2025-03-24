// #pragma once
#ifndef AUDIO_H
#define AUDIO_H

#include <Arduino.h>
#include <driver/i2s.h>
#include <freertos/FreeRTOS.h>
#include <driver/gpio.h>

#define SAMPLE_RATE 44100
#define WAVE_TABLE_SIZE 256
#define N_VALUE 0.4f
#define M_VALUE 0.4f

// I2S Microphone Settings
#define I2S_MIC_CHANNEL I2S_CHANNEL_FMT_ONLY_LEFT
#define I2S_MIC_SERIAL_CLOCK 18
#define I2S_MIC_LEFT_RIGHT_CLOCK 19
#define I2S_MIC_SERIAL_DATA 21

// speaker settings
#define USE_I2S_SPEAKER_OUTPUT
#define I2S_SPEAKER_SERIAL_CLOCK 26
#define I2S_SPEAKER_LEFT_RIGHT_CLOCK 25
#define I2S_SPEAKER_SERIAL_DATA 22
// Shutdown line if you have this wired up or -1 if you don't
#define I2S_SPEAKER_SD_PIN 5


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

    i2s_pin_config_t i2s_mic_pins = {
        .bck_io_num = I2S_MIC_SERIAL_CLOCK,
        .ws_io_num = I2S_MIC_LEFT_RIGHT_CLOCK,
        .data_out_num = I2S_PIN_NO_CHANGE,
        .data_in_num = I2S_MIC_SERIAL_DATA
    };

    i2s_pin_config_t i2s_speaker_pins = {
        .bck_io_num = I2S_SPEAKER_SERIAL_CLOCK,
        .ws_io_num = I2S_SPEAKER_LEFT_RIGHT_CLOCK,
        .data_out_num = I2S_SPEAKER_SERIAL_DATA,
        .data_in_num = I2S_PIN_NO_CHANGE
    };

    i2s_port_t m_i2s_port = I2S_NUM_0;

    const uint8_t modeTableMap(int mode);
    
public:
    // audio(i2s_port_t i2s_port, i2s_pin_config_t &i2s_pins, int size);
    audio(int size);
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
