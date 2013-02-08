dnl
dnl  \file       m4/ac_java.m4
dnl  \brief      \$Id$
dnl  
dnl  (C) 2009 - NHI - #1 - Project - Group
dnl  
dnl  \version    \$Rev$
dnl  \author     EMail: aotto1968 at users.berlios.de
dnl  \attention  this software has GPL permissions to copy
dnl              please contact AUTHORS for additional information
dnl
# ===========================================================================
#             http://autoconf-archive.cryp.to/ac_check_class.html
# ===========================================================================
#
# SYNOPSIS
#
#   AC_CHECK_CLASS
#
# DESCRIPTION
#
#   AC_CHECK_CLASS tests the existence of a given Java class, either in a
#   jar or in a '.class' file.
#
#   *Warning*: its success or failure can depend on a proper setting of the
#   CLASSPATH env. variable.
#
#   Note: This is part of the set of autoconf M4 macros for Java programs.
#   It is VERY IMPORTANT that you download the whole set, some macros depend
#   on other. Unfortunately, the autoconf archive does not support the
#   concept of set of macros, so I had to break it for submission. The
#   general documentation, as well as the sample configure.in, is included
#   in the AC_PROG_JAVA macro.
#
# LAST MODIFICATION
#
#   2008-04-12
#
# COPYLEFT
#
#   Copyright (c) 2008 Stephane Bortzmeyer <bortzmeyer@pasteur.fr>
#
#   This program is free software; you can redistribute it and/or modify it
#   under the terms of the GNU General Public License as published by the
#   Free Software Foundation; either version 2 of the License, or (at your
#   option) any later version.
#
#   This program is distributed in the hope that it will be useful, but
#   WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
#   Public License for more details.
#
#   You should have received a copy of the GNU General Public License along
#   with this program. If not, see <http://www.gnu.org/licenses/>.
#
#   As a special exception, the respective Autoconf Macro's copyright owner
#   gives unlimited permission to copy, distribute and modify the configure
#   scripts that are the output of Autoconf when processing the Macro. You
#   need not follow the terms of the GNU General Public License when using
#   or distributing such scripts, even though portions of the text of the
#   Macro appear in them. The GNU General Public License (GPL) does govern
#   all other use of the material that constitutes the Autoconf Macro.
#
#   This special exception to the GPL applies to versions of the Autoconf
#   Macro released by the Autoconf Macro Archive. When you make and
#   distribute a modified version of the Autoconf Macro, you may extend this
#   special exception to the GPL to apply to your modified version as well.

AC_DEFUN([AC_CHECK_CLASS],[
AC_REQUIRE([AC_PROG_JAVA])
ac_var_name=`echo $1 | sed 's/\./_/g'`
dnl Normaly I'd use a AC_CACHE_CHECK here but since the variable name is
dnl dynamic I need an extra level of extraction
AC_MSG_CHECKING([for $1 class])
AC_CACHE_VAL(ac_cv_class_$ac_var_name, [
if test x$ac_cv_prog_uudecode_base64 = xyes; then
dnl /**
dnl  * Test.java: used to test dynamicaly if a class exists.
dnl  */
dnl public class Test
dnl {
dnl
dnl public static void
dnl main( String[] argv )
dnl {
dnl     Class lib;
dnl     if (argv.length < 1)
dnl      {
dnl             System.err.println ("Missing argument");
dnl             System.exit (77);
dnl      }
dnl     try
dnl      {
dnl             lib = Class.forName (argv[0]);
dnl      }
dnl     catch (ClassNotFoundException e)
dnl      {
dnl             System.exit (1);
dnl      }
dnl     lib = null;
dnl     System.exit (0);
dnl }
dnl
dnl }
cat << \EOF > Test.uue
begin-base64 644 Test.class
yv66vgADAC0AKQcAAgEABFRlc3QHAAQBABBqYXZhL2xhbmcvT2JqZWN0AQAE
bWFpbgEAFihbTGphdmEvbGFuZy9TdHJpbmc7KVYBAARDb2RlAQAPTGluZU51
bWJlclRhYmxlDAAKAAsBAANlcnIBABVMamF2YS9pby9QcmludFN0cmVhbTsJ
AA0ACQcADgEAEGphdmEvbGFuZy9TeXN0ZW0IABABABBNaXNzaW5nIGFyZ3Vt
ZW50DAASABMBAAdwcmludGxuAQAVKExqYXZhL2xhbmcvU3RyaW5nOylWCgAV
ABEHABYBABNqYXZhL2lvL1ByaW50U3RyZWFtDAAYABkBAARleGl0AQAEKEkp
VgoADQAXDAAcAB0BAAdmb3JOYW1lAQAlKExqYXZhL2xhbmcvU3RyaW5nOylM
amF2YS9sYW5nL0NsYXNzOwoAHwAbBwAgAQAPamF2YS9sYW5nL0NsYXNzBwAi
AQAgamF2YS9sYW5nL0NsYXNzTm90Rm91bmRFeGNlcHRpb24BAAY8aW5pdD4B
AAMoKVYMACMAJAoAAwAlAQAKU291cmNlRmlsZQEACVRlc3QuamF2YQAhAAEA
AwAAAAAAAgAJAAUABgABAAcAAABtAAMAAwAAACkqvgSiABCyAAwSD7YAFBBN
uAAaKgMyuAAeTKcACE0EuAAaAUwDuAAasQABABMAGgAdACEAAQAIAAAAKgAK
AAAACgAAAAsABgANAA4ADgATABAAEwASAB4AFgAiABgAJAAZACgAGgABACMA
JAABAAcAAAAhAAEAAQAAAAUqtwAmsQAAAAEACAAAAAoAAgAAAAQABAAEAAEA
JwAAAAIAKA==
====
EOF
                if uudecode$EXEEXT Test.uue; then
                        :
                else
                        echo "configure: __oline__: uudecode had trouble decoding base 64 file 'Test.uue'" >&AS_MESSAGE_LOG_FD
                        echo "configure: failed file was:" >&AS_MESSAGE_LOG_FD
                        cat Test.uue >&AS_MESSAGE_LOG_FD
                        ac_cv_prog_uudecode_base64=no
                fi
        rm -f Test.uue
        if AC_TRY_COMMAND($JAVA $JAVAFLAGS Test $1) >/dev/null 2>&1; then
                eval "ac_cv_class_$ac_var_name=yes"
        else
                eval "ac_cv_class_$ac_var_name=no"
        fi
        rm -f Test.class
else
        AC_TRY_COMPILE_JAVA([$1], , [eval "ac_cv_class_$ac_var_name=yes"],
                [eval "ac_cv_class_$ac_var_name=no"])
fi
eval "ac_var_val=$`eval echo ac_cv_class_$ac_var_name`"
eval "HAVE_$ac_var_name=$`echo ac_cv_class_$ac_var_val`"
HAVE_LAST_CLASS=$ac_var_val
if test x$ac_var_val = xyes; then
        ifelse([$2], , :, [$2])
else
        ifelse([$3], , :, [$3])
fi
])
dnl for some reason the above statment didn't fall though here?
dnl do scripts have variable scoping?
eval "ac_var_val=$`eval echo ac_cv_class_$ac_var_name`"
AC_MSG_RESULT($ac_var_val)
])
# ===========================================================================
#           http://autoconf-archive.cryp.to/ac_check_java_home.html
# ===========================================================================
#
# SYNOPSIS
#
#   AC_CHECK_JAVA_HOME
#
# DESCRIPTION
#
#   Check for Sun Java (JDK / JRE) installation, where the 'java' VM is in.
#   If found, set environment variable JAVA_HOME = Java installation home,
#   else left JAVA_HOME untouch, which in most case means JAVA_HOME is
#   empty.
#
# LAST MODIFICATION
#
#   2008-04-12
#
# COPYLEFT
#
#   Copyright (c) 2008 Gleen Salmon <gleensalmon@yahoo.com>
#
#   This program is free software; you can redistribute it and/or modify it
#   under the terms of the GNU General Public License as published by the
#   Free Software Foundation; either version 2 of the License, or (at your
#   option) any later version.
#
#   This program is distributed in the hope that it will be useful, but
#   WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
#   Public License for more details.
#
#   You should have received a copy of the GNU General Public License along
#   with this program. If not, see <http://www.gnu.org/licenses/>.
#
#   As a special exception, the respective Autoconf Macro's copyright owner
#   gives unlimited permission to copy, distribute and modify the configure
#   scripts that are the output of Autoconf when processing the Macro. You
#   need not follow the terms of the GNU General Public License when using
#   or distributing such scripts, even though portions of the text of the
#   Macro appear in them. The GNU General Public License (GPL) does govern
#   all other use of the material that constitutes the Autoconf Macro.
#
#   This special exception to the GPL applies to versions of the Autoconf
#   Macro released by the Autoconf Macro Archive. When you make and
#   distribute a modified version of the Autoconf Macro, you may extend this
#   special exception to the GPL to apply to your modified version as well.

