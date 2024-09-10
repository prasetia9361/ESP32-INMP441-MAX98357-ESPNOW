#pragma once

class I2SMEMSSampler;
class Transport;
class OutputBuffer;

class Application
{
private:
    I2SMEMSSampler *m_input;
    Transport *m_transport;
    OutputBuffer *m_output_buffer;

public:
    Application();
    void begin();
    void loop();
};
