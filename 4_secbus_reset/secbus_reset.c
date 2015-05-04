/*
 *00:1f.3
 * Secondary bus reset is done using the bridge control register. On setting
 * the 'secondary bus reset bit', the physical layter would transmit TS1 ordered
 * set with Hot Reset bit asserted
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>

#include "../memmap.h"
#include "../pci.h"

int
main(int argc, char *argv[])
{

	volatile uint8_t *addr;
	uint8_t bus, dev, fun, reset;

	if (argc != 5) {
		printf("\nInvalid argument count");
		printf("\n Usage: secbus_reset busnum devnum funnum regoffset 1/0 ");
		printf("\n Usage: last argument details, 1=assert reset, 0=deassert-reset\n");
		return -1;
	}

	bus = strtol(argv[1], NULL, 16);	
	dev = strtol(argv[2], NULL, 16);	
	fun = strtol(argv[3], NULL, 16);	
	reset = atoi(argv[4]);	
	mmap_init();
	if (device_map(bus, dev, fun, (void **)&addr) != -1) {
		uint16_t brdg_ctrl = *((volatile uint16_t *) (addr + BRDG_CTRL));
		
		if (reset) {
			if (!(brdg_ctrl & SECBUS_RESET)) {
				brdg_ctrl |= SECBUS_RESET;
			}
		} else {
			if (brdg_ctrl & SECBUS_RESET) {
				brdg_ctrl &= ~SECBUS_RESET;
			}
		}
	
		*((volatile uint32_t *) (addr + BRDG_CTRL)) = brdg_ctrl; 
		printf("\nDevice configured");	
		brdg_ctrl = *((volatile uint32_t *) (addr + BRDG_CTRL));
		printf("\n Bridge control :%04x\n", brdg_ctrl);
	} else
		printf("\nFailed to map MCFG space for device\n");

	mmap_exit();	

	return 0;
}
