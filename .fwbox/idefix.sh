if nc -vz 172.22.0.3 22; then
    FWBOX="ssh,host=172.22.0.3,port=22"
elif nc -vz 192.168.1.122 22; then
    FWBOX="ssh,host=192.168.1.122,port=22"
else
    echo "could not reach any host to connect to" >&2
    exit 1
fi

FWBOX_GPIO_RESET="gpio@48000000 0"
FWBOX_GPIO_POWER="gpio@48000000 1"

fwbox_gpioset() {
    fwbox_gpioset_zephyr "$@"
}

. $FWBOX_PATH/.fwbox/common.sh
