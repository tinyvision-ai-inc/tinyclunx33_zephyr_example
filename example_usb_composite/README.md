# USB Composite Example

This is a combination of the UVC and CDC Echo examples in a single combined device.

This requires adjustments in `prj.conf` and `app.overlay`,
and sometimes in `src/main.c` to allow both interfaces to co-exists,
but is otherwise relatively transparent.

See the READMEs for the other two exmamples to test this exapmle:

- [example_usb_uvc](../example_usb_uvc/)
- [example_usb_cdc_echo](../example_usb_cdc_echo/)
