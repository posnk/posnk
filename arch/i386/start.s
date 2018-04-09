[BITS 32]       ; 32 bit code
[section .multiboot] ; keep NASM happy
 
MULTIBOOT_PAGE_ALIGN	equ 1<<0
MULTIBOOT_MEMORY_INFO	equ 1<<1
MULTIBOOT_GRAPHICS	equ 1<<2
MULTIBOOT_HEADER_MAGIC	equ 0x1BADB002
MULTIBOOT_HEADER_FLAGS	equ MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO
MULTIBOOT_CHECKSUM	equ -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)


ALIGN 4
multiboot_header:
	dd MULTIBOOT_HEADER_MAGIC
	dd MULTIBOOT_HEADER_FLAGS
	dd MULTIBOOT_CHECKSUM
	dd 0 
	dd 0 
	dd 0
	dd 0
	dd 0
	dd 0 ; mode type: lfb
	dd 1024 ; width
	dd 768  ; height
	dd 32   ; depth

[section .text] 

[global i386_start]  ; make 'start' function global
[global i386_init_switch_gdt]  ; make 'start' function global
[extern i386_init_mm]  ; our C kernel main

; the kernel entry point
i386_start:

	; back up multiboot parameters

	mov ecx, ebx
	mov edx, eax

	; load sane descriptors

	lgdt [i386_init_gdt_ptr]
	mov ax, 0x20
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
 
	; load the page directory

	mov eax, i386_init_pdir
	mov cr3, eax

	; enable paging

	mov eax, cr0
	or  eax, 0x80000001
	mov cr0, eax
	

	; jump to the higher half kernel
	jmp 0x18:i386_init_higherhalf
 
i386_init_higherhalf:
	; from now the CPU will translate automatically every address
	; by adding the base 0x40000000
 
	mov esp, i386_sys_stack ; set up a new stack for our kernel
	mov ebp, 0xCAFE8007	; set ebp as a token for the tracer
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
align   4096,db 0
i386_init_pts:

%assign i 0
%rep 2048
	dd 0x10B + i
	%assign i i + 4096
%endrep

align   4096,db 0

; page directory mapping the first 8MB of physical addresses to both 0 and 3GB

i386_init_pdir:
	dd i386_init_pts + 0x10B
	dd i386_init_pts + 0x10B + 4096
%rep 766
	dd 0x0
%endrep
	dd i386_init_pts + 0x10B
	dd i386_init_pts + 0x10B + 4096
%rep 254
	dd 0x0
%endrep

ALIGN 4
i386_init_gdt_ptr:
	dw i386_init_gdt_end - i386_init_gdt - 1 ; size of the GDT
	dd i386_init_gdt ; linear address of GDT

i386_init_gdt:
	dd 0, 0							; null gate
	db 0xFF, 0xFF, 0, 0, 0, 10011010b, 11001111b, 0x40	; code selector 0x08: base 0x00000000, limit 0xFFFFFFFF, type 0x9A, granularity 0xCF
	db 0xFF, 0xFF, 0, 0, 0, 10010010b, 11001111b, 0x40	; data selector 0x10: base 0x00000000, limit 0xFFFFFFFF, type 0x92, granularity 0xCF

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
