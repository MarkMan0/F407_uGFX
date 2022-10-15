#pragma once
#include "STHAL.h"
#include "ring_buffer.h"
#include "utils.h"


extern "C" void USB_CDC_Receive_callback(uint8_t* buff, size_t len);


class ISerial {
public:
  virtual size_t available() const = 0;

  virtual size_t write(uint8_t) = 0;
  virtual size_t write(const uint8_t* data, size_t len) = 0;
  virtual size_t write(const char* str) = 0;


  virtual size_t read(uint8_t* buff, size_t max_len) = 0;
  virtual size_t read(char* buff, size_t max_len) = 0;



  virtual char c() = 0;
  virtual uint8_t u8() = 0;
  virtual uint16_t u16() = 0;
  virtual uint32_t u32() = 0;
  virtual int8_t i8() = 0;
  virtual int16_t i16() = 0;
  virtual int32_t i32() = 0;
  virtual float flt() = 0;
};


class USB_UART : public ISerial {
public:
  void init();

  size_t available() const override;

  size_t write(uint8_t) override;
  size_t write(const uint8_t* data, size_t len) override;
  size_t write(const char* str) override;

  size_t printf(const char* fmt, ...);


  size_t read(uint8_t* buff, size_t max_len) override;
  size_t read(char* buff, size_t max_len) override;

  template <class T>
  T read() {
    // wait at least one byte
    while (rx_buffer_.is_empty()) {
      vTaskDelay(5);
    }
    // special case for 1 byte
    if constexpr (sizeof(T) == 1) {
      auto val = rx_buffer_.pop();
      return *(reinterpret_cast<T*>(&val));
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

  char c() override {
    return read<char>();
  }
  uint8_t u8() override {
    return read<uint8_t>();
  }
  uint16_t u16() override {
    return read<uint16_t>();
  }
  uint32_t u32() override {
    return read<uint32_t>();
  }
  int8_t i8() override {
    return read<int8_t>();
  }
  int16_t i16() override {
    return read<int16_t>();
  }
  int32_t i32() override {
    return read<int32_t>();
  }
  float flt() override {
    return read<float>();
  }

  void send_task();

  friend void ::USB_CDC_Receive_callback(uint8_t* buff, size_t len);

private:
  RingBuffer<uint8_t, 512, true> rx_buffer_;
  RingBuffer<uint8_t, 512> tx_buffer_;
};