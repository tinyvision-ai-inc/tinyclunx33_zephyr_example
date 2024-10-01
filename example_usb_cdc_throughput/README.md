# USB CDC Throughput example

Release:
[2024_09_28.bin](https://github.com/tinyvision-ai-inc/tinyclunx33_zephyr_example/releases/download/2024_09_28/tinyclunx33_zephyr_example_usb_cdc_throughput_2024_09_28.bin)

Works with:
[RTLv0.8](https://github.com/tinyvision-ai-inc/tinyclunx33_public/releases/tag/v0.8),
[RTLv0.9](https://github.com/tinyvision-ai-inc/tinyclunx33_public/releases/tag/v0.9),
[RTLv0.10](https://github.com/tinyvision-ai-inc/tinyclunx33_public/releases/tag/v0.10)

This firmware can be used to measure the software performance of BULK USB CDC transfers.

It is expected to be **very low** as does not make use of the hardware acceleration provided with the tinyCLUNX33!
If the goal is to reach high-bandwidth transfer, you would rather be interested in
[example_usb_uvc_manager](../example_usb_uvc_manager/)
to reach above 3 Gbit/s of bandwidth at application layer.

It enumerates as a USB CDC ACM interface: a serial port.

Shortly after connecting to this serial console, it continuously dumps test data to allow the host to measure the end-to-end throughput.

To test the demo:

```
# Plug the device to USB3 then run this
picocom /dev/ttyACM0 && dd if=/dev/ttyACM0 of=/dev/null bs=1M status=progress

# Immediately quit with Ctrl-A then Ctrl-Q before the transfer starts
```

The statistics will be reported every second by `dd`, with the speed in kByte/s, to multiply by 8 to get kbit/s.
