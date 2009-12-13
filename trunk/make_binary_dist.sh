#!/bin/sh
#+
#§  \file       make_binary_dist.sh
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
  echo "ERROR: you have to run the './configure' script first"
  exit 1
}
set -x
. ./env.sh
TOPDIR="$PWD"

make dist

rm -fr binary-build
mkdir binary-build
cd binary-build

tar -xf ../$PACKAGE-$PACKAGE_VERSION.tar.gz

cd $PACKAGE-$PACKAGE_VERSION

PKG=${PACKAGE}-${PACKAGE_VERSION}-${host}
FINAL_PKG=$TOPDIR/binary-dist/$PKG.zip

export PATH=$HOME/ext/$MACHTYPE/thread/bin:$PATH

ARGS=""
test "$USE_JAVA"    == "yes"  && ARGS="$ARGS --enable-java"
test "$USE_PYTHON"  == "yes"  && ARGS="$ARGS --enable-python"
test "$USE_CSHARP"  == "yes"  && ARGS="$ARGS --enable-csharp"
test "$USE_VB"	    == "yes"  && ARGS="$ARGS --enable-vb"
test "$USE_PERL"    == "yes"  && ARGS="$ARGS --enable-perl"
test "$USE_TCLSH"   == "yes"  && ARGS="$ARGS --enable-tcl"
test "$USE_CXX"	    == "yes"  && ARGS="$ARGS --enable-cxx"

bash ./configure --prefix=/usr/local --enable-threads $ARGS || exit 1

make || exit 1

test -d /tmp/$PKG || mkdir /tmp/$PKG

make DESTDIR=/tmp/$PKG/ install || exit 1

test -f $FINAL_PKG && rm $FINAL_PKG

(cd /tmp; zip -r $FINAL_PKG $PKG)

make uninstall

test -d /tmp/$PKG && rm -fr /tmp/$PKG
