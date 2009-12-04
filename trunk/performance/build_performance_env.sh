#! /bin/bash
#+
#§  \file       performance/build_performance_env.sh
#§  \brief      \$Id$
#§  
#§  (C) 2009 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

export PATH=$PWD:/bin:/usr/bin
export LD_LIBRARY_PAT=""
test ! -f ./env.sh && {
  echo "ERROR: you have to run the '../configure' script first"
  exit 1
}
set -x
. ./env.sh

## create: $PACKAGE-$PACKAGE_VERSION.tar.gz
(cd ..; make dist) || exit 1

## config/build/install
Build() {
  (
    BUILD=$1; shift
    rm -fr $BUILD
    mkdir $BUILD
    cd $BUILD
    tar -xzf ../../$PACKAGE-$PACKAGE_VERSION.tar.gz || exit 1

    (
      cd $PACKAGE-$PACKAGE_VERSION

      ../../performance_$BUILD.env ./configure --prefix=$OLDPWD $@ \
	--enable-java --enable-python --enable-csharp --enable-cxx \
	  --enable-tcl --enable-perl --enable-vb --enable-static || exit 1

      make || exit 1

      make install || exit 1
    )
  )
}

## main
Build thread --enable-threads
Build nothread 
