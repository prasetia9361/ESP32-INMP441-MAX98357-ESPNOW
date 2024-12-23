#pragma once
#include <stdlib.h>
#include <stdint.h>
#include <ArduinoJson.h>

class OutputBuffer;

class Transport
{
protected:
  typedef struct message{
    uint8_t m_buffer[127];
    char data[20] = ""; // Initialize with an empty string
    int dataLen;
  }message;
  message messageData;
  char jsonData[1024];
  String _jsonData;
  uint8_t *bufferValue = NULL;
  int m_buffer_size = 0;
  int m_index = 0;
  int m_header_size;
  bool stateBinding = false;

  OutputBuffer *m_output_buffer = NULL;
  virtual void addPeer() = 0;
  virtual void send() = 0;
  virtual void bindingMode() = 0;

public:
  Transport(OutputBuffer *output_buffer, size_t buffer_size);
  
  int set_header(const int header_size, const uint8_t *header);
  virtual bool begin() = 0;
  bool setBinding(bool bindingState);
  bool getBinding(){return stateBinding;}

  void add_sample(int16_t sample);
  void sendChar(const char *data);
  void flush();
  void statusBinding();
  void peerReady();


};
