# Configuration common to all boxes

: ${BAUD:=192000}

rtl=v0.7
rtl=https://github.com/tinyvision-ai-inc/tinyclunx33_public/releases/download/$rtl/tinyclunx33_rtl_reference_design_$rtl.bit

FWBOX_GPIOSET="${FWBOX:?} picocom,port=/dev/ttyACM0 repl"
FWBOX_CONSOLE="${FWBOX:?} console,port=/dev/ttyUSB1,baud=$BAUD"
FWBOX_PICOCOM="${FWBOX:?} picocom,port=/dev/ttyUSB1,baud=$BAUD"
FWBOX_USB3CDC="${FWBOX:?} console,port=/dev/ttyACM1"

fwbox_do_flash_zephyr() {
    fwbox_ecpprog 0x100000 <build/zephyr/zephyr.bin
}

fwbox_do_flash_rtl() {
    curl -Ls "$rtl" | fwbox_ecpprog 0x000000
}

fwbox_do_all() (
    set -eu
    west build -b tinyclunx33
    fwbox_do_flash_zephyr
    fwbox_do_power_cycle
    fwbox_do_reset
)

fwbox_do_video_capture() (
    set -eu 
    echo "fwbox: capturing to localhost:/tmp/video0.mp4" >&2
    echo 0xffffffff | fwbox_run dd of=/sys/module/uvcvideo/parameters/trace
    fwbox_run ffmpeg -y -t 5 -i /dev/video0 /tmp/video0.mp4
    fwbox_run cat /tmp/video0.mp4 >/tmp/video0.mp4
)

fwbox_do_usb3cdc() {
    FWBOX_CONSOLE="$FWBOX_USB3CDC" fwbox_do_console
}

fwbox_do_lsusb() {
    fwbox_run lsusb -v -d 1209:0001
}

fwbox_do_usb_capture() {
    fwbox_run modprobe usb
    fwbox_run tcpdump -i usbmon0 -AX
}
