CFLAGS = -Wall

all: shell

shell: shell.o parse.o
	gcc -o shell shell.o parse.o ${CFLAGS}

shell.o: shell.c shell.h
	gcc -o shell.o shell.c shell.h parse.h ${CFLAGS}

parse.o: parse.c parse.h
	gcc -o parse.o parse.c parse.h ${CFLAGS}

clean:
	rm *.o shell

cleanall:
	rm *.o shell cscope.* TAGS

TAGS:
	find . -name "*.[chS]" | xargs etags -a

cscope:
	find . -name "*.[chS]" > cscope.files
	cscope -b -q -k
