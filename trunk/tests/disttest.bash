#!/bin/bash

#
# The goal ist to make an complete-as-possible testsycle
#

# find env.sh config file
abs_top_builddir=$PWD
while [[ ! -f "$abs_top_builddir/env.sh" && "$abs_top_builddir" != '/' ]] ; do
  abs_top_builddir=$(dirname "$abs_top_builddir")
done
if [[ -r "${abs_top_builddir%/}/env.sh" ]] ; then
  echo "using: ${abs_top_builddir%/}/env.sh"
  . "${abs_top_builddir%/}/env.sh"
else
  echo "unable to find the 'env.sh' configuration file"
  echo "please build first and than use this tool in the build-tree"
  exit
fi

##
## =========================================================================
## Command-Line
##

eval "$($abs_top_srcdir/sbin/SetupEnv "$0" "$@" <<-'EOF'
G_Shell 'bash'
# ---------------------------------------------------------------------
G_Date='$Date: 2013-04-13 15:40:34 +0000 (Sa, 13. Apr 2013) $'
G_Revision='$Revision: 680 $'
G_Author='$Author: aotto1968 $'
G_Source='$URL: svn+ssh://svn.berlios.de/svnroot/repos/nhi1/trunk/bin/Nhi1Config $'
# --------------------------------------------------------------------
G_Description='frontend to the mass distribution test'
#G_HelpProc='Usage'
# -------------------------------------------------------------------
G_Option break-on-non-option
G_Argument 'static:static:yes:test the static distribution:B'
G_Argument 'jobs:jobs:1:number of parallel jobs:'
EOF
  )"


# use distribution
dist="$abs_top_builddir/$PACKAGE-$PACKAGE_VERSION.tar.bz2"

if test ! -s "$dist" ; then
  ( cd $abs_top_builddir && make dist )
fi

# test cases

declare -a L1 = ( --enable-static --enable-shared )

declare -a L2 = ( --with-cc )

declare -a L3 = ( --enable-thread=yes --enable-thread=no )

declare -a threadYesLng  = (c cxx tcl perl python php ruby java csharp go)
declare -a threadNoLng   = (c cxx tcl perl python php ruby)
declare -a staticYesLang = (c cxx)
declare -a staticNoLang  = (c cxx tcl perl python php ruby java csharp go)
declare -a enable        = (brain guard)



