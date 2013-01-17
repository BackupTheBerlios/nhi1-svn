dnl
dnl  \file       acinclude.m4
dnl  \brief      \$Id$
dnl  
dnl  (C) 2007 - NHI - #1 - Project - Group
dnl  
dnl  \version    \$Rev$
dnl  \author     EMail: aotto1968 at users.berlios.de
dnl  \attention  this software has GPL permissions to copy
dnl              please contact AUTHORS for additional information
dnl

dnl get the socklen_t type

AC_DEFUN([TYPE_SOCKLEN_T],
  [AC_CACHE_CHECK([for socklen_t], ac_cv_type_socklen_t, [
    AC_COMPILE_IFELSE([
      AC_LANG_PROGRAM([
#include <sys/types.h>
#include <sys/socket.h>
        ],[
socklen_t len = 42;
      ])] ,
      [ac_cv_type_socklen_t=yes],
      [ac_cv_type_socklen_t=no]
    )
  ])]
  if test "$ac_cv_type_socklen_t" != "yes"; then
    AC_DEFINE(socklen_t, int, [Substitute for socklen_t])
  fi
)

dnl check for HUGE_VAL

AC_DEFUN([TYPE_HUGE_VAL],
  [AC_CACHE_CHECK([for HUGE_VAL is a function], ac_cv_type_huge_val, [
    AC_COMPILE_IFELSE([
      AC_LANG_PROGRAM([
#include <math.h>
       ],[
double len = (HUGE_VAL()); 
      ])],
      [ac_cv_type_huge_val=yes],
      [ac_cv_type_huge_val=no]
    )
  ])]
  if test "$ac_cv_type_huge_val" = "yes"; then
    AC_DEFINE([MQ_HUGE_VAL], [(HUGE_VAL())], [Substitute for HUGE_VAL])
    AC_DEFINE([MQ_HUGE_VALF], [(HUGE_VALF())], [Substitute for HUGE_VALF])
  else
    AC_DEFINE([MQ_HUGE_VAL], [HUGE_VAL], [Substitute for HUGE_VAL])
    AC_DEFINE([MQ_HUGE_VALF], [HUGE_VALF], [Substitute for HUGE_VALF])
  fi
)

dnl test on alignment

AC_DEFUN([AX_CHECK_ALIGNED_ACCESS_REQUIRED],[
  AC_CACHE_CHECK([if pointers to integers require aligned access],
    [ax_cv_have_aligned_access_required], [
    AC_COMPILE_IFELSE([
      AC_LANG_PROGRAM([
#include <stdio.h>
#include <stdlib.h>
	],[
char* string = malloc(40);
int i;
for (i=0; i < 40; i++) string[[i]] = 255;
void* s = string;
int* p = s+1;
int* q = s+2;
if (*p != *q) { return(1); }
      ])],
      [ax_cv_have_aligned_access_required=no],
      [ax_cv_have_aligned_access_required=yes]
    )
  ])]
  if test "$ax_cv_have_aligned_access_required" = yes ; then
    AC_DEFINE([HAVE_ALIGNED_ACCESS_REQUIRED], [1],
      [Define if pointers to integers require aligned access])
  fi
)

#------------------------------------------------------------------------
# SC_SET_VPATH_HOOK --
#
#       add support for VPATH build
#
# Arguments:
#       none
#	need variable "VPATH_FILES" be defined
#
# Results:
#
#       Add a new variable VPATH_HOOK and VPATH_HOOK cleanup to automake
#
#------------------------------------------------------------------------

AC_DEFUN([SC_SET_VPATH_HOOK], [
    AC_MSG_CHECKING([add VPATH hook])
    AC_SUBST([VPATH_HOOK], ['@if test ! -f .vpath_files -a "$(srcdir)" != "." ; then (cd "$(srcdir)" && cp -r $(VPATH_FILES) "$(abs_builddir)";) && touch ".vpath_files" && chmod -R u+w $(VPATH_FILES) && echo "VPATH copy"; else true; fi && touch ".vpath_hook"'])
    AC_SUBST([VPATH_HOOK_CLEANUP], ['-rm -f .vpath_hook; test -f .vpath_files && rm -fr .vpath_files $(VPATH_FILES)'])
    AC_SUBST([VPATH_HOOK_DIST], ['-rm -f $(distdir)/.vpath_hook'])
])

#------------------------------------------------------------------------
# SC_ENABLE_SYMBOLS --
#
#       Specify if debugging symbols should be used.
#
# Arguments:
#       none
#
#       Requires the following vars to be set in configure.in:
#               CFLAGS
#
# Results:
#
#       Adds the following arguments to configure:
#               --enable-symbols
#
#------------------------------------------------------------------------

