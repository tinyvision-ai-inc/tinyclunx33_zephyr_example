#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/video.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(app, LOG_LEVEL_INF);

static const struct device *uvc0_dev = DEVICE_DT_GET(DT_NODELABEL(uvc0));

int uvc0_enqueue_first_frame(struct video_buffer *vbuf)
{
	struct video_format fmt = {0};
	int ret;

	ret = video_get_format(uvc0_dev, VIDEO_EP_IN, &fmt);
	if (ret == -EAGAIN) {
		return false;
	}
	if (ret) {
		LOG_ERR("failed to get the video format");
		return ret;
	}

	/* The buffer address is the FIFO endpoint */
	vbuf->buffer = (void *)DT_REG_ADDR_BY_NAME(DT_NODELABEL(uvcmanager0), fifo);
	vbuf->size = vbuf->bytesused = fmt.pitch * fmt.height;
	vbuf->flags = VIDEO_BUF_EOF;

	LOG_INF("frame %ux%u, %u bytes, addr %p", fmt.width, fmt.height, fmt.pitch * fmt.height, vbuf->buffer);

	/* Only the first buffer needs to be enqueued when using the UVC Manager */
	ret = video_enqueue(uvc0_dev, VIDEO_EP_IN, vbuf);
	if (ret) {
		LOG_ERR("UVC0: failed to enqueue the first frame");
		return ret;
	}
	LOG_DBG("enqueued %p to %s", vbuf, uvc0_dev->name);

	LOG_INF("UVC0 ready for I/O");

	return true;
}

int main(void)
{
	struct video_buffer vbuf = {0};
	int ret;

	/* Get the video format once it is selected by the host */
	for (bool uvc0_ready = 0; !uvc0_ready;) {
		if (!uvc0_ready) {
			ret = uvc0_enqueue_first_frame(&vbuf);
			if (ret < 0) {
				LOG_ERR("UVC0: failed to enqueue the first frame");
				return ret;
			}
			uvc0_ready = ret;
		}

		k_sleep(K_MSEC(10));
	}

	k_sleep(K_FOREVER);
	return 0;
}
