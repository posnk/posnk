[BITS 32]       ; 32 bit code
[section .multiboot] ; keep NASM happy
 
MULTIBOOT_PAGE_ALIGN	equ 1<<0
MULTIBOOT_MEMORY_INFO	equ 1<<1
MULTIBOOT_HEADER_MAGIC	equ 0x1BADB002
MULTIBOOT_HEADER_FLAGS	equ MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO
MULTIBOOT_CHECKSUM	equ -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

ALIGN 4
multiboot_header:
	dd MULTIBOOT_HEADER_MAGIC
	dd MULTIBOOT_HEADER_FLAGS
	dd MULTIBOOT_CHECKSUM

[section .text] 

[global i386_start]  ; make 'start' function global
[global i386_init_switch_gdt]  ; make 'start' function global
[extern i386_init_mm]  ; our C kernel main

; the kernel entry point
i386_start:

	; back up multiboot parameters

	mov ecx, ebx
	mov edx, eax

	; here's the trick: we load a GDT with a base address
	; of 0x40000000 for the code (0x08) and data (0x10) segments

	lgdt [i386_init_trick_gdt]
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
 
	; jump to the higher half kernel
	jmp 0x08:i386_init_higherhalf
 
i386_init_higherhalf:
	; from now the CPU will translate automatically every address
	; by adding the base 0x40000000
 
	mov esp, i386_sys_stack ; set up a new stack for our kernel
	mov ebp, 0xCAFE57AC	; set ebp as a token for the tracer
	; push the multiboot parameters
	
	push edx
	push ecx
 
	call i386_init_mm ; jump to our C kernel ;)
 
	; just a simple protection...
	jmp $

i386_init_switch_gdt:
	mov ax, 0x20
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
 
	; jump to the higher half kernel
	jmp 0x18:i386_init_switch_gdt_ret
 
i386_init_switch_gdt_ret:
	ret

[section .setup]

i386_init_trick_gdt:
	dw i386_init_gdt_end - i386_init_gdt - 1 ; size of the GDT
	dd i386_init_gdt ; linear address of GDT
 
i386_init_gdt:
	dd 0, 0							; null gate
	db 0xFF, 0xFF, 0, 0, 0, 10011010b, 11001111b, 0x40	; code selector 0x08: base 0x40000000, limit 0xFFFFFFFF, type 0x9A, granularity 0xCF
	db 0xFF, 0xFF, 0, 0, 0, 10010010b, 11001111b, 0x40	; data selector 0x10: base 0x40000000, limit 0xFFFFFFFF, type 0x92, granularity 0xCF

; Kernel Space code (Offset: 24 0x18 bytes)
 	dw 0FFFFh 			; limit low
	dw 0 				; base low
	db 0 				; base middle
	db 10011010b 			; access - Notice that bits 5 and 6 (privilege level) are 0 for Ring 0
	db 11001111b 			; granularity
	db 0 				; base high
 
; Kernel Space data (Offset: 32 (0x20) bytes
	dw 0FFFFh 			; limit low (Same as code)
	dw 0 				; base low
	db 0 				; base middle
	db 10010010b 			; access - Notice that bits 5 and 6 (privilege level) are 0 for Ring 0
	db 11001111b 			; granularity
	db 0				; base high
 
; User Space code (Offset: 40 (0x28) bytes)
	dw 0FFFFh 			; limit low
	dw 0 				; base low
	db 0 				; base middle
	db 11111010b 			; access - Notice that bits 5 and 6 (privilege level) are 11b for Ring 3
	db 11001111b 			; granularity
	db 0 				; base high
 
; User Space data (Offset: 48 (0x30) bytes
	dw 0FFFFh 			; limit low (Same as code)
	dw 0 				; base low
	db 0 				; base middle
	db 11110010b 			; access - Notice that bits 5 and 6 (privilege level) are 11b for Ring 3
	db 11001111b 			; granularity
	db 0				; base high

[global i386_tss_descriptor]
i386_tss_descriptor:
; Task Switch Segment
	dw 0FFFFh 			; limit low (Same as code)
	dw 0 				; base low
	db 0 				; base middle
	db 0x89 			; access - Notice that bits 5 and 6 (privilege level) are 11b for Ring 3
	db 0x40 			; granularity
	db 0				; base high
i386_init_gdt_end:

[section .bss]


resb 0x1000
i386_sys_stack:	; our kernel stack
