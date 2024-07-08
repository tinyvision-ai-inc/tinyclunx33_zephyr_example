if nc -vz 172.22.0.2 22; then
    FWBOX="ssh,host=172.22.0.2,port=22"
elif nc -vz 192.168.15.100 22; then
    FWBOX="ssh,host=192.168.15.100,port=22"
else
    echo "could not reach any host to connect to" >&2
    exit 1
fi

FWBOX_GPIO_RESET="x 22"
FWBOX_GPIO_POWER="x 20"

fwbox_gpioset() {
    fwbox_gpioset_micropython "$@"
}

. $FWBOX_PATH/.fwbox/common.sh
