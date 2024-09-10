#pragma once
#include <driver/i2s.h>

class I2SMEMSSampler {
   private:
    i2s_pin_config_t m_i2sPins;
    bool m_fixSPH0645;
    int16_t *m_raw_samples;
    int m_raw_samples_size;
    int m_read_head;

   protected:
    i2s_port_t m_i2sPort = I2S_NUM_0;
    i2s_config_t m_i2s_config;

    void configureI2S();
    void unConfigureI2S() {};

   public:
    I2SMEMSSampler(i2s_port_t i2s_port, i2s_pin_config_t &i2s_pins,
                   i2s_config_t i2s_config, int raw_samples_size,
                   bool fixSPH0645 = false);
    void start();
    void stop();
    ~I2SMEMSSampler();

    int read(int16_t *samples, int count);
};
