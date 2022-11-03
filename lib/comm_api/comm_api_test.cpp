#ifdef TESTING
  #include "comm_api.h"
  #include "fakeit.hpp"
  #include "unity.h"

// create mock

using namespace fakeit;
  #define M_RUN_TEST(tst)                                                                                              \
    do {                                                                                                               \
      msetUp();                                                                                                        \
      RUN_TEST(tst);                                                                                                   \
      mtearDown();                                                                                                     \
    } while (0)


static Mock<IHWMessage> mock;
static CommClass comm;

static void cb(const void* ptr, size_t sz) {
  comm.receive(ptr, sz);
}

template <class T>
static void call_receive(T data) {
  mock.get().IHWMessage::receive(&data, sizeof(data));
}

static void call_receive(void* data, size_t sz) {
  mock.get().IHWMessage::receive(data, sz);
}

static void msetUp() {
  comm.set_hw_msg(&mock.get());
  mock.get().IHWMessage::set_receive_cb(cb);
  When(Method(mock, status)).AlwaysReturn(true);
  When(Method(mock, init)).Return();
  When(Method(mock, transmit)).AlwaysDo([](const void* data, size_t sz) { return sz; });
}

static void mtearDown() {
  comm.empty_rx();
  When(Method(mock, transmit)).AlwaysDo([](const void* p, size_t sz) { return sz; });
  comm.flush();
  mock.Reset();
}

static const mixer::ProgramVolume volume(100, 30, "Hello World");

static constexpr size_t buff_sz = 100;
static uint8_t buff[buff_sz] = { 0 };

static size_t populate_buff_with_load() {
  memset(buff, 0, buff_sz);
  size_t buf_len = 0;
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
  insert(static_cast<uint8_t>(volume.muted_));
  insert(name_sz);
  insert(crc_from_to(from, idx));

  from = idx;
  memcpy(buff + idx, volume.name_, name_sz);
  idx += name_sz;
  insert(crc_from_to(from, idx));

  buf_len = idx;

  return buf_len;
}



void test_load_volumes() {
  comm.init();
  CommAPI& api = CommAPI::get_instance();

  api.init(&comm);

  When(Method(mock, transmit)).AlwaysDo([](const void* data, size_t sz) {
    if (sz >= 1 && *static_cast<const uint8_t*>(data) == 0x01) {
      call_receive(buff, populate_buff_with_load());
    }
    return sz;
  });

  api.load_volumes();
  std::optional opt = api.get_volumes()[0];
  TEST_ASSERT_TRUE(opt.has_value());

  TEST_ASSERT_EQUAL_UINT32(volume.pid_, opt->pid_);
  TEST_ASSERT_EQUAL_UINT8(volume.volume_, opt->volume_);
  TEST_ASSERT_EQUAL_STRING(volume.name_, opt->name_);
}

void mixer_api_test() {
  M_RUN_TEST(test_load_volumes);
}

#endif