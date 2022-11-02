/**
 * @file ring_buffer.h
 * @brief Simple ring-buffer implementation
 */

#pragma once
#include "passert.h"
#include <cstdint>
#include <array>

#ifdef TESTING
void ring_buffer_tests();
#endif

/**
 * @brief Non-overwriting ring buffer implementation
 * @details Can be read and written in the same time, but doesn't support multiple writes
 * @tparam T type in buffer
 * @tparam N number of elements in buffer
 */
template <class T = uint8_t, uint16_t N = 64, bool ADD_VOLATILE = false>
class RingBuffer {
private:
  using index_t = typename std::conditional<ADD_VOLATILE, volatile uint16_t, uint16_t>::type;

public:
  using data_t = T;
  std::array<T, N> buff_;
  index_t read_head_{},  ///< contains valid data, which can be read
      write_head_{},     ///< non-reserved space, where we can write new data, but won't be read
      tail_{};           ///< where we read, as expected
  bool is_full_ = false;

  /// @brief readhead to writehead, making data available for reading
  void commit() {
    read_head_ = write_head_;
  }

  /// Place and element into the buffer
  uint16_t push(const data_t& d) {
    if (is_full()) {
      return 0;
    }
    buff_[write_head_] = d;
    write_head_ = (write_head_ + 1) % N;
    is_full_ = is_full_ || (write_head_ == tail_);
    commit();
    return 1;
  }

  /// Place @p n elements into the buffer
  /// @return Number of elements written
  uint16_t push(const T* data, uint16_t n) {
    /// TODO: optimize by copying in larger chunks to continuous space
    int cnt = 0;
    for (; cnt < n && not is_full(); ++cnt) {
      buff_[write_head_] = data[cnt];
      write_head_ = (write_head_ + 1) % N;
      is_full_ = is_full_ || (write_head_ == tail_);
    }
    commit();
    return cnt;
  }

  /// Take one element from the buffer
  [[nodiscard]] data_t pop() {
    passert(!is_empty());

    data_t ret = buff_[tail_];
    tail_ = (tail_ + 1) % N;
    is_full_ = false;
    return ret;
  }

  /// Move tail_ pointer by @p n. Essentially deletes the elements
  void pop(size_t n) {
    passert(size() >= n);
    tail_ = (tail_ + n) % N;
    is_full_ = false;
  }

  void pop(T* dest, size_t n) {
    /// TODO: optimize
    passert(size() >= n);
    for (size_t i = 0; i < n; ++i) {
      dest[i] = pop();
    }
  }

  /// First element in the buffer
  [[nodiscard]] const data_t& peek() const {
    passert(!is_empty());
    return buff_[tail_];
  }

  [[nodiscard]] bool is_full() const {
    return is_full_;
  }
  [[nodiscard]] bool is_empty() const {
    return ((not is_full_) && (read_head_ == tail_));
  }

  [[nodiscard]] uint16_t size() const {
    if (is_full()) {
      return N;
    }

    if (read_head_ >= tail_) {
      return (read_head_ - tail_);
    } else {
      return (N + read_head_ - tail_);
    }
  }

  /// size of continuously occupied space after tail_
  [[nodiscard]] uint16_t size_cont() const {
    if (read_head_ < tail_) {
      return N - tail_;
    } else {
      return size();
    }
  }

  [[nodiscard]] uint16_t free() const {
    if (read_head_ >= write_head_) {
      return N - size() - (read_head_ - write_head_);
    } else {
      return N - size() - (write_head_ - read_head_);
    }
  }

  /// size of continuous free space after head_
  [[nodiscard]] uint16_t free_cont() const {
    if (write_head_ < tail_) {
      return tail_ - write_head_;
    } else {
      return N - write_head_;
    }
  }

  /// @brief Moves head by @p n, if @p n number of continuous space is available
  /// <b>Must call commit after</b>
  /// @returns pointer to beginning of the reserved space
  [[nodiscard]] data_t* reserve(uint16_t n) {
    if (free_cont() < n) {
      return nullptr;
    }

    data_t* ret = &(buff_[write_head_]);
    write_head_ = (write_head_ + n) % N;

    return ret;
  }

  [[nodiscard]] uint16_t capacity() const {
    return N;
  }

  void reset() {
    is_full_ = false;
    read_head_ = 0;
    write_head_ = 0;
    tail_ = 0;
  }
};
