#ifndef PTL_CTRB_EXPRESSION_HH
#define PTL_CTRB_EXPRESSION_HH

#include <memory>
#include <string>
#include <list>

namespace domain { namespace expression {

// ToDo: Move this to the Visitor
class Visitable;

class Node {
public:
   virtual ~Node() {}
   virtual std::string to_string() const = 0;
   virtual void dispatch( Visitable & e ) = 0;
//   virtual void dispatch( Visitable const & e ) = 0;
};

using Node_sp = std::shared_ptr< Node >;

template< typename Value_t >
class BaseLeafNode : public Node {
public:
   BaseLeafNode( Value_t const & v ) : m_value( v ) {}
   ~BaseLeafNode() {}
   std::string to_string() const {
      return "{" + std::to_string( m_value ) + "}"; }

   virtual void dispatch( Visitable & e );
//   virtual void dispatch( Visitable const & e );

   Value_t const & value() const { return m_value; }

private:
   Value_t m_value;
};

template< char OP >
class OpNode : public Node {
public:
   std::string to_string() const { return std::string() + OP; }
   virtual void dispatch( Visitable & e );
//   virtual void dispatch( Visitable const & e );
};

using LeafNode = BaseLeafNode< long >;
using AddNode = OpNode< '+' >;
using SubNode = OpNode< '-' >;
using DivNode = OpNode< '/' >;
using MulNode = OpNode< '*' >;

class Visitable {
public:
   virtual ~Visitable() {}
   virtual void visit( LeafNode & n ) = 0;
   virtual void visit( AddNode & n ) = 0;
   virtual void visit( SubNode & n ) = 0;
   virtual void visit( DivNode & n ) = 0;
   virtual void visit( MulNode & n ) = 0;
};

class ActionPrint : public Visitable {
public:
   ActionPrint( std::ostream & out )
      : m_out( out ) {}
   ~ActionPrint() {}

   void visit( LeafNode & n ) { m_out << n.to_string(); }
   void visit( AddNode & n ) { m_out << n.to_string(); }
   void visit( SubNode & n ) { m_out << n.to_string(); }
   void visit( DivNode & n ) { m_out << n.to_string(); }
   void visit( MulNode & n ) { m_out << n.to_string(); }

   // First level of dispatching
   void visit( Node_sp const & node ) {
      node->dispatch( *this );
   }

#if 0
   void visit( Node const & node ) const {
      node.dispatch( *this );
   }
#endif

   void result() const {}
private:
   std::ostream & m_out;
};

// Second level of dispatching
// Now the dispatcher functions can be implemented:
template<>
void LeafNode::dispatch( Visitable & e ) { e.visit( *this ); }
#if 0
template<>
void LeafNode::dispatch( Visitable const & e ) { e.visit( *this ); }
#endif
template< char OP >
void OpNode<OP>::dispatch( Visitable & e ) { e.visit( *this ); }


}}

#endif
