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
    
    unsigned long currentTime;

public:
    Application();
    void begin();
    void loop();
};
