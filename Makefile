CC = gcc 
CFLAGS = -Wall -std=c99 -pedantic -g 
LIB  = -L. -lmylib 

all:	vcftool test
#Done in Thornboro
vcftool:	vcftool.c vcutil.c vcftool.o vcutil.o 
	$(CC)	vcftool.o vcutil.o -o vcftool	$(LIBS) 


test: 
	gcc -I/usr/include/python3.1 -fPIC -c vcutil.c 
	gcc -shared vcutil.o -o vcutil.so
	chmod +x gui.py
	./gui.py




clean: 
	rm -f *.o executable core*


