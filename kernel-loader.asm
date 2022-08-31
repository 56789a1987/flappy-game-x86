bits 32
extern main

lidt [idt_descriptor]

; remap the IRQs
; ICW1
mov al, 0x11
out 0x20, al
out 0xA0, al

; ICW2
mov al, 0x20 ; IRQ0-IRQ7 -> interrupts 0x20-0x27
out 0x21, al
mov al, 0x28 ; IRQ8-IRQ15 -> interrupts 0x28-0x2F
out 0xA1, al

; ICW3
mov al, 0x4
out 0x21, al
mov al, 0x2
out 0xA1, al

; ICW4
mov al, 0x1
out 0x21, al
out 0xA1, al

; mask
mov al, 0x0
out 0x21, al
out 0xA1, al

; enable interrupts
sti

call main

end_loop:
	hlt
	jmp end_loop

BASE_ADDR equ 0x1000

%macro begin_interrupt 0
	pusha
%endmacro

%macro end_interrupt 0
	mov al, 0x20
	out 0xa0, al
	out 0x20, al
	popa
	iret
%endmacro

extern timer_handler_cb
extern keyboard_handler_cb
extern mouse_handler_cb

hwi_handler:
	begin_interrupt
	end_interrupt

timer_handler:
	begin_interrupt
	call timer_handler_cb
	end_interrupt

keyboard_handler:
	begin_interrupt
	call keyboard_handler_cb
	end_interrupt

mouse_handler:
	begin_interrupt
	call mouse_handler_cb
	end_interrupt

%macro irq_reg 1
	dw (BASE_ADDR + %1 - $$) & 0xffff
	dw 0x8
	db 0x0
	db 0x8E
	dw (BASE_ADDR + %1 - $$) >> 16
%endmacro

idt_start:
	times 8 * 32 db 0
	; programmable interrupt timer
	irq_reg timer_handler
	; keyboard on PS/2
	irq_reg keyboard_handler
	; cascaded signals from IRQs 8–15
	irq_reg hwi_handler
	; COM2
	irq_reg hwi_handler
	; COM1
	irq_reg hwi_handler
	; LPT2
	irq_reg hwi_handler
	; floppy disk
	irq_reg hwi_handler
	; LPT1
	irq_reg hwi_handler
	; CMOS real-time clock
	irq_reg hwi_handler
	; open interrupt/available / SCSI / NIC
	irq_reg hwi_handler
	; open interrupt/available / SCSI / NIC
	irq_reg hwi_handler
	; open interrupt/available / SCSI / NIC
	irq_reg hwi_handler
	; mouse on PS/2
	irq_reg mouse_handler
	; CPU co-processor / FPU / Inter-processor
	irq_reg hwi_handler
	; primary ATA channel
	irq_reg hwi_handler
	; secondary ATA channel
	irq_reg hwi_handler

idt_end:

idt_descriptor:
	dw idt_end - idt_start
	dd idt_start
