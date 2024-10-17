#include <zephyr/shell/shell.h>

#define WISHBONE_BASE_ADDR 0xb0000000

struct reg_data {
    unsigned int adr;
    unsigned int val;
    const char *name;
};

struct reg_data registers[] = {
    {WISHBONE_BASE_ADDR + 0x07000000, 0, "CSR_SCRATCH"},
    {WISHBONE_BASE_ADDR + 0x07000004, 0, "CSR_CONTROL"},
    {WISHBONE_BASE_ADDR + 0x07000008, 0, "CSR_STATUS"},
    {WISHBONE_BASE_ADDR + 0x0700000C, 0, "CSR_CONFIG"},
    {WISHBONE_BASE_ADDR + 0x07000010, 0, "CSR_NUM_FRAMES"},
    {WISHBONE_BASE_ADDR + 0x07000014, 0, "CSR_IMAGE_STATS"},
    {WISHBONE_BASE_ADDR + 0x07000018, 0, "CSR_IMAGE_GAIN"},
    {WISHBONE_BASE_ADDR + 0x0700001C, 0, "CSR_CHAN_AVG_0"},
    {WISHBONE_BASE_ADDR + 0x07000020, 0, "CSR_CHAN_AVG_1"},
    {WISHBONE_BASE_ADDR + 0x07000024, 0, "CSR_CHAN_AVG_2"},
    {WISHBONE_BASE_ADDR + 0x07000028, 0, "CSR_CHAN_AVG_3"}
};

static void reg_32b_read(unsigned int addr, unsigned int *val)
{
    *val = *((volatile unsigned int *)addr);
}

static void CsrRegTest(const struct shell *shell)
{

    int num_registers = sizeof(registers) / sizeof(registers[0]);

    for (int i = 0; i < num_registers; i++) {
        reg_32b_read(registers[i].adr, &registers[i].val);

        shell_print(shell,"%s\t (0x%x)\t = 0x%x", registers[i].name, registers[i].adr, registers[i].val);
    }
}

SHELL_CMD_REGISTER(csr_test, NULL, "Dump CSR registers", CsrRegTest);