#!/bin/sh

echo rvm_autoupdate_flag=0 >> ~/.rvmrc

# needed for mtools
sudo apt-get update

sudo apt-get install -y \
	curl \
	mtools \
	libjson-perl \
	libwww-perl \
	libsdl1.2-dev

curl --get https://tho-otto.de/download/mag-hdd.tar.bz2 --output mag-hdd.tar.bz2
curl --get https://tho-otto.de/download/aranym-1.1.0-xenial-x86_64-4ebb186.tar.xz --output aranym.tar.xz

tar xvf mag-hdd.tar.bz2
test -f config-hdd || exit 1

aranym="${SCRIPT_DIR}/aranym"
mkdir "$aranym"
cd "$aranym"
tar xvf ../../aranym.tar.xz
cd ../..

image=hdd.img
offset=0

echo "drive c: file=\"$PWD/$image\" MTOOLS_SKIP_CHECK=1 MTOOLS_LOWER_CASE=1 MTOOLS_NO_VFAT=1 offset=$offset" > ~/.mtoolsrc
