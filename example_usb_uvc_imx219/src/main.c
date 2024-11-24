#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/video.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(app, LOG_LEVEL_DBG);

static const struct device *uvc_dev = DEVICE_DT_GET(DT_NODELABEL(uvc0));
static const struct device *uvcmanager_dev = DEVICE_DT_GET(DT_NODELABEL(uvcmanager0));

int main(void)
{
	struct video_buffer vbuf = {0};
	struct video_format fmt = {0};
	int ret;

	/* Get the video format once it is selected by the host */
	while (true) {
		ret = video_get_format(uvc_dev, VIDEO_EP_IN, &fmt);
		if (ret == 0) {
			break;
		}
		if (ret != -EAGAIN) {
			LOG_ERR("Failed to get the video format");
			return ret;
		}

		k_sleep(K_MSEC(10));
	}

	LOG_DBG("frame %ux%u, %u bytes", fmt.width, fmt.height, fmt.pitch * fmt.height);

	/* The buffer address is not used, only the size matters */
	vbuf.size = vbuf.bytesused = fmt.pitch * fmt.height;

	/* Only the first buffer needs to be enqueued when using the UVC Manager */
	ret = video_enqueue(uvcmanager_dev, VIDEO_EP_IN, &vbuf);
	if (ret) {
		LOG_ERR("failed to enqueue the video buffer");
		return ret;
	}

	k_sleep(K_FOREVER);
	return 0;
}
