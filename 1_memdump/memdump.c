#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>

int
main(int argc, char *argv[])
{

	volatile uint32_t *addr;
	uint64_t paddr;
	int fd, i;

	if (argc != 2) {
		printf("\nInvalid argument count");
		printf("\n Usage: memdump physical_address");
		return -1;
	}

	paddr = strtol(argv[1], NULL, 16);

	if (!paddr)
		return -1;

	fd = open("/dev/mem", O_RDWR);

	 if (!fd) {
        printf("\nError: Unable to open /dev/mem");
        return -1;
    }

	printf("\n mmap physical address :%lx", paddr);
	addr =  mmap(NULL, 4096, PROT_READ, MAP_SHARED, fd, paddr);	

	 if (addr == MAP_FAILED) {
        printf("\nFailed to mmap physical address:%lx", paddr);
        return -1;
    }

	for (i = 0; i < 4; i++) {
		printf("\n%08x\t%08x\t%08x\t%08x\t", *addr, *(addr + 1), *(addr + 2),
			   *(addr + 3));
		addr += 4;
	}	

	printf("\n");
	close(fd);
	return 0;
}
