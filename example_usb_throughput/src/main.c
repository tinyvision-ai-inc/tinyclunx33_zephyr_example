#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/clock_control.h>
#include <zephyr/drivers/usb/udc.h>
#include <zephyr/usb/usbd.h>
#include <zephyr/usb/bos.h>
#include <zephyr/usb/class/usb_cdc_raw.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

USBD_DEVICE_DEFINE(my_usbd, DEVICE_DT_GET(DT_NODELABEL(zephyr_udc0)), 0x1209, 0x0001);
USBD_DEVICE_QUALIFIER_DEFINE(my_usbd_dev_qualifier);
USBD_CONFIGURATION_DEFINE(my_usbd_config, USB_SCD_SELF_POWERED, 100);
USBD_BOS_DEFINE(my_usbd_bos);
USBD_DESC_LANG_DEFINE(my_usbd_lang);
USBD_DESC_MANUFACTURER_DEFINE(my_usbd_manufacturer, "tinyVision.ai");
USBD_DESC_PRODUCT_DEFINE(my_usbd_product, "tinyCLUNX33");
//USBD_DESC_SERIAL_NUMBER_DEFINE(my_usbd_serial_number, "0123456789ABCDEF");

/* Static pool of buffer with no data in it */
NET_BUF_POOL_DEFINE(_buf_pool, 2, 0, sizeof(struct udc_buf_info), NULL);

int main(void)
{
	const struct device *cdc0 = DEVICE_DT_GET(DT_NODELABEL(cdc0));
	const struct device *pll0 = DEVICE_DT_GET(DT_NODELABEL(pll0));
	struct net_buf *buf;
	int err;

	k_sleep(K_MSEC(200));
	clock_control_on(pll0, NULL);
	k_sleep(K_MSEC(200));

	err = usbd_add_descriptor(&my_usbd, &my_usbd_dev_qualifier);
	err |= usbd_add_descriptor(&my_usbd, &my_usbd_bos);
	err |= usbd_add_descriptor(&my_usbd, &my_usbd_lang);
	err |= usbd_add_descriptor(&my_usbd, &my_usbd_manufacturer);
	err |= usbd_add_descriptor(&my_usbd, &my_usbd_product);
	//err |= usbd_add_descriptor(&my_usbd, &my_usbd_serial_number);
	err |= usbd_add_configuration(&my_usbd, &my_usbd_config);
	err |= usbd_register_class(&my_usbd, "cdc_raw_0", 1);
	__ASSERT_NO_MSG(err == 0);

	err = usbd_init(&my_usbd);
	__ASSERT_NO_MSG(err == 0);

	err = usbd_enable(&my_usbd);
	__ASSERT_NO_MSG(err == 0);

	/* Allocate a buffer with existing data */
	buf = net_buf_alloc_with_data(&_buf_pool,
		(void *)0xb1100000, 1024 * 63, Z_TIMEOUT_NO_WAIT);
	__ASSERT_NO_MSG(buf != NULL);

	LOG_DBG("0");

	/* Wait that a serial console connects */
	while (!cdc_raw_is_ready(cdc0)) {
		k_sleep(K_MSEC(100));
	}
	LOG_DBG("1");

	/* Then give a bit of time for the console to disconnect */
	k_sleep(K_SECONDS(10));
	LOG_DBG("2");

	/* Then perform the bulk throughput test */
	for (int i = 0;;) {
		k_sleep(K_MSEC(100));
		if (cdc_raw_write(cdc0, buf) == 0) {
			LOG_INF("%d", i++);
		}
	}

	return 0;
}
