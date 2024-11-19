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
USBD_DESC_CONFIG_DEFINE(usbd_fs_cfg_desc, "FullSpeed Configuration");
USBD_DESC_CONFIG_DEFINE(usbd_hs_cfg_desc, "HighSpeed Configuration");

USBD_CONFIGURATION_DEFINE(usbd_config_fs, USB_SCD_SELF_POWERED, 100, &usbd_fs_cfg_desc);
USBD_CONFIGURATION_DEFINE(usbd_config_hs, USB_SCD_SELF_POWERED, 100, &usbd_hs_cfg_desc);

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

	usbd_device_set_code_triple(&usbd, USBD_SPEED_FS, USB_BCC_MISCELLANEOUS, 0x02, 0x01);
	usbd_device_set_code_triple(&usbd, USBD_SPEED_HS, USB_BCC_MISCELLANEOUS, 0x02, 0x01);

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
