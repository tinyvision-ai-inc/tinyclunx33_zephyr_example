# sudo git clone https://github.com/firmware-box/fwbox /opt/fwbox
. /opt/fwbox/fwbox.sh

# Exit on first failure
set -eu

# Select which board to use for testing
fwbox_use idefix

# Build and upload the firmware
fwbox_do_all

# Give it time to enumerate
sleep 5

# Probe the console
(sleep 0.4; printf '\x01\x11'; sleep 1) | fwbox_run picocom --quiet /dev/ttyACM1
fwbox_run timeout 10 dd if=/dev/ttyACM1 of=/dev/null bs=1M count=10 iflag=fullblock status=progress
