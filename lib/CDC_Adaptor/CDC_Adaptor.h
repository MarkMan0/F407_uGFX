#pragma once


#include "IHWMessage.h"
#include "usbd_cdc.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"

class CDC_Adaptor : public IHWMessage {
public:
  void init() override {
    MX_USB_DEVICE_Init();
  }

  size_t transmit(const void* data, size_t sz) override {
    auto ret = CDC_Transmit_FS(static_cast<uint8_t*>(const_cast<void*>(data)), sz);
    if (ret == USBD_OK) {
      return sz;
    } else {
      return 0;
    }
  }


  void receive(void* buff, size_t sz) {
    if (receive_cb_) {
      receive_cb_(buff, sz);
    }
  }

  void deinit() override {
  }

  static CDC_Adaptor& get_instance() {
    static CDC_Adaptor c;
    return c;
  }

private:
  CDC_Adaptor() = default;
  CDC_Adaptor(const CDC_Adaptor&) = delete;
  CDC_Adaptor& operator=(const CDC_Adaptor&) = delete;
};