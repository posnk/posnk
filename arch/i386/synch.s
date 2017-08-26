;
; arch/i386/prot_asm.s
;
; Part of P-OS kernel.
;
; Used by Peter Bosch <peterbosc@gmail.com>
;
; Changelog:
; 26-08-2017 - Created
;


[BITS 32]
[section .text]

[global enable]
enable:
	pushf
	sti
	pop	  eax
	and   eax, 0x200
	ret

[global disable]
disable:
	pushf
	cli
	pop   eax
	and   eax, 0x200
	ret
	
[global restore]
restore:
	push  ebx
	mov   ebx, [esp + 4]
	pushf
	pop   eax
	and   ebx, 0x200
	or    eax, ebx
	push  eax
	popf
	pop   ebx
	ret
