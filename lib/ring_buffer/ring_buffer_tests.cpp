// #define TESTING
#ifdef TESTING
  #include "ring_buffer.h"
  #include "unity.h"

  #ifndef UNUSED
    #define UNUSED(x) (void)(x)
  #endif

void test_buffer_create() {
  RingBuffer<uint8_t, 10> buffer;

  TEST_ASSERT_TRUE(buffer.is_empty());
  TEST_ASSERT_FALSE(buffer.is_full());
  TEST_ASSERT_EQUAL(10, buffer.free());
  TEST_ASSERT_EQUAL(10, buffer.free_cont());
}


/******** PUSH - POP *********/

void test_push_one() {
  RingBuffer<uint8_t, 10> buffer;

  buffer.push(10);
  buffer.push(20);
  TEST_ASSERT_FALSE(buffer.is_empty());
  TEST_ASSERT_FALSE(buffer.is_full());
  TEST_ASSERT_EQUAL(8, buffer.free());
  TEST_ASSERT_EQUAL(10, buffer.pop());
  TEST_ASSERT_EQUAL(20, buffer.pop());

  TEST_ASSERT_TRUE(buffer.is_empty());
  TEST_ASSERT_FALSE(buffer.is_full());
}


void test_push_n() {
  RingBuffer<int, 4> buff;

  int arr[] = { 0, 1, 2, 3, 4 };
  TEST_ASSERT_EQUAL(3, buff.push(arr, 3));
  TEST_ASSERT_EQUAL(1, buff.push(arr, 2));

  UNUSED(buff.pop());
  TEST_ASSERT_EQUAL(1, buff.push(arr, 2));
}


void test_reserve_and_push() {
  RingBuffer<char, 5> buff{};

  TEST_ASSERT_NULL(buff.reserve(6));

  auto ptr = buff.reserve(2);
  *ptr = 1;
  *(ptr + 1) = 2;
  buff.commit();

  TEST_ASSERT_EQUAL(2, buff.size());
  TEST_ASSERT_EQUAL(1, buff.pop());
  TEST_ASSERT_EQUAL(2, buff.pop());

  ptr = buff.reserve(4);
  TEST_ASSERT_NULL(ptr);
  ptr = buff.reserve(3);
  *ptr = 3;
  *(ptr + 1) = 4;
  *(ptr + 3) = 5;


  ptr = buff.reserve(3);
  TEST_ASSERT_NULL(ptr);
  ptr = buff.reserve(2);
}


void test_pop_n() {
  RingBuffer<float, 5> buff;

  buff.push(1);
  buff.push(2);

  buff.pop(2);
  TEST_ASSERT_TRUE(buff.is_empty());

  buff.push(12);
  buff.push(13);
  buff.push(14);

  buff.pop(1);
  TEST_ASSERT_EQUAL(2, buff.size());
}

void test_pop_to() {
  RingBuffer<int, 5> buff;
  buff.push(1);
  buff.push(2);
  buff.push(3);
  int dst[3] = { 0, 0, 0 };
  int exp[3] = { 1, 2, 3 };
  buff.pop(dst, 3);
  TEST_ASSERT_EQUAL_INT_ARRAY(exp, dst, 3);
  TEST_ASSERT_EQUAL(0, buff.size());
}


/************** UTILITY ************/

void test_is_full() {
  RingBuffer<uint8_t, 2> buff;
  buff.push(1);
  buff.push(2);
  TEST_ASSERT_TRUE(buff.is_full());
  UNUSED(buff.pop());
  TEST_ASSERT_FALSE(buff.is_full());
}

void test_is_empty() {
  RingBuffer<uint8_t, 2> buff;

  TEST_ASSERT_TRUE(buff.is_empty());
  buff.push(1);
  TEST_ASSERT_FALSE(buff.is_empty());
  buff.push(2);
  TEST_ASSERT_FALSE(buff.is_empty());
  UNUSED(buff.pop());
  TEST_ASSERT_FALSE(buff.is_empty());
  UNUSED(buff.pop());
  TEST_ASSERT_TRUE(buff.is_empty());
}

