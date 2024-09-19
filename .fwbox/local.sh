FWBOX="local"
FWBOX_DEV_TTYACM="/dev/ttyACM1"
FWBOX_DEV_VIDEO="/dev/video2"
FWBOX_GPIO_RESET="gpio@48000000 1"
FWBOX_GPIO_POWER="gpio@48000000 0"
FWBOX_GPIO="$FWBOX picocom,port=/dev/ttyACM0 repl"

fwbox_gpioset() {
    fwbox_gpioset_zephyr "$@"
}

. $FWBOX_PATH/.fwbox/common.sh

