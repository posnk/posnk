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

; This code segment is invoked by the interrupt handler stubs
; it will push the processor state and call the C interrupt handler
; which can then either return or branch out to another process.
; Expected stack contents:
;	C	(SS)
;	C	(ESP)
;	C	EFLAGS
;	C	CS
;	C	EIP
;	C/K	Error Code
;	K	Interrrupt Id
i386_entry_core:

	; Store registers ( pushes 8 dwords )
	pusha

	; Store data segment
	xor	eax, eax
	mov	ax, ds
	push	eax

	; Switch to supervisor segment for data access
	mov	ax, 0x20
	mov	ds, ax
	mov	es, ax
	mov	fs, ax
	mov	gs, ax

	; Set magic base pointer to stop kernel stacktraces
	mov	ebp, 0xCAFE57AC

	; Push the stack address
	push	esp

	; Clear the direction flag
	cld

	; Call the interrupt handler
	call	i386_handle_interrupt

	; "Pop" the stack address
	add	esp, 4

	; Restore data segment
	pop	eax
	mov	ds, ax
	mov	es, ax
	mov	fs, ax
	mov	gs, ax

	; Restore registers
	popa

	; Pop error code and interrupt number
	add	esp, 8

	; Exit interrupt handler
	iret

[global i386_fork_exit]
[extern i386_user_exit]
; Simulate interrupt return to duplicate user state
i386_fork_exit:
	; disable interrupts
	cli

	; alloca( sizeof( i386_isr_stack_t ) )

	sub esp, 64

	; Set magic base pointer to stop kernel stacktraces
	mov	ebp, 0xCAFE57AC

	; Push the stack address
	push	esp

	; Call the user state restore function
	call	i386_user_exit

	; "Pop" the stack address
	add	esp, 4

	; Restore data segment
	pop	eax
	mov	ds, ax
	mov	es, ax
	mov	fs, ax
	mov	gs, ax

	; Restore registers
	popa

	; Pop error code and interrupt number
	add	esp, 8

	; Exit interrupt handler
	iret

%macro ISR_SYSCALL 1

[global i386_isr_entry_%1]
i386_isr_entry_%1:

	; Push fake error code
	push	dword 0x0BADCA11

	; Push interrupt id
	push	dword %1

	; Jump to common entry code
	jmp	i386_entry_core

%endmacro



; ISR macro for interrupts and exceptions that do not post an error code
%macro ISR_NOCODE 1

[global i386_isr_entry_%1]
i386_isr_entry_%1:

	; Push fake error code
	push	dword 0x0BADC0D3

	; Push interrupt id
	push	dword %1

	; Jump to common entry code
	jmp	i386_entry_core


%endmacro

; ISR macro for interrupts and exceptions that post an error code
%macro ISR_CODE 1

[global i386_isr_entry_%1]
i386_isr_entry_%1:

	; Push interrupt id
	push	dword %1

	; Jump to common entry code
	jmp	i386_entry_core

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
ISR_SYSCALL 81h ; Syscall Interrupt
