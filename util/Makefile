CFLAGS+=-Wall -Wextra -std=gnu99
LDFLAGS=-lm

BINS=converge-check difference ferh-cell-compare half-column minima data-transform

all: $(BINS)

clean:
	rm -f $(BINS)

$(BINS): $(@:=.c)
	$(CC) $(CFLAGS) $(@:=.c) -o $@ $(LDFLAGS)
