# sudo git clone https://github.com/firmware-box/fwbox /opt/fwbox
. /opt/fwbox/fwbox.sh

# Description of how to connect to various resources
FWBOX="ssh,host=172.22.0.3,port=22"
FWBOX_GPIO="$FWBOX picocom,port=/dev/ttyACM0 micropython"
FWBOX_LOGS="$FWBOX console,port=/dev/ttyUSB1,baud=153600"

# Configuration for built-in actions
FWBOX_GPIO_RESET="gpio@48000000 0"
FWBOX_GPIO_POWER="gpio@48000000 1"

# Alias to choose the syntax for built-in actions
alias fwbox_gpioset=fwbox_gpioset_zephyr

fwbox_do_flash_zephyr() {
    fwbox_flash 0x100000 <build/zephyr/zephyr.bin
}
