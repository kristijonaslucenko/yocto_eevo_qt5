#
# This file was derived from the 'Hello World!' example recipe in the
# Yocto Project Development Manual.
#

DESCRIPTION = "Simple helloworld application"
SECTION = "testing"
DEPENDS = ""

FILESEXTRAPATHS_prepend := "${THISDIR}/serial-test:"

LICENSE = "CLOSED"

USER = "kristijonaslucenko"

PR = "r0"

SRC_URI = "file://linux-serial-test.c"

S = "${WORKDIR}"

do_compile() {
	${CC} linux-serial-test.c -o serial-test
}

do_install() {
	install -d ${D}${bindir}
	install -m 0755 serial-test ${D}${bindir}
}
