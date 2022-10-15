#include "usb_uart.h"
#include "usbd_cdc.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"



void USB_UART::init() {
  MX_USB_DEVICE_Init();
}

size_t USB_UART::write(const uint8_t* data, size_t len) {
  if (tx_buffer_.free() < len) {
    return 0;
  }
  if (auto ptr = tx_buffer_.reserve(len)) {
    memcpy(ptr, data, len);
    tx_buffer_.commit();
  } else {
    for (size_t i = 0; i < len; ++i) {
      tx_buffer_.push(data[i]);
    }
  }
  return len;
}

size_t USB_UART::write(const char* str) {
  return write(reinterpret_cast<const uint8_t*>(str), strlen(str));
}

size_t USB_UART::write(uint8_t c) {
  if (tx_buffer_.is_full()) {
    return 0;
  }
  tx_buffer_.push(c);
  return 1;
}


size_t USB_UART::available() const {
  return rx_buffer_.size();
}

size_t USB_UART::read(uint8_t* buff, size_t max_len) {
  // TODO improve by copying continuous space
  auto occ = rx_buffer_.size();
  size_t i = 0;
  while (occ && i < max_len) {
    buff[i] = rx_buffer_.pop();
    --occ;
    ++i;
  }
  return i;
}

size_t USB_UART::read(char* buff, size_t max_len) {
  return read(reinterpret_cast<uint8_t*>(buff), max_len);
}



void USB_UART::send_task() {
  while (auto n = tx_buffer_.size_cont()) {
    if (USBD_OK == CDC_Transmit_FS(const_cast<uint8_t*>(&tx_buffer_.peek()), n)) {
      tx_buffer_.pop(n);
    }
  }
}