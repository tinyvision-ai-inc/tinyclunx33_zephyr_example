# sudo git clone https://github.com/firmware-box/fwbox /opt/fwbox
. /opt/fwbox/fwbox.sh

# Exit on first failure
set -eu

# Build and upload the firmware
fwbox_use
fwbox_do_all

# Give it time to enumerate
sleep 10

# Run the test utility
read num unit <<EOF
$(fwbox_run python - "$FWBOX_DEV_TTYACM" <test.py)
EOF

echo "$num $unit"
test "$num" -gt 500
test "$unit" = kbit/s