AC_DEFUN([SC_ENABLE_SYMBOLS], [
    AC_MSG_CHECKING([for build with symbols])
    AC_ARG_ENABLE(symbols,
	AC_HELP_STRING([--enable-symbols], [build with debugging support]),
	enable_symbols=yes, enable_symbols=no
    )
    if test "x$enable_symbols" = "xyes"; then
      CFLAGS="-g $CFLAGS"
      CPPFLAGS="-D_DEBUG $CPPFLAGS"
      AC_SUBST([VB_DEBUG], [-debug:full])
      AC_SUBST([JAVA_DEBUG], [-g])
      AC_SUBST([CSHARP_DEBUG], ['-debug -define:_DEBUG'])
    else
      CFLAGS="-O3 $CFLAGS"
      CXXFLAGS="-O3 $CXXFLAGS"
      CPPFLAGS="-DNDEBUG $CPPFLAGS"
    fi
    AM_CONDITIONAL([DEBUG], [test "$enable_symbols" = "yes"])
    AC_MSG_RESULT($symbol)
])

#------------------------------------------------------------------------
# ACX_PTHREAD --
#
# code from: http://autoconf-archive.cryp.to/acx_pthread.html
#------------------------------------------------------------------------
AC_DEFUN([ACX_PTHREAD], [
AC_REQUIRE([AC_CANONICAL_HOST])
AC_LANG_SAVE
AC_LANG_C
acx_pthread_ok=no

# We used to check for pthread.h first, but this fails if pthread.h
# requires special compiler flags (e.g. on True64 or Sequent).
# It gets checked for in the link test anyway.

# First of all, check if the user has set any of the PTHREAD_LIBS,
# etcetera environment variables, and if threads linking works using
# them:
if test x"$PTHREAD_LIBS$PTHREAD_CFLAGS" != x; then
        save_CFLAGS="$CFLAGS"
        CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
        save_LIBS="$LIBS"
        LIBS="$PTHREAD_LIBS $LIBS"
        AC_MSG_CHECKING([for pthread_join in LIBS=$PTHREAD_LIBS with CFLAGS=$PTHREAD_CFLAGS])
        AC_TRY_LINK_FUNC(pthread_join, acx_pthread_ok=yes)
        AC_MSG_RESULT($acx_pthread_ok)
        if test x"$acx_pthread_ok" = xno; then
                PTHREAD_LIBS=""
                PTHREAD_CFLAGS=""
        fi
        LIBS="$save_LIBS"
        CFLAGS="$save_CFLAGS"
fi

# We must check for the threads library under a number of different
# names; the ordering is very important because some systems
# (e.g. DEC) have both -lpthread and -lpthreads, where one of the
# libraries is broken (non-POSIX).

# Create a list of thread flags to try.  Items starting with a "-" are
# C compiler flags, and other items are library names, except for "none"
# which indicates that we try without any flags at all, and "pthread-config"
# which is a program returning the flags for the Pth emulation library.

acx_pthread_flags="pthreads none -Kthread -kthread lthread -pthread -pthreads -mthreads pthread --thread-safe -mt pthread-config"

# The ordering *is* (sometimes) important.  Some notes on the
# individual items follow:

# pthreads: AIX (must check this before -lpthread)
# none: in case threads are in libc; should be tried before -Kthread and
#       other compiler flags to prevent continual compiler warnings
# -Kthread: Sequent (threads in libc, but -Kthread needed for pthread.h)
# -kthread: FreeBSD kernel threads (preferred to -pthread since SMP-able)
# lthread: LinuxThreads port on FreeBSD (also preferred to -pthread)
# -pthread: Linux/gcc (kernel threads), BSD/gcc (userland threads)
# -pthreads: Solaris/gcc
# -mthreads: Mingw32/gcc, Lynx/gcc
# -mt: Sun Workshop C (may only link SunOS threads [-lthread], but it
#      doesn't hurt to check since this sometimes defines pthreads too;
#      also defines -D_REENTRANT)
#      ... -mt is also the pthreads flag for HP/aCC
# pthread: Linux, etcetera
# --thread-safe: KAI C++
# pthread-config: use pthread-config program (for GNU Pth library)

case "${host_cpu}-${host_os}" in
        *solaris*)

        # On Solaris (at least, for some versions), libc contains stubbed
        # (non-functional) versions of the pthreads routines, so link-based
        # tests will erroneously succeed.  (We need to link with -pthreads/-mt/
        # -lpthread.)  (The stubs are missing pthread_cleanup_push, or rather
        # a function called by this macro, so we could check for that, but
        # who knows whether they'll stub that too in a future libc.)  So,
        # we'll just look for -pthreads and -lpthread first:

        acx_pthread_flags="-pthreads pthread -mt -pthread $acx_pthread_flags"
        ;;
esac

if test x"$acx_pthread_ok" = xno; then
for flag in $acx_pthread_flags; do

        case $flag in
                none)
                AC_MSG_CHECKING([whether pthreads work without any flags])
                ;;

                -*)
                AC_MSG_CHECKING([whether pthreads work with $flag])
                PTHREAD_CFLAGS="$flag"
                ;;

                pthread-config)
                AC_CHECK_PROG(acx_pthread_config, pthread-config, yes, no)
                if test x"$acx_pthread_config" = xno; then continue; fi
                PTHREAD_CFLAGS="`pthread-config --cflags`"
                PTHREAD_LIBS="`pthread-config --ldflags` `pthread-config --libs`"
                ;;

                *)
                AC_MSG_CHECKING([for the pthreads library -l$flag])
                PTHREAD_LIBS="-l$flag"
                ;;
        esac

        save_LIBS="$LIBS"
        save_CFLAGS="$CFLAGS"
        LIBS="$PTHREAD_LIBS $LIBS"
        CFLAGS="$CFLAGS $PTHREAD_CFLAGS"

        # Check for various functions.  We must include pthread.h,
        # since some functions may be macros.  (On the Sequent, we
        # need a special flag -Kthread to make this header compile.)
        # We check for pthread_join because it is in -lpthread on IRIX
        # while pthread_create is in libc.  We check for pthread_attr_init
        # due to DEC craziness with -lpthreads.  We check for
        # pthread_cleanup_push because it is one of the few pthread
        # functions on Solaris that doesn't have a non-functional libc stub.
        # We try pthread_create on general principles.
        AC_TRY_LINK([#include <pthread.h>],
                    [pthread_t th; pthread_join(th, 0);
                     pthread_attr_init(0); pthread_cleanup_push(0, 0);
                     pthread_create(0,0,0,0); pthread_cleanup_pop(0); ],
                    [acx_pthread_ok=yes])

        LIBS="$save_LIBS"
        CFLAGS="$save_CFLAGS"

        AC_MSG_RESULT($acx_pthread_ok)
        if test "x$acx_pthread_ok" = xyes; then
                break;
        fi

        PTHREAD_LIBS=""
        PTHREAD_CFLAGS=""
done
fi

# Various other checks:
if test "x$acx_pthread_ok" = xyes; then
        save_LIBS="$LIBS"
        LIBS="$PTHREAD_LIBS $LIBS"
        save_CFLAGS="$CFLAGS"
        CFLAGS="$CFLAGS $PTHREAD_CFLAGS"

        # Detect AIX lossage: JOINABLE attribute is called UNDETACHED.
        AC_MSG_CHECKING([for joinable pthread attribute])
        attr_name=unknown
        for attr in PTHREAD_CREATE_JOINABLE PTHREAD_CREATE_UNDETACHED; do
            AC_TRY_LINK([#include <pthread.h>], [int attr=$attr; return attr;],
                        [attr_name=$attr; break])
        done
        AC_MSG_RESULT($attr_name)
        if test "$attr_name" != PTHREAD_CREATE_JOINABLE; then
            AC_DEFINE_UNQUOTED(PTHREAD_CREATE_JOINABLE, $attr_name,
                               [Define to necessary symbol if this constant
                                uses a non-standard name on your system.])
        fi

        AC_MSG_CHECKING([if more special flags are required for pthreads])
        flag=no
        case "${host_cpu}-${host_os}" in
            *-aix* | *-freebsd* | *-darwin*) flag="-D_THREAD_SAFE";;
            *solaris* | *-osf* | *-hpux*) flag="-D_REENTRANT";;
        esac
        AC_MSG_RESULT(${flag})
        if test "x$flag" != xno; then
            PTHREAD_CFLAGS="$flag $PTHREAD_CFLAGS"
        fi

        LIBS="$save_LIBS"
        CFLAGS="$save_CFLAGS"

        # More AIX lossage: must compile with xlc_r or cc_r
        if test x"$GCC" != xyes; then
          AC_CHECK_PROGS(PTHREAD_CC, xlc_r cc_r, ${CC})
        else
          PTHREAD_CC=$CC
        fi
