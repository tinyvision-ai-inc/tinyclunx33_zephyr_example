#include <zephyr/drivers/video.h>
#include <zephyr/drivers/video-controls.h>
#include <zephyr/usb/class/usbd_uvc.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(app_main, LOG_LEVEL_INF);

static const struct device *const uvc0_dev = DEVICE_DT_GET(DT_NODELABEL(uvc0));
static const struct device *const source0_dev = DEVICE_DT_GET(DT_NODELABEL(uvcmanager0));

int app_usb_init(void);

int main(void)
{
	int ret;

	uvc_set_video_dev(uvc0_dev, source0_dev);

	ret = app_usb_init();
	if (ret != 0) {
		LOG_ERR("Failed to initialize USB");
		return ret;
	}

	ret = video_set_ctrl(source0_dev, VIDEO_CID_GAIN, (void *)555);
	if (ret != 0) {
		LOG_ERR("Cannot set the default gain for %s", source0_dev->name);
		return ret;
	}

	return 0;
}
