#include <ctl/tree.hh>
#include <ptl/visitor.hh>
#include <domain/expression/expression.hh>
#include <domain/generic/print.hh>

#include <gtest/gtest.h>
#include <iostream>
#include <sstream>

class IntTreeTest : public ::testing::Test {
public:

   void test_expr1();
   void test_expr_node1();
};

using namespace ctl;
using namespace ptl;
using namespace domain::expression;

// ToDo: Visitable is two times included in the template:
//       once for the combiner and once for the visitor.

TEST_F(IntTreeTest, test_expr1) {

   using ttree_sp = tree_sp< int > ;

   ttree_sp const itree(
      ctl::make_tree< int >(
         { '+', { 1, 2, { '*', { 3, 4, 5 } } } } ) );

   using PostFixPrint = ptl::visitor::combiner<
      domain::generic::print< int >, ttree_sp >;

   std::stringstream sstream;

   ptl::visitor::visitor< ttree_sp, PostFixPrint >::accept< void >(
      itree->cbegin_depth_first(),
      itree->cend_depth_first(), sstream );

   std::cout << sstream.str() << std::endl;
}

// === Test with self-made Tree Node Types

class TstTreeNode {
public:
   TstTreeNode( int v )
   : m_data( v ) {}

   std::ostream & print( std::ostream & out ) const {
      out << "[TN: " << m_data << "]";
      return out;
   }

private:
   int m_data;
};

std::ostream & operator<<( std::ostream & out, TstTreeNode const & tn ) {
   return tn.print( out );
}

TEST_F(IntTreeTest, test_expr_node1) {

   using ttree_sp = tree_sp< TstTreeNode > ;

   tree_sp< TstTreeNode > const itree(
      ctl::make_tree< TstTreeNode, int >(
         { '+', { 1, 2, { '*', { 3, 4, 5 } } } } ) );

   using PostFixPrint = ptl::visitor::combiner<
      domain::generic::print< TstTreeNode >, ttree_sp >;

   std::stringstream sstream;

   ptl::visitor::visitor< ttree_sp, PostFixPrint >::accept< void >(
      itree->cbegin_depth_first(),
      itree->cend_depth_first(), sstream );

   std::cout << sstream.str() << std::endl;

}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
