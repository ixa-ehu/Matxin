#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT([matxin-es-eu], [1.5.0], no-define)
AM_INIT_AUTOMAKE([matxin-es-eu], [1.5.0], no-define)
PACKAGE=matxin-es-eu

GENERIC_MAJOR_VERSION=1
GENERIC_MINOR_VERSION=5
GENERIC_MICRO_VERSION=0
GENERIC_EXTRA_VERSION=

GENERIC_RELEASE=$GENERIC_MAJOR_VERSION.$GENERIC_MINOR_VERSION
GENERIC_RELEASE=$GENERIC_MAJOR_VERSION.$GENERIC_MINOR_VERSION.$GENERIC_MICRO_VERSION-$GENERIC_EXTRA_VERSION
AC_SUBST(GENERIC_RELEASE)
AC_SUBST(GENERIC_VERSION)

#
# API version (often = GENERIC_MAJOR_VERSION.GENERIC_MINOR_VERSION)
#
GENERIC_API_VERSION=$GENERIC_MAJOR_VERSION
AC_SUBST(GENERIC_API_VERSION)

AC_SUBST(GENERIC_LIBRARY_NAME, $PACKAGE)

#
# Required library versions
#
m4_define([required_lttoolbox_version], [3.1.1])
m4_define([required_libxml_version], [2.6.17])
m4_define([required_matxin_version], [1.5.0])
m4_define([required_libpcre_version], [6.4])

#
# Our configure script should create Makefiles
#
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

#
# Set default paths for PKG_CONFIG_PATH in case it's not defined
#
if ! test $PKG_CONFIG_PATH ; then
  PKG_CONFIG_PATH="/usr/lib/pkgconfig:/usr/local/lib/pkgconfig"
fi

#
# Checks for programs.
#
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB
AC_LANG([C++])

AC_MSG_CHECKING([Operating System])
OSTYPE=$($(which uname) -s)
AC_MSG_RESULT([$OSTYPE])
AC_SUBST(OSTYPE)

# We need this to determine whether we use zcat or gzcat
AM_CONDITIONAL(OS_IS_SOLARIS, [test x"$OSTYPE" = xSunOS])
AM_CONDITIONAL(OS_IS_DARWIN, [test x"$OSTYPE" = xDarwin])

AC_PATH_PROG(FLEX, flex, no)
if test x$ac_cv_path_FLEX = x
then
  AC_MSG_ERROR([flex not found. You may want to adjust your PATH.])
fi
if test x$ac_cv_path_FLEX = xno
then
  AC_MSG_ERROR([flex not found. You may want to adjust your PATH.])
fi

AC_PATH_PROG(PERL, perl, no)
if test x$ac_cv_path_PERL = x
then
  AC_MSG_ERROR([perl not found. You may want to adjust your PATH.])
fi
if test x$ac_cv_path_PERL = xno
then
  AC_MSG_ERROR([perl not found. You may want to adjust your PATH.])
fi

AC_PATH_PROG(BASH, bash, no)
if test x$ac_cv_path_BASH = x
then
  AC_MSG_ERROR([bash not found. You may want to adjust your PATH.])
fi
if test x$ac_cv_path_BASH = xno
then
  AC_MSG_ERROR([bash not found. You may want to adjust your PATH.])
fi

AC_PATH_PROG(LT_COMP, lt-comp, no)
if test x$ac_cv_path_LT_COMP = x
then
  AC_MSG_ERROR([lt-comp not found. You may want to adjust your PATH.])
fi
if test x$ac_cv_path_LT_COMP = xno
then
  AC_MSG_ERROR([lt-comp not found. You may want to adjust your PATH.])
fi

#
# Checks for libraries.
#
AC_CHECK_LIB(boost_filesystem, main, [true],AC_MSG_ERROR([boost_filesystem found]))

PKG_CHECK_MODULES(MATXIN, [dnl
 lttoolbox-3.1 >= required_lttoolbox_version dnl
 matxin-engine >= required_matxin_version dnl
 libxml-2.0 >= required_libxml_version dnl
 libpcre >= required_libpcre_version], CPPFLAGS="$CPPFLAGS $MATXIN_CPPFLAGS $MATXIN_CFLAGS"; LIBS="$LIBS $MATXIN_LIBS")

#
# Checks for header files.
#
AC_HEADER_STDC

#
# Checks for typedefs, structures, and compiler characteristics.
#
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T


AC_SUBST(MATXINDATADIR, $prefix/share/matxin/$PACKAGE)

#
# Generate these files
#
AC_CONFIG_FILES([Makefile
		 es-eu.cfg])
AC_OUTPUT

