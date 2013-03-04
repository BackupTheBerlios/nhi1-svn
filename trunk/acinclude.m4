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

AC_DEFUN([OT_CHECK_THREAD],[
  pushdef([ID],$1)
  AS_IF([test "$enable_threads" = no], [
    AC_MSG_ERROR(ID require thread support)
  ])
  popdef([ID])
])

AC_DEFUN([OT_REQUIRE_PROG],[
    pushdef([VARIABLE],$1)
    pushdef([EXECUTABLE],$2)
    pushdef([PATH_PROG],$3)

    AC_PATH_PROG([]VARIABLE[],[]EXECUTABLE[],[no],[]PATH_PROG[])
    AS_IF([test "$VARIABLE" = "no"], [
      AC_MSG_ERROR([the tool ']EXECUTABLE[' is required - exit])
    ])

    popdef([PATH_PROG])
    popdef([EXECUTABLE])
    popdef([VARIABLE])
])

AC_DEFUN([OT_WITH_PROG],[
  pushdef([ID],$1)
  pushdef([VARIABLE],$2)
  pushdef([EXECUTABLE],$3)
  pushdef([TYPE],$4)
  pushdef([PATH_PROG],$5)

  AC_ARG_VAR(VARIABLE,Absolute path to 'ID' TYPE)

  AC_MSG_CHECKING(for build with: ID);
  AC_ARG_WITH(ID,AS_HELP_STRING([--with-ID=[[[PATH]]]],absolute path to 'ID' TYPE), [
    AS_IF([test "$withval" != yes -a "$withval" != no],[
      VARIABLE="$withval"
      withval='yes'
      AC_MSG_RESULT($VARIABLE)
    ],[
      AC_MSG_RESULT($withval)
      AS_IF([test "$withval" = yes], [
	AC_PATH_PROGS([]VARIABLE[],[]EXECUTABLE[],[no],[]PATH_PROG[])
	AS_IF([test "$VARIABLE" = no], [
	  AC_MSG_ERROR(unable to find the tool 'ID' - exit)
	])
      ])
    ])
  ],[
    AC_MSG_RESULT([no])
  ])

  pushdef([FLAG],[USE_]translit($1,[a-z],[A-Z]))
  AC_SUBST(FLAG, "$withval")
  AM_CONDITIONAL(FLAG, [test -n "$VARIABLE"])
  popdef([FLAG])

  popdef([PATH_PROG])
  popdef([TYPE])
  popdef([EXECUTABLE])
  popdef([VARIABLE])
  popdef([ID])
])

AC_DEFUN([OT_ENABLE],[
  pushdef([ID],$1)
  pushdef([TXT],[$2])
  pushdef([VAR],[enable_]$1)
  pushdef([FLAG],[USE_]translit($1,[a-z],[A-Z]))

  AC_MSG_CHECKING(for build with: ID);
  AC_ARG_ENABLE(ID,AS_HELP_STRING([--enable-ID], [TXT]), [
    AC_MSG_RESULT($enableval)
    AS_IF([test "$enableval" != 'yes' -a "$enableval" != 'no'],[
      AC_MSG_ERROR(only accept 'yes' or 'no' as argument for 'ID' - exit)
    ])
  ],[
    VAR='no'
    AC_MSG_RESULT($VAR)
  ])

  AC_SUBST(FLAG, "$VAR")
  AM_CONDITIONAL(FLAG, [test "$VAR" = 'yes'])

  popdef([FLAG])
  popdef([VAR])
  popdef([TXT])
  popdef([ID])
])

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
# SC_TOOL_ROOT --
#
#       add support for VPATH build
#
# Arguments:
#       none
#
# Results:
#       Add a new variable NHI1_TOOL_ROOT
#
#------------------------------------------------------------------------

