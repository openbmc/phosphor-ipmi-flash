AM_CPPFLAGS = -I$(top_srcdir)/
	$(GTEST_CFLAGS) \
	$(GMOCK_CFLAGS)
AM_CXXFLAGS = \
	$(GTEST_MAIN_CFLAGS)
AM_LDFLAGS = \
	$(GMOCK_LIBS) \
	$(GTEST_MAIN_LIBS)

# Run all 'check' test programs
check_PROGRAMS = \
	blob_unittest

TESTS = $(check_PROGRAMS)

blob_unittest_SOURCES = blob_unittest.cpp
blob_unittest_LDADD = $(top_builddir)/blob_handler.o
