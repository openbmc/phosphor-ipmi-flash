noinst_PROGRAMS = burn_my_bmc

burn_my_bmc_SOURCES = main.cpp
burn_my_bmc_LDADD = libupdater.la $(CODE_COVERAGE_LIBS)
burn_my_bmc_CXXFLAGS = -I$(top_srcdir) $(CODE_COVERAGE_CXXFLAGS)

noinst_LTLIBRARIES = libupdater.la
libupdater_la_LDFLAGS = -static $(CODE_COVERAGE_LIBS) $(IPMIBLOB_LIBS) $(PCILIB_LIBS)
libupdater_la_CXXFLAGS = -I$(top_srcdir) $(CODE_COVERAGE_CXXFLAGS) $(IPMIBLOB_CFLAGS) $(PCILIB_CFLAGS)
libupdater_la_SOURCES = \
	$(top_srcdir)/util.cpp \
	updater.cpp \
	handler.cpp \
	helper.cpp \
	bt.cpp \
	lpc.cpp \
	io.cpp \
	pci.cpp \
	p2a.cpp \
	$(top_srcdir)/internal/sys.cpp

SUBDIRS = . test
