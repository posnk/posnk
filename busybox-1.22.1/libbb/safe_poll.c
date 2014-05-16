/* vi: set sw=4 ts=4: */
/*
 * Utility routines.
 *
 * Copyright (C) 2007 by Denys Vlasenko <vda.linux@googlemail.com>
 *
 * Licensed under GPLv2, see file LICENSE in this source tree.
 */

#include "libbb.h"

/* Wrapper which restarts poll on EINTR or ENOMEM.
 * On other errors does perror("poll") and returns.
 * Warning! May take longer than timeout_ms to return! */
//int FAST_FUNC safe_poll(struct pollfd *ufds, nfds_t nfds, int timeout)
//{
//		return 0;
//}//
