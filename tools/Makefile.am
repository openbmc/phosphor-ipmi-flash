bin_PROGRAMS = burn_my_bmc

burn_my_bmc_SOURCES = main.cpp
burn_my_bmc_LDADD = libupdater.la $(CODE_COVERAGE_LIBS)
burn_my_bmc_CXXFLAGS = \
	-I$(top_srcdir) \
	$(CODE_COVERAGE_CXXFLAGS)

noinst_LTLIBRARIES = libupdater.la
libupdater_la_LDFLAGS = -static $(CODE_COVERAGE_LIBS) $(IPMIBLOB_LIBS) $(PCILIB_LIBS)
libupdater_la_CXXFLAGS = \
	-I$(top_srcdir) \
	$(CODE_COVERAGE_CXXFLAGS) \
	$(IPMIBLOB_CFLAGS) \
	$(PCILIB_CFLAGS)
libupdater_la_SOURCES = \
	updater.cpp \
	handler.cpp \
	helper.cpp \
	bt.cpp \
	lpc.cpp \
	io.cpp \
	net.cpp \
	pci.cpp \
	progress.cpp

if ENABLE_ASPEED_P2A
libupdater_la_SOURCES += p2a_aspeed.cpp
else
libupdater_la_SOURCES += p2a_nuvoton.cpp
endif

libupdater_la_LIBADD = $(top_builddir)/libfirmware_common.la

SUBDIRS = . test
