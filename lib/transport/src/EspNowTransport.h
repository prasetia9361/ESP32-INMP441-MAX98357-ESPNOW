#pragma once

#include "Transport.h"
#include "spiffsHandler.h"

class OutputBuffer;
class spiffsHandler;

class EspNowTransport: public Transport {
private:
  spiffsHandler* spiffs;
  uint8_t m_wifi_channel;
  uint8_t transmitterMAC[6];
  typedef struct message {
      uint8_t *m_buffer;
      const char *data;
      int dataLen;
  } message;
  message messageData;

 protected:
  void addPeer();
  void send();
  void bindingMode();

public:
  EspNowTransport(OutputBuffer *output_buffer,spiffsHandler *_spiffs, uint8_t wifi_channel);
  virtual bool begin() override;
  friend void receiveCallback(const uint8_t *macAddr, const uint8_t *data, int dataLen);
};
