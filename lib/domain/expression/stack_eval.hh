#ifndef PTL_CTRB_EXPRESSION_STACK_EVAL_HH
#define PTL_CTRB_EXPRESSION_STACK_EVAL_HH

#include "domain/expression/expression.hh"

#include <stack>

namespace domain { namespace expression {

class stack_eval : public Visitable {
public:
   stack_eval() {}
   ~stack_eval() {}

   void visit( LeafNode & n ) {
      m_stack.push( n.value() );
   }

   void visit( AddNode & ) {
      execute_op( [](int a, int b){ return a + b; } );
   }

   void visit( SubNode & ) {
      execute_op( [](int a, int b){ return a - b; } );
   }

   void visit( DivNode & ) {
      execute_op( [](int a, int b){ return a / b; } );
   }

   void visit( MulNode & ) {
      execute_op( [](int a, int b){ return a * b; } );
   }

   // First level of dispatching
   void visit( Node_sp const & node ) { node->dispatch( *this ); }
//   void visit( Node_sp const & node ) const { node->dispatch( *this ); }

   long result() const { return m_stack.top(); }

private:
   void execute_op( std::function< long(long, long) > f ) {
      long const t1( m_stack.top() );
      m_stack.pop();
      long const t2( m_stack.top() );
      m_stack.pop();
      m_stack.push( f(t1, t2 ) );
   }

   std::stack< long > m_stack;
};


}}

#endif
