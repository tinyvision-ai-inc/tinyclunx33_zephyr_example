#ifndef LATTICE_RX_DPHY_H
#define LATTICE_RX_DPHY_H

#include <zephyr/types.h>

#define LATTICE_RX_DPHY_BASE 0xb2000400

/* Register access types */
#define REG_ACCESS_RW  0
#define REG_ACCESS_R   1  
#define REG_ACCESS_W1C 2

/* Register descriptor structure */
struct reg_desc {
    const char *name;
    uint32_t addr;
    uint8_t access;
    const char *description;
};

/* Packet Parser Registers */
static const struct reg_desc lattice_rx_dphy_regs[] = {
/*    {
        .name = "LANE_SETTING",
        .addr = 0x0A,
        .access = REG_ACCESS_RW,
        .description = "Number of Lane Select"
    },
*/
    {
        .name = "VC_DT",
        .addr = 0x1F,
        .access = REG_ACCESS_R,
        .description = "Virtual channel and Data type register"
    },
    {
        .name = "WCL",
        .addr = 0x20,
        .access = REG_ACCESS_R,
        .description = "Word count low register"
    },
    {
        .name = "WCH", 
        .addr = 0x21,
        .access = REG_ACCESS_R,
        .description = "Word count high register"
    },
    {
        .name = "ECC",
        .addr = 0x22,
        .access = REG_ACCESS_R,
        .description = "ECC register / Extended virtual channel ID"
    },
    {
        .name = "VC_DT2",
        .addr = 0x23,
        .access = REG_ACCESS_R,
        .description = "Virtual channel 2 and Data type 2 register"
    },
    {
        .name = "WC2L",
        .addr = 0x24,
        .access = REG_ACCESS_R,
        .description = "Word count 2 low register"
    },
    {
        .name = "WC2H",
        .addr = 0x25,
        .access = REG_ACCESS_R,
        .description = "Word count 2 high register"
    },
    {
        .name = "ECC2",
        .addr = 0x26,
        .access = REG_ACCESS_R,
        .description = "ECC 2 register"
    },
    {
        .name = "REFDT",
        .addr = 0x27,
        .access = REG_ACCESS_RW,
        .description = "Reference data type"
    },
    {
        .name = "ERROR_STATUS",
        .addr = 0x28,
        .access = REG_ACCESS_W1C,
        .description = "ECC and CRC error status"
    },
    {
        .name = "ERROR_STATUS_EN",
        .addr = 0x29,
        .access = REG_ACCESS_RW,
        .description = "ECC and CRC error status enable"
    },
    {
        .name = "CRC_BYTE_LOW",
        .addr = 0x30,
        .access = REG_ACCESS_R,
        .description = "Received payload CRC LSB"
    },
    {
        .name = "CRC_BYTE_HIGH",
        .addr = 0x31,
        .access = REG_ACCESS_R,
        .description = "Received payload CRC MSB"
    },
    {
        .name = "ERROR_CTRL",
        .addr = 0x32,
        .access = REG_ACCESS_W1C,
        .description = "Hard D-PHY control error"
    },
    {
        .name = "ERROR_HS_SOT",
        .addr = 0x33,
        .access = REG_ACCESS_W1C,
        .description = "Hard D-PHY Start-of-Transmit error"
    },
    {
        .name = "ERROR_HS_SOT_SYNC",
        .addr = 0x34,
        .access = REG_ACCESS_W1C,
        .description = "Hard D-PHY Start-of-Transmit sync error"
    },
    {
        .name = "CONTROL",
        .addr = 0x35,
        .access = REG_ACCESS_RW,
        .description = "Parser Controls"
    },
    {
        .name = "NOCIL_DSETTLE",
        .addr = 0x36,
        .access = REG_ACCESS_RW,
        .description = "Data Settle register"
    },
    {
        .name = "NOCIL_RXFIFODEL_LSB",
        .addr = 0x37,
        .access = REG_ACCESS_RW,
        .description = "RX_FIFO read delay LSB register"
    },
    {
        .name = "NOCIL_RXFIFODEL_MSB",
        .addr = 0x38,
        .access = REG_ACCESS_RW,
        .description = "RX_FIFO read delay MSB register"
    },
    {
        .name = "ERROR_SOT_SYNC_DET",
        .addr = 0x39,
        .access = REG_ACCESS_W1C,
        .description = "Soft D-PHY SOT sync detect error"
    }
};


#endif /* LATTICE_RX_DPHY_H */
