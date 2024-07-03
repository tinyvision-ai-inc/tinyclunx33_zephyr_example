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
fwbox_run picocom --exit-after 200 /dev/ttyACM1

# Start performing a transfer
fwbox_run timeout 10 dd if=/dev/ttyACM1 of=/dev/null bs=$((0x00100000)) count=10G status=progress
