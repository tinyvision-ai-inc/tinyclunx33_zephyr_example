#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/video.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(app, LOG_LEVEL_DBG);

static const struct device *uvc0_dev = DEVICE_DT_GET(DT_NODELABEL(uvc0));
static const struct device *uvc1_dev = DEVICE_DT_GET(DT_NODELABEL(uvc1));
const uintptr_t uvcmanager0_fifo = DT_REG_ADDR_BY_NAME(DT_NODELABEL(uvcmanager0), fifo);
const uintptr_t uvcmanager1_fifo = DT_REG_ADDR_BY_NAME(DT_NODELABEL(uvcmanager1), fifo);

int uvcmanager_enqueue_first_frame(const struct device *uvc_dev, uintptr_t fifo, struct video_buffer *vbuf)
{
	struct video_format fmt = {0};
	int ret;

	ret = video_get_format(uvc_dev, VIDEO_EP_IN, &fmt);
	if (ret == -EAGAIN) {
		return ret;
	}
	if (ret) {
		LOG_ERR("failed to get the video format");
		return ret;
	}

	/* The buffer address is the FIFO endpoint */
	vbuf->buffer = (void *)fifo;
	vbuf->size = vbuf->bytesused = fmt.pitch * fmt.height;
	vbuf->flags = VIDEO_BUF_EOF;

	LOG_INF("frame %ux%u, %u bytes, addr %p", fmt.width, fmt.height, fmt.pitch * fmt.height, vbuf->buffer);

	/* Only the first buffer needs to be enqueued when using the UVC Manager */
	ret = video_enqueue(uvc_dev, VIDEO_EP_IN, vbuf);
	if (ret) {
		LOG_ERR("%s: failed to enqueue the first frame", uvc_dev->name);
		return ret;
	}
	LOG_DBG("enqueued %p to %s", vbuf->buffer, uvc_dev->name);

	LOG_INF("FIFO 0x%08lx ready for I/O", fifo);
	return true;
}

int main(void)
{
	struct video_buffer vbuf0 = {0}, vbuf1 = {0};
	int ret;

	/* Get the video format once it is selected by the host */
	for (bool uvc0_ready = false, uvc1_ready = false; !uvc0_ready || !uvc1_ready;) {
		ret = uvcmanager_enqueue_first_frame(uvc0_dev, uvcmanager0_fifo, &vbuf0);
		if (ret < 0) {
			return ret;
		}
		uvc1_ready = ret;

		ret = uvcmanager_enqueue_first_frame(uvc1_dev, uvcmanager1_fifo, &vbuf1);
		if (ret < 0) {
			return ret;
		}
		uvc1_ready = ret;

		k_sleep(K_MSEC(10));
	}

	k_sleep(K_FOREVER);
	return 0;
}
