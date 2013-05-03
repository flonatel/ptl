#include <ptl/observer.hh>

#include <gtest/gtest.h>

class ObserverTest : public ::testing::Test {
public:
   void test_register_notify();
   void test_register_notify_two_obj();
   void test_register_notify_two_methods();
};

class A {
public:
   A() : m_cm2_called( false ), m_str( "<unset>" ), m_i( 21 ) {}

   using callback_type = void(std::string const &, int);

   void call_me(std::string const & str, int i) {
      m_str = str; m_i = i;
   }

   void call_me2(std::string const &, int) {
      m_cm2_called = true;
   }

   std::string const & get_string() const { return m_str; }
   int get_int() const { return m_i; }
   bool cm2_called() const { return m_cm2_called; }

private:
   bool m_cm2_called;
   std::string m_str;
   int m_i;
};


TEST_F(ObserverTest, test_register_notify) {
   using namespace std::placeholders; // for _1, _2, _3...

   ptl::observer::subject< A::callback_type >  subject;
   A a;

   subject.register_observer( std::bind( &A::call_me, &a, _1, _2 ) );
   subject.notify_observers( "Hello", 77 );

   ASSERT_EQ( a.get_string(), "Hello" );
   ASSERT_EQ( a.get_int(), 77 );
   ASSERT_FALSE( a.cm2_called() );
}

TEST_F(ObserverTest, test_register_notify_two_obj) {
   using namespace std::placeholders; // for _1, _2, _3...

   ptl::observer::subject< A::callback_type >  subject;
   A a1;
   A a2;

   subject.register_observer( std::bind( &A::call_me, &a1, _1, _2 ) );
   subject.register_observer( std::bind( &A::call_me, &a2, _1, _2 ) );
   subject.notify_observers( "Hello", 77 );

   ASSERT_EQ( a1.get_string(), "Hello" );
   ASSERT_EQ( a1.get_int(), 77 );
   ASSERT_EQ( a2.get_string(), "Hello" );
   ASSERT_EQ( a2.get_int(), 77 );
}

TEST_F(ObserverTest, test_register_notify_two_methods) {
   using namespace std::placeholders; // for _1, _2, _3...

   ptl::observer::subject< A::callback_type >  subject;
   A a;

   subject.register_observer( std::bind( &A::call_me, &a, _1, _2 ) );
   subject.register_observer( std::bind( &A::call_me2, &a, _1, _2 ) );
   subject.notify_observers( "Hello", 77 );

   ASSERT_EQ( a.get_string(), "Hello" );
   ASSERT_EQ( a.get_int(), 77 );
   ASSERT_TRUE( a.cm2_called() );
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
