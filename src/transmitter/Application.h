#pragma once

class I2SMEMSSampler;
class Transport;

class Application {
   private:
    I2SMEMSSampler *m_input;
    Transport *m_transport;
    // IndicatorLed *m_indicator_led;

   public:
    Application();
    void begin();
    void loop();
};
