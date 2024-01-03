#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>
#include <zephyr/drivers/usb/udc.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/usb/usbd.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

#include "si5351.h"

USBD_DEVICE_DEFINE(usbd_dev, DEVICE_DT_GET(DT_NODELABEL(zephyr_udc0)), 0x1209, 0x0001);
USBD_DEVICE_QUALIFIER_DEFINE(usbd_device_qualifier);
USBD_CONFIGURATION_DEFINE(usbd_config, USB_SCD_SELF_POWERED, 200);
USBD_DESC_LANG_DEFINE(usbd_lang);
//USBD_DESC_MANUFACTURER_DEFINE(usbd_manufacturer, "tinyVision.ai");
USBD_DESC_PRODUCT_DEFINE(usbd_product, "-> tinyCLUNX33 <-");
//USBD_DESC_SERIAL_NUMBER_DEFINE(usbd_serial_number, "0123456789ABCDEF");

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

	uint8_t reg_model_lsb = 0;
	uint8_t reg_model_msb = 0;

	err = usbd_add_descriptor(&usbd_dev, &usbd_device_qualifier);
	err |= usbd_add_descriptor(&usbd_dev, &usbd_lang);
	//err |= usbd_add_descriptor(&usbd_dev, &usbd_manufacturer);
	err |= usbd_add_descriptor(&usbd_dev, &usbd_product);
	//err |= usbd_add_descriptor(&usbd_dev, &usbd_serial_number);
	err |= usbd_add_configuration(&usbd_dev, &usbd_config);
	err |= usbd_register_class(&usbd_dev, "cdc_acm_0", 1);
	__ASSERT_NO_MSG(err == 0);

	__ASSERT_NO_MSG(err == 0);
	err = usbd_init(&usbd_dev);

	err = usbd_enable(&usbd_dev);
	__ASSERT_NO_MSG(err == 0);

	/* As soon as interrupts are enabled in the hardware architecture, this
	 * can be removed */
	while (1) {
		k_sleep(K_MSEC(10));
		usb23_irq_handler(DEVICE_DT_GET(DT_NODELABEL(zephyr_udc0)));
	}

	return 0;
}
