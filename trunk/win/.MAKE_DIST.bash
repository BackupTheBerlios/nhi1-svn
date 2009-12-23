#!/usr/bin/env bash
#+
#§  \file       win/.MAKE_DIST.bash
#§  \brief      \$Id$
#§  
#§  (C) 2009 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

. ./env.sh

PKG=${PACKAGE}-${PACKAGE_VERSION}-i686-pc-win32
FINAL_PKG=../binary-dist/$PKG.zip

(cd ../theLink/tclmsgque; $TCLSH ./.MAKE_INDEX.tcl;)

rm -fr $PKG
mkdir $PKG
mkdir $PKG/bin
mkdir $PKG/lib
mkdir $PKG/include

for f in $(find .. -type f | grep -v win) ; do
    [[ "$f" == *win* ]] && continue
    [[ "$f" == *example* ]] && continue
    [[ "$f" == *tests* ]] && continue
    [[ "$f" == *depends* ]] && continue
    [[ "$f" == *py* ]] && continue
    case "$f" in
	*aguard.exe)	    cp $f $PKG/bin;;
	*.dll)		    cp $f $PKG/bin;;
	*.pyd)		    cp $f $PKG/bin;;
	*.lib)		    cp $f $PKG/lib;;
	*.exp)		    cp $f $PKG/lib;;
	*.jar)		    cp $f $PKG/bin;;
	*/msgque.h)	    cp $f $PKG/include;;
	*/ccmsgque.h)	    cp $f $PKG/include;;
	*/pkgIndex.tcl)	    cp $f $PKG/bin;;
    esac
done

zip -r $FINAL_PKG $PKG


