//#define TESTING
#ifdef TESTING
  #include "utils.h"
  #include "unity.h"


void test_constrain() {
  TEST_ASSERT_EQUAL(3, utils::constrain(3, 0, 5));
  TEST_ASSERT_EQUAL(0, utils::constrain(-5, 0, 5));
  TEST_ASSERT_EQUAL(5, utils::constrain(8, 0, 5));
}

void test_within() {
  TEST_ASSERT_TRUE(utils::within(1, 0, 2));
  TEST_ASSERT_TRUE(utils::within(1, 1, 2));
  TEST_ASSERT_TRUE(utils::within(1, 0, 1));
  TEST_ASSERT_FALSE(utils::within(1, 2, 4));
  TEST_ASSERT_FALSE(utils::within(5, 2, 4));
}

void test_between() {
  TEST_ASSERT_TRUE(utils::between(1, 0, 2));
  TEST_ASSERT_FALSE(utils::between(1, 1, 2));
  TEST_ASSERT_FALSE(utils::between(1, 0, 1));
  TEST_ASSERT_FALSE(utils::between(1, 2, 4));
  TEST_ASSERT_FALSE(utils::between(5, 2, 4));
}

void test_elapsed() {
  TEST_ASSERT_TRUE(utils::elapsed(200, 150));
  TEST_ASSERT_TRUE(utils::elapsed(10, UINT32_MAX - 30));  // overflow
}


void test_mem2T() {
  volatile int x = 10;
  TEST_ASSERT_EQUAL_INT(x, utils::mem2T<int>(reinterpret_cast<uint8_t*>(const_cast<int*>(&x))));

  volatile float f = 3.14f;
  TEST_ASSERT_EQUAL_FLOAT(f, utils::mem2T<float>(reinterpret_cast<uint8_t*>(const_cast<float*>(&f))));
}

void test_crc32mpeg2() {
  uint8_t buff[] = { 0x01, 0x02, 0x03 };
  uint32_t crc = utils::crc32mpeg2(buff, 3);
  TEST_ASSERT_EQUAL_UINT32(0x1B0D6951, crc);
}

void test_utils() {
  RUN_TEST(test_constrain);
  RUN_TEST(test_within);
  RUN_TEST(test_between);
  RUN_TEST(test_elapsed);
  RUN_TEST(test_mem2T);
  RUN_TEST(test_crc32mpeg2);
}



#endif