AC_DEFUN([SC_TOOL_ROOT], [
    AC_ARG_VAR([NHI1_TOOL_ROOT], [path to the toplevel tool directory (Nhi1BuildLanguage)])
    AC_SUBST([NHI1_TOOL_ROOT])
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
	AS_HELP_STRING([--enable-symbols], [build with debugging support]),
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
AC_LANG_PUSH([C])
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
	AC_LINK_IFELSE(
	  [AC_LANG_PROGRAM([[#include <pthread.h>]],
	    [[pthread_t th; pthread_join(th, 0);
	      pthread_attr_init(0); pthread_cleanup_push(0, 0);
	      pthread_create(0,0,0,0); pthread_cleanup_pop(0); ]]
	  )],
	  [acx_pthread_ok=yes]
	)

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
	  AC_LINK_IFELSE(
	    [AC_LANG_PROGRAM([[#include <pthread.h>]],
	       [[int attr=$attr; return attr;]]
	    )],
	    [attr_name=$attr; break]
	  )
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
AC_LANG_POP
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
      AS_HELP_STRING([--enable-threads], [build with thread support]),
      enable_threads=yes, enable_threads=no
  )
  AC_MSG_RESULT($enable_threads)
  if test "$enable_threads" = "yes"; then
    # we use native windows threads
    if test "$host_os" != "mingw32" ; then
      ACX_PTHREAD
      if test "$acx_pthread_ok" = "yes" ; then
	LIBS="$PTHREAD_LIBS $LIBS"
	CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
	CC="$PTHREAD_CC"
	#AX_TLS
      fi
    fi
    if test "$host_os" = "mingw32" -o "$acx_pthread_ok" = "yes" ; then
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
  OT_WITH_PROG(java, JAVA, java, runtime)
  AS_IF([test -n "$JAVA"], [
    OT_CHECK_THREAD([java])
    AC_ARG_VAR([JAVA_HOME],[Absolute path to to the JAVA home directory])
    AC_SUBST([JAVA_HOME])
    dirs=()
    for x in $(find "$JAVA_HOME/include" -type d); do
      dirs+=("-I$x")
    done
    AC_SUBST([JAVA_CPPFLAGS], [${dirs[[@]]}])
    OT_REQUIRE_PROG([JAVAC], [javac], [$JAVA_HOME/bin]) 
    OT_REQUIRE_PROG([JAVAH], [javah], [$JAVA_HOME/bin]) 
    OT_REQUIRE_PROG([JAR],   [jar],   [$JAVA_HOME/bin]) 
  ])
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
  OT_WITH_PROG(python, PYTHON, python3, interpreter)
  AS_IF([test -n "$PYTHON"], [
    AS_IF([$PKG_CONFIG --exists python3], [
      AC_SUBST([PYTHON_CFLAGS], [$($PKG_CONFIG --cflags python3)])
      AC_SUBST([PYTHON_LDFLAGS],[$($PKG_CONFIG --libs python3)])
    ],[
      AC_MSG_ERROR([unable to find the package 'python3'])
    ])
  ])
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
  OT_WITH_PROG(csharp, CSCOMP, [csc gmcs], compiler)
  AS_IF([test -n "$CSCOMP"], [
    OT_CHECK_THREAD([CSharp])
    CSHARP_DEBUG=''
    CSHARP_OPT=''
    AS_IF([test "$host_os" = "mingw32"], [
      CLREXEC=''
      AS_IF([test "$enable_symbols" = no], [
	CSHARP_DEBUG='-optimize'
      ])
    ],[
      AS_IF([$PKG_CONFIG --atleast-version 2.10.6 mono], [
	OT_REQUIRE_PROG([CLREXEC], [mono])
      ],[
	AC_MSG_ERROR([unable to find package 'momo' with minimal version 2.10.6])
      ])
      CSHARP_OPT='-v'
    ])
    AC_SUBST([CSHARP_OPT])
    AC_SUBST([CSHARP_DEBUG])
  ])
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
  OT_WITH_PROG(vb, VBCOMP, [vbnc vbc], compiler)
  AS_IF([test -n "$VBCOMP"], [
    AS_IF([test -z "$CSCOMP"], [
      AC_MSG_ERROR([VisualBasic requires a C[#] build too])
    ])
  ])
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
  OT_WITH_PROG(cxx, CXX, ,compiler)
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
  OT_WITH_PROG(php, PHP, php, interpreter)
  AS_IF([test -n "$PHP"], [
    PHP="$PHP -c $ac_pwd/theLink/msgqueforphp/php.ini"
    OT_REQUIRE_PROG([PHPIZE],    [phpize])
    OT_REQUIRE_PROG([PHPCONFIG], [php-config])
  ])
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

AC_DEFUN([SC_ENABLE_GO], [
  OT_WITH_PROG(go, GO, go, compiler)
  AS_IF([test -n "$GO"], [
    OT_CHECK_THREAD([go])
  ])
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
  OT_WITH_PROG(perl, PERL, perl, interpreter)
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
  OT_WITH_PROG(tcl, TCLSH, [tclsh8.6 tclsh86 tclsh86g tclsh8.5 tclsh85 tclsh85g], interpreter)
  AS_IF([test -n "$TCLSH"], [
    #-----------------------------------
    AC_MSG_CHECKING(for location of tclConfig.sh script)
    #-----------------------------------
    TCLBINDIR=$(AS_DIRNAME([$TCLSH]))
    tclcnf="$TCLBINDIR/../lib $TCLBINDIR/../lib64 /lib /lib64 /lib32 /usr/lib /usr/local/lib /usr/local/tcl/lib" 

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
  ])
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
  OT_WITH_PROG(ruby, RUBY, ruby, interpreter)
  AS_IF([test -n "$RUBY"], [
    AS_IF([$PKG_CONFIG --exists ruby-1.9], [
      AC_SUBST([RUBY_CFLAGS], [$($PKG_CONFIG --cflags ruby-1.9)])
      AC_SUBST([RUBY_LDFLAGS],[$($PKG_CONFIG --libs ruby-1.9)])
    ],[
      AC_MSG_ERROR([unable to find the package 'ruby-1.9'])
    ])
  ])
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
  OT_ENABLE([brain], [build theBrain, NHI1 database support])
#  if test "$enable_brain" = "yes"; then
#    if $PKG_CONFIG --exists kyotocabinet ; then
#      if $PKG_CONFIG --atleast-version 1.2.76  kyotocabinet ; then
#	AC_SUBST(BRAIN_CFLAGS,[$($PKG_CONFIG --cflags kyotocabinet)])
#	AC_SUBST(BRAIN_LDADD,[$($PKG_CONFIG --libs kyotocabinet)])
#	AC_SUBST(BRAIN_LDADD_STATIC,[$($PKG_CONFIG --static --libs kyotocabinet)])
#      else
#	AC_MSG_ERROR([unable to get minimal version 1.2.76])
#      fi
#    else
#      AC_MSG_ERROR([unable to find the package 'kyotocabinet'])
#    fi
#  fi
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
  OT_ENABLE([guard], [build theGuard, NHI1 encryption support])
])

#------------------------------------------------------------------------
# include additional macros --
#------------------------------------------------------------------------

