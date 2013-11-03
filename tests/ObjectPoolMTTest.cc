#include <ptl/object_pool.hh>

#include <thread>
#include <atomic>
#include <gtest/gtest.h>

class ObjectPoolMTTest : public ::testing::Test {
public:
};

template< typename OBJ_TYPE >
using mtqueue = ptl::object_pool::pool<
   OBJ_TYPE,
   ptl::object_pool::strategies::threading::multi,
   ptl::object_pool::strategies::notify_not_full::all,
   ptl::object_pool::strategies::notify_not_empty::all,
   ptl::object_pool::strategies::termination::terminatable,
   ptl::object_pool::strategies::container::queue,
   ptl::object_pool::strategies::size_handling::constant >;

ptl::object_pool::strategies::size_handling::constant csize( 777 );

TEST_F(ObjectPoolMTTest, test_two_threads_simple) {

   mtqueue< int > mtqi( csize );

   std::thread t_send(
      [&mtqi](){ mtqi.push( 9 ); } );
   std::thread t_recv(
      [&mtqi](){
         int const c( mtqi.pop() );
         ASSERT_EQ( c, 9 ); } );
   t_send.join();
   t_recv.join();
};

TEST_F(ObjectPoolMTTest, test_two_threads_simple_100) {

   mtqueue< int > mtqi( csize );

   std::thread t_send(
      [&mtqi](){ for( int i(0); i < 100; ++i ) {
            mtqi.push( i + 9 ); } } );
   std::thread t_recv(
      [&mtqi](){
         for( int i(0); i < 100; ++i ) {
            int const c( mtqi.pop() );
            ASSERT_EQ( c, i + 9 ); } } );
   t_send.join();
   t_recv.join();
};

TEST_F(ObjectPoolMTTest, test_one_thread_terminate) {

   mtqueue< int > mtqi( csize );

   std::thread t_recv(
      [&mtqi]() {
         try {
            while( true ) {
               mtqi.pop();
            }
         } catch( ptl::object_pool::terminate_error & te ) {
            // normal termination...
         }
      } );

   mtqi.register_terminator();
   mtqi.start();
   mtqi.terminate();
   t_recv.join();
}

TEST_F(ObjectPoolMTTest, test_many_thread_terminate) {

   mtqueue< int > mtqi( csize );
   std::atomic_long overall_cnt( 0 );

   std::shared_ptr< std::thread > t_recvs[25];

   for( int i( 0 ); i < 25; ++i ) {
      t_recvs[ i ] = std::make_shared< std::thread >(
         [&mtqi, &overall_cnt]() {
            try {
               while( true ) {
                  mtqi.pop();
                  ++overall_cnt;
               }
            } catch( ptl::object_pool::terminate_error & te ) {
               // normal termination...
            }
         } );
   }

   mtqi.register_terminator();
   mtqi.start();

   // Add some data to the queue:
   for( int i( 0 ); i < 10000; ++i ) {
      mtqi.push( i + 99 );
   }

   mtqi.terminate();

   for( int i( 0 ); i < 25; ++i ) {
      t_recvs[ i ]->join();
   }

   ASSERT_EQ( overall_cnt.load(), 10000 );
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
