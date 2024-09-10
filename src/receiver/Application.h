#pragma once

class Output;
class Transport;
class OutputBuffer;
class Application {
   private:
    Output *m_output;
    Transport *m_transport;
    OutputBuffer *m_output_buffer;

   public:
    Application();
    void begin();
    void loop();
};
