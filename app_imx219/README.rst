USB IMX219 Example
##################


Building
========

Visit the
`tinyclunx33_sdk <https://github.com/tinyvision-ai-inc/tinyvision_zephyr_sdk/>`_
documentation for a complete guide.

Example using the Rev2 SoM, Rev2 Devkit, FPGA release `rtl010`:

.. code-block:: console

   west build --board tinyclunx33@rev2/rtl010 --shield tinyclunx33_devkit_rev2
   west flash


Crafting
========

This uses the USB Video class (UVC) to display the image feed from an IMX219 camera over MIPI.

* Attach an `IMX219 module <https://tinyvision.ai/products/imx219-raspberry-pi-camera-v2>`_
  to an FPC cable

* Attach a
  `Syzygy dual <https://tinyvision.ai/products/syzygy-adapters>`_ or
  `Syzygy in/out <https://tinyvision.ai/products/syzygy-mipi-to-usb-input-output-adapter>`_
  adapter to the other end.

* Plug the adapter to the adapter on the right side: ``Expansion #2`` for Rev1, ``Pod A`` for Rev2


Running
=======

After programming the device and power cycling the board, a video interface
would show-up, such as `/dev/video0` or `/dev/video2` on Linux.

See the
`Zephyr UVC sample <https://github.com/tinyvision-ai-inc/zephyr/blob/pr-usb-uvc/samples/subsys/usb/uvc/README.rst#playing-the-stream>`_
for a more commplete guide on how to build.
