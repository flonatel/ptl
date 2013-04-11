#ifndef CTL_TREE_HH
#define CTL_TREE_HH

#include <vector>
#include <memory>
#include <stack>
// ToDo: only for error management:
#include <cstdlib>
#include <iostream>

namespace ctl {

// ======================================================================
// ======================================================================
// === Interface

namespace internal {
// ======================================================================
// === TreeInitializer
// A Tree initializer can be used to initialize a tree at compile time.
// It is possible to have two different types: one for the inner nodes
// of the tree and one for the outer parts.
//
// [Implementation detail:
//  This implements a 'fat' interface because there is the need, that
//  the representation of inner and leaf trees is handled within the
//  same class because of the initializer_list.]
template< typename InitValue >
class tree_initializer {
public:
   using self_t = tree_initializer< InitValue >;
   using self_sp = std::shared_ptr< self_t >;

   // Initializer Interface
   // With the help of the next two constructors it is possible to
   // initialize a complete tree by means of initializer list(s).
   tree_initializer( InitValue const & v );
   tree_initializer( InitValue const & v,
                     std::initializer_list< self_t > subtree );

   bool is_inner() const;
   InitValue const & value() const;

   using subtreectr_t = std::vector< self_sp >;
   typename subtreectr_t::const_iterator cbegin() const;
   typename subtreectr_t::const_iterator cend() const;

private:
   bool const is_inner_tree_node_;
   InitValue const value_;
   subtreectr_t subtrees_;
};

} // namespace Internal

// ======================================================================
// === Tree Interface
// A tree can contain two different type of nodes: inner nodes and
// leaf nodes.  Leaf nodes do not have any subnodes.  Both nodes can
// have value types - which might be different to each other.
// Note that this container is build up in the way, that each of the
// subtrees is again a tree.  (Therefore the term 'node' referes
// always to a (complete) tree - also one node is a tree.)
//
// 'Inner' type values are stored in the respectivley nodes. It must
// be possible to convert the 'InitValue' type to an 'Value'.
//
// [Implementation detail:
//  There is the need for a common base class for the different node,
//  because they will go into the same container.  There are other
//  ways how this can be done (e.g. using some mechanism like
//  boost::any), but all of them need casting things which I want not
//  to use.]

namespace internal {
template< typename Value >
class tree_const_iterator_depth_first;
}

template< typename Value >
class tree : public std::enable_shared_from_this< tree< Value > > {
public:
   tree( Value const & v );
   virtual ~tree();

   using const_iterator_depth_first
      = internal::tree_const_iterator_depth_first< Value >;

   const_iterator_depth_first cbegin_depth_first();
   const_iterator_depth_first cend_depth_first();

   Value const & value() const;

private:
   Value value_;
};

// Everything is done using shared_ptr
template< typename Value >
using tree_sp = std::shared_ptr< tree < Value > >;

namespace internal {

template< typename Value >
class inner_tree
   : public tree< Value > {
public:
   inner_tree( Value const & iv );

   void push_back( tree_sp< Value > const & t) {
      nodes_.push_back( t );
   }

   using subtreectr_t = std::vector< tree_sp< Value > >;
   using const_iterator = typename subtreectr_t::const_iterator;

   const_iterator cbegin() const;
   const_iterator cend() const;

private:
   // This is the container with all the sub-trees.
   std::vector< tree_sp< Value > > nodes_;
};

template< typename Value >
using inner_tree_sp = std::shared_ptr< inner_tree < Value > >;


template< typename Value >
class leaf_tree
   : public tree< Value > {
public:
   leaf_tree( Value const & v );
};

// === Iterator tree_const_iterator_depth_first
template< typename Value >
class tree_const_iterator_depth_first {
public:
   // Returns an iterator which points to the beginning.
   tree_const_iterator_depth_first( tree_sp< Value > const & t );
   // Returns an iterator which points to the end.
   tree_const_iterator_depth_first( tree_sp< Value > const & t, int );

   bool operator!=( tree_const_iterator_depth_first const & that ) const;
   // ToDo: get the return value right
   void operator++();
   // [Note: Because of this operator* there is the need to have only
   //        one value for Value and Leafs.
   //        If there would be two, what would this operator return?]
   Value const & operator*() const;

private:
   struct iter_data {
      iter_data( typename inner_tree< Value >::const_iterator b,
                 typename inner_tree< Value >::const_iterator e )
         : current( b ), end( e ) {}

