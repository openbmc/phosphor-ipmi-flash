# Initialization
AC_PREREQ([2.69])
AC_INIT([phosphor-ipmi-flash], [0.1], [https://github.com/openbmc/phosphor-ipmi-flash/issues])
AC_LANG([C++])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIRS([m4])
AM_INIT_AUTOMAKE([subdir-objects -Wall -Wno-portability -Werror foreign dist-xz])
AM_SILENT_RULES([yes])

# Make sure the default CFLAGS of `-O2 -g` don't override CODE_COVERAGE_CFLAGS
# It is important that this comes before AC_PROG_C{C,XX}, as we are attempting
# to stop them from populating default CFLAGS and CXXFLAGS.
AS_IF([test "x$enable_tests" = "xno"], [enable_code_coverage=no])
AS_IF([test "x$enable_code_coverage" != "xno"], [
    AS_IF([test "x${CXXFLAGS+set}" != "xset"], [
        AC_SUBST(CXXFLAGS, [""])
    ])
    AS_IF([test "x${CFLAGS+set}" != "xset"], [
        AC_SUBST(CFLAGS, [""])
    ])
])

# If building tests, enable both hardware options that are configuration controlled
AS_IF([test "x$enable_tests" = "xyes"], [enable_static_layout=yes])
AS_IF([test "x$enable_tests" = "xyes"], [enanble_tarball_ubi=yes])

AS_IF([test "x$enable_tests" = "xyes"], [enable_pci_bridge=yes])
AS_IF([test "x$enable_tests" = "xyes"], [enable_aspeed_p2a=yes])

# You can only build main.o with one of the lpc implementations, however, specify both.
AS_IF([test "x$enable_tests" = "xyes"], [enable_lpc_bridge=yes])
AS_IF([test "x$enable_tests" = "xyes"], [enable_aspeed_lpc=yes])
AS_IF([test "x$enable_tests" = "xyes"], [enable_nuvoton_lpc=yes])

# Checks for programs.
AC_PROG_CXX
AM_PROG_AR
AC_PROG_INSTALL
AC_PROG_MAKE_SET

# Checks for typedefs, structures, and compiler characteristics.
AX_CXX_COMPILE_STDCXX_17([noext])
AX_APPEND_COMPILE_FLAGS([-Wall -Werror], [CXXFLAGS])

# Checks for library functions.
LT_INIT # Required for systemd linking

# Checks for libraries.
PKG_CHECK_MODULES(
    [SDBUSPLUS],
    [sdbusplus],
    [AC_DEFINE(HAVE_SDBUSPLUS, [1], [Found openbmc/sdbusplus package])],
    [AC_MSG_ERROR([The openbmc/sdbusplus package is required])]
)
PKG_CHECK_MODULES([PHOSPHOR_LOGGING], [phosphor-logging])
PKG_CHECK_MODULES(
    [IPMIBLOB],
    [ipmiblob],
    [],
    [AC_MSG_ERROR(
        [Could not find ipmiblob...openbmc/ipmi-blob-tool package required])
    ]
)
PKG_CHECK_MODULES(
    [PCILIB],
    [libpci],
    [],
    [AC_MSG_ERROR([Could not find libpci... pciutils package required])]
)
AC_CHECK_HEADER(
    [blobs-ipmid],
    [AC_MSG_ERROR(["phosphor-ipmi-blobs required and not found."])]
)
AX_PTHREAD([], [AC_MSG_ERROR(["pthread required and not found"])])

# Configurability
# Enable static layout for firmware image staging.
AC_ARG_ENABLE([static-layout],
    AS_HELP_STRING([--enable-static-layout],
                   [Enable static layout firmware update via Blobs]))
AS_IF([test "x$enable_static_layout" = "xyes"], [
    AX_APPEND_COMPILE_FLAGS([-DENABLE_STATIC_LAYOUT], [CXXFLAGS])
])
# Enable tarball for firmware image staging.
AC_ARG_ENABLE([tarball-ubi],
    AS_HELP_STRING([--enable-tarball-ubi],
                   [Enable tarball firmware update via Blobs]))
AS_IF([test "x$enanble_tarball_ubi" = "xyes"], [
    AX_APPEND_COMPILE_FLAGS([-DENABLE_TARBALL_UBI], [CXXFLAGS])
])

# Enable P2A, and or LPC (block-transfer is always enabled).
AC_ARG_ENABLE([pci-bridge],
    AS_HELP_STRING([--enable-pci-bridge],
                   [Enable pci-bridge transport mechanism]))
AM_CONDITIONAL(ENABLE_PCI_BRIDGE, [test "x$enable_pci_bridge" = "xyes"])
AS_IF([test "x$enable_pci_bridge" = "xyes"], [
    AX_APPEND_COMPILE_FLAGS([-DENABLE_PCI_BRIDGE], [CXXFLAGS])
])
AC_ARG_ENABLE([lpc-bridge],
    AS_HELP_STRING([--enable-lpc-bridge],
                   [Enable lpc-bridge transport mechanism]))
AM_CONDITIONAL(ENABLE_LPC_BRIDGE, [test "x$enable_lpc_bridge" = "xyes"])
AS_IF([test "x$enable_lpc_bridge" = "xyes"], [
    AX_APPEND_COMPILE_FLAGS([-DENABLE_LPC_BRIDGE], [CXXFLAGS])
])

# The address used for mapping P2A or LPC into the BMC's memory-space:
# e.g. https://github.com/openbmc/linux/blob/1da2ce51886a3b2f5db2087f26c661e13ee13b84/arch/arm/boot/dts/aspeed-bmc-quanta-q71l.dts#L26
# or https://github.com/openbmc/linux/blob/1da2ce51886a3b2f5db2087f26c661e13ee13b84/arch/arm/boot/dts/aspeed-bmc-opp-zaius.dts#L166
# for PCI, this address is passed back to the host and is used directly.
AC_ARG_VAR(MAPPED_ADDRESS, [The base address of the memory region reserved for mapping.])
AS_IF([test "x$MAPPED_ADDRESS" == "x"],
    [AC_DEFINE_UNQUOTED(MAPPED_ADDRESS, [0], [Default address to 0.])],
    [AC_DEFINE_UNQUOTED(MAPPED_ADDRESS, [$MAPPED_ADDRESS], [Value for memory region mapping.])]
)

AC_ARG_ENABLE([aspeed-lpc],
    AS_HELP_STRING([--enable-aspeed-lpc],
                   [Enable external transfers using Aspeed LPC]))
AM_CONDITIONAL(ENABLE_ASPEED_LPC, [test "x$enable_aspeed_lpc" = "xyes"])
AS_IF([test "x$enable_aspeed_lpc" = "xyes"], [
    AC_CHECK_HEADER(
        [linux/aspeed-lpc-ctrl.h],
        [],
        [AC_MSG_ERROR([Could not find linux/aspeed-lpc-ctrl.h])]
    )

    AX_APPEND_COMPILE_FLAGS([-DASPEED_LPC], [CXXFLAGS])
])
AC_ARG_ENABLE([nuvoton-lpc],
    AS_HELP_STRING([--enable-nuvoton-lpc],
                   [Enable external transfers using Nuvoton LPC SHM]))
AM_CONDITIONAL(ENABLE_NUVOTON_LPC, [test "x$enable_nuvoton_lpc" = "xyes"])
AS_IF([test "x$enable_nuvoton_lpc" = "xyes"], [
    AX_APPEND_COMPILE_FLAGS([-DNUVOTON_LPC], [CXXFLAGS])
])

AC_ARG_ENABLE([aspeed-p2a],
    AS_HELP_STRING([--enable-aspeed-p2a],
                   [Enable external transfers using Aspeed PCI-to-AHB]))
AM_CONDITIONAL(ENABLE_ASPEED_P2A, [test "x$enable_aspeed_p2a" = "xyes"])
AS_IF([test "x$enable_aspeed_p2a" = "xyes"], [
    AC_CHECK_HEADER(linux/aspeed-p2a-ctrl.h,
                    [HAVE_UAPI_LINUX_P2A_CTRL_H=""],
                    [HAVE_UAPI_LINUX_P2A_CTRL_H="-I linux/aspeed-p2a-ctrl.h"])
    AS_IF([test "$HAVE_UAPI_LINUX_P2A_CTRL_H" != ""],
        AC_MSG_WARN([Could not find linux/aspeed-p2a-ctrl.h: Attempting to download locally for building from openbmc/linux/+/dev-5.0])
        AC_SUBST([PCI_BMC_DL],
                 [`mkdir -p linux;wget https://raw.githubusercontent.com/openbmc/linux/dev-5.0/include/uapi/linux/aspeed-p2a-ctrl.h -O linux/aspeed-p2a-ctrl.h`])
    )
    AX_APPEND_COMPILE_FLAGS([-DASPEED_P2A], [CXXFLAGS])
])

AC_ARG_VAR(STATIC_HANDLER_STAGED_NAME, [The file to use for staging the firmware update.])
AS_IF([test "x$STATIC_HANDLER_STAGED_NAME" == "x"], [STATIC_HANDLER_STAGED_NAME="/run/initramfs/bmc-image"])
AC_DEFINE_UNQUOTED([STATIC_HANDLER_STAGED_NAME], ["$STATIC_HANDLER_STAGED_NAME"], [The file to use for staging the firmware update.])

AC_ARG_VAR(TARBALL_STAGED_NAME, [The file to use for staging the firmware update.])
AS_IF([test "x$TARBALL_STAGED_NAME" == "x"], [TARBALL_STAGED_NAME="/tmp/image-update.tar"])
AC_DEFINE_UNQUOTED([TARBALL_STAGED_NAME], ["$TARBALL_STAGED_NAME"], [The file to use for staging the firmware update.])

AC_ARG_VAR(HASH_FILENAME, [The file to use for the hash provided.])
AS_IF([test "x$HASH_FILENAME" == "x"], [HASH_FILENAME="/tmp/bmc.sig"])
AC_DEFINE_UNQUOTED([HASH_FILENAME], ["$HASH_FILENAME"], [The file to use for the hash provided.])

AC_CHECK_HEADER(linux/ipmi.h, [HAVE_LINUX_IPMI_H=""], [HAVE_LINUX_IPMI_H="-I linux/ipmi.h"])
AS_IF([test "$HAVE_LINUX_IPMI_H" != ""],
    AC_MSG_WARN([Could not find linux/ipmi.h: Attempting to download locally for building from openbmc/linux/+/dev-4.18])
    AC_SUBST([IPMI_BMC_DL],
             [`mkdir -p linux;wget https://raw.githubusercontent.com/openbmc/linux/dev-4.18/include/uapi/linux/ipmi.h -O linux/ipmi.h`])
)

AC_CHECK_HEADER(linux/ipmi_msgdefs.h, [HAVE_LINUX_IPMIMSGDEFS_H=""], [HAVE_LINUX_IPMIMSGDEFS_H="-I linux/ipmi_msgdefs.h"])
AS_IF([test "$HAVE_LINUX_IPMIMSGDEFS_H" != ""],
    AC_MSG_WARN([Could not find linux/ipmi_msgdefs.h: Attempting to download locally for building from openbmc/linux/+/dev-4.18])
    AC_SUBST([IPMIMSG_BMC_DL],
             [`mkdir -p linux;wget https://raw.githubusercontent.com/openbmc/linux/dev-4.18/include/uapi/linux/ipmi_msgdefs.h -O linux/ipmi_msgdefs.h`])
)

# Make it possible for users to choose if they want test support
# explicitly or not at all
AC_ARG_ENABLE([tests], AC_HELP_STRING([--disable-tests],
                                      [Build test cases]))

# Check/set gtest specific functions.
AS_IF([test "x$enable_tests" != "xno"], [
    PKG_CHECK_MODULES([GTEST], [gtest], [], [true])
    PKG_CHECK_MODULES([GMOCK], [gmock], [], [true])
    AX_PTHREAD

    AX_SAVE_FLAGS_WITH_PREFIX(OLD, [CPPFLAGS])
    AX_APPEND_COMPILE_FLAGS([$GTEST_CFLAGS], [CPPFLAGS])
    AC_LANG_PUSH([C++])
    AC_CHECK_HEADERS([gtest/gtest.h], [
        AS_IF([test "x$GTEST_CFLAGS" = "x"], [
            AS_IF([test "x$PTHREAD_CFLAGS" = "x"], [
                AX_APPEND_COMPILE_FLAGS(["-DGTEST_HAS_PTHREAD=0"], [GTEST_CFLAGS])
            ], [
                AX_APPEND_COMPILE_FLAGS(["-DGTEST_HAS_PTHREAD=1"], [GTEST_CFLAGS])
                AX_APPEND_COMPILE_FLAGS([$PTHREAD_CFLAGS], [GTEST_CFLAGS])
            ])
        ])
    ], [
        AS_IF([test "x$enable_tests" = "xyes"], [
            AC_MSG_ERROR([Testing enabled but could not find gtest/gtest.h])
        ])
    ])
    AC_LANG_POP([C++])
    AX_RESTORE_FLAGS_WITH_PREFIX(OLD, [CPPFLAGS])

    AX_SAVE_FLAGS_WITH_PREFIX(OLD, [CPPFLAGS])
    AX_APPEND_COMPILE_FLAGS([$GMOCK_CFLAGS], [CPPFLAGS])
    AC_LANG_PUSH([C++])
    AC_CHECK_HEADERS([gmock/gmock.h], [], [
        AS_IF([test "x$enable_tests" = "xyes"], [
            AC_MSG_ERROR([Testing enabled but could not find gmock/gmock.h])
        ])
    ])
    AC_LANG_POP([C++])
    AX_RESTORE_FLAGS_WITH_PREFIX(OLD, [CPPFLAGS])

    AX_SAVE_FLAGS_WITH_PREFIX(OLD, [LDFLAGS])
    AX_APPEND_COMPILE_FLAGS([$GTEST_LIBS], [LDFLAGS])
    AC_CHECK_LIB([gtest], [main], [
        AS_IF([test "x$GTEST_LIBS" = "x"], [
            AX_APPEND_COMPILE_FLAGS([-lgtest], [GTEST_LIBS])
        ])
    ], [
        AS_IF([test "x$enable_tests" = "xyes"], [
            AC_MSG_ERROR([Testing enabled but couldn't find gtest libs])
        ])
    ])
    AX_RESTORE_FLAGS_WITH_PREFIX(OLD, [LDFLAGS])

    AX_SAVE_FLAGS_WITH_PREFIX(OLD, [LDFLAGS])
    AX_APPEND_COMPILE_FLAGS([$GMOCK_LIBS], [LDFLAGS])
    AC_CHECK_LIB([gmock], [main], [
        AS_IF([test "x$GMOCK_LIBS" = "x"], [
            AX_APPEND_COMPILE_FLAGS([-lgmock], [GMOCK_LIBS])
        ])
    ], [
        AS_IF([test "x$enable_tests" = "xyes"], [
            AC_MSG_ERROR([Testing enabled but couldn't find gmock libs])
        ])
    ])
    AX_RESTORE_FLAGS_WITH_PREFIX(OLD, [LDFLAGS])
])

# Check for valgrind
AS_IF([test "x$enable_tests" = "xno"], [enable_valgrind=no])
m4_foreach([vgtool], [valgrind_tool_list],
    [AX_VALGRIND_DFLT(vgtool, [off])])
AX_VALGRIND_DFLT([memcheck], [on])
AX_VALGRIND_CHECK
AM_EXTRA_RECURSIVE_TARGETS([check-valgrind])
m4_foreach([vgtool], [valgrind_tool_list],
    [AM_EXTRA_RECURSIVE_TARGETS([check-valgrind-]vgtool)])

# Code coverage
AX_CODE_COVERAGE
AS_IF([test "x$CODE_COVERAGE_ENABLED" = "xyes"], [
    AX_APPEND_COMPILE_FLAGS([-DHAVE_GCOV], [CODE_COVERAGE_CPPFLAGS])
])
m4_ifdef([_AX_CODE_COVERAGE_RULES],
    [AM_CONDITIONAL(AUTOCONF_CODE_COVERAGE_2019_01_06, [true])],
    [AM_CONDITIONAL(AUTOCONF_CODE_COVERAGE_2019_01_06, [false])])
AX_ADD_AM_MACRO_STATIC([])

# Add --enable-oe-sdk flag to configure script
AC_ARG_ENABLE([oe-sdk],
    AS_HELP_STRING([--enable-oe-sdk], [Link testcases absolutely against OE SDK so they can be ran within it.])
)

# Check for OECORE_TARGET_SYSROOT in the environment.
AC_ARG_VAR(OECORE_TARGET_SYSROOT,
    [Path to the OE SDK SYSROOT])

# Configure OESDK_TESTCASE_FLAGS environment variable, which will be later
# used in test/Makefile.am
AS_IF([test "x$enable_oe_sdk" == "xyes"],
    AS_IF([test "x$OECORE_TARGET_SYSROOT" == "x"],
          AC_MSG_ERROR([OECORE_TARGET_SYSROOT must be set with --enable-oe-sdk])
    )
    AC_MSG_NOTICE([Enabling OE-SDK at $OECORE_TARGET_SYSROOT])
    [
        testcase_flags="-Wl,-rpath,\${OECORE_TARGET_SYSROOT}/lib"
        testcase_flags="${testcase_flags} -Wl,-rpath,\${OECORE_TARGET_SYSROOT}/usr/lib"
        testcase_flags="${testcase_flags} -Wl,-dynamic-linker,`find \${OECORE_TARGET_SYSROOT}/lib/ld-*.so | sort -r -n | head -n1`"
    ]
    AC_SUBST([OESDK_TESTCASE_FLAGS], [$testcase_flags])
)

# Create configured output
AC_CONFIG_FILES([Makefile test/Makefile tools/Makefile])
AC_OUTPUT