else
        PTHREAD_CC="$CC"
fi

AC_SUBST(PTHREAD_LIBS)
AC_SUBST(PTHREAD_CFLAGS)
AC_SUBST(PTHREAD_CC)

# Finally, execute ACTION-IF-FOUND/ACTION-IF-NOT-FOUND:
if test x"$acx_pthread_ok" = xyes; then
        ifelse([$1],,AC_DEFINE(HAVE_PTHREAD,1,[Define if you have POSIX threads libraries and header files.]),[$1])
        :
else
        acx_pthread_ok=no
        $2
fi
AC_LANG_RESTORE
])
dnl ACX_PTHREAD


#------------------------------------------------------------------------
# ACX_TLS --
#
# code from: http://autoconf-archive.cryp.to/acx_tls.html
#------------------------------------------------------------------------
AC_DEFUN([AX_TLS], [
  AC_MSG_CHECKING(for thread local storage (TLS) class)
  AC_CACHE_VAL(ac_cv_tls, [
    ax_tls_keywords="__thread __declspec(thread) none"
    for ax_tls_keyword in $ax_tls_keywords; do
       case $ax_tls_keyword in
          none) ac_cv_tls=none ; break ;;
          *)
             AC_TRY_COMPILE(
                [#include <stdlib.h>
                 static void
                 foo(void) {
                 static ] $ax_tls_keyword [ int bar;
                 exit(1);
                 }],
                 [],
                 [ac_cv_tls=$ax_tls_keyword ; break],
                 ac_cv_tls=none
             )
          esac
    done
])

  if test "$ac_cv_tls" != "none"; then
    dnl AC_DEFINE([TLS], [], [If the compiler supports a TLS storage class define it to that here])
    AC_DEFINE_UNQUOTED([TLS], $ac_cv_tls, [If the compiler supports a TLS storage class define it to that here])
  fi
  AC_MSG_RESULT($ac_cv_tls)
])

