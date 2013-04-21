#ifndef PTL_OBSERVER_HH
#define PTL_OBSERVER_HH

#include <functional>
#include <list>

namespace ptl { namespace observer {

template< typename T, typename CB = typename T::callback_type >
class subject {
public:
   using callback_function_type = std::function< CB >;

   void register_observer( T & t, callback_function_type const & f ) {
      observers_.push_back( cb_data( t, f ) );
   }

   template< typename ... Args >
   void notify_observers( Args && ... args ) {
      for( auto & it : observers_ ) {
         it.cf_( it.t_, args ... );
      }
   }

private:
   struct cb_data {
      cb_data( T & t, callback_function_type const & cf )
         : t_( t ), cf_( cf ) {}

      T & t_;
      callback_function_type const & cf_;
   };

   std::list< cb_data > observers_;
};

}}

#endif
