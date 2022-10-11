#include "usb_uart.h"
#include "usbd_cdc.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"



void USB_UART::init() {
  MX_USB_DEVICE_Init();
}

void USB_UART::send(const uint8_t* data, size_t len) {
  if (auto ptr = tx_buffer_.reserve(len)) {
    memcpy(ptr, data, len);
  } else {
    for (size_t i = 0; i < len; ++i) {
      tx_buffer_.push(data[i]);
    }
  }
}

void USB_UART::send(const char* str) {
  return send(reinterpret_cast<const uint8_t*>(str), strlen(str));
}

void USB_UART::putc(char c) {
  tx_buffer_.push(static_cast<uint8_t>(c));
}

size_t USB_UART::available() const {
  return rx_buffer_.get_num_occupied();
}

size_t USB_UART::receive(uint8_t* buff, size_t max_len) {
  // TODO improve by copying continuous space
  auto occ = rx_buffer_.get_num_occupied();
  size_t i = 0;
  while (occ && i < max_len) {
    buff[i] = rx_buffer_.pop();
    --occ;
    ++i;
  }
  return i;
}

size_t USB_UART::receive(char* buff, size_t max_len) {
  return receive(reinterpret_cast<uint8_t*>(buff), max_len);
}

char USB_UART::getc() {
  if (rx_buffer_.is_empty()) {
    return -1;
  }
  return static_cast<char>(rx_buffer_.pop());
}


void USB_UART::send_task() {
  while (auto n = tx_buffer_.get_num_occupied_continuous()) {
    CDC_Transmit_FS(const_cast<uint8_t*>(&tx_buffer_.peek()), n);
    tx_buffer_.pop(n);
  }
}