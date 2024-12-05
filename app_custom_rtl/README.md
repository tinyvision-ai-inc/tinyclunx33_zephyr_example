# Custom RTL examle

This is a starting point for building a custom RTL design specific to an application.
See [`drivers/video/example.c`](../drivers/video/example.c) for the driver assotiated with this example.


## Building guide

Visit the
[`tinyclunx33_sdk`](https://github.com/tinyvision-ai-inc/zephyr_internal/tree/tinyclunx33_sdk)
documentation for a complete guide.

Example using the Rev2 SoM, Rev2 Devkit:

```
west build --board tinyclunx33@rev2/custom --shield tinyclunx33_devkit_rev2
west flash
```

## Running guide

After programming the device and power cycling the board, a video interface
would show-up, such as `/dev/video0` or `/dev/video2` on Linux.

See the
[UVC](https://github.com/tinyvision-ai-inc/zephyr/blob/pr-usb-uvc/samples/subsys/usb/uvc/README.rst#playing-the-stream)
Zephyr sample for a more commplete guide.
