# tinyCLUNX33

Sample project showing how run Zephyr on a VexRiscv CPU running on the
tinyCLUNX33.

```
# Prepare a workspace
mkdir "$HOME/zephyrproject"
cd "$HOME/zephyrproject"

# Initialize the repos in that workspace
west init -m https://github.com/tinyvision-ai-inc/tinyclunx33_zephyr_example
west update

# Program the FPGA image, only needed the first time
wget https://github.com/tinyvision-ai-inc/tinyclunx33_public/releases/download/v0.5/tinyclunx33_rtl_reference_design_v0.5.bit
sudo ecpprog tinyclunx33_rtl_reference_design_v0.5.bit

# Visit the example directory you wish to run
cd tinyclunx33_zephyr_example/example_usb_uvc
cd tinyclunx33_zephyr_example/example_usb_cdc_shell
cd tinyclunx33_zephyr_example/example_usb_cdc_echo
cd tinyclunx33_zephyr_example/example_usb_cdc_throughput

# To build for the tinyVision.ai tinyCLUNX33 board:
west build -p -b tinyclunx33

# Then Zephyr to program the board
west flash

# Connect to the FTDI serial interface
picocom -b 153600 /dev/ttyUSB1

# In a new terminal, watch the Linux kernel messages
sudo dmesg -w

# Then connect the DATA USB cable interface and observe the effect in dmesg
```

There is a README present in each example directory for example-specific instructions.
