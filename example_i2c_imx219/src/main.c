#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/usb/udc.h>
#include <zephyr/drivers/video.h>
#include <zephyr/usb/usbd.h>
#include <zephyr/usb/bos.h>
#include <zephyr/logging/log.h>
#include <stdio.h>
LOG_MODULE_REGISTER(app, LOG_LEVEL_DBG);

#define UDC0 DEVICE_DT_GET(DT_NODELABEL(zephyr_udc0))
#define UVC0 DEVICE_DT_GET(DT_NODELABEL(uvc0))

#define SPEEDS (USB_BOS_SPEED_SUPERSPEED_GEN1 | USB_BOS_SPEED_HIGHSPEED | USB_BOS_SPEED_FULLSPEED)

static const struct usb_bos_capability_superspeed_usb bos_cap_ss = {
	.bLength = sizeof(struct usb_bos_capability_superspeed_usb),
	.bDescriptorType = USB_DESC_DEVICE_CAPABILITY,
	.bDevCapabilityType = USB_BOS_CAPABILITY_SUPERSPEED_USB,
	.bmAttributes = 0,
	.wSpeedsSupported = sys_cpu_to_le16(SPEEDS),
	.bFunctionnalSupport = 1,
	.bU1DevExitLat = 10,
	.wU2DevExitLat = sys_cpu_to_le16(1023),
};
USBD_DESC_BOS_DEFINE(usbd_bos_cap_ss, sizeof(bos_cap_ss), &bos_cap_ss);

static const struct usb_bos_capability_lpm bos_cap_lpm = {
	.bLength = sizeof(struct usb_bos_capability_lpm),
	.bDescriptorType = USB_DESC_DEVICE_CAPABILITY,
	.bDevCapabilityType = USB_BOS_CAPABILITY_EXTENSION,
	.bmAttributes = USB_BOS_ATTRIBUTES_LPM,
};
USBD_DESC_BOS_DEFINE(usbd_bos_cap_lpm, sizeof(bos_cap_lpm), &bos_cap_lpm);

USBD_DEVICE_DEFINE(usbd, UDC0, 0x1209, 0x0001);
USBD_CONFIGURATION_DEFINE(usbd_config, USB_SCD_SELF_POWERED, 100);
USBD_DESC_MANUFACTURER_DEFINE(usbd_manufacturer, "tinyVision.ai");
USBD_DESC_PRODUCT_DEFINE(usbd_product, "tinyCLUNX33");
USBD_DESC_LANG_DEFINE(usbd_lang);

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
