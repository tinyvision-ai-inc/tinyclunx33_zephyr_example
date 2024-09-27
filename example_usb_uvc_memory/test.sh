# sudo git clone https://github.com/firmware-box/fwbox /opt/fwbox
. /opt/fwbox/fwbox.sh

# Exit on first failure
set -eu

# Build and upload the firmware
fwbox_use
fwbox_do_all

# Give it time to enumerate
sleep 3

# Record 10 video frames in the background
fwbox_do_video_capture 10 & pid=$!

# Give some time for the video capture to start
sleep 1

# Trigger the prompt
FWBOX=$FWBOX_USB_REPL fwbox_run ""

# Trigger video frames
for x in $(seq 15); do
    FWBOX=$FWBOX_USB_REPL fwbox_run "video frame"
done

# Give some time for the video capture to complete
sleep 2

# Check that the number of frames is correct
test "$(fwbox_frame_count "$FWBOX_TMP_VIDEO.mkv")" = 10
