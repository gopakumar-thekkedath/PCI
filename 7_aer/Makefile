all: aer 

aer : aer.o ../memmap.o 
	gcc -o aer aer.o ../memmap.o

clean:
	rm -f aer *.o ../*.o 