#------------------------------------------------------------------------
# SC_ENABLE_THREAD --
#
#       Specify if thread support is needed
#       UNIX:  try to use pthread
#       WIN32: use native threads
#
# Arguments:
#       none
#
# Results:
#
#------------------------------------------------------------------------

AC_DEFUN([SC_ENABLE_THREADS], [
  AC_MSG_CHECKING([for build with thread])
  AC_ARG_ENABLE(threads,
      AC_HELP_STRING([--enable-threads], [build with thread support]),
      enable_threads=yes, enable_threads=no
  )
  AC_MSG_RESULT($enable_threads)
  if test "$enable_threads" = "yes"; then
    # we use native windows threads
    if test "$host_os" != "mingw32" ; then
      ACX_PTHREAD
      if test "$acx_pthread_ok" == "yes" ; then
	LIBS="$PTHREAD_LIBS $LIBS"
	CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
	CC="$PTHREAD_CC"
	#AX_TLS
      fi
    fi
    if test "$host_os" == "mingw32" -o "$acx_pthread_ok" == "yes" ; then
      AC_DEFINE([MQ_HAS_THREAD], [], [does the user require thread support])
    fi
  fi
  AM_CONDITIONAL([HAS_THREAD], [test x$enable_threads = xyes])
])

#------------------------------------------------------------------------
# SC_ENABLE_JAVA --
#
#       Specify if java support is needed
#
# Arguments:
#       none
#
# Results:
#
#------------------------------------------------------------------------

AC_DEFUN([SC_ENABLE_JAVA], [
  AC_MSG_CHECKING([for build with java])
  AC_ARG_ENABLE(java,
      AC_HELP_STRING([--enable-java], [build theLink with JAVA support]),
      enable_java=yes, enable_java=no
  )
  AC_MSG_RESULT($enable_java)
  if test x$enable_java = xyes; then
    if test x$enable_threads = xno; then
      AC_MSG_ERROR([JAVA require thread support])
    fi
    m4_include([m4/ac_java.m4])
    #------------------------------------
    AC_MSG_CHECKING(get java support);echo
    #------------------------------------
    AC_ARG_VAR([JAVA], [path to the 'java' tool])
    AC_PATH_PROG([JAVA], [java]) 
    if test "$build_os" == "cygwin"; then
      JAVA=$(cygpath -m "$JAVA")
    fi
    AC_ARG_VAR([JAVAC], [path to the 'javac' tool])
    AC_ARG_VAR([JAVAH], [path to the 'javah' tool])
    AC_PATH_PROG([JAVAH], [javah]) 
    AC_ARG_VAR([JAR], [path to the 'jar' tool])
    AC_PATH_PROG([JAR], [jar]) 
    AC_PROG_JAVAC
    AC_CHECK_CLASSPATH
    AC_JNI_INCLUDE_DIR
    for JNI_INCLUDE_DIR in $JNI_INCLUDE_DIRS
    do
      JAVA_CPPFLAGS="$JAVA_CPPFLAGS -I$JNI_INCLUDE_DIR"
    done
    AC_SUBST([JAVA_CPPFLAGS])
  fi
  AC_SUBST([USE_JAVA], $enable_java)
  AM_CONDITIONAL([USE_JAVA], [test x$enable_java = xyes])
])

#------------------------------------------------------------------------
# SC_ENABLE_PYTHON --
#
#       Specify if python support is needed
#
# Arguments:
#       none
#
# Results:
#
#------------------------------------------------------------------------

AC_DEFUN([SC_ENABLE_PYTHON], [
  AC_MSG_CHECKING([for build with python])
  AC_ARG_ENABLE(python,
      AC_HELP_STRING([--enable-python], [build theLink with PYTHON support]),
      enable_python=yes, enable_python=no
  )
  AC_MSG_RESULT($enable_python)
  if test x$enable_python = xyes; then
    PYTHON_VERSION=3
    m4_include([m4/ac_python_devel.m4])
    #------------------------------------
    AC_MSG_CHECKING(get python support);echo
    #------------------------------------
    AC_PYTHON_DEVEL([>= '3.0'])
    #AM_PATH_PYTHON([3.0])
  fi
  AC_SUBST([USE_PYTHON], $enable_python)
  AM_CONDITIONAL([USE_PYTHON], [test x$enable_python = xyes])
])

