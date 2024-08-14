#include <zephyr/drivers/usb/udc.h>
#include <zephyr/usb/usbd.h>
#include <zephyr/usb/bos.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(app_usb, LOG_LEVEL_DBG);

static const struct device *const udc0_dev = DEVICE_DT_GET(DT_NODELABEL(zephyr_udc0));

USBD_DEVICE_DEFINE(usbd, udc0_dev, 0x1209, 0x0001);

USBD_DESC_MANUFACTURER_DEFINE(usbd_manufacturer, "tinyVision.ai");
USBD_DESC_PRODUCT_DEFINE(usbd_product, "tinyCLUNX33");
USBD_DESC_LANG_DEFINE(usbd_lang);

static const struct usb_bos_capability_superspeed_usb bos_cap_ss = {
	.bLength = sizeof(struct usb_bos_capability_superspeed_usb),
	.bDescriptorType = USB_DESC_DEVICE_CAPABILITY,
	.bDevCapabilityType = USB_BOS_CAPABILITY_SUPERSPEED_USB,
	.bmAttributes = 0,
	.wSpeedsSupported = sys_cpu_to_le16(USB_BOS_SPEED_SUPERSPEED_GEN1 |
					    USB_BOS_SPEED_HIGHSPEED | USB_BOS_SPEED_FULLSPEED),
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

struct usbd_desc_node *const usbd_descs[] = {&usbd_lang, &usbd_manufacturer, &usbd_product,
					     &usbd_bos_cap_lpm, &usbd_bos_cap_ss};

USBD_CONFIGURATION_DEFINE(usbd_config, USB_SCD_SELF_POWERED, 100);

int
app_usb_init(void)
{
	int ret;

	LOG_DBG("Adding USB descriptors");
	for (size_t i = 0; i < ARRAY_SIZE(usbd_descs); i++) {
		ret = usbd_add_descriptor(&usbd, usbd_descs[i]);
		if (ret) {
			LOG_ERR("failed to add descriptor %u/%u", i, ARRAY_SIZE(usbd_descs));
			return ret;
		}
	}

	LOG_DBG("Adding USB configuration");
	ret = usbd_add_configuration(&usbd, USBD_SPEED_SS, &usbd_config);
	if (ret) {
		LOG_ERR("failed to add SuperSpeed, configuration");
		return ret;
	}

	usbd_device_set_code_triple(&usbd, USBD_SPEED_SS, USB_BCC_MISCELLANEOUS, 0x02, 0x01);

	LOG_DBG("Adding USB classes");
	ret = usbd_register_all_classes(&usbd, USBD_SPEED_SS, 1);
	if (ret) {
		LOG_ERR("failed to register USB clases");
		return ret;
	}

	LOG_DBG("Finalize USB configuration");
	ret = usbd_init(&usbd);
	if (ret) {
		LOG_ERR("failed to initialize USB");
		return ret;
	}

	LOG_DBG("Starting USB operation");
	ret = usbd_enable(&usbd);
	if (ret) {
		LOG_ERR("failed to enable the USB device");
		return ret;
	}

	return 0;
}

SYS_INIT(app_usb_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);