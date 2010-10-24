#!/bin/bash

echo 'DX_DIST = \' > list.mk

for f in $(svn list -R 2>&1) ; do
  [[ "$f" == "doxygen.sh" ]] && continue
  test -d $f && continue
  echo "$f \\" >> list.mk
done

echo "doxygen.sh" >> list.mk

