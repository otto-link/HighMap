#!/bin/bash

# format Doxygen comments in headers
INCLUDE_DIRS="HighMap/include"

echo "- uncrustify"

for F in `find ${INCLUDE_DIRS}/. -type f -iname \*.hpp`; do
    echo ${F}
    uncrustify -c scripts/uncrustify_config.cfg --replace ${F} --no-backup -q
done
