#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/video.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(app, LOG_LEVEL_DBG);

static const struct device *uvc0_dev = DEVICE_DT_GET(DT_NODELABEL(uvc0));
static const struct device *uvc1_dev = DEVICE_DT_GET(DT_NODELABEL(uvc1));

int main(void)
{
	struct video_buffer vbuf = {0};
	struct video_buffer vbuf2 = {0}; 
	struct video_format fmt = {0};
	struct video_format fmt2 = {0};
	static uint8_t toggle = 0x00;
	int ret = 0;

	/* Get the video format selected by the host */
	ret = video_get_format(uvc0_dev, VIDEO_EP_IN, &fmt);
	if (ret != 0) {
		LOG_ERR("failed to get the video format");
		return ret;
	}

	/* The buffer address is the FIFO endpoint */
	vbuf.buffer = (void *)DT_REG_ADDR_BY_NAME(DT_NODELABEL(uvcmanager0), fifo);
	vbuf.size = vbuf.bytesused = fmt.pitch * fmt.height;
	vbuf.flags = VIDEO_BUF_EOF;

	LOG_DBG("frame %ux%u, %u bytes, addr %p", fmt.width, fmt.height, fmt.pitch * fmt.height, vbuf.buffer);

	/* Only the first buffer needs to be enqueued when using the UVC Manager */
	ret = video_enqueue(uvc0_dev, VIDEO_EP_IN, &vbuf);
	if (ret != 0) {
		LOG_ERR("failed to enqueue the video buffer");
		return 0;
	}

	while (1)
	{
		ret = video_get_format(uvc1_dev, VIDEO_EP_IN, &fmt2);
		if (ret != 0) {
			LOG_ERR("failed to get the video format");
			return ret;
		}
		
		vbuf2.buffer = (void *)DT_REG_ADDR(DT_NODELABEL(ram1));
		vbuf2.size = vbuf2.bytesused = fmt2.pitch * fmt2.height;
		vbuf2.flags = VIDEO_BUF_EOF;

		LOG_DBG("frame %ux%u, %u bytes, addr %p", fmt2.width, fmt2.height, fmt2.pitch * fmt2.height, vbuf2.buffer);

		ret = video_enqueue(uvc1_dev, VIDEO_EP_IN, &vbuf2);
		if (ret != 0) {
			LOG_ERR("failed to enqueue the video buffer");
			return 0;
		}

		ret = video_dequeue(uvc0_dev, VIDEO_EP_IN, NULL, K_FOREVER);
		if (ret != 0) {
			LOG_ERR( "could not dequeue video buffer");
			return 0;
		}
		
		k_sleep(K_MSEC(10));
		//k_sleep(K_FOREVER);

	}

	// k_sleep(K_FOREVER);
	return 0;
}
