# USB Shell Example

Tested with:
[rtl009](https://github.com/tinyvision-ai-inc/tinyclunx33_public/releases/tag/v0.9),
[rtl010](https://github.com/tinyvision-ai-inc/tinyclunx33_public/releases/tag/v0.10)


## Building guide

Visit the
[`tinyvision_zephyr_sdk`](https://github.com/tinyvision-ai-inc/tinyvision_zephyr_sdk)
documentation for a complete guide.

Example using the Rev2 SoM, Rev2 Devkit, FPGA releasea `rtl010`:

```
west build --board tinyclunx33@rev2/rtl010 --shield tinyclunx33_devkit_rev2
west flash
```


## Running guide

After programming the device and power cycling the board, a serial interface
would show-up, such as `/dev/ttyACM0` or `/dev/ttyACM1` on Linux or `COM0`, `COM1`... on Windows.

You can then connect to it using a serial console viewer such as `picocom` or `puTTY`.
The baudrate is ignored as this is a virtual serial port over USB.

```
picocom /dev/ttyACM0
```
