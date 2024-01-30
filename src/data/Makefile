video_frames.h: video_frames.mjpeg
	od -An -tx1 $< | sed -r 's/ ([0-9a-f]{2})/ 0x\1,/g' >$@
