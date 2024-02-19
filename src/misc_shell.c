/*
 * Miscellaneous shell commands
 *
 * Copyright (c) 2024 Emcraft Systems
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#include <zephyr/shell/shell.h>
#include <app_version.h>
#include "csr.h"

typedef struct fpga_rev1 {
	uint8_t patch;
	uint8_t minor;
	uint8_t major;
	uint8_t reserved;
} fpga_rev1_t;

typedef struct fpga_rev2 {
	uint16_t build;
	uint16_t reserved;
} fpga_rev2_t;

static int cmd_fw_version(const struct shell *sh, size_t argc, char **argv)
{
	uint32_t rev1 = 0;
	uint32_t rev2 = 0;
	char * str = APP_VERSION_STRING;

	rev1 = csr_read_reg(CSR_REG_REV1);
	rev2 = csr_read_reg(CSR_REG_REV2);

	shell_print(sh, "Firmware version: %s", str);
	shell_print(sh, "FPGA version: %d.%d.%d-%d",
			((fpga_rev1_t *)&rev1)->major,
			((fpga_rev1_t *)&rev1)->minor,
			((fpga_rev1_t *)&rev1)->patch,
			((fpga_rev2_t *)&rev2)->build);

	return 0;
}

SHELL_CMD_REGISTER(fw_version, NULL, "Print firmware version",
		cmd_fw_version);
