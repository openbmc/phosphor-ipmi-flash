noinst_PROGRAMS = burn_my_bmc

burn_my_bmc_SOURCES = main.cpp
burn_my_bmc_LDADD = libupdater.la
burn_my_bmc_CXXFLAGS = -I$(top_srcdir)

noinst_LTLIBRARIES = libupdater.la
libupdater_la_LDFLAGS = -static
libupdater_la_CXXFLAGS = -I$(top_srcdir)
libupdater_la_SOURCES = \
	updater.cpp \
	bt.cpp \
	lpc.cpp \
	blob_handler.cpp \
	ipmi_handler.cpp \
	crc.cpp \
	io.cpp \
	../internal/sys.cpp
