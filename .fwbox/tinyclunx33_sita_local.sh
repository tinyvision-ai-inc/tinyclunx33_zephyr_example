# sudo git clone https://github.com/firmware-box/fwbox /opt/fwbox
. /opt/fwbox/fwbox.sh

# Description of how to connect to various resources
FWBOX="ssh,host=192.168.15.100,port=22"
FWBOX_GPIO="$FWBOX picocom,port=/dev/ttyACM0 zephyr"
FWBOX_LOGS="$FWBOX console,port=/dev/ttyUSB1,baud=153600"

# Configuration for built-in actions
FWBOX_GPIO_RESET="x 22"
FWBOX_GPIO_POWER="x 20"

# Alias to choose the syntax for built-in actions
alias fwbox_gpioset=fwbox_gpioset_micropython

fwbox_do_flash_zephyr() {
    fwbox_flash 0x100000 <build/zephyr/zephyr.bin
}
