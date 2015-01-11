all:	pc

pc:	pc.c
	gcc -Wall -std=c11 -o pc $^
