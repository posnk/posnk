[BITS 32]
[section .text]

[global i386_phys_copy_frame]


;STACK ON ENTRY
;
;PARAM2  +12
;PARAM1  +8
;EIP     +4
;EBP	 +0
; <- EBP 0s
i386_phys_copy_frame:
	push ebp
	mov ebp, esp
	
	mov esi, [ebp + 8] ; source
	mov edi, [ebp + 12] ; dest

	pushfd ; Preserve flags

	; Disable interrupts as we are going to kill the processors ability to access RAM

	cli
 
	; Load CS with the trick segment so processor can still locate our code
	jmp 0x08:.frame_trick

.frame_trick:
	; Disable paging so we can access physical memory
	mov eax, cr0
	and eax, 7FFFFFFFh
	mov cr0, eax

	; counter register
	mov ebx, 0

	; main copy loop
.copy_loop:
	; load esi[ebx] into eax (dword)
	mov eax, [ebx*4+esi]
	; write eax into edi[ebx] (dword)
	mov [ebx*4+edi], eax
	;ebx++
	inc ebx
	;ebx == 1024? 
	cmp ebx, 1024
	; No: loop, Yes: exit
	jne .copy_loop

	; Re-enable paging
	mov eax, cr0
	or  eax, 80000000h
	mov cr0, eax

	; Disable GDT trick
	jmp 0x18:.frame_exit

.frame_exit:

	; Return

	popfd
	leave
	ret
