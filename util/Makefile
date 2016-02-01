CC=gcc-4.9
CFLAGS=-Wall -Wextra -std=gnu99
LDFLAGS=-lm

BINS=converge-check difference ferh-cell-compare half-column minima

all: $(BINS)

clean:
	rm -f $(BINS)

$(BINS): $(@:=.c)
	$(CC) $(CFLAGS) $(@:=.c) -o $@ $(LDFLAGS)
