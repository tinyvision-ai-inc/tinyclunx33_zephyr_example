FWBOX="local"
FWBOX_VIDEO="video2"

. $FWBOX_PATH/.fwbox/common.sh

fwbox_do_power_cycle() {
	echo -n "Power cycle the board and press [enter]: " >/dev/tty
	read
}

fwbox_do_reset() {
	echo -n "Hit SW1 and press [enter]: " >/dev/tty
	read
}
