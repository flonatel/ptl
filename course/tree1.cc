#include <vector>
#include <memory>
#include <iostream>
#include <stack>

#include <cstdlib> // for abort() - might be removed for production
#include <cassert>

// ================================================== Tree

class TreeNode {
public:
   virtual ~TreeNode() {}

   // Returns a string representation of exactly this node.
   virtual std::string to_string() const = 0;
};

using TreeNode_sp = std::shared_ptr< TreeNode >;
using TreeNodes_il = std::initializer_list< TreeNode_sp >;

// later..... template< typename LNType >
class LeafNode : public TreeNode {
public:
   LeafNode( long const & v ) : m_value( v ) {}

   long value() const { return m_value; }

   std::string to_string() const {
      return "{" + std::to_string( m_value ) + "}"; }

private:
   long m_value;
};

class CompositeNode : public TreeNode {
public:
   CompositeNode( TreeNodes_il il )
   : m_subnodes( il.begin(), il.end() ) {}

   using container = std::vector< TreeNode_sp >;
   using const_iterator = container::const_iterator;

   const_iterator cbegin() const { return m_subnodes.cbegin(); }
   const_iterator cend() const { return m_subnodes.cend(); }

   std::string to_string() const { return "<Helper CompositeNode>"; }

private:
   container m_subnodes;
};

using CompositeNode_sp = std::shared_ptr< CompositeNode >;

template< char OP >
class OpCompositeNode : public CompositeNode {
public:
   OpCompositeNode( TreeNodes_il il )
   : CompositeNode( il ) {}

   std::string to_string() const { return std::string() + op; }

private:
   char op = OP;
};

using CompositeAddNode = OpCompositeNode< '+' >;
using CompositeSubNode = OpCompositeNode< '-' >;
using CompositeDivNode = OpCompositeNode< '/' >;
using CompositeMulNode = OpCompositeNode< '*' >;

// ================================================== ActionPrint

class ActionPrint {
public:
   void apply( TreeNode_sp const & node, std::ostream & out ) {
      out << node->to_string();
   }

   void result() const {}
};

// ================================================== ExpressionTreeEval

class ExpressionTreeEval {
public:
   void apply( TreeNode_sp const & node ) {
      std::shared_ptr< LeafNode > const ln(
         std::dynamic_pointer_cast< LeafNode >( node ) );
      if( ln ) {
         m_stack.push( ln->value() );
         return;
      }

      // Implement the own logic of the different operators.
      std::shared_ptr< CompositeAddNode > const add(
         std::dynamic_pointer_cast< CompositeAddNode >( node ) );
      if( add ) {
         execute_op( [](int a, int b){ return a + b; } );
         return;
      }

      std::shared_ptr< CompositeMulNode > const mul(
         std::dynamic_pointer_cast< CompositeMulNode >( node ) );
      if( mul ) {
         execute_op( [](int a, int b){ return a * b; } );
         return;
      }

      // Something very strange happend
      std::cerr << "ExpressionTreeEval:: invalid node type ["
                << node->to_string() << "]"
                << std::endl;
      abort();
   }
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


// ================================================== DeepFirstWalker

class DeepFirstWalker {
public:
   DeepFirstWalker( TreeNode_sp const & node )
   : m_composite_node( TreeNodes_il( { node } ) ) {
      m_stack.push( iter_data( m_composite_node.cbegin(),
                               m_composite_node.cend() ) );
      go_deep( node );
   }

   bool end() const {
      return m_stack.empty();
   }

   // Todo: correct return type
   void operator++() {
      if( end() ) { abort(); }

      // Are there any nodes left on the same level?
      if( m_stack.top().current != m_stack.top().end ) {
         ++m_stack.top().current;

         if( m_stack.top().current == m_stack.top().end ) {
            m_stack.pop();
            return;
         }
         go_deep( *m_stack.top().current );
         return;
      }

      std::cerr << "DeepFirstWalker::++" << std::endl;
      abort();
   }

   TreeNode_sp const & operator*() const {
      return *m_stack.top().current;
   }

private:

   struct iter_data {
      iter_data( CompositeNode::const_iterator b,
                 CompositeNode::const_iterator e )
         : current( b ), end( e ) {}

      CompositeNode::const_iterator current;
      CompositeNode::const_iterator const end;
   };

