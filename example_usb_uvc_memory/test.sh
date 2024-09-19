# sudo git clone https://github.com/firmware-box/fwbox /opt/fwbox
. /opt/fwbox/fwbox.sh

# Exit on first failure
set -eu

# Build and upload the firmware
fwbox_use
fwbox_do_all

# Give it time to enumerate
sleep 10

# Probe the console
fwbox_do_video_capture 10 & pid=$!

# Trigger video frames
for x in $(seq 10); do
    FWBOX=$FWBOX_USB_REPL fwbox_run "video frame"
done

# Give some time for the video capture to complete
sleep 1
clear

# Check that the video duration is correct
test "$(fwbox_frame_count "$FWBOX_TMP_VIDEO.mkv")" = 10
