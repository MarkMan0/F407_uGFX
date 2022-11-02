#include "CDC_Adaptor.h"



extern "C" void USB_CDC_Receive_callback(uint8_t* buff, size_t size) {
  CDC_Adaptor::get_instance().receive(buff, size);
}
