SUMMARY = "A simple Qt test, prints "test" 10 times"

LICENSE = "CLOSED"

SRC_URI = "file://hotplugwatcher.tar.gz "

S = "${WORKDIR}"

inherit qt4e

QMAKE_PROFILES = "${S}/hotplugwatcher.pro"

do_install() {
	install -d ${D}${bindir}
	install -m 0755 ${S}/${BPN} ${D}${bindir}
}

