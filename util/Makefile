CFLAGS+=-Wall -Wextra -std=gnu99
LDFLAGS=-lm

BINS=converge-check difference ferh-cell-compare half-column minima data-transform scf-join scf-moments agr-to-csv

all: $(BINS)

clean:
	rm -f $(BINS)

$(BINS): $(@:=.c)
	$(CC) $(CFLAGS) $(@:=.c) -o $@ $(LDFLAGS)
