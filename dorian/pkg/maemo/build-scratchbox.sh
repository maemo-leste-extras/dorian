#!/bin/bash

# Creates .deb package for Maemo, using scratchbox.
# Assumes all source files are copied to $BUILD_DIR.

set -x

BUILD_DIR=/home/user/dorian-build
PKG_DIR=/home/user/dorian-pkg

[ `id -u` != "0" ] && { echo "Run as root"; exit 1; }
[ ! -d ${BUILD_DIR} ] && { echo "Missing $BUILD_DIR"; exit 1; }
[ ! -x /usr/bin/uuencode ] && { echo "Missing /usr/bin/uuencode"; exit 1; }

VERSION=`cat ${BUILD_DIR}/pkg/version.txt`
cd ${BUILD_DIR}
make distclean || true
qmake && make || exit 1

[ -d ${PKG_DIR}/root ] && rm -rf ${PKG_DIR}/root
install -m 755 -d ${PKG_DIR}/root
install -m 755 -d ${PKG_DIR}/root/usr
install -m 755 -d ${PKG_DIR}/root/usr/bin
install -m 755 -d ${PKG_DIR}/root/usr/share/icons/hicolor/48x48
install -m 755 -d ${PKG_DIR}/root/usr/share/icons/hicolor/scalable/hildon
install -m 755 -d ${PKG_DIR}/root/usr/share/applications/hildon
install -m 755 -d ${PKG_DIR}/root/DEBIAN
install -m 755 ${BUILD_DIR}/dorian ${PKG_DIR}/root/usr/bin/dorian
install -m 644 ${BUILD_DIR}/pkg/maemo/control ${PKG_DIR}/root/DEBIAN/control
install -m 644 ${BUILD_DIR}/pkg/changelog ${PKG_DIR}/root/DEBIAN/changelog
install -m 755 ${BUILD_DIR}/pkg/maemo/postinst ${PKG_DIR}/root/DEBIAN/postinst
install -m 644 ${BUILD_DIR}/pkg/maemo/dorian-48.png \
    ${PKG_DIR}/root/usr/share/icons/hicolor/48x48/dorian.png
install -m 644 ${BUILD_DIR}/pkg/maemo/dorian-64.png \
    ${PKG_DIR}/root/usr/share/icons/hicolor/scalable/hildon/dorian.png
install -m 644 ${BUILD_DIR}/pkg/maemo/dorian.desktop \
    ${PKG_DIR}/root/usr/share/applications/hildon/dorian.desktop
echo "Version: $VERSION" >> ${PKG_DIR}/root/DEBIAN/control
echo "Installed-Size: "`du -c -k ${PKG_DIR} | tail -1 | cut -f1` \
    >> ${PKG_DIR}/root/DEBIAN/control
echo "XB-Maemo-Icon-26:" >> ${PKG_DIR}/root/DEBIAN/control
uuencode -m ${BUILD_DIR}/pkg/maemo/dorian-48.png dorian.png | \
    (read; cat) | grep -v '^====' | \
    sed 's/^/ /' >> ${PKG_DIR}/root/DEBIAN/control
chmod a+r ${PKG_DIR}/root/DEBIAN/control
dpkg-deb -b ${PKG_DIR}/root ${PKG_DIR}/dorian-maemo-${VERSION}.deb
echo "Built $PKG_DIR/dorian-maemo-$VERSION.deb"
