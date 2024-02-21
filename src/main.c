#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>
#include <zephyr/drivers/usb/udc.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/usb/usbd.h>
#include <zephyr/usb/bos.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

#include "si5351.h"

extern bool usbd_cdc_uvc_data_terminal_ready;

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
#if 0
	struct usb_bos_capability_superspeed_usb superspeed_usb;
#endif
} __packed my_usbd_bos_desc = {
	.bos = {
		.bLength = sizeof(struct usb_bos_descriptor),
		.bDescriptorType = USB_DESC_BOS,
		.wTotalLength = sys_cpu_to_le16(sizeof(my_usbd_bos_desc)),
		.bNumDeviceCaps = 1, // TODO: this skips superspeed_usb!
	},
	.lpm = {
		.bLength = sizeof(struct usb_bos_capability_lpm),
		.bDescriptorType = USB_DESC_DEVICE_CAPABILITY,
		.bDevCapabilityType = USB_BOS_CAPABILITY_EXTENSION,
		.bmAttributes = USB_BOS_ATTRIBUTES_LPM | USB_BOS_ATTRIBUTES_BESL,
	},
#if 0
	.superspeed_usb = {
		.bLength = sizeof(struct usb_bos_capability_superspeed_usb),
		.bDescriptorType = USB_DESC_DEVICE_CAPABILITY,
		.bDevCapabilityType = USB_BOS_CAPABILITY_SUPERSPEED_USB,
		.bmAttributes = USB_BOS_ATTRIBUTES_LPM | USB_BOS_ATTRIBUTES_BESL,
		.wSpeedsSupported = sys_cpu_to_le16(USB_BOS_SPEED_SUPERSPEED_GEN1),
		.bFunctionnalSupport = USB_BOS_SPEED_SUPERSPEED_GEN1,
		.bU1DevExitLat = 10,
		.wU2DevExitLat = sys_cpu_to_le16(2047),
	},
#endif
};
static struct usbd_desc_node my_usbd_bos = {
	.desc = &my_usbd_bos_desc,
};

void si5351_i2c_write(uint8_t i2c_addr, uint8_t reg_addr, uint8_t reg_data)
{
	i2c_reg_write_byte(DEVICE_DT_GET(DT_NODELABEL(i2c0)),
			i2c_addr, reg_addr, reg_data);
}

int main(void)
{
	int err = 0;
	bool done = false;

	/* Configure the external PLL over I2C, switching the CPU clock over it.
	 * This is done by hardware, no software config to make that happen. */
	k_sleep(K_MSEC(100));
	LOG_INF("Initializing the si5351 PLL");
	si5351_init();
	k_sleep(K_MSEC(100));

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
	while (true) {
		k_sleep(K_MSEC(1));
		usb23_irq_handler(DEVICE_DT_GET(DT_NODELABEL(zephyr_udc0)));
		if (usbd_cdc_uvc_data_terminal_ready && !done) {
			/* This memory does not contain any meaningful data,
			 * and is just there to provide an example of what
			 */
			cdc_uvc_enqueue_in(
				DT_REG_ADDR(DT_CHOSEN(lattice_usb23_dma)),
				DT_REG_SIZE(DT_CHOSEN(lattice_usb23_dma)));
			done = true;
		}
	}

	return 0;
}
