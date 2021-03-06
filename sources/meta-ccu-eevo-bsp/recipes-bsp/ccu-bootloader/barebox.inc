SUMMARY = "The Barebox Bootloader"
DESCRIPTION = "Barebox (formerly known as u-boot-v2) is a bootloader that \
               inherits the best of U-Boot and the Linux kernel: The size and \
	       look-and-feel of u-boot, with driver model and lots of design \
	       concepts from the kernel. \
	       The bb recipe adds config fragment support and kconfig \
	       command handling using the yocto kern-tools-native and \
	       kconfig-frontends-native."
HOMEPAGE = "http://www.barebox.org"
SECTION = "bootloaders"
PROVIDES = "virtual/bootloader"
LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://COPYING;md5=057bf9e50e1ca857d0eb97bfe4ba8e5d"
SRC_URI = "http://barebox.org/download/barebox-${PV}.tar.bz2"

# SRC_URI_append = " \
#    file://0001-Makefile-add-TARGETCC.patch \
#    ${@bb.utils.contains("EXTRA_IMAGE_FEATURES", "debug-tweaks", "file://far.env.bin", "", d)} \
#"

SRC_URI_append = " \
    ${@bb.utils.contains("EXTRA_IMAGE_FEATURES", "debug-tweaks", "file://far.env.bin", "", d)} \
"
FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

inherit kernel-arch deploy
inherit kconfig

# Increase INC_PR everytime the include file is changed.
INC_PR = "r6"

PACKAGE_ARCH = "${MACHINE_ARCH}"

B = "${WORKDIR}/build"

# verbose build 0/1
VERBOSE_BUILD ?= "0"
EXTRA_OEMAKE = 'TARGETCC="${CC}" V=${VERBOSE_BUILD} CROSS_COMPILE=${TARGET_PREFIX} CC="${TARGET_PREFIX}gcc ${TOOLCHAIN_OPTIONS}"'
# target rootfs boot directory
base_bootdir = "/boot"
PATCHTOOL = "git"

DEPENDS += "lzop-native"

# name of the image created by barebox Makefile
BAREBOX_BIN ?= "ccu-bootloader.bin"
# filename of file to be deployed by this recipe
BAREBOX_IMAGE_BASE_NAME ?= "ccu-bootloader-${MACHINE}-${PKGV}-${PKGR}-${DATETIME}"
BAREBOX_IMAGE_BASE_NAME[vardepsexclude] += "DATETIME"
BAREBOX_BIN_SYMLINK ?= "ccu-bootloader.bin"

# recipe private variables
BAREBOX_COMMON_ENVDIR = "${WORKDIR}/commonenv"
BAREBOX_MACHINE_ENVDIR = "${WORKDIR}/machineenv"
BAREBOX_BOARD_ENVDIR = "${WORKDIR}/boardenv"


do_configure_append() {
    # Add environment directories to .config if they exist
    env_dirs=""
    for env_dir in .commonenv .boardenv .machineenv; do   # order is important!
        if test -d "${S}/$env_dir"; then
            env_dirs="$env_dirs $env_dir"
        fi
    done

    if test ! -z "$env_dirs"; then
        kconfig_set DEFAULT_ENVIRONMENT_PATH "\"$env_dirs\""
    fi
}

# its possible to give three environment folders to barebox: commonenv,
# boardenv and machineenv.
python do_prepare_env() {
    import shutil, subprocess
    S = d.getVar('S', True)
    machine_env_dir = d.getVar('BAREBOX_MACHINE_ENVDIR', True)
    board_env_dir = d.getVar('BAREBOX_BOARD_ENVDIR', True)
    common_env_dir = d.getVar('BAREBOX_COMMON_ENVDIR', True)
    if os.path.isdir(common_env_dir):
        bb.note("copying common environment to source directory")
        oe.path.copytree(common_env_dir, oe.path.join(S, '.commonenv'))
    if os.path.isdir(board_env_dir):
        bb.note("copying board environment to source directory")
        oe.path.copytree(board_env_dir, oe.path.join(S, '.boardenv'))
    if os.path.isdir(machine_env_dir):
        bb.note("copying machine environment to source directory")
        oe.path.copytree(machine_env_dir, oe.path.join(S, '.machineenv'))

    # Install the script far into the barebox environment. It will only be
    # added, if you enable "debug-tweaks" in EXTRA_IMAGE_FEATURES in your
    # local.conf.
    workdir_far_env_bin = os.path.join("${WORKDIR}", "far.env.bin")
    if os.path.isfile(workdir_far_env_bin):
        bb.utils.mkdirhier(os.path.join(S, ".commonenv/bin"))
        shutil.copyfile(workdir_far_env_bin,
                        os.path.join(S, ".commonenv/bin", "far"))
}
addtask prepare_env after do_patch before do_configure

do_compile () {
	unset CFLAGS LDFLAGS
	oe_runmake all
}

do_install () {
	bbnote "Installing barebox image on target rootfs"
	install -d ${D}${base_bootdir}
	install ${B}/${BAREBOX_BIN} ${D}${base_bootdir}/${BAREBOX_IMAGE_BASE_NAME}.bin
	ln -sf ${BAREBOX_IMAGE_BASE_NAME}.bin ${D}${base_bootdir}/${BAREBOX_BIN_SYMLINK}

	if [ -e ${B}/scripts/bareboxenv-target ] ; then
		bbnote "Installing target env tool"
		install -d ${D}${base_sbindir}
		install -m 744 ${B}/scripts/bareboxenv-target ${D}${base_sbindir}/bareboxenv
	fi

	if [ -e ${B}/scripts/bareboxcrc32-target ] ; then
		bbnote "Installing target crc32 tool"
		install -d ${D}${base_sbindir}
		install -m 744 ${B}/scripts/bareboxcrc32-target ${D}${base_sbindir}/bareboxcrc32
	fi

	if [ -e ${B}/scripts/kernel-install-target ] ; then
		bbnote "Installing target kernel-install tool"
		install -d ${D}${base_sbindir}
		install -m 744 ${B}/scripts/kernel-install-target ${D}${base_sbindir}/barebox-kernel-install
	fi
}
do_install[prefuncs] += "package_get_auto_pr"
FILES_${PN} = ""
FILES_${PN} += "${base_bootdir}"
PACKAGES =+ "${PN}-targettools"
FILES_${PN}-targettools += "${base_sbindir}/barebox*"

do_deploy () {
	bbnote "Deploying barebox"
	install -m 644 ${B}/${BAREBOX_BIN} ${DEPLOYDIR}/${BAREBOX_IMAGE_BASE_NAME}.bin
	ln -sf ${BAREBOX_IMAGE_BASE_NAME}.bin ${DEPLOYDIR}/${BAREBOX_BIN_SYMLINK}

	bbnote "Installing host barebox toolset"
	install -d ${STAGING_BINDIR_NATIVE}
	install -m 755 ${B}/scripts/bareboxenv ${STAGING_BINDIR_NATIVE}/

	install -m 644 ${B}/.config ${DEPLOYDIR}/${BAREBOX_IMAGE_BASE_NAME}.config
	ln -sf ${BAREBOX_IMAGE_BASE_NAME}.config ${DEPLOYDIR}/${PN}.config
}
do_deploy[prefuncs] += "package_get_auto_pr"
addtask deploy before do_build after do_compile
