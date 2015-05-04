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
get_pcie_devtype(uint32_t reg)
{
	uint8_t type = (uint8_t) ((reg >> 4) & 0xf);

	switch (type) {
		case ENDPOINT:
			printf("\n End point device");
			break;
		case LEGACY_ENDPOINT:
		case ROOT_PORT:
			printf("\n Root Port");
			break;
		case US_PORT:
		case DS_PORT:
		case PCIE_PCI_BRIDGE:
		case PCI_PCIE_BRIDGE:
			break;
		case RC_INTEGRATED_EP:
			printf("\n RC Integrated End Point");
			break;
		case RC_EVENT_COLLECTOR:
			break; 
	}
}

static void
max_payload(uint8_t val, char *str)
{
	switch (val) {
	case 0:
		printf("\n%s 128 bytes", str);
		break;
	case 1:
		printf("\n%s 256 bytes", str);
		break;
	case 2:
		printf("\n%s 512 bytes", str);
		break;
	case 3:
		printf("\n%s 1024 bytes", str);
		break;
	case 4:
		printf("\n%s 2048 bytes", str);
		break;
	case 5:
		printf("\n%s 4096 bytes", str);
		break;
	}
}

static void
device_status(uint16_t reg)
{
	if (reg & 1)
		printf("\n Correctable Error Detected");
	if (reg & 2)
		printf("\n Non-Fatal Error Detected");
	if (reg & 4)
		printf("\n Fatal Error Detected");
	if (reg & 8)
		printf("\n Unsupported Request Detected");
}

static void
device_control(uint16_t reg)
{
	max_payload((uint8_t)((reg >> 5) & 0x7), "Max Payload Set");
	max_payload((uint8_t)((reg >> 12) & 0x7), "Max Read Request");
	
	if (reg & 1)
		printf("\n Correctable Error Reporting Enabled");
	else
		printf("\n Correctable Error Reporting Disabled");
	
	if (reg & 2)
		printf("\n Non-Fatal Error Reporting Enabled");
	else
		printf("\n Non-Fatal Error Reporting Disabled");
	
	if (reg & 4)
		printf("\n Fatal Error Reporting Enabled");
	else
		printf("\n Fatal Error Reporting Disabled");

	/*
 	 * It is not recommended to set this bit for devices that do not
	 * implement role based error reporting as such devices would
	 * report uncorrectable error even for coniguration transactions.
	 */	
	if (reg & 8)
		printf("\n UR Request Reporting Enabled");
	else
		printf("\n UR Request Reporting Disabled");

	if (reg & 16)
		printf("\n Relaxed Ordering Enabled");
	else
		printf("\n Relaxed Ordering Disabled");
		
}


static void
scan_pcie_cap(void)
{
	uint8_t cap_start = *((volatile uint8_t *) (addr + CAP_PTR));
	if (cap_start) {
		cap_hdr_ptr = (struct cap_hdr_ptr *) (addr + cap_start);

		while (cap_hdr_ptr->cap_id) {
			if (cap_hdr_ptr->cap_id == PCIE_CAP) {
				pcie_cap = (struct pcie_cap *)cap_hdr_ptr;
				get_pcie_devtype(pcie_cap->pcie_caps_reg);
				max_payload((uint8_t)(pcie_cap->dev_cap & 0x7), "Max Payload Supported");
				device_control(pcie_cap->dev_ctrl);
				device_status(pcie_cap->dev_stat);
				break;
			}
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
			scan_pcie_cap();
		} else {
			printf("\n No supporot for new capabilities !!\n");
		}
	
	} else
		printf("\nFailed to map MCFG space for device\n");

	mmap_exit();	
	printf("\n");
	return 0;
}
