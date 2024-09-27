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
fwbox_do_video_capture 3

# Check that the video duration is correct
test "$(fwbox_frame_count "$FWBOX_TMP_VIDEO.mkv")" = 3
