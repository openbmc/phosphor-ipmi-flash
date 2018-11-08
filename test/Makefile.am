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
	firmware_handler_unittest \
	firmware_stat_unittest \
	firmware_canhandle_unittest \
	firmware_open_unittest \
	firmware_write_unittest \
	firmware_writemeta_unittest

TESTS = $(check_PROGRAMS)

firmware_handler_unittest_SOURCES = firmware_handler_unittest.cpp
firmware_handler_unittest_LDADD = $(top_builddir)/firmware_handler.o

firmware_stat_unittest_SOURCES = firmware_stat_unittest.cpp
firmware_stat_unittest_LDADD = $(top_builddir)/firmware_handler.o

firmware_canhandle_unittest_SOURCES = firmware_canhandle_unittest.cpp
firmware_canhandle_unittest_LDADD = $(top_builddir)/firmware_handler.o

firmware_open_unittest_SOURCES = firmware_open_unittest.cpp
firmware_open_unittest_LDADD = $(top_builddir)/firmware_handler.o

firmware_write_unittest_SOURCES = firmware_write_unittest.cpp
firmware_write_unittest_LDADD = $(top_builddir)/firmware_handler.o

firmware_writemeta_unittest_SOURCES = firmware_writemeta_unittest.cpp
firmware_writemeta_unittest_LDADD = $(top_builddir)/firmware_handler.o
