# USB IMX219 Demo

Release:
[2024_09_28.bin](https://github.com/tinyvision-ai-inc/tinyclunx33_zephyr_example/releases/download/2024_09_28/tinyclunx33_zephyr_example_usb_uvc_imx219_2024_09_28.bin)

Works with:
[RTLv0.10.mipi](https://github.com/tinyvision-ai-inc/tinyclunx33_public/releases/tag/v0.10)

This uses the USB Video class (UVC) to display a a MIPI feed from the IMX219 in 1080p,
and an extra feed for demo to allow experimenting with control.

Crafting guide:

- Attach an [IMX219 module](https://tinyvision.ai/products/imx219-raspberry-pi-camera-v2) to an FPC cable

- Attach a
  [Syzygy dual](https://tinyvision.ai/products/syzygy-adapters) or
  [Syzygy in/out](https://tinyvision.ai/products/syzygy-mipi-to-usb-input-output-adapter)
  adapter to the other end.

- Plug the adapter to the adapter on the right side: `Expansion #2` for Rev1, `Pod A` for Rev2

- Load [`RTLv0.10.mipi`](https://github.com/tinyvision-ai-inc/tinyclunx33_public/releases/tag/v0.10).

After programming the device and power cycling the board, a video interface
would show-up, such as `/dev/video0` or `/dev/video2` on Linux.

```
# Use a web browser to open the camera, such as https://meet.jit.si/

# Or play it using https://gstreamer.freedesktop.org/
gst-launch-1.0 v4l2src device=/dev/video0 ! videoconvert ! autovideosink

# Or play it using https://mpv.io/
mpv /dev/video0

# Or play it using https://www.videolan.org/vlc/
vlc v4l2:///dev/video0

# Or play it using https://ffmpeg.org/ffplay.html
ffplay /dev/video0
```
