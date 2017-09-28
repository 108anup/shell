CFLAGS = -Wall -lreadline

all: shell

shell: shell.o parse.o
	gcc -o shell shell.o parse.o ${CFLAGS}

shell.o: shell.c shell.h parse.h
	gcc -c shell.c ${CFLAGS}

parse.o: parse.c parse.h
	gcc -c parse.c ${CFLAGS}

clean:
	rm *.o shell

cleanall:
	rm *.o shell cscope.* TAGS

TAGS:
	find . -name "*.[chS]" | xargs etags -a

cscope:
	find . -name "*.[chS]" > cscope.files
	cscope -b -q -k
