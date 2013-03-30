#+
#:  \file       configure.ac
#:  \brief      \$Id$
#:  
#:  (C) 2007 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

# set NHI1 VERSION information also in:
AC_INIT([NHI1],[0.17],[aotto1968@users.berlios.de],[NHI1],[http://developer.berlios.de/projects/nhi1/])
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE([gnu tar-ustar no-dist-gzip dist-bzip2 -Wno-portability])
#missing AM_EXTRA_RECURSIVE_TARGETS([perf])
AC_CONFIG_HEADER([mqconfig.h])

# set libmsgque VERSION information also in:
# > theLink/perlmsgque/Net-PerlMsgque/lib/Net/PerlMsgque.pm
# > theLink/csmsgque/MqS.cs
AC_SUBST([LIBMSGQUE_VERSION], ["5.5"])
AC_DEFINE([LIBMSGQUE_VERSION], ["5.5"], [version of the 'libmsgque' package])

# set library VERSION information also in:
# > theLink/csmsgque/MqS.cs
#
#  Updating library version information
#  ====================================
#
#  If you want to use libtool's versioning system, then you must specify
#  the version information to libtool using the `-version-info' flag
#  during link mode (*note Link mode::).
#
#     This flag accepts an argument of the form
#  `CURRENT[:REVISION[:AGE]]'.  So, passing `-version-info 3:12:1' sets
#  CURRENT to 3, REVISION to 12, and AGE to 1.
#
#     If either REVISION or AGE are omitted, they default to 0.  Also note
#  that AGE must be less than or equal to the CURRENT interface number.
#
#     Here are a set of rules to help you update your library version
#  information:
#
#    1. Start with version information of `0:0:0' for each libtool library.
#
#    2. Update the version information only immediately before a public
#       release of your software.  More frequent updates are unnecessary,
#       and only guarantee that the current interface number gets larger
#       faster.
#
#    3. If the library source code has changed at all since the last
#       update, then increment REVISION (`C:R:A' becomes `C:r+1:A').
#
#    4. If any interfaces have been added, removed, or changed since the
#       last update, increment CURRENT, and set REVISION to 0.
#
#    5. If any interfaces have been added since the last public release,
#       then increment AGE.
#
#    6. If any interfaces have been removed since the last public release,
#       then set AGE to 0.
#
#     *_Never_* try to set the interface numbers so that they correspond
#  to the release number of your package.  This is an abuse that only
#  fosters misunderstanding of the purpose of library versions.  Instead,
#  use the `-release' flag (*note Release numbers::), but be warned that
#  every release of your package will not be binary compatible with any
#  other release.
AC_SUBST([VERSION_INFO], [22:0:0])

AC_CONFIG_MACRO_DIR([m4])

AM_PROG_CC_C_O
AM_PROG_AR

GCC=yes
AC_PROG_CXX
AC_PROG_CC_C99
if test "$ac_cv_prog_cc_c99" = "no"; then AC_MSG_ERROR([require c99 mode to compile]); fi

CFLAGS="-Wall -Wcast-align $CFLAGS"
if test "$build_os" != "cygwin"; then CFLAGS="-fvisibility=hidden $CFLAGS"; fi
SC_ENABLE_THREADS

#------------------------------------
AC_MSG_CHECKING(get libtool support);echo
#------------------------------------
AC_DISABLE_STATIC
LT_INIT
LT_LIB_DLLOAD

#------------------------------------
AC_MSG_CHECKING(programs);echo
#------------------------------------
AC_C_BIGENDIAN
SC_ENABLE_SYMBOLS
SC_WITH_TOOL_ROOT

SC_ENABLE_CACHE

if test "$host_os" = "mingw32"; then
  CYGPATH_M='cygpath -m'
else
  CYGPATH_M='echo'
fi
AC_SUBST([CYGPATH_M])

SC_WITH_CXX
SC_WITH_TCL
SC_WITH_PYTHON
SC_WITH_PERL
SC_WITH_PHP
SC_WITH_RUBY
SC_WITH_JAVA
SC_WITH_CSHARP
SC_WITH_VB
SC_WITH_GO
SC_WITH_WINSDK

SC_ENABLE_BRAIN
SC_ENABLE_GUARD

#------------------------------------
AC_MSG_CHECKING(for doxygen support);echo
#------------------------------------
OT_REQUIRE_PROG([DOXYGEN],  [doxygen])
OT_REQUIRE_PROG([DOXYPERL], [perl])
AC_PATH_PROG([DOT],[dot])

#------------------------------------
AC_MSG_CHECKING([for special portability support]);echo
#------------------------------------
AC_CHECK_FUNCS([signal])
AC_CHECK_HEADERS([signal.h])
AC_CHECK_FUNCS([strtoul strtoull strtof strtod])
AC_CHECK_FUNCS([getline])

if test "$build_os" = "mingw32" -a "$OSTYPE" = "cygwin" ; then
  AC_MSG_ERROR([as '--build=...' you have chosen 'mingw32' but your calling shell is from 'cygwin'])
elif test "$host_os" = "mingw32"; then
  WINLIBS="-lws2_32"
    # FD_SET and select
    # A number of FD_XXX macros are defined in the Winsock2.h header file for use 
    # in porting applications to Windows from the UNIX environment. These macros 
    # are used with the select and WSAPoll functions for porting applications to 
    # Windows. The maximum number of sockets that a Windows Sockets application 
    # can use is not affected by the manifest constant FD_SETSIZE. This value 
    # defined in the Winsock2.h header file is used in constructing the FD_SET 
    # structures used with select function. The default value in Winsock2.h is 64. 
    # If an application is designed to be capable of working with more than 64 
    # sockets using the select and WSAPoll functions, the implementor should 
    # define the manifest FD_SETSIZE in every source file before including the 
    # Winsock2.h header file. One way of doing this may be to include the 
    # definition within the compiler options in the makefile. For example, 
    # you could add "-DFD_SETSIZE=128" as an option to the compiler command line 
    # for Microsoft C++. It must be emphasized that defining FD_SETSIZE as a 
    # particular value has no effect on the actual number of sockets provided by 
    # a Windows Sockets service provider. This value only affects the FD_XXX 
    # macros used by the select and WSAPoll functions.
  WINDEFS="-DFD_SETSIZE=1024"
  if test ! -f /cygdrive/c/WINDOWS/system32/ws2_32.dll; then
    AC_MSG_ERROR([unable to find library: /cygdrive/c/WINDOWS/system32/ws2_32.dll])
  fi
  HAS_FORK=0
else
  AC_CHECK_HEADERS([sys/filio.h])
  AC_SEARCH_LIBS([bind], [socket nsl])
  AC_SEARCH_LIBS([gethostbyname], [socket nsl])
  AC_SEARCH_LIBS([inet_aton], [socket nsl resolv])
  AC_CHECK_FUNCS([posix_spawn vfork execvp])
  AC_CHECK_FUNCS([socketpair])
  WINLIBS=""
  if test "$build_os" = "cygwin" ; then
    WINDEFS="-DFD_SETSIZE=1024"
    HAS_FORK=0
  else
    AC_CHECK_FUNCS([fork])
    WINDEFS=""
    HAS_FORK=1
  fi
fi
AC_SUBST([WINLIBS])
AC_SUBST([WINDEFS])
AC_SUBST([HAS_FORK])

#------------------------------------
AC_MSG_CHECKING(define system environment settings);echo
#------------------------------------
TYPE_SOCKLEN_T
TYPE_HUGE_VAL
AX_CHECK_ALIGNED_ACCESS_REQUIRED

AC_MSG_CHECKING([for windows build])
if test "$host_os" = "mingw32"; then
  AC_MSG_RESULT([yes])
  AC_DEFINE([MQ_IS_WIN32], [], [is this a WIN32 like system (e.g. WINDOWS)])
  AC_SUBST([FILE_SEP], [\\\\])
  AC_SUBST([PATH_SEP], [\;])
  AC_SUBST([MQ_IS], [WIN32])
  AM_CONDITIONAL([WIN32], [true])
  HAS_UDS=0
else
  AC_MSG_RESULT([no])
  AC_DEFINE([MQ_IS_POSIX], [], [is this a POSIX like system (e.g. UNIX)])
  if test "$build_os" != "cygwin"; then
    AC_DEFINE([MQ_HAVE_UDS], [], [are Unix-Domain-Socket available?])
    HAS_UDS=1
  else
    HAS_UDS=0
  fi
  AC_SUBST([FILE_SEP], [/])
  AC_SUBST([PATH_SEP], [:])
  AC_SUBST([MQ_IS], [POSIX])
  AM_CONDITIONAL([WIN32], [false])
fi
AC_SUBST([HAS_UDS])

# create some windows style path names
test "x$prefix" = xNONE && my_prefix=$ac_default_prefix
AC_SUBST([prefix_m], "$($CYGPATH_M "$my_prefix")")
AC_SUBST([abs_top_builddir_m], "$($CYGPATH_M "$ac_pwd")")
case $srcdir in
  .)  # We are building in place.
    my_abs_top_srcdir=$ac_pwd ;;
  [[\\/]]* | ?:{[\\/]]* )  # Absolute name.
    my_abs_top_srcdir=$srcdir ;;
  *) # Relative name.
    my_abs_top_srcdir=$ac_pwd/$srcdir ;;
