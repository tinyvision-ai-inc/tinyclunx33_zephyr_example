#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/video.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(app, LOG_LEVEL_DBG);

static const struct device *uvc0_dev = DEVICE_DT_GET(DT_NODELABEL(uvc0));
static const struct device *udc0_dev = DEVICE_DT_GET(DT_NODELABEL(zephyr_udc0));

int main(void)
{
	struct video_buffer vbuf = {0};
	struct video_format fmt = {0};
	int ret = 0;

	/* Get the video format selected by the host */
	ret = video_get_format(uvc0_dev, VIDEO_EP_IN, &fmt);
	if (ret != 0) {
		LOG_ERR("failed to get the video format");
		return ret;
	}

	/* The buffer address is the FIFO endpoint */
	vbuf.buffer = (void *)DT_REG_ADDR_BY_NAME(DT_NODELABEL(uvcmanager), fifo);
	vbuf.size = vbuf.bytesused = fmt.pitch * fmt.height;
	vbuf.flags = VIDEO_BUF_EOF;

	LOG_DBG("frame %ux%u, %u bytes, addr %p", fmt.width, fmt.height, fmt.pitch * fmt.height, vbuf.buffer);

	/* Only the first buffer needs to be enqueued when using the UVC Manager */
	ret = video_enqueue(uvc0_dev, VIDEO_EP_IN, &vbuf);
	if (ret != 0) {
		LOG_ERR("failed to enqueue the video buffer");
		return 0;
	}

	while (true) {
		usb23_irq_handler(udc0_dev);
		k_sleep(K_MSEC(10));
	}

	return 0;
}
