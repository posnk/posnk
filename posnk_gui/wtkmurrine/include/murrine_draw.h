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

#ifndef MURRINE_DRAW_H
#define MURRINE_DRAW_H

#include "murrine_style.h"
#include "murrine_types.h"

#include <cairo.h>

void murrine_register_style_murrine (MurrineStyleFunctions *functions);
void murrine_register_style_rgba (MurrineStyleFunctions *functions);

#endif /* MURRINE_DRAW_H */
