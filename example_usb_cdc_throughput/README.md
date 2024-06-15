This firmware can be used to measure throughput of the USB3 interface.

It enumerates as a USB CDC ACM interface: a serial port.

Shortly after connecting to this serial console, it continuously dumps test data to allow the host to measure the end-to-end throughput.

To test the demo:

```
# Plug the device to USB3 then run this
picocom /dev/ttyACM0 && dd if=/dev/ttyACM0 of=/dev/null bs=1M status=progress

# Immediately quit with Ctrl-A then Ctrl-Q before the transfer starts
```

The statistics will be reported every second by `dd`, with the speed in kByte/s, to multiply by 8 to get kbit/s.
