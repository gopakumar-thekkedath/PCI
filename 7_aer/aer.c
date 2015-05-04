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

static volatile uint8_t *addr;
static struct pcie_cap *pcie_cap;
static struct ext_cap_hdr *ext_cap_hdr_ptr;

static void
scan_pcie_ext_cap(void)
{
	uint16_t cap_start = 256;
	ext_cap_hdr_ptr = (struct ext_cap_hdr_ptr *) (addr + cap_start);

	while (1) {
		if (ext_cap_hdr_ptr->cap_id == AER_CAP) {
			printf("\n AER Capabilities present");
			struct aer_cap *aer_cap = (struct pcie_cap *)ext_cap_hdr_ptr;
			break;
		}
		if (!(ext_cap_hdr_ptr->offset_ver >> 4))
			break; 
		ext_cap_hdr_ptr = (struct cap_hdr_ptr *)(addr + (ext_cap_hdr_ptr->offset_ver >> 4));
	}

}

int
main(int argc, char *argv[])
{

	uint8_t bus, dev, fun;

	if (argc != 4) {
		printf("\nInvalid argument count");
		printf("\n Usage: pcie_cap busnum devnum funnum regoffset");
		return -1;
	}

	bus = strtol(argv[1], NULL, 16);	
	dev = strtol(argv[2], NULL, 16);	
	fun = strtol(argv[3], NULL, 16);

	printf("\n bus %x dev %x fun %x", bus, dev, fun);
	mmap_init();
	if (device_map(bus, dev, fun, (void **)&addr) != -1) {
		uint32_t cmd_stat = *((volatile uint32_t *) (addr + CMD_STATUS));
		
		if ((cmd_stat >> 16) & CAP_LIST) {
			scan_pcie_ext_cap();
		} else {
			printf("\n No supporot for new capabilities !!\n");
		}
	
	} else
		printf("\nFailed to map MCFG space for device\n");

	mmap_exit();	
	printf("\n");
	return 0;
}
