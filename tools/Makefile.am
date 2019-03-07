noinst_PROGRAMS = burn_my_bmc

burn_my_bmc_SOURCES = main.cpp
burn_my_bmc_LDADD = libupdater.la
burn_my_bmc_CXXFLAGS = -I$(top_srcdir)

noinst_LTLIBRARIES = libupdater.la
libupdater_la_LDFLAGS = -static $(IPMIBLOB_LIBS)
libupdater_la_CXXFLAGS = -I$(top_srcdir) $(IPMIBLOB_CFLAGS)
libupdater_la_SOURCES = \
	updater.cpp \
	bt.cpp \
	lpc.cpp \
	io.cpp \
	../internal/sys.cpp
