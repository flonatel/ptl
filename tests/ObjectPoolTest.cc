#include <ptl/object_pool.hh>

#include <gtest/gtest.h>

class ObjectPoolTest : public ::testing::Test {
public:
};

template< typename OBJ_TYPE >
using mtqueue = ptl::object_pool::pool<
   OBJ_TYPE,
   ptl::object_pool::policies::threading::multi,
   ptl::object_pool::policies::notify::all,
   ptl::object_pool::policies::notify::all,
   ptl::object_pool::policies::termination::terminatable,
   ptl::object_pool::policies::container::queue,
   ptl::object_pool::policies::size_handling::constant >;

class A {
};

ptl::object_pool::policies::size_handling::constant csize( 777 );

TEST_F(ObjectPoolTest, test_compile) {

   mtqueue< std::string > const mtqs( csize );
   (void)mtqs;

   mtqueue< int > const mtqi( csize );
   (void)mtqi;

   mtqueue< A > const mtqa( csize );
   (void)mtqa;
}

TEST_F(ObjectPoolTest, test_push) {

   mtqueue< int > mtqi( csize );
   mtqi.push( 7 );
   ASSERT_EQ( mtqi.size(), 1U );
}

TEST_F(ObjectPoolTest, test_push_10) {

   mtqueue< int > mtqi( csize );
   for( int i( 0 ); i < 10; ++i ) {
      mtqi.push( 7 );
   }
   ASSERT_EQ( mtqi.size(), 10U );
}

TEST_F(ObjectPoolTest, test_push_and_pop) {

   mtqueue< int > mtqi( csize );
   mtqi.push( 7 );
   ASSERT_EQ( mtqi.size(), 1U );
   int const c( mtqi.pop() );
   ASSERT_EQ( mtqi.size(), 0U );
   ASSERT_EQ( c, 7 );
}

TEST_F(ObjectPoolTest, test_push_and_pop_10) {

   mtqueue< int > mtqi( csize );
   for( int i( 0 ); i < 10; ++i ) {
      mtqi.push( i + 7 );
   }
   ASSERT_EQ( mtqi.size(), 10U );
   for( int i( 0 ); i < 10; ++i ) {
      int const c( mtqi.pop() );
      ASSERT_EQ( c, i + 7 );
   }
   ASSERT_EQ( mtqi.size(), 0U );
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
