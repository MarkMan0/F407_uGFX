#pragma once
#include <optional>
#include "ISerial.h"
#include "utils.h"
#include <array>

#ifdef TESTING
void mixer_api_test();
#endif

using utils::mem2T;

namespace mixer {
  struct ProgramVolume {
    static inline constexpr size_t NAME_SZ = 30;
    ProgramVolume() = default;
    ProgramVolume(int16_t pid, int16_t vol) : pid_(pid), volume_(vol) {
    }
    ProgramVolume(int16_t pid, int16_t vol, const char* name) : pid_(pid), volume_(vol) {
      strncpy(name_, name, NAME_SZ - 1);
    }
    int16_t pid_ = 0;
    char name_[NAME_SZ] = { 0 };
    uint8_t volume_ = 0;
  };

  enum MixerError_t {
    OK = 0,
    CRC_ERR,
    BUFF_SZ_ERR,
  };
}  // namespace mixer


class CommAPI {
public:
  static inline constexpr size_t MAX_SUPPORTED_PROGRAMS = 5;
  using volume_t = std::optional<mixer::ProgramVolume>;
  using ret_t = mixer::MixerError_t;

  ret_t load_volumes();
  void set_volume(const mixer::ProgramVolume&);
  ret_t load_image(int16_t pid, uint8_t* buff, size_t sz);

  void echo(const char*);

  bool changes() {
    /// TODO: implement
    return true;
  }

  const std::array<volume_t, MAX_SUPPORTED_PROGRAMS>& get_volumes() const {
    return volumes_;
  }

  void set_uart(ISerial*);

  static CommAPI& get_instance() {
    static CommAPI api;
    return api;
  }

private:
  CommAPI() = default;
  bool verify_read(size_t n);
  volume_t load_one();
  std::array<volume_t, MAX_SUPPORTED_PROGRAMS> volumes_;
  ISerial* uart_;
  static inline constexpr size_t BUFF_SZ = 256;
  uint8_t buffer_[BUFF_SZ];
};
