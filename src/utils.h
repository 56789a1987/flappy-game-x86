void outb(unsigned short port, unsigned char value);
unsigned char inb(unsigned short port);
unsigned short inw(unsigned short port);

void set_timer(unsigned long frequency);
void start_beep(unsigned long frequency);
void stop_beep();

void clear_rect(short x, short y, short w, short h);

unsigned long xorshf96();
