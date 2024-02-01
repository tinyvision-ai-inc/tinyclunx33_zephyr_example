#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>
#include <zephyr/drivers/usb/udc.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/usb/usbd.h>
#include <zephyr/usb/bos.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

#include "si5351.h"

USBD_DEVICE_DEFINE(my_usbd_dev, DEVICE_DT_GET(DT_NODELABEL(zephyr_udc0)), 0x1209, 0x0001);
USBD_DEVICE_QUALIFIER_DEFINE(my_usbd_device_qualifier);
USBD_CONFIGURATION_DEFINE(my_usbd_config, USB_SCD_SELF_POWERED, 100);
USBD_DESC_LANG_DEFINE(my_usbd_lang);
USBD_DESC_MANUFACTURER_DEFINE(my_usbd_manufacturer, "tinyVision.ai");
USBD_DESC_PRODUCT_DEFINE(my_usbd_product, "tinyCLUNX33");
//USBD_DESC_SERIAL_NUMBER_DEFINE(my_usbd_serial_number, "0123456789ABCDEF");

struct my_usbd_bos {
	struct usb_bos_descriptor bos;
	struct usb_bos_capability_lpm lpm;
} __packed bos_desc_my_usbd_bos = {
	.bos = {
		.bLength = sizeof(struct usb_bos_descriptor),
		.bDescriptorType = USB_DESC_BOS,
		.wTotalLength = sizeof(struct my_usbd_bos),
		.bNumDeviceCaps = 1,
	},
	.lpm = {
		.bLength = sizeof(struct usb_bos_capability_lpm),
		.bDescriptorType = USB_DESC_DEVICE_CAPABILITY,
		.bDevCapabilityType = USB_BOS_CAPABILITY_EXTENSION,
		.bmAttributes = USB_BOS_ATTRIBUTES_LPM | USB_BOS_ATTRIBUTES_BESL,
	},
};
static struct usbd_desc_node my_usbd_bos = {
	.desc = &bos_desc_my_usbd_bos,
};

char const video_frames[] = {
#include "data/video_frames.h"
};

void si5351_i2c_write(uint8_t i2c_addr, uint8_t reg_addr, uint8_t reg_data)
{
	i2c_reg_write_byte(DEVICE_DT_GET(DT_NODELABEL(i2c0)),
			i2c_addr, reg_addr, reg_data);
}

int main(void)
{
	int err = 0;

	/* Configure the external PLL over I2C, switching teh CPU clock over it.
	 * This is done by hardware, no software config to make that happen. */
	k_sleep(K_MSEC(100));
	LOG_INF("Initializing the si5351 PLL");
	si5351_init();
	k_sleep(K_MSEC(500));

	err = usbd_add_descriptor(&my_usbd_dev, &my_usbd_device_qualifier);
	err |= usbd_add_descriptor(&my_usbd_dev, &my_usbd_bos);
	err |= usbd_add_descriptor(&my_usbd_dev, &my_usbd_lang);
	err |= usbd_add_descriptor(&my_usbd_dev, &my_usbd_manufacturer);
	err |= usbd_add_descriptor(&my_usbd_dev, &my_usbd_product);
	//err |= usbd_add_descriptor(&my_usbd_dev, &my_usbd_serial_number);
	err |= usbd_add_configuration(&my_usbd_dev, &my_usbd_config);
	err |= usbd_register_class(&my_usbd_dev, "cdc_uvc_0", 1);
	__ASSERT_NO_MSG(err == 0);

	err = usbd_init(&my_usbd_dev);
	__ASSERT_NO_MSG(err == 0);

	err = usbd_enable(&my_usbd_dev);
	__ASSERT_NO_MSG(err == 0);

	/* As soon as interrupts are enabled in the hardware architecture, this
	 * can be removed */
	for (int i = 0;; i++) {
		k_sleep(K_MSEC(10));
		usb23_irq_handler(DEVICE_DT_GET(DT_NODELABEL(zephyr_udc0)));

		if (i == 100) {
			//cdc_uvc_enqueue_in(video_frames, sizeof(video_frames));
			i = 0;
		}
	}

	return 0;
}
