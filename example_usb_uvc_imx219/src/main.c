#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/usb/udc.h>
#include <zephyr/drivers/video.h>
#include <zephyr/usb/usbd.h>
#include <zephyr/usb/bos.h>
#include <zephyr/logging/log.h>
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
	struct video_buffer vbuf = {0};
	struct video_format fmt = {0};
	int ret = 0;

	ret |= usbd_add_descriptor(&usbd, &usbd_lang);
	ret |= usbd_add_descriptor(&usbd, &usbd_manufacturer);
	ret |= usbd_add_descriptor(&usbd, &usbd_product);
	ret |= usbd_add_descriptor(&usbd, &usbd_bos_cap_lpm);
	ret |= usbd_add_descriptor(&usbd, &usbd_bos_cap_ss);
	__ASSERT_NO_MSG(ret == 0);

	ret = usbd_add_configuration(&usbd, USBD_SPEED_SS, &usbd_config);
	__ASSERT_NO_MSG(ret == 0);

	usbd_device_set_code_triple(&usbd, USBD_SPEED_SS, USB_BCC_MISCELLANEOUS, 0x02, 0x01);

	ret = usbd_register_all_classes(&usbd, USBD_SPEED_SS, 1);
	__ASSERT_NO_MSG(ret == 0);

	ret = usbd_init(&usbd);
	__ASSERT_NO_MSG(ret == 0);

	ret = usbd_enable(&usbd);
	__ASSERT_NO_MSG(ret == 0);

	ret = video_get_format(UVC0, VIDEO_EP_IN, &fmt);
	if (ret != 0) {
		LOG_ERR("failed to get the video format");
	}

	/* The buffer address is the FIFO endpoint */
	vbuf.buffer = (void *)DT_REG_ADDR_BY_NAME(DT_NODELABEL(uvcmanager), fifo);
	vbuf.size = vbuf.bytesused = fmt.pitch * fmt.height;
	vbuf.flags = VIDEO_BUF_EOF;

	LOG_DBG("frame %ux%u, %u bytes, addr %p", fmt.width, fmt.height, fmt.pitch * fmt.height, vbuf.buffer);

	/* Only the first buffer needs to be enqueued when using the UVC Manager */
	ret = video_enqueue(UVC0, VIDEO_EP_IN, &vbuf);
	if (ret != 0) {
		LOG_ERR("failed to enqueue the video buffer");
		return 0;
	}

	while (true) {
		usb23_irq_handler(UDC0);
		k_sleep(K_MSEC(10));
	}

	return 0;
}
