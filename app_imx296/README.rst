THE IMX296 DRIVER IS NOT FUNCTIONAL YET

USB IMX296 Example
##################

**Note: this only works with the latest versions (``1.x.x``, not ``0.xx``). Make sure you use the
correct RTL release when testing**


Building
========

Visit the
`tinyclunx33_sdk <https://github.com/tinyvision-ai-inc/tinyvision_zephyr_sdk/>`_
documentation for a complete guide.

Example using the Rev2 SoM, Rev2 Devkit, FPGA release `rtl_1_0`:

.. code-block:: console

   west build --board tinyclunx33@rev2/rtl_1_0 --shield tinyclunx33_devkit_rev2
   west flash


Crafting
========

This uses the USB Video class (UVC) to display the image feed from two IMX296 cameras over MIPI.

* Attach an IMX296 module to one end of an FPC cable.

* Attach a `Syzygy dual <https://tinyvision.ai/products/syzygy-adapters>`_
  adapter to the other end of the cable.

* Plug the adapter to the devkit, on the right side: ``Pod A`` (for Rev2)


Running
=======

After programming the device and power cycling the board, a video interface
would show-up, such as `/dev/video0` or `/dev/video2` on Linux.

See the
`Zephyr UVC sample <https://github.com/tinyvision-ai-inc/zephyr/blob/pr-usb-uvc/samples/subsys/usb/uvc/README.rst#playing-the-stream>`_
for a more commplete guide on how to build.
