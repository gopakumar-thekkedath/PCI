all: regread

regread: regread.o ../memmap.o 
	gcc -o regread regread.o ../memmap.o

clean:
	rm -f regread *.o ../*.o 
