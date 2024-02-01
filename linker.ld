#include <zephyr/devicetree.h>
#include <zephyr/linker/linker-tool.h>

#define DMA_ORIGIN DT_REG_ADDR(DT_CHOSEN(lattice_usb23_dma))
#define DMA_LENGTH DT_REG_SIZE(DT_CHOSEN(lattice_usb23_dma))

MEMORY
{
	USB23_DMA (rw) : ORIGIN = DMA_ORIGIN, LENGTH = DMA_LENGTH
}

SECTIONS
{
	SECTION_PROLOGUE(usb23_dma,,)
	{
		/* All symbols with this name are meant to land into the memory
		 * accessible to USB23 so it can fetch this content. */
		. = ALIGN(16);
		*(.bss.usb23_dma_*);

		/* Temporary workaround for getting the USB packets filled by
		 * Zephyr land int the DMA-defined region */
		. = ALIGN(16);
		*(*.kheap_buf_net_buf_mem_pool_udc_ep_pool);
	} GROUP_LINK_IN(USB23_DMA)
}

#include <zephyr/arch/riscv/common/linker.ld>
