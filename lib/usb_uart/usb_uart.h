#pragma once
#include "ring_buffer.h"
#include "utils.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "ISerial.h"
#include "IHWMessage.h"


/// @brief Define basic timeouts for UART
namespace UART_TimingConfig {
  static constexpr uint32_t WRITE_RETRY_TICKS = pdMS_TO_TICKS(5), READ_RETRY_TICKS = pdMS_TO_TICKS(5),
                            READ_MAX_RETRY = 100, WRITE_MAX_RETRY = 100;
};

/// @brief Implementation of Serial interface over USB CDC
class USB_UART : public ISerial {
public:
  /// @brief get reference to singleton
  static USB_UART& get_instance() {
    static USB_UART self;
    return self;
  }

  /// @brief Initialize the USB CDC interface
  void init();

  size_t available() const override;


  size_t wait_for(size_t n) const override;

  size_t write(uint8_t) override;
  size_t write(const uint8_t* data, size_t len) override;
  size_t write(const char* str) override;

  void flush() override;

  size_t read(uint8_t* buff, size_t max_len) override;
  size_t read(char* buff, size_t max_len) override;

  void empty_rx() override;

  /// @brief Read arbitrary data from the UART
  /// @tparam T type of data to read
  /// @todo This seems really inefficient
  template <class T>
  T read() {
    // wait at least one byte
    uint32_t cnt = UART_TimingConfig::READ_MAX_RETRY;
    while (rx_buffer_.is_empty()) {
      if (--cnt == 0) {
        return -1;
      }
      vTaskDelay(UART_TimingConfig::READ_RETRY_TICKS);
    }
    // special case for 1 byte
    if constexpr (sizeof(T) == 1) {
      auto val = rx_buffer_.pop();
      return *(reinterpret_cast<T*>(&val));
    } else {
      constexpr auto n_bytes = sizeof(T);
      uint32_t cnt = UART_TimingConfig::READ_MAX_RETRY;
      while (rx_buffer_.size() < n_bytes) {
        if (--cnt == 0) {
          return -1;
        }
        vTaskDelay(UART_TimingConfig::READ_RETRY_TICKS);
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

  /// @brief Set transmit task, which will be notified when write() is called
  void set_tx_task(xTaskHandle);

  /// @brief Called from transmit task to empty tx buffer
  void send_task();

  /// @brief Put data from HW interface to internal buffer
  /// @param buff incoming data
  /// @param len length of buff in bytes
  void receive(const void* buff, size_t len);

  /// @brief set the adaptor from the hardware
  /// @param msg pointer to class implementing interface
  void set_hw_msg(IHWMessage* msg) {
    hw_msg_ = msg;
  }

private:
  USB_UART() = default;
  USB_UART(const USB_UART&) = delete;
  USB_UART& operator=(const USB_UART&) = delete;
  void notify_tx_task();
  xTaskHandle tx_task_{};
  xSemaphoreHandle flush_mtx_{};  ///< flush can be called by user and task, need to lock it
  RingBuffer<uint8_t, 512, true> rx_buffer_;
  RingBuffer<uint8_t, 512> tx_buffer_;
  IHWMessage* hw_msg_ = nullptr;
};
