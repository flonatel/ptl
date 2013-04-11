
# noinst_PROGRAMS += tests/PTL/VisitorTreeTest

#TESTS += tests/PTL/VisitorTreeTest

#tests_PTL_VisitorTreeTest_SOURCES = \
#	tests/VisitorTreeTest.cc

#tests_PTL_VisitorTreeTest_CPPFLAGS = \
#        -I$(top_srcdir)/${GOOGLE_TEST_INCLUDE} \
#        -I$(top_srcdir)/lib

#tests_PTL_VisitorTreeTest_LDADD = \
#        contrib/gmock/lib/libgtest.la

noinst_PROGRAMS += tests/PTL/VisitorListTest

TESTS += tests/PTL/VisitorListTest

tests_PTL_VisitorListTest_SOURCES = \
	tests/VisitorListTest.cc

tests_PTL_VisitorListTest_CPPFLAGS = \
        -I$(top_srcdir)/${GOOGLE_TEST_INCLUDE} \
        -I$(top_srcdir)/lib

tests_PTL_VisitorListTest_LDADD = \
        contrib/gmock/lib/libgtest.la


noinst_PROGRAMS += tests/PTL/IntTreeTest

TESTS += tests/PTL/IntTreeTest

tests_PTL_IntTreeTest_SOURCES = \
	tests/IntTreeTest.cc

tests_PTL_IntTreeTest_CPPFLAGS = \
        -I$(top_srcdir)/${GOOGLE_TEST_INCLUDE} \
        -I$(top_srcdir)/lib

tests_PTL_IntTreeTest_LDADD = \
        contrib/gmock/lib/libgtest.la

# Local Variables:
# mode: makefile
# End:
