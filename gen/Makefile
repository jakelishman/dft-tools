CFLAGS+=-Wall -Wextra -std=gnu99
LDFLAGS=-lm

OBJS=utils.o generator.o
GENS=fcc-af fcc-f fcc-as-bct fcc-nonmag cscl-nonmag bct-af

all: $(GENS)

clean:
	rm -f $(GENS) $(OBJS)

utils.o: utils.c
	$(CC) $(CFLAGS) -c utils.c -o utils.o

generator.o: generator.c
	$(CC) $(CFLAGS) -c generator.c -o generator.o

$(GENS): $(@:=.c) $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(@:=.c) $(OBJS) -o $@
