#include <ptl/object_pool.hh>

#include <gtest/gtest.h>

class ObjectPoolTest : public ::testing::Test {
public:
   void test_st_fail_init_only();
   void test_st_fail_init_only_lambda();
   void test_st_fail_get_one();
   void test_st_fail_get_all();
   void test_st_fail_get_too_many();
   void test_st_fail_put_many();

   void test_st_alloc_init_only();
   void test_st_alloc_get_all();
};

class A {
public:
   A( int i ) : _i( i ) {
      std::cerr << "A( int i ) [" << _i << "]" << std::endl;
      ++cnstr_call_cnt;
   }

   // Be sure that this is not copied around
   A() = delete;
//   A( A const & ) = delete;
   A & operator=( A const & ) = delete;

   int value() const { return _i; }

   static int cnstr_call_cnt;

private:
   int _i;
};

using a_sp = std::shared_ptr< A >;
using a_ptr = A *;

int A::cnstr_call_cnt( 0 );

a_ptr a_factory( int i ) {
   return new A( i );
}

TEST_F(ObjectPoolTest, test_st_fail_init_only) {
   A::cnstr_call_cnt = 0;
   int const j { 9 };
   ptl::object_pool::pool< A > a_pool( std::bind( a_factory, j ) );
   ASSERT_EQ( A::cnstr_call_cnt, 7 );
}

TEST_F(ObjectPoolTest, test_st_fail_init_only_lambda) {
   A::cnstr_call_cnt = 0;
   int const j { 10 };
   ptl::object_pool::pool< A > a_pool(
      [j]() -> a_ptr { return new A( j ); } );
   ASSERT_EQ( A::cnstr_call_cnt, 7 );
}

TEST_F(ObjectPoolTest, test_st_fail_get_one) {
   A::cnstr_call_cnt = 0;
   int const j { 11 };
   ptl::object_pool::pool< A > a_pool( std::bind( a_factory, j ) );

   auto a( a_pool.get() );

   ASSERT_EQ( A::cnstr_call_cnt, 7 );
   ASSERT_EQ( a->value(), j );
}

#if 0
TEST_F(ObjectPoolTest, test_st_fail_get_all) {
   A::cnstr_call_cnt = 0;
   int const j { 9 };
   ptl::object_pool::pool< A > a_pool( std::bind( a_factory, j ) );

   for( int i{ 0 }; i < 7; ++i ) {
      auto a( a_pool.get() );
      ASSERT_EQ( a->value(), 9 );
   }

   ASSERT_EQ( A::cnstr_call_cnt, 7 );
}

TEST_F(ObjectPoolTest, test_st_fail_get_too_many) {
   A::cnstr_call_cnt = 0;
   int const j { 9 };
   ptl::object_pool::pool< A > a_pool( std::bind( a_factory, j ) );

   for( int i{ 0 }; i < 7; ++i ) {
      auto a( a_pool.get() );
      ASSERT_EQ( a->value(), 9 );
   }

   ASSERT_EQ( A::cnstr_call_cnt, 7 );
   ASSERT_THROW( a_pool.get(), std::runtime_error );
}

TEST_F(ObjectPoolTest, test_st_fail_put_many) {
   A::cnstr_call_cnt = 0;
   int const j { 9 };
   ptl::object_pool::pool< A > a_pool( std::bind( a_factory, j ) );

   for( int i{ 0 }; i < 15; ++i ) {
      auto a( a_pool.get() );
      ASSERT_EQ( a->value(), 9 );
      a_pool.put( a );
   }

   ASSERT_EQ( A::cnstr_call_cnt, 7 );
}

// ==================================================

TEST_F(ObjectPoolTest, test_st_alloc_init_only) {
   A::cnstr_call_cnt = 0;
   int const j { 9 };
   ptl::object_pool::pool< A, 7,
                           ptl::object_pool::strategies::alloc_new > a_pool(
      std::bind( a_factory, j ) );
   ASSERT_EQ( A::cnstr_call_cnt, 0 );
}

TEST_F(ObjectPoolTest, test_st_alloc_get_all) {
   A::cnstr_call_cnt = 0;
   int const j { 9 };
   ptl::object_pool::pool<
      A, 7, ptl::object_pool::strategies::alloc_new > a_pool(
         std::bind( a_factory, j ) );

   for( int i{ 0 }; i < 7; ++i ) {
      auto a( a_pool.get() );
      ASSERT_EQ( a->value(), 9 );
   }

   ASSERT_EQ( A::cnstr_call_cnt, 7 );
}
#endif

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
