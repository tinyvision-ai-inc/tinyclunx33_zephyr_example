FWBOX="local"
FWBOX_DEV_VIDEO="/dev/video2"
FWBOX_DEV_ACM="/dev/ttyACM0"

fwbox_do_power_cycle() {
	printf 'Power cycle the board and press [enter]: \a' >/dev/tty
	read </dev/tty
}

fwbox_do_reset() {
	printf 'Hit SW1 and press [enter]: \a' >/dev/tty
	read </dev/tty
}

. $FWBOX_PATH/.fwbox/common.sh