   void go_deep( TreeNode_sp const & node ) {
      // Either a fat interface or a dynamic cast can be used to
      // get the next steps which must be done.
      // Here the dynamic cast solution is used.

      std::shared_ptr< LeafNode > const ln(
         std::dynamic_pointer_cast< LeafNode >( node ) );
      if( ln ) {
         return;
      }

      std::shared_ptr< CompositeNode > const cn(
         std::dynamic_pointer_cast< CompositeNode >( node ) );
      if( cn ) {
         m_stack.push( iter_data( cn->cbegin(), cn->cend() ) );
         go_deep( * cn->cbegin() );
         return;
      }

      // Something very strange happend
      std::cerr << "DeepFirstWalker::go_deep" << std::endl;
      abort();
   }

   CompositeNode m_composite_node;
   std::stack< iter_data > m_stack;
};

// ================================================== NodeFirstWalker

class NodeFirstWalker {
public:
   NodeFirstWalker( TreeNode_sp const & node )
   : m_composite_node( TreeNodes_il( { node } ) ) {
      m_stack.push( iter_data( m_composite_node.cbegin(),
                               m_composite_node.cend() ) );
   }

   bool end() const {
      return m_stack.empty();
   }

   void operator++() {
      assert( ! end() );
      TreeNode_sp const & cnode( *m_stack.top().current );

      // Up it goes....
      ++m_stack.top().current;
      while( ! m_stack.empty()
             && m_stack.top().current == m_stack.top().end ) {
         m_stack.pop();
      }

      std::shared_ptr< CompositeNode > const cn(
         std::dynamic_pointer_cast< CompositeNode >( cnode ) );
      if( cn ) {
         m_stack.push( iter_data( cn->cbegin(), cn->cend() ) );
         return;
      }
   }

   TreeNode_sp const & operator*() const {
      return *m_stack.top().current;
   }

private:

   struct iter_data {
      iter_data( CompositeNode::const_iterator b,
                 CompositeNode::const_iterator e )
         : current( b ), end( e ) {}

      CompositeNode::const_iterator current;
      CompositeNode::const_iterator const end;
   };

   CompositeNode m_composite_node;
   std::stack< iter_data > m_stack;
};

// ================================================== Combiner
// This combines the Walker and the Action and calls the
// action with all the single visited nodes.

template< typename Walker, typename Action >
class Combiner {
public:
   template< typename RetType, typename ... Args >
   static RetType apply ( TreeNode_sp const & et, Args && ... args ) {
      Action action;
      for( Walker walker( et ); ! walker.end(); ++walker ) {
         action.apply( *walker, std::forward< Args >( args ) ... );
      }
      return action.result();
   }
};

// ================================================== Combinations
// Some 'special' combinations of TreeWalker and Action
// for simple use.

using PostFixPrint = Combiner< DeepFirstWalker, ActionPrint >;
using PreFixPrint = Combiner< NodeFirstWalker, ActionPrint >;
using StackEval = Combiner< DeepFirstWalker, ExpressionTreeEval >;

// ================================================== Executor
// Executes the apply: forwarding to the combiner.

template< typename Combiner_t >
struct Executor {
   template< typename RetType, typename ... Args >
   static RetType apply( TreeNode_sp const & et, Args && ... args ) {
      return Combiner_t::template apply< RetType >(
         et, std::forward< Args >( args ) ... );
   }
};

// ==================================================

int main() {

   // Build up the (example) tree
   TreeNode_sp const l1(
      std::make_shared< LeafNode >( -5 ) );
   TreeNode_sp const l2(
      std::make_shared< LeafNode >(  3 ) );
   TreeNode_sp const l3(
      std::make_shared< LeafNode >(  4 ) );

   TreeNode_sp const add34(
      std::make_shared< CompositeAddNode >(
         TreeNodes_il( { l2, l3 } ) ) );

   TreeNode_sp const et(
      std::make_shared< CompositeMulNode >(
         TreeNodes_il( { l1, add34 } ) ) );

   std::cout << "PostFixPrint [";
   Executor< PostFixPrint >::apply< void >( et, std::cout );
   std::cout << "]" << std::endl;

   std::cout << "StackEval    ["
             << Executor< StackEval >::apply< long >( et )
             << "]" << std::endl;

   std::cout << "PreFixPrint  [";
   Executor< PreFixPrint >::apply< void >( et, std::cout );
   std::cout << "]" << std::endl;

}
