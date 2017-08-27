[BITS 32]
[section .text]

; stack
; --
; ARG1.. 28
; ARG0   24
; EIP    20
; EAX    1C
; ECX    18
; EDX    14
; EBX    10
; BadESP C
; EBP    8
; ESI    4
; EDI    0
; May clobber: EAX, ECX, EDX
; void i386_do_context_switch( uint32_t esp, uint32_t pagedir, uint32_t *old_esp)
[global i386_do_context_switch]
i386_do_context_switch:
	pusha                     ; Save caller state
	mov    ebp  , esp         ; Set base pointer
	mov    eax  , [ebp+0x24]  ; eax=param0
	mov    ecx  , [ebp+0x28]  ; ecx=param1
	mov    edx  , [ebp+0x2C]  ; edx=param2
	mov    [edx], esp         ; *param2 = esp
	mov    esp  , eax         ; esp = param0
	mov    cr3  , ecx         ; cr3 = param1
	popa                      ; Restore caller state
	ret                       ; Return
	
[extern dbgapi_invoke_kdbg]
[global i386_cs_debug_attach]
i386_cs_debug_attach:
	mov ebp, esp
	cli
	mov eax, [ebp+4]  ; param 1 (esp)
	mov ebx, [ebp+8]  ; param 2 (ebp)
	mov ecx, [ebp+12] ; param 3 (eip)
	mov edx, [ebp+16] ; param 4 (page directory phys_addr)

	mov esp, eax
	mov ebp, ebx
	mov cr3, edx
	mov eax, 0xFFFDCAFE
	push ecx
	push ebx
	mov ebp, esp
	push ecx
	call dbgapi_invoke_kdbg
	hlt
