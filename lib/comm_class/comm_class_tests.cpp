//#define TESTING
#ifdef TESTING

  #include "fakeit.hpp"

  #include "IHWMessage.h"
  #include "comm_class.h"
  #include <unity.h>

using namespace fakeit;

Mock<IHWMessage> mock;
CommClass comm;
static void cb(const void* ptr, size_t sz) {
  comm.receive(ptr, sz);
}

void setUp() {
  comm.set_hw_msg(&mock.get());
  mock.get().IHWMessage::set_receive_cb(cb);
  When(Method(mock, status)).AlwaysReturn(true);
}

void tearDown() {
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



void comm_class_test() {
  RUN_TEST(test_init_called);
}

#endif