#pragma once
#include "STHAL.h"
#include "ring_buffer.h"
#include "utils.h"


extern "C" void USB_CDC_Receive_callback(uint8_t* buff, size_t len);

class USB_UART {
public:
  void init();
  void send(const uint8_t* data, size_t len);
  void send(const char* str);
  void putc(char c);

  void printf(const char* fmt, ...);

  size_t available() const;

  size_t receive(uint8_t* buff, size_t max_len);
  size_t receive(char* buff, size_t max_len);

  template <class T>
  T receive() {
    // wait at least one byte
    while (rx_buffer_.is_empty()) {
      vTaskDelay(5);
    }
    // special case for 1 byte
    if constexpr (sizeof(T) == 1) {
      return rx_buffer_.pop();
    } else {
      constexpr auto n_bytes = sizeof(T);
      while (rx_buffer_.size() < n_bytes) {
        vTaskDelay(5);
      }
      uint8_t buff[n_bytes] = { 0 };
      for (unsigned i = 0; i < n_bytes; ++i) {
        buff[i] = rx_buffer_.pop();
      }

      return utils::mem2T<T>(buff);
    }
  }

  char getc();

  void send_task();

  friend void ::USB_CDC_Receive_callback(uint8_t* buff, size_t len);

private:
  RingBuffer<uint8_t, 512, true> rx_buffer_;
  RingBuffer<uint8_t, 512> tx_buffer_;
};