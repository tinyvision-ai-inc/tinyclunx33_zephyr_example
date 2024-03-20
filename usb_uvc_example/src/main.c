#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/usb/udc.h>
#include <zephyr/drivers/clock_control.h>
#include <zephyr/usb/usbd.h>
#include <zephyr/usb/bos.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

USBD_DEVICE_DEFINE(my_usbd_dev, DEVICE_DT_GET(DT_NODELABEL(zephyr_udc0)), 0x1209, 0x0001);
USBD_DEVICE_QUALIFIER_DEFINE(my_usbd_device_qualifier);
USBD_CONFIGURATION_DEFINE(my_usbd_config, USB_SCD_SELF_POWERED, 100);
USBD_DESC_LANG_DEFINE(my_usbd_lang);
USBD_DESC_MANUFACTURER_DEFINE(my_usbd_manufacturer, "tinyVision.ai");
USBD_DESC_PRODUCT_DEFINE(my_usbd_product, "tinyCLUNX33");
//USBD_DESC_SERIAL_NUMBER_DEFINE(my_usbd_serial_number, "0123456789ABCDEF");

struct {
	struct usb_bos_descriptor bos;
	struct usb_bos_capability_lpm lpm;
	struct usb_bos_capability_superspeed_usb superspeed_usb;
} __packed my_usbd_bos_desc = {
	.bos = {
		.bLength = sizeof(struct usb_bos_descriptor),
		.bDescriptorType = USB_DESC_BOS,
		.wTotalLength = sys_cpu_to_le16(sizeof(my_usbd_bos_desc)),
		.bNumDeviceCaps = 2,
	},
	.lpm = {
		.bLength = sizeof(struct usb_bos_capability_lpm),
		.bDescriptorType = USB_DESC_DEVICE_CAPABILITY,
		.bDevCapabilityType = USB_BOS_CAPABILITY_EXTENSION,
		.bmAttributes = USB_BOS_ATTRIBUTES_LPM, //  | USB_BOS_ATTRIBUTES_BESL
	},
	.superspeed_usb = {
		.bLength = sizeof(struct usb_bos_capability_superspeed_usb),
		.bDescriptorType = USB_DESC_DEVICE_CAPABILITY,
		.bDevCapabilityType = USB_BOS_CAPABILITY_SUPERSPEED_USB,
		.bmAttributes = USB_BOS_ATTRIBUTES_LPM, //| USB_BOS_ATTRIBUTES_BESL,
		.wSpeedsSupported = sys_cpu_to_le16(USB_BOS_SPEED_SUPERSPEED_GEN1
			| USB_BOS_SPEED_HIGHSPEED | USB_BOS_SPEED_FULLSPEED),
		.bFunctionnalSupport = 1,
		.bU1DevExitLat = 10,
		.wU2DevExitLat = sys_cpu_to_le16(1023),
	},
};
static struct usbd_desc_node my_usbd_bos = {
	.desc = &my_usbd_bos_desc,
};

int main(void)
{
	int err = 0;

	clock_control_on(DEVICE_DT_GET(DT_NODELABEL(pll0)), NULL);

	err = usbd_add_descriptor(&my_usbd_dev, &my_usbd_device_qualifier);
	err |= usbd_add_descriptor(&my_usbd_dev, &my_usbd_bos);
	err |= usbd_add_descriptor(&my_usbd_dev, &my_usbd_lang);
	err |= usbd_add_descriptor(&my_usbd_dev, &my_usbd_manufacturer);
	err |= usbd_add_descriptor(&my_usbd_dev, &my_usbd_product);
	//err |= usbd_add_descriptor(&my_usbd_dev, &my_usbd_serial_number);
	err |= usbd_add_configuration(&my_usbd_dev, &my_usbd_config);
	err |= usbd_register_class(&my_usbd_dev, "uvc_0", 1);
	__ASSERT_NO_MSG(err == 0);

	err = usbd_init(&my_usbd_dev);
	__ASSERT_NO_MSG(err == 0);

	err = usbd_enable(&my_usbd_dev);
	__ASSERT_NO_MSG(err == 0);

	/* As soon as interrupts are enabled in the hardware architecture, this
	 * can be removed */
	while (true) {
		k_sleep(K_MSEC(1));
		usb23_irq_handler(DEVICE_DT_GET(DT_NODELABEL(zephyr_udc0)));
	}

	return 0;
}
