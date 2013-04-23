#!/bin/bash -u
#+
#:  \file       tests/TestControl.bash
#:  \brief      \$Id$
#:  
#:  (C) 2013 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

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
G_Date='$Date$'
G_Revision='$Revision$'
G_Author='$Author: aotto1968 $'
G_Source='$URL: svn+ssh://svn.berlios.de/svnroot/repos/nhi1/trunk/bin/Nhi1Config $'
# --------------------------------------------------------------------
G_Description='frontend to the mass distribution test'
#G_HelpProc='Usage'
# -------------------------------------------------------------------
G_Option break-on-non-option
G_Argument 'static:static:yes:test the static distribution:B'
G_Argument 'only:only:*:restrict language to:L:*:c:cxx:tcl:perl:python:php:ruby:java:csharp:go'
G_Argument 'clean:clean:no:clean all left-over data:B'
EOF
  )"

if test "$clean" = "yes" ; then
  rm -f distcheck.*.log
  chmod -R u+w ../test-*
  rm -fr ../test-*
  exit 0
fi

export PATH="/usr/local/bin:$PATH"

# use distribution
dist="$abs_top_builddir/$PACKAGE-$PACKAGE_VERSION.tar.bz2"

# test cases

declare -A threadsLng=( \
  [yes]="c cxx tcl perl python php ruby java csharp go" \
  [no]="c cxx tcl perl python php ruby" \
)
declare -A staticLng=( \
  [yes]="c cxx" \
  [no]="c cxx tcl perl python php ruby java csharp go" \
)
declare -a tests
declare -a flags
declare -i num=0

rm -f distcheck.*.log

# DISTCHECK_CONFIGURE_FLAGS=--with-guard TESTS_ENVIRONMENT="/home/dev01/Project/NHI1/bin/Nhi1Exec --testing --only-c"

for threads in yes no ; do
  for static in yes no; do
	for threadsL in ${threadsLng[$threads]} ; do
	  for staticL in ${staticLng[$static]} ; do
		if [[ "$threadsL" == "$staticL" && "$threadsL" == $only ]] ; then
		  tests=($abs_top_srcdir/bin/Nhi1Exec --only-$threadsL)
		  flags=(--enable-threads=$threads --enable-static=$static)
		  test "$threadsL" != "c" && flags+=(--with-$staticL=yes)
		  echo "run job $num: ${flags[*]}"
		  IFS=',' ; sem --id "$$" -j150% "$abs_top_srcdir/tests/TestJob.bash" $num $PACKAGE_VERSION "${flags[*]}" "${tests[*]}" ; unset IFS
		  (( num+=1 ))
		fi
	  done
	done
  done
done

sem --id "$$" --wait

for ((i=0;i<num;i++)) ; do
  if test -e ./distcheck.$i.log ; then
	echo "test '$i' failed"
	echo "====================================================="
	head -2 ./distcheck.$i.log
	echo "====================================================="
	tail -10 ./distcheck.$i.log
  fi
done

exit 0