#------------------------------------------------------------------------
# SC_ENABLE_CSHARP --
#
#       Specify if C# support is needed
#
# Arguments:
#       none
#
# Results:
#
#------------------------------------------------------------------------

AC_DEFUN([SC_ENABLE_CSHARP], [
  AC_MSG_CHECKING([for build with csharp])
  AC_ARG_ENABLE(csharp,
      AC_HELP_STRING([--enable-csharp], [build theLink with C# support]),
      enable_csharp=yes, enable_csharp=no
  )
  AC_MSG_RESULT($enable_csharp)
  if test x$enable_csharp = xyes; then
    if test x$enable_threads = xno; then
      AC_MSG_ERROR([C[#] require thread support])
    fi
    AC_ARG_VAR( [CSCOMP], [C# compiler])
    AC_ARG_VAR( [CLREXEC], [CLR runtime])
    if test "$host_os" == "mingw32" ; then
      AC_PATH_PROGS( [CSCOMP], [csc] )
      if test "x$CSCOMP" == "x"; then
	AC_MSG_ERROR([unable to find useable CSCOMP])
      fi
      CLREXEC=""
      if test "x$enable_symbols" = "xno"; then
	AC_SUBST([CSHARP_DEBUG], ['-optimize'])
      fi
      AC_SUBST([CSHARP_OPT], [])
    else
      AC_PROG_AWK
      AC_PATH_PROGS( [CSCOMP], [gmcs] )
      AC_PATH_PROGS( [CLREXEC], [mono] )
      if test x$CSCOMP = x ; then
	AC_MSG_ERROR([unable to find C[#] compiler 'gmcs'])
      fi
      if test x$CLREXEC = x ; then
	AC_MSG_ERROR([unable to find CLR runtime 'mono'])
      fi
      if $CLREXEC -V | $AWK '/version/ {found=1;if ([$]5 < 2.4) exit(0);else exit(1);};END {if(found==0) exit(0);}' ; then
	AC_MSG_ERROR([mono version have to be >= 2.4])
      fi
      AC_SUBST([CSHARP_OPT], [-v])
    fi
  fi
  AC_SUBST([USE_CSHARP], $enable_csharp)
  AM_CONDITIONAL([USE_CSHARP], [test x$enable_csharp = xyes])
])

#------------------------------------------------------------------------
# SC_ENABLE_VB --
#
#       Specify if VisualBasic (VB) support is needed
#
# Arguments:
#       none
#
# Results:
#
#------------------------------------------------------------------------

AC_DEFUN([SC_ENABLE_VB], [
  AC_MSG_CHECKING([for build with VB])
  AC_ARG_ENABLE(vb,
      AC_HELP_STRING([--enable-vb], [build theLink with VB support]),
      enable_vb=yes, enable_vb=no
  )
  AC_MSG_RESULT($enable_vb)
  if test x$enable_vb = xyes; then
    if test x$enable_csharp = no ; then
      AC_MSG_ERROR([a VisualBasic build "--enable-vb requires a C+ build "--enable-csharp" too])
    fi
    if test "$host_os" != "mingw32" ; then
      AC_ARG_VAR( [VBCOMP], [VisualBasic compiler])
      AC_PATH_PROGS( [VBCOMP], [vbnc] )
      if test "x${VBCOMP}" = "x" ; then
	AC_MSG_ERROR([unable to find VisualBasic compiler 'vbnc'])
      fi
    else
      AC_ARG_VAR( [VBCOMP], [VisualBasic compiler])
      AC_PATH_PROGS( [VBCOMP], [vbc] )
      if test "x${VBCOMP}" = "x" ; then
	AC_MSG_ERROR([unable to find VisualBasic compiler 'vbc'])
      fi
    fi
  fi
  AC_SUBST([USE_VB], $enable_vb)
  AM_CONDITIONAL([USE_VB], [test x$enable_vb = xyes])
])

#------------------------------------------------------------------------
# SC_ENABLE_CXX --
#
#       Specify if C++ support is needed
#
# Arguments:
#       none
#
# Results:
#
#------------------------------------------------------------------------

AC_DEFUN([SC_ENABLE_CXX], [
  AC_MSG_CHECKING([for build with C++])
  AC_ARG_ENABLE(cxx,
      AC_HELP_STRING([--enable-cxx], [build theLink with C++ support]),
      enable_cxx=yes, enable_cxx=no
  )
  AC_MSG_RESULT($enable_cxx)
  if test "x$enable_cxx" = "xyes"; then
    dnl AC_PROG_CXX
    if test "x$enable_symbols" = "xyes"; then
      CXXFLAGS="-g $CXXFLAGS"
    fi
    if test "x$enable_threads" = "xyes"; then
      CXXFLAGS="$CXXFLAGS $PTHREAD_CFLAGS"
    fi
  fi
  AC_SUBST([USE_CXX], $enable_cxx)
  AM_CONDITIONAL([USE_CXX], [test x$enable_cxx = xyes])
])

#------------------------------------------------------------------------
# SC_ENABLE_PHP --
#
#       Specify if PHP support is needed
#
# Arguments:
#       none
#
# Results:
#
#------------------------------------------------------------------------

AC_DEFUN([SC_ENABLE_PHP], [
  AC_MSG_CHECKING([for build with PHP])
  AC_ARG_ENABLE(php,
      AC_HELP_STRING([--enable-php], [build theLink with PHP support]),
      enable_php=yes, enable_php=no
  )
  AC_MSG_RESULT($enable_php)
  if test x$enable_php = xyes; then
    AC_ARG_VAR([PHP], [path to the 'php' tool])
    AC_PATH_PROG([PHP], [php]) 
    AC_ARG_VAR([PHPIZE], [path to the 'phpize' tool])
    AC_PATH_PROG([PHPIZE], [phpize]) 
    AC_ARG_VAR([PHPCONFIG], [path to the 'php-config' tool])
    AC_PATH_PROG([PHPCONFIG], [php-config]) 
  fi
  AC_SUBST([USE_PHP], $enable_php)
  AM_CONDITIONAL([USE_PHP], [test x$enable_php = xyes])
])

#------------------------------------------------------------------------
# SC_ENABLE_GO --
#
#       Specify if GO support is needed
#
# Arguments:
#       none
#
# Results:
#
#------------------------------------------------------------------------

#AC_DEFUN([SC_ENABLE_GO], [
#  AC_SUBST([USE_GO], [no])
#  AM_CONDITIONAL([USE_GO], [0])
#])

AC_DEFUN([SC_ENABLE_GO], [
  AC_SUBST([USE_GO], [no])
  AC_MSG_CHECKING([for build with GO])
  AC_ARG_ENABLE(go,
      AC_HELP_STRING([--enable-go], [build theLink with GO support]),
      enable_go=yes, enable_go=no
  )
  AC_MSG_RESULT($enable_go)
  if test x$enable_go = xyes; then
    AC_ARG_VAR([GOROOT], [directory of the 'go' installation])
    if test x"$GOROOT" = x ; then
      AC_MSG_ERROR([GOROOT is required to use GO])
    else
      AC_MSG_RESULT([checking for build with GOROOT... $GOROOT])
    fi
  fi
  AC_SUBST([USE_GO], $enable_go)
  AM_CONDITIONAL([USE_GO], [test x$enable_go = xyes])
])

#------------------------------------------------------------------------
# SC_ENABLE_PERL --
#
#       Specify if PERL support is needed
#
# Arguments:
#       none
#
# Results:
#
#------------------------------------------------------------------------

AC_DEFUN([SC_ENABLE_PERL], [
  AC_MSG_CHECKING([for build with PERL])
  AC_ARG_ENABLE(perl,
      AC_HELP_STRING([--enable-perl], [build theLink with PERL support]),
      enable_perl=yes, enable_perl=no
  )
  AC_MSG_RESULT($enable_perl)
  if test x$enable_perl = xyes; then
    AC_ARG_VAR([PERL], [perl runtime])
    AC_PATH_PROGS([PERL], [perl]) 
    if test "x$PERL" == "x"; then
      AC_MSG_ERROR([unable to find useable PERL])
    fi
    if test "$build_os" == "cygwin"; then
      PERL=$(cygpath -m "$PERL")
    fi
  fi
  AC_SUBST([USE_PERL], $enable_perl)
  AM_CONDITIONAL([USE_PERL], [test x$enable_perl = xyes])
])

#------------------------------------------------------------------------
# SC_ENABLE_TCL --
#
#       Specify if TCL support is needed
#
# Arguments:
#       none
#
# Results:
#
#------------------------------------------------------------------------

AC_DEFUN([SC_ENABLE_TCL], [
  AC_MSG_CHECKING([for build with TCL])
  AC_ARG_ENABLE(tcl, 
      AC_HELP_STRING([--enable-tcl], [build theLink with TCL support]),
      enable_tcl=yes, enable_tcl=no
  )
  AC_MSG_RESULT($enable_tcl)
  if test x$enable_tcl = xyes; then
    AC_ARG_VAR([TCLSH], [tcl runtime])
    AC_PATH_PROGS( [TCLSH], [tclsh86 tclsh86g tclsh8.6 tclsh85 tclsh85g tclsh8.5 tclsh] ) 
    if test "x$TCLSH" == "x"; then
      AC_MSG_ERROR([unable to find useable TCLSH])
    fi
    if test "$build_os" == "cygwin"; then
      TCLSH=$(cygpath -m "$TCLSH")
    fi
    AC_SUBST([TCLBINDIR], [$(dirname $TCLSH)])
    #-----------------------------------
    AC_MSG_CHECKING(for location of tclConfig.sh script)
    #-----------------------------------
    AC_ARG_WITH(tclcfg_path,
	[  --with-tclcfg-path=DIR  Directory tclConfig.sh is located in.],
	[ tclcnf="${withval}" ],
	[ tclcnf="$TCLBINDIR/../lib $TCLBINDIR/../lib64 /lib /lib64 /lib32 /usr/lib /usr/local/lib /usr/local/tcl/lib" ])

    for prim in $tclcnf; do
      for try in ${prim} ${prim}/tcl8.* ; do
	if test -f $try/tclConfig.sh; then
	    AC_MSG_RESULT($try/tclConfig.sh)
	    . $try/tclConfig.sh
	    TCL_LIBS="$TCL_LIB_SPEC $TCL_LIBS"
	    AC_SUBST(TCL_PACKAGE_PATH)
	    TCL_INCLUDE_DIR="${TCL_INCLUDE_SPEC#-I}"
	    test -z "$TCL_INCLUDE_DIR" && TCL_INCLUDE_DIR="$try/../include"

	    ## add support for tcl STUBS
	    if test "$build_os" = "cygwin" ; then
		AC_SUBST([TCL_CFLAGS], [])
		AC_SUBST([TCL_LIBADD], [${TCL_LIB_SPEC}])
	    else
		if test ${TCL_SUPPORTS_STUBS} ; then
		    AC_SUBST([TCL_CFLAGS], [-DUSE_TCL_STUBS])
		    AC_SUBST([TCL_LIBADD], [${TCL_STUB_LIB_SPEC}])
		fi
	    fi

	    ## check if a public 'tcl.h' header file is available
	    if test ! -f "${TCL_INCLUDE_DIR}/tcl.h"; then
		AC_MSG_ERROR([Could not find tcl.h in ${TCL_INCLUDE_DIR}])
	    fi

	    ## check for empty TCL_INCLUDE_SPEC
	    test -z ${TCL_INCLUDE_SPEC} && TCL_INCLUDE_SPEC="-I${TCL_INCLUDE_DIR}"

	    AC_SUBST(TCL_INCLUDE_SPEC)
	    break 2
	fi
      done
    done

    if test -z "$TCL_LIBS"; then
	AC_MSG_RESULT([not found])
	AC_MSG_ERROR([Could not find tclConfig.sh in any of '$tclcnf'])
    fi
  fi
  AC_SUBST([USE_TCL], $enable_tcl)
  AM_CONDITIONAL([USE_TCL], [test x$enable_tcl = xyes])
])

#------------------------------------------------------------------------
# SC_ENABLE_RUBY --
#
#       Specify if RUBY support is needed
#
# Arguments:
#       none
#
# Results:
#
#------------------------------------------------------------------------

AC_DEFUN([SC_ENABLE_RUBY], [
  AC_MSG_CHECKING([for build with RUBY])
  AC_ARG_ENABLE(ruby, 
      AC_HELP_STRING([--enable-ruby], [build theLink with RUBY support]),
      enable_ruby=yes, enable_ruby=no
  )
  AC_MSG_RESULT($enable_ruby)
  if test "x$enable_ruby" = xyes; then
    AX_WITH_RUBY([no])
    if test "x$RUBY" = "xno"; then
      AC_MSG_ERROR([Could not find "ruby"])
    fi
    AX_PROG_RUBY_VERSION([1.9.2], ruby_version_flag=ok, ruby_version_flag=ko)
    if test "x$ruby_version_flag" = "xko"; then
      AC_MSG_ERROR([Could not find RUBY version 1.9.2 or later])
    fi
    AX_RUBY_DEVEL($ruby_version)
  fi
  AC_SUBST([USE_RUBY], $enable_ruby)
  AM_CONDITIONAL([USE_RUBY], [test "x$enable_ruby" = xyes])
])

#------------------------------------------------------------------------
# SC_ENABLE_BRAIN --
#
#       Specify if BRAIN database support is needed
#
# Arguments:
#       none
#
# Results:
#
#------------------------------------------------------------------------

AC_DEFUN([SC_ENABLE_BRAIN], [
  AC_MSG_CHECKING([for build with theBrain (only on UNIX)])
  AC_ARG_ENABLE(brain,
      AC_HELP_STRING([--enable-brain], [build theBrain, NHI1 database support]),
      enable_brain=yes, enable_brain=no
  )
  AC_MSG_RESULT($enable_brain)
  if test x$enable_brain = xyes; then
    BRAIN_CPPFLAGS="-D_GNU_SOURCE=1 -D_REENTRANT -D__EXTENSIONS__"
    BRAIN_CFLAGS="-pedantic -fsigned-char"
    ## check endian
    AC_C_BIGENDIAN(BRAIN_CPPFLAGS="$BRAIN_CPPFLAGS -D_MYBIGEND")
    # Fastest mode
    AC_ARG_ENABLE(fastest,
      AC_HELP_STRING([--enable-fastest], [build theBrain for fastest run]))
    if test "$enable_fastest" = "yes"
    then
      BRAIN_CFLAGS="-std=c99 -Wall -fPIC -pedantic -fsigned-char -O3"
      BRAIN_CFLAGS="$BRAIN_CFLAGS -fomit-frame-pointer -fforce-addr -minline-all-stringops"
      BRAIN_CPPFLAGS="$BRAIN_CPPFLAGS -D_MYFASTEST"
    fi

    # 64-bit offset mode
    AC_ARG_ENABLE(off64,
      AC_HELP_STRING([--enable-off64], [build theBrain with 64-bit file offset on 32-bit system]))
    if test "$enable_off64" = "yes"
    then
      BRAIN_CPPFLAGS="$BRAIN_CPPFLAGS -D_FILE_OFFSET_BITS=64"
    fi

    # Swapping byte-orders mode
    AC_ARG_ENABLE(swab,
      AC_HELP_STRING([--enable-swab], [build theBrain for swapping byte-orders]))
    if test "$enable_swab" = "yes"
    then
      BRAIN_CPPFLAGS="$BRAIN_CPPFLAGS -D_MYSWAB"
    fi

    # Micro yield mode
    AC_ARG_ENABLE(uyield,
      AC_HELP_STRING([--enable-uyield], [build theBrain for detecting race conditions]))
    if test "$enable_uyield" = "yes"
    then
      BRAIN_CPPFLAGS="$BRAIN_CPPFLAGS -D_MYMICROYIELD"
    fi
    # Disable the unified buffer cache assumption
    AC_ARG_ENABLE(ubc,
      AC_HELP_STRING([--disable-ubc], [build theBrain without the unified buffer cache assumption]))
    if test "$enable_ubc" = "no"
    then
      BRAIN_CPPFLAGS="$BRAIN_CPPFLAGS -D_MYNOUBC"
    fi
    ## check for thread support
    if test "$enable_threads" = "yes" ; then
      AC_CHECK_LIB(rt, main,
	[BRAIN_LIBADD="$BRAIN_LIBADD -lrt"], 
	[AC_MSG_FAILURE([realtime library '-lrt' not found])]
      )
    else
      BRAIN_CPPFLAGS="$BRAIN_CPPFLAGS -D_MYNOPTHREAD"
    fi
    ## -lm -lc
    AC_CHECK_LIB(m, main,
      [BRAIN_LIBADD="$BRAIN_LIBADD -lm"],
      [AC_MSG_FAILURE([library '-lm' not found])]
    )
    AC_CHECK_LIB(c, main,
      [BRAIN_LIBADD="$BRAIN_LIBADD -lc"],
      [AC_MSG_FAILURE([library '-lc' not found])]
    )
    ## check for LZMA
    AC_CHECK_LIB(lzma, main,
      [BRAIN_LIBADD="$BRAIN_LIBADD -llzma"] 
      [BRAIN_CPPFLAGS="$BRAIN_CPPFLAGS -D_MYEXLZMA"],
    )
    ## check for LZO
    AC_CHECK_LIB(lzma, main,
      [BRAIN_LIBADD="$BRAIN_LIBADD -llzma"] 
      [BRAIN_CPPFLAGS="$BRAIN_CPPFLAGS -D_MYEXLZO"],
    )
    ## check for bzip
    AC_CHECK_LIB(z, main,
      [BRAIN_LIBADD="$BRAIN_LIBADD -lz"], 
      [BRAIN_CPPFLAGS="$BRAIN_CPPFLAGS -D_MYNOBZIP"]
    )
    ##AC_CHECK_HEADER([zlib.h]
    AC_CHECK_LIB(bz2, main,
      [BRAIN_LIBADD="$BRAIN_LIBADD -lbz2"], 
      [BRAIN_CPPFLAGS="$BRAIN_CPPFLAGS -D_MYNOZLIB"]
    )
    AC_SUBST([BRAIN_CPPFLAGS])
    AC_SUBST([BRAIN_CFLAGS])
    AC_SUBST([BRAIN_LIBADD])
  fi
  AC_SUBST([USE_BRAIN], $enable_brain)
  AM_CONDITIONAL([USE_BRAIN], [test x$enable_brain = xyes])
])

#------------------------------------------------------------------------
# SC_ENABLE_GUARD --
#
#       Specify if GUARD support is needed
#
# Arguments:
#       none
#
# Results:
#
#------------------------------------------------------------------------

AC_DEFUN([SC_ENABLE_GUARD], [
  AC_MSG_CHECKING([for build with GUARD])
  AC_ARG_ENABLE(guard,
      AC_HELP_STRING([--enable-guard], [build theLink with GUARD support]),
      enable_guard=yes, enable_guard=no
  )
  AC_MSG_RESULT($enable_guard)
  AC_SUBST([USE_GUARD], $enable_guard)
  AM_CONDITIONAL([USE_GUARD], [test x$enable_guard = xyes])
])

#------------------------------------------------------------------------
# include additional macros --
#------------------------------------------------------------------------

