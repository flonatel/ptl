#ifndef PTL_OBJECT_POOL_HH
#define PTL_OBJECT_POOL_HH

#include <functional>
#include <vector>

namespace ptl { namespace object_pool {

class strategy {};

namespace strategies {

template< typename ObjectType, int PoolSize >
class fail {
public:
   using factory_type = std::function< ObjectType() >;

   static void init( std::vector< ObjectType> & objects,
                     factory_type const & factory ) {
      objects.reserve( PoolSize );
      for( unsigned long cnt( 0 ); cnt < PoolSize; ++cnt ) {
         objects.emplace_back( factory() );
      }
   }
};

class alloc_new {
};

class multithread_block {
};

}

template< typename ObjectType,
          int      PoolSize = 7,
          template< typename ObjectType, int PoolSize > class StrategyType
          = strategies::fail /* < ObjectType, PoolSize >*/ >
class pool {
public:
   using factory_type = std::function< ObjectType() >;

   pool( factory_type const & factory )
   : _factory( factory ) {
      StrategyType< ObjectType, PoolSize >::init( _objects, _factory );
   }

private:
   // ToDo: Move to alloc_new?
   factory_type const _factory;
  std::vector< ObjectType > _objects;
};

}}

#endif
