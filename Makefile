CC = gcc 
CFLAGS = -Wall -std=c99 -pedantic -g 
LIB  = -L. -lmylib 

all:	vcftool  
#Done in Thornboro
vcftool:	vcftool.c vcutil.c vcftool.o vcutil.o 
	$(CC)	vcftool.o vcutil.o -o vcftool	$(LIBS) 






clean: 
	rm -f *.o executable core*


