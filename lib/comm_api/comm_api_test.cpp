#ifdef TESTING
  #include "comm_api.h"
  #include "ISerial.h"
  #include "fakeit.hpp"
  #include "unity.h"

// create mock

namespace MockHelp {

  static size_t buf_len = 0;
  static size_t read_idx = 0;
  static uint8_t buff[100] = { 0 };
  static const mixer::ProgramVolume volume(100, 30, "Hello World");

  uint8_t read_u8() {
    return buff[read_idx++];
  }
  uint32_t read_u32() {
    auto val = utils::mem2T<uint32_t>(buff + read_idx);
    read_idx += 4;
    return val;
  }
  size_t available() {
    return buf_len - read_idx;
  }
  size_t wait_for(size_t n) {
    return n;
  }

  void populate_buff() {
    read_idx = 0;
    buf_len = 0;
    unsigned idx = 0;
    auto insert = [&idx](auto what) -> void {
      memcpy(buff + idx, &what, sizeof(what));
      idx += sizeof(what);
    };

    auto crc_from_to = [](unsigned from, unsigned to) -> uint32_t { return utils::crc32mpeg2(buff + from, to - from); };

    // one entry
    insert((uint8_t)1);
    insert(crc_from_to(0, 1));

    const uint8_t name_sz = strlen(volume.name_) + 1;

    auto from = idx;
    insert(volume.pid_);
    insert(volume.volume_);
    insert(name_sz);
    insert(crc_from_to(from, idx));

    from = idx;
    memcpy(buff + idx, volume.name_, name_sz);
    idx += name_sz;
    insert(crc_from_to(from, idx));

    buf_len = idx;
  }
}  // namespace MockHelp


fakeit::Mock<ISerial> create_mock() {
  using namespace fakeit;
  fakeit::Mock<ISerial> mock;

  When(Method(mock, available)).AlwaysDo(MockHelp::available);
  When(Method(mock, u8)).AlwaysDo(MockHelp::read_u8);
  When(Method(mock, u32)).AlwaysDo(MockHelp::read_u32);
  When(Method(mock, wait_for)).AlwaysDo(MockHelp::wait_for);


  Fake(OverloadedMethod(mock, write, size_t(uint8_t)));
  Fake(OverloadedMethod(mock, write, size_t(const uint8_t*, size_t)));
  Fake(OverloadedMethod(mock, write, size_t(const char*)));

  Fake(Method(mock, flush));

  Fake(OverloadedMethod(mock, read, size_t(uint8_t*, size_t)));
  Fake(OverloadedMethod(mock, read, size_t(char*, size_t)));

  Fake(Method(mock, empty_rx));

  Fake(Method(mock, c));
  Fake(Method(mock, u16));
  Fake(Method(mock, i8));
  Fake(Method(mock, i16));
  Fake(Method(mock, i32));
  Fake(Method(mock, flt));

  return mock;
}


auto mock = create_mock();
void test_sequence() {
  MockHelp::populate_buff();
  MixerAPI api;
  api.set_uart(&(mock.get()));


  api.load_volumes();
  std::optional opt = api.volumes_[0];
  TEST_ASSERT_TRUE(opt.has_value());
  volatile bool b = opt.has_value();
  auto ret = *opt;
  auto exp = MockHelp::volume;

  TEST_ASSERT_EQUAL_UINT32(exp.pid_, ret.pid_);
  TEST_ASSERT_EQUAL_UINT8(exp.volume_, ret.volume_);
  TEST_ASSERT_EQUAL_STRING(exp.name_, ret.name_);
}

void mixer_api_test() {
  RUN_TEST(test_sequence);
}

#endif