#pragma once

#include <driver/i2s.h>
#include <freertos/FreeRTOS.h>

class Output {
   private:
    int16_t *m_frames;

   protected:
    i2s_port_t m_i2s_port = I2S_NUM_0;

   public:
    Output(i2s_port_t i2s_port);
    ~Output();
    virtual void start(uint16_t sample_rate) = 0;
    void stop();
    virtual int16_t process_sample(int16_t sample) { return sample; }
    void write(int16_t *samples, int count);
};
