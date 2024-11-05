#pragma once

// class I2SMEMSSampler;
class Transport;
class OutputBuffer;
class spiffsHandler;

class Application
{
private:
    // I2SMEMSSampler *m_input;
    Transport *m_transport;
    OutputBuffer *m_output_buffer;
    spiffsHandler *spiffs;
    
    unsigned long currentTime;
    

public:
    Application();
    void begin();
    void loop();
};
