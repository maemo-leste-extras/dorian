#!/bin/bash
wget --quiet \
  "http://maemo.org/download-stats/fetch.php?unixname=dorian&os=Maemo5" -O -|\
  awk -F'[' \
    '/^ *var d/{for(x=1;x<NF;x++){split($x,a,",");sum+=a[2]}}END{print sum}'
