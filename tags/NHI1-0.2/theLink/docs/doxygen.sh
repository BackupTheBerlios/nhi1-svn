#!/bin/bash

## get env
. $(dirname $0)/../../env.sh

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
    set libmsgque* ccmsgque* tclmsgque* pymsgque* javamsgque* csmsgque* man main
}
(
  for D
  do
    echo "======================================================================="
    pushd $D || {
      echo ERROR: $1 is no directory
      exit 2
    }
    shift

  ## customizing
    export  SRCDIR="$abs_top_builddir/theLink"
    export  VERSION="$PACKAGE_VERSION"
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

  ## is a local Doxyfile available ?
    if [[ -f local.generic ]] ; then
      LG="local.generic"
    else
      LG=""
    fi

  ## search and generate the files
    for F in Doxyfile* ; do
      test ! -f $F && continue
      P="${F##*.}"
      test "$F" == "$P" && P="$(basename $PWD)"
      echo "#########################################################################"
      echo "start: $F"
      export  PROJECT="$P"
      export  DOCDIR="$abs_top_builddir/html/theLink/$PROJECT"
      rm -fr "$DOCDIR"
      mkdir -p "$DOCDIR"
      cat "$SRCDIR/docs/Doxyfile.generic" $LG "$F" | \
	"$DOXYGEN" - 2>&1
    done

    popd
  done
)
