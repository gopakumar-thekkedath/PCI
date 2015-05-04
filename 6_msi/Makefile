all: msi 

msi: msi.o ../memmap.o 
	gcc -o msi msi.o ../memmap.o

clean:
	rm -f msi *.o ../*.o 
