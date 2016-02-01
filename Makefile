BACKUP_DIR=~/sprkkr-backup/

all:
	cd gen; make all
	cd util; make all

clean:
	cd gen; make clean
	cd util; make clean

backup: backup-dir
	mv *-backup_*.tar.gz $(BACKUP_DIR)

backup-dir:
	mkdir -p $(BACKUP_DIR)
