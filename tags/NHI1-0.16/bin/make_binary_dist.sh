#!/bin/sh
#+
#§  \file       bin/make_binary_dist.sh
#§  \brief      \$Id$
#§  
#§  (C) 2009 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§


test ! -f ./env.sh && {
  echo "ERROR: no './env.sh' in the current diretcory. have you run the './configure' script ?"
  exit 1
}
#set -x
. ./env.sh
cd $abs_top_builddir

set -x

STEP=${STEP:-1}

((STEP <= 1)) && make dist

((STEP <= 2)) && rm -fr binary-build
mkdir binary-build
cd binary-build

((STEP <= 3)) && tar -xjf ../$PACKAGE-$PACKAGE_VERSION.tar.bz2

cd $PACKAGE-$PACKAGE_VERSION

PKG=${PACKAGE}-${PACKAGE_VERSION}-${host}
FINAL_PKG=$abs_top_srcdir/binary-dist/$PKG.tar.bz2

export PATH=$HOME/ext/$MACHTYPE/thread/bin:$PATH

ARGS=""
test "$USE_JAVA"    == "yes"  && ARGS="$ARGS --enable-java"
test "$USE_PYTHON"  == "yes"  && ARGS="$ARGS --enable-python"
test "$USE_RUBY"    == "yes"  && ARGS="$ARGS --enable-ruby"
test "$USE_CSHARP"  == "yes"  && ARGS="$ARGS --enable-csharp"
test "$USE_GO"	    == "yes"  && ARGS="$ARGS --enable-go"
test "$USE_VB"	    == "yes"  && ARGS="$ARGS --enable-vb"
test "$USE_PERL"    == "yes"  && ARGS="$ARGS --enable-perl"
test "$USE_PHP"     == "yes"  && ARGS="$ARGS --enable-php"
test "$USE_TCL"     == "yes"  && ARGS="$ARGS --enable-tcl"
test "$USE_CXX"	    == "yes"  && ARGS="$ARGS --enable-cxx"
test "$USE_BRAIN"   == "yes"  && ARGS="$ARGS --enable-brain"
test "$USE_GUARD"   == "yes"  && ARGS="$ARGS --enable-guard"

if test "$ostype" == "cygwin"; then

  # Java -------------------------------------------------------------------------------------------

  export JAVA_HOME=/cygdrive/c/Programme/Java/jdk1.6.0_24/
  export PATH="$JAVA_HOME/bin:$PATH"

  # CSharp -----------------------------------------------------------------------------------------

  CSDIR=/cygdrive/c/Windows/Microsoft.NET/Framework64
  FrameworkDir=$(cygpath -w $CSDIR)
  FrameworkVersion=v4.0.30319
  PATH=$CSDIR/$FrameworkVersion:'/cygdrive/c/Program Files/Microsoft SDKs/Windows/v7.1/Bin/x64':$PATH

  # Main -------------------------------------------------------------------------------------------

  export CC="ccache x86_64-w64-mingw32-gcc"
  export CXX="ccache x86_64-w64-mingw32-g++"

  ARGS="--build=i686-pc-cygwin --host=x86_64-w64-mingw32 $ARGS"
fi


((STEP <= 4)) && ( bash ./configure --prefix=/usr/local --enable-static --enable-threads $ARGS || exit 1 )

((STEP <= 5)) && ( make || exit 1 )

echo "#########################################################"

test -d /tmp/$PKG || mkdir /tmp/$PKG

((STEP <= 6)) && ( make DESTDIR=/tmp/$PKG/ install || exit 1 )

test -f $FINAL_PKG && rm $FINAL_PKG

(cd /tmp; tar --format=ustar -chf - "$PKG" | bzip2 -9 -c >"$FINAL_PKG")

((STEP <= 7)) && make DESTDIR=/tmp/$PKG/ uninstall

test -d /tmp/$PKG && rm -fr /tmp/$PKG
