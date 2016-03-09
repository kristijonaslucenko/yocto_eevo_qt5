# Copyright (C) 2015 Christian Hemp <c.hemp@phytec.de>
# Released under the MIT license (see COPYING.MIT for the terms)

DESCRIPTION = "Phytec Gstreamer examples"
HOMEPAGE = "http://www.phytec.de"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=b234ee4d69f5fce4486a80fdaf4a4263"

SECTION = "multimedia"

S = "${WORKDIR}/gstreamer_examples"

PR = "r0"

SRC_URI = "ftp://ftp.phytec.de/pub/ImageProcessing/phyCAM_Software/i.MX6-gstreamer_examples/${@"${PN}".replace('-', '_')}-${PV}.tar.gz"

SRC_URI[md5sum] = "20704ebb8f488628187743b04781c180"
SRC_URI[sha256sum] = "2c7f3354b4e41e412e7e73eb9fdcf826519192ce049f10a9e90815cdeaadd0e6"

GSTREAMER_EXAMPLES_DIR = "/home/root/gstreamer_examples"

do_compile() {
	${CC} ${CFLAGS} ${LDFLAGS} -o tools/i2c tools/i2c.c
}

do_install() {
	DESTDIR="${D}${GSTREAMER_EXAMPLES_DIR}"

	for directory in `find -type d`; do
		if [ ${directory} != "./patches" ]; then
			install -d ${DESTDIR}/${directory}
		fi
	done

	for text in `find -name '*.txt'`; do
		install -m 0644 ${text} ${DESTDIR}/${text}
	done

	for scripts in `find -name '*.sh'`; do
		install -m 0755 ${scripts} ${DESTDIR}/${scripts}
	done

	install -m 0755 tools/i2c ${DESTDIR}/tools
}

FILES_${PN} = "${GSTREAMER_EXAMPLES_DIR}/*"
FILES_${PN}-dbg = " \
    ${prefix}/src \
    ${GSTREAMER_EXAMPLES_DIR}/tools/.debug \
"
RDEPENDS_${PN} += " \
	gst-plugin-i2c \
	media-ctl \
	v4l-utils \
	gstreamer1.0 \
	gstreamer1.0-plugins-good-multifile \
	gstreamer1.0-plugins-good-video4linux2 \
	gstreamer1.0-plugins-bad-fbdevsink \
	gstreamer1.0-plugins-bad-bayer \
	gstreamer1.0-plugins-good-jpeg \
"
