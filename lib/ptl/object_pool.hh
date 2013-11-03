#ifndef PTL_OBJECT_POOL_HH
#define PTL_OBJECT_POOL_HH

#include <cstdlib>
#include <mutex>
#include <condition_variable>
#include <queue>

/*
 * Object Pool
 * This file contains a policy based object pool implementation.
 * The interface reflects the use cases of a stock for objects
 * wich has to be worked on by some instance.
 * The following behaviour is configurable with the help of the
 * different policies:
 * o threading::multi / threading::single
 * o notify::all / notify::none
 * o termination::terminatable / termination::run_forever
 * o size_handling::constant / size_handling::unlimited
 * Please note, that the details of the threading constructs of
 * implementation of the different policies must match, e.g. all
 * must use e.g. C++ std::thread / std::mutex / std::condition_variable.
 */
namespace ptl { namespace object_pool {

class terminate_except
   : public std::runtime_error {
public:
   terminate_except( )
      : std::runtime_error( "object_pool termination" ) {
   }
};

namespace policies {

/*
 * o threading::multi / threading::single:
 *   The multi-threaded version does a correct locking everywhere
 *   where it's needed.  The single threaded version does the
 *   apropriate locking for a single thread, which is it does nothing.
 */

namespace threading {

class multi {
public:
   class lock {
   public:
      lock( multi & m )
      : lock_( m.get_mutex() ) {}

      std::unique_lock< std::mutex > & get_lock() {
         return lock_;
      }
   private:
      std::unique_lock< std::mutex > lock_;
   };

private:
   std::mutex & get_mutex() {
      return mutex_;
   }

   std::mutex mutex_;

   friend class lock;
};

// XXX To implement
// XXX To test
class single {
};

}

/*
 * o notify::all / notify::none:
 * Send out notifications (or not) when the pool is full or empty.
 */

namespace notify {

template< typename POLICIY_THREADING >
class all {
public:
   void notify() {
      cv_not_prop_.notify_all();
   }

   void wait( typename POLICIY_THREADING::lock & lock ) {
      cv_not_prop_.wait( lock.get_lock() );
   }

private:
   std::condition_variable cv_not_prop_;
};

// XXX To implement
// XXX To test
class none {
};

}

/*
 * o termination::terminatable / termination::run_forever:
 *   Configures if the pool propagates termination requests
 *   or not.
 */
namespace termination {

template< typename POLICIY_THREADING >
class terminatable {
public:
   terminatable()
      : started_( false ),
        terminate_cnt_( 0 ) {
   }

   bool should_terminate() const {
      return started_ == true and terminate_cnt_ == 0;
   }

   void start() {
      if( started_ ) {
         // This should never happen: programing bug:
         // Start was already called.
         abort();
      }
      started_ = true;
      if( terminate_cnt_ == 0 ) {
         // This should never happen - programming bug:
         // There was no 'register_terminator' called.
         abort();
      }
      cv_wait_for_start_.notify_all();
   }

   void terminate( typename POLICIY_THREADING::lock & lock ) {
      // Ensure that this thread was really started.
      while( not started_ ) {
         cv_wait_for_start_.wait( lock.get_lock() );
      }
      --terminate_cnt_;
      if( terminate_cnt_ < 0 ) {
         // Programming bug:
         // terminate() was called to often for this pool.
         abort();
      }
      if( terminate_cnt_ == 0) {
         cv_wait_for_termination_.notify_all();
      }
   }

   void register_terminator() {
      if( started_ ) {
         // This should never happen: programing bug:
         // Start was already called - before finishing to register
         // all terminators.
         abort();
      }
      ++terminate_cnt_;
   }

private:
   bool started_;
   long terminate_cnt_;

   std::condition_variable cv_wait_for_start_;
   std::condition_variable cv_wait_for_termination_;
};

// XXX To implement
// XXX To test
class run_forever {
};

}

/*
 * This is an abstraction of the underlaying container.
 * Currently only a queue is implemented.
 */
namespace container {

template< typename OBJ_TYPE >
class queue {
public:
   void push( OBJ_TYPE const & t ) {
      queue_.push( t );
   }

