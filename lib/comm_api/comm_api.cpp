#include "comm_api.h"
#include <cstring>
#include "utils.h"
#include <type_traits>

namespace mixer {
  enum commands : uint8_t {
    LOAD_ALL = 0x01,
    READ_IMG = 0x02,
    SET_VOLUME = 0x03,
    RESPONSE_OK = 0xA0,
  };
}

bool CommAPI::verify_read(size_t n) {
  // message size is n + sizeof(u32)

  if (0 == uart_->wait_for(n + 4)) {
    return false;
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

CommAPI::ret_t CommAPI::load_volumes() {
  uart_->empty_rx();

  uart_->write(mixer::commands::LOAD_ALL);
  uart_->flush();

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


CommAPI::volume_t CommAPI::load_one() {
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

void CommAPI::set_uart(ISerial* u) {
  uart_ = u;
}


CommAPI::ret_t CommAPI::load_image(int16_t pid, uint8_t* buff, size_t max_sz) {
  uart_->empty_rx();
  uart_->write(mixer::commands::READ_IMG);

  uint8_t msg_buff[8] = { 0 };
  static_assert(std::is_same<decltype(pid), int16_t>::value);

  *reinterpret_cast<int16_t*>(msg_buff) = pid;
  *reinterpret_cast<uint32_t*>(msg_buff + 2) = utils::crc32mpeg2(msg_buff, 2);
  uart_->write(msg_buff, 6);
  uart_->flush();

  if (not verify_read(sizeof(uint32_t))) {
    return ret_t::CRC_ERR;
  }

  const uint32_t msg_len = utils::mem2T<uint32_t>(buffer_);

  if (max_sz < msg_len) {
    return ret_t::BUFF_SZ_ERR;
  }

  constexpr uint32_t chunk_size = BUFF_SZ - sizeof(uint32_t);
  *reinterpret_cast<uint32_t*>(msg_buff) = chunk_size;
  *reinterpret_cast<uint32_t*>(msg_buff + 4) = utils::crc32mpeg2(msg_buff, 4);

  uart_->write(msg_buff, 8);
  uart_->flush();


  for (uint32_t read_bytes = 0; read_bytes < msg_len;) {
    uint32_t bytes_to_read = std::min(chunk_size, msg_len - read_bytes);
    if (not verify_read(bytes_to_read)) {
      return ret_t::CRC_ERR;
    }

    memcpy(buff + read_bytes, buffer_, bytes_to_read);
    read_bytes += bytes_to_read;
    uart_->write(mixer::commands::RESPONSE_OK);
    uart_->flush();
  }

  return ret_t::OK;
}

void CommAPI::set_volume(const mixer::ProgramVolume& vol) {
  uart_->empty_rx();
  uart_->write(mixer::commands::SET_VOLUME);

  static_assert(std::is_same<int16_t, decltype(vol.pid_)>::value);
  static_assert(std::is_same<uint8_t, decltype(vol.volume_)>::value);

  constexpr size_t buff_sz = sizeof(vol.pid_) + sizeof(vol.volume_) + sizeof(uint32_t);

  uint8_t msg_buff[buff_sz] = { 0 };
  *reinterpret_cast<int16_t*>(msg_buff) = vol.pid_;
  *reinterpret_cast<uint8_t*>(msg_buff + 2) = vol.volume_;
  *reinterpret_cast<uint32_t*>(msg_buff + 3) = utils::crc32mpeg2(msg_buff, 3);


  uart_->write(msg_buff, buff_sz);
  uart_->flush();
}