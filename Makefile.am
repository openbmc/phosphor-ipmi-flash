AM_DEFAULT_SOURCE_EXT = .cpp

libfirmwareblobdir = ${libdir}/ipmid-providers
libfirmwareblob_LTLIBRARIES = libfirmwareblob.la
libfirmwareblob_la_SOURCES = \
	main.cpp \
	firmware_handler.cpp \
	lpc_handler.cpp \
	pci_handler.cpp \
	file_handler.cpp
libfirmwareblob_la_LDFLAGS = \
	$(SDBUSPLUS_LIBS) \
	$(PHOSPHOR_LOGGING_LIBS) \
	-version-info 0:0:0 -shared
libfirmwareblob_la_CXXFLAGS = \
	$(SDBUSPLUS_CFLAGS) \
	$(PHOSPHOR_LOGGING_CFLAGS) \
	-flto

SUBDIRS = . test
