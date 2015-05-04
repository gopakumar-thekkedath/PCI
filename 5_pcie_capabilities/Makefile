all: pcie_cap 

pcie_cap: pcie_cap.o ../memmap.o 
	gcc -o pcie_cap pcie_cap.o ../memmap.o

clean:
	rm -f pcie_cap *.o ../*.o 
