# USB IMX219 Example

Release:
[2024_09_28.bin](https://github.com/tinyvision-ai-inc/tinyclunx33_zephyr_example/releases/download/2024_09_28/tinyclunx33_zephyr_example_usb_uvc_imx219_2024_09_28.bin)

Works with:
[rtl009](https://github.com/tinyvision-ai-inc/tinyclunx33_public/releases/tag/v0.9),
[rtl010](https://github.com/tinyvision-ai-inc/tinyclunx33_public/releases/tag/v0.10)


## Building guide

Visit the
[`tinyclunx33_sdk`](https://github.com/tinyvision-ai-inc/zephyr_internal/tree/tinyclunx33_sdk)
documentation for a complete guide.

Example using the Rev2 SoM, Rev2 Devkit, FPGA releasea `rtl010`:

```
west build --board tinyclunx33@rev2/rtl010 --shield tinyclunx33_devkit_rev2
west flash
```


## Crafting guide

This uses the USB Video class (UVC) to display the image feed from an IMX219 camera over MIPI.

- Attach an [IMX219 module](https://tinyvision.ai/products/imx219-raspberry-pi-camera-v2) to an FPC cable

- Attach a
  [Syzygy dual](https://tinyvision.ai/products/syzygy-adapters) or
  [Syzygy in/out](https://tinyvision.ai/products/syzygy-mipi-to-usb-input-output-adapter)
  adapter to the other end.

- Plug the adapter to the adapter on the right side: `Expansion #2` for Rev1, `Pod A` for Rev2

- Load the [`RTLv0.9.mipi`](https://github.com/tinyvision-ai-inc/tinyclunx33_public/releases/tag/v0.9) which is built exclusively for this example.


## Running guide

After programming the device and power cycling the board, a video interface
would show-up, such as `/dev/video0` or `/dev/video2` on Linux.

See the
[UVC](https://github.com/tinyvision-ai-inc/zephyr/blob/pr-usb-uvc/samples/subsys/usb/uvc/README.rst#playing-the-stream)
Zephyr sample for a more commplete guide.
