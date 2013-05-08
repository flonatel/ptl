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
   void test_st_alloc_get_too_many();
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

using a_uptr = std::unique_ptr< A >;

int A::cnstr_call_cnt( 0 );

a_uptr a_factory( int i ) {
   return std::unique_ptr< A >( new A( i ) );
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
      [j]() -> a_uptr { return std::unique_ptr< A >( new A( j ) ); } );
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

TEST_F(ObjectPoolTest, test_st_fail_get_all) {
   A::cnstr_call_cnt = 0;
   int const j { 12 };
   ptl::object_pool::pool< A > a_pool( std::bind( a_factory, j ) );

   for( int i{ 0 }; i < 12; ++i ) {
      auto a( a_pool.get() );
      ASSERT_EQ( a->value(), j );
   }

   ASSERT_EQ( A::cnstr_call_cnt, 7 );
}

TEST_F(ObjectPoolTest, test_st_fail_get_too_many) {
   A::cnstr_call_cnt = 0;
   int const j { 13 };
   ptl::object_pool::pool< A > a_pool( std::bind( a_factory, j ) );
   std::vector< std::shared_ptr< A > > av;

   for( int i{ 0 }; i < 7; ++i ) {
      av.push_back( a_pool.get() );
      ASSERT_EQ( av.back()->value(), j );
   }

   ASSERT_EQ( A::cnstr_call_cnt, 7 );
   ASSERT_THROW( a_pool.get(), std::runtime_error );
}

TEST_F(ObjectPoolTest, test_st_fail_put_many) {
   A::cnstr_call_cnt = 0;
   int const j { 14 };
   ptl::object_pool::pool< A > a_pool( std::bind( a_factory, j ) );

   for( int i{ 0 }; i < 15; ++i ) {
      auto a( a_pool.get() );
      ASSERT_EQ( a->value(), j );
   }

   ASSERT_EQ( A::cnstr_call_cnt, 7 );
}

// ==================================================

TEST_F(ObjectPoolTest, test_st_alloc_init_only) {
   A::cnstr_call_cnt = 0;
   int const j { 15 };
   ptl::object_pool::pool< A, 7,
                           ptl::object_pool::strategies::alloc_new > a_pool(
      std::bind( a_factory, j ) );
   ASSERT_EQ( A::cnstr_call_cnt, 0 );
}

TEST_F(ObjectPoolTest, test_st_alloc_get_all) {
   A::cnstr_call_cnt = 0;
   int const j { 16 };
   ptl::object_pool::pool<
      A, 7, ptl::object_pool::strategies::alloc_new > a_pool(
         std::bind( a_factory, j ) );
   std::vector< std::shared_ptr< A > > av;

   for( int i{ 0 }; i < 7; ++i ) {
      av.push_back( a_pool.get() );
      ASSERT_EQ( av.back()->value(), j );
   }

   ASSERT_EQ( A::cnstr_call_cnt, 7 );
}

TEST_F(ObjectPoolTest, test_st_alloc_get_too_many) {
   A::cnstr_call_cnt = 0;
   int const j { 17 };
   ptl::object_pool::pool< A, 7,
                           ptl::object_pool::strategies::alloc_new > a_pool(
      std::bind( a_factory, j ) );
   std::vector< std::shared_ptr< A > > av;

   for( int i{ 0 }; i < 7; ++i ) {
      av.push_back( a_pool.get() );
      ASSERT_EQ( av.back()->value(), j );
   }

   ASSERT_EQ( A::cnstr_call_cnt, 7 );
   ASSERT_THROW( a_pool.get(), std::runtime_error );
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
