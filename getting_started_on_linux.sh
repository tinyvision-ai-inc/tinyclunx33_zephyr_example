#!/bin/sh
set -eux
python -m venv ~/zephyrproject/.venv
cd ~/zephyrproject
. .venv/bin/activate
python -m pip install west ninja
west init -m https://github.com/tinyvision-ai-inc/tinyclunx33_zephyr_example
west update
west packages pip --install
west sdk install -t riscv64-zephyr-elf
git clone --depth 1 git@github.com:tinyvision-ai-inc/zephyr_private
west build --board tinyclunx33@rev2/rtl010 --shield tinyclunx33_devkit_rev2 tinyclunx33_zephyr_example/app_imx219
: Setup completed successfully
