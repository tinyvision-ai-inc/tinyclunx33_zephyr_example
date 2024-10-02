#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/video.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(app, LOG_LEVEL_DBG);

static const struct device *uvc0_dev = DEVICE_DT_GET(DT_NODELABEL(uvc0));

int main(void)
{
	struct video_buffer vbuf = {0};
	struct video_format fmt = {0};
	int err;

	/* Get the video format once it is selected by the host */
	while (true) {
		err = video_get_format(uvc0_dev, VIDEO_EP_IN, &fmt);
		if (err == 0) {
			break;
		}
		if (err != -EAGAIN) {
			LOG_ERR("failed to get the video format");
			return err;
		}

		k_sleep(K_MSEC(10));
	}

	/* The buffer address is the FIFO endpoint */
	vbuf.buffer = (void *)DT_REG_ADDR_BY_NAME(DT_NODELABEL(uvcmanager), fifo);
	vbuf.size = vbuf.bytesused = fmt.pitch * fmt.height;
	vbuf.flags = VIDEO_BUF_EOF;

	LOG_DBG("frame %ux%u, %u bytes, addr %p", fmt.width, fmt.height, fmt.pitch * fmt.height, vbuf.buffer);

	/* Only the first buffer needs to be enqueued when using the UVC Manager */
	err = video_enqueue(uvc0_dev, VIDEO_EP_IN, &vbuf);
	if (err) {
		LOG_ERR("failed to enqueue the video buffer");
		return err;
	}

	k_sleep(K_FOREVER);
	return 0;
}
