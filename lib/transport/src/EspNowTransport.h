#pragma once

#include "Transport.h"
#include "spiffs_handler.h"

class OutputBuffer;
class spiffs_handler;

class EspNowTransport: public Transport {
private:
  spiffs_handler* spiffs;
  uint8_t m_wifi_channel;
  uint8_t transmitterMAC[6];

protected:
  void addPeer();
  void send();
  void bindingMode();

public:
  EspNowTransport(OutputBuffer *output_buffer,spiffs_handler *_spiffs, uint8_t wifi_channel);
  virtual bool begin() override;
  friend void receiveCallback(const uint8_t *macAddr, const uint8_t *data, int dataLen);
};
