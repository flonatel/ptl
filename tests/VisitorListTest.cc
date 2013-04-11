#include <domain/expression/expression.hh>
#include <domain/expression/stack_eval.hh>
#include <ptl/visitor.hh>

#include <iostream>

using namespace ptl;
using namespace domain::expression;

using NodeList = std::list< Node_sp >;

using PostFixPrint = ptl::visitor::combiner< ActionPrint, NodeList >;
using StackEval = ptl::visitor::combiner< stack_eval, NodeList >;

// ToDo: Write it as { -5, 3, 4, +, * }

NodeList build_list() {
   NodeList rval;

   // Build up the (example) list
   rval.emplace_back( std::make_shared< LeafNode >( -5 ) );
   rval.emplace_back( std::make_shared< LeafNode >(  3 ) );
   rval.emplace_back( std::make_shared< LeafNode >(  4 ) );
   rval.emplace_back( std::make_shared< AddNode >() );
   rval.emplace_back( std::make_shared< MulNode >() );

   return rval;
}

int main() {
   using namespace ptl::visitor;

   NodeList el( build_list() );

   ptl::visitor::visitor< NodeList, PostFixPrint >::accept< void >(
      el.begin(), el.end(), std::cout );

   std::cout << "Result ["
             << ptl::visitor::visitor< NodeList, StackEval >
                    ::accept< long >( el.begin(), el.end() )
             << "]" << std::endl;

   return 0;
}
