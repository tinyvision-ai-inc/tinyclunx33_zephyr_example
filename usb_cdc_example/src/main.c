#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/clock_control.h>
#include <zephyr/drivers/usb/udc.h>
#include <zephyr/usb/usbd.h>
#include <zephyr/usb/bos.h>
#include <zephyr/usb/class/usb_cdc_raw.h>

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
	const struct device *udc0 = DEVICE_DT_GET(DT_NODELABEL(zephyr_udc0));
	const struct device *raw0 = DEVICE_DT_GET(DT_NODELABEL(raw0));
	struct net_buf *buf;
	int err;

	k_sleep(K_MSEC(200));
	clock_control_on(DEVICE_DT_GET(DT_NODELABEL(pll0)), NULL);
	k_sleep(K_MSEC(200));

	err = usbd_add_descriptor(&my_usbd_dev, &my_usbd_device_qualifier);
	err |= usbd_add_descriptor(&my_usbd_dev, &my_usbd_bos);
	err |= usbd_add_descriptor(&my_usbd_dev, &my_usbd_lang);
	err |= usbd_add_descriptor(&my_usbd_dev, &my_usbd_manufacturer);
	err |= usbd_add_descriptor(&my_usbd_dev, &my_usbd_product);
	//err |= usbd_add_descriptor(&my_usbd_dev, &my_usbd_serial_number);
	err |= usbd_add_configuration(&my_usbd_dev, &my_usbd_config);
	err |= usbd_register_class(&my_usbd_dev, "cdc_raw_0", 1);
	__ASSERT_NO_MSG(err == 0);

	err = usbd_init(&my_usbd_dev);
	__ASSERT_NO_MSG(err == 0);

	err = usbd_enable(&my_usbd_dev);
	__ASSERT_NO_MSG(err == 0);

	/* Glue with the CDC RAW so that we can react to read and write completion. */
	cdc_raw_set_read_callback(raw0, &_read_callback);
	cdc_raw_set_write_callback(raw0, &_write_callback);

	/* Enqueue a first read request, the rest happens from the callbacks */
	buf = net_buf_alloc_with_data(&_buf_pool,
		usb23_dma_buf, sizeof(usb23_dma_buf), Z_TIMEOUT_NO_WAIT);
	__ASSERT_NO_MSG(err == 0);

	/* Enqueue a first read request, the rest happens from the callbacks */
	err = cdc_raw_read(raw0, buf);
	__ASSERT_NO_MSG(err == 0);

	/* As soon as interrupts are enabled in the hardware architecture, this
	 * can be removed */
	for (int i = 0;; i++) {
		k_sleep(K_MSEC(1));
		usb23_irq_handler(udc0);
	}

	return 0;
}
