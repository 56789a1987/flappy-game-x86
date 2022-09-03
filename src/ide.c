#include "common.h"
#include "utils.h"
#include "ide.h"

void ata_handler_cb() {
}

void ata_io_wait() {
	inb(0x1fc);
	inb(0x1fc);
	inb(0x1fc);
	inb(0x1fc);
}

void ata_wait_ready() {
	while (inb(0x1f7) & 0x80);
}

void ata_wait() {
	ata_io_wait();
	ata_wait_ready();
}

void ide_rw_setup(unsigned long lba) {
	outb(0x1fc, 0x02);
	ata_wait_ready();

	outb(0x1f6, 0xe0 | (lba >> 24 & 0xf)); // number of sectors
	outb(0x1f1, 0x00);
	outb(0x1f2, 0x01);
	outb(0x1f3, lba & 0xff);
	outb(0x1f4, lba >> 8 & 0xff);
	outb(0x1f5, lba >> 16 & 0xff);
}

bool ide_read_sector(unsigned long lba, unsigned char* buffer) {
	bool status;
	ide_rw_setup(lba);
	outb(0x1f7, 0x20); // read PIO

	ata_wait();
	status = inb(0x1f7);
	status = (status & 0x01) || (status & 0x20) || !(status & 0x08); // has error

	insw(0x1f0, buffer, IDE_BUFFER_SIZE_W);
	ata_wait();

	return !status;
}

void ide_write_sector(unsigned long lba, unsigned char* buffer) {
	ide_rw_setup(lba);
	outb(0x1f7, 0x30); // write PIO

	ata_wait();
	outsw(0x1f0, buffer, IDE_BUFFER_SIZE_W);
	outb(0x1f7, 0xe7); // cache flush

	ata_wait();
}
