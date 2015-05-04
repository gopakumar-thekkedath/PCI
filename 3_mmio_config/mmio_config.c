/*
 * 00:1f.3
 * Disable/Enable MMIO access for a device.
 * After disabling, expect to get all 0xff on access to MMIO space
 * Device is expected to respond with UR?
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
	uint8_t bus, dev, fun, enable;

	if (argc != 5) {
		printf("\nInvalid argument count");
		printf("\n Usage: regread busnum devnum funnum regoffset 1/0 ");
		printf("\n Usage: last argument details, 1=enable, 0=disable\n");
		return -1;
	}

	bus = strtol(argv[1], NULL, 16);	
	dev = strtol(argv[2], NULL, 16);	
	fun = strtol(argv[3], NULL, 16);	
	enable = atoi(argv[4]);	
	mmap_init();
	if (device_map(bus, dev, fun, (void **)&addr) != -1) {
		uint32_t cmd_stat = *((volatile uint32_t *) (addr + CMD_STATUS));
		
		if (enable) {
			if (!(cmd_stat & MEM_SPACE)) {
				cmd_stat |= MEM_SPACE;
			}
		} else {
			if (cmd_stat & MEM_SPACE) {
				cmd_stat &= ~MEM_SPACE;
			}
		}
	
		*((volatile uint32_t *) (addr + CMD_STATUS)) = cmd_stat;
		printf("\nDevice configured");	
		cmd_stat = *((volatile uint32_t *) (addr + CMD_STATUS));
		printf("\n Command and status Reg :%08x\n", cmd_stat);
	} else
		printf("\nFailed to map MCFG space for device\n");

	mmap_exit();	

	return 0;
}
