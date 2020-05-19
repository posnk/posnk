;
; arch/i386/prot_asm.s
;
; Part of P-OS kernel.
;
; Used by Peter Bosch <peterbosc@gmail.com>
; Source: OSDEV wiki (really cba to write this code myself, it would end up
; being the very same code anyways)
;
; Changelog:
; 06-04-2013 - Created
;


[BITS 32]
[section .text]

[global i386_tss_flush]
i386_tss_flush:
	mov ax, 0x3B      ; Load the index of our TSS structure - The index is
	                  ; 0x38, as it is the 7th selector and each is 8 bytes
	                  ; long, but we set the bottom two bits (making 0x3B)
	                  ; so that it has an RPL of 3, not zero.
	ltr ax            ; Load 0x3B into the task state register.
	ret

[global i386_protection_user_call]
i386_protection_user_call:
     mov esi, [esp + 4]
     mov edi, [esp + 8]
     pushf

     mov ebx, [esp]
     or  ebx, 0x200
     
     mov ax,0x33
     mov ds,ax
     mov es,ax 
     mov fs,ax 
     mov gs,ax  ;we don't need to worry about SS. it's handled by iret
 
     push 0x33  ;user data segment with bottom 2 bits set for ring 3
     push edi 
     push ebx
     push 0x2B  ;user code segment with bottom 2 bits set for ring 3
     push esi 
     iret
