all : tests

tests : mytest simpletest
	mytest simpletest

mytest : mytest.o smalloc.o testhelpers.o
	gcc -Wall -g -o mytest mytest.o smalloc.o testhelpers.o

simpletest : simpletest.o smalloc.o testhelpers.o
	gcc -Wall -g -o simpletest simpletest.o smalloc.o testhelpers.o
	
%.o : %.c smalloc.h
	gcc -Wall -g -c $<

clean : 
	rm mytest simpletest *.o
