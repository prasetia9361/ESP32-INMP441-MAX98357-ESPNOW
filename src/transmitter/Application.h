#pragma once

class I2SMEMSSampler;
class Transport;
class OutputBuffer;
class spiffs_handler;

class Application
{
private:
    I2SMEMSSampler *m_input;
    Transport *m_transport;
    OutputBuffer *m_output_buffer;
    spiffs_handler *spiffs;
    // int buttonState;
    // int pressCount = 0;
    unsigned long currentTime;
    // unsigned long lastPress;

public:
    Application();
    void begin();
    void loop();
};
