SUMMARY = "Eevo's headless image"
DESCRIPTION = "no graphics support in this image"
LICENSE = "MIT"
inherit core-image

CONFLICT_DISTRO_FEATURES = "x11 wayland"
REQUIRED_DISTRO_FEATURES = "opengl"

inherit core-image distro_features_check populate_sdk_qt5


IMAGE_ROOTFS_SIZE ?= "8192"

IMAGE_INSTALL = " \
    packagegroup-core-boot \
    packagegroup-hwtools \
    packagegroup-benchmark \
    packagegroup-userland \
    packagegroup-base \
    ${@bb.utils.contains("MACHINE_FEATURES", "alsa", "packagegroup-audio", "", d)} \
    serial-test \
    flashingscript \
    hotplugwatcher \
    minicom \
    qtest \
    sound \
    gnupg \
    libarchive \
    qtbase \
    qtbase-plugins \
    qtbase-fonts \
    qtsvg \
    qtsvg-plugins \
    qtdeclarative \
    qtdeclarative-plugins \
    qtdeclarative-qmlplugins \
    qtquickcontrols-qmlplugins \
    qtmultimedia \
    qtmultimedia-plugins \
    qtmultimedia-qmlplugins \ 	
    ${@bb.utils.contains("MACHINE_FEATURES", "sgx", "omap3-sgx-modules libgles2 libegl", "",d)} \
"

# qtwebkit \
# qtwebkit-qmlplugins \
