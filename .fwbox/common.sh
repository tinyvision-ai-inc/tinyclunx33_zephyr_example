# Configuration common to all boxes

#BAUD=192000 # 100 MHz
BAUD=156200 # 80 MHz

FWBOX_GPIOSET="${FWBOX:?} picocom,port=/dev/ttyACM0 repl"
FWBOX_CONSOLE="${FWBOX:?} console,port=/dev/ttyUSB1,baud=$BAUD"
FWBOX_PICOCOM="${FWBOX:?} picocom,port=/dev/ttyUSB1,baud=$BAUD"
FWBOX_USB_CONSOLE="${FWBOX:?} console,port=/dev/ttyACM1"
FWBOX_USB_REPL="${FWBOX:?} picocom,port=/dev/ttyACM1 repl"

fwbox_do_flash_zephyr() {
    fwbox_ecpprog 0x100000 <build/zephyr/zephyr.bin
}

fwbox_do_flash_rtl() { local v=${1:-v0.8}
    local url=https://github.com/tinyvision-ai-inc/tinyclunx33_public/releases/download/$v/tinyclunx33_rtl_reference_design_$v.bit
    echo "$url"
    curl -Ls "$url" | fwbox_ecpprog 0x000000
}

fwbox_do_all() (
    set -eu
    west build -b tinyclunx33
    fwbox_do_flash_zephyr
    fwbox_do_power_cycle
    fwbox_do_reset
)

fwbox_do_video_capture() ( local frames=${1:-50}
    set -eu 
    echo "fwbox: capturing to localhost:/tmp/video0.mkv" >&2
    echo 0xffffffff | fwbox_run dd of=/sys/module/uvcvideo/parameters/trace
    fwbox_run ffmpeg -y -i "/dev/$FWBOX_VIDEO" -c copy -frames "$frames" "/tmp/$FWBOX_VIDEO.mkv"
    fwbox_run cat "/tmp/$FWBOX_VIDEO.mkv" >/tmp/$FWBOX_VIDEO.mkv
)

fwbox_do_usb_console() {
    FWBOX_CONSOLE="$FWBOX_USB_CONSOLE" fwbox_do_console
}

fwbox_do_lsusb() {
    fwbox_run lsusb -v -d 1209:0001
}

fwbox_do_usb_capture() {
    fwbox_run modprobe usb
    fwbox_run tcpdump -i usbmon0 -AX
}

fwbox_frame_count() { local file=$1
    ffprobe -v error -select_streams v:0 -count_frames -show_entries stream=nb_read_frames -of default=nokey=1:noprint_wrappers=1 "$file"
}
