CC=gcc
CFLAGS=-g

SRCDIR=src
INCLDIR=include
LIBDIR=lib


master: $(SRCDIR)/master.c $(LIBDIR)/utils.o child
	$(CC) $(CFLAGS) -I$(INCLDIR) $(LIBDIR)/utils.o $(SRCDIR)/master.c -o master

child: $(SRCDIR)/child.c $(LIBDIR)/utils.o
	$(CC) $(CFLAGS) -I$(INCLDIR) $(LIBDIR)/utils.o $(SRCDIR)/child.c -o child

$(LIBDIR)/utils.o: $(SRCDIR)/utils.c
	$(CC) $(CFLAGS) -I$(INCLDIR) -c $(SRCDIR)/utils.c -o $(LIBDIR)/utils.o

.PHONY: clean run run1 run2 run3 run4 run5 run6 run7 run8

run: 
	rm -f ./output/*
	make run1 > ./output/run1.out
	make run2 > ./output/run2.out
	make run3 > ./output/run3.out
	make run4 > ./output/run4.out
	make run5 > ./output/run5.out
	make run6 > ./output/run6.out
	make run7 > ./output/run7.out
	make run8 > ./output/run8.out
	

run1:
	./master input/ip1 Imagine

run2:
	./master input/ip2 I

run3:
	./master input/ip3 me

run4:
	./master input/ip4 they

run5:
	./master input/ip5 us

run6:
	./master input/ip6 just

run7:
	./master input/ip7 word
	
run8:
	./master input/ip8 word

clean:
	rm master
	rm child
	rm $(LIBDIR)/utils.o
