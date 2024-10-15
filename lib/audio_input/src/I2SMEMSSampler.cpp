#include "I2SMEMSSampler.h"

#include "soc/i2s_reg.h"

I2SMEMSSampler::I2SMEMSSampler(
    i2s_port_t i2s_port, 
    i2s_pin_config_t &i2s_pins,
    i2s_config_t i2s_config,
    int raw_samples_size,
    bool fixSPH0645)
{
    m_i2sPort = i2s_port;
    m_i2s_config = i2s_config;
    m_read_head = 0;
    m_i2sPins = i2s_pins;
    m_fixSPH0645 = fixSPH0645;
    m_raw_samples_size = raw_samples_size;
    m_raw_samples = (int16_t *)malloc(sizeof(int16_t) * raw_samples_size);
}

I2SMEMSSampler::~I2SMEMSSampler() { free(m_raw_samples); }

void I2SMEMSSampler::start()
{
    i2s_driver_install(m_i2sPort, &m_i2s_config, 0, NULL);
    i2s_set_pin(m_i2sPort, &m_i2sPins);
}

void I2SMEMSSampler::stop()
{
    unConfigureI2S();
    i2s_driver_uninstall(m_i2sPort);
}

int I2SMEMSSampler::read(int16_t *samples, int count)
{
    size_t bytes_read = 0;
    if (count > m_raw_samples_size)
    {
        count = m_raw_samples_size;
    }
    i2s_read(m_i2sPort, m_raw_samples, count, &bytes_read, portMAX_DELAY);
    int samples_read = bytes_read / sizeof(int16_t);
    // int samples_read = bytes_read / 2;
    for (int i = 0; i < samples_read; i++)
    {
        samples[i] = m_raw_samples[i];
    }
    return samples_read;
}
