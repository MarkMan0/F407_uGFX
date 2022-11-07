#pragma once
#include <optional>
#include "comm_class.h"
#include "utils.h"
#include <array>
#include "FreeRTOS.h"
#include "semphr.h"

#ifdef TESTING
void mixer_api_test();
#endif


namespace mixer {

  /// @brief Basic info about an audio session
  struct ProgramVolume {
    static inline constexpr size_t NAME_SZ = 30;  ///< Max name length
    ProgramVolume() = default;
    ProgramVolume(int16_t pid, int16_t vol) : pid_(pid), volume_(vol) {
    }
    ProgramVolume(int16_t pid, int16_t vol, const char* name) : pid_(pid), volume_(vol) {
      strncpy(name_, name, NAME_SZ - 1);
    }
    int16_t pid_ = 0;
    char name_[NAME_SZ] = { 0 };
    uint8_t volume_ = 0;
    bool muted_ = false;
  };

  enum MixerError_t {
    OK = 0,
    CRC_ERR,
    BUFF_SZ_ERR,
  };
}  // namespace mixer


class CommAPI {
public:
  static inline constexpr size_t MAX_SUPPORTED_PROGRAMS = 5;  ///< can only render 5 lines
  using volume_t = std::optional<mixer::ProgramVolume>;
  using ret_t = mixer::MixerError_t;

  /// @brief load sessions into internal buffer
  /// @return 0 on success
  ret_t load_volumes();

  /// @brief set volume for session
  /// @param pid PID of the session
  /// @param vol volume 0-100%
  void set_volume(int16_t pid, uint8_t vol);

  /// @brief Load image for session
  /// @param pid PID of session
  /// @param buff destination
  /// @param sz size of @p buff
  /// @return 0 on success
  ret_t load_image(int16_t pid, uint8_t* buff, size_t sz);

  /// @brief Mute/unmute session
  /// @param pid PID of session
  /// @param mute mute request
  void set_mute(int16_t pid, bool mute);

  /// @brief Print to remote console
  /// @param str null terminated array
  void echo(const char* str);

  /// @brief check if sessions have changed since last check
  /// @return 0 if changes, 1 if no changes, 2 on comm failure
  uint8_t changes();

  /// @brief return reference to internal buffer of sessions
  const std::array<volume_t, MAX_SUPPORTED_PROGRAMS>& get_volumes() const {
    return volumes_;
  }

  /// @brief Initialize the singleton, set uart, create mutex
  /// @param uart the UART for communication with PC
  void init(CommClass* uart);

  /// @brief Time since last successful communication in os ticks
  TickType_t since_last_success() const;

  /// @brief retrieve reference to singleton
  /// @return refernece
  static CommAPI& get_instance() {
    static CommAPI api;
    return api;
  }

private:
  CommAPI() = default;
  CommAPI(const CommAPI&) = delete;
  CommAPI& operator=(const CommAPI&) = delete;

  /// @brief Send failure data to port
  /// @return always ret_t::CRC_ERR
  ret_t comm_failure();

  /// @brief Send success data to port
  /// @return always ret_t::OK
  ret_t comm_success();

  /// @brief reads n+4 bytes and checks CRC at the end of buffer
  /// @details uses timeout from UART. Reads into internal buffer
  /// @param n number of bytes to read, without CRC
  /// @return true, if read @p n bytes and CRC is correct
  bool verify_read(size_t n);

  /// @brief Load a session from the UART
  /// @return session info or std::nullopt
  volume_t load_one();
  TickType_t last_successful_comm_ = 0;
  std::array<volume_t, MAX_SUPPORTED_PROGRAMS> volumes_;
  CommClass* uart_;
  static inline constexpr size_t BUFF_SZ = 256;
  uint8_t buffer_[BUFF_SZ];  ///< used for CRC and serial communication
  SemaphoreHandle_t mtx_;
};
