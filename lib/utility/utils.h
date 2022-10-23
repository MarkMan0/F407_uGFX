#pragma once

#include <cstdint>
#include <cstring>

#ifdef TESTING
void test_utils();
#endif

namespace utils {


  template <class T, class U, class V>
  constexpr inline T constrain(T val, U low, V high) {
    if (val <= low) {
      return low;
    } else if (val >= high) {
      return high;
    } else {
      return val;
    }
  }


  template <class T, class K, class L>
  inline constexpr bool within(T val, K low, L high) {
    return (val >= low) && (val <= high);
  }


  template <class T, class K, class L>
  inline constexpr bool between(T val, K low, L high) {
    return (val > low) && (val < high);
  }

  inline constexpr bool elapsed(uint32_t now, uint32_t next) {
    return static_cast<int32_t>(now - next) >= 0;
  }

  template <class T>
  inline constexpr T sign(T val) {
    if (val > 0) return 1;
    if (val < 0) return -1;
    return 0;
  }


  template <class T>
  inline T mem2T(uint8_t* mem) {
    T ret;
    memcpy(&ret, mem, sizeof(T));
    return ret;
  }

  inline constexpr uint32_t crc32mpeg2(const uint8_t* buf, size_t len, uint32_t crc = 0xffffffff) {
    for (size_t i = 0; i < len; ++i) {
      crc ^= buf[i] << 24;
      for (int j = 0; j < 8; ++j) {
        if ((crc & 0x80000000) == 0) {
          crc = crc << 1;
        } else {
          crc = (crc << 1) ^ 0x104c11db7;
        }
      }
    }
    return crc;
  }

}  // namespace utils
