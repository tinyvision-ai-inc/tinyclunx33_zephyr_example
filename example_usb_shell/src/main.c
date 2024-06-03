#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/usb/udc.h>
#include <zephyr/drivers/video.h>
#include <zephyr/usb/usbd.h>
#include <zephyr/usb/bos.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

USBD_DEVICE_DEFINE(my_usbd, DEVICE_DT_GET(DT_NODELABEL(zephyr_udc0)), 0x1209, 0x0001);

int main(void)
{
	int err = 0;
#if 0
	USBD_DEVICE_QUALIFIER_DEFINE(my_usbd_dev_qualifier);
	err |= usbd_add_descriptor(&my_usbd, &my_usbd_dev_qualifier);
	__ASSERT_NO_MSG(err == 0);
#endif
	USBD_BOS_DEFINE(my_usbd_bos);
	err |= usbd_add_descriptor(&my_usbd, &my_usbd_bos);
	__ASSERT_NO_MSG(err == 0);

	USBD_DESC_LANG_DEFINE(my_usbd_lang);
	err |= usbd_add_descriptor(&my_usbd, &my_usbd_lang);
	__ASSERT_NO_MSG(err == 0);

	USBD_DESC_MANUFACTURER_DEFINE(my_usbd_manufacturer, "tinyVision.ai");
	err |= usbd_add_descriptor(&my_usbd, &my_usbd_manufacturer);
	__ASSERT_NO_MSG(err == 0);

	USBD_DESC_PRODUCT_DEFINE(my_usbd_product, "tinyCLUNX33");
	err |= usbd_add_descriptor(&my_usbd, &my_usbd_product);
	__ASSERT_NO_MSG(err == 0);
#if 0
	USBD_DESC_SERIAL_NUMBER_DEFINE(my_usbd_serial_number);
	err |= usbd_add_descriptor(&my_usbd, &my_usbd_serial_number);
	__ASSERT_NO_MSG(err == 0);
#endif
	USBD_CONFIGURATION_DEFINE(my_usbd_config, USB_SCD_SELF_POWERED, 100);
	err |= usbd_add_configuration(&my_usbd, &my_usbd_config);
	__ASSERT_NO_MSG(err == 0);

	err |= usbd_register_class(&my_usbd, "cdc_acm_0", 1);
	__ASSERT_NO_MSG(err == 0);

	err |= usbd_init(&my_usbd);
	__ASSERT_NO_MSG(err == 0);

	err |= usbd_enable(&my_usbd);
	__ASSERT_NO_MSG(err == 0);

	k_sleep(K_FOREVER);
	return 0;
}
