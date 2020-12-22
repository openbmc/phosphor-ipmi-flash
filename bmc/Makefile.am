AM_DEFAULT_SOURCE_EXT = .cpp
noinst_LTLIBRARIES = libbmc_common.la

# shared functionality between firmware and version blob handlers
libbmc_common_la_SOURCES = \
	buildjson.cpp \
	file_handler.cpp \
	fs.cpp \
	general_systemd.cpp \
	skip_action.cpp

libbmc_common_la_CXXFLAGS = \
	-I$(top_srcdir) \
	$(SDBUSPLUS_CFLAGS) \
	$(PHOSPHOR_LOGGING_CFLAGS) \
	$(CODE_COVERAGE_CXXFLAGS) \
	-flto
libbmc_common_la_LDFLAGS = \
	$(SDBUSPLUS_LIBS) \
	$(PHOSPHOR_LOGGING_LIBS) \
	$(CODE_COVERAGE_LIBS) \
	-lstdc++fs
