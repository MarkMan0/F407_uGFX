#pragma once
#include <cstdint>
#include <cstddef>

class IHWMessage {
public:
  using receive_cb_t = void (*)(const void*, size_t);
  virtual void init() = 0;

  virtual size_t transmit(const void* buff, size_t sz) = 0;


  virtual void set_receive_cb(receive_cb_t receive_cb) {
    receive_cb_ = receive_cb;
  }

  virtual void deinit() = 0;

  virtual ~IHWMessage();

protected:
  receive_cb_t receive_cb_ = nullptr;
};
