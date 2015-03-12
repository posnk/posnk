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

#include "arch/armv7/cpu.h"

.global armv7_call_kern

armv7_call_kern:
	mov	r4,	r0
	mov	r0,	r2
	mov	sp,	r1
	mov	pc,	r4
	b	.

.ltorg
