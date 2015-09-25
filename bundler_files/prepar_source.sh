#!/bin/bash

VERSION="0.21.3"

#SCRIPT=$(readlink -f $0)
# Absolute path this script is in, thus /home/user/bin
#DEST_PATH=$(dirname $SCRIPT)
DEST_PATH="/Users/quentinbramas/Documents"
echo "DEST_PATH = $DEST_PATH"

mkdir /tmp/texiteasy-$VERSION
cp -r ../source/* /tmp/texiteasy-$VERSION
mkdir /tmp/texiteasy-$VERSION/share
cp -r deb/share/* /tmp/texiteasy-$VERSION/share
cd /tmp
tar czf $DEST_PATH/texiteasy.tar.gz texiteasy-$VERSION
rm -r /tmp/texiteasy-$VERSION
#mv /tmp/texiteasy-0.1.10.tar.gz /tmp/myscript/texiteasy-$VERSION.tar.gz
