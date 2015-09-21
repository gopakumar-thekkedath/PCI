#include <linux/init.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/interrupt.h>

#include <asm/io.h>
#include <asm/irq.h>
#include <asm/uaccess.h>

#define TEST_VID 0x8086
#define TEST_DID 0x9c3a

#define DRV_NAME "Test SysTable Driver"

static void
display_table_details(uint8_t *mem)
{
    printk(KERN_ALERT "\nBase =%lx", *((uint64_t *)(mem + 2)));
    printk(KERN_ALERT "\nSize =%lx\n", *((uint16_t *)mem));
}

static int
test_init(void)
{
	uint8_t mem[10];
	uint64_t reg;

	printk(KERN_ALERT "\n===GDT===");
    asm volatile ("sgdt %0" : "=m"(mem) : : "memory");
    display_table_details(mem);
	
	printk(KERN_ALERT "\n===IDT===");
    asm volatile ("sidt %0" : "=m"(mem) : : "memory");
    display_table_details(mem);
    
    asm volatile("mov %%cr3, %0" :"=q"(reg) : :);
	
	printk(KERN_ALERT "\n===CR3===");
    printk(KERN_ALERT "\nPaging Structure Address=%lx", reg);

	return 0;
}

static void
test_exit(void)
{
}


module_init(test_init);
module_exit(test_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("xyz");
MODULE_DESCRIPTION("Test SysTable Driver");
