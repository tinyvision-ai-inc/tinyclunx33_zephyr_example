# tinyCLUNX33

Sample project showing how run Zephyr on a VexRiscv CPU running on the
tinyCLUNX33.

```shell
# Setup a venv into which install dependencies
python3 -m venv ~/zephyrproject/.venv
. ~/zephyrproject/.venv/bin/activate
python3 -m pip install west

# Clone the example repositories
cd ~/zephyrproject
west init -m https://github.com/tinyvision-ai-inc/tinyclunx33_zephyr_example
west update

# Visit the example directory you wish to run
cd tinyclunx33_zephyr_example/example_usb_cdc_echo/
cd tinyclunx33_zephyr_example/example_usb_cdc_shell/
cd tinyclunx33_zephyr_example/example_usb_cdc_throughput/
cd tinyclunx33_zephyr_example/example_usb_uvc/

# To build for the tinyVision.ai tinyCLUNX33 board:
west build -b tinyclunx33

# Then Zephyr to program the board
west flash

# Program the FPGA image, only needed the first time
latest=v0.7/tinyclunx33_rtl_reference_design_v0.7.bit
wget https://github.com/tinyvision-ai-inc/tinyclunx33_public/releases/download/$latest
sudo ecpprog tinyclunx33_rtl_reference_design_v0.7.bit

# Power-cycle the board

# Connect to the FTDI serial interface
picocom -b 153600 /dev/ttyUSB1

# In a new terminal, watch the Linux kernel messages
sudo dmesg -w

# Connect the DATA USB interface and observe the effect in dmesg
```

There is a README present in each example directory for usage instructions of each example.
