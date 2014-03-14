CC = gcc 
CFLAGS = -Wall -std=c99 -pedantic -g 
LIB  = -L. -lpython3.1 

all:  Vcf.so vcftool
#Done in Thornboro
vcftool:   vcftool.o vcutil.o 
	$(CC) $(LIB)	vcftool.o vcutil.o -o vcftool	$(LIBS) 
vcftool.o:  vcftool.c vcftool.h
	$(CC) $(CFLAGS) -c  $< -o $@

vcutil.o:   vcutil.c vcutil.h
	$(CC) $(CFLAGS) -c $< -o $@



Vcf.so: VcfPython.o 
	gcc -shared VcfPython.o -o Vcf.so

VcfPython.o: vcutil.c vcutil.h
	$(CC) $(CFLAGS)  -I/usr/include/python3.1 -c -fPIC $< -o $@ -D _pyDef 

