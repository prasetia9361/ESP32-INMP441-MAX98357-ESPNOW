#include "audio.h"
#include "soc/i2s_reg.h"
#include <esp_log.h>

static const char *TAG = "OUT";

// // number of frames to try and send at once (a frame is a left and right sample)
// // const int NUM_FRAMES_TO_SEND = 256;
// audio::audio(){
audio::audio(i2s_port_t i2s_port, i2s_pin_config_t &i2s_pins, int size){
    m_i2s_port = i2s_port;
    m_i2s_pins = i2s_pins;
    m_raw_samples_size = size;
    m_read_head = 0;
    //spekaer size = 256
    m_frames = (int16_t *)malloc(2 * sizeof(int16_t) * m_raw_samples_size);

    //mic size = 128
    m_raw_samples = (int16_t *)malloc(sizeof(int16_t) * m_raw_samples_size);


}

// audio::~audio(){
//     free(m_frames);
//     free(m_raw_samples);
// }

void audio::startSpeaker(uint16_t sample_rate){
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 4, 1)
        .sample_rate = sample_rate,
#else
        .sample_rate = (int)sample_rate,
#endif
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 2, 0)
        .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_STAND_I2S),
#else
        .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S),
#endif
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 4,
        .dma_buf_len = 64,
        .use_apll = false,
        .tx_desc_auto_clear = true,
        .fixed_mclk = 0,
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 4, 1)
        .mclk_multiple = I2S_MCLK_MULTIPLE_DEFAULT,  // Unused
        .bits_per_chan = I2S_BITS_PER_CHAN_DEFAULT   // Use bits per sample
#endif
    };
    i2s_driver_install(m_i2s_port, &i2s_config, 0, NULL);
    i2s_set_pin(m_i2s_port, &m_i2s_pins);
    i2s_zero_dma_buffer(m_i2s_port);

    i2s_start(m_i2s_port);
}

void audio::stopAudio(){
    i2s_stop(m_i2s_port);
    i2s_driver_uninstall(m_i2s_port);
}

void audio::write(int16_t *samples, int count){
    int sample_index = 0;
    while (sample_index < count)
    {
        int samples_to_send = 0;
        for (int i = 0; i < m_raw_samples_size && sample_index < count; i++)
        {
            int sample = process_sample(samples[sample_index]);
            // Serial.println(samples[sample_index]);
            m_frames[i * 2] = sample;     // left channel
            m_frames[i * 2 + 1] = sample; // right channel

            samples_to_send++;
            sample_index++;
        }
        size_t bytes_written = 0;
        i2s_write(m_i2s_port, m_frames, samples_to_send * sizeof(int16_t) * 2, &bytes_written, portMAX_DELAY);
        if (bytes_written != samples_to_send * sizeof(int16_t) * 2)
        {
            ESP_LOGE(TAG, "Did not write all bytes");
        }
    }
}

void audio::startMic(uint16_t sample_rate){
    // i2s config for reading from I2S
i2s_config_t i2s_mic_Config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = sample_rate,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 2, 0)
    .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_STAND_I2S),
#else
    .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S),
#endif
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 64,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0};

    i2s_driver_install(m_i2s_port, &i2s_mic_Config, 0, NULL);
    i2s_set_pin(m_i2s_port, &m_i2s_pins);
    i2s_zero_dma_buffer(m_i2s_port);

    i2s_start(m_i2s_port);
}

int audio::read(int16_t *samples, int count){
    size_t bytes_read = 0;
    if (count > m_raw_samples_size)
    {
        count = m_raw_samples_size;
    }
    i2s_read(m_i2s_port, m_raw_samples, count, &bytes_read, portMAX_DELAY);
    int samples_read = bytes_read / sizeof(int16_t);
    // int samples_read = bytes_read / 2;
    for (int i = 0; i < samples_read; i++)
    {
        samples[i] = m_raw_samples[i];
    }
    return samples_read;
}
