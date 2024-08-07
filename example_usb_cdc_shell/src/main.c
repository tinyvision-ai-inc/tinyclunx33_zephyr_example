#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/clock_control.h>
#include <zephyr/drivers/usb/udc.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/shell/shell.h>
#include <zephyr/usb/usbd.h>
#include <zephyr/usb/bos.h>
#include <zephyr/usb/class/usb_cdc_raw.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app, LOG_LEVEL_DBG);

#define UDC0 DEVICE_DT_GET(DT_NODELABEL(zephyr_udc0))
#define CDC0 DEVICE_DT_GET(DT_NODELABEL(cdc0))
#define I2C0 DEVICE_DT_GET(DT_NODELABEL(i2c0))

#define SPEEDS (USB_BOS_SPEED_SUPERSPEED_GEN1 | USB_BOS_SPEED_HIGHSPEED | USB_BOS_SPEED_FULLSPEED)

static const struct usb_bos_capability_superspeed_usb bos_cap_ss = {
	.bLength = sizeof(struct usb_bos_capability_superspeed_usb),
	.bDescriptorType = USB_DESC_DEVICE_CAPABILITY,
	.bDevCapabilityType = USB_BOS_CAPABILITY_SUPERSPEED_USB,
	.bmAttributes = 0,
	.wSpeedsSupported = sys_cpu_to_le16(SPEEDS),
	.bFunctionnalSupport = 1,
	.bU1DevExitLat = 10,
	.wU2DevExitLat = sys_cpu_to_le16(1023),
};
USBD_DESC_BOS_DEFINE(my_usbd_bos_cap_ss, sizeof(bos_cap_ss), &bos_cap_ss);

static const struct usb_bos_capability_lpm bos_cap_lpm = {
	.bLength = sizeof(struct usb_bos_capability_lpm),
	.bDescriptorType = USB_DESC_DEVICE_CAPABILITY,
	.bDevCapabilityType = USB_BOS_CAPABILITY_EXTENSION,
	.bmAttributes = USB_BOS_ATTRIBUTES_LPM,
};
USBD_DESC_BOS_DEFINE(my_usbd_bos_cap_lpm, sizeof(bos_cap_lpm), &bos_cap_lpm);

USBD_DEVICE_DEFINE(my_usbd, UDC0, 0x1209, 0x0001);
USBD_CONFIGURATION_DEFINE(my_usbd_config, USB_SCD_SELF_POWERED, 100);
USBD_DESC_MANUFACTURER_DEFINE(my_usbd_manufacturer, "tinyVision.ai");
USBD_DESC_PRODUCT_DEFINE(my_usbd_product, "tinyCLUNX33");
USBD_DESC_LANG_DEFINE(my_usbd_lang);
USBD_DESC_SERIAL_NUMBER_DEFINE(my_usbd_serial_number);

int main(void)
{
	int err = 0;

	err |= usbd_add_descriptor(&my_usbd, &my_usbd_lang);
	err |= usbd_add_descriptor(&my_usbd, &my_usbd_manufacturer);
	err |= usbd_add_descriptor(&my_usbd, &my_usbd_product);
	err |= usbd_add_descriptor(&my_usbd, &my_usbd_bos_cap_lpm);
	err |= usbd_add_descriptor(&my_usbd, &my_usbd_bos_cap_ss);
	//err |= usbd_add_descriptor(&my_usbd, &my_usbd_serial_number);
	err |= usbd_add_configuration(&my_usbd, USBD_SPEED_SS, &my_usbd_config);
	__ASSERT_NO_MSG(err == 0);

	usbd_device_set_code_triple(&my_usbd, USBD_SPEED_SS, USB_BCC_MISCELLANEOUS, 0x02, 0x01);

	err |= usbd_register_all_classes(&my_usbd, USBD_SPEED_SS, 1);
	__ASSERT_NO_MSG(err == 0);

	err |= usbd_init(&my_usbd);
	__ASSERT_NO_MSG(err == 0);

	err |= usbd_enable(&my_usbd);
	__ASSERT_NO_MSG(err == 0);

	while (true) {
		usb23_irq_handler(UDC0);
		k_sleep(K_MSEC(10));
	}
	return 0;
}

static int cmd_example123_subcommand1(const struct shell *sh, size_t argc, char **argv)
{
	uint8_t addr = 0x10;
	uint8_t reg = 0x41;
	uint8_t val = 0xff;
	int ret;

	ret = i2c_reg_write_byte(I2C0, addr, reg, val);
	if (ret < 0) {
		shell_error(sh, "write failed: addr=0x%02x reg=0x%02x val=0x%02x", addr, reg, val);
		return ret;
	}
	shell_print(sh, "wrote to addr=0x%02x reg=0x%02x val=0x%02x", addr, reg, val);
	return 0;
}

static int cmd_example123_subcommand2(const struct shell *sh, size_t argc, char **argv)
{
	uint8_t addr = 0x10;
	const char wbuf[] = {0x00};
	char rbuf[10];
	int ret;

	ret = i2c_write_read(I2C0, addr, wbuf, sizeof(wbuf), rbuf, sizeof(rbuf));
	if (ret < 0) {
		shell_error(sh, "read failed: addr=0x%02x reg=0x%02x size=%d", addr, wbuf[0],
			    sizeof(rbuf));
		return ret;
	}

	shell_hexdump(sh, rbuf, sizeof(rbuf));
	return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_example123,
			       SHELL_CMD(subcommand1, NULL, "First example command",
					 &cmd_example123_subcommand1),
			       SHELL_CMD(subcommand2, NULL, "Second example command",
					 &cmd_example123_subcommand2),
			       SHELL_SUBCMD_SET_END);
SHELL_CMD_REGISTER(example123, &sub_example123, "Demo commands", NULL);
