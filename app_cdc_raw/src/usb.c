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

USBD_CONFIGURATION_DEFINE(usbd_config_fs, USB_SCD_SELF_POWERED, 100);
USBD_CONFIGURATION_DEFINE(usbd_config_hs, USB_SCD_SELF_POWERED, 100);
USBD_CONFIGURATION_DEFINE(usbd_config_ss, USB_SCD_SELF_POWERED, 100);

int app_usb_init(void)
{
	int err;

	LOG_DBG("Adding USB descriptors");
	err = usbd_add_descriptor(&usbd, &usbd_lang);
	if (err) {
		LOG_ERR("failed to add lang descriptor");
		return err;
	}
	err = usbd_add_descriptor(&usbd, &usbd_manufacturer);
	if (err) {
		LOG_ERR("failed to add manufacturer descriptor");
		return err;
	}
	err = usbd_add_descriptor(&usbd, &usbd_product);
	if (err) {
		LOG_ERR("failed to add product descriptor");
		return err;
	}
	err = usbd_add_descriptor(&usbd, &usbd_bos_cap_lpm);
	if (err) {
		LOG_ERR("failed to add bos_cap_lpm descriptor");
		return err;
	}
	err = usbd_add_descriptor(&usbd, &usbd_bos_cap_ss);
	if (err) {
		LOG_ERR("failed to add bos_cap_ss descriptor");
		return err;
	}

	LOG_DBG("Adding USB configuration");
	err = usbd_add_configuration(&usbd, USBD_SPEED_FS, &usbd_config_fs);
	if (err) {
		LOG_ERR("failed to add FullSpeed, configuration");
		return err;
	}
	err = usbd_add_configuration(&usbd, USBD_SPEED_HS, &usbd_config_hs);
	if (err) {
		LOG_ERR("failed to add HighSpeed, configuration");
		return err;
	}
	err = usbd_add_configuration(&usbd, USBD_SPEED_SS, &usbd_config_ss);
	if (err) {
		LOG_ERR("failed to add SuperSpeed, configuration");
		return err;
	}

	usbd_device_set_code_triple(&usbd, USBD_SPEED_FS, USB_BCC_MISCELLANEOUS, 0x02, 0x01);
	usbd_device_set_code_triple(&usbd, USBD_SPEED_HS, USB_BCC_MISCELLANEOUS, 0x02, 0x01);
	usbd_device_set_code_triple(&usbd, USBD_SPEED_SS, USB_BCC_MISCELLANEOUS, 0x02, 0x01);

	LOG_DBG("Adding USB classes");
	err = usbd_register_all_classes(&usbd, USBD_SPEED_FS, 1);
	if (err) {
		LOG_ERR("failed to register FullSpeed USB clases");
		return err;
	}
	err = usbd_register_all_classes(&usbd, USBD_SPEED_HS, 1);
	if (err) {
		LOG_ERR("failed to register HighSpeed USB clases");
		return err;
	}
	err = usbd_register_all_classes(&usbd, USBD_SPEED_SS, 1);
	if (err) {
		LOG_ERR("failed to register SuperSpeed USB clases");
		return err;
	}

	LOG_DBG("Finalize USB configuration");
	err = usbd_init(&usbd);
	if (err) {
		LOG_ERR("failed to initialize USB");
		return err;
	}

	LOG_DBG("Starting USB operation");
	err = usbd_enable(&usbd);
	if (err) {
		LOG_ERR("failed to enable the USB device");
		return err;
	}

	return 0;
}

SYS_INIT(app_usb_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
