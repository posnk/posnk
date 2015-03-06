#
# arch/armv7/start.S
#
# Part of P-OS kernel.
#
# Written by Peter Bosch <peterbosc@gmail.com>
#
# Changelog:
# 06-03-2015 - Created
#

.section .setup
.global armv7_start

armv7_start:
	ldr	%sp,	=armv7_loader_stack_bottom
	ldr	%pc,	=armv7_entry
	b	.

.ltorg

armv7_loader_stack_top:
	.balign 4
	.space 4096
armv7_loader_stack_bottom:
