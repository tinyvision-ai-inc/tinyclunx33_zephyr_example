# Description of how to connect to various resources
FWBOX="ssh,host=172.22.0.3,port=22"

# Common config to all remote boxes
. $FWBOX_PATH/common.sh

# Configuration for built-in actions
FWBOX_GPIO_RESET="gpio@48000000 0"
FWBOX_GPIO_POWER="gpio@48000000 1"

# Alias to choose the syntax for built-in actions
fwbox_gpioset() {
    fwbox_gpioset_zephyr "$@"
}
