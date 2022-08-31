# $@ = target file
# $< = first dependency
# $^ = all dependencies

CC   := i386-elf-gcc
LD   := i386-elf-ld
PY   := python
NASM := nasm
QEMU := qemu-system-x86_64
SRC  := ./src
OBJ  := ./obj
SRCS := $(wildcard $(SRC)/*.c)
OBJS := $(patsubst $(SRC)/%.c,$(OBJ)/%.o,$(SRCS))

# First rule is the one executed when no parameters are fed to the Makefile
all: run

kernel.bin: kernel-loader.o $(OBJS)
	$(LD) -melf_i386 --oformat binary -o $@ -Ttext 0x1000 $^

kernel-loader.o: kernel-loader.asm
	$(NASM) $< -f elf32 -o $@

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) -m32 -ffreestanding -c $< -o $@

boot.bin: boot.asm
	$(NASM) $< -f bin -o $@

os-image.img: boot.bin kernel.bin
	cat $^ > $@

run: os-image.img
	$(QEMU) -drive file=$<,format=raw -soundhw pcspk

assets:
	$(PY) createData.py

clean:
	$(RM) *.bin *.o $(OBJ)/*.o

.PHONY: run assets clean all
