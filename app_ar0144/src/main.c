#include <zephyr/drivers/usb/udc.h>
#include <zephyr/usb/usbd.h>
#include <zephyr/usb/bos.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/shell/shell.h>
#include "cam_test.h"

LOG_MODULE_REGISTER(main, LOG_LEVEL_ERR);

const struct device *uvc0_dev = DEVICE_DT_GET(DT_NODELABEL(uvc0));
const struct device *uvcmanager0_dev = DEVICE_DT_GET(DT_NODELABEL(uvcmanager0));

const struct device *uvc1_dev = DEVICE_DT_GET(DT_NODELABEL(uvc1));
const struct device *uvcmanager1_dev = DEVICE_DT_GET(DT_NODELABEL(uvcmanager1));

static int cmd_mipi_init_regs(void)
{
    volatile uint32_t *mipi_base = (volatile uint32_t *)0xb2000400;
    // Setup the REF_DT to RAW8
    mipi_base[0x27] = 0x2A;
    // Set the settle time
    mipi_base[0x36] = 0x6;
    return 0;   
}

static int cmd_mipi2_init_regs(void)
{
    volatile uint32_t *mipi_base = (volatile uint32_t *)0xb2100400;
    // Setup the REF_DT to RAW8
    mipi_base[0x27] = 0x2A;
    // Set the settle time
    mipi_base[0x36] = 0x6;
    return 0;   
}

int main(void)
{
	int ret;

    ret = cmd_mipi_init_regs();
    ret = cmd_mipi2_init_regs();
	uvc_set_video_dev(uvc0_dev, uvcmanager0_dev);
    uvc_set_video_dev(uvc1_dev, uvcmanager1_dev);

	ret = app_usb_init();
	if (ret != 0) {
		LOG_ERR("Failed to initialize USB");
		return ret;
	}
    ar0144Init();
	return 0;
}
