#!/bin/bash

## get env
. $(dirname $0)/../../env.sh

## template definition
man=local.generic
libmsgque_private=../man/Doxyfile.msgque
ccmsgque_private=../man/Doxyfile.msgque
javamsgque_private=../man/Doxyfile.msgque
pymsgque_private=../man/Doxyfile.msgque
rubymsgque_private=../man/Doxyfile.msgque
tclmsgque_private=../man/Doxyfile.msgque
msgqueforphp_private=../man/Doxyfile.msgque

main=../man/Doxyfile.msgque

libmsgque=../libmsgque_public/Doxyfile
javamsgque=../javamsgque_public/Doxyfile
ccmsgque=../ccmsgque_public/Doxyfile
csmsgque=../csmsgque_public/Doxyfile
gomsgque=../libmsgque_public/Doxyfile
vbmsgque=../csmsgque_public/Doxyfile
perlmsgque=../libmsgque_public/Doxyfile
msgqueforphp=../libmsgque_public/Doxyfile
pymsgque=../libmsgque_public/Doxyfile
rubymsgque=../libmsgque_public/Doxyfile
tclmsgque=../libmsgque_public/Doxyfile

export HAVE_DOT=NO
[[ $1 == "-usedot" && ! -z "$DOT" ]] && {
    HAVE_DOT=YES
    shift
}
[[ $1 == "-release" && ! -z "$DOT" ]] && {
    RELEASE=YES
    shift
}
(( $# < 1 )) && {
    echo "usage: $(basename $0) [-usedot] [ALL|DIRECTORIES...]" 1>&2
    exit 1
}
[[ -z "$DOXYGEN" ]] && {
    echo ERROR: no 'doxygen' support available 2>&1
    exit 1
}
[[ $1 == "ALL" ]] && {
    set man *_private main
}
(
  for D
  do
    echo "======================================================================="
    if test -f $D ; then
      FN=$(basename $D)
      D=$(dirname $D)
    else
      FN="Doxyfile*"
    fi
    pushd $D || {
      echo ERROR: $1 is no directory
      exit 2
    }
    shift

  ## customizing
    export  SRCDIR="$abs_top_builddir/theLink"
    export  VERSION="$LIBMSGQUE_VERSION"
    export  PERL_PATH="$DOXYPERL"
    export  DOT_PATH="$(dirname '$DOT')"
    export  GENERATE_MAN='NO' 
    export  GENERATE_RTF='NO' 
    export  GENERATE_XML='NO' 
    export  GENERATE_HTMLHELP='NO' 
    export  GENERATE_CHI='NO' 
    export  GENERATE_HTML='YES' 
    export  GENERATE_LATEX='NO' 
    if [[ "$RELEASE" == "YES" ]] ; then
      export  HTML_FOOTER="../footer.html"
    else
      export  HTML_FOOTER=""
    fi

  ## search and generate the files
    for F in $FN ; do
      test ! -f $F && continue
      P="${F##*.}"
      test "$F" == "$P" && P="$(basename $PWD)"
      test "$D" == "$P" && D=dummy
      echo "#########################################################################"
      echo "start: $F"
      export  PROJECT="$P"
      export  DOCDIR="$abs_top_builddir/html/theLink/$PROJECT"
      echo "PROJECT: $PROJECT"
      echo "DOCDIR: $DOCDIR"
      rm -fr "$DOCDIR"
      mkdir -p "$DOCDIR"
      echo "cat $SRCDIR/docs/Doxyfile.generic ${!D} ${!P} $F | $DOXYGEN"
      cat "$SRCDIR/docs/Doxyfile.generic" ${!D} ${!P} "$F" | "$DOXYGEN" - 2>&1
    done

    popd
  done
)

