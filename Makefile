CC = gcc 
CFLAGS = -Wall -std=c99 -pedantic -g 
LIB  = -L. -lmylib 

all:	vcftool test
#Done in Thornboro
vcftool:	vcftool.c  -I/usr/include/python3.1 -fPIC -c vcutil.c vcftool.o vcutil.o 
	$(CC)	-I/usr/include/python3.1 -fPIC vcftool.o vcutil.o -o vcftool	$(LIBS) 


test: 
	gcc  vcutil.c 
	gcc -shared vcutil.o -o vcutil.so
	chmod +x gui.py
	./gui.py




clean: 
	rm -f *.o executable core*


