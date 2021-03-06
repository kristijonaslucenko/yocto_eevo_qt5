DESCRIPTION = "Copies files to the image"
LICENSE = "CLOSED"
PV = '0.0.1'

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"
SRC_URI += "file://police_s.wav  file://5.wav file://pew.wav file://stereo_test.wav "

S = "${WORKDIR}"

FILES_SOLIBSDEV = ""
FILES_${PN} += "${libdir}/*.wav"

do_install() {
  install -d ${D}${libdir}
  install -m 755 police_s.wav ${D}${libdir}
  install -m 755 5.wav ${D}${libdir}
  install -m 755 pew.wav ${D}${libdir}
  install -m 755 stereo_test.wav ${D}${libdir}
}


