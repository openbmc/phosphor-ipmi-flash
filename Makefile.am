AM_DEFAULT_SOURCE_EXT = .cpp

libflashcmdsdir = ${libdir}/ipmid-providers
libflashcmds_LTLIBRARIES = libflashcmds.la
libflashcmds_la_SOURCES = main.cpp \
			  flash-ipmi.cpp \
			  ipmi.cpp

libflashcmds_la_LDFLAGS = $(SYSTEMD_LIBS) \
                          $(PHOSPHOR_DBUS_INTERFACES_LIBS) \
                          $(PHOSPHOR_LOGGING_LIBS) \
                          -version-info 0:0:0 -shared
libflashcmds_la_CXXFLAGS = $(SYSTEMD_CFLAGS) \
                           $(PHOSPHOR_DBUS_INTERFACES_CFLAGS) \
                           $(PHOSPHOR_LOGGING_CFLAGS)

SUBDIRS = . test
