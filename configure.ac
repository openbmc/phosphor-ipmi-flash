# Initialization
AC_PREREQ([2.69])
AC_INIT([phosphor-ipmi-flash], [0.1], [https://github.com/openbmc/phosphor-ipmi-flash/issues])
AC_LANG([C++])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIRS([m4])
AM_INIT_AUTOMAKE([subdir-objects -Wall -Werror foreign dist-xz])
AM_SILENT_RULES([yes])

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
PKG_CHECK_MODULES([SDBUSPLUS], [sdbusplus], ,[AC_MSG_ERROR([The openbmc/sdbusplus package is required])])
PKG_CHECK_MODULES([PHOSPHOR_LOGGING], [phosphor-logging],, [AC_MSG_ERROR([Could not find phosphor-logging...openbmc/phosphor-logging package required])])
AC_CHECK_HEADER([blobs-ipmid], [AC_MSG_ERROR(["phosphor-ipmi-blobs required and not found."])])
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
AS_IF([test "x$enable_pci_bridge" = "xyes"], [
    AX_APPEND_COMPILE_FLAGS([-DENABLE_PCI_BRIDGE], [CXXFLAGS])
])
AC_ARG_ENABLE([lpc-bridge],
    AS_HELP_STRING([--enable-lpc-bridge],
                   [Enable lpc-bridge transport mechanism]))
AS_IF([test "x$enable_lpc_bridge" = "xyes"], [
    AX_APPEND_COMPILE_FLAGS([-DENABLE_LPC_BRIDGE], [CXXFLAGS])
])

# The address used for mapping P2A or LPC into the BMC's memory-space:
# e.g. https://github.com/openbmc/linux/blob/1da2ce51886a3b2f5db2087f26c661e13ee13b84/arch/arm/boot/dts/aspeed-bmc-quanta-q71l.dts#L26
# or https://github.com/openbmc/linux/blob/1da2ce51886a3b2f5db2087f26c661e13ee13b84/arch/arm/boot/dts/aspeed-bmc-opp-zaius.dts#L166
# for PCI, this address is passed back to the host and is used directly.
AC_ARG_VAR(MAPPED_ADDRESS, [The base address of the memory region reserved for mapping.])
AS_IF([test "x$MAPPED_ADDRESS" == "x"],
    [AC_DEFINE(MAPPED_ADDRESS, [0], [Default address to 0.])],
    [AC_DEFINE(MAPPED_ADDRESS, [$MAPPED_ADDRESS], [Value for memory region mapping.])]
)

AC_ARG_ENABLE([aspeed-lpc],
    AS_HELP_STRING([--enable-aspeed-lpc],
                   [Enable external transfers using Aspeed LPC]))
AS_IF([test "x$enable_aspeed_lpc" = "xyes"], [
    AC_CHECK_HEADER(linux/aspeed-lpc-ctrl.h,
                    [HAVE_UAPI_LINUX_LPC_CTRL_H=""],
                    [HAVE_UAPI_LINUX_LPC_CTRL_H="-I linux/aspeed-lpc-ctrl.h"])
    AS_IF([test "$HAVE_UAPI_LINUX_LPC_CTRL_H" != ""],
        AC_MSG_WARN([Could not find linux/aspeed-lpc-ctrl.h: Attempting to download locally for building from openbmc/linux/+/dev-4.18])
        AC_SUBST([BT_BMC_DL],
                 [`mkdir -p linux;wget https://raw.githubusercontent.com/openbmc/linux/dev-4.18/include/uapi/linux/aspeed-lpc-ctrl.h -O linux/aspeed-lpc-ctrl.h`])
    )
    AX_APPEND_COMPILE_FLAGS([-DASPEED_LPC], [CXXFLAGS])
])
AC_ARG_ENABLE([nuvoton-lpc],
    AS_HELP_STRING([--enable-nuvoton-lpc],
                   [Enable external transfers using Nuvoton LPC SHM]))
AS_IF([test "x$enable_nuvoton_lpc" = "xyes"], [
    AX_APPEND_COMPILE_FLAGS([-DNUVOTON_LPC], [CXXFLAGS])
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

AC_CHECK_HEADER(linux/ipmi.h, [HAVE_LINUX_NCSI_H=""], [HAVE_LINUX_NCSI_H="-I linux/ipmi.h"])
AS_IF([test "$HAVE_LINUX_NCSI_H" != ""],
    AC_MSG_WARN([Could not find linux/ipmi.h: Attempting to download locally for building from openbmc/linux/+/dev-4.18])
    AC_SUBST([IPMI_BMC_DL],
             [`mkdir -p linux;wget https://raw.githubusercontent.com/openbmc/linux/dev-4.18/include/uapi/linux/ipmi.h -O linux/ipmi.h`])
)

# Check/set gtest specific functions.
PKG_CHECK_MODULES([GTEST], [gtest], [], [AC_MSG_NOTICE([gtest not found, tests will not build])])
PKG_CHECK_MODULES([GMOCK], [gmock], [], [AC_MSG_NOTICE([gmock not found, tests will not build])])
PKG_CHECK_MODULES([GTEST_MAIN], [gtest_main], [], [AC_MSG_NOTICE([gtest_main not found, tests will not build])])

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
