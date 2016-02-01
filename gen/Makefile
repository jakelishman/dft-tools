CC=gcc-4.9
CFLAGS=-Wall -Wextra -std=gnu99
LDFLAGS=-lm

OBJS=utils.o generator.o

utils.o: utils.c
	$(CC) $(CFLAGS) -c utils.c -o utils.o

generator.o: generator.c
	$(CC) $(CFLAGS) -c generator.c -o generator.o

%: %.c $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $< $(OBJS) -o $@
