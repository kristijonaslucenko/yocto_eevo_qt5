SUMMARY = "A simple Qt test, prints "test" 10 times"

LICENSE = "CLOSED"

PR = "r2"

SRC_URI = "file://qtest.tar.gz "

S = "${WORKDIR}"

inherit qt4e


do_install() {
	install -d ${D}${bindir}
	install -m 0755 ${S}/${BPN} ${D}${bindir}
}

