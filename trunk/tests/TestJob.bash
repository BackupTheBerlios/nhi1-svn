#!/bin/bash -u
#+
#:  \file       tests/TestJob.bash
#:  \brief      \$Id$
#:  
#:  (C) 2013 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

num="$1"; shift

exec 1>./distcheck.$num.log 2>&1

echo "args = $*"

ver="$1"; shift
IFS=','
flags=($1); shift
tests=($1); shift
unset IFS

echo "flags = $flags"
echo "tests = $tests"
echo "==========================================================================="

if make -C .. distcheck PACKAGE=test-$num DISTCHECK_CONFIGURE_FLAGS="${flags[*]}" TESTS_ENVIRONMENT="${tests[*]}" ; then
#if (( $num % 2 )) ; then
  chown -R u+w ../test-$num-$ver
  rm -fr ../test-$num-$ver
  rm -f  ../test-$num-$ver.tar.bz2
  exec 1>/dev/null 2>&1
  rm ./distcheck.$num.log
fi

exit 0
