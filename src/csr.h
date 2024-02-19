/*
 * Access routines for the Control and State Registers block
 *
 * Copyright (c) 2024 Emcraft Systems
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#ifndef __CSR_H__
#define __CSR_H__

/* CSR - Control & Status Registers block */
#define CSR_OFFSET	0xB2000000

/* CSR registers */
#define CSR_REG_REV1	0x70
#define CSR_REG_REV2	0x74

static inline uint32_t csr_read_reg(uint32_t reg) {
	char * p = (char *)CSR_OFFSET ;
	return *(uint32_t *)(p + reg);
}

#endif /* __CSR_H__ */
