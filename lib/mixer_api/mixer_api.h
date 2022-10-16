#pragma once
#include "STHAL.h"
#include <optional>
#include "usb_uart.h"
#include "utils.h"

#ifdef TESTING
void mixer_api_test();
#endif

using utils::mem2T;

namespace mixer {
  struct ProgramVolume {
    static inline constexpr size_t NAME_SZ = 30;
    ProgramVolume() = default;
    ProgramVolume(int pid, int vol) : pid_(pid), volume_(vol) {
    }
    ProgramVolume(int pid, int vol, const char* name) : pid_(pid), volume_(vol) {
      strncpy(name_, name, NAME_SZ - 1);
    }
    uint32_t pid_ = 0;
    char name_[NAME_SZ] = { 0 };
    uint8_t volume_ = 0;
  };

  enum MixerError_t {
    OK = 0,
    CRC_ERR,
  };
}  // namespace mixer


class MixerAPI {
public:
  static inline constexpr size_t MAX_SUPPORTED_PROGRAMS = 5;
  using volume_t = std::optional<mixer::ProgramVolume>;
  using ret_t = mixer::MixerError_t;

  ret_t load_volumes();
  void set_volume(const mixer::ProgramVolume&);
  void load_bmp(int pid, uint8_t* buff, size_t sz);

  void set_uart(ISerial*);

public:
  bool verify_read(size_t n);
  volume_t load_one();
  volume_t volumes_[MAX_SUPPORTED_PROGRAMS];
  ISerial* uart_;
  static inline constexpr size_t BUFF_SZ = 256;
  uint8_t buffer_[BUFF_SZ];
};
