#define IDE_BUFFER_SIZE 512
#define IDE_BUFFER_SIZE_W 256

bool ide_read_sector(unsigned long lba, unsigned char* buffer);
void ide_write_sector(unsigned long lba, unsigned char* buffer);
