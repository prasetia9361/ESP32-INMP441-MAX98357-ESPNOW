#include "mic.h"
#include "soc/i2s_reg.h"
#include <esp_log.h>
#include "config.h"

i2s_pin_config_t i2s_mic_pins = {
    .bck_io_num = I2S_MIC_SERIAL_CLOCK,
    .ws_io_num = I2S_MIC_LEFT_RIGHT_CLOCK,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_MIC_SERIAL_DATA
};

mic::mic(i2s_port_t _i2sPort, i2s_pin_config_t &_i2sPin, i2s_config_t &_i2sConfig,int size){
    i2sPort = _i2sPort;
    i2s_mic_pins = _i2sPin;
    i2s_mic_Config = _i2sConfig;
    rawSamplesSize = size;
    micReadHead = 0;
    rawSamples = (int32_t *)malloc(sizeof(int32_t)* size);
}

mic::~mic(){
    free(rawSamples);
}

void mic::configMic(uint16_t sampleRate){
i2s_mic_Config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = sampleRate,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_STAND_I2S),
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 4,
    .dma_buf_len = 128,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0};
    i2s_driver_install(i2sPort, &i2s_mic_Config, 0, NULL);
    i2s_set_pin(i2sPort, &i2s_mic_pins);
    i2s_start(i2sPort);
}

void mic::startMic(){
    i2s_zero_dma_buffer(i2sPort);
    i2s_start(i2sPort);
}

int mic::read(int16_t *samples, int count) {
    size_t bytes_read = 0;
    if (count > rawSamplesSize) {
        count = rawSamplesSize;
    }

    i2s_read(i2sPort, rawSamples,count * sizeof(int32_t), &bytes_read, portMAX_DELAY);
    int samplesRead = bytes_read / sizeof(int32_t);
    
    for (int i = 0; i < samplesRead; i++) {
        int16_t temp = (abs(rawSamples[i]) < noiseThreshold) ? rawSamples[i] : rawSamples[i] << 8;
        int16_t temp = rawSamples[i] >> 8;
        int32_t temp = rawSamples[i] >> 13;
        Serial.println(temp);
        samples[i] = constrain((int16_t)temp, -INT16_MAX, INT16_MAX);
        samples[i] = (temp > INT16_MAX) ? INT16_MAX : (temp < -INT16_MAX) ? -INT16_MAX : (int16_t)temp;
    }
    free(rawSamples);
    return samplesRead;
}

void mic::stopAudio(){
    i2s_stop(i2sPort);
    esp_err_t result = i2s_driver_uninstall(i2sPort);
    if (result == ESP_OK) {
        i2s_zero_dma_buffer(i2sPort);
    } else {
        Serial.print("[DEBUG] i2s_driver_uninstall gagal, kode error: ");
        Serial.println(result);
    }
}
