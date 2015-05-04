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
static struct cap_hdr *cap_hdr_ptr;

static void
msg_decode (uint8_t msg, char *str)
{
	switch (msg) {
	case 0:
		printf("\n1 Msg %s", str);
		break; 
	case 1:
		printf("\n2 Msg %s", str);
		break; 
	case 2:
		printf("\n4 Msg %s", str);
		break; 
	case 3:
		printf("\n8 Msg %s", str);
		break; 
	case 4:
		printf("\n16 Msg %s", str);
		break; 
	case 5:
		printf("\n32 Msg %s", str);
		break; 
	}
}

static void
decode_msiaddr(uint32_t addr)
{
	printf("\nAPIC ID:%x", (addr >> 12) & 0xff);
	printf("\nRedirection Hint :%u", (addr >> 3) & 1);
	if (addr & (1 << 2))
		printf("\n Logical Destination Mode");
	else
		printf("\n Physical Destination Mode");
}

static void
decode_msidata(uint16_t data)
{
	uint8_t mode = (data >> 8) & 7;

	if (data & (1 << 15))
		printf("\n Level Triggered");
	else
		printf("\n Edge Triggered");

	switch (mode) {
	case 0:
		printf("\n Fixed");
		break;
	case 1:
		printf("\n Lowest Prio");
		break;
	default:
		printf("\n Unusaul");
	}

	printf("\n Interrupt Vector:%u", data & 0xff);
}

static void
scan_msi_cap(void)
{
	uint8_t cap_start = *((volatile uint8_t *) (addr + CAP_PTR));
	if (cap_start) {
		cap_hdr_ptr = (struct cap_hdr_ptr *) (addr + cap_start);
		
		while (cap_hdr_ptr->cap_id) {
			if (cap_hdr_ptr->cap_id == MSI_CAP) {
				struct msi_cap_common *common = (struct msi_cap_common *)cap_hdr_ptr;
				printf("\nDetetced MSI Cap\n");
				if (common->msg_ctrl & (1 << 7)) {
					struct msi_cap64 *msi_64 = (struct msi_cap64 *)common;
					uint64_t addr = (uint64_t)msi_64->msg_addr_low | 
									 ((uint64_t)msi_64->msg_addr_high) << 32;
					uint16_t data = msi_64->msg_data;
					printf("\n 64 bit cap");
					printf("\nMsg Addr:%lx Msg Data:%x", addr, data);
					msg_decode((common->msg_ctrl >> 1) & 7, "Supported by Device");
					msg_decode((common->msg_ctrl >> 4) & 7, "Allocated to Device");
					decode_msiaddr((uint32_t)addr);
					decode_msidata(data);	
				} else {
					struct msi_cap32 *msi_32 = (struct msi_cap32 *)common;
					uint32_t addr = msi_32->msg_addr;
					uint16_t data = msi_32->msg_data;
					printf("\n 32 bit cap");
					printf("\nMsg Addr:%x Msg Data:%x", addr, data);
					decode_msiaddr(addr);
					decode_msidata(data);	
				}
				break;
			}
			if (!cap_hdr_ptr->next_ptr)
				break;
			cap_hdr_ptr = (struct cap_hdr_ptr *)(addr + cap_hdr_ptr->next_ptr);
		}

	}
}

int
main(int argc, char *argv[])
{

	uint8_t bus, dev, fun;

	if (argc != 4) {
		printf("\nInvalid argument count");
		printf("\n Usage: msi busnum devnum funnum");
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
			scan_msi_cap();
		} else {
			printf("\n No supporot for new capabilities !!\n");
		}
	
	} else
		printf("\nFailed to map MCFG space for device\n");

	mmap_exit();	
	printf("\n");
	return 0;
}
