CC = gcc
CFLAGS = -I./lib -g -O2 -D_REENTRANT -Wall
LIBS = ./lib/libunp.a -lpthread
CLEANFILES = core core.* *.core *.o temp.* *.out typescript* \
		*.lc *.lh *.bsdi *.sparc *.uw
PROGS =	cl sv

all: ${PROGS}

client:	cl.c
	${CC} ${CFLAGS} -o $@ cl.c ${LIBS}
server: sv.c ./lib/huffman.c
	${CC} ${CFLAGS} -o $@ sv.c ./lib/list.c ./lib/huffman.c ./lib/sigchldwaitpid.o ${LIBS}
del:
	rm clientDB/*.txt clientDB/*.zip serverDB/*.txt serverDB/*.zip
