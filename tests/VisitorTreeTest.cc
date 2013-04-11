#include <ctl/tree.hh>
#include <ptl/visitor.hh>
#include <domain/expression/expression.hh>
#include <domain/generic/print.hh>

#include <iostream>

using namespace ctl;
using namespace ptl;
using namespace domain::expression;

#if 0
ExprTree build_tree() {

   // Build up the (example) tree
   ExprTree const l1( std::make_shared< LeafNode >( -5 ) );
   ExprTree const l2( std::make_shared< LeafNode >(  3 ) );
   ExprTree const l3( std::make_shared< LeafNode >(  4 ) );

   ExprTree const add34(
      std::make_shared< AddNode >(),
      ExprTree::initializer_list( { l2, l3 } ) );

   ExprTree const et(
      std::make_shared< MulNode >(),
      ExprTree::initializer_list( { l1, add34 } ) );

   return et;
}
#endif

#if 0
using PostFixPrint = PTL::Visitor::Combiner< ActionPrint, ExprTree >;
using StackEval = PTL::Visitor::Combiner< ExpressionStackEval, ExprTree >;
#endif

int main() {

// ToDo: Extend the make_tree function that it is able to handle
//       two different classes for initialization.
//       make_tree< Node, int, InnerNode, char, LeafNode >

   tree_sp< TstTreeNode > const itree(
      ctl::make_tree< TstTreeNode, int >(
         { '+', { 1, 2, { '*', { 3, 4, 5 } } } } ) );


   ExprTree et( build_tree() );

   PTL::Visitor::Visitor< ExprTree, PostFixPrint >::accept< void >(
      et.iterator_deep_first_begin(),
      et.iterator_deep_first_end(), std::cout );

   std::cout << "Result ["
             << PTL::Visitor::Visitor<
                ExprTree, StackEval >::accept< long >(
                   et.iterator_deep_first_begin(),
                   et.iterator_deep_first_end() )
             << "]" << std::endl;

   return 0;
}
