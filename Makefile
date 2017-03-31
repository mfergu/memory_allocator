CC=clang
CFLAGS=-std=c11 -Weverything -g

BINS=libmyalloc.so 

all: $(BINS)

#myalloc.o: myalloc.c wrap_lib.o
#	$(CC) $(CFLAGS) -g -fPIC -c -o myalloc.o myalloc.c wrap_lib.o
#wrap_lib.o: wrap_lib.c
#	$(CC) $(CFLAGS)  -fPIC -shared -g -c  wrap_lib.o wrap_lib.c -ldl

	
libmyalloc.so:  myalloc.c
	$(CC) $(CFLAGS) -fPIC -shared -o libmyalloc.so myalloc.c -ldl

clean:
	rm $(BINS) *.o


