;
; arch/i386/isr_entry.s
;
; Part of P-OS kernel.
;
; Written by Peter Bosch <peterbosc@gmail.com>
;
; Changelog:
; 31-03-2014 - Created
; 03-04-2013 - Added ISR stack selection
;


[BITS 32]
[section .text]

[extern i386_handle_interrupt]

%macro ISR_SYSCALL 1
	
[global i386_isr_entry_%1]		
; Stack here
;   SS         
;   ESP        <-- ESP before interrupt
;   EFLAGS
;   CS
;   EIP   	   <-- ESP at entry
;   0x0BADCA11 <+- ESP at pusha
;   EAX         |
;   ECX         |
;   EDX         |
;   EBX         |
;   ESP      ---/
;   EBP
;   ESI
;   EDI        <-- ESP after pusha
;   DS
;   interrupt id
i386_isr_entry_%1:	
	
	push dword 0x0BADCA11		; Copy error code to ISR stack	

	; Save all registers on the stack
	pusha 
	mov ebp, 0xCAFE57AC	; set ebp as a token for the tracer

	; Save data segment
	mov ax, ds
	push eax

	; Select kernel data segment
	mov ax, 0x20
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	; Tell the OS which interrupt was called
	push dword %1
	
	; Call C language part of ISR
	call i386_handle_interrupt
	
	; "Pop" the ISR number
	add esp, 4

	; Restore previous data segment
	pop eax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	
	; Restore all registers
	popa

	add esp, 4 ; Skip error code and EIP
	
	; Resume execution
	iret
%endmacro



; ISR macro for interrupts and exceptions that do not post an error code
%macro ISR_NOCODE 1
	
[global i386_isr_entry_%1]		


; Stack here
;   SS         
;   ESP        <-- ESP before interrupt
;   EFLAGS
;   CS
;   EIP   	   <-- ESP at entry
;   EAX        <-------------------\
;---Switched                       |
;   ESP         -------------------+
;   EIP                            |
;   0x0BADC0DE <+- ESP at pusha    |
;   EAX  28     |                  |
;   ECX  24     |                  |
;   EDX  20     |                  |
;   EBX  16     |                  |
;   ESP  12  ---/ 1)               / 2)
;   EBP  8
;   ESI  4
;   EDI  0     <-- ESP after pusha
;   DS
;   interrupt_no
i386_isr_entry_%1:	

	push eax

	mov eax, esp
	mov esp, dword 0xBFFFFFFF
	
	push eax			; Push esp to ISR stack
	push dword [eax+4]		; Copy EIP to ISR stack
	push dword 0x0BADC0DE		; Copy error code to ISR stack	

	; Save all registers on the stack
	pusha 
	mov ebp, 0xCAFE57AC	; set ebp as a token for the tracer

	; Hack pusha register set to contain correct value
	clc
	mov ebx, esp	; ebx = &pusha_registers
	add ebx, 12	    ; ebx = &pusha_registers.esp
	mov [ebx], eax	; pusha_registers.esp = task esp
	mov eax, [eax]	; Load stored EAX
	add ebx, 16	    ; pusha_registers.eax
	mov [ebx], eax	; pusha_registers.eax = eax

	; Save data segment
	mov  ax, ds
	push eax        ; 

	; Select kernel data segment
	mov ax, 0x20
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	; Tell the OS which interrupt was called
	push dword %1
	
	; Call C language part of ISR
	call i386_handle_interrupt
	
	; "Pop" the ISR number
	add esp, 4

	; Restore previous data segment
	pop eax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	
	; Restore all registers
	popa

	add esp, 8 ; Skip error code and EIP
	
	; Restore task stack
	pop esp
	
	; Restore EAX
	pop eax

	; Resume execution
	iret
%endmacro

; ISR macro for interrupts and exceptions that post an error code
%macro ISR_CODE 1

[global i386_isr_entry_%1]	

i386_isr_entry_%1:

	push eax

	mov eax, esp
	mov esp, dword 0xBFFFFFFF
		

	push eax			; Push esp to ISR stack
	push dword [eax+8]		; Copy EIP to ISR stack
	push dword [eax+4]		; Copy error code to ISR stack	

	; Save all registers on the stack
	pusha 

	; Hack pusha register set to contain correct value
	clc
	mov ebx, esp	; \/
	add ebx, 12	; pusha_registers.esp
	mov [ebx], eax	; pusha_registers.esp = task esp
	mov eax, [eax]	; Load stored EAX
	add ebx, 16	; pusha_registers.eax
	mov [ebx], eax	; pusha_registers.eax = eax
	mov ebp, 0xCAFE57AC	; set ebp as a token for the tracer

	; Save data segment
	mov ax, ds
	push eax

	; Select kernel data segment
	mov ax, 0x20
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	; Tell the OS which interrupt was called
	push dword %1
	
	; Call C language part of ISR
	call i386_handle_interrupt
	
	; "Pop" the ISR number
	add esp, 4

	; Restore previous data segment
	pop eax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	
	; Restore all registers
	popa

	add esp, 8 ; Skip error code and EIP
	
	; Restore task stack
	pop esp
	
	; Restore EAX
	pop eax

	; "Pop" the error code
	add esp, 4

	; Resume execution
	iret

%endmacro

; Exception Handlers

ISR_NOCODE 0 ; Divide by zero
ISR_NOCODE 1 ; Debug
ISR_NOCODE 3 ; Breakpoint
ISR_NOCODE 4 ; Overflow
ISR_NOCODE 5 ; Bounds check
ISR_NOCODE 6 ; Invalid Opcode
ISR_NOCODE 7 ; No Coprocessor
ISR_CODE 8   ; System Error (Double Fault)
ISR_NOCODE 9 ; Coprocessor Segment Overrun
ISR_CODE 10  ; Invalid TSS
ISR_CODE 11  ; Segment not present
ISR_CODE 12  ; Stack exception
ISR_CODE 13  ; GPF
ISR_CODE 14  ; Page Fault
ISR_NOCODE 16; Coprocessor Error

; Hardware ISRs

%assign i 32
%rep    16
        ISR_NOCODE i
%assign i i+1
%endrep

; Software ISRS

ISR_SYSCALL 80h ; Syscall Interrupt
