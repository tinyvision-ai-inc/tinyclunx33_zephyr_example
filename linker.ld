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
		/* All symbols with this section are going to a memory region
		 * accessible to the USB23 core performing DMA requests */

		. = ALIGN(16);
		*(.bss.usb23_dma_*);
		*(.bss.uvc_payload_header_*);
	} GROUP_LINK_IN(USB23_DMA)
}

#include <zephyr/arch/riscv/common/linker.ld>
