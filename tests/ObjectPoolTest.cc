#include <ptl/object_pool.hh>

#include <gtest/gtest.h>

class ObjectPoolTest : public ::testing::Test {
public:
   void test_st_fail_init_only();
   void test_st_fail_init_only_lambda();
   void test_st_fail_init_only_cstr();
   void test_st_fail_get_one();
   void test_st_fail_get_all();
   void test_st_fail_get_too_many();
   void test_st_fail_put_many();
};

class A {
public:
   A( int i ) : _i( i ) {
      ++cnstr_call_cnt;
   }

   int value() const { return _i; }

   static int cnstr_call_cnt;

private:
   int _i;
};

int A::cnstr_call_cnt( 0 );

A a_factory( int i ) {
   return A( i );
}

TEST_F(ObjectPoolTest, test_st_fail_init_only) {
   A::cnstr_call_cnt = 0;
   int const j { 9 };
   ptl::object_pool::pool< A > a_pool( std::bind( a_factory, j ) );
   ASSERT_EQ( A::cnstr_call_cnt, 7 );
}

TEST_F(ObjectPoolTest, test_st_fail_init_only_lambda) {
   A::cnstr_call_cnt = 0;
   int const j { 9 };
   ptl::object_pool::pool< A > a_pool( []() { return A( j ); } );
   ASSERT_EQ( A::cnstr_call_cnt, 7 );
}

TEST_F(ObjectPoolTest, test_st_fail_get_one) {
   A::cnstr_call_cnt = 0;
   int const j { 9 };
   ptl::object_pool::pool< A > a_pool( std::bind( a_factory, j ) );

   A & a( a_pool.get() );

   ASSERT_EQ( A::cnstr_call_cnt, 7 );
   ASSERT_EQ( a.value(), 9 );
}

TEST_F(ObjectPoolTest, test_st_fail_get_all) {
   A::cnstr_call_cnt = 0;
   int const j { 9 };
   ptl::object_pool::pool< A > a_pool( std::bind( a_factory, j ) );

   for( int i{ 0 }; i < 7; ++i ) {
      A & a( a_pool.get() );
      ASSERT_EQ( a.value(), 9 );
   }

   ASSERT_EQ( A::cnstr_call_cnt, 7 );
}

TEST_F(ObjectPoolTest, test_st_fail_get_too_many) {
   A::cnstr_call_cnt = 0;
   int const j { 9 };
   ptl::object_pool::pool< A > a_pool( std::bind( a_factory, j ) );

   for( int i{ 0 }; i < 7; ++i ) {
      A & a( a_pool.get() );
      ASSERT_EQ( a.value(), 9 );
   }

   ASSERT_EQ( A::cnstr_call_cnt, 7 );
   ASSERT_THROW( a_pool.get(), std::runtime_error );
}

TEST_F(ObjectPoolTest, test_st_fail_put_many) {
   A::cnstr_call_cnt = 0;
   int const j { 9 };
   ptl::object_pool::pool< A > a_pool( std::bind( a_factory, j ) );

   for( int i{ 0 }; i < 15; ++i ) {
      A & a( a_pool.get() );
      ASSERT_EQ( a.value(), 9 );
      a_pool.put( a );
   }

   ASSERT_EQ( A::cnstr_call_cnt, 7 );
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
