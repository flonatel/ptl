#include <ptl/observer.hh>

#include <gtest/gtest.h>

class ObserverTest : public ::testing::Test {
public:
   void test_register_notify();
};

class A {
public:
   A() : m_str( "<unset>" ), m_i( 21 ) {}

   using callback_type = void(A &, std::string const &, int);

   void call_me(std::string const & str, int i) {
      m_str = str; m_i = i;
   }

   std::string const & get_string() const { return m_str; }
   int get_int() const { return m_i; }

private:
   std::string m_str;
   int m_i;
};


TEST_F(ObserverTest, test_register_notify) {

   ptl::observer::subject< A >  subject;
   A a;

   subject.register_observer( a, &A::call_me );
   subject.notify_observers( "Hello", 77 );

   ASSERT_EQ( a.get_string(), "Hello" );
   ASSERT_EQ( a.get_int(), 77 );
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
