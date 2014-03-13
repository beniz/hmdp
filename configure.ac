#configure for hmdp
#author E. Benazera
AC_INIT(hmdp, 1.0)
AC_CONFIG_SRCDIR(Makefile.am)
AM_INIT_AUTOMAKE
AM_CONFIG_HEADER(config.h)

ac_default_prefix=`pwd`
bindir=$ac_default_prefix/bin/$host
libdir=$ac_default_prefix/lib/$host
#AC_CONFIG_AUX_DIR(.)
AC_PROG_INSTALL
AC_CANONICAL_HOST

#check for types

#dnl checks for programs
AC_PROG_CC(gcc cc)
AC_PROG_CXX(c++ g++ egcs gcc)
AC_PROG_RANLIB
AC_PROG_MAKE_SET
AC_PROG_AWK
AM_PROG_LEX
AC_PROG_YACC
AC_PROG_RANLIB

#options
#- linear programming
AC_ARG_ENABLE(lp,
AC_HELP_STRING([--enable-lp],[Enable the linear programming optimization (default is NO)]),
[case "${enableval}" in 
	yes) lp=true ;;
	no) lp=false ;;
	*) AC_MSG_ERROR(bad_value ${enableval} for --enable-lp) ;;
esac],[lp=false])
AM_CONDITIONAL(LP, test x$lp = xtrue)

#- probabilistic pddl parser
AC_ARG_ENABLE(ppddl,
AC_HELP_STRING([--enable-ppddl],[Enable the connection to a probabilistic pddl parser (default is YES)]),
[case "${enableval}" in
	yes) ppddl=true ;;
	no) ppddl=false ;;
	*) AC_MSG_ERROR(bad_value ${enableval} for --enable-ppddl) ;;
esac],[ppddl=true])
AM_CONDITIONAL(PPDDL, test x$ppddl = xtrue)

#- structures for coverage-set algorithm (CSA)
AC_ARG_ENABLE(csa,
AC_HELP_STRING([--enable-csa],[Enable the data structures for coverage-set algorithm (default is NO)]),
[case "${enableval}" in
	yes) csa=true ;;
	no) csa=false ;;
	*) AC_MSG_ERROR(bad_value ${enableval} for --enable-csa) ;;
esac],[csa=false])
AM_CONDITIONAL(CSA, test x$csa = xtrue)

#compilation flags
#- Linear programming
if test x$lp = xtrue; then
	AC_DEFINE(HAVE_LP,1,"Linear Programming")
fi
#- Probabilistic pddl
if test x$ppddl = xtrue; then
	AC_DEFINE(HAVE_PPDDL,1,"Probabilistic ppddl for planning domain & problem modeling")
fi
#- CSA
if test x$csa = xtrue; then
	AC_DEFINE(HAVE_CSA,1,"Data structures for Coverage Set Algorithm")
fi

# Check if there is google-gflags library installed.
SAVE_CFLAGS="$CFLAGS"
SAVE_LIBS="$LIBS"
AC_ARG_WITH(gflags, AS_HELP_STRING[--with-gflags=GFLAGS_DIR],
  GFLAGS_CFLAGS="-I${with_gflags}/include"
  GFLAGS_LIBS="-L${with_gflags}/lib -lgflags"
  CFLAGS="$CFLAGS $GFLAGS_CFLAGS"
  LIBS="$LIBS $GFLAGS_LIBS"
)
AC_CHECK_LIB(gflags, main, ac_cv_have_libgflags=1, ac_cv_have_libgflags=0)
if test x"$ac_cv_have_libgflags" = x"1"; then
   AC_DEFINE(HAVE_LIB_GFLAGS, 1, [define if you have google gflags library])
   if test x"$GFLAGS_LIBS" = x""; then
      GFLAGS_LIBS="-lgflags"
   fi
else
  GFLAGS_CFLAGS=
  GFLAGS_LIBS=
fi

#output
AC_OUTPUT(Makefile src/Makefile src/base/Makefile src/tests/Makefile src/loaders/Makefile src/engine/Makefile src/csa/Makefile src/hmdpsim/Makefile src/apps/Makefile)
