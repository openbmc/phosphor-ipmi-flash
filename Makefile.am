AM_DEFAULT_SOURCE_EXT = .cpp

libfirmwareblobdir = ${libdir}/ipmid-providers
libfirmwareblob_LTLIBRARIES = libfirmwareblob.la
libfirmwareblob_la_SOURCES = main.cpp \
		      firmware_handler.cpp
libfirmwareblob_la_LDFLAGS = $(PHOSPHOR_LOGGING_LIBS) \
                        -version-info 0:0:0 -shared
libfirmwareblob_la_CXXFLAGS = $(PHOSPHOR_LOGGING_CFLAGS) \
                         -flto

SUBDIRS = . test
