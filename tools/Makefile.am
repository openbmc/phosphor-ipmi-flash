noinst_PROGRAMS = burn_my_bmc

AM_CPPFLAGS = -I./ipmitool/include

burn_my_bmc_SOURCES = main.cpp
burn_my_bmc_LDADD = libupdater.la
burn_my_bmc_CXXFLAGS =

noinst_LTLIBRARIES = libupdater.la libipmitool.la
libupdater_la_LDFLAGS = -static
libupdater_la_LIBADD = libipmitool.la
libupdater_la_SOURCES = \
	updater.cpp

libipmitool_la_LDFLAGS = -static
libipmitool_la_LIBADD =
libipmitool_la_SOURCES = \
	ipmitool/src/plugins/open/open.c
