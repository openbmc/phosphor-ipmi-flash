@VALGRIND_CHECK_RULES@

AM_CPPFLAGS = \
	-I$(top_srcdir)/ \
	-I$(top_srcdir)/bmc/ \
	-I$(top_srcdir)/bmc/test \
	-I$(top_srcdir)/bmc/version-handler \
	$(GTEST_CFLAGS) \
	$(GMOCK_CFLAGS) \
	$(CODE_COVERAGE_CPPFLAGS)
AM_CXXFLAGS = \
	$(CODE_COVERAGE_CXXFLAGS)
AM_LDFLAGS = \
	$(GTEST_LIBS) \
	$(GMOCK_LIBS) \
	-lgmock_main \
	$(OESDK_TESTCASE_FLAGS) \
	$(CODE_COVERAGE_LIBS)

# Run all 'check' test programs
check_PROGRAMS = \
	version_json_unittest \
	version_canhandle_enumerate_unittest \
	version_createhandler_unittest \
	version_open_unittest \
	version_close_unittest \
	version_read_unittest

TESTS = $(check_PROGRAMS)

version_json_unittest_SOURCES = version_json_unittest.cpp
version_json_unittest_LDADD = $(top_builddir)/bmc/version-handler/libversionblob_common.la

version_canhandle_enumerate_unittest_SOURCES = version_canhandle_enumerate_unittest.cpp
version_canhandle_enumerate_unittest_LDADD = $(top_builddir)/bmc/version-handler/libversionblob_common.la

version_createhandler_unittest_SOURCES = version_createhandler_unittest.cpp
version_createhandler_unittest_LDADD = $(top_builddir)/bmc/version-handler/libversionblob_common.la

version_open_unittest_SOURCES = version_open_unittest.cpp
version_open_unittest_LDADD = $(top_builddir)/bmc/version-handler/libversionblob_common.la

version_close_unittest_SOURCES = version_close_unittest.cpp
version_close_unittest_LDADD = $(top_builddir)/bmc/version-handler/libversionblob_common.la

version_read_unittest_SOURCES = version_read_unittest.cpp
version_read_unittest_LDADD = $(top_builddir)/bmc/version-handler/libversionblob_common.la
