#+
#:  \file       etc/.bashrc
#:  \brief      \$Id$
#:  
#:  (C) 2013 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

# There are 3 different types of shells in bash: the login shell, normal shell
# and interactive shell. Login shells read ~/.profile and interactive shells
# read ~/.bashrc; in our setup, /etc/profile sources ~/.bashrc - thus all
# settings made here will also take effect in a login shell.
#
# NOTE: It is recommended to make language settings in ~/.profile rather than
# here, since multilingual X sessions would not work properly if LANG is over-
# ridden in every subshell.

# Some applications read the EDITOR variable to determine your favourite text
# editor. So uncomment the line below and enter the editor of your choice :-)
#export EDITOR=/usr/bin/vim
#export EDITOR=/usr/bin/mcedit

# For some news readers it makes sense to specify the NEWSSERVER variable here
#export NEWSSERVER=your.news.server

# If you want to use a Palm device with Linux, uncomment the two lines below.
# For some (older) Palm Pilots, you might need to set a lower baud rate
# e.g. 57600 or 38400; lowest is 9600 (very slow!)
#
#export PILOTPORT=/dev/pilot
#export PILOTRATE=115200

test -s ~/.alias && . ~/.alias || true

# The CCACHE_DIR environment variable specifies where ccache will keep its cached compiler output. The default is
# $HOME/.ccache.
export CCACHE_DIR="/build/$USER/Cache"

# If you set the environment variable CCACHE_HARDLINK then ccache will attempt to use hard links from the cache directory when
# creating the compiler output rather than using a file copy. Using hard links may be slightly faster in some situations, but
# can confuse programs like make that rely on modification times. Another thing to keep in mind is that if the resulting
# object file is modified in any way, this corrupts the cached object file as well. Hard links are never made for compressed
# cache files. This means that you should not set the CCACHE_COMPRESS variable if you want to use hard links.
export CCACHE_HARDLINK=1

export SVN_SSH="ssh -l aotto1968"
export CTAGS="--c-kinds=+p --extra=+f"

# If  this  environment  variable exists, it will be expected to contain a set of default options which are read when ctags
# starts, after the configuration files listed in FILES, below, are read, but before any command  line  options  are  read.
# Options  appearing  on  the command line will override options specified in this variable. Only options will be read from
# this variable. Note that all white space in this variable is considered a separator, making  it  impossible  to  pass  an
# option parameter containing an embedded space. If this is a problem, use a configuration file instead.
export CTAGS="--c-kinds=+p"

if test "$OSTYPE" = "cygwin" ; then
  export TERM=xterm
  ## add missing system variables after login via ssh, need for compiling
  if [ "$SSH_TTY" ]; then
    pushd . >/dev/null
    for __dir in \
    /proc/registry/HKEY_LOCAL_MACHINE/SYSTEM/CurrentControlSet/Control/Session\ Manager/Environment \
    /proc/registry/HKEY_CURRENT_USER/Environment
    do
      cd "$__dir"
      for __var in *; do
	__upper=${__var^^}
	test -z "${!__upper}" && export $__var="$(<$__var)" >/dev/null 2>&1
      done
    done
    unset __dir
    unset __var
    unset __upper
    popd >/dev/null
  fi
fi

export NHI1_HOME=$(dirname $(dirname $(readlink -e ~/.bashrc)))
export NHI1_TOOL_ROOT="$HOME/ext/$MACHTYPE"
export PATH="$NHI1_HOME/bin:$PATH"

