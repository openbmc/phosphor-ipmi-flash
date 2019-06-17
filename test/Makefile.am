@VALGRIND_CHECK_RULES@

AM_CPPFLAGS = \
	-I$(top_srcdir)/ \
	-I$(top_srcdir)/tools/ \
	$(GTEST_CFLAGS) \
	$(GMOCK_CFLAGS) \
	$(CODE_COVERAGE_CPPFLAGS)
AM_CXXFLAGS = \
	$(SDBUSPLUS_CFLAGS) \
	$(PHOSPHOR_LOGGING_CFLAGS) \
	$(CODE_COVERAGE_CXXFLAGS)
AM_LDFLAGS = \
	$(GTEST_LIBS) \
	$(GMOCK_LIBS) \
	-lgmock_main \
	$(OESDK_TESTCASE_FLAGS) \
	$(SDBUSPLUS_LIBS) \
	$(PHOSPHOR_LOGGING_LIBS) \
	$(CODE_COVERAGE_LIBS)

# Run all 'check' test programs
check_PROGRAMS = \
	firmware_createhandler_unittest \
	firmware_handler_unittest \
	firmware_stat_unittest \
	firmware_canhandle_unittest \
	firmware_open_unittest \
	firmware_write_unittest \
	firmware_writemeta_unittest \
	firmware_close_unittest \
	firmware_sessionstat_unittest \
	firmware_commit_unittest \
	file_handler_unittest \
	firmware_state_notyetstarted_unittest \
	firmware_state_uploadinprogress_unittest \
	firmware_state_verificationpending_unittest \
	firmware_state_verificationstarted_unittest \
	firmware_state_verificationcompleted_unittest \
	firmware_state_updatepending_unittest \
	firmware_state_updatestarted_unittest \
	firmware_state_updatecompleted_unittest \
	firmware_state_notyetstarted_tarball_unittest

if BUILD_HOST_TOOL
check_PROGRAMS += \
	tools_bt_unittest \
	tools_lpc_unittest \
	tools_updater_unittest
endif

TESTS = $(check_PROGRAMS)

firmware_createhandler_unittest_SOURCES = firmware_createhandler_unittest.cpp
firmware_createhandler_unittest_LDADD = $(top_builddir)/libfirmwareblob_common.la

firmware_handler_unittest_SOURCES = firmware_handler_unittest.cpp
firmware_handler_unittest_LDADD = $(top_builddir)/libfirmwareblob_common.la

firmware_stat_unittest_SOURCES = firmware_stat_unittest.cpp
firmware_stat_unittest_LDADD = $(top_builddir)/libfirmwareblob_common.la

firmware_canhandle_unittest_SOURCES = firmware_canhandle_unittest.cpp
firmware_canhandle_unittest_LDADD = $(top_builddir)/libfirmwareblob_common.la

firmware_open_unittest_SOURCES = firmware_open_unittest.cpp
firmware_open_unittest_LDADD = $(top_builddir)/libfirmwareblob_common.la

firmware_write_unittest_SOURCES = firmware_write_unittest.cpp
firmware_write_unittest_LDADD = $(top_builddir)/libfirmwareblob_common.la

firmware_writemeta_unittest_SOURCES = firmware_writemeta_unittest.cpp
firmware_writemeta_unittest_LDADD = $(top_builddir)/libfirmwareblob_common.la

firmware_close_unittest_SOURCES = firmware_close_unittest.cpp
firmware_close_unittest_LDADD = $(top_builddir)/libfirmwareblob_common.la

firmware_sessionstat_unittest_SOURCES = firmware_sessionstat_unittest.cpp
firmware_sessionstat_unittest_LDADD = $(top_builddir)/libfirmwareblob_common.la

firmware_commit_unittest_SOURCES = firmware_commit_unittest.cpp
firmware_commit_unittest_LDADD = $(top_builddir)/libfirmwareblob_common.la

file_handler_unittest_SOURCES = file_handler_unittest.cpp
file_handler_unittest_LDADD = $(top_builddir)/libfirmwareblob_common.la -lstdc++fs

firmware_state_notyetstarted_unittest_SOURCES = firmware_state_notyetstarted_unittest.cpp
firmware_state_notyetstarted_unittest_LDADD = $(top_builddir)/libfirmwareblob_common.la

firmware_state_uploadinprogress_unittest_SOURCES = firmware_state_uploadinprogress_unittest.cpp
firmware_state_uploadinprogress_unittest_LDADD = $(top_builddir)/libfirmwareblob_common.la

firmware_state_verificationpending_unittest_SOURCES = firmware_state_verificationpending_unittest.cpp
firmware_state_verificationpending_unittest_LDADD = $(top_builddir)/libfirmwareblob_common.la

firmware_state_verificationstarted_unittest_SOURCES = firmware_state_verificationstarted_unittest.cpp
firmware_state_verificationstarted_unittest_LDADD = $(top_builddir)/libfirmwareblob_common.la

firmware_state_verificationcompleted_unittest_SOURCES = firmware_state_verificationcompleted_unittest.cpp
firmware_state_verificationcompleted_unittest_LDADD = $(top_builddir)/libfirmwareblob_common.la

firmware_state_updatepending_unittest_SOURCES = firmware_state_updatepending_unittest.cpp
firmware_state_updatepending_unittest_LDADD = $(top_builddir)/libfirmwareblob_common.la

firmware_state_updatestarted_unittest_SOURCES = firmware_state_updatestarted_unittest.cpp
firmware_state_updatestarted_unittest_LDADD = $(top_builddir)/libfirmwareblob_common.la

firmware_state_updatecompleted_unittest_SOURCES = firmware_state_updatecompleted_unittest.cpp
firmware_state_updatecompleted_unittest_LDADD = $(top_builddir)/libfirmwareblob_common.la

firmware_state_notyetstarted_tarball_unittest_SOURCES = firmware_state_notyetstarted_tarball_unittest.cpp
firmware_state_notyetstarted_tarball_unittest_LDADD = $(top_builddir)/libfirmwareblob_common.la

if BUILD_HOST_TOOL
tools_bt_unittest_SOURCES = tools_bt_unittest.cpp
tools_bt_unittest_LDADD = $(top_builddir)/tools/libupdater.la

tools_lpc_unittest_SOURCES = tools_lpc_unittest.cpp
tools_lpc_unittest_LDADD = $(top_builddir)/tools/libupdater.la

tools_updater_unittest_SOURCES = tools_updater_unittest.cpp
tools_updater_unittest_LDADD = $(top_builddir)/tools/libupdater.la
endif
