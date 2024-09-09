#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/video.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(app, LOG_LEVEL_DBG);

static const struct device *uvc0_dev = DEVICE_DT_GET(DT_NODELABEL(uvc0));
static const struct device *uvc1_dev = DEVICE_DT_GET(DT_NODELABEL(uvc1));

int main(void)
{
    printf("hello\n");
    k_sleep(K_SECONDS(5));
    printf("test1......\n");

    const struct device *video_dev = DEVICE_DT_GET(DT_NODELABEL(imx219));
    if (video_dev == NULL || !device_is_ready(video_dev))
    {
        printf("Could not get I2C cam device\n");
        return;
    }
	printk("Video device: %s\n", video_dev->name);

}
