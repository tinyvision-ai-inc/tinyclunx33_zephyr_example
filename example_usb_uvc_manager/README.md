# USB UVC Example

This uses the USB Video class (UVC) to display a test pattern.

After programming the device and power cycling the board, a video interface
would show-up, such as on `/dev/video0` on Linux.

```
# Use a web browser to open the camera

# Or play it using https://gstreamer.freedesktop.org/
gst-launch-1.0 v4l2src device=/dev/video0 ! videoconvert ! autovideosink

# Or play it using https://mpv.io/
mpv /dev/video0

# Or play it using https://www.videolan.org/vlc/
vlc v4l2:///dev/video0

# Or play it using https://ffmpeg.org/ffplay.html
ffplay /dev/video0

# Or record it using https://ffmpeg.org/
ffmpeg -i /dev/video0 tinyclunx33_capture.mp4

# Or record it using https://guvcview.sourceforge.net/
guvcview --device=/dev/video0

# Or record the raw data using V4L2 utilities
v4l2-ctl --device /dev/video0 --stream-mmap --stream-count=500 --stream-to=tinyclunx33_capture.bin

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
