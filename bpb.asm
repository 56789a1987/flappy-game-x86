jmp start
nop

bpb_info:
	OEMname:           db "mkfs.fat"
	bytesPerSector:    dw 512
	sectPerCluster:    db 1
	reservedSectors:   dw 1
	numFAT:            db 2
	numRootDirEntries: dw 224
	numSectors:        dw 2880
	mediaType:         db 0xf0
	numFATsectors:     dw 9
	sectorsPerTrack:   dw 18
	numHeads:          dw 2
	numHiddenSectors:  dd 0
	numSectorsHuge:    dd 0
	driveNum:          db 0
	reserved:          db 0
	signature:         db 0x29
	volumeID:          dd 0x2d7e5a1a
	volumeLabel:       db "NO NAME    "
	fileSysType:       db "FAT12   "

gdt_start:
	dd 0x0  ;4 bytes
	dd 0x0  ;4 bytes

gdt_code:
	dw 0xffff    ;segment length, bits 0-15
	dw 0x0       ;segment base, bits 0-15
	db 0x0       ;segment base, bits 16-23
	db 10011010b ;flags (8 bits)
	db 11001111b ;flags (4 bits) + segment length, bits 16-19
	db 0x0       ;segment base, bits 24-31

gdt_data:
	dw 0xffff
	dw 0x0
	db 0x0
	db 10010010b
	db 11001111b
	db 0x0

gdt_end:

gdt_descriptor:
	dw gdt_end - gdt_start - 1  ;size (16-bit), always one less of its true size
	dd gdt_start                ;address (32-bit)

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start
