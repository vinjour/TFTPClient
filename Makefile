CC=gcc
FLAGS=-Wall  -g

all : gettftp puttftp

gettftp : 	gettftp.o
		$(CC) gettftp.o -o gettftp

gettftp.o :		gettftp.c gettftp.h
					$(CC) -c gettftp.c $(FLAGS) -o gettftp.o

puttftp : 	puttftp.o
		$(CC) puttftp.o -o puttftp

puttftp.o :		puttftp.c puttftp.h
					$(CC) -c puttftp.c $(FLAGS) -o puttftp.o

clean :
		rm -f *.o
		rm -f gettftp puttftp