      typename inner_tree< Value >::const_iterator current;
      typename inner_tree< Value >::const_iterator const end;
   };


   void go_depth( tree_sp< Value > const & tree );

   // The handling of the top level end iterator is done in the way,
   // that an additional root-root container is used to store the one
   // and only node.  The end of this container is then the global
   // end.
   // [Design: To be able to uniformly handle this, there must be some
   //          magic done to be 'convert' the passed in pointer to a
   //          shared_ptr.
   //          This is done in the way that the initial deleter is set
   //          to some null-deleter.
   //  Conclusion: Only the type is needed - not the functionality.]
   typename inner_tree< Value >::subtreectr_t root_root_tree_;
   std::stack< iter_data > stack_;
};

} // namespace internal
// ======================================================================
// === Tree Utilities

// === make_tree
// Creates a tree from the tree_initializer

template< typename Value, typename InitValue = Value >
tree_sp< Value >
make_tree( internal::tree_initializer< InitValue > const & ti );


// ======================================================================
// ======================================================================
// === Implementation

namespace internal {
// ======================================================================
// === TreeInitializer

template< typename InitValue >
tree_initializer< InitValue >::tree_initializer(
   InitValue const & v )
   : is_inner_tree_node_( false ),
     value_( v ) {}

template< typename InitValue >
tree_initializer< InitValue >::tree_initializer(
   InitValue const & v,
   std::initializer_list< self_t > subtree )
   : is_inner_tree_node_( true ),
     value_( v ) {
   for( auto it: subtree ) {
      subtrees_.push_back( std::make_shared< self_t >( it ) );
   }
}

template< typename InitValue >
bool tree_initializer< InitValue >::is_inner() const {
   return is_inner_tree_node_;
}

template< typename InitValue >
InitValue const & tree_initializer< InitValue >::value() const {
   return value_;
}

template< typename InitValue >
typename tree_initializer< InitValue >::subtreectr_t::const_iterator
tree_initializer< InitValue >::cbegin() const {
   return subtrees_.cbegin();
}

template< typename InitValue >
typename tree_initializer< InitValue >::subtreectr_t::const_iterator
tree_initializer< InitValue >::cend() const {
   return subtrees_.cend();
}

} // namespace internal

// ======================================================================
// === Tree

namespace internal {
// == Iterators
// = const_iterator_depth_first

template< typename Value >
tree_const_iterator_depth_first< Value >::tree_const_iterator_depth_first(
   tree_sp< Value > const & t )
   : root_root_tree_( { t } ) {
   std::cerr << "tcidf begin()" << std::endl;
   stack_.push( iter_data( root_root_tree_.cbegin(),
                           root_root_tree_.cend() ) );
   go_depth( *root_root_tree_.begin() );
}

template< typename Value >
tree_const_iterator_depth_first< Value >::tree_const_iterator_depth_first(
   tree_sp< Value > const & t, int )
   : root_root_tree_( { t } ) {
   std::cerr << "tcidf end()" << std::endl;
#if 0
   stack_.push( iter_data( root_root_tree_.cend(),
                           root_root_tree_.cend() ) );
#endif
}

template< typename Value >
void tree_const_iterator_depth_first< Value >::operator++() {
   if( stack_.empty() ) { abort(); }

   std::cerr << "tcidf operator++: " << stack_.size() << std::endl;

   // Are there any nodes left on the same level?
   if( stack_.top().current != stack_.top().end ) {
      ++stack_.top().current;

      if( stack_.top().current == stack_.top().end ) {
         stack_.pop();
         std::cerr << "tcidf operator++: pop out to " << stack_.size() << std::endl;
         return;
      }
      go_depth( *stack_.top().current );
      return;
   }

   std::cerr << "tree_const_iterator_depth_first< Value >"
      "::operator++(): failure" << std::endl;
   abort();
}

template< typename Value >
bool tree_const_iterator_depth_first< Value >::operator!=(
   tree_const_iterator_depth_first const & that ) const {

   if( stack_.size() == 0 and that.stack_.size() == 0 ) {
      return false;
   } else if( stack_.size() == 0 or that.stack_.size() == 0 ) {
      return true;
   }

   std::cout << "op!=" << stack_.size() << "/"
             << that.stack_.size() << std::endl;
   std::cout << "op!=" << ( stack_.size() != that.stack_.size() ) << std::endl;
   std::cout << "op!=" << ( stack_.top().current != that.stack_.top().current ) << std::endl;

   return stack_.size() != that.stack_.size()
      or stack_.top().current != that.stack_.top().current;
}

template< typename Value >
void tree_const_iterator_depth_first< Value >::go_depth(
   tree_sp< Value > const & tree ) {
   // Either a fat interface or a dynamic cast can be used to
   // get the next steps which must be done.
   // Here the dynamic cast solution is used.

   std::shared_ptr< leaf_tree< Value > > const ln(
      std::dynamic_pointer_cast< leaf_tree< Value > >( tree ) );
   if( ln ) {
      return;
   }

   std::shared_ptr< inner_tree< Value > > const cn(
      std::dynamic_pointer_cast< inner_tree< Value > >( tree ) );
   if( cn ) {
      stack_.push( iter_data( cn->cbegin(), cn->cend() ) );
      go_depth( * cn->cbegin() );
      return;
   }

   // Something very strange happend
   std::cerr << "tree_const_iterator_depth_first< Value >::go_depth"
             << std::endl;
   abort();
}

template< typename Value >
Value const & tree_const_iterator_depth_first< Value >::operator*() const {
   std::cerr << "operator*()" << std::endl;
   return (*stack_.top().current)->value();
}

// == Tree internals
template< typename Value >
inner_tree< Value >::inner_tree( Value const & iv )
: tree< Value >( iv ) {}

template< typename Value >
typename inner_tree< Value >::const_iterator
inner_tree< Value >::cbegin() const {
   return nodes_.cbegin();
}

template< typename Value >
typename inner_tree< Value >::const_iterator
inner_tree< Value >::cend() const {
   return nodes_.cend();
}


template< typename Value >
leaf_tree< Value >::leaf_tree( Value const & l )
: tree< Value >( l ) {}

}

template< typename Value >
tree< Value >::tree( Value const & v )
: value_( v ) {}

template< typename Value >
Value const & tree< Value >::value() const {
   return value_;
}

template< typename Value >
typename tree< Value >::const_iterator_depth_first
tree< Value >::cbegin_depth_first() {
   return internal::tree_const_iterator_depth_first< Value >(
      this->shared_from_this() );
}

template< typename Value >
typename tree< Value >::const_iterator_depth_first
tree< Value >::cend_depth_first() {
   return internal::tree_const_iterator_depth_first< Value >(
      this->shared_from_this(), 1);
}

template< typename Value >
tree< Value >::~tree() {}

// ======================================================================
// === Tree Utilities

// === make_tree

namespace internal {
// == Helper
template< typename Value, typename InitValue = Value >
tree_sp< Value >
make_inner_tree( tree_initializer< InitValue > const & ti );

template< typename Value, typename InitValue = Value >
tree_sp< Value >
make_leaf_tree( tree_initializer< InitValue > const & ti );

// == Implementation

template< typename Value, typename InitValue >
tree_sp< Value >
make_inner_tree( tree_initializer< InitValue > const & ti ) {
   inner_tree_sp< Value > rtree(
      std::make_shared< inner_tree< Value > >( ti.value() ) );
   for( auto it( ti.cbegin() ); it != ti.cend(); ++it ) {
      rtree->push_back( make_tree< Value, InitValue >( **it ) );
   }
   return rtree;
}

template< typename Value, typename InitValue >
tree_sp< Value >
make_leaf_tree( tree_initializer< InitValue > const & ti ) {
   return std::make_shared< leaf_tree< Value > >( ti.value() );
}

} // namespace internal

template< typename Value, typename InitValue >
tree_sp< Value >
make_tree( internal::tree_initializer< InitValue > const & ti ) {
   return ti.is_inner()
      ? internal::make_inner_tree< Value, InitValue >( ti )
      : internal::make_leaf_tree< Value, InitValue >( ti );
}

}

#endif
