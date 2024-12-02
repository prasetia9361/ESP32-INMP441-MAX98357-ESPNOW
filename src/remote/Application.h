#pragma once

class Transport;
class OutputBuffer;
class spiffsHandler;

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
    // void tick();
};