void test_overflow() {
  RingBuffer<uint8_t, 3> buff;

  buff.push(0);
  buff.push(1);
  buff.push(2);

  TEST_ASSERT_EQUAL(0, buff.pop());
  buff.push(3);
  TEST_ASSERT_TRUE(buff.is_full());
  TEST_ASSERT_EQUAL(1, buff.pop());
  TEST_ASSERT_EQUAL(2, buff.pop());
  TEST_ASSERT_EQUAL(3, buff.pop());
  TEST_ASSERT_TRUE(buff.is_empty());

  buff.push(4);
  buff.push(5);
  TEST_ASSERT_EQUAL(4, buff.pop());
  TEST_ASSERT_EQUAL(5, buff.pop());
  TEST_ASSERT_TRUE(buff.is_empty());
}

void test_peek() {
  RingBuffer<uint8_t, 4> buff;

  buff.push(0);
  buff.push(1);
  TEST_ASSERT_EQUAL(0, buff.peek());
  TEST_ASSERT_EQUAL(0, buff.pop());
  TEST_ASSERT_EQUAL(1, buff.peek());
}

void test_free_size() {
  RingBuffer<uint8_t, 5> buff;

  TEST_ASSERT_EQUAL(5, buff.free());
  TEST_ASSERT_EQUAL(0, buff.size());

  buff.push(1);
  TEST_ASSERT_EQUAL(4, buff.free());
  TEST_ASSERT_EQUAL(1, buff.size());


  buff.push(2);
  TEST_ASSERT_EQUAL(3, buff.free());
  TEST_ASSERT_EQUAL(2, buff.size());

  UNUSED(buff.pop());
  TEST_ASSERT_EQUAL(4, buff.free());
  TEST_ASSERT_EQUAL(1, buff.size());


  UNUSED(buff.pop());
  TEST_ASSERT_EQUAL(5, buff.free());
  TEST_ASSERT_EQUAL(0, buff.size());

  buff.push(0);
  buff.push(1);
  buff.push(2);
  buff.push(3);
  buff.push(4);
  TEST_ASSERT_EQUAL(0, buff.free());
  TEST_ASSERT_EQUAL(5, buff.size());
  buff.push(5);
  TEST_ASSERT_EQUAL(0, buff.free());
  TEST_ASSERT_EQUAL(5, buff.size());
}


void test_size_cont() {
  RingBuffer<int, 5> buff;
  int arr[] = { 1, 2, 3, 4, 5 };

  TEST_ASSERT_EQUAL(0, buff.size_cont());

  buff.push(arr, 3);
  TEST_ASSERT_EQUAL(3, buff.size_cont());

  buff.push(arr, 2);
  TEST_ASSERT_EQUAL(5, buff.size_cont());

  buff.pop(1);
  TEST_ASSERT_EQUAL(4, buff.size_cont());

  buff.pop(2);
  buff.push(10);
  TEST_ASSERT_EQUAL(2, buff.size_cont());
  buff.pop(2);
  TEST_ASSERT_EQUAL(1, buff.size_cont());
}


void test_free_cont() {
  RingBuffer<char, 5> buff{};

  TEST_ASSERT_EQUAL(5, buff.free_cont());
  buff.push(1);
  buff.push(2);
  TEST_ASSERT_EQUAL(3, buff.free_cont());

  UNUSED(buff.pop());
  UNUSED(buff.pop());
  TEST_ASSERT_EQUAL(3, buff.free_cont());

  buff.push(3);
  buff.push(4);
  UNUSED(buff.pop());
  TEST_ASSERT_EQUAL(1, buff.free_cont());
  buff.push(5);
  TEST_ASSERT_EQUAL(3, buff.free_cont());
}



void ring_buffer_tests() {
  RUN_TEST(test_buffer_create);
  RUN_TEST(test_push_one);
  RUN_TEST(test_push_n);
  RUN_TEST(test_reserve_and_push);
  RUN_TEST(test_pop_n);
  RUN_TEST(test_pop_to);
  RUN_TEST(test_is_full);
  RUN_TEST(test_is_empty);
  RUN_TEST(test_overflow);
  RUN_TEST(test_peek);
  RUN_TEST(test_free_size);
  RUN_TEST(test_size_cont);
  RUN_TEST(test_free_cont);
}




#endif