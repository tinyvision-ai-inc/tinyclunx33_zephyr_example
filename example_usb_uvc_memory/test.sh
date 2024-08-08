# sudo git clone https://github.com/firmware-box/fwbox /opt/fwbox
. /opt/fwbox/fwbox.sh

# Exit on first failure
set -eu

# Select which board to use for testing
fwbox_use idefix

# Build and upload the firmware
fwbox_do_all

# Give it time to enumerate
sleep 10

# Probe the console
fwbox_do_video_capture 15 & pid=$!

# Trigger 15 video frames
FWBOX=$FWBOX_USB_REPL fwbox_run "video frame" "video frame" "video frame"
FWBOX=$FWBOX_USB_REPL fwbox_run "video frame" "video frame" "video frame"
FWBOX=$FWBOX_USB_REPL fwbox_run "video frame" "video frame" "video frame"
FWBOX=$FWBOX_USB_REPL fwbox_run "video frame" "video frame" "video frame"
FWBOX=$FWBOX_USB_REPL fwbox_run "video frame" "video frame" "video frame"

# Give some time for the video capture to complete
clear
sleep 1

# Check that the video duration is correct
test "$(fwbox_frame_count /tmp/video0.mkv)" = 15
