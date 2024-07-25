#!/bin/sh

#
# This script uses a pre-build image for ARAnyM,
# where Pure-C, GEMINI and a few other tools are installed,
# a custom pcmake tool to build a Pure-C project file from the
# command line, and mtools to access the image
#

export BUILDROOT="${SCRIPT_DIR}/tmp"
export OUT="${SCRIPT_DIR}/out"
echo "BUILDROOT=$BUILDROOT" >> $GITHUB_ENV 
echo "OUT=$OUT" >> $GITHUB_ENV

mkdir -p "${BUILDROOT}"
mkdir -p "${OUT}"

aranym="${SCRIPT_DIR}/aranym"
aranym="$aranym/usr/bin/aranym-jit"

unset CC CXX

SRCDIR=c:/src/wifi_da

clash_option="-D s"
mmd $clash_option $SRCDIR
mcopy $clash_option -bso *.c *.h *.rsh *.prj $SRCDIR
mcopy $clash_option -bso scsidrv/*.h c:/pc/include/scsidrv

export SDL_VIDEODRIVER=dummy
export SDL_AUDIODRIVER=dummy

mdel C:/status.txt
mcopy -o ${SCRIPT_DIR}/autobld.sh C:/autobld.sh
"$aranym" -c config-hdd
echo ""
echo "##################################"
echo "error output from emulator run:"
echo "##################################"
mtype C:/errors.txt | grep -v ": entering directory" | grep -v ": processing "
echo ""
status=`mtype -t C:/status.txt`
mtype "$SRCDIR/pcerr.txt" | grep -F "Error 
Fatal 
Warning "
echo ""
test "$status" != "0" && exit 1

mcopy -b "$SRCDIR/wifi_da.acc" .
mcopy -b "$SRCDIR/wifi_dbg.prg" .

zip wifi_da-atari.zip wifi_dbg.prg wifi_da.acc
