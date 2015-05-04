#ifndef _MEMMAP_H_
#define _MEMMAP_H_
extern int mmap_init();
extern int device_map(uint8_t, uint8_t, uint8_t, void **);
extern void mmap_exit();
#endif
