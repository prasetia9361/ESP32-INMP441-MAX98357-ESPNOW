#pragma once

class Output;
class Transport;
class OutputBuffer;
class spiffsHandler;

class Application {
   private:
    Output *m_output;
    Transport *m_transport;
    spiffsHandler *spiffs;
    OutputBuffer *m_output_buffer;
    
    unsigned long currentTime;
    volatile bool stateBinding = false;

   public:
    Application();
    void begin();
    void loop();
};
