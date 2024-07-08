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
FWBOX="$FWBOX picocom,port=/dev/ttyACM1" fwbox_run helloworld | grep HELLOWORLD
