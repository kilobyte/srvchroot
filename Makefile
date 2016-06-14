all:	srvchroot

srvchroot:	srvchroot.c
	gcc -Wall -std=c11 -o srvchroot $^

clean:
	rm -f srvchroot *.o
