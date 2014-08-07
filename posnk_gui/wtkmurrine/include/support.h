/* Murrine theme engine
 * Copyright (C) 2006-2007-2008-2009 Andrea Cimitan
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 */

#ifndef SUPPORT_H
#define SUPPORT_H

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>


#include "murrine_types.h"
#include "cairo-support.h"

#define MAX(a, b)  (((a) > (b)) ? (a) : (b))
#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))

#define RADIO_SIZE 13
#define CHECK_SIZE 13

/* Opacity settings */
#define GRADIENT_OPACITY 0.90
#define WINDOW_OPACITY 0.88
#define ENTRY_OPACITY 0.90
#define NOTEBOOK_OPACITY 0.92
#define MENUBAR_OPACITY 0.88
#define MENUBAR_GLOSSY_OPACITY 0.88
#define MENUBAR_STRIPED_OPACITY 0.94
#define TOOLBAR_OPACITY 0.88
#define TOOLBAR_GLOSSY_OPACITY 0.88
#define MENU_OPACITY 0.90
#define TOOLTIP_OPACITY 0.90

#define TRUE (1)
#define FALSE (0)

#define WTK_SHADOW_ETCHED_IN	(1)
#define WTK_SHADOW_IN		(0)

#define WTK_STATE_NORMAL	(0)
#define WTK_STATE_HOVER		(1)
#define WTK_STATE_SELECTED	(2)
#define WTK_STATE_DISABLED	(3)
#define WTK_STATE_ACTIVE	(4)
#endif /* SUPPORT_H */
