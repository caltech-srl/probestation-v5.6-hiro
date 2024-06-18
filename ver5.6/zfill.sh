#!/bin/sh
# 
# reset MISC
#
##########################################################################

./mterm.exe &
sleep 2
echo "FRESH ZFILL" |./mupld_c.exe
sleep 2
echo
echo ":q " |./mupld_c.exe
echo


exit
