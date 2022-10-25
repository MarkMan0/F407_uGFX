#pragma once

#include <cstddef>
#include <cstdint>

/// @brief Interface for serial communication in API or mock
class ISerial {
public:
  virtual size_t available() const = 0;

  virtual size_t wait_for(size_t n) const = 0;

  virtual size_t write(uint8_t) = 0;
  virtual size_t write(const uint8_t* data, size_t len) = 0;
  virtual size_t write(const char* str) = 0;

  virtual void flush() = 0;

  virtual size_t read(uint8_t* buff, size_t max_len) = 0;
  virtual size_t read(char* buff, size_t max_len) = 0;

  virtual void empty_rx() = 0;

  virtual char c() = 0;
  virtual uint8_t u8() = 0;
  virtual uint16_t u16() = 0;
  virtual uint32_t u32() = 0;
  virtual int8_t i8() = 0;
  virtual int16_t i16() = 0;
  virtual int32_t i32() = 0;
  virtual float flt() = 0;
};
