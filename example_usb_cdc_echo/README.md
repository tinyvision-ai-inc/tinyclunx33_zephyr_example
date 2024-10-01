# USB CDC Echo Example

Release:
[2024_09_28.bin](https://github.com/tinyvision-ai-inc/tinyclunx33_zephyr_example/releases/download/2024_09_28/tinyclunx33_zephyr_example_usb_cdc_echo_2024_09_28.bin)

Works with:
[RTLv0.8](https://github.com/tinyvision-ai-inc/tinyclunx33_public/releases/tag/v0.8),
[RTLv0.9](https://github.com/tinyvision-ai-inc/tinyclunx33_public/releases/tag/v0.9),
[RTLv0.10](https://github.com/tinyvision-ai-inc/tinyclunx33_public/releases/tag/v0.10)

This uses the CDC ACM USB class, in a custom `usbd_cdc_raw.c` class to allow
to enqueue data without using a ring buffer.

After programming the device and power cycling the board, a serial interface
would show-up, such as on `/dev/ttyACM0` on Linux, and everything written to it
will be read back after conversion to upper case:

```
picocom /dev/ttyACM0
```
