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
	ipmi_starttransfer_unittest

ipmi_starttransfer_unittest_SOURCES = ipmi_starttransfer_unittest.cpp
ipmi_starttransfer_unittest_LDADD = $(top_builddir)/ipmi.o
