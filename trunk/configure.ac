AC_PREREQ(2.59)
AC_INIT(ggtl, 2.1.3, stig@brautaset.org)

AC_CONFIG_AUX_DIR([config])
AC_CONFIG_MACRO_DIR([config])
AC_CONFIG_SRCDIR([ggtl/ggtl.c])

AM_INIT_AUTOMAKE([foreign])

# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL

# prove is used to summarize test results
AC_CHECK_PROG(PROVE, prove, yes)
AM_CONDITIONAL(HAVE_PROVE, test x$PROVE = xyes)


# Libraries
AC_CHECK_LIB(sl, sl_push, [], [ AC_MSG_ERROR([
  Fatal error -- the sl library is missing.
  sl is available from http://dev.brautaset.org/sl/download/
  ])
])

# we depend on libtap for most of the tests
AC_CHECK_LIB(tap, plan_tests, [LIBTAP=1])
AM_CONDITIONAL(HAVE_LIBTAP, test x$LIBTAP = x1)

AC_CHECK_FUNCS(gettimeofday)

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([sl/sl.h sys/time.h])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
