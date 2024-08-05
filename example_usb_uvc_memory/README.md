# USB UVC from Memory Example

This uses the USB Video class (UVC) to display a solid color image pink or green,
generated with `memset()`

Every time a the shell command `video frame` is run, a new frame is produced,
with a different color.

After programming the device and power cycling the board, a video interface
would show-up, such as on `/dev/video0` on Linux.

See [example_uvc_manager](../example_uvc_manager) for a full list of commands.

```
# Play the camera feed using VLC
vlc v4l2:///dev/video0

# Record a single raw frame using V4L2
v4l2-ctl --device=/dev/video0 --stream-mmap --stream-count=1 --stream-to=video0.bin

# Record an MKV video without re-encoding the data and preserving the timing of every frame
ffmpeg -i /dev/video0 -c copy video0.mkv
```
