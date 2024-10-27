#include "Transport.h"

#include "Arduino.h"

Transport::Transport(OutputBuffer *output_buffer, size_t buffer_size) {
    m_output_buffer = output_buffer;
    m_buffer_size = buffer_size;
    bufferValue = (uint8_t *)malloc(m_buffer_size);
    // messageData.m_buffer = (uint8_t *)malloc(m_buffer_size);
    m_index = 0;
    m_header_size = 0;
    stateBinding = false;
}

void Transport::add_sample(int16_t sample) {
    // bufferValue[m_index + m_header_size] = (sample + 4096) >> 5;
    messageData.m_buffer[m_index + m_header_size] = (sample + 32768) >> 8;
    // bufferValue[m_index + m_header_size] = (sample + 16384) >> 7;
    // Serial.println(messageData.m_buffer[m_index + m_header_size]);
    // messageData.m_buffer = bufferValue;
    // memcpy(messageData.m_buffer, bufferValue, m_index + m_header_size);
    m_index++;
    // messageData.dataLen = m_index + m_header_size;

    if ((m_index + m_header_size) == m_buffer_size) {
        send();
        m_index = 0;
    }
}

void Transport::flush() {
    // messageData.dataLen = m_index;
    if (m_index > 0) {
        send();
        m_index = 0;
    }
}

void Transport::statusBinding() { bindingMode(); }

void Transport::peerReady() { addPeer(); }

int Transport::set_header(const int header_size, const uint8_t *header) {
    if ((header_size < m_buffer_size) && (header)) {
        m_header_size = header_size;
        memcpy(bufferValue, header, header_size);
        // memcpy(messageData.m_buffer, header, header_size);
        return 0;
    } else {
        return -1;
    }
}

bool Transport::setBinding(bool bindingState) {
    stateBinding = bindingState;
    return stateBinding;
}
