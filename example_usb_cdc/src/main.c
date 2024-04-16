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

/* And then define the actual buffer that is going to be used, with a prefix
 * that makes the linker script place it on a DMA-accessible memory */
uint8_t usb23_dma_buf[1024];

int _read_callback(const struct device *dev, struct net_buf *buf, int err)
{
	if (err) {
		LOG_ERR("USB read error %d", err);
		return err;
	}

	LOG_HEXDUMP_DBG(buf->data, buf->len, "USB read complete");

	/* Once we read something, echo the same buffer back, so it is not
	 * freed now yet... */
	return cdc_raw_write(dev, buf);
}

int _write_callback(const struct device *dev, struct net_buf *buf, int err)
{
	if (err) {
		LOG_ERR("USB read error %d", err);
		return err;
	}

	/* Now that the chain is complete, we can free that buffer, but we can
	 * also re-use it for the next read */
	return cdc_raw_read(dev, buf);
}

int main(void)
{
	const struct device *cdc0 = DEVICE_DT_GET(DT_NODELABEL(cdc0));
	const struct device *pll0 = DEVICE_DT_GET(DT_NODELABEL(pll0));
	struct net_buf *buf;
	int err;

	clock_control_on(pll0, NULL);

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

	/* Glue with the CDC RAW so that we can react to read and write completion. */
	cdc_raw_set_read_callback(cdc0, &_read_callback);
	cdc_raw_set_write_callback(cdc0, &_write_callback);

	/* Enqueue a first read request, the rest happens from the callbacks */
	buf = net_buf_alloc_with_data(&_buf_pool,
		usb23_dma_buf, sizeof(usb23_dma_buf), Z_TIMEOUT_NO_WAIT);
	__ASSERT_NO_MSG(err == 0);

	/* Enqueue a first read request, the rest happens from the callbacks */
	err = cdc_raw_read(cdc0, buf);
	__ASSERT_NO_MSG(err == 0);

	k_sleep(K_FOREVER);

	return 0;
}
