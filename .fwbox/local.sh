FWBOX="local"

FWBOX_GPIO_RESET="gpio@48000000 0"
FWBOX_GPIO_POWER="gpio@48000000 1"

fwbox_gpioset() {
    fwbox_gpioset_zephyr "$@"
}

. $FWBOX_PATH/.fwbox/common.sh

#FWBOX_GPIOSET="${FWBOX:?} picocom,port=/dev/ttyACM1 repl"
#FWBOX_USB_CDC="${FWBOX:?} console,port=/dev/ttyACM2"
