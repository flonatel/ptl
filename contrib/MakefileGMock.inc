noinst_LTLIBRARIES += contrib/gmock/lib/libgmock.la \
        contrib/gmock/lib/libgtest.la

contrib_gmock_lib_libgmock_la_SOURCES = \
        contrib/gmock/src/gmock-all.cc

contrib_gmock_lib_libgmock_la_CPPFLAGS = \
        -I$(top_srcdir)/${GOOGLE_MOCK_BASE} \
        -I$(top_srcdir)/${GOOGLE_MOCK_INCLUDE} \
        -I$(top_srcdir)/${GOOGLE_TEST_INCLUDE}


contrib_gmock_lib_libgtest_la_SOURCES = \
        contrib/gmock/gtest/src/gtest-all.cc

contrib_gmock_lib_libgtest_la_CPPFLAGS = \
        -I$(top_srcdir)/${GOOGLE_TEST_BASE} \
        -I$(top_srcdir)/${GOOGLE_TEST_INCLUDE}

# Local Variables:
# mode: makefile
# End:
