#!/bin/bash

VERSION="0.2.4"

SCRIPT=$(readlink -f $0)
# Absolute path this script is in, thus /home/user/bin
SCRIPTPATH=$(dirname $SCRIPT)
echo "executing script from $SCRIPTPATH"

mkdir /tmp/texiteasy-$VERSION
cp -r ../source/* /tmp/texiteasy-$VERSION
mkdir /tmp/texiteasy-$VERSION/share
cp -r deb/share/* /tmp/texiteasy-$VERSION/share
cd /tmp
tar czf $SCRIPTPATH/texiteasy.tar.gz texiteasy-$VERSION
#mv /tmp/texiteasy-0.1.10.tar.gz /tmp/myscript/texiteasy-$VERSION.tar.gz
