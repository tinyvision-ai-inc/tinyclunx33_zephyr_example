#include <zephyr/drivers/video.h>
#include <zephyr/drivers/video-controls.h>
#include <zephyr/usb/class/usbd_uvc.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(app_main, LOG_LEVEL_INF);

static const struct device *const uvc0_dev = DEVICE_DT_GET(DT_NODELABEL(uvc0));
static const struct device *const uvcmanager0_dev = DEVICE_DT_GET(DT_NODELABEL(uvcmanager0));

int app_usb_init(void);

#warning The IMX296 driver not functional yet

int main(void)
{
	int ret;

	uvc_set_video_dev(uvc0_dev, uvcmanager0_dev);

	LOG_INF("Initializing USB and associated USB classes");

	ret = app_usb_init();
	if (ret != 0) {
		LOG_ERR("Failed to initialize USB");
		return ret;
	}

	LOG_INF("Started USB, configuring video stream default values");

	ret = video_set_ctrl(uvcmanager0_dev, VIDEO_CID_GAIN, (void *)555);
	if (ret != 0) {
		LOG_ERR("Cannot set the default gain for %s", uvcmanager0_dev->name);
		return ret;
	}

	LOG_INF("Done, exiting main() and letting the USB stack run");

	return 0;
}
