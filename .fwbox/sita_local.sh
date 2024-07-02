# Description of how to connect to various resources
FWBOX="ssh,host=192.168.15.100,port=22"

# Common config to all remote boxes
. $FWBOX_PATH/common.sh

# Configuration for built-in actions
FWBOX_GPIO_RESET="x 22"
FWBOX_GPIO_POWER="x 20"

# Alias to choose the syntax for built-in actions
fwbox_gpioset() {
    fwbox_gpioset_micropython "$@"
}
