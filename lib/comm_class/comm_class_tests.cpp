//#define TESTING
#ifdef TESTING

  #include "fakeit.hpp"

  #include "IHWMessage.h"
  #include "comm_class.h"
  #include <unity.h>

  #define M_RUN_TEST(tst)                                                                                              \
    do {                                                                                                               \
      msetUp();                                                                                                        \
      RUN_TEST(tst);                                                                                                   \
      mtearDown();                                                                                                     \
    } while (0)

using namespace fakeit;

Mock<IHWMessage> mock;
CommClass comm;
static void cb(const void* ptr, size_t sz) {
  comm.receive(ptr, sz);
}

template <class T>
static void call_receive(T data) {
  mock.get().IHWMessage::receive(&data, sizeof(data));
}

static void msetUp() {
  comm.set_hw_msg(&mock.get());
  mock.get().IHWMessage::set_receive_cb(cb);
  When(Method(mock, status)).AlwaysReturn(true);
}

static void mtearDown() {
  comm.empty_rx();
  When(Method(mock, transmit)).AlwaysDo([](const void* p, size_t sz) { return sz; });
  comm.flush();
  mock.Reset();
}


void test_init_called() {
  When(Method(mock, init)).Return();
  comm.init();
  Verify(Method(mock, init));
}

void test_available() {
  TEST_ASSERT_EQUAL(0, comm.available());
  int32_t d = 123;
  call_receive(d);
  TEST_ASSERT_EQUAL(4, comm.available());
}

void test_read_buff() {
  uint8_t data = 0xAB;
  call_receive(data);
  uint8_t dst = 0;
  comm.read(&dst, 1);
  TEST_ASSERT_EQUAL_UINT8(data, dst);
}

void test_read_any() {
  float f = 3.14;
  size_t s = 12;
  call_receive(f);
  call_receive(s);

  TEST_ASSERT_EQUAL(f, comm.read<float>());
  TEST_ASSERT_EQUAL(s, comm.read<size_t>());
}

void test_write() {
  uint8_t data = 0xBB;
  uint8_t received = 0;
  When(Method(mock, transmit)).Do([&received](const void* ptr, size_t sz) {
    TEST_ASSERT_EQUAL(1, sz);
    received = *static_cast<const uint8_t*>(ptr);
    return sz;
  });

  comm.write(data);
  comm.flush();
  TEST_ASSERT_EQUAL_UINT8(data, received);
}

void comm_class_test() {
  M_RUN_TEST(test_init_called);
  M_RUN_TEST(test_available);
  M_RUN_TEST(test_read_buff);
  M_RUN_TEST(test_read_any);
  M_RUN_TEST(test_write);
}

#endif
