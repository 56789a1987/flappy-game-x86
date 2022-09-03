void outb(unsigned short port, unsigned char value);
void outw(unsigned short port, unsigned short value);
unsigned char inb(unsigned short port);
unsigned short inw(unsigned short port);
void outsw(unsigned short port, unsigned char *data, unsigned long size);
void insw(unsigned short port, unsigned char *data, unsigned long size);

extern unsigned long tick;

void set_timer(unsigned long frequency);

void start_beep(unsigned long frequency);
void stop_beep();

unsigned char number_size(unsigned long value);
void zero_memory(unsigned char* buffer, unsigned size);
void clear_rect(short x, short y, short w, short h);

unsigned long xorshf96();
