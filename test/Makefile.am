AM_CPPFLAGS = -I$(top_srcdir)/ \
	-I$(top_srcdir)/tools/ \
	$(GTEST_CFLAGS) \
	$(GMOCK_CFLAGS)
AM_CXXFLAGS = \
	$(GTEST_MAIN_CFLAGS) \
	$(SDBUSPLUS_CFLAGS) \
	$(PHOSPHOR_LOGGING_CFLAGS)
AM_LDFLAGS = \
	$(GMOCK_LIBS) \
	$(GTEST_MAIN_LIBS) \
	$(OESDK_TESTCASE_FLAGS) \
	$(SDBUSPLUS_LIBS) \
	$(PHOSPHOR_LOGGING_LIBS)

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
	firmware_delete_unittest \
	firmware_sessionstat_unittest \
	firmware_commit_unittest \
	file_handler_unittest \
	tools_blob_unittest \
	tools_updater_unittest

TESTS = $(check_PROGRAMS)

firmware_createhandler_unittest_SOURCES = firmware_createhandler_unittest.cpp
firmware_createhandler_unittest_LDADD = $(top_builddir)/firmware_handler.o

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

firmware_close_unittest_SOURCES = firmware_close_unittest.cpp
firmware_close_unittest_LDADD = $(top_builddir)/firmware_handler.o

firmware_delete_unittest_SOURCES = firmware_delete_unittest.cpp
firmware_delete_unittest_LDADD = $(top_builddir)/firmware_handler.o

firmware_sessionstat_unittest_SOURCES = firmware_sessionstat_unittest.cpp
firmware_sessionstat_unittest_LDADD = $(top_builddir)/firmware_handler.o

firmware_commit_unittest_SOURCES = firmware_commit_unittest.cpp
firmware_commit_unittest_LDADD = $(top_builddir)/firmware_handler.o

file_handler_unittest_SOURCES = file_handler_unittest.cpp
file_handler_unittest_LDADD = $(top_builddir)/file_handler.o -lstdc++fs

tools_blob_unittest_SOURCES = tools_blob_unittest.cpp
tools_blob_unittest_LDADD = $(top_builddir)/tools/blob_handler.o

tools_updater_unittest_SOURCES = tools_updater_unittest.cpp
tools_updater_unittest_LDADD = $(top_builddir)/tools/updater.o
