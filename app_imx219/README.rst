USB IMX219 Example
##################

**Note: this only works with the latest versions (``1.x.x``, not ``0.xx``). Make sure you use the
correct RTL release when testing**


Building
========

Visit the
`tinyclunx33_sdk <https://github.com/tinyvision-ai-inc/tinyvision_zephyr_sdk/>`_
documentation for a complete guide.

Example using the Rev2 SoM and Devkit, FPGA release `rtl_1_0`:

.. code-block:: console

   west build --board tinyclunx33@rev2/rtl_1_0
   west flash


Crafting
========

This uses the USB Video class (UVC) to display the image feed from two IMX219 cameras over MIPI.

* Attach a first `IMX219 module <https://tinyvision.ai/products/imx219-raspberry-pi-camera-v2>`_
  to an FPC cable 1.

* Attach a first `IMX219 module <https://tinyvision.ai/products/imx219-raspberry-pi-camera-v2>`_
  to an FPC cable 2.

* Attach a `Syzygy dual <https://tinyvision.ai/products/syzygy-adapters>`_
  adapter to the other end of the cable 1.

* Attach the same `Syzygy dual <https://tinyvision.ai/products/syzygy-adapters>`_
  adapter to the other end of the cable 2.

* Plug the adapter to the devkit, on the right side: ``Pod A`` (for Rev2)


Running
=======

After programming the device and power cycling the board, a video interface
would show-up, such as `/dev/video0` or `/dev/video2` on Linux.

See the
`Zephyr UVC sample <https://github.com/tinyvision-ai-inc/zephyr/blob/pr-usb-uvc/samples/subsys/usb/uvc/README.rst#playing-the-stream>`_
for a more commplete guide on how to build.
