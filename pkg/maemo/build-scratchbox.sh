#!/bin/bash

# Creates .deb package for Maemo, using scratchbox.
# Assumes all source files are copied to $BUILD_DIR and build preparations
# were done in $AUTO_DIR.

set -x

BUILD_DIR=/home/user/dorian-build
AUTO_DIR=/home/user/dorian-auto

[ `id -u` != "0" ] && { echo "Run as root"; exit 1; }
[ ! -d ${AUTO_DIR} ] && { echo "Missing $AUTO_DIR"; exit 1; }
[ ! -d ${BUILD_DIR} ] && { echo "Missing $BUILD_DIR"; exit 1; }

VERSION=`cat ${BUILD_DIR}/pkg/version.txt`
cd ${AUTO_DIR}/dorian-${VERSION} || exit 1
dpkg-buildpackage -rfakeroot
