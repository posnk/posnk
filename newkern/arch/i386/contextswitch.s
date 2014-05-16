[BITS 32]
[section .text]

[global i386_get_return_address]
i386_get_return_address:
  pop eax
  jmp eax 

[global i386_do_context_switch]
i386_do_context_switch:
	mov ebp, esp
	mov eax, [ebp+4]  ; param 1 (esp)
	mov ebx, [ebp+8]  ; param 2 (ebp)
	mov ecx, [ebp+12] ; param 3 (eip)
	mov edx, [ebp+16] ; param 4 (page directory phys_addr)

	mov esp, eax
	mov ebp, ebx
	mov cr3, edx
	mov eax, 0xFFFDCAFE

	jmp ecx
