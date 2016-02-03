CFLAGS+=-Wall -Wextra -std=gnu99
LDFLAGS=-lm

SEQS=vary-z vary-ratio const-volume

all: $(SEQS)

clean:
	rm -f $(SEQS)

%: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@
