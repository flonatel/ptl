#ifndef PTL_OBSERVER_HH
#define PTL_OBSERVER_HH

#include <functional>
#include <list>

namespace ptl { namespace observer {

/*
 * Implementation of the Observer Pattern.
 *
 * To use this:
 * o Create a subject object.
 * o Register as many methods as you want.
 * For each call of the 'notify_observers', all registered
 * objects / methods are called.
 */
template< typename CB >
class subject {
public:
   using callback_function_type = std::function< CB >;

   void register_observer( callback_function_type const & f ) {
      observers_.push_back( cb_data( f ) );
   }

   template< typename ... Args >
   void notify_observers( Args && ... args ) {
      for( auto & it : observers_ ) {
         it.cf_( args ... );
      }
   }

private:
   struct cb_data {
      cb_data( callback_function_type const & cf )
         : cf_( cf ) {}

      callback_function_type const cf_;
   };

   std::list< cb_data > observers_;
};

}}

#endif
