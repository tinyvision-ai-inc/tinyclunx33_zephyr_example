# USB UVC Example

This uses the USB Video class (UVC), with the feed parameters for now hardcoded
into the `usbd_uvc.c` class implementation.

After programming the device and power cycling the board, a video interface
would show-up, such as on `/dev/video2` on Linux, and can be opened with tools
such as
[VLC](https://www.videolan.org/vlc/),
[Gstreamer](https://gstreamer.freedesktop.org/),
[ffplay](https://ffmpeg.org/ffplay.html),
[guvcview](https://guvcview.sourceforge.net/),
or libraries such as [OpenCV](https://opencv.org/),
or eventually [libuvc](https://github.com/libuvc/libuvc) for low-level access.

```
gst-launch-1.0 v4l2src device=/dev/video2 ! videoconvert ! autovideosink
mpv /dev/video2
ffplay /dev/video2
vlc v4l2:///dev/video2
```


## Using a different data source

Not every address is accessible from everything: the USB23 core own DMA engine
is only able to access what is rooted to it, while the CPU has access to
everything.

Here are some notable examples of things that are not accessible from the USB23
core, which performs DMA requests to transfer the USB data in and out:

- RISC-V main RAM: stack or heap buffers (variable, arrays, malloc() results...)
- Flash: Variables and buffers marked as `const`

On the other hand, it has a direct access to:

- All cores attached to AXI64, such as high-throughput MIPI data pipes.
- A small "scratch" RAM block is used as temporary buffer.

This temporary buffer can be used to exchange data with the CPU.
A linker script parameter is set so that each variable with a name starting by
`usb23_dma_` gets allocated onto that exchange region, which can be used from
the driver or application alike.