AC_DEFUN([AC_CHECK_JAVA_HOME],[
AC_REQUIRE([AC_EXEEXT])dnl
TRY_JAVA_HOME=`ls -dr /usr/java/* 2> /dev/null | head -n 1`
if test x$TRY_JAVA_HOME != x; then
	PATH=$PATH:$TRY_JAVA_HOME/bin
fi
AC_PATH_PROG(JAVA_PATH_NAME, java$EXEEXT)
if test x$JAVA_PATH_NAME != x; then
	JAVA_HOME=`echo $JAVA_PATH_NAME | sed "s/\(.*\)[[/]]bin[[/]]java$EXEEXT$/\1/"`
fi;dnl
])
# ===========================================================================
#             http://autoconf-archive.cryp.to/ac_check_junit.html
# ===========================================================================
#
# SYNOPSIS
#
#   AC_CHECK_JUNIT
#
# DESCRIPTION
#
#   AC_CHECK_JUNIT tests the availability of the Junit testing framework,
#   and set some variables for conditional compilation of the test suite by
#   automake.
#
#   If available, JUNIT is set to a command launching the text based user
#   interface of Junit, @JAVA_JUNIT@ is set to $JAVA_JUNIT and @TESTS_JUNIT@
#   is set to $TESTS_JUNIT, otherwise they are set to empty values.
#
#   You can use these variables in your Makefile.am file like this :
#
#    # Some of the following classes are built only if junit is available
#    JAVA_JUNIT  = Class1Test.java Class2Test.java AllJunitTests.java
#
#    noinst_JAVA = Example1.java Example2.java @JAVA_JUNIT@
#
#    EXTRA_JAVA  = $(JAVA_JUNIT)
#
#    TESTS_JUNIT = AllJunitTests
#
#    TESTS       = StandaloneTest1 StandaloneTest2 @TESTS_JUNIT@
#
#    EXTRA_TESTS = $(TESTS_JUNIT)
#
#    AllJunitTests :
#       echo "#! /bin/sh" > $@
#       echo "exec @JUNIT@ my.package.name.AllJunitTests" >> $@
#       chmod +x $@
#
# LAST MODIFICATION
#
#   2008-04-12
#
# COPYLEFT
#
#   Copyright (c) 2008 Luc Maisonobe <luc@spaceroots.org>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved.

AC_DEFUN([AC_CHECK_JUNIT],[
AC_CACHE_VAL(ac_cv_prog_JUNIT,[
AC_CHECK_CLASS(junit.textui.TestRunner)
if test x"`eval 'echo $ac_cv_class_junit_textui_TestRunner'`" != xno ; then
  ac_cv_prog_JUNIT='$(CLASSPATH_ENV) $(JAVA) $(JAVAFLAGS) junit.textui.TestRunner'
fi])
AC_MSG_CHECKING([for junit])
if test x"`eval 'echo $ac_cv_prog_JUNIT'`" != x ; then
  JUNIT="$ac_cv_prog_JUNIT"
  JAVA_JUNIT='$(JAVA_JUNIT)'
  TESTS_JUNIT='$(TESTS_JUNIT)'
else
  JUNIT=
  JAVA_JUNIT=
  TESTS_JUNIT=
fi
AC_MSG_RESULT($JAVA_JUNIT)
AC_SUBST(JUNIT)
AC_SUBST(JAVA_JUNIT)
AC_SUBST(TESTS_JUNIT)])
# ===========================================================================
#            http://autoconf-archive.cryp.to/ac_java_options.html
# ===========================================================================
#
# SYNOPSIS
#
#   AC_JAVA_OPTIONS
#
# DESCRIPTION
#
#   AC_JAVA_OPTIONS adds configure command line options used for Java m4
#   macros. This Macro is optional.
#
#   Note: This is part of the set of autoconf M4 macros for Java programs.
#   It is VERY IMPORTANT that you download the whole set, some macros depend
#   on other. Unfortunately, the autoconf archive does not support the
#   concept of set of macros, so I had to break it for submission. The
#   general documentation, as well as the sample configure.in, is included
#   in the AC_PROG_JAVA macro.
#
# LAST MODIFICATION
#
#   2008-04-12
#
# COPYLEFT
#
#   Copyright (c) 2008 Devin Weaver <ktohg@tritarget.com>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved.

