AM_CPPFLAGS = -I$(top_srcdir)/ \
	$(GTEST_CFLAGS) \
	$(GMOCK_CFLAGS)
AM_CXXFLAGS = \
	$(GTEST_MAIN_CFLAGS)
AM_LDFLAGS = \
	$(GMOCK_LIBS) \
	$(GTEST_MAIN_LIBS) \
	$(OESDK_TESTCASE_FLAGS)

# Run all 'check' test programs
check_PROGRAMS = \
	firmware_handler \
	firmware_stat

TESTS = $(check_PROGRAMS)

firmware_handler_SOURCES = firmware_handler_unittest.cpp
firmware_handler_LDADD = $(top_builddir)/firmware_handler.o

firmware_stat_SOURCES = firmware_stat_unittest.cpp
firmware_stat_LDADD = $(top_builddir)/firmware_handler.o

