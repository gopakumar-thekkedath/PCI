#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>

#include "../memmap.h"

int
main(int argc, char *argv[])
{

	volatile uint8_t *addr;
	uint8_t bus, dev, fun;
	uint16_t offset;

	if (argc != 5) {
		printf("\nInvalid argument count");
		printf("\n Usage: regread busnum devnum funnum regoffset");
		return -1;
	}

	bus = strtol(argv[1], NULL, 16);	
	dev = strtol(argv[2], NULL, 16);	
	fun = strtol(argv[3], NULL, 16);	
	offset = strtol(argv[4], NULL, 16);	
	
	mmap_init();
	if (device_map(bus, dev, fun, (void **)&addr) != -1) {
		printf("%08x\n", *((uint32_t *)(addr + offset)));
	} else
		printf("\nFailed to map MCFG space for device\n");

	mmap_exit();	

	return 0;
}
