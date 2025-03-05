#include <zephyr/shell/shell.h>
#include "lattice_rx_dphy.h"

LOG_MODULE_REGISTER(lattice_rx_dphy, LOG_LEVEL_DBG);

static int cmd_mipi_dump_regs(const struct shell *sh, size_t argc, char **argv)
{
    if (argc != 1) {
        shell_error(sh, "Usage: mipi dump_regs (display MIPI CSI-2 registers)");
        return -EINVAL;
    }

    volatile uint32_t *mipi_base = (volatile uint32_t *)0xb2000400;
 
    volatile uint32_t reg_val;
    for (size_t i = 0; i < ARRAY_SIZE(lattice_rx_dphy_regs); i++) {
        reg_val = mipi_base[lattice_rx_dphy_regs[i].addr];
        shell_print(sh, "%-20s %02x: %08x",
                   lattice_rx_dphy_regs[i].name,
                   lattice_rx_dphy_regs[i].addr,
                   reg_val);
    }
    return 0;
}

static int cmd_mipi_init_regs(const struct shell *sh, size_t argc, char **argv)
{
    if (argc != 1) {
        shell_error(sh, "Usage: mipi init_regs (initialize MIPI CSI-2 registers)");
        return -EINVAL;
    }

    volatile uint32_t *mipi_base = (volatile uint32_t *)0xb2000400;
    // Setup the REF_DT to RAW8
    mipi_base[0x27] = 0x2A;
    // Set the settle time
    mipi_base[0x36] = 0x6;
    return 0;   
}

static int cmd_mipi_clear_errors(const struct shell *sh, size_t argc, char **argv)
{
    if (argc != 1) {
        shell_error(sh, "Usage: mipi clear (clear MIPI error registers)");
        return -EINVAL;
    }

    volatile uint32_t *mipi_base = (volatile uint32_t *)0xb2000400;
    
    // Clear error registers by writing 1 to them
    mipi_base[0x28] = 0xFF;
    mipi_base[0x32] = 0xFF;
    mipi_base[0x33] = 0xFF;
    mipi_base[0x34] = 0xFF;
    mipi_base[0x39] = 0xFF;
    
    shell_print(sh, "MIPI error registers cleared");
    return 0;
}

static int cmd_mipi_settle(const struct shell *sh, size_t argc, char **argv)
{
    if (argc != 2) {
        shell_error(sh, "Usage: mipi cil <+/-> (increment or decrement CIL)");
        return -EINVAL;
    }

    volatile uint32_t *mipi_base = (volatile uint32_t *)0xb2000400;
    uint32_t current_settle = mipi_base[0x36];
    uint32_t new_settle;

    if (argv[1][0] == '+') {
        new_settle = current_settle + 1;
    } else if (argv[1][0] == '-') {
        new_settle = current_settle - 1;
    } else {
        shell_error(sh, "Invalid argument - must be + or -");
        return -EINVAL;
    }

    mipi_base[0x36] = new_settle;
    shell_print(sh, "Settle register changed from 0x%x to 0x%x", current_settle, new_settle);
    return 0;
}

static int cmd_mipi_nocil_rxfifodel(const struct shell *sh, size_t argc, char **argv)
{
    if (argc != 2) {
        shell_error(sh, "Usage: mipi cil <+/-> (increment or decrement NOCIL_RXFIFODEL_LSB)");
        return -EINVAL;
    }

    volatile uint32_t *mipi_base = (volatile uint32_t *)0xb2000400;
    uint32_t current_delay = mipi_base[0x37];
    uint32_t new_delay;

    if (argv[1][0] == '+') {
        new_delay = current_delay + 1;
    } else if (argv[1][0] == '-') {
        new_delay = current_delay - 1;
    } else {
        shell_error(sh, "Invalid argument - must be + or -");
        return -EINVAL;
    }

    mipi_base[0x37] = new_delay;
    shell_print(sh, "NOCIL_RXFIFODEL_LSB register changed from 0x%x to 0x%x", current_delay, new_delay);
    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(mipi_cmds,
    SHELL_CMD(clear, NULL, "Clear MIPI CSI-2 error registers", cmd_mipi_clear_errors),
    SHELL_CMD(dump, NULL, "Display MIPI CSI-2 registers", cmd_mipi_dump_regs),
    SHELL_CMD(init, NULL, "Initialize MIPI CSI-2 registers", cmd_mipi_init_regs),
    SHELL_CMD(settle, NULL, "Increment or decrement Settle CYCLE register", cmd_mipi_settle),
    SHELL_CMD(delay, NULL, "Increment or decrement RXFIFODEL_LSB register", cmd_mipi_nocil_rxfifodel),
    SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(mipi, &mipi_cmds, "MIPI CSI-2 commands", NULL);