AC_DEFUN([AC_JAVA_OPTIONS],[
AC_ARG_WITH(java-prefix,
                        [  --with-java-prefix=PFX  prefix where Java runtime is installed (optional)])
AC_ARG_WITH(javac-flags,
                        [  --with-javac-flags=FLAGS flags to pass to the Java compiler (optional)])
AC_ARG_WITH(java-flags,
                        [  --with-java-flags=FLAGS flags to pass to the Java VM (optional)])
JAVAPREFIX=$with_java_prefix
JAVACFLAGS=$with_javac_flags
JAVAFLAGS=$with_java_flags
AC_SUBST(JAVAPREFIX)dnl
AC_SUBST(JAVACFLAGS)dnl
AC_SUBST(JAVAFLAGS)dnl
AC_SUBST(JAVA)dnl
AC_SUBST(JAVAC)dnl
])
# ===========================================================================
#           http://autoconf-archive.cryp.to/ac_jni_include_dir.html
# ===========================================================================
#
# SYNOPSIS
#
#   AC_JNI_INCLUDE_DIR
#
# DESCRIPTION
#
#   AC_JNI_INCLUDE_DIR finds include directories needed for compiling
#   programs using the JNI interface.
#
#   JNI include directories are usually in the java distribution This is
#   deduced from the value of JAVAC. When this macro completes, a list of
#   directories is left in the variable JNI_INCLUDE_DIRS.
#
#   Example usage follows:
#
#       AC_JNI_INCLUDE_DIR
#
#       for JNI_INCLUDE_DIR in $JNI_INCLUDE_DIRS
#       do
#               JAVA_CPPFLAGS="-I$JNI_INCLUDE_DIR"
#       done
#
#   If you want to force a specific compiler:
#
#   - at the configure.in level, set JAVAC=yourcompiler before calling
#   AC_JNI_INCLUDE_DIR
#
#   - at the configure level, setenv JAVAC
#
#   Note: This macro can work with the autoconf M4 macros for Java programs.
#   This particular macro is not part of the original set of macros.
#
# LAST MODIFICATION
#
#   2008-04-12
#
# COPYLEFT
#
#   Copyright (c) 2008 Don Anderson <dda@sleepycat.com>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved.

AC_DEFUN([AC_JNI_INCLUDE_DIR],[

JNI_INCLUDE_DIRS=""

test "x$JAVAC" = x && AC_MSG_ERROR(['\$JAVAC' undefined])
AC_PATH_PROG(_ACJNI_JAVAC, $JAVAC, no)
test "x$_ACJNI_JAVAC" = xno && AC_MSG_ERROR([$JAVAC could not be found in path])

_ACJNI_FOLLOW_SYMLINKS("$_ACJNI_JAVAC")
_JTOPDIR=$(echo "$_ACJNI_FOLLOWED" | sed -e 's://*:/:g' -e 's:/[[^/]]*$::')
case "$host_os" in
        darwin*)        _JTOPDIR=`echo "$_JTOPDIR" | sed -e 's:/[[^/]]*$::'`
                        _JINC="$_JTOPDIR/Headers";;
        freebsd*)       _JTOPDIR=`head -n 1 "/usr/local/etc/javavms" | sed -E 's|[[:space:]]*#.*||' 2>/dev/null`
			_JTOPDIR=`dirname "${_JTOPDIR}"`
			_JTOPDIR=`dirname "${_JTOPDIR}"`
                        _JINC="$_JTOPDIR/include";;
        *)              _JINC="$_JTOPDIR/include";;
esac
#echo "host_os<$host_os>"
#echo "_JTOPDIR<$_JTOPDIR>"
#echo "_JINC<$_JINC>"
if test -f "$_JINC/jni.h"; then
        JNI_INCLUDE_DIRS="$JNI_INCLUDE_DIRS $_JINC"
else
        _JTOPDIR=`echo "$_JTOPDIR" | sed -e 's:/[[^/]]*$::'`
        if test -f "$_JTOPDIR/include/jni.h"; then
                JNI_INCLUDE_DIRS="$JNI_INCLUDE_DIRS $_JTOPDIR/include"
        else
                AC_MSG_ERROR([cannot find java include files])
        fi
fi

# get the likely subdirectories for system specific java includes
case "$host_os" in
bsdi*)          _JNI_INC_SUBDIRS="bsdos";;
freebsd*)          _JNI_INC_SUBDIRS="freebsd";;
linux*)         _JNI_INC_SUBDIRS="linux genunix";;
osf*)           _JNI_INC_SUBDIRS="alpha";;
solaris*)       _JNI_INC_SUBDIRS="solaris";;
mingw*)			_JNI_INC_SUBDIRS="win32";;
cygwin*)		_JNI_INC_SUBDIRS="win32";;
*)              _JNI_INC_SUBDIRS="genunix";;
esac

# add any subdirectories that are present
for JINCSUBDIR in $_JNI_INC_SUBDIRS
do
        if test -d "$_JTOPDIR/include/$JINCSUBDIR"; then
                JNI_INCLUDE_DIRS="$JNI_INCLUDE_DIRS $_JTOPDIR/include/$JINCSUBDIR"
        fi
done
])

