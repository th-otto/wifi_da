#!mupfel

#
# a mupfel script, executed inside ARAnyM
#

set -e

cd C:\src\wifi_da
echo "%033v%c"

ERRFILE="C:\src\wifi_da\pcerr.txt"
rm -f "$ERRFILE"

export PCCFLAGS=-DDEBUG_LOGGING=1
pcmake -B -F wifi_da.prj >> "$ERRFILE"
mv wifi_da.acc wifi_dbg.prg
unset PCCFLAGS
pcmake -B -F wifi_da.prj >> "$ERRFILE"

set +e
