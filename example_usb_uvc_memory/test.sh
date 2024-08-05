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
fwbox_do_video_capture & pid=$!

# Send 104 video frames
for i in $(seq 104); do
	FWBOX=$FWBOX_USB_CONSOLE fwbox_run "video frame"
done

# Check that the video duration is correct
ffprobe /tmp/video0.mp4
