USB IMX219 Example
##################


Building
========

Visit the
`tinyclunx33_sdk <https://github.com/tinyvision-ai-inc/zephyr_internal/tree/tinyclunx33_sdk>`_
documentation for a complete guide.

Example using the Rev2 SoM, Rev2 Devkit, FPGA releasea ``rtl008``:

.. code-block:: console

   west build --board tinyclunx33@rev2/rtl008 --shield tinyclunx33_devkit_rev2
   west flash


Crafting
========

This uses the USB Video class (UVC) to display the image feed from an
internal test pattern and there is no external hardware needed besides
the SoM and the Devkit or equivalent board setup.


Running
=======

After programming the device and power cycling the board, a video interface
would show-up, such as ``/dev/video0`` or ``/dev/video2`` on Linux.

See the
`Zephyr UVC sample <https://github.com/tinyvision-ai-inc/zephyr/blob/pr-usb-uvc/samples/subsys/usb/uvc/README.rst#playing-the-stream>`_
for a more commplete guide.