   std::size_t size() const {
      return queue_.size();
   }

   OBJ_TYPE pop() {
      OBJ_TYPE rval( queue_.front() );
      queue_.pop();
      return rval;
   }

   bool empty() {
      return queue_.empty();
   }

private:
   std::queue< OBJ_TYPE > queue_;
};

}

/*
 * o size_handling::constant / size_handling::unlimited
 *   The constant size handling allows a constant number
 *   of objects. The unlimited allows unlimited number
 *   of elements.
 */

namespace size_handling {

class constant {
public:
   constant( std::size_t const max_size )
      : max_size_( max_size ) {
   }

   bool free_slot_available( std::size_t const cur_size ) {
      return cur_size < max_size_;
   }

private:
   std::size_t max_size_;
};

// XXX To implement
// XXX To test
class unlimited {
};

}

}

template< typename OBJ_TYPE,
          typename POLICIY_THREADING,
          template< typename POLICIY_THREADING_1 >
             class POLICIY_NOTIFY_NOT_FULL,
          template< typename POLICIY_THREADING_2 >
             class POLICIY_NOTIFY_NOT_EMPTY,
          template< typename POLICIY_THREADING_3 >
             class POLICIY_TERMINATION,
          template< typename OBJ_TYPE_1 >
             class POLICIY_CONTAINER,
          typename POLICIY_SIZE_HANDLING >
class pool {
public:
   pool( POLICIY_SIZE_HANDLING const & size_handling )
     : size_handling_( size_handling ) {
   }

   void push( OBJ_TYPE const & t ) {
      {
         typename POLICIY_THREADING::lock lock( threading_ );
         if( termination_.should_terminate() ) {
            // Try to push something in a termianted pool
            // -> implementation bug of non library source code.
            abort();
         }

         while( not termination_.should_terminate()
                and not size_handling_.free_slot_available(
                   container_.size() ) ) {
            notify_not_full_.wait( lock );
         }

         container_.push( t );
      }
      notify_not_empty_.notify();
   }

   OBJ_TYPE pop() {
      typename POLICIY_THREADING::lock lock( threading_ );

      while( not termination_.should_terminate()
             and container_.empty() ){
         notify_not_empty_.wait( lock );
      }

      // As long as there is some data in the queue, return this -
      // even if the queue was already terminated.  (This ensures
      // that all data in the system is handled before the
      // thread / process stops.
      if( not container_.empty() ) {
         OBJ_TYPE const rval( container_.pop() );
         if( size_handling_.free_slot_available(
                container_.size() ) ) {
            notify_not_full_.notify();
         }
         return rval;
      }

      // When the pool is empty and the termination flag was set,
      // through out an appropriate exception.
      throw ptl::object_pool::terminate_except();
   }

   std::size_t size() {
      typename POLICIY_THREADING::lock lock( threading_ );
      return container_.size();
   }

   void start() {
      typename POLICIY_THREADING::lock lock( threading_ );
      termination_.start();
   }

   void terminate() {
      typename POLICIY_THREADING::lock lock( threading_ );
      termination_.terminate( lock );
      // Also notify the not full and not empty that they can stop
      // processing.
      notify_not_full_.notify();
      notify_not_empty_.notify();
   }

   void register_terminator() {
      typename POLICIY_THREADING::lock lock( threading_ );
      termination_.register_terminator();
   }

   bool should_terminate() {
      typename POLICIY_THREADING::lock lock( threading_ );
      return termination_.should_terminate();
   }

private:
   POLICIY_THREADING threading_;
   POLICIY_NOTIFY_NOT_FULL< POLICIY_THREADING > notify_not_full_;
   POLICIY_NOTIFY_NOT_EMPTY< POLICIY_THREADING > notify_not_empty_;
   POLICIY_TERMINATION< POLICIY_THREADING > termination_;
   POLICIY_CONTAINER< OBJ_TYPE > container_;
   POLICIY_SIZE_HANDLING size_handling_;
};

}}

#endif
