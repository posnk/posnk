;Copyright (C) 2017 Peter Bosch
;
;This program is free software; you can redistribute it and/or
;modify it under the terms of the GNU General Public License
;as published by the Free Software Foundation; either version 2
;of the License, or (at your option) any later version.
;
;This program is distributed in the hope that it will be useful,
;but WITHOUT ANY WARRANTY; without even the implied warranty of
;MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;GNU General Public License for more details.
;
;You should have received a copy of the GNU General Public License
;along with this program; if not, write to the Free Software
;Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

;
; userlib/i386/syscall.s
;
; Part of P-OS kernel.
;
; Written by Peter Bosch <peterbosc@gmail.com>
;
; Changelog:
; 16-07-2017 - Created
;

[BITS 32]
[section .text]

[extern errno]

[global syscall]
syscall:
	push	ebp
	mov	ebp,	esp
	push	esi
	push	edi
	push	ebx
	
	mov	eax, [ebp+8]	; id
	mov	ecx, [ebp+12]	; a
	mov	edx, [ebp+16]	; b
	mov	esi, [ebp+20]	; c
	mov	edi, [ebp+24]	; d
	mov	ebx, [ebp+28]	; e
	push	dword [ebp+32]	; f
	int	129		; syscall
	pop	esi
	
	mov	[ errno ], ecx	; errno
	
	pop	ebx
	pop	edi
	pop	esi
	pop	ebp
	ret
	
