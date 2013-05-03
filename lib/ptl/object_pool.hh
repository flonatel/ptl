#ifndef PTL_OBJECT_POOL_HH
#define PTL_OBJECT_POOL_HH

#include <functional>
#include <vector>
#include <bitset>
#include <memory>
#include <stdexcept>

namespace ptl { namespace object_pool {

class strategy {};

namespace strategies {

template< typename ObjectType, int PoolSize >
class fail {
public:
   using factory_type = std::function< ObjectType() >;

   fail( std::vector< ObjectType> & objects,
         factory_type const & factory ) {
      objects.reserve( PoolSize );
      for( unsigned long cnt( 0 ); cnt < PoolSize; ++cnt ) {
         objects.emplace_back( factory() );
      }
   }

   void no_more_objects() {
      throw std::runtime_error( "No more objects" );
   }
};

template< typename ObjectType, int PoolSize >
class alloc_new {
public:
   using factory_type = std::function< ObjectType() >;

   alloc_new( std::vector< ObjectType> & objects,
              factory_type const & factory)
   : _objects( objects ),
      _factory( factory ) {
      objects.reserve( PoolSize );
   }

   void no_more_objects() {
      if( _objects.size() < PoolSize ) {
         _objects.emplace_back( _factory() );
         return;
      }
      throw std::runtime_error( "No more objects" );
   }

private:
   std::vector< ObjectType > & _objects;
   factory_type const _factory;
};

class multithread_block {
};

}

template< typename ObjectType,
          int      PoolSize = 7,
          template< typename ObjectTypeL, int PoolSizeL > class StrategyType
             = strategies::fail >
class pool {
public:
   using factory_type = std::function< ObjectType() >;

   pool( factory_type const & factory )
   : _strategy( StrategyType< ObjectType, PoolSize >( _objects, factory ) ) {}

   ObjectType & get() {
      int const first_free( search_first_free() );
      if( first_free < 0 ) {
         _strategy.no_more_objects();
      }
      alloc_object( first_free );
      return _objects[ first_free ];
   }

   void put( ObjectType & obj ) {
      for( int i{ 0 }; i < PoolSize; ++i ) {
         if( & _objects[ i ] == & obj ) {
            free_object( i );
            return;
         }
      }
      // Try to put an object which is not part of the pool.
      abort();
   }

private:
   int search_first_free() const {
      for( int i{ 0 }; i < PoolSize; ++i ) {
         if( not _used[i] ) {
            return i;
         }
      }
      return -1;
   }

   void alloc_object( int n ) {
      _used.set( n );
   }

   void free_object( int n ) {
      _used.reset( n );
   }

   std::vector< ObjectType > _objects;
   std::bitset< PoolSize >   _used;
   StrategyType< ObjectType, PoolSize > _strategy;
};

}}

#endif
