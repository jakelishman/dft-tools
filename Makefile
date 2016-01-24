CC=gcc-4.9
CFLAGS=-Wall -Wextra -std=gnu99
LDFLAGS=-lm

BINS= minima ferh-cell-compare half
BACKUP_DIR=~/sprkkr-backup/

all: $(BINS)

clean:
	rm $(BINS)

backup: | backup-dir
	mv *-backup_*.tar.gz $(BACKUP_DIR)

backup-dir:
	mkdir -p $(BACKUP_DIR)
