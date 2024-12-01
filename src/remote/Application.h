#pragma once

// class I2SMEMSSampler;
class Transport;
class OutputBuffer;
class spiffsHandler;
// class LGFX;

class Application
{
private:
    Transport *m_transport;
    OutputBuffer *m_output_buffer;
    spiffsHandler *spiffs;
    
    unsigned long currentTime;
    

public:
    Application();
    void begin();
    void loop();
    void tick();
};
