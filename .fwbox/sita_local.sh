FWBOX="ssh,host=192.168.15.100,port=22"
FWBOX_GPIO_RESET="x 22"
FWBOX_GPIO_POWER="x 20"

fwbox_gpioset() {
    fwbox_gpioset_micropython "$@"
}

. $FWBOX_PATH/common.sh
