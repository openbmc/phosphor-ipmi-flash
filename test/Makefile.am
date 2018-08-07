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
	ipmi_starttransfer_unittest \
	ipmi_flashdata_unittest \
	ipmi_flashfinish_unittest \
	ipmi_starthash_unittest \
	ipmi_hashdata_unittest \
	ipmi_hashfinish_unittest \
	ipmi_startverify_unittest \
	ipmi_abort_unittest \
	ipmi_validate_unittest \
	ipmi_command_unittest \
	flash_start_unittest \
	flash_flashdata_unittest \
	flash_flashfinish_unittest \
	flash_hashstart_unittest \
	flash_hashdata_unittest \
	flash_hashfinish_unittest

TESTS = $(check_PROGRAMS)

ipmi_starttransfer_unittest_SOURCES = ipmi_starttransfer_unittest.cpp
ipmi_starttransfer_unittest_LDADD = $(top_builddir)/ipmi.o

ipmi_flashdata_unittest_SOURCES = ipmi_flashdata_unittest.cpp
ipmi_flashdata_unittest_LDADD = $(top_builddir)/ipmi.o

ipmi_flashfinish_unittest_SOURCES = ipmi_flashfinish_unittest.cpp
ipmi_flashfinish_unittest_LDADD = $(top_builddir)/ipmi.o

ipmi_starthash_unittest_SOURCES = ipmi_starthash_unittest.cpp
ipmi_starthash_unittest_LDADD = $(top_builddir)/ipmi.o

ipmi_hashdata_unittest_SOURCES = ipmi_hashdata_unittest.cpp
ipmi_hashdata_unittest_LDADD = $(top_builddir)/ipmi.o

ipmi_hashfinish_unittest_SOURCES = ipmi_hashfinish_unittest.cpp
ipmi_hashfinish_unittest_LDADD = $(top_builddir)/ipmi.o

ipmi_startverify_unittest_SOURCES = ipmi_startverify_unittest.cpp
ipmi_startverify_unittest_LDADD = $(top_builddir)/ipmi.o

ipmi_abort_unittest_SOURCES = ipmi_abort_unittest.cpp
ipmi_abort_unittest_LDADD = $(top_builddir)/ipmi.o

ipmi_validate_unittest_SOURCES = ipmi_validate_unittest.cpp
ipmi_validate_unittest_LDADD = $(top_builddir)/ipmi.o

ipmi_command_unittest_SOURCES = ipmi_command_unittest.cpp
ipmi_command_unittest_LDADD = $(top_builddir)/ipmi.o

flash_start_unittest_SOURCES = flash_start_unittest.cpp
flash_start_unittest_LDADD = $(top_builddir)/flash-ipmi.o

flash_flashdata_unittest_SOURCES = flash_flashdata_unittest.cpp
flash_flashdata_unittest_LDADD = $(top_builddir)/flash-ipmi.o

flash_flashfinish_unittest_SOURCES = flash_flashfinish_unittest.cpp
flash_flashfinish_unittest_LDADD = $(top_builddir)/flash-ipmi.o

flash_hashstart_unittest_SOURCES = flash_hashstart_unittest.cpp
flash_hashstart_unittest_LDADD = $(top_builddir)/flash-ipmi.o

flash_hashdata_unittest_SOURCES = flash_hashdata_unittest.cpp
flash_hashdata_unittest_LDADD = $(top_builddir)/flash-ipmi.o

flash_hashfinish_unittest_SOURCES = flash_hashfinish_unittest.cpp
flash_hashfinish_unittest_LDADD = $(top_builddir)/flash-ipmi.o
