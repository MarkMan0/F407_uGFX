#include "comm_api.h"
#include <cstring>
#include "utils.h"
#include <type_traits>
#include "sem_lock.h"

namespace mixer {
  enum commands : uint8_t {
    LOAD_ALL = 0x01,
    READ_IMG = 0x02,
    SET_VOLUME = 0x03,
    ECHO = 0x04,
    SET_MUTE = 0x05,
    RESPONSE_OK = 0xA0,
  };
}

bool CommAPI::verify_read(size_t n) {
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
  utils::Lock lck(mtx_);

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
  if (not verify_read(sizeof(vol.pid_) + sizeof(vol.volume_) + sizeof(uint8_t) + sizeof(uint8_t))) {
    return std::nullopt;
  }
  size_t offset = 0;
  vol.pid_ = mem2T<decltype(vol.pid_ + offset)>(buffer_);
  offset += sizeof(vol.pid_);
  vol.volume_ = mem2T<decltype(vol.volume_)>(buffer_ + offset);
  offset += sizeof(vol.volume_);
  vol.muted_ = mem2T<uint8_t>(buffer_ + offset);
  offset += sizeof(uint8_t);

  uint8_t name_len = mem2T<uint8_t>(buffer_ + offset);

  if (not verify_read(name_len * sizeof(char))) {
    return std::nullopt;
  }

  for (unsigned i = 0; i < name_len && i < vol.NAME_SZ - 1; ++i) {
    vol.name_[i] = static_cast<char>(buffer_[i]);
  }

  return vol;
}

void CommAPI::init(ISerial* u) {
  uart_ = u;
  mtx_ = xSemaphoreCreateMutex();
}

/// @details Loading is done in multiple steps.
///   First, we send the PID with it's CRC.
///   Next, we read the length of the image in bytes from the PC
///   After, we send the max chunk size our buffer can hold ( size of buffer minus 4(CRC))
///   Next, we read the chunks and verify the CRC for each one. If OK, we send a byte, indicating we are ready for next
///     chunk
///   After all chunks are correctly received, we are done
CommAPI::ret_t CommAPI::load_image(int16_t pid, uint8_t* buff, size_t max_sz) {
  utils::Lock lck(mtx_);
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

void CommAPI::set_volume(int16_t pid, uint8_t vol) {
  utils::Lock lck(mtx_);
  uart_->empty_rx();
  uart_->write(mixer::commands::SET_VOLUME);

  static_assert(std::is_same<int16_t, decltype(mixer::ProgramVolume::pid_)>::value);
  static_assert(std::is_same<uint8_t, decltype(mixer::ProgramVolume::volume_)>::value);

  constexpr size_t buff_sz = sizeof(pid) + sizeof(vol) + sizeof(uint32_t);

  uint8_t msg_buff[buff_sz] = { 0 };
  *reinterpret_cast<int16_t*>(msg_buff) = pid;
  *reinterpret_cast<uint8_t*>(msg_buff + 2) = vol;
  *reinterpret_cast<uint32_t*>(msg_buff + 3) = utils::crc32mpeg2(msg_buff, 3);


  uart_->write(msg_buff, buff_sz);
  uart_->flush();
}

void CommAPI::echo(const char* c) {
  utils::Lock lck(mtx_);
  uart_->write(mixer::commands::ECHO);
  uart_->write(c);
}

void CommAPI::set_mute(int16_t pid, bool mute) {
  utils::Lock lck(mtx_);
  constexpr size_t buff_sz = 1 + 2 + 1 + 4;  // cmd, pid, muted, crc
  uint8_t buff[buff_sz];
  buff[0] = mixer::commands::SET_MUTE;
  *reinterpret_cast<int16_t*>(buff + 1) = pid;
  buff[3] = mute;

  *reinterpret_cast<uint32_t*>(buff + 4) = utils::crc32mpeg2(buff + 1, buff_sz - 5);

  uart_->write(buff, buff_sz);
}