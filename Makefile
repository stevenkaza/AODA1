cC = gcc
CFLAGS = -Wall -std=c99 -pedantic -g
LIB  = -L. -lpython3.1
 
all:  test vcftool
#Done in Thornboro
vcftool:	vcftool.c vcutil.c vcftool.o vcutil.o
	$(CC) $(LIB)	vcftool.o vcutil.o -o vcftool   $(LIBS)
 
 
test:
	gcc -c vcutil.c -o vcutil.o -I/usr/include/python3.1 -L. -lpython3.1 -fPIC
	gcc -shared vcutil.o -o Vcf.so
	chmod +x xvcf.py
	
 
 

















clean: 
	rm -f *.o executable core*


