#pragma once
#include <cstdint>
#include <cstddef>

class IHWMessage {
public:
  virtual void init() = 0;
  virtual size_t transmit(const void* buff, size_t sz) = 0;
  virtual size_t receive(void* dest, size_t sz) = 0;
  virtual void deinit() = 0;

  virtual ~IHWMessage();
};
