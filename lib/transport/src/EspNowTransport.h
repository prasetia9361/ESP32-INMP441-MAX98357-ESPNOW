#pragma once

#include "Transport.h"
#include "spiffsHandler.h"

class OutputBuffer;
class spiffsHandler;

typedef struct message {
    uint8_t *m_buffer = NULL;
    const char *data;
    int dataLen;
} message;

class EspNowTransport: public Transport {
private:
  spiffsHandler* spiffs;
  uint8_t m_wifi_channel;
  uint8_t transmitterMAC[6];
  message messageReceiver;

 protected:
  void addPeer();
  void send();
  void bindingMode();

public:
  EspNowTransport(OutputBuffer *output_buffer,spiffsHandler *_spiffs, uint8_t wifi_channel);
  virtual bool begin() override;
  friend void receiveCallback(const uint8_t *macAddr, const uint8_t *data, int dataLen);
};
