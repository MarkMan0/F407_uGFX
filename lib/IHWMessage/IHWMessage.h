#pragma once
#include <cstdint>
#include <cstddef>

/// @brief Interface for messaging hardware (UART, CDC ...)
class IHWMessage {
public:
  using receive_cb_t = void (*)(const void*, size_t);

  /// @brief Initialize the hardware interface
  virtual void init() = 0;

  /// @brief transmit data through the interface
  /// @param buff outgoing data
  /// @param sz length of data
  /// @return number of bytes sent
  virtual size_t transmit(const void* buff, size_t sz) = 0;

  /// @brief Interface status
  /// @return true if ready and configured
  virtual bool status() const = 0;

  /// @brief This callback will be called from the receive ISR
  /// @param receive_cb function pointer to callback, it should save the data
  virtual void set_receive_cb(receive_cb_t receive_cb) {
    receive_cb_ = receive_cb;
  }

  /// @brief Deinitialize the hardware interface
  virtual void deinit() = 0;

  virtual ~IHWMessage() = default;

protected:
  receive_cb_t receive_cb_ = nullptr;
};
