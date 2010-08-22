#!/bin/bash

# Do preparations for maemo.org autobuilder: Create changes, dsc and tar.gz
# files in $AUTO_DIR. Assumes all source files are copied to $BUILD_DIR.

set -x

BUILD_DIR=/home/user/dorian-build
AUTO_DIR=/home/user/dorian-auto

[ `id -u` != "0" ] && { echo "Run as root"; exit 1; }
[ ! -d ${BUILD_DIR} ] && { echo "Missing $BUILD_DIR"; exit 1; }
[ ! -x /usr/bin/uuencode ] && { echo "Missing /usr/bin/uuencode"; exit 1; }

VERSION=`tr -d '"' < ${BUILD_DIR}/pkg/version.txt`
cd ${BUILD_DIR}
make distclean 2>/dev/null || true

rm -rf ${AUTO_DIR}
mkdir -p ${AUTO_DIR}
cp -Rf ${BUILD_DIR} ${AUTO_DIR}/dorian-${VERSION}
cd ${AUTO_DIR}/dorian-${VERSION}
mkdir debian
cp pkg/changelog pkg/maemo/control pkg/maemo/rules pkg/maemo/postinst \
    pkg/maemo/optify debian
chmod a+rx debian/postinst
echo "Version: $VERSION" >> debian/control
echo "Installed-Size: "`du -c -k ${AUTO_DIR} | tail -1 | cut -f1` \
    >> debian/control
echo "XB-Maemo-Icon-26:" >> debian/control
uuencode -m pkg/maemo/icon-48/dorian.png dorian.png | \
    (read; cat) | grep -v '^====' | \
    sed 's/^/ /' >> debian/control

chmod -R a+r ${AUTO_DIR}
find ${AUTO_DIR} -type d -exec chmod a+x {} \;

echo "----------------------"
cat debian/control
echo "----------------------"

dpkg-buildpackage -rfakeroot -sa -S

echo "Autobuilder input is ready in ${AUTO_DIR}"
