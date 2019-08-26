#!/bin/sh
PKG_NAME="modbus_connect"
BUILDROOT="/home/openwrt/code/openwrt-18.06"
CLEAN="package/$PKG_NAME/clean"
COMPILE="package/$PKG_NAME/compile"
REPO="mips_24kc"
TARGET="bin/packages/$REPO/base/$PKG_NAME*.ipk root@192.168.1.1:/tmp"

echo "changing to directory $BUILDROOT"
cd $BUILDROOT
echo "making 'make $CLEAN'"
make $CLEAN

if [ $? -eq 0 ];
then
    echo "'make $CLEAN' -> OK"
else
    echo "'make $CLEAN' -> ERROR: $?"
fi

echo "making 'make $COMPILE V=s'"
make $COMPILE V=s

if [ $? -eq 0 ];
then
    echo "'make $COMPILE' -> OK"
else
    echo "'make $COMPILE' -> ERROR: $?"
fi

echo "copying file to router: 'scp $TARGET'"
scp $TARGET
if [ $? -eq 0 ];
then
    echo "'scp $TARGET' -> OK"
else
    echo "'scp $TARGET' -> ERROR: $?"
fi

return $?