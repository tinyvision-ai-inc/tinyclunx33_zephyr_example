# sudo git clone https://github.com/firmware-box/fwbox /opt/fwbox
. /opt/fwbox/fwbox.sh

# Exit on first failure
set -eu

# Build and upload the firmware
fwbox_use
fwbox_do_all

# Give it time to enumerate
sleep 5

# Probe the console
FWBOX="$FWBOX_USB_REPL" fwbox_run helloworld | grep HELLOWORLD
