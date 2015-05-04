all: secbus_reset 

secbus_reset: secbus_reset.o ../memmap.o 
	gcc -o secbus_reset secbus_reset.o ../memmap.o

clean:
	rm -f secbus_reset *.o ../*.o 
