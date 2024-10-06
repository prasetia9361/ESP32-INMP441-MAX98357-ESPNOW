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
    // int buttonState;
    // int pressCount = 0;
    unsigned long currentTime;
    // unsigned long lastPress;
    volatile bool stateBinding = false;

   public:
    Application();
    void begin();
    void loop();
};
