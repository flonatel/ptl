#ifndef PTL_VISITOR_HH
#define PTL_VISITOR_HH

#include <memory>

namespace ptl { namespace visitor {

/*
 * Implementation of the Visitor Pattern.
 *
 * The Visitor consists of three parts:
 * o the visited objects (typical some container or hirachy)
 * o the iterator (which iterates in some way over the visited
 *   objects)
 * o the action which is executed / evaluated for each visited
 *   object.
 */
template< typename Visitable, typename Combiner >
class visitor {
public:
   template< typename RetType, typename Iterator, typename ... Args >
   static RetType accept( Iterator begin,
                          Iterator end, Args && ... args ) {
      return Combiner::template accept< RetType >(
         begin, end, std::forward< Args >( args ) ... );
   }

};

// ToDo: Add static_assert -> put into separate class.
// ToDo: Modify that the result is only returned if the Action class
//       has an appropriate method.
/*
 * Combiner
 * Combines the action with the visitable.
 */
template< typename Action, typename Visitable >
class combiner {
public:
   template< typename RetType, typename Iterator, typename ... Args >
   static RetType accept( Iterator & begin,
                          Iterator & end, Args && ... args ) {
      Action action( args ... );
      for(Iterator it( begin ); it!=end; ++it ) {
         action.visit( *it );
      }
      return action.result();
   }
};

}}

#endif
