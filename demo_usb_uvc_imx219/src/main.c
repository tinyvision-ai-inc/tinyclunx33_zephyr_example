#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/video.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(app, LOG_LEVEL_INF);

static const struct device *uvc0_dev = DEVICE_DT_GET(DT_NODELABEL(uvc0));
static const struct device *uvc1_dev = DEVICE_DT_GET(DT_NODELABEL(uvc1));
static const struct device *sw_dev = DEVICE_DT_GET(DT_NODELABEL(sw_generator));

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
	vbuf->buffer = (void *)DT_REG_ADDR_BY_NAME(DT_NODELABEL(uvcmanager), fifo);
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

int uvc1_enqueue_first_frame(void)
{
	struct video_format fmt = {0};
	struct video_buffer *vbuf;
	int ret;

	ret = video_get_format(uvc1_dev, VIDEO_EP_IN, &fmt);
	if (ret == -EAGAIN) {
		return false;
	}
	if (ret) {
		LOG_ERR("failed to get the video format");
		return ret;
	}

	LOG_INF("Allocating a video buffer of %u bytes for UVC1", fmt.pitch * fmt.height);

	/* If this gets stuck, you might need to increase CONFIG_VIDEO_BUFFER_POOL_SZ_MAX */
	vbuf = video_buffer_alloc(fmt.pitch * fmt.height);
	if (vbuf == NULL) {
		LOG_ERR("could not allocate the video buffer");
		return -ENOMEM;
	}

	for (size_t i = 0; i < vbuf->size; i++) {
		vbuf->buffer[i] ^= 0xff;
	}
	vbuf->bytesused = vbuf->size;
	vbuf->flags = VIDEO_BUF_EOF;

	ret = video_enqueue(sw_dev, VIDEO_EP_OUT, vbuf);
	if (ret != 0) {
		LOG_ERR("could not enqueue video buffer to %s", sw_dev->name);
		return ret;
	}
	LOG_DBG("enqueued %p to %s", vbuf, sw_dev->name);

	LOG_INF("UVC1 ready for I/O");

	return true;
}

void uvc1_io_thread(void *, void *, void *)
{
	struct video_buffer *vbuf;
	int ret;

	/* UVC needs the poll API implemented */
	while (true) {
		ret = video_dequeue(sw_dev, VIDEO_EP_OUT, &vbuf, K_FOREVER);
		if (ret != 0) {
			LOG_ERR("could not dequeue video buffer from %s", sw_dev->name);
			return;
		}
		LOG_DBG("dequeued %p from %s", vbuf, sw_dev->name);

		ret = video_enqueue(uvc1_dev, VIDEO_EP_IN, vbuf);
		if (ret != 0) {
			LOG_ERR("could not enqueue video buffer to %s", uvc1_dev->name);
			return;
		}
		LOG_DBG("enqueued %p to %s", vbuf, uvc1_dev->name);

		ret = video_dequeue(uvc1_dev, VIDEO_EP_IN, &vbuf, K_FOREVER);
		if (ret != 0) {
			LOG_ERR("could not dequeue video buffer from %s", uvc1_dev->name);
			return;
		}
		LOG_DBG("dequeued %p from %s", vbuf, uvc1_dev->name);

		ret = video_enqueue(sw_dev, VIDEO_EP_OUT, vbuf);
		if (ret != 0) {
			LOG_ERR("could not enqueue video buffer to %s", sw_dev->name);
			return;
		}
		LOG_DBG("enqueued %p to %s", vbuf, sw_dev->name);
	}
}

K_THREAD_DEFINE(uvc1_io, 1024, uvc1_io_thread, NULL, NULL, NULL, CONFIG_MAIN_THREAD_PRIORITY, 0, 0);

int main(void)
{
	struct video_buffer vbuf = {0};
	int ret;

	/* Get the video format once it is selected by the host */
	for (bool uvc0_ready = 0, uvc1_ready = 0; !uvc0_ready || !uvc1_ready;) {
		if (!uvc0_ready) {
			ret = uvc0_enqueue_first_frame(&vbuf);
			if (ret < 0) {
				LOG_ERR("UVC0: failed to enqueue the first frame");
				return ret;
			}
			uvc0_ready = ret;
		}

		if (!uvc1_ready) {
			ret = uvc1_enqueue_first_frame();
			if (ret < 0) {
				LOG_ERR("UVC1: failed to enqueue the first frame");
				return ret;
			}
			uvc1_ready = ret;
		}

		k_sleep(K_MSEC(10));
	}

	k_sleep(K_FOREVER);
	return 0;
}
