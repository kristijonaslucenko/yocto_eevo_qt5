FILESEXTRAPATHS_prepend := "${THISDIR}/hotplugservice:"

FILES_${PN} += "${libdir}/*.service"

S = "${WORKDIR}"

inherit systemd

SYSTEMD_SERVICE_${PN} = "hotplug.service"

do_install_append () {
			install -d ${D}${systemd_unitdir}/system
			install -m 0644 ${WORKDIR}/hotplug.service ${D}${systemd_unitdir}/system/
			sed -i -e 's,@BASE_BINDIR@,${base_bindir},g' \
			-e 's,@SYSCONFDIR@,${sysconfdir},g' \
			-e 's,@BINDIR@,${bindir},g' \
			-e 's,@SYSTEMD_UNITDIR@,${systemd_unitdir},g' \
			${D}${systemd_unitdir}/system/hotplug.service
}

pkg_postinst_hotplugwatcher () {
#!/bin/sh
OPTS=""

if [ -n "$D" ]; then
    OPTS="--root=$D"
fi

if type systemctl >/dev/null 2>/dev/null; then
	systemctl $OPTS enable hotplug.service

	if [ -z "$D" -a "enable" = "enable" ]; then
		systemctl restart hotplug.service
	fi
fi
}


FILES_${PN} += "${systemd_unitdir}/system/"

