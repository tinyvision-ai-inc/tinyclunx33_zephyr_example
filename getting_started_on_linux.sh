#!/bin/sh

# Stop on first error
set -eux

# Initialize a new python project
python -m venv ~/zephyrproject/.venv
cd ~/zephyrproject
. .venv/bin/activate

# Install the initial dependencies
python -m pip install west ninja

# Clone this repo (no need to clone it yourself)
west init -m https://github.com/tinyvision-ai-inc/tinyclunx33_zephyr_example

# Clone the dependencies
west update

# Install the software dependencies
west packages pip --install
west sdk install -t riscv64-zephyr-elf

# Extra licensed firmware for the IMX219 example
git clone --depth 1 git@github.com:tinyvision-ai-inc/zephyr_private

# Build the firmware for RTL v1.0
west build --pristine --board tinyclunx33@rev2/rtl_1_0 tinyclunx33_zephyr_example/app_imx219_dual
mv build/zephyr/zephyr.bin tinyclunx33_zephyr_example_app_imx219_dual_rtl_1_0_on_$(date +%Y_%m_%d).bin

# Build the firmware for RTL v1.1
west build --pristine --board tinyclunx33@rev2/rtl_1_1 tinyclunx33_zephyr_example/app_imx219_dual
mv build/zephyr/zephyr.bin tinyclunx33_zephyr_example_app_imx219_dual_rtl_1_1_on_$(date +%Y_%m_%d).bin
