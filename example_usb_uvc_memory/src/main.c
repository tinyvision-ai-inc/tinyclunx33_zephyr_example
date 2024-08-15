#include <stdlib.h>

#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/video.h>
#include <zephyr/shell/shell.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(app, LOG_LEVEL_DBG);

const struct device *const uvc0_dev = DEVICE_DT_GET(DT_NODELABEL(uvc0));

int main(void)
{
	int ret = 0;

	ret = video_stream_start(uvc0_dev);
	if (ret) {
		LOG_DBG("could not");
		return ret;
	}

	k_sleep(K_FOREVER);
	return 0;
}

/*
 * Send a single video frame whenever hitting the "video frame" command on the shell.
 * This permits to open a video feed with a webcam viewer tool such as VLC, and manually
 * step through every frame.
 *
 * If wanting a continuous video feed instead, repeat this function in loop, eventually
 * only the enqueue()/dequeue() part for efficiency, and possibly with several buffers
 * as done in the video Zephyr samples.
 */
static int cmd_video_frame(const struct shell *sh, size_t argc, char **argv)
{
	struct video_buffer *vbuf;
	struct video_format fmt = {0};
	static uint8_t byte = 0x00;
	int ret;

	ret = video_get_format(uvc0_dev, VIDEO_EP_IN, &fmt);
	if (ret != 0) {
		shell_error(sh, "could not allocate the video buffer");
		return ret;
	}

	shell_print(sh, "Preparing a %ux%u frame, %u bytes",
		    fmt.pitch, fmt.height, fmt.pitch * fmt.height);

	/* If this gets stuck, you might need to increase CONFIG_VIDEO_BUFFER_POOL_SZ_MAX */
	vbuf = video_buffer_alloc(fmt.pitch * fmt.height);
	if (vbuf == NULL) {
		shell_error(sh, "could not allocate the video buffer");
		return -ENOMEM;
	}

	memset(vbuf->buffer, byte, vbuf->size);
	vbuf->bytesused = vbuf->size;
	vbuf->flags = VIDEO_BUF_EOF;

	ret = video_enqueue(uvc0_dev, VIDEO_EP_IN, vbuf);
	if (ret != 0) {
		shell_error(sh, "could not enqueue video buffer");
		goto end;
	}

	ret = video_dequeue(uvc0_dev, VIDEO_EP_IN, &vbuf, K_FOREVER);
	if (ret != 0) {
		shell_error(sh, "could not dequeue video buffer");
		goto end;
	}

	byte ^= 0xff;
end:
	video_buffer_release(vbuf);
	return ret;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_video,
                               SHELL_CMD(frame, NULL, "submit a frame of specified color",
                                         &cmd_video_frame),
                               SHELL_SUBCMD_SET_END);
SHELL_CMD_REGISTER(video, &sub_video, "Video controls", NULL);
