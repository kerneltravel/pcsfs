#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([pcsfs], [0.1], [madoldman@madoldman.org])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR(admin)
AM_INIT_AUTOMAKE(@PACKAGE_NAME@, @PACKAGE_VERSION@)
# Checks for programs.
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
LIBCURL_CHECK_CONFIG 
PKG_CHECK_MODULES(JSON, [json], [LIBS="$LIBS $JSON_LIBS" CFLAGS="$CFLAGS $JSON_CFLAGS"], 
		  AC_MSG_ERROR(Cannot find JSON: Is json-config in path?))
PKG_CHECK_MODULES(FUSE, [fuse], [LIBS="$LIBS $FUSE_LIBS" CFLAGS="$CFLAGS $FUSE_CFLAGS -DFUSE_USE_VERSION=26"], 
		  AC_MSG_ERROR(Cannot find FUSE: Is fuse-config in path?))

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
