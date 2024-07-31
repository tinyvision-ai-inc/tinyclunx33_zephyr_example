#include <stdio.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/shell/shell.h>
#define MUX_ADDR 0x71

static void sel_cam(void)
{

	const struct device *mux_dev = DEVICE_DT_GET(DT_NODELABEL(i2c0));
	if (mux_dev == NULL || !device_is_ready(mux_dev)) {
		printf("Could not get I2C mux device\n");
		return;
	}

	printf("Selecting camera \n\r");
	uint8_t val = 0x4;
	int ret = i2c_reg_write_byte(mux_dev, MUX_ADDR, 0, val);
	if (ret) {
		printf("Unable write to Cam. (err %i)\n", ret);
		return;
	}
	printf("Cam 0 selected\n");
}

SHELL_STATIC_SUBCMD_SET_CREATE(cam_sel_sub, SHELL_CMD(sel_cam, NULL, "Select camera 10", &sel_cam),
			       SHELL_SUBCMD_SET_END);

SHELL_CMD_REGISTER(cam_sel, &cam_sel_sub, "camera selector command", NULL);