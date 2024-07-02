FWBOX="ssh,host=172.22.0.3,port=22"
FWBOX_GPIO_RESET="gpio@48000000 0"
FWBOX_GPIO_POWER="gpio@48000000 1"

fwbox_gpioset() {
    fwbox_gpioset_zephyr "$@"
}

. $FWBOX_PATH/common.sh
