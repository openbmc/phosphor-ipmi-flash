noinst_PROGRAMS = burn_my_bmc

AM_CPPFLAGS = -I./libs -I./libs/ipmitool/include

burn_my_bmc_SOURCES = main.cpp
burn_my_bmc_LDADD = libupdater.la
burn_my_bmc_CXXFLAGS =

noinst_LTLIBRARIES = libupdater.la libipmi.la libipmitool.la
libupdater_la_LDFLAGS = -static
libupdater_la_LIBADD = libipmi.la
libupdater_la_SOURCES = \
	updater.cpp

libipmi_la_LDFLAGS = -static
libipmi_la_LIBADD = libipmitool.la
libipmi_la_SOURCES = \
	raw.cpp

libipmitool_la_LDFLAGS = -static
libipmitool_la_LIBADD =
libipmitool_la_SOURCES = \
	libs/ipmitool.c \
	libs/ipmitool/src/plugins/open/open.c

SUBDIRS = . test