esac
AC_SUBST([abs_top_srcdir_m], "$($CYGPATH_M "$my_abs_top_srcdir")")

AC_SUBST([path],[$PATH])
AC_SUBST([ld_library_path],[$LD_LIBRARY_PATH])

AC_CONFIG_FILES([Makefile tests/Makefile
      theLink/Makefile theLink/acmds/Makefile theLink/sqlite/Makefile theLink/tests/Makefile 
	theLink/docs/Makefile bin/Makefile sbin/Makefile etc/Makefile
	  theLink/libmsgque/Makefile theLink/example/c/Makefile
	    theLink/ccmsgque/Makefile theLink/example/cc/Makefile
	      theLink/tclmsgque/Makefile theLink/example/tcl/Makefile env.sh env.tcl
		theLink/javamsgque/Makefile theLink/example/java/Makefile theLink/javamsgque/Manifest.txt
		  theLink/pymsgque/Makefile theLink/example/python/Makefile
		    theLink/csmsgque/Makefile theLink/example/csharp/Makefile
		      theLink/msgqueforphp/Makefile theLink/example/php/Makefile theLink/msgqueforphp/php.ini
			theLink/perlmsgque/Makefile theLink/example/perl/Makefile
			  theLink/perlmsgque/Net-PerlMsgque/setup.sh theLink/perlmsgque/Net-PerlMsgque/setup.bat 
			  theLink/perlmsgque/Net-PerlMsgque/make.bat
			    theLink/rubymsgque/Makefile theLink/example/ruby/Makefile
			      theLink/gomsgque/Makefile theLink/example/go/Makefile theLink/gomsgque/src/gomsgque/gomsgque.go
				theLink/example/vb/Makefile
				  performance/Makefile performance/build_performance_env.sh performance/performance.sh
				    theBrain/Makefile theBrain/abrain/Makefile theBrain/atrans/Makefile
				      theBrain/tests/Makefile
					theGuard/Makefile theGuard/aguard/Makefile theGuard/tests/Makefile ])

AC_OUTPUT