# _ACJNI_FOLLOW_SYMLINKS <path>
# Follows symbolic links on <path>,
# finally setting variable _ACJNI_FOLLOWED
# --------------------
AC_DEFUN([_ACJNI_FOLLOW_SYMLINKS],[
# find the include directory relative to the javac executable
_cur="$1"
while ls -ld "$_cur" 2>/dev/null | grep " -> " >/dev/null; do
        AC_MSG_CHECKING(symlink for $_cur)
        _slink=`ls -ld "$_cur" | sed 's/.* -> //'`
        case "$_slink" in
        /*) _cur="$_slink";;
        # 'X' avoids triggering unwanted echo options.
        *) _cur=`echo "X$_cur" | sed -e 's/^X//' -e 's:[[^/]]*$::'`"$_slink";;
        esac
        AC_MSG_RESULT($_cur)
done
_ACJNI_FOLLOWED="$_cur"
])# _ACJNI
# ===========================================================================
#            http://autoconf-archive.cryp.to/ac_prog_java_cc.html
# ===========================================================================
#
# SYNOPSIS
#
#   AC_PROG_JAVA_CC
#
# DESCRIPTION
#
#   Finds the appropriate java compiler on your path. By preference the java
#   compiler is gcj, then jikes then javac.
#
#   The macro can take one argument specifying a space separated list of
#   java compiler names.
#
#   For example:
#
#     AC_PROG_JAVA_CC(javac, gcj)
#
#   The macro also sets the compiler options variable: JAVA_CC_OPTS to
#   something sensible:
#
#    - for GCJ it sets it to: @GCJ_OPTS@
#      (if GCJ_OPTS is not yet defined then it is set to "-C")
#
#    - no other compiler has applicable options yet
#
#   Here's an example configure.in:
#
#     AC_INIT(Makefile.in)
#     AC_PROG_JAVA_CC()
#     AC_OUTPUT(Makefile)
#     dnl End.
#
#   And here's the start of the Makefile.in:
#
#     PROJECT_ROOT      := @srcdir@
#     # Tool definitions.
#     JAVAC             := @JAVA_CC@
#     JAVAC_OPTS        := @JAVA_CC_OPTS@
#     JAR_TOOL          := @jar_tool@
#
# LAST MODIFICATION
#
#   2008-04-12
#
# COPYLEFT
#
#   Copyright (c) 2008 Nic Ferrier <nferrier@tapsellferrier.co.uk>
#
#   This program is free software; you can redistribute it and/or modify it
#   under the terms of the GNU General Public License as published by the
#   Free Software Foundation; either version 2 of the License, or (at your
#   option) any later version.
#
#   This program is distributed in the hope that it will be useful, but
#   WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
#   Public License for more details.
#
#   You should have received a copy of the GNU General Public License along
#   with this program. If not, see <http://www.gnu.org/licenses/>.
#
#   As a special exception, the respective Autoconf Macro's copyright owner
#   gives unlimited permission to copy, distribute and modify the configure
#   scripts that are the output of Autoconf when processing the Macro. You
#   need not follow the terms of the GNU General Public License when using
#   or distributing such scripts, even though portions of the text of the
#   Macro appear in them. The GNU General Public License (GPL) does govern
#   all other use of the material that constitutes the Autoconf Macro.
#
#   This special exception to the GPL applies to versions of the Autoconf
#   Macro released by the Autoconf Macro Archive. When you make and
#   distribute a modified version of the Autoconf Macro, you may extend this
#   special exception to the GPL to apply to your modified version as well.

# AC_PROG_JAVA_CC([COMPILER ...])
# --------------------------
# COMPILER ... is a space separated list of java compilers to search for.
# This just gives the user an opportunity to specify an alternative
# search list for the java compiler.
AC_DEFUN([AC_PROG_JAVA_CC],
[AC_ARG_VAR([JAVA_CC],                [java compiler command])dnl
AC_ARG_VAR([JAVA_CC_FLAGS],           [java compiler flags])dnl
m4_ifval([$1],
      [AC_CHECK_TOOLS(JAVA_CC, [$1])],
[AC_CHECK_TOOL(JAVA_CC, gcj)
if test -z "$JAVA_CC"; then
  AC_CHECK_TOOL(JAVA_CC, javac)
fi
if test -z "$JAVA_CC"; then
  AC_CHECK_TOOL(JAVA_CC, jikes)
fi
])

if test "$JAVA_CC" = "gcj"; then
   if test "$GCJ_OPTS" = ""; then
      AC_SUBST(GCJ_OPTS,-C)
   fi
   AC_SUBST(JAVA_CC_OPTS, @GCJ_OPTS@,
        [Define the compilation options for GCJ])
fi
test -z "$JAVA_CC" && AC_MSG_ERROR([no acceptable java compiler found in \$PATH])
])# AC_PROG_JAVA_CC
# ===========================================================================
#             http://autoconf-archive.cryp.to/ac_prog_javac.html
# ===========================================================================
#
# SYNOPSIS
#
#   AC_PROG_JAVAC
#
# DESCRIPTION
#
#   AC_PROG_JAVAC tests an existing Java compiler. It uses the environment
#   variable JAVAC then tests in sequence various common Java compilers. For
#   political reasons, it starts with the free ones.
#
#   If you want to force a specific compiler:
#
#   - at the configure.in level, set JAVAC=yourcompiler before calling
#   AC_PROG_JAVAC
#
#   - at the configure level, setenv JAVAC
#
#   You can use the JAVAC variable in your Makefile.in, with @JAVAC@.
#
#   *Warning*: its success or failure can depend on a proper setting of the
#   CLASSPATH env. variable.
#
#   TODO: allow to exclude compilers (rationale: most Java programs cannot
#   compile with some compilers like guavac).
#
#   Note: This is part of the set of autoconf M4 macros for Java programs.
#   It is VERY IMPORTANT that you download the whole set, some macros depend
#   on other. Unfortunately, the autoconf archive does not support the
#   concept of set of macros, so I had to break it for submission. The
#   general documentation, as well as the sample configure.in, is included
#   in the AC_PROG_JAVA macro.
#
# LAST MODIFICATION
#
#   2008-04-12
#
# COPYLEFT
#
#   Copyright (c) 2008 Stephane Bortzmeyer <bortzmeyer@pasteur.fr>
#
#   This program is free software; you can redistribute it and/or modify it
#   under the terms of the GNU General Public License as published by the
#   Free Software Foundation; either version 2 of the License, or (at your
#   option) any later version.
#
#   This program is distributed in the hope that it will be useful, but
#   WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
#   Public License for more details.
#
#   You should have received a copy of the GNU General Public License along
#   with this program. If not, see <http://www.gnu.org/licenses/>.
#
#   As a special exception, the respective Autoconf Macro's copyright owner
#   gives unlimited permission to copy, distribute and modify the configure
#   scripts that are the output of Autoconf when processing the Macro. You
#   need not follow the terms of the GNU General Public License when using
#   or distributing such scripts, even though portions of the text of the
#   Macro appear in them. The GNU General Public License (GPL) does govern
#   all other use of the material that constitutes the Autoconf Macro.
#
#   This special exception to the GPL applies to versions of the Autoconf
#   Macro released by the Autoconf Macro Archive. When you make and
#   distribute a modified version of the Autoconf Macro, you may extend this
#   special exception to the GPL to apply to your modified version as well.

AC_DEFUN([AC_PROG_JAVAC],[
AC_REQUIRE([AC_EXEEXT])dnl
if test "x$JAVAPREFIX" = x; then
        test "x$JAVAC" = x && AC_CHECK_PROGS(JAVAC, "gcj$EXEEXT -C" guavac$EXEEXT jikes$EXEEXT javac$EXEEXT)
else
        test "x$JAVAC" = x && AC_CHECK_PROGS(JAVAC, "gcj$EXEEXT -C" guavac$EXEEXT jikes$EXEEXT javac$EXEEXT, $JAVAPREFIX)
fi
test "x$JAVAC" = x && AC_MSG_ERROR([no acceptable Java compiler found in \$PATH])
AC_PROG_JAVAC_WORKS
AC_PROVIDE([$0])dnl
])
# ===========================================================================
#          http://autoconf-archive.cryp.to/ac_prog_javac_works.html
# ===========================================================================
#
# SYNOPSIS
#
#   AC_PROG_JAVAC_WORKS
#
# DESCRIPTION
#
#   Internal use ONLY.
#
#   Note: This is part of the set of autoconf M4 macros for Java programs.
#   It is VERY IMPORTANT that you download the whole set, some macros depend
#   on other. Unfortunately, the autoconf archive does not support the
#   concept of set of macros, so I had to break it for submission. The
#   general documentation, as well as the sample configure.in, is included
#   in the AC_PROG_JAVA macro.
#
# LAST MODIFICATION
#
#   2008-04-12
#
# COPYLEFT
#
#   Copyright (c) 2008 Stephane Bortzmeyer <bortzmeyer@pasteur.fr>
#
#   This program is free software; you can redistribute it and/or modify it
#   under the terms of the GNU General Public License as published by the
#   Free Software Foundation; either version 2 of the License, or (at your
#   option) any later version.
#
#   This program is distributed in the hope that it will be useful, but
#   WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
#   Public License for more details.
#
#   You should have received a copy of the GNU General Public License along
#   with this program. If not, see <http://www.gnu.org/licenses/>.
#
#   As a special exception, the respective Autoconf Macro's copyright owner
#   gives unlimited permission to copy, distribute and modify the configure
#   scripts that are the output of Autoconf when processing the Macro. You
#   need not follow the terms of the GNU General Public License when using
#   or distributing such scripts, even though portions of the text of the
#   Macro appear in them. The GNU General Public License (GPL) does govern
#   all other use of the material that constitutes the Autoconf Macro.
#
#   This special exception to the GPL applies to versions of the Autoconf
#   Macro released by the Autoconf Macro Archive. When you make and
#   distribute a modified version of the Autoconf Macro, you may extend this
#   special exception to the GPL to apply to your modified version as well.

AC_DEFUN([AC_PROG_JAVAC_WORKS],[
AC_CACHE_CHECK([if $JAVAC works], ac_cv_prog_javac_works, [
JAVA_TEST=Test.java
CLASS_TEST=Test.class
cat << \EOF > $JAVA_TEST
/* [#]line __oline__ "configure" */
public class Test {
}
EOF
if AC_TRY_COMMAND($JAVAC $JAVACFLAGS $JAVA_TEST) >/dev/null 2>&1; then
  ac_cv_prog_javac_works=yes
else
  AC_MSG_ERROR([The Java compiler $JAVAC failed (see config.log, check the CLASSPATH?)])
  echo "configure: failed program was:" >&AS_MESSAGE_LOG_FD
  cat $JAVA_TEST >&AS_MESSAGE_LOG_FD
fi
rm -f $JAVA_TEST $CLASS_TEST
])
AC_PROVIDE([$0])dnl
])
# ===========================================================================
#            http://autoconf-archive.cryp.to/ac_prog_javadoc.html
# ===========================================================================
#
# SYNOPSIS
#
#   AC_PROG_JAVADOC
#
# DESCRIPTION
#
#   AC_PROG_JAVADOC tests for an existing javadoc generator. It uses the
#   environment variable JAVADOC then tests in sequence various common
#   javadoc generator.
#
#   If you want to force a specific compiler:
#
#   - at the configure.in level, set JAVADOC=yourgenerator before calling
#   AC_PROG_JAVADOC
#
#   - at the configure level, setenv JAVADOC
#
#   You can use the JAVADOC variable in your Makefile.in, with @JAVADOC@.
#
#   Note: This macro depends on the autoconf M4 macros for Java programs. It
#   is VERY IMPORTANT that you download that whole set, some macros depend
#   on other. Unfortunately, the autoconf archive does not support the
#   concept of set of macros, so I had to break it for submission.
#
#   The general documentation of those macros, as well as the sample
#   configure.in, is included in the AC_PROG_JAVA macro.
#
# LAST MODIFICATION
#
#   2008-04-12
#
# COPYLEFT
#
#   Copyright (c) 2008 Egon Willighagen <e.willighagen@science.ru.nl>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved.

