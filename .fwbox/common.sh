# Configuration common to all boxes

FWBOX_GPIO="${FWBOX:?} picocom,port=/dev/ttyACM0 repl"
FWBOX_LOGS="${FWBOX:?} console,port=/dev/ttyUSB1,baud=153600"

fwbox_do_flash_zephyr() {
    fwbox_flash_ecpprog 0x100000 <build/zephyr/zephyr.bin
}

fwbox_do_all() {
    fwbox_do_flash_zephyr
    fwbox_do_power_cycle
    fwbox_do_reset
}

fwbox_do_video_capture() {
    echo 0xffffffff | fwbox_run dd of=/sys/module/uvcvideo/parameters/trace
    fwbox_run ffmpeg -y -t 3 -i /dev/video0 -f mp4 video0.mp4
    fwbox_run cat video0.mp4 >video0.mp4
}
