#include <ptl/object_pool.hh>

#include <gtest/gtest.h>

class ObjectPoolTest : public ::testing::Test {
public:
   void test_st_fail_init_only();
};

class A {
public:
   A( int i ) : _i( i ) {
      ++cnstr_call_cnt;
   }

   static int cnstr_call_cnt;

private:
   int _i;
};

int A::cnstr_call_cnt( 0 );

A a_factory( int i ) {
   return A( i );
}

TEST_F(ObjectPoolTest, test_st_fail_init_only) {

   int const j { 9 };
   ptl::object_pool::pool< A > a_pool( std::bind( a_factory, j ) );

   ASSERT_EQ( A::cnstr_call_cnt, 7 );
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
