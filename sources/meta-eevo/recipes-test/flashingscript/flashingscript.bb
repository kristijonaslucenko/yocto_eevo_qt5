DESCRIPTION = "Copies files to the image"
LICENSE = "CLOSED"
PV = '0.0.1'

FILESEXTRAPATHS_prepend := "${THISDIR}/flashingscript:"
SRC_URI += "file://barebox.bin  file://flash_script.sh file://MLO file://sx "

S = "${WORKDIR}"

FILES_SOLIBSDEV = ""
FILES_${PN} += "${libdir}/*"

do_install() {
  install -d ${D}${libdir}
  install -m 755 barebox.bin ${D}${libdir}
  install -m 755 flash_script.sh ${D}${libdir}
  install -m 755 MLO ${D}${libdir}
  install -m 755 sx ${D}${libdir}
}


