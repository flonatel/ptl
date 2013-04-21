#ifndef PTL_OBSERVER_HH
#define PTL_OBSERVER_HH

#include <functional>

namespace ptl { namespace observer {

template< typename T, typename CB = typename T::callback_type >
class subject {
public:
   void register_observer( std::function< CB > const & f );

   template< typename ... Args >
   void notify_observers( Args && ... args );
};

}}

#endif
