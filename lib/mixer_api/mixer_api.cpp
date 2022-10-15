#include "mixer_api.h"
#include <cstring>
#include "utils.h"

namespace mixer {
  enum commands : uint8_t {
    LOAD_ALL = 0x01,
  };
}

bool MixerAPI::verify_read(size_t n) {
  // message size is n + sizeof(u32)
  while (uart_->available() < n + 4) {
    vTaskDelay(5);
  }

  unsigned i = 0;

  // read data
  for (i = 0; i < n; ++i) {
    buffer_[i] = uart_->u8();
  }
  auto crc = utils::crc32mpeg2(buffer_, n);
  uint32_t crc_in = uart_->u32();

  return crc == crc_in;
}

MixerAPI::ret_t MixerAPI::load_volumes() {
  while (uart_->available()) {
    uart_->u8();
  }

  uart_->write(mixer::commands::LOAD_ALL);

  if (not verify_read(sizeof(uint8_t))) {
    return ret_t::CRC_ERR;
  }

  uint8_t n_data = buffer_[0];

  for (unsigned i = 0; i < MAX_SUPPORTED_PROGRAMS; ++i) {
    volumes_[i] = std::nullopt;
  }

  for (unsigned i = 0; i < n_data; ++i) {
    volumes_[i] = load_one();
    if (not volumes_[i]) {
      return ret_t::CRC_ERR;
    }
  }
  return ret_t::OK;
}


MixerAPI::volume_t MixerAPI::load_one() {
  mixer::ProgramVolume vol;
  // first part: PID, volume, name_len
  if (not verify_read(sizeof(vol.pid_) + sizeof(vol.volume_) + sizeof(uint8_t))) {
    return std::nullopt;
  }
  vol.pid_ = mem2T<decltype(vol.pid_)>(buffer_);
  vol.volume_ = mem2T<decltype(vol.volume_)>(buffer_ + sizeof(vol.pid_));

  uint8_t name_len = mem2T<uint8_t>(buffer_ + sizeof(vol.pid_) + sizeof(vol.volume_));

  if (not verify_read(name_len * sizeof(char))) {
    return std::nullopt;
  }

  for (unsigned i = 0; i < name_len && i < vol.NAME_SZ - 1; ++i) {
    vol.name_[i] = static_cast<char>(buffer_[i]);
  }

  return vol;
}

void MixerAPI::set_uart(ISerial* u) {
  uart_ = u;
}