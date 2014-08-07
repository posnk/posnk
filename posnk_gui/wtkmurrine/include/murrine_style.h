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

#ifndef MURRINE_STYLE_H
#define MURRINE_STYLE_H

#include "murrine_types.h"

typedef struct _MurrineStyle MurrineStyle;
typedef struct _MurrineStyleClass MurrineStyleClass;

#define MURRINE_TYPE_STYLE              (murrine_style_get_type())
#define MURRINE_STYLE(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), MURRINE_TYPE_STYLE, MurrineStyle))
#define MURRINE_STYLE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), MURRINE_TYPE_STYLE, MurrineStyleClass))
#define MURRINE_IS_STYLE(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), MURRINE_TYPE_STYLE))
#define MURRINE_IS_STYLE_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), MURRINE_TYPE_STYLE))
#define MURRINE_STYLE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), MURRINE_TYPE_STYLE, MurrineStyleClass))

struct _MurrineStyle
{

	MurrineColors colors;

	MurrineProfiles profile;

	double   contrast;
	double   glow_shade;
	double   gradient_shades[4];
	double   highlight_shade;
	double   lightborder_shade;

	uint8   glazestyle;
	uint8   glowstyle;
	uint8   lightborderstyle;
	uint8   listviewheaderstyle;
	uint8   listviewstyle;
	uint8   menubaritemstyle;
	uint8   menubarstyle;
	uint8   menuitemstyle;
	uint8   menustyle;
	uint8   progressbarstyle;
	uint8   reliefstyle;
	uint8   roundness;
	uint8   scrollbarstyle;
	uint8   sliderstyle;
	uint8   stepperstyle;
	uint8   toolbarstyle;

	boolean animation;
	boolean colorize_scrollbar;
	boolean gradients;
	boolean has_focus_color;
	boolean has_scrollbar_color;
	boolean rgba;
};

struct _MurrineTheme {
	MurrineStyle	default_style;
	MurrineStyle	button_style;
	MurrineStyle	dark_style;
	MurrineStyle	wide_style;
	MurrineStyle	wider_style;
	MurrineStyle	entry_style;
	MurrineStyle	notebook_button_style;
	MurrineStyle	spinbutton_style;
	MurrineStyle	scrollbar_style;
	MurrineStyle	overlay_scrollbar_style;
	MurrineStyle	scale_style;
	MurrineStyle	notebook_bg_style;
	MurrineStyle	notebook_style;
	MurrineStyle	statusbar_style;
	MurrineStyle	comboboxentry_style;
	MurrineStyle	menubar_style;
	MurrineStyle	toolbar_style;
	MurrineStyle	toolbar_button_style;
};

struct _MurrineStyleClass
{

	MurrineStyleFunctions style_functions[MRN_NUM_DRAW_STYLES];
};

#endif /* MURRINE_STYLE_H */
