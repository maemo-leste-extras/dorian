#!/bin/bash

set -x

sh autobuild.sh

VERSION=`cat ../version.txt`
BUILD_DIR=/home/user/dorian-auto/dorian-${VERSION}
SB_BUILD_DIR=/scratchbox/users/`id -un`${BUILD_DIR}
[ ! -d ${SB_BUILD_DIR} ] && { echo "Missing $SB_BUILD_DIR"; exit 1; }
chmod a+rx ${SB_BUILD_DIR}/pkg/maemo/build-scratchbox.sh
scratchbox fakeroot ${BUILD_DIR}/pkg/maemo/build-scratchbox.sh
