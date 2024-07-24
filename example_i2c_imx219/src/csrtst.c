#include <stdio.h>
#include <zephyr/shell/shell.h>

#define WISHBONE_BASE_ADDR		0xb0000000


static void reg_32b_read(unsigned int addr, unsigned int *val)
{
	*val = *((volatile unsigned int *)addr);
}

static void CsrRegTest(void) {
	unsigned int val, adr;

	// Read back a bunch of registers from the CSR
	adr = WISHBONE_BASE_ADDR + 0x02000000;
	for (int i = 0; i<=10; i++) {
		reg_32b_read(adr, &val);
		printf("0x%x = 0x%x\n\r", adr, val);
		adr += 4;
	}
}


SHELL_CMD_REGISTER(csr_test, NULL, "Dump CSR registers", CsrRegTest);