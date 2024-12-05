#pragma once

#include "Transport.h"
#include "memory.h"

class OutputBuffer;
class memory;

typedef struct message {
    uint8_t m_buffer[127];
    char data[12];
    int dataLen;
} message;

class EspNowTransport: public Transport {
private:
  memory* m_memory;
  uint8_t m_wifi_channel;
  uint8_t transmitterMAC[6];
  // message messageData;

 protected:
  void addPeer();
  void send();
  void bindingMode();
  

public:
  EspNowTransport(OutputBuffer *output_buffer,memory *_memory, uint8_t wifi_channel);
  virtual bool begin() override;
  friend void receiveCallback(const uint8_t *macAddr, const uint8_t *data, int dataLen);
};
