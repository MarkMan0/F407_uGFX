#pragma once
#include "STHAL.h"
#include "ring_buffer.h"


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

  char getc();

  void send_task();

  friend void ::USB_CDC_Receive_callback(uint8_t* buff, size_t len);

private:
  RingBuffer<uint8_t, 512, true> rx_buffer_;
  RingBuffer<uint8_t, 512> tx_buffer_;
};