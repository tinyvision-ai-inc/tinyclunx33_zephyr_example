USB IMX219 Example
##################

To build this sample:

.. code-block:: console

   . ./env.sh
   west build
   west flash

Then power cycle by disconnecting and reconnecting every cable.

Then, a webcam device should become visible,
that supports all the resolutions that both the tinyCLUNX33 and the IMX219 support.

For instance on Linux, to play the stream at 1918x1078 resolution, run ``qcam -s width=1918``
then select a camera in the list whose name ends-up with ``1209:1``.

See `Zephyr UVC documentation <https://builds.zephyrproject.io/zephyr/pr/76798/docs/samples/subsys/usb/uvc/README.html>`_
for a more complete introduction to testing UVC devices.
