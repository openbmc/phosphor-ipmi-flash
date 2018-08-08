
# Initialization
AC_PREREQ([2.69])
AC_INIT([flash-ipmi], [1.0], [https://www.google.com])
AC_LANG([C++])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([subdir-objects -Wall -Werror foreign dist-xz])
AM_SILENT_RULES([yes])

# Checks for programs.
AC_PROG_CXX
AM_PROG_AR
AC_PROG_INSTALL
AC_PROG_MAKE_SET

# Checks for typedefs, structures, and compiler characteristics.
AX_CXX_COMPILE_STDCXX_14([noext])
AX_APPEND_COMPILE_FLAGS([-Wall], [CXXFLAGS])

# Checks for libraries.
PKG_CHECK_MODULES([SYSTEMD], [libsystemd >= 221], [], [AC_MSG_ERROR(["systemd required and not found"])])
PKG_CHECK_MODULES([SDBUSPLUS], [sdbusplus], ,[AC_MSG_ERROR([The openbmc/sdbusplus package is required])])
PKG_CHECK_MODULES([PHOSPHOR_LOGGING], [phosphor-logging], ,[AC_MSG_ERROR([The openbmc/phosphor-logging package is required])])
PKG_CHECK_MODULES([PHOSPHOR_DBUS_INTERFACES], [phosphor-dbus-interfaces], [], [AC_MSG_ERROR(["phosphor-dbus-interfaces required and not found."])])
AC_CHECK_HEADER([host-ipmid], [AC_MSG_ERROR(["phosphor-host-ipmid required and not found."])])

# Checks for library functions.
LT_INIT # Required for systemd linking

# Check/set gtest specific functions.
PKG_CHECK_MODULES([GTEST], [gtest], [], [AC_MSG_NOTICE([gtest not found, tests will not build])])
PKG_CHECK_MODULES([GMOCK], [gmock], [], [AC_MSG_NOTICE([gmock not found, tests will not build])])
PKG_CHECK_MODULES([GTEST_MAIN], [gtest_main], [], [AC_MSG_NOTICE([gtest_main not found, tests will not build])])

# Do you want to register as a Google OEM IPMI handler as well as firmware?
AC_ARG_ENABLE([google],
    AS_HELP_STRING([--enable-google], [Enable registration as Google OEM IPMI Handler])
)

AC_ARG_VAR(ENABLE_GOOGLE, [Enable registration as Google OEM IPMI Handler])

AS_IF([test "x$enable_google" == "xyes"],
      [ENABLE_GOOGLE="yes"]
      AC_DEFINE_UNQUOTED([ENABLE_GOOGLE], ["$ENABLE_GOOGLE"], [Enable registration as Google OEM IPMI Handler])
)

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

AC_ARG_VAR(STAGING_PATH, [The staging path for the flash image.])
AS_IF([test "x$STAGING_PATH" == "x"], [STAGING_PATH="/run/initramfs/bmc-image"])
AC_DEFINE_UNQUOTED([STAGING_PATH], ["$STAGING_PATH"], [The staging path for the flash image.])

# Create configured output
AC_CONFIG_FILES([Makefile test/Makefile tools/Makefile])
AC_OUTPUT
