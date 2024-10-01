# USB IMX219 Example

Release:
[2024_09_28.bin](https://github.com/tinyvision-ai-inc/tinyclunx33_zephyr_example/releases/download/2024_09_28/tinyclunx33_zephyr_example_usb_uvc_imx219_2024_09_28.bin)

Works with:
[RTLv0.9](https://github.com/tinyvision-ai-inc/tinyclunx33_public/releases/tag/v0.9),
[RTLv0.10](https://github.com/tinyvision-ai-inc/tinyclunx33_public/releases/tag/v0.10)

This uses the USB Video class (UVC) to display a test pattern.

Crafting guide:

- Attach an [IMX219 module](https://tinyvision.ai/products/imx219-raspberry-pi-camera-v2) to an FPC cable

- Attach a
  [Syzygy dual](https://tinyvision.ai/products/syzygy-adapters) or
  [Syzygy in/out](https://tinyvision.ai/products/syzygy-mipi-to-usb-input-output-adapter)
  adapter to the other end.

- Plug the adapter to the adapter on the right side: `Expansion #2` for Rev1, `Pod A` for Rev2

- Load the [`RTLv0.9.mipi`](https://github.com/tinyvision-ai-inc/tinyclunx33_public/releases/tag/v0.9) which is built exclusively for this example.

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

# Or play it using https://opencv.org/
import cv2
cv2.namedWindow("preview")
vc = cv2.VideoCapture(0)
while (val := vc.read())[0]:
    cv2.waitKey(20)
    cv2.imshow("preview", val[1])
cv2.destroyWindow("preview")
vc.release()
```
