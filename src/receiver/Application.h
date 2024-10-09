#pragma once

class Output;
class Transport;
class OutputBuffer;
class spiffs_handler;

class Application {
   private:
    Output *m_output;
    Transport *m_transport;
    spiffs_handler *spiffs;
    OutputBuffer *m_output_buffer;
    
    unsigned long currentTime;
    volatile bool stateBinding = false;

   public:
    Application();
    void begin();
    void loop();
};