AC_DEFUN([AC_PROG_JAVADOC],[
AC_REQUIRE([AC_EXEEXT])dnl
if test "x$JAVAPREFIX" = x; then
        test "x$JAVADOC" = x && AC_CHECK_PROGS(JAVADOC, javadoc$EXEEXT)
else
        test "x$JAVADOC" = x && AC_CHECK_PROGS(JAVADOC, javadoc, $JAVAPREFIX)
fi
test "x$JAVADOC" = x && AC_MSG_ERROR([no acceptable javadoc generator found in \$PATH])
AC_PROVIDE([$0])dnl
])
# ===========================================================================
#             http://autoconf-archive.cryp.to/ac_prog_javah.html
# ===========================================================================
#
# SYNOPSIS
#
#   AC_PROG_JAVAH
#
# DESCRIPTION
#
#   AC_PROG_JAVAH tests the availability of the javah header generator and
#   looks for the jni.h header file using JAVA_CPPFLAGS. If available, JAVAH 
#   is set to the full path of javah.
#
# LAST MODIFICATION
#
#   2008-04-12
#
# COPYLEFT
#
#   Copyright (c) 2008 Luc Maisonobe <luc@spaceroots.org>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved.

AC_DEFUN([AC_PROG_JAVAH],[
AC_REQUIRE([AC_CANONICAL_SYSTEM])dnl
AC_REQUIRE([AC_PROG_CPP])dnl
AC_PATH_PROG(JAVAH,javah)
if test x"`eval 'echo $ac_cv_path_JAVAH'`" != x ; then
  ac_save_CPPFLAGS="$CPPFLAGS"
  CPPFLAGS="$JAVA_CPPFLAGS"
  AC_PREPROC_IFELSE(
    [AC_LANG_SOURCE([[#include <jni.h>]])],,
    [AC_MSG_ERROR([unable to include <jni.h>])]
  )
  CPPFLAGS="$ac_save_CPPFLAGS"
fi])

# ===========================================================================
#              http://autoconf-archive.cryp.to/ac_prog_java.html
# ===========================================================================
#
# SYNOPSIS
#
#   AC_PROG_JAVA
#
# DESCRIPTION
#
#   Here is a summary of the main macros:
#
#   AC_PROG_JAVAC: finds a Java compiler.
#
#   AC_PROG_JAVA: finds a Java virtual machine.
#
#   AC_CHECK_CLASS: finds if we have the given class (beware of CLASSPATH!).
#
#   AC_CHECK_RQRD_CLASS: finds if we have the given class and stops
#   otherwise.
#
#   AC_TRY_COMPILE_JAVA: attempt to compile user given source.
#
#   AC_TRY_RUN_JAVA: attempt to compile and run user given source.
#
#   AC_JAVA_OPTIONS: adds Java configure options.
#
#   AC_PROG_JAVA tests an existing Java virtual machine. It uses the
#   environment variable JAVA then tests in sequence various common Java
#   virtual machines. For political reasons, it starts with the free ones.
#   You *must* call [AC_PROG_JAVAC] before.
#
#   If you want to force a specific VM:
#
#   - at the configure.in level, set JAVA=yourvm before calling AC_PROG_JAVA
#
#     (but after AC_INIT)
#
#   - at the configure level, setenv JAVA
#
#   You can use the JAVA variable in your Makefile.in, with @JAVA@.
#
#   *Warning*: its success or failure can depend on a proper setting of the
#   CLASSPATH env. variable.
#
#   TODO: allow to exclude virtual machines (rationale: most Java programs
#   cannot run with some VM like kaffe).
#
#   Note: This is part of the set of autoconf M4 macros for Java programs.
#   It is VERY IMPORTANT that you download the whole set, some macros depend
#   on other. Unfortunately, the autoconf archive does not support the
#   concept of set of macros, so I had to break it for submission.
#
#   A Web page, with a link to the latest CVS snapshot is at
#   <http://www.internatif.org/bortzmeyer/autoconf-Java/>.
#
#   This is a sample configure.in Process this file with autoconf to produce
#   a configure script.
#
#      AC_INIT(UnTag.java)
#
#      dnl Checks for programs.
#      AC_CHECK_CLASSPATH
#      AC_PROG_JAVAC
#      AC_PROG_JAVA
#
#      dnl Checks for classes
#      AC_CHECK_RQRD_CLASS(org.xml.sax.Parser)
#      AC_CHECK_RQRD_CLASS(com.jclark.xml.sax.Driver)
#
#      AC_OUTPUT(Makefile)
#
# LAST MODIFICATION
#
#   2008-04-12
#
# COPYLEFT
#
#   Copyright (c) 2008 Stephane Bortzmeyer <bortzmeyer@pasteur.fr>
#
#   This program is free software; you can redistribute it and/or modify it
#   under the terms of the GNU General Public License as published by the
#   Free Software Foundation; either version 2 of the License, or (at your
#   option) any later version.
#
#   This program is distributed in the hope that it will be useful, but
#   WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
#   Public License for more details.
#
#   You should have received a copy of the GNU General Public License along
#   with this program. If not, see <http://www.gnu.org/licenses/>.
#
#   As a special exception, the respective Autoconf Macro's copyright owner
#   gives unlimited permission to copy, distribute and modify the configure
#   scripts that are the output of Autoconf when processing the Macro. You
#   need not follow the terms of the GNU General Public License when using
#   or distributing such scripts, even though portions of the text of the
#   Macro appear in them. The GNU General Public License (GPL) does govern
#   all other use of the material that constitutes the Autoconf Macro.
#
#   This special exception to the GPL applies to versions of the Autoconf
#   Macro released by the Autoconf Macro Archive. When you make and
#   distribute a modified version of the Autoconf Macro, you may extend this
#   special exception to the GPL to apply to your modified version as well.

AC_DEFUN([AC_PROG_JAVA],[
AC_REQUIRE([AC_EXEEXT])dnl
if test x$JAVAPREFIX = x; then
  test x$JAVA = x && AC_CHECK_PROGS(JAVA, kaffe$EXEEXT java$EXEEXT)
else
  test x$JAVA = x && AC_CHECK_PROGS(JAVA, kaffe$EXEEXT java$EXEEXT, $JAVAPREFIX)
fi
test x$JAVA = x && AC_MSG_ERROR([no acceptable Java virtual machine found in \$PATH])
AC_PROG_JAVA_WORKS
AC_PROVIDE([$0])dnl
])
# ===========================================================================
#           http://autoconf-archive.cryp.to/ac_prog_java_works.html
# ===========================================================================
#
# SYNOPSIS
#
#   AC_PROG_JAVA_WORKS
#
# DESCRIPTION
#
#   Internal use ONLY.
#
#   Note: This is part of the set of autoconf M4 macros for Java programs.
#   It is VERY IMPORTANT that you download the whole set, some macros depend
#   on other. Unfortunately, the autoconf archive does not support the
#   concept of set of macros, so I had to break it for submission. The
#   general documentation, as well as the sample configure.in, is included
#   in the AC_PROG_JAVA macro.
#
# LAST MODIFICATION
#
#   2008-04-12
#
# COPYLEFT
#
#   Copyright (c) 2008 Stephane Bortzmeyer <bortzmeyer@pasteur.fr>
#
#   This program is free software; you can redistribute it and/or modify it
#   under the terms of the GNU General Public License as published by the
#   Free Software Foundation; either version 2 of the License, or (at your
#   option) any later version.
#
#   This program is distributed in the hope that it will be useful, but
#   WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
#   Public License for more details.
#
#   You should have received a copy of the GNU General Public License along
#   with this program. If not, see <http://www.gnu.org/licenses/>.
#
#   As a special exception, the respective Autoconf Macro's copyright owner
#   gives unlimited permission to copy, distribute and modify the configure
#   scripts that are the output of Autoconf when processing the Macro. You
#   need not follow the terms of the GNU General Public License when using
#   or distributing such scripts, even though portions of the text of the
#   Macro appear in them. The GNU General Public License (GPL) does govern
#   all other use of the material that constitutes the Autoconf Macro.
#
#   This special exception to the GPL applies to versions of the Autoconf
#   Macro released by the Autoconf Macro Archive. When you make and
#   distribute a modified version of the Autoconf Macro, you may extend this
#   special exception to the GPL to apply to your modified version as well.

AC_DEFUN([AC_PROG_JAVA_WORKS], [
AC_CHECK_PROG(uudecode, uudecode$EXEEXT, yes)
if test x$uudecode = xyes; then
AC_CACHE_CHECK([if uudecode can decode base 64 file], ac_cv_prog_uudecode_base64, [
dnl /**
dnl  * Test.java: used to test if java compiler works.
dnl  */
dnl public class Test
dnl {
dnl
dnl public static void
dnl main( String[] argv )
dnl {
dnl     System.exit (0);
dnl }
dnl
dnl }
cat << \EOF > Test.uue
begin-base64 644 Test.class
yv66vgADAC0AFQcAAgEABFRlc3QHAAQBABBqYXZhL2xhbmcvT2JqZWN0AQAE
bWFpbgEAFihbTGphdmEvbGFuZy9TdHJpbmc7KVYBAARDb2RlAQAPTGluZU51
bWJlclRhYmxlDAAKAAsBAARleGl0AQAEKEkpVgoADQAJBwAOAQAQamF2YS9s
YW5nL1N5c3RlbQEABjxpbml0PgEAAygpVgwADwAQCgADABEBAApTb3VyY2VG
aWxlAQAJVGVzdC5qYXZhACEAAQADAAAAAAACAAkABQAGAAEABwAAACEAAQAB
AAAABQO4AAyxAAAAAQAIAAAACgACAAAACgAEAAsAAQAPABAAAQAHAAAAIQAB
AAEAAAAFKrcAErEAAAABAAgAAAAKAAIAAAAEAAQABAABABMAAAACABQ=
====
EOF
if uudecode$EXEEXT Test.uue; then
        ac_cv_prog_uudecode_base64=yes
else
        echo "configure: __oline__: uudecode had trouble decoding base 64 file 'Test.uue'" >&AS_MESSAGE_LOG_FD
        echo "configure: failed file was:" >&AS_MESSAGE_LOG_FD
        cat Test.uue >&AS_MESSAGE_LOG_FD
        ac_cv_prog_uudecode_base64=no
fi
rm -f Test.uue])
fi
if test x$ac_cv_prog_uudecode_base64 != xyes; then
        rm -f Test.class
        AC_MSG_WARN([I have to compile Test.class from scratch])
        if test x$ac_cv_prog_javac_works = xno; then
                AC_MSG_ERROR([Cannot compile java source. $JAVAC does not work properly])
        fi
        if test x$ac_cv_prog_javac_works = x; then
                AC_PROG_JAVAC
        fi
fi
AC_CACHE_CHECK(if $JAVA works, ac_cv_prog_java_works, [
JAVA_TEST=Test.java
CLASS_TEST=Test.class
TEST=Test
changequote(, )dnl
cat << \EOF > $JAVA_TEST
/* [#]line __oline__ "configure" */
public class Test {
public static void main (String args[]) {
        System.exit (0);
} }
EOF
changequote([, ])dnl
if test x$ac_cv_prog_uudecode_base64 != xyes; then
        if AC_TRY_COMMAND($JAVAC $JAVACFLAGS $JAVA_TEST) && test -s $CLASS_TEST; then
                :
        else
          echo "configure: failed program was:" >&AS_MESSAGE_LOG_FD
          cat $JAVA_TEST >&AS_MESSAGE_LOG_FD
          AC_MSG_ERROR(The Java compiler $JAVAC failed (see config.log, check the CLASSPATH?))
        fi
fi
if AC_TRY_COMMAND($JAVA $JAVAFLAGS $TEST) >/dev/null 2>&1; then
  ac_cv_prog_java_works=yes
else
  echo "configure: failed program was:" >&AS_MESSAGE_LOG_FD
  cat $JAVA_TEST >&AS_MESSAGE_LOG_FD
  AC_MSG_ERROR(The Java VM $JAVA failed (see config.log, check the CLASSPATH?))
fi
rm -fr $JAVA_TEST $CLASS_TEST Test.uue
])
AC_PROVIDE([$0])dnl
]
)
# ===========================================================================
#          http://autoconf-archive.cryp.to/ac_try_compile_java.html
# ===========================================================================
#
# SYNOPSIS
#
#   AC_TRY_COMPILE_JAVA
#
# DESCRIPTION
#
#   AC_TRY_COMPILE_JAVA attempt to compile user given source.
#
#   *Warning*: its success or failure can depend on a proper setting of the
#   CLASSPATH env. variable.
#
#   Note: This is part of the set of autoconf M4 macros for Java programs.
#   It is VERY IMPORTANT that you download the whole set, some macros depend
#   on other. Unfortunately, the autoconf archive does not support the
#   concept of set of macros, so I had to break it for submission. The
#   general documentation, as well as the sample configure.in, is included
#   in the AC_PROG_JAVA macro.
#
# LAST MODIFICATION
#
#   2008-04-12
#
# COPYLEFT
#
#   Copyright (c) 2008 Devin Weaver <ktohg@tritarget.com>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved.

AC_DEFUN([AC_TRY_COMPILE_JAVA],[
AC_REQUIRE([AC_PROG_JAVAC])dnl
cat << \EOF > Test.java
/* [#]line __oline__ "configure" */
ifelse([$1], , , [import $1;])
public class Test {
[$2]
}
EOF
if AC_TRY_COMMAND($JAVAC $JAVACFLAGS Test.java) && test -s Test.class
then
dnl Don't remove the temporary files here, so they can be examined.
  ifelse([$3], , :, [$3])
else
  echo "configure: failed program was:" >&AS_MESSAGE_LOG_FD
  cat Test.java >&AS_MESSAGE_LOG_FD
ifelse([$4], , , [  rm -fr Test*
  $4
])dnl
fi
rm -fr Test*])
# ===========================================================================
#            http://autoconf-archive.cryp.to/ac_try_run_javac.html
# ===========================================================================
#
# SYNOPSIS
#
#   AC_TRY_RUN_JAVA
#
# DESCRIPTION
#
#   AC_TRY_RUN_JAVA attempt to compile and run user given source.
#
#   *Warning*: its success or failure can depend on a proper setting of the
#   CLASSPATH env. variable.
#
#   Note: This is part of the set of autoconf M4 macros for Java programs.
#   It is VERY IMPORTANT that you download the whole set, some macros depend
#   on other. Unfortunately, the autoconf archive does not support the
#   concept of set of macros, so I had to break it for submission. The
#   general documentation, as well as the sample configure.in, is included
#   in the AC_PROG_JAVA macro.
#
# LAST MODIFICATION
#
#   2008-04-12
#
# COPYLEFT
#
#   Copyright (c) 2008 Devin Weaver <ktohg@tritarget.com>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved.

AC_DEFUN([AC_TRY_RUN_JAVA],[
AC_REQUIRE([AC_PROG_JAVAC])dnl
AC_REQUIRE([AC_PROG_JAVA])dnl
cat << \EOF > Test.java
/* [#]line __oline__ "configure" */
ifelse([$1], , , [include $1;])
public class Test {
[$2]
}
EOF
if AC_TRY_COMMAND($JAVAC $JAVACFLAGS Test.java) && test -s Test.class && ($JAVA $JAVAFLAGS Test; exit) 2>/dev/null
then
dnl Don't remove the temporary files here, so they can be examined.
  ifelse([$3], , :, [$3])
else
  echo "configure: failed program was:" >&AS_MESSAGE_LOG_FD
  cat Test.java >&AS_MESSAGE_LOG_FD
ifelse([$4], , , [  rm -fr Test*
  $4
])dnl
fi
rm -fr Test*])
# ===========================================================================
#           http://autoconf-archive.cryp.to/ac_check_classpath.html
# ===========================================================================
#
# SYNOPSIS
#
#   AC_CHECK_CLASSPATH
#
# DESCRIPTION
#
#   AC_CHECK_CLASSPATH just displays the CLASSPATH, for the edification of
#   the user.
#
#   Note: This is part of the set of autoconf M4 macros for Java programs.
#   It is VERY IMPORTANT that you download the whole set, some macros depend
#   on other. Unfortunately, the autoconf archive does not support the
#   concept of set of macros, so I had to break it for submission. The
#   general documentation, as well as the sample configure.in, is included
#   in the AC_PROG_JAVA macro.
#
# LAST MODIFICATION
#
#   2008-04-12
#
# COPYLEFT
#
#   Copyright (c) 2008 Stephane Bortzmeyer <bortzmeyer@pasteur.fr>
#
#   This program is free software; you can redistribute it and/or modify it
#   under the terms of the GNU General Public License as published by the
#   Free Software Foundation; either version 2 of the License, or (at your
#   option) any later version.
#
#   This program is distributed in the hope that it will be useful, but
#   WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
#   Public License for more details.
#
#   You should have received a copy of the GNU General Public License along
#   with this program. If not, see <http://www.gnu.org/licenses/>.
#
#   As a special exception, the respective Autoconf Macro's copyright owner
#   gives unlimited permission to copy, distribute and modify the configure
#   scripts that are the output of Autoconf when processing the Macro. You
#   need not follow the terms of the GNU General Public License when using
#   or distributing such scripts, even though portions of the text of the
#   Macro appear in them. The GNU General Public License (GPL) does govern
#   all other use of the material that constitutes the Autoconf Macro.
#
#   This special exception to the GPL applies to versions of the Autoconf
#   Macro released by the Autoconf Macro Archive. When you make and
#   distribute a modified version of the Autoconf Macro, you may extend this
#   special exception to the GPL to apply to your modified version as well.

AC_DEFUN([AC_CHECK_CLASSPATH],[
if test "x$CLASSPATH" = x; then
        echo "You have no CLASSPATH, I hope it is good"
else
        echo "You have CLASSPATH $CLASSPATH, hope it is correct"
fi
])


# ===========================================================================
#              http://autoconf-archive.cryp.to/ac_prog_jar.html
# ===========================================================================
#
# SYNOPSIS
#
#   AC_PROG_JAR
#
# DESCRIPTION
#
#   AC_PROG_JAR tests for an existing jar program. It uses the environment
#   variable JAR then tests in sequence various common jar programs.
#
#   If you want to force a specific compiler:
#
#   - at the configure.in level, set JAR=yourcompiler before calling
#   AC_PROG_JAR
#
#   - at the configure level, setenv JAR
#
#   You can use the JAR variable in your Makefile.in, with @JAR@.
#
#   Note: This macro depends on the autoconf M4 macros for Java programs. It
#   is VERY IMPORTANT that you download that whole set, some macros depend
#   on other. Unfortunately, the autoconf archive does not support the
#   concept of set of macros, so I had to break it for submission.
#
#   The general documentation of those macros, as well as the sample
#   configure.in, is included in the AC_PROG_JAVA macro.
#
# LAST MODIFICATION
#
#   2008-04-12
#
# COPYLEFT
#
#   Copyright (c) 2008 Egon Willighagen <e.willighagen@science.ru.nl>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved.

AC_DEFUN([AC_PROG_JAR],[
AC_REQUIRE([AC_EXEEXT])dnl
if test "x$JAVAPREFIX" = x; then
        test "x$JAR" = x && AC_CHECK_PROGS(JAR, jar$EXEEXT)
else
        test "x$JAR" = x && AC_CHECK_PROGS(JAR, jar, $JAVAPREFIX)
fi
test "x$JAR" = x && AC_MSG_ERROR([no acceptable jar program found in \$PATH])
AC_PROVIDE([$0])dnl
])
