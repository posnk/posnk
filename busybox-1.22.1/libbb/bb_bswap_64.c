/*
 * Utility routines.
 *
 * Copyright (C) 2010 Denys Vlasenko
 *
 * Licensed under GPLv2, see file LICENSE in this source tree.
 */

#include "libbb.h"

#define BSWAP_8(x)      ((x) & 0xff)
#define BSWAP_16(x)     ((BSWAP_8(x) << 8) | BSWAP_8((x) >> 8))
#define BSWAP_32(x)     ((BSWAP_16(x) << 16) | BSWAP_16((x) >> 16))
#define BSWAP_64(x)     ((BSWAP_32(x) << 32) | BSWAP_32((x) >> 32))

#if !(ULONG_MAX > 0xffffffff)
uint64_t FAST_FUNC bb_bswap_64(uint64_t x)
{
	return BSWAP_64(x);
}
#endif
