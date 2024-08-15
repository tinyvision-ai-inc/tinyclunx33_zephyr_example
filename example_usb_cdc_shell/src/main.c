#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/shell/shell.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(app, LOG_LEVEL_DBG);

static const struct device *const i2c0_dev = DEVICE_DT_GET(DT_NODELABEL(i2c0));

int main(void)
{
	return 0;
}

static int cmd_example123_subcommand1(const struct shell *sh, size_t argc, char **argv)
{
	uint8_t addr = 0x10;
	uint8_t reg = 0x41;
	uint8_t val = 0xff;
	int ret;

	ret = i2c_reg_write_byte(i2c0_dev, addr, reg, val);
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

	ret = i2c_write_read(i2c0_dev, addr, wbuf, sizeof(wbuf), rbuf, sizeof(rbuf));
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
