#!/bin/bash

# Prepare source for uploading to the Maemo autobuilder

set -x

SRC_DIR=/home/polster/workspace/dorian
BUILD_DIR=/home/user/dorian-build
SB_BUILD_DIR=/scratchbox/users/`id -un`${BUILD_DIR}

[ ! -d ${SRC_DIR} ] && { echo "Missing $SRC_DIR"; exit 1; }
[ -d ${SB_BUILD_DIR} ] && rm -rf ${SB_BUILD_DIR}
mkdir -p ${SB_BUILD_DIR}
cp -R ${SRC_DIR}/* ${SB_BUILD_DIR}
sb-conf select FREMANTLE_ARMEL
chmod a+rx ${SB_BUILD_DIR}/pkg/maemo/autobuild-scratchbox.sh
scratchbox fakeroot ${BUILD_DIR}/pkg/maemo/autobuild-scratchbox.sh
