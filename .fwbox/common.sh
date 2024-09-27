# Common configuration

# Default values that can be overriden
: ${FWBOX_BAUD:=192000} # 100 MHz
: ${FWBOX_BAUD:=156200} # 80 MHz
: ${FWBOX_DEV_VIDEO:=/dev/video2}
: ${FWBOX_DEV_TTYACM:=/dev/ttyACM0}
: ${FWBOX_DEV_TTYUSB:=/dev/ttyUSB1}

# Values derived from above
FWBOX_CONSOLE="${FWBOX:?} console,port=$FWBOX_DEV_TTYUSB,baud=$FWBOX_BAUD"
FWBOX_PICOCOM="${FWBOX:?} picocom,port=$FWBOX_DEV_TTYUSB,baud=$FWBOX_BAUD"
FWBOX_USB_CONSOLE="${FWBOX:?} console,port=$FWBOX_DEV_TTYACM"
FWBOX_USB_REPL="${FWBOX:?} picocom,port=$FWBOX_DEV_TTYACM repl"
FWBOX_TMP_VIDEO=/tmp/${FWBOX_DEV_VIDEO##*/}

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
)

fwbox_do_video_capture() ( local frames=${1:-50}
    tmp=$(mktemp)
    set -eu 
    echo "fwbox: capturing to localhost:$FWBOX_TMP_VIDEO.mkv" >&2
    #echo 0xffffffff | fwbox_run dd of=/sys/module/uvcvideo/parameters/trace
    fwbox_run ffmpeg -y -i "$FWBOX_DEV_VIDEO" -c copy -frames "$frames" "$FWBOX_TMP_VIDEO.mkv"
    fwbox_run cat "$FWBOX_TMP_VIDEO.mkv" >$tmp
    mv "$tmp" "$FWBOX_TMP_VIDEO.mkv"
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
