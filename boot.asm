bits 16
org 0x7c00

%include "bpb.asm"

KERNEL_OFFSET equ 0x1000

start:
	xor ax, ax
	mov ds, ax
	mov ss, ax
	mov sp, 0x7c00

	; set video mode
	mov ah, 0
	mov al, 0x13
	int 0x10

	; load kernel from disk
	mov ah, 0x02 ; function 02
	mov al, 24 ; sectors to read
	mov ch, 0 ; cylinder
	mov cl, 2 ; from sector
	mov dh, 0 ; head
	mov bx, KERNEL_OFFSET
	int 0x13

	; switch to protected mode
	cli
	lgdt [gdt_descriptor]
	mov eax, cr0
	or eax, 0x1
	mov cr0, eax

	; begin 32-bit
	jmp CODE_SEG:init_32bit

bits 32

init_32bit:
	mov ax, DATA_SEG
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	mov esp, 0x90000

	call KERNEL_OFFSET

end_loop:
	hlt
	jmp end_loop

times 510-($-$$) db 0
dw 0xAA55
