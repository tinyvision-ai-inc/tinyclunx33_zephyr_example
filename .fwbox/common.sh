# Configuration common to all boxes

rtl=v0.7
rtl=https://github.com/tinyvision-ai-inc/tinyclunx33_public/releases/download/$rtl/tinyclunx33_rtl_reference_design_$rtl.bit

FWBOX_GPIO="${FWBOX:?} picocom,port=/dev/ttyACM0 repl"
FWBOX_LOGS="${FWBOX:?} console,port=/dev/ttyUSB1,baud=153600"

fwbox_do_flash_zephyr() {
    fwbox_flash_ecpprog 0x100000 <build/zephyr/zephyr.bin
}

fwbox_do_flash_rtl() {
    curl -Ls "$rtl" | fwbox_flash_ecpprog 0x000000
}

fwbox_do_all() (
    set -eu
    west build
    fwbox_do_flash_zephyr
    fwbox_do_power_cycle
    fwbox_do_reset
)

fwbox_do_video_capture() (
    set -eu 
    echo "fwbox: capturing to localhost:/tmp/video0.mp4" >&2
    echo 0xffffffff | fwbox_run dd of=/sys/module/uvcvideo/parameters/trace
    fwbox_run ffmpeg -y -t 3 -i /dev/video0 /tmp/video0.mp4
    fwbox_run cat /tmp/video0.mp4 >/tmp/video0.mp4
)
