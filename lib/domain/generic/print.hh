#ifndef DOMAIN_GENERIC_PRINT_HH
#define DOMAIN_GENERIC_PRINT_HH

namespace domain { namespace generic {

template< typename T >
class print {
public:
   print( std::ostream & out )
      : out_( out ) {}

   void visit( T const & node );
   void result() const {}
private:
   std::ostream & out_;
};

template< typename T >
void print< T >::visit( T const & v ) {
   out_ << "{" << v << "}";
}

}}

#endif
