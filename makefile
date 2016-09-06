all: main

arp.o : arp.c arp.h
	gcc -c arp.c -o arp.o
main.o: main.c arp.h
	gcc -c main.c -o main.o
main: main.o arp.o
	gcc  main.o arp.o -o main -lrt
	rm -f *.o
