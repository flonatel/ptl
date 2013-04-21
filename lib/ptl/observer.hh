#ifndef PTL_OBSERVER_HH
#define PTL_OBSERVER_HH

#include <functional>
#include <list>

namespace ptl { namespace observer {

template< typename T, typename CB = typename T::callback_type >
class subject {
public:
   using CBF = std::function< CB >;

   void register_observer( T & t, std::function< CB > const & f ) {
      observers_.push_back( sdata( t, f ) );
   }

   template< typename ... Args >
   void notify_observers( Args && ... args ) {
      for( auto & it : observers_ ) {
         it.cbf( it.t, args ... );
      }
   }

private:
   struct sdata {
      sdata( T & pt, std::function< CB > const & pf )
         : t( pt ), cbf( pf ) {}

      T & t;
      CBF const & cbf;
   };

   std::list< sdata > observers_;
};

}}

#endif
