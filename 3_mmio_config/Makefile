all: mmio_config 

mmio_config: mmio_config.o ../memmap.o 
	gcc -o mmio_config mmio_config.o ../memmap.o

clean:
	rm -f mmio_config *.o ../*.o 
