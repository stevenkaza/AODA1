CC = gcc 
CFLAGS = -Wall -std=c99 -pedantic -g 
LIB  = -L. -lmylib 

all:	sanityTests  
#Done in Thornboro
sanityTests:	sanity.c vcutil.c sanity.o vcutil.o 
	$(CC)	sanity.o vcutil.o -o runTests	$(LIBS) 






clean: 
	rm -f *.o executable core*


