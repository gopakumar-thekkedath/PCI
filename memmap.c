#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>

/*
 * MCFG_START should ideally be obtained by parsing
 * ACPI tables.
 */
#define MCFG_START	0x92ff5000UL 

enum mmcfg {
    BUS_SHIFT  = 20,
    DEV_SHIFT  = 15,
    FUNC_SHIFT = 12,
};

static int fd;
static uint64_t mcfg_paddr;
static void *mcfg_vaddr;
static void *mcfgtab_vaddr;

struct sdth {
    uint8_t sig[4];
    uint32_t len;
    uint8_t rev;
    uint8_t chksum;
    uint8_t oemid[6];
    uint64_t oem_tabid;
    uint32_t oem_rev;
    uint32_t creator_id;
    uint32_t creator_rev;
}__attribute__((packed));


/* PCI MCFG table */
struct mcfg {
    struct sdth sdth;
    uint8_t reserved[8];
}__attribute__((packed));

/*PCI Config Space Base Address Allocation Structure */
struct pci_config_struct {
    uint64_t base_addr;
    uint16_t segment;
    uint8_t  sbus;
    uint8_t  ebus;
    uint8_t  reserved[4];
}__attribute__((packed));

int
device_map(uint8_t bus, uint8_t dev, uint8_t fun, void **addr)
{
	uint64_t tmp;

	mcfg_vaddr = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 
					 mcfg_paddr | (bus << BUS_SHIFT) | (dev << DEV_SHIFT) | 
					 (fun << FUNC_SHIFT));
	if (mcfg_vaddr == MAP_FAILED) {
		printf("\nFailed to mmap PCI MCFG Region:%lx", mcfg_paddr);
		return -1;
	}

	*addr = mcfg_vaddr;

	return 0;
}

int
mmap_init()
{
	struct mcfg *mcfg;
	struct pci_config_struct *pconfig;
	fd = open("/dev/mem", O_RDWR);

	if (!fd) {
		printf("\nError: Unable to open /dev/mem");
		return -1;
	}

	mcfgtab_vaddr = mmap(NULL, 4096, PROT_READ, MAP_SHARED, fd, MCFG_START);

	if (mcfgtab_vaddr == MAP_FAILED) {
		printf("\nFailed to mmap physical address:%lx", MCFG_START);
		return -1;
	}

	mcfg = (struct mcfg*)mcfgtab_vaddr;
	pconfig = (struct pci_config_struct *)(mcfg + 1);

	mcfg_paddr = pconfig->base_addr;

	return 0;
}

void
mmap_exit(void)
{
	if (mcfgtab_vaddr)
		munmap(mcfgtab_vaddr, 4096);
	if (mcfg_vaddr) 
		munmap(mcfg_vaddr, 4096);
	close(fd);
}
