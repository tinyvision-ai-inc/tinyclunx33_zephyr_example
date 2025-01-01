Custom RTL examle
#################

This is a starting point for building a custom RTL design specific to an application.
See `drivers/video/example.c <../drivers/video/example.c>`_ in this repo for the driver
associated with this example.


Building
========

Visit the
`tinyclunx33_sdk <https://github.com/tinyvision-ai-inc/zephyr_internal/tree/tinyclunx33_sdk>`_
documentation for a complete guide.

Example using the Rev2 SoM, Rev2 Devkit:

.. code-block:: console

   west build --board tinyclunx33@rev2/custom --shield tinyclunx33_devkit_rev2
   west flash


Running
=======

After programming the device and power cycling the board, a video interface
would show-up, such as ``/dev/video0`` or ``/dev/video2`` on Linux.

See the
`Zephyr UVC sample <https://github.com/tinyvision-ai-inc/zephyr/blob/pr-usb-uvc/samples/subsys/usb/uvc/README.rst#playing-the-stream>`_
for a more commplete guide.
