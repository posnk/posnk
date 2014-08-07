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

#include <cairo.h>

#include "murrine_draw.h"
#include "murrine_style.h"
#include "murrine_types.h"
#include "support.h"
#include "cairo-support.h"

static void
murrine_draw_inset (cairo_t *cr,
                    const MurrineRGB *bg_color,
                    double x, double y, double w, double h,
                    double radius, uint8 corners)
{
	MurrineRGB highlight, shadow;
	radius = MIN (radius, MIN (w/2.0, h/2.0));

	murrine_shade (bg_color, 1.15, &highlight);
	murrine_shade (bg_color, 0.4, &shadow);

	/* highlight */
	cairo_move_to (cr, x+w+(radius*-0.2928932188), y-(radius*-0.2928932188));

	if (corners & MRN_CORNER_TOPRIGHT)
		cairo_arc (cr, x+w-radius, y+radius, radius, M_PI*1.75, M_PI*2);
	else
		cairo_line_to (cr, x+w, y);

	if (corners & MRN_CORNER_BOTTOMRIGHT)
		cairo_arc (cr, x+w-radius, y+h-radius, radius, 0, M_PI*0.5);
	else
		cairo_line_to (cr, x+w, y+h);

	if (corners & MRN_CORNER_BOTTOMLEFT)
		cairo_arc (cr, x+radius, y+h-radius, radius, M_PI*0.5, M_PI*0.75);
	else
		cairo_line_to (cr, x, y+h);

	murrine_set_color_rgba (cr, &highlight, 0.48);
	cairo_stroke (cr);

	/* shadow */
	cairo_move_to (cr, x+(radius*0.2928932188), y+h+(radius*-0.2928932188));

	if (corners & MRN_CORNER_BOTTOMLEFT)
		cairo_arc (cr, x+radius, y+h-radius, radius, M_PI*0.75, M_PI);
	else
		cairo_line_to (cr, x, y+h);

	if (corners & MRN_CORNER_TOPLEFT)
		cairo_arc (cr, x+radius, y+radius, radius, M_PI, M_PI*1.5);
	else
		cairo_line_to (cr, x, y);

	if (corners & MRN_CORNER_TOPRIGHT)
		cairo_arc (cr, x+w-radius, y+radius, radius, M_PI*1.5, M_PI*1.75);
	else
		cairo_line_to (cr, x+w, y);

	murrine_set_color_rgba (cr, &shadow, 0.12);
	cairo_stroke (cr);
}

static void
murrine_draw_highlight_and_shade (cairo_t *cr,
                                  const MurrineColors    *colors,
                                  const ShadowParameters *widget,
                                  int width, int height, int radius)
{
	MurrineRGB highlight;
	MurrineRGB shadow;
	uint8 corners = widget->corners;
	double x = 1.0;
	double y = 1.0;
	width  -= 3;
	height -= 3;
	radius = MIN (radius, MIN ((double)width/2.0, (double)height/2.0));

	if (radius < 0)
		radius = 0;

	murrine_shade (&colors->bg[0], 1.15, &highlight);
	murrine_shade (&colors->bg[0], 0.4, &shadow);

	cairo_save (cr);

	/* Top/Left highlight */
	if (corners & MRN_CORNER_BOTTOMLEFT)
		cairo_move_to (cr, x, y+height-radius);
	else
		cairo_move_to (cr, x, y+height);

	murrine_rounded_corner (cr, x, y, radius, corners & MRN_CORNER_TOPLEFT);

	if (corners & MRN_CORNER_TOPRIGHT)
		cairo_line_to (cr, x+width-radius, y);
	else
		cairo_line_to (cr, x+width, y);

	if (widget->shadow & MRN_SHADOW_OUT)
		murrine_set_color_rgba (cr, &highlight, 0.5);
	else
		murrine_set_color_rgba (cr, &shadow, 0.13);

	cairo_stroke (cr);

	/* Bottom/Right highlight -- this includes the corners */
	cairo_move_to (cr, x+width-radius, y); /* topright and by radius to the left */
	murrine_rounded_corner (cr, x+width, y, radius, corners & MRN_CORNER_TOPRIGHT);
	murrine_rounded_corner (cr, x+width, y+height, radius, corners & MRN_CORNER_BOTTOMRIGHT);
	murrine_rounded_corner (cr, x, y+height, radius, corners & MRN_CORNER_BOTTOMLEFT);

	if (widget->shadow & MRN_SHADOW_OUT)
		murrine_set_color_rgba (cr, &shadow, 0.13);
	else
		murrine_set_color_rgba (cr, &highlight, 0.5);

	cairo_stroke (cr);

	cairo_restore (cr);
}

static void
murrine_rgba_draw_button (cairo_t *cr,
                          const MurrineColors    *colors,
                          const WidgetParameters *widget,
                          int x, int y, int width, int height,
                          boolean horizontal)
{
	double xos = widget->xthickness > 2 ? 1 : 0;
	double yos = widget->ythickness > 2 ? 1 : 0;
	double glow_shade_custom = widget->glow_shade;
	double highlight_shade_custom = widget->highlight_shade;
	double lightborder_shade_custom = widget->lightborder_shade;
	MurrineRGB fill = colors->bg[widget->state_type];
	MurrineRGB border = colors->shade[!widget->disabled ? 8 : 6];
	MurrineGradients mrn_gradient_custom = widget->mrn_gradient;

	if (widget->disabled)
	{
		mrn_gradient_custom = get_decreased_gradient_shades (widget->mrn_gradient, 3.0);
		glow_shade_custom = get_decreased_shade (widget->glow_shade, 2.0);
		highlight_shade_custom = get_decreased_shade (widget->highlight_shade, 2.0);
		lightborder_shade_custom = get_decreased_shade (widget->lightborder_shade, 2.0);
	}
	else
		murrine_shade (&colors->shade[8], 0.95, &border);

	/* Default button */
	if (widget->is_default)
	{
		murrine_shade (&border, 0.8, &border);
		murrine_mix_color (&fill, &colors->spot[1], 0.2, &fill);
	}

	if (!horizontal)
		murrine_exchange_axis (cr, &x, &y, &width, &height);

	cairo_translate (cr, x, y);

	if (!widget->active && !widget->disabled && widget->reliefstyle > 1 && xos >= 0.5 && yos >= 0.5)
	{
		murrine_rounded_rectangle (cr, xos-0.5, yos-0.5, width-(xos*2)+1, height-(yos*2)+1, widget->roundness+1, widget->corners);
		murrine_set_color_rgba (cr, &border, 0.08);
		cairo_stroke (cr);
	}
	else if (widget->reliefstyle != 0 && xos >= 0.5 && yos >= 0.5)
		murrine_draw_inset (cr, &widget->parentbg, xos-0.5, yos-0.5,
		                    width-(xos*2)+1, height-(yos*2)+1,
		                    widget->roundness+1, widget->corners);

	murrine_mix_color (&border, &widget->parentbg, 0.2, &border);
	murrine_mix_color (&border, &fill, 0.25, &border);

	cairo_save (cr);

	cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);

	murrine_rounded_rectangle_closed (cr, xos+1, yos+1, width-(xos*2)-2, height-(yos*2)-2, widget->roundness-1, widget->corners);
	cairo_clip_preserve (cr);

	murrine_draw_glaze (cr, &fill,
	                    glow_shade_custom, highlight_shade_custom, !widget->active ? lightborder_shade_custom : 1.0,
	                    mrn_gradient_custom, widget, xos+1, yos+1, width-(xos*2)-2, height-(yos*2)-2,
	                    widget->roundness, widget->corners, horizontal);

	cairo_restore (cr);

	/* Draw pressed button shadow */
	if (widget->active)
	{
		cairo_pattern_t *pat;
		MurrineRGB shadow;

		murrine_shade (&fill, 0.94, &shadow);

		cairo_save (cr);

		murrine_rounded_rectangle_closed (cr, xos+1, yos+1, width-(xos*2)-2, height-(yos*2)-2, widget->roundness-1,
		                                  widget->corners & (MRN_CORNER_TOPLEFT | MRN_CORNER_TOPRIGHT | MRN_CORNER_BOTTOMLEFT));
		cairo_clip (cr);

		cairo_rectangle (cr, xos+1, yos+1, width-(xos*2)-2, 3);
		pat = cairo_pattern_create_linear (xos+1, yos+1, xos+1, yos+4);
		murrine_pattern_add_color_stop_rgba (pat, 0.0, &shadow, 0.58);
		murrine_pattern_add_color_stop_rgba (pat, 1.0, &shadow, 0.0);
		cairo_set_source (cr, pat);
		cairo_fill (cr);
		cairo_pattern_destroy (pat);

		cairo_rectangle (cr, xos+1, yos+1, 3, height-(yos*2)-2);
		pat = cairo_pattern_create_linear (xos+1, yos+1, xos+4, yos+1);
		murrine_pattern_add_color_stop_rgba (pat, 0.0, &shadow, 0.58);
		murrine_pattern_add_color_stop_rgba (pat, 1.0, &shadow, 0.0);
		cairo_set_source (cr, pat);
		cairo_fill (cr);
		cairo_pattern_destroy (pat);

		cairo_restore (cr);
	}

	/* Draw the border */
	murrine_set_color_rgb (cr, &border);
	murrine_rounded_rectangle (cr, xos+0.5, yos+0.5, width-(xos*2)-1, height-(yos*2)-1, widget->roundness, widget->corners);
	cairo_stroke (cr);
}

static void
murrine_rgba_draw_entry (cairo_t *cr,
                         const MurrineColors    *colors,
                         const WidgetParameters *widget,
                         const FocusParameters  *focus,
                         int x, int y, int width, int height)
{
	const MurrineRGB *base = &colors->base[widget->state_type];
	MurrineRGB border = colors->shade[widget->disabled ? 4 : 5];
	int radius = CLAMP (widget->roundness, 0, 3);

	murrine_shade (&border, 0.92, &border);

	if (widget->focus)
		border = focus->color;

	cairo_translate (cr, x+0.5, y+0.5);

	cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);

	/* Fill the entry's base color */
	cairo_rectangle (cr, 1.5, 1.5, width-4, height-4);
	murrine_set_color_rgba (cr, base, ENTRY_OPACITY);
	cairo_fill (cr);

	cairo_set_operator (cr, CAIRO_OPERATOR_OVER);

	if (widget->reliefstyle != 0)
		murrine_draw_inset (cr, &widget->parentbg, 0, 0, width-1, height-1, radius+1, widget->corners);

	/* Draw the focused border */
	if (widget->focus)
	{
		MurrineRGB focus_shadow;
		murrine_shade (&border, 1.54, &focus_shadow);

		cairo_rectangle (cr, 2, 2, width-5, height-5);
		murrine_set_color_rgba (cr, &focus_shadow, 0.5);
		cairo_stroke(cr);
	}
	else if (widget->mrn_gradient.gradients)
	{
		MurrineRGB highlight;
		murrine_shade (base, 1.15, &highlight);

		cairo_move_to (cr, 2, height-3);
		cairo_line_to (cr, 2, 2);
		cairo_line_to (cr, width-3, 2);

		murrine_set_color_rgba (cr, &highlight, widget->disabled ? 0.3 : 0.6);
		cairo_stroke (cr);
	}

	murrine_set_color_rgb (cr, &border);
	murrine_rounded_rectangle (cr, 1, 1, width-3, height-3, radius, widget->corners);
	cairo_stroke (cr);
}

static void
murrine_scale_draw_gradient (cairo_t *cr,
                             const MurrineRGB *c1,
                             const MurrineRGB *c2,
                             double lightborder_shade,
                             int x, int y, int width, int height,
                             boolean alpha, boolean horizontal)
{
	murrine_set_color_rgba (cr, c1, alpha ? 0.46 : 1.0);
	cairo_rectangle (cr, x, y, width, height);
	cairo_fill (cr);

	murrine_set_color_rgba (cr, c2, 0.82);
	cairo_rectangle (cr, x, y, width, height);
	cairo_stroke (cr);

	if (lightborder_shade != 1.0)
	{
		MurrineRGB lightborder;
		murrine_shade (c1, lightborder_shade, &lightborder);
		
		if (horizontal)
		{
			cairo_move_to (cr, x+1, y+height-0.5);
			cairo_rel_line_to (cr, 0, -height+1.5);
			cairo_rel_line_to (cr, width-2, 0);
			cairo_rel_line_to (cr, 0, height-1.5);
		}
		else
		{
			cairo_move_to (cr, x+width-0.5, y+1);
			cairo_rel_line_to (cr, -width+1.5, 0);
			cairo_rel_line_to (cr, 0, height-2);
			cairo_rel_line_to (cr, width-1.5, 0);
		}
		murrine_set_color_rgba (cr, &lightborder, 0.5);
		cairo_stroke (cr);
	}
}

#define TROUGH_SIZE 6
static void
murrine_rgba_draw_scale_trough (cairo_t *cr,
                                const MurrineColors    *colors,
                                const WidgetParameters *widget,
                                const SliderParameters *slider,
                                int x, int y, int width, int height)
{
	int     trough_width, trough_height;
	double  translate_x, translate_y;

	cairo_save (cr);

	if (slider->horizontal)
	{
		trough_width  = width;
		trough_height = TROUGH_SIZE;

		translate_x   = x;
		translate_y   = y+(height/2)-(TROUGH_SIZE/2);
	}
	else
	{
		trough_width  = TROUGH_SIZE;
		trough_height = height;

		translate_x   = x+(width/2)-(TROUGH_SIZE/2);
		translate_y   = y;
	}

	cairo_translate (cr, translate_x+0.5, translate_y+0.5);

	if (!slider->fill_level && widget->reliefstyle != 0)
		murrine_draw_inset (cr, &widget->parentbg, 0, 0, trough_width, trough_height, 0, 0);

	if (!slider->lower && !slider->fill_level)
	{
		MurrineRGB fill;
		murrine_shade (&widget->parentbg, 0.95, &fill);

		murrine_scale_draw_gradient (cr, &fill, &colors->shade[5],
		                             1.0,
		                             1.0, 1.0, trough_width-2, trough_height-2,
		                             TRUE, slider->horizontal);
	}
	else
	{
		murrine_scale_draw_gradient (cr, &colors->spot[1], &colors->spot[2],
		                             widget->disabled ? 1.0 : widget->lightborder_shade,
		                             1.0, 1.0, trough_width-2, trough_height-2,
		                             FALSE, slider->horizontal);
	}

	cairo_restore (cr);
}

static void
murrine_rgba_draw_progressbar_trough (cairo_t *cr,
                                      const MurrineColors    *colors,
                                      const WidgetParameters *widget,
                                      int x, int y, int width, int height)
{
	const MurrineRGB *border = &colors->shade[4];
	MurrineRGB fill;
	int roundness = MIN (widget->roundness, MIN ((height-2.0)/2.0, (width-2.0)/2.0));

	murrine_shade (&widget->parentbg, 0.95, &fill);

	/* Create trough box */
	murrine_rounded_rectangle_closed (cr, x+1, y+1, width-2, height-2, roundness, widget->corners);
	murrine_set_color_rgba (cr, &fill, 0.8);
	cairo_fill (cr);

	/* Draw border */
	murrine_rounded_rectangle (cr, x+0.5, y+0.5, width-1, height-1, roundness, widget->corners);
	murrine_set_color_rgba (cr, border, 0.8);
	cairo_stroke (cr);

	if (widget->mrn_gradient.gradients)
	{
		cairo_pattern_t  *pat;
		MurrineRGB        shadow;

		murrine_shade (border, 0.94, &shadow);

		/* clip the corners of the shadows */
		murrine_rounded_rectangle_closed (cr, x+1, y+1, width-2, height-2, roundness, widget->corners);
		cairo_clip (cr);

		/* Top shadow */
		cairo_rectangle (cr, x+1, y+1, width-2, 4);
		pat = cairo_pattern_create_linear (x, y, x, y+4);
		murrine_pattern_add_color_stop_rgba (pat, 0.0, &shadow, 0.26);
		murrine_pattern_add_color_stop_rgba (pat, 1.0, &shadow, 0.0);
		cairo_set_source (cr, pat);
		cairo_fill (cr);
		cairo_pattern_destroy (pat);

		/* Left shadow */
		cairo_rectangle (cr, x+1, y+1, 4, height-2);
		pat = cairo_pattern_create_linear (x, y, x+4, y);
		murrine_pattern_add_color_stop_rgba (pat, 0.0, &shadow, 0.26);
		murrine_pattern_add_color_stop_rgba (pat, 1.0, &shadow, 0.0);
		cairo_set_source (cr, pat);
		cairo_fill (cr);
		cairo_pattern_destroy (pat);
	}
}

static void
murrine_rgba_draw_progressbar_fill (cairo_t *cr,
                                    const MurrineColors         *colors,
                                    const WidgetParameters      *widget,
                                    const ProgressBarParameters *progressbar,
                                    int x, int y, int width, int height,
                                    gint offset)
{
	double     tile_pos = 0;
	double     stroke_width;
	int        x_step;
	int        roundness;
	const      MurrineRGB *fill = &colors->spot[1];
	MurrineRGB border = colors->spot[2];

	/* progressbar->orientation < 2 == boolean is_horizontal */
	if (progressbar->orientation < 2)
	{
		if (progressbar->orientation == MRN_ORIENTATION_LEFT_TO_RIGHT)
			rotate_mirror_translate (cr, 0, x, y, FALSE, FALSE);
		else
			rotate_mirror_translate (cr, 0, x+width, y, TRUE, FALSE);
	}
	else
	{
		int tmp = height; height = width; width = tmp;

		x = x+1; y = y-1; width = width+2; height = height-2;

		if (progressbar->orientation == MRN_ORIENTATION_TOP_TO_BOTTOM)
			rotate_mirror_translate (cr, M_PI/2, x, y, FALSE, FALSE);
		else
			rotate_mirror_translate (cr, M_PI/2, x, y+width, TRUE, FALSE);
	}

	roundness = MIN (widget->roundness, (height-2.0)/2.0);
	stroke_width = height*2;
	x_step = (((float)stroke_width/10)*offset);

	cairo_save (cr);

	cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);

	murrine_rounded_rectangle_closed (cr, 2, 1, width-4+roundness, height-2,
	                                  roundness, MRN_CORNER_TOPLEFT | MRN_CORNER_BOTTOMLEFT);
	cairo_clip (cr);
	murrine_rounded_rectangle_closed (cr, 2-roundness, 1, width-4+roundness, height-2,
	                                  roundness, MRN_CORNER_TOPRIGHT | MRN_CORNER_BOTTOMRIGHT);
	cairo_clip (cr);

	cairo_rectangle (cr, 2, 1, width-4, height-2);

	murrine_draw_glaze (cr, fill,
	                    widget->glow_shade, widget->highlight_shade, widget->lightborder_shade,
	                    widget->mrn_gradient, widget, 2, 1, width-4, height-2,
	                    roundness, widget->corners, TRUE);

	cairo_set_operator (cr, CAIRO_OPERATOR_OVER);

	switch (progressbar->style)
	{
		case 0:
			break;
		default:
		case 1:
		{
			cairo_set_operator (cr, CAIRO_OPERATOR_OVER);

			/* Draw strokes */
			while (tile_pos <= width+x_step-2)
			{
				cairo_move_to (cr, stroke_width/2-x_step, 0);
				cairo_line_to (cr, stroke_width-x_step, 0);
				cairo_line_to (cr, stroke_width/2-x_step, height);
				cairo_line_to (cr, -x_step, height);

				cairo_translate (cr, stroke_width, 0);
				tile_pos += stroke_width;
			}

			murrine_set_color_rgba (cr, &colors->spot[2], 0.15);
			cairo_fill (cr);
			break;
		}
		case 2:
		{
			MurrineRGB highlight;
			int step = 18;
			int i;

			murrine_shade (fill, widget->lightborder_shade*widget->highlight_shade, &highlight);

			for (i=step; i<width-3; i+=step)
			{
				cairo_move_to (cr, i-0.5, 1);
				cairo_line_to (cr, i-0.5, height-1);
				murrine_set_color_rgba (cr, &highlight, 0.5*widget->mrn_gradient.rgba_opacity);
				cairo_stroke (cr);

				cairo_move_to (cr, i+0.5, 1);
				cairo_line_to (cr, i+0.5, height-1);
				murrine_set_color_rgba (cr, &colors->spot[2], 0.25);
				cairo_stroke (cr);
			}
			break;
		}
	}

	cairo_restore (cr);

	cairo_save (cr);

	murrine_rounded_rectangle_closed (cr, 1, 0, width-2+roundness, height,
	                                  roundness, MRN_CORNER_TOPLEFT | MRN_CORNER_BOTTOMLEFT);
	cairo_clip (cr);
	murrine_rounded_rectangle_closed (cr, 1-roundness, 0, width-2+roundness, height,
	                                  roundness, MRN_CORNER_TOPRIGHT | MRN_CORNER_BOTTOMRIGHT);
	cairo_clip (cr);

	/* Draw the border */
	murrine_mix_color (&border, fill, 0.28, &border);
	murrine_set_color_rgb (cr, &border);
	murrine_rounded_rectangle (cr, 1.5, 0.5, width-3, height-1, roundness, widget->corners);
	cairo_stroke (cr);

	cairo_restore (cr);
}

static void
murrine_rgba_draw_menubar (cairo_t *cr,
                           const MurrineColors *colors,
                           const WidgetParameters *widget,
                           int x, int y, int width, int height,
                           int menubarstyle)
{
	const MurrineRGB *fill = &colors->bg[0];

	cairo_translate (cr, x, y);
	cairo_rectangle (cr, 0, 0, width, height);

	cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);

	/* Glass menubar */
	switch (menubarstyle)
	{
		default:
		case 0:
		{
			cairo_pattern_t *pat;

			pat = cairo_pattern_create_linear (0, 0, width, 0);
			murrine_pattern_add_color_stop_rgba (pat, 0.0, fill, MENUBAR_OPACITY);
			murrine_pattern_add_color_stop_rgba (pat, 0.5, fill, (MENUBAR_OPACITY-0.04));
			murrine_pattern_add_color_stop_rgba (pat, 1.0, fill, MENUBAR_OPACITY);
			cairo_set_source (cr, pat);
			cairo_rectangle  (cr, 0, 0, width, height);
			cairo_fill       (cr);
			cairo_pattern_destroy (pat);

			cairo_set_operator (cr, CAIRO_OPERATOR_OVER);
			break;
		}
		case 1:
		{
			/* XXX: should use another gradient rgba_opacity */
			int os = (widget->glazestyle == 2) ? 1 : 0;
			murrine_draw_glaze (cr, fill,
			                    widget->glow_shade, widget->highlight_shade, widget->glazestyle == 2 ? widget->lightborder_shade : 1.0,
			                    widget->mrn_gradient, widget, os, os, width-os*2, height-os*2,
			                    widget->roundness, widget->corners, TRUE);
			break;
		}
		case 2:
		{
			cairo_pattern_t *pat;
			MurrineRGB lower;
			murrine_shade (fill, 0.95, &lower);

			pat = cairo_pattern_create_linear (0, 0, 0, height);
			murrine_pattern_add_color_stop_rgba (pat, 0.0, fill, MENUBAR_OPACITY);
			murrine_pattern_add_color_stop_rgba (pat, 1.0, &lower, MENUBAR_OPACITY);
			cairo_set_source (cr, pat);
			cairo_fill (cr);
			cairo_pattern_destroy (pat);

			cairo_set_operator (cr, CAIRO_OPERATOR_OVER);
			break;
		}
		case 3:
		{
			cairo_pattern_t *pat;
			MurrineRGB low, top;
			murrine_shade (fill, 0.9, &top);
			murrine_shade (fill, 1.1, &low);
			pat = cairo_pattern_create_linear (0, 0, 0, height);
			murrine_pattern_add_color_stop_rgba (pat, 0.0, &top, MENUBAR_STRIPED_OPACITY);
			murrine_pattern_add_color_stop_rgba (pat, 1.0, &low, MENUBAR_STRIPED_OPACITY);
			cairo_set_source (cr, pat);
			cairo_fill (cr);

			cairo_set_operator (cr, CAIRO_OPERATOR_OVER);

			cairo_pattern_destroy (pat);
			int counter = -height;
			murrine_shade (&low, 0.9, &low);
			murrine_set_color_rgba (cr, &low, MENUBAR_STRIPED_OPACITY);
			while (counter < width)
			{
				cairo_move_to (cr, counter, height);
				cairo_line_to (cr, counter+height, 0);
				cairo_stroke  (cr);
				counter += 5;
			}
			break;
		}
	}

	/* Draw bottom line */
	if (menubarstyle == 1 && widget->glazestyle == 2)
		cairo_rectangle (cr, 0.5, 0.5, width-1, height-1);
	else
	{
		cairo_move_to        (cr, 0, height-0.5);
		cairo_line_to        (cr, width, height-0.5);
	}

	murrine_set_color_rgb (cr, &colors->shade[4]);
	cairo_stroke          (cr);
}

static void
murrine_rgba_draw_toolbar (cairo_t *cr,
                           const MurrineColors    *colors,
                           const WidgetParameters *widget,
                           const ToolbarParameters *toolbar,
                           int x, int y, int width, int height)
{
	const MurrineRGB *dark = &colors->shade[4];
	const MurrineRGB *fill = &colors->bg[0];
	const MurrineRGB *top  = &colors->shade[0];

	cairo_translate (cr, x, y);
	cairo_rectangle (cr, 0, 0, width, height);

	cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);

	/* Glass toolbar */
	switch (toolbar->style)
	{
		default:
		case 0:
			murrine_set_color_rgba (cr, fill, TOOLBAR_OPACITY);
			cairo_fill (cr);

			cairo_set_operator (cr, CAIRO_OPERATOR_OVER);

			/* Draw highlight */
			if (!toolbar->topmost)
			{
				murrine_set_color_rgba (cr, top, 0.5);
				cairo_move_to          (cr, 0, 0.5);
				cairo_line_to          (cr, width, 0.5);
				cairo_stroke           (cr);
			}
			break;
		case 1:
		{
			int os = (widget->glazestyle == 2) ? 1 : 0;
			murrine_draw_glaze (cr, fill,
			                    widget->glow_shade, widget->highlight_shade, widget->glazestyle == 2 ? widget->lightborder_shade : 1.0,
			                    widget->mrn_gradient, widget, os, os, width-os*2, height-os*2,
			                    widget->roundness, widget->corners, TRUE);

			cairo_set_operator (cr, CAIRO_OPERATOR_OVER);
			break;
		}
		case 2:
		{
			cairo_pattern_t *pat;
			MurrineRGB lower;
			murrine_shade (fill, 0.95, &lower);
			pat = cairo_pattern_create_linear (0, 0, 0, height);
			murrine_pattern_add_color_stop_rgba (pat, 0.0, fill, TOOLBAR_OPACITY);
			murrine_pattern_add_color_stop_rgba (pat, 1.0, &lower, TOOLBAR_OPACITY);
			cairo_set_source (cr, pat);
			cairo_pattern_destroy (pat);
			cairo_fill (cr);

			cairo_set_operator (cr, CAIRO_OPERATOR_OVER);

			/* Draw highlight */
			if (!toolbar->topmost)
			{
				cairo_move_to         (cr, 0, 0.5);
				cairo_line_to         (cr, width, 0.5);
				murrine_set_color_rgb (cr, top);
				cairo_stroke          (cr);
			}
			break;
		}
	}

	/* Draw shadow */
	murrine_set_color_rgb (cr, dark);
	/* Draw bottom line */
	if (toolbar->style == 1 && widget->glazestyle == 2)
		cairo_rectangle (cr, 0.5, 0.5, width-1, height-1);
	else
	{
		cairo_move_to        (cr, 0, height-0.5);
		cairo_line_to        (cr, width, height-0.5);
	}
	cairo_stroke          (cr);
}

static void
murrine_get_frame_gap_clip (int x, int y, int width, int height,
                            const FrameParameters *frame,
                            MurrineRectangle      *bevel,
                            MurrineRectangle      *border)
{
	switch (frame->gap_side)
	{
		case MRN_GAP_TOP:
			MURRINE_RECTANGLE_SET ((*bevel),  1.5+frame->gap_x, -0.5,
			                       frame->gap_width-3, 2.0);
			MURRINE_RECTANGLE_SET ((*border), 0.5+frame->gap_x, -0.5,
			                       frame->gap_width-2, 2.0);
			break;
		case MRN_GAP_BOTTOM:
			MURRINE_RECTANGLE_SET ((*bevel),  1.5+frame->gap_x, height-2.5,
			                       frame->gap_width-3, 2.0);
			MURRINE_RECTANGLE_SET ((*border), 0.5+frame->gap_x, height-1.5,
			                       frame->gap_width-2, 2.0);
			break;
		case MRN_GAP_LEFT:
			MURRINE_RECTANGLE_SET ((*bevel),  -0.5, 1.5+frame->gap_x,
			                       2.0, frame->gap_width-3);
			MURRINE_RECTANGLE_SET ((*border), -0.5, 0.5+frame->gap_x,
			                       1.0, frame->gap_width-2);
			break;
		case MRN_GAP_RIGHT:
			MURRINE_RECTANGLE_SET ((*bevel),  width-2.5, 1.5+frame->gap_x,
			                       2.0, frame->gap_width-3);
			MURRINE_RECTANGLE_SET ((*border), width-1.5, 0.5+frame->gap_x,
			                       1.0, frame->gap_width-2);
			break;
	}
}

static void
murrine_rgba_draw_frame (cairo_t *cr,
                         const MurrineColors    *colors,
                         const WidgetParameters *widget,
                         const FrameParameters  *frame,
                         int x, int y, int width, int height)
{
	MurrineRGB *border = frame->border;
	MurrineRectangle bevel_clip;
	MurrineRectangle frame_clip;
	const MurrineRGB *dark = &colors->shade[5];
	MurrineRGB highlight, shadow_color;

	murrine_shade (&colors->bg[0], 1.15, &highlight);
	murrine_shade (&colors->bg[0], 0.4, &shadow_color);

	if (frame->shadow == MRN_SHADOW_NONE)
		return;

	if (frame->gap_x != -1)
		murrine_get_frame_gap_clip (x, y, width, height,
		                            frame, &bevel_clip, &frame_clip);

	cairo_translate      (cr, x+0.5, y+0.5);

	/* save everything */
	cairo_save (cr);

	/* Set clip for the bevel */
	if (frame->gap_x != -1)
	{
		/* Set clip for gap */
		cairo_set_fill_rule (cr, CAIRO_FILL_RULE_EVEN_ODD);
		cairo_rectangle     (cr, -0.5, -0.5, width, height);
		cairo_rectangle     (cr, bevel_clip.x, bevel_clip.y, bevel_clip.width, bevel_clip.height);
		cairo_clip          (cr);
	}

	/* Draw the bevel */
	if (frame->shadow == MRN_SHADOW_ETCHED_IN || frame->shadow == MRN_SHADOW_ETCHED_OUT)
	{
		if (frame->shadow == MRN_SHADOW_ETCHED_IN)
			murrine_rounded_rectangle (cr, 1, 1, width-2, height-2, widget->roundness, widget->corners);
		else
			murrine_rounded_rectangle (cr, 0, 0, width-2, height-2, widget->roundness, widget->corners);
		murrine_set_color_rgba (cr, &highlight, 0.5);
		cairo_stroke (cr);
	}
	else if (frame->shadow != MRN_SHADOW_FLAT)
	{
		ShadowParameters shadow;
		shadow.corners = widget->corners;
		shadow.shadow  = frame->shadow;
		murrine_draw_highlight_and_shade (cr, colors, &shadow, width, height, widget->roundness-1);
	}

	/* restore the previous clip region */
	cairo_restore (cr);
	cairo_save    (cr);
	if (frame->gap_x != -1)
	{
		/* Set clip for gap */
		cairo_set_fill_rule (cr, CAIRO_FILL_RULE_EVEN_ODD);
		cairo_rectangle     (cr, -0.5, -0.5, width, height);
		cairo_rectangle     (cr, frame_clip.x, frame_clip.y, frame_clip.width, frame_clip.height);
		cairo_clip          (cr);
	}

	/* Draw frame */
	if (frame->shadow == MRN_SHADOW_ETCHED_IN || frame->shadow == MRN_SHADOW_ETCHED_OUT)
	{
		murrine_set_color_rgb (cr, dark);
		if (frame->shadow == MRN_SHADOW_ETCHED_IN)
			murrine_rounded_rectangle (cr, 0, 0, width-2, height-2, widget->roundness, widget->corners);
		else
			murrine_rounded_rectangle (cr, 1, 1, width-2, height-2, widget->roundness, widget->corners);
	}
	else
	{
		murrine_set_color_rgb (cr, border);
		murrine_rounded_rectangle (cr, 0, 0, width-1, height-1, widget->roundness, widget->corners);
	}
	cairo_stroke  (cr);
	cairo_restore (cr);
}

static void
murrine_rgba_draw_separator (cairo_t *cr,
                             const MurrineColors       *colors,
                             const WidgetParameters    *widget,
                             const SeparatorParameters *separator,
                             int x, int y, int width, int height)
{
	const MurrineRGB *dark      = &colors->shade[4];
	const MurrineRGB *highlight = &colors->shade[0];

	if (separator->horizontal)
	{
		cairo_translate       (cr, x, y+0.5);

		cairo_move_to         (cr, 0.0,     0.0);
		cairo_line_to         (cr, width+1, 0.0);
		murrine_set_color_rgb (cr, dark);
		cairo_stroke          (cr);

		cairo_move_to          (cr, 0.0,   1.0);
		cairo_line_to          (cr, width, 1.0);
		murrine_set_color_rgba (cr, highlight, 0.5);
		cairo_stroke           (cr);
	}
	else
	{
		cairo_translate       (cr, x+0.5, y);

		cairo_move_to         (cr, 0.0, 0.0);
		cairo_line_to         (cr, 0.0, height);
		murrine_set_color_rgb (cr, dark);
		cairo_stroke          (cr);

		cairo_move_to          (cr, 1.0, 0.0);
		cairo_line_to          (cr, 1.0, height);
		murrine_set_color_rgba (cr, highlight, 0.5);
		cairo_stroke           (cr);
	}
}

static void
murrine_rgba_draw_tab (cairo_t *cr,
                       const MurrineColors    *colors,
                       const WidgetParameters *widget,
                       const TabParameters    *tab,
                       int x, int y, int width, int height)
{
	const MurrineRGB *stripe_fill = &colors->spot[1];
	const MurrineRGB *stripe_border = &colors->spot[2];
	const MurrineRGB *fill = &colors->bg[widget->state_type];
	const MurrineRGB *border = &colors->shade[!widget->active ? 5 : 4];
	cairo_pattern_t  *pat;

	/* Set clip */
	cairo_rectangle (cr, x, y, width, height);
	cairo_clip      (cr);
	cairo_new_path  (cr);

	cairo_translate      (cr, x+0.5, y+0.5);

	cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);

	/* Make the tabs slightly bigger than they should be, to create a gap */
	/* And calculate the strip size too, while you're at it */
	if (tab->gap_side == MRN_GAP_TOP || tab->gap_side == MRN_GAP_BOTTOM)
	{
		height += 3.0;

		if (tab->gap_side == MRN_GAP_TOP)
			cairo_translate (cr, 0.0, -3.0); /* gap at the other side */
	}
	else
	{
		width += 3.0;

		if (tab->gap_side == MRN_GAP_LEFT)
			cairo_translate (cr, -3.0, 0.0); /* gap at the other side */
	}

	/* Set tab shape */
	murrine_rounded_rectangle_closed (cr, 0, 0, width-1, height-1, widget->roundness, widget->corners);

	/* Draw fill */
	if (!widget->active)
	{
		murrine_set_color_rgba (cr, fill, NOTEBOOK_OPACITY);
		cairo_fill (cr);
	}

	if (widget->active)
	{
		MurrineRGB shade1, shade2, shade3, shade4, highlight;
		MurrineGradients mrn_gradient_custom = mrn_gradient_custom = get_decreased_gradient_shades (widget->mrn_gradient, 3.0);
		double highlight_shade_custom = get_decreased_shade (widget->highlight_shade, 2.0);
		double lightborder_shade_custom = get_decreased_shade (widget->lightborder_shade, 2.0);

		murrine_shade (fill, mrn_gradient_custom.gradient_shades[0]*highlight_shade_custom, &shade1);
		murrine_shade (fill, mrn_gradient_custom.gradient_shades[1]*highlight_shade_custom, &shade2);
		murrine_shade (fill, mrn_gradient_custom.gradient_shades[2], &shade3);
		murrine_shade (fill, mrn_gradient_custom.gradient_shades[3], &shade4);

		switch (tab->gap_side)
		{
			case MRN_GAP_TOP:
				pat = cairo_pattern_create_linear (0, height-2, 0, 0);
				break;
			case MRN_GAP_BOTTOM:
				pat = cairo_pattern_create_linear (0, 1, 0, height);
				break;
			case MRN_GAP_LEFT:
				pat = cairo_pattern_create_linear (width-2, 0, 1, 0);
				break;
			case MRN_GAP_RIGHT:
				pat = cairo_pattern_create_linear (1, 0, width-2, 0);
				break;
		}

		murrine_rounded_rectangle_closed (cr, 0, 0, width-1, height-1, widget->roundness, widget->corners);

		murrine_pattern_add_color_stop_rgba (pat, 0.00, &shade1, NOTEBOOK_OPACITY);
		murrine_pattern_add_color_stop_rgba (pat, 0.45, &shade2, NOTEBOOK_OPACITY);
		murrine_pattern_add_color_stop_rgba (pat, 0.45, &shade3, NOTEBOOK_OPACITY);
		murrine_pattern_add_color_stop_rgba (pat, 1.00, &shade4, NOTEBOOK_OPACITY);
		cairo_set_source (cr, pat);
		cairo_fill (cr);
		cairo_pattern_destroy (pat);

		/* Draw lightborder */
		cairo_set_operator (cr, CAIRO_OPERATOR_OVER);

		murrine_shade (fill, lightborder_shade_custom*highlight_shade_custom, &highlight);
		murrine_shade (&highlight, mrn_gradient_custom.gradient_shades[0]*highlight_shade_custom, &shade1);
		murrine_shade (&highlight, mrn_gradient_custom.gradient_shades[1]*highlight_shade_custom, &shade2);
		murrine_shade (&highlight, mrn_gradient_custom.gradient_shades[2], &shade3);
		murrine_shade (&highlight, mrn_gradient_custom.gradient_shades[3], &shade4);

		switch (tab->gap_side)
		{
			case MRN_GAP_TOP:
				pat = cairo_pattern_create_linear (0, height-2, 0, 0);
				break;
			case MRN_GAP_BOTTOM:
				pat = cairo_pattern_create_linear (0, 1, 0, height);
				break;
			case MRN_GAP_LEFT:
				pat = cairo_pattern_create_linear (width-2, 0, 1, 0);
				break;
			case MRN_GAP_RIGHT:
				pat = cairo_pattern_create_linear (1, 0, width-2, 0);
				break;
		}

		murrine_rounded_rectangle_closed (cr, 1, 1, width-3, height-3, widget->roundness, widget->corners);

		murrine_pattern_add_color_stop_rgba (pat, 0.00, &shade1, 0.5*NOTEBOOK_OPACITY);
		murrine_pattern_add_color_stop_rgba (pat, 0.45, &shade2, 0.5*NOTEBOOK_OPACITY);
		murrine_pattern_add_color_stop_rgba (pat, 0.45, &shade3, 0.5*NOTEBOOK_OPACITY);
		murrine_pattern_add_color_stop_rgba (pat, 1.00, &shade4, 0.5*NOTEBOOK_OPACITY);
		cairo_set_source (cr, pat);
		cairo_stroke (cr);
		cairo_pattern_destroy (pat);
	}
	else
	{
		MurrineRGB shade1, shade2, shade3, shade4, highlight;
		MurrineGradients mrn_gradient_custom = mrn_gradient_custom = get_decreased_gradient_shades (widget->mrn_gradient, 3.0);
		double highlight_shade_custom = get_decreased_shade (widget->highlight_shade, 2.0);

		murrine_shade (fill, mrn_gradient_custom.gradient_shades[0]*highlight_shade_custom, &shade1);
		murrine_shade (fill, mrn_gradient_custom.gradient_shades[1]*highlight_shade_custom, &shade2);
		murrine_shade (fill, mrn_gradient_custom.gradient_shades[2], &shade3);
		murrine_shade (fill, 1.0, &shade4); /* this value should change as draw_frame */

		/* Draw shade */
		switch (tab->gap_side)
		{
			case MRN_GAP_TOP:
				pat = cairo_pattern_create_linear (0, height-2, 0, 0);
				break;
			case MRN_GAP_BOTTOM:
				pat = cairo_pattern_create_linear (0, 0, 0, height);
				break;
			case MRN_GAP_LEFT:
				pat = cairo_pattern_create_linear (width-2, 0, 0, 0);
				break;
			case MRN_GAP_RIGHT:
				pat = cairo_pattern_create_linear (0, 0, width, 0);
				break;
		}

		murrine_rounded_rectangle_closed (cr, 0, 0, width-1, height-1, widget->roundness, widget->corners);

		murrine_pattern_add_color_stop_rgba (pat, 0.00, &shade1, NOTEBOOK_OPACITY);
		murrine_pattern_add_color_stop_rgba (pat, 0.45, &shade2, NOTEBOOK_OPACITY);
		murrine_pattern_add_color_stop_rgba (pat, 0.45, &shade3, NOTEBOOK_OPACITY);
		murrine_pattern_add_color_stop_rgba (pat, 1.00, &shade4, NOTEBOOK_OPACITY);
		cairo_set_source (cr, pat);
		cairo_fill (cr);
		cairo_pattern_destroy (pat);

		/* Draw lightborder */
		cairo_set_operator (cr, CAIRO_OPERATOR_OVER);

		murrine_shade (fill, widget->lightborder_shade*highlight_shade_custom, &highlight);
		murrine_shade (&highlight, mrn_gradient_custom.gradient_shades[0]*highlight_shade_custom, &shade1);
		murrine_shade (&highlight, mrn_gradient_custom.gradient_shades[1]*highlight_shade_custom, &shade2);
		murrine_shade (&highlight, mrn_gradient_custom.gradient_shades[2], &shade3);
		murrine_shade (fill, 1.15, &shade4); /* this value should change as draw_frame */

		switch (tab->gap_side)
		{
			case MRN_GAP_TOP:
				pat = cairo_pattern_create_linear (0, height-2, 0, 0);
				break;
			case MRN_GAP_BOTTOM:
				pat = cairo_pattern_create_linear (0, 0, 0, height);
				break;
			case MRN_GAP_LEFT:
				pat = cairo_pattern_create_linear (width-2, 0, 0, 0);
				break;
			case MRN_GAP_RIGHT:
				pat = cairo_pattern_create_linear (0, 0, width, 0);
				break;
		}

		murrine_rounded_rectangle_closed (cr, 1, 1, width-3, height-3, widget->roundness, widget->corners);

		murrine_pattern_add_color_stop_rgba (pat, 0.00, &shade1, 0.5*NOTEBOOK_OPACITY);
		murrine_pattern_add_color_stop_rgba (pat, 0.45, &shade2, 0.5*NOTEBOOK_OPACITY);
		murrine_pattern_add_color_stop_rgba (pat, 0.45, &shade3, 0.5*NOTEBOOK_OPACITY);
		murrine_pattern_add_color_stop_rgba (pat, 1.00, &shade4, 0.5*NOTEBOOK_OPACITY);
		cairo_set_source (cr, pat);
		cairo_stroke (cr);
		cairo_pattern_destroy (pat);
	}

	murrine_rounded_rectangle (cr, 0, 0, width-1, height-1, widget->roundness, widget->corners);
	murrine_set_color_rgb  (cr, border);
	cairo_stroke (cr);
}

static void
murrine_rgba_draw_scrollbar_trough (cairo_t *cr,
                                    const MurrineColors       *colors,
                                    const WidgetParameters    *widget,
                                    const ScrollBarParameters *scrollbar,
                                    int x, int y, int width, int height)
{
	const MurrineRGB *border = &colors->shade[scrollbar->stepperstyle < 1 ? 4 : 5];
	MurrineRGB fill;

	murrine_shade (&widget->parentbg, scrollbar->stepperstyle < 1 ? 0.95 : 1.065, &fill);

	if (!scrollbar->horizontal)
	{
		cairo_translate (cr, x, y);
	}
	else
	{
		int tmp = height;
		rotate_mirror_translate (cr, M_PI/2, x, y, FALSE, FALSE);
		height = width;
		width = tmp;
	}

	/* Draw fill */
	murrine_set_color_rgba (cr, &fill, 0.4);
	clearlooks_rounded_rectangle (cr, 1, 0, width-2, height, widget->roundness, widget->corners);
	cairo_fill (cr);

	/* Draw border */
	murrine_set_color_rgba (cr, border, 0.82);
	murrine_rounded_rectangle (cr, 0.5, 0.5, width-1, height-1, widget->roundness, widget->corners);
	cairo_stroke (cr);
}

void
murrine_rgba_draw_scrollbar_stepper (cairo_t *cr,
                                     const MurrineColors       *colors,
                                     const WidgetParameters    *widget,
                                     const ScrollBarParameters *scrollbar,
                                     int x, int y, int width, int height)
{
	const MurrineRGB *fill  = &colors->bg[widget->state_type];
	MurrineRGB border_normal;

	murrine_shade (&colors->shade[7], 0.95, &border_normal);

	if (!scrollbar->horizontal)
		murrine_exchange_axis (cr, &x, &y, &width, &height);

	/* Border color */
	murrine_mix_color (&border_normal, fill, 0.45, &border_normal);

	cairo_translate (cr, x, y);

	cairo_save (cr);

	cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);

	murrine_rounded_rectangle_closed (cr, 1, 1, width-2, height-2, widget->roundness-1, widget->corners);
	cairo_clip_preserve(cr);

	murrine_draw_glaze (cr, fill,
	                    widget->glow_shade, widget->highlight_shade, widget->lightborder_shade,
	                    widget->mrn_gradient, widget, 1, 1, width-2, height-2,
	                    widget->roundness, widget->corners, TRUE);

	cairo_restore (cr);

	murrine_set_color_rgb (cr, &border_normal);
	murrine_rounded_rectangle (cr, 0.5, 0.5, width-1, height-1, widget->roundness, widget->corners);
	cairo_stroke (cr);
}

void
murrine_rgba_draw_scrollbar_slider (cairo_t *cr,
                                    const MurrineColors       *colors,
                                    const WidgetParameters    *widget,
                                    const ScrollBarParameters *scrollbar,
                                    int x, int y, int width, int height)
{
	if (scrollbar->stepperstyle < 1)
	{
		if (scrollbar->junction & MRN_JUNCTION_BEGIN)
		{
			if (scrollbar->horizontal)
			{
				x -= 1;
				width += 1;
			}
			else
			{
				y -= 1;
				height += 1;
			}
		}
		if (scrollbar->junction & MRN_JUNCTION_END)
		{
			if (scrollbar->horizontal)
				width += 1;
			else
				height += 1;
		}
	}

	/* Set colors */
	MurrineRGB fill = scrollbar->has_color ? scrollbar->color : colors->bg[0];
	MurrineRGB border;

	murrine_shade (&colors->shade[7], 0.95, &border);

	if (widget->prelight)
		murrine_shade (&fill, 1.04, &fill);

	/* Draw the border */
	murrine_mix_color (&border, &fill, 0.5, &border);

	if (scrollbar->horizontal)
		cairo_translate (cr, x, y);
	else
	{
		int tmp = height;
		rotate_mirror_translate (cr, M_PI/2, x, y, FALSE, FALSE);
		height = width;
		width = tmp;
	}

	cairo_save (cr);

	cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);

	murrine_rounded_rectangle_closed (cr, 1, 1, width-2, height-2, widget->roundness-1, widget->corners);
	cairo_clip_preserve (cr);

	murrine_draw_glaze (cr, &fill,
	                    widget->glow_shade, widget->highlight_shade, widget->lightborder_shade,
	                    widget->mrn_gradient, widget, 1, 1, width-2, height-2,
	                    widget->roundness, widget->corners, TRUE);

	/* Draw the options */
	MurrineRGB style;
	if (scrollbar->style > 0)
		murrine_shade (&fill, 0.55, &style);

	cairo_set_operator (cr, CAIRO_OPERATOR_OVER);

	switch (scrollbar->style)
	{
		case 1:
		{
			int circ_radius = 2;
			int circ_space = 5;
			int i;
			int x1 = circ_space+circ_radius;
			int y1 = height/2;
			for (i = circ_space; i < width-circ_space; i += 2*circ_radius+circ_space)
			{
				cairo_move_to (cr, i, 1);
				cairo_arc (cr, x1, y1, circ_radius, 0, M_PI*2);

				x1 += 2*circ_radius+circ_space;

				cairo_close_path (cr);
				murrine_set_color_rgba (cr, &style, 0.15);
				cairo_fill (cr);
			}
			break;
		}
		case 3:
		case 4:
		{
			int counter = -width;
			cairo_save (cr);
			cairo_rectangle (cr, 1, 1, width-2, height-2);
			cairo_clip (cr);
			cairo_new_path (cr);
			cairo_set_line_width (cr, 5.0); /* stroke width */
			murrine_set_color_rgba (cr, &style, 0.08);
			while (counter < height)
			{
				cairo_move_to (cr, width, counter);
				cairo_line_to (cr, 0, counter+width);
				cairo_stroke  (cr);
				counter += 12;
			}
			cairo_restore (cr);
			break;
		}
		case 5:
		case 6:
		{
			int stroke_width = 7;
			int stroke_space = 5;
			int i;
			murrine_set_color_rgba (cr, &style, 0.08);
			for (i = stroke_space; i < width-stroke_space; i += stroke_width+stroke_space)
			{
				cairo_move_to (cr, i, 1);
				cairo_rel_line_to (cr, 0, height-2);
				cairo_rel_line_to (cr, stroke_width, 0);
				cairo_rel_line_to (cr, 0, -(height-2));
				cairo_fill (cr);
			}
			break;
		}
	}
	/* Draw the handle */
	if (scrollbar->style > 0 && scrollbar->style % 2 == 0)
	{
		double bar_x = width/2-3.5;
		int i;
		for (i=0; i<3; i++)
		{
			cairo_move_to (cr, bar_x, 5);
			cairo_line_to (cr, bar_x, height-5);
			murrine_set_color_rgb (cr, &border);
			cairo_stroke (cr);

			bar_x += 3;
		}
	}

	cairo_restore (cr);

	murrine_set_color_rgb (cr, &border);
	murrine_rounded_rectangle (cr, 0.5, 0.5, width-1, height-1, widget->roundness, widget->corners);
	cairo_stroke (cr);
}

static void
murrine_rgba_draw_tooltip (cairo_t *cr,
                           const MurrineColors    *colors,
                           const WidgetParameters *widget,
                           int x, int y, int width, int height)
{
	MurrineRGB border, highlight;
	MurrineGradients mrn_gradient_custom = mrn_gradient_custom = get_decreased_gradient_shades (widget->mrn_gradient, 2.0);

	murrine_shade (&colors->bg[widget->state_type], 0.6, &border);
	murrine_shade (&colors->bg[widget->state_type], 1.0, &highlight);

	cairo_save (cr);

	cairo_translate (cr, x, y);

	cairo_rectangle (cr, 0, 0, width, height);
	cairo_set_operator (cr, CAIRO_OPERATOR_CLEAR);
	cairo_fill (cr);
	cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);

	murrine_set_gradient (cr, &colors->bg[widget->state_type], mrn_gradient_custom, 0, 0, 0, height, widget->mrn_gradient.gradients, FALSE);
	clearlooks_rounded_rectangle (cr, 0, 0, width, height, widget->roundness > 1 ? widget->roundness+1 : 0, widget->corners);
	cairo_fill (cr);

	murrine_set_gradient (cr, &highlight, mrn_gradient_custom, 0, 0, 0, height, widget->mrn_gradient.gradients, TRUE);
	clearlooks_rounded_rectangle (cr, 0, 0, width, height/2, widget->roundness > 1 ? widget->roundness+1 : 0,
	                              widget->corners == MRN_CORNER_ALL ? MRN_CORNER_TOPLEFT | MRN_CORNER_TOPRIGHT : MRN_CORNER_NONE);
	cairo_fill (cr);

	murrine_set_color_rgb (cr, &border);
	murrine_rounded_rectangle (cr, 0.5, 0.5, width-1, height-1, widget->roundness, widget->corners);
	cairo_stroke (cr);

	cairo_restore (cr);
}

static void
murrine_rgba_draw_handle (cairo_t *cr,
                          const MurrineColors    *colors,
                          const WidgetParameters *widget,
                          const HandleParameters *handle,
                          int x, int y, int width, int height)
{
	const MurrineRGB *dark  = &colors->shade[5];

	int bar_height;
	int bar_width  = 4;
	int i, bar_y = 1;
	int num_bars, bar_spacing;
	num_bars    = 3;
	bar_spacing = 3;
	bar_height = num_bars*bar_spacing;

	if (handle->horizontal)
	{
		int tmp = height;
		rotate_mirror_translate (cr, M_PI/2, x+0.5+width/2-bar_height/2, y+height/2-bar_width/2, FALSE, FALSE);
		height = width;
		width = tmp;
	}
	else
	{
		cairo_translate (cr, x+width/2-bar_width/2, y+height/2-bar_height/2+0.5);
	}

	for (i=0; i<num_bars; i++)
	{
		murrine_set_color_rgb (cr, dark);
		cairo_move_to (cr, 0, bar_y);
		cairo_line_to (cr, bar_width, bar_y);
		cairo_stroke (cr);

		bar_y += bar_spacing;
	}
}


static void
murrine_rgba_draw_radiobutton (cairo_t *cr,
                               const MurrineColors      *colors,
                               const WidgetParameters   *widget,
                               const CheckboxParameters *checkbox,
                               int x, int y, int width, int height,
                               double trans)
{
	MurrineRGB border;
	const MurrineRGB *dot;
	const MurrineRGB *bg = &colors->base[0];
	gboolean inconsistent = FALSE;
	gboolean draw_bullet = (checkbox->shadow_type == GTK_SHADOW_IN);
	int roundness = 5;
	double highlight_shade_custom = widget->highlight_shade;
	double lightborder_shade_custom = widget->lightborder_shade;
	MurrineGradients mrn_gradient_custom = widget->mrn_gradient;

	inconsistent = (checkbox->shadow_type == GTK_SHADOW_ETCHED_IN);
	draw_bullet |= inconsistent;

	width = height = 14;

	if (widget->state_type == GTK_STATE_INSENSITIVE)
	{
		border = colors->shade[4];
		dot    = &colors->shade[4];
		bg     = &colors->bg[0];

		mrn_gradient_custom = get_decreased_gradient_shades (widget->mrn_gradient, 3.0);
		highlight_shade_custom = get_decreased_shade (widget->highlight_shade, 2.0);
		lightborder_shade_custom = get_decreased_shade (widget->lightborder_shade, 2.0);
	}
	else
	{
		border = colors->shade[6];
		if (draw_bullet)
		{
			border = colors->spot[2];
			bg     = &colors->spot[1];
		}
		dot    = &colors->text[widget->state_type];
	}
	murrine_mix_color (&border, widget->state_type != GTK_STATE_INSENSITIVE ?
	                   draw_bullet ? &colors->spot[1] : &colors->bg[0] : &colors->bg[0], 0.2, &border);

	cairo_translate (cr, x, y);

	if (widget->xthickness > 2 && widget->ythickness > 2)
	{
		if (widget->reliefstyle > 1 && draw_bullet)
		{
			MurrineRGB shadow;
			murrine_shade (&border, 0.9, &shadow);

			murrine_set_color_rgba (cr, &shadow, 0.08);
			murrine_rounded_rectangle (cr, 0.5, 0.5, width-1, height-1, roundness+1, widget->corners);
			cairo_stroke (cr);
		}
		else if (widget->reliefstyle != 0)
			murrine_draw_inset (cr, &widget->parentbg, 0.5, 0.5, width-1, height-1, roundness+1, widget->corners);
	}

	cairo_save (cr);

	murrine_rounded_rectangle_closed (cr, 1.5, 1.5, width-3, height-3, roundness, widget->corners);
	cairo_clip_preserve (cr);

	if (draw_bullet)
	{
		murrine_draw_glaze (cr, bg,
		                    widget->glow_shade, highlight_shade_custom, lightborder_shade_custom,
		                    mrn_gradient_custom, widget, 2, 2, width-4, height-4,
		                    roundness, widget->corners, TRUE);
	}
	else
	{
		murrine_set_color_rgb (cr, bg);
		cairo_fill (cr);
	}

	cairo_restore (cr);

	murrine_rounded_rectangle_closed (cr, 1.5, 1.5, width-3, height-3, roundness, widget->corners);
	murrine_set_color_rgb (cr, &border);
	cairo_stroke (cr);

	if (draw_bullet)
	{
		if (inconsistent)
		{
			cairo_save (cr);
			cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
			cairo_set_line_width (cr, 2.0);

			murrine_set_color_rgba (cr, dot, trans);
			cairo_move_to(cr, 5, 7);
			cairo_line_to(cr, 9, 7);
			cairo_stroke (cr);
			cairo_restore (cr);
		}
		else
		{
			murrine_set_color_rgba (cr, dot, trans);
			cairo_arc (cr, 7, 7, 2.5, 0, G_PI*2);
			cairo_fill (cr);
		}
	}
}

static void
murrine_rgba_draw_checkbox (cairo_t *cr,
                            const MurrineColors      *colors,
                            const WidgetParameters   *widget,
                            const CheckboxParameters *checkbox,
                            int x, int y, int width, int height,
                            double trans)
{
	MurrineRGB border;
	const MurrineRGB *dot;
	const MurrineRGB *bg = &colors->base[0];
	gboolean inconsistent = FALSE;
	gboolean draw_bullet = (checkbox->shadow_type == GTK_SHADOW_IN);
	int roundness = CLAMP (widget->roundness, 0, 2);
	double highlight_shade_custom = widget->highlight_shade;
	double lightborder_shade_custom = widget->lightborder_shade;
	MurrineGradients mrn_gradient_custom = widget->mrn_gradient;

	inconsistent = (checkbox->shadow_type == GTK_SHADOW_ETCHED_IN);
	draw_bullet |= inconsistent;

	width = height = 14;

	if (widget->state_type == GTK_STATE_INSENSITIVE)
	{
		border = colors->shade[4];
		dot    = &colors->shade[4];
		bg     = &colors->bg[0];

		mrn_gradient_custom = get_decreased_gradient_shades (widget->mrn_gradient, 3.0);
		highlight_shade_custom = get_decreased_shade (widget->highlight_shade, 2.0);
		lightborder_shade_custom = get_decreased_shade (widget->lightborder_shade, 2.0);
	}
	else
	{
		border = colors->shade[6];
		if (draw_bullet)
		{
			border = colors->spot[2];
			bg     = &colors->spot[1];
		}
		dot    = &colors->text[widget->state_type];
	}
	murrine_mix_color (&border, widget->state_type != GTK_STATE_INSENSITIVE ?
	                   draw_bullet ? &colors->spot[1] : &colors->bg[0] : &colors->bg[0], 0.2, &border);

	cairo_translate (cr, x, y);

	if (widget->xthickness > 2 && widget->ythickness > 2)
	{
		if (widget->reliefstyle > 1 && draw_bullet)
		{
			MurrineRGB shadow;
			murrine_shade (&border, 0.9, &shadow);

			murrine_set_color_rgba (cr, &shadow, 0.08);
			murrine_rounded_rectangle (cr, 0.5, 0.5, width-1, height-1, roundness+1, widget->corners);
			cairo_stroke (cr);
		}
		else if (widget->reliefstyle != 0)
			murrine_draw_inset (cr, &widget->parentbg, 0.5, 0.5, width-1, height-1, roundness+1, widget->corners);
	}

	cairo_save (cr);

	murrine_rounded_rectangle_closed (cr, 1.5, 1.5, width-3, height-3, roundness, widget->corners);
	cairo_clip_preserve (cr);

	if (draw_bullet)
	{
		murrine_draw_glaze (cr, bg,
		                    widget->glow_shade, highlight_shade_custom, lightborder_shade_custom,
		                    mrn_gradient_custom, widget, 2, 2, width-4, height-4,
		                    roundness, widget->corners, TRUE);
	}
	else
	{
		murrine_set_color_rgb (cr, bg);
		cairo_fill (cr);
	}

	cairo_restore (cr);

	murrine_rounded_rectangle_closed (cr, 1.5, 1.5, width-3, height-3, roundness, widget->corners);
	murrine_set_color_rgb (cr, &border);
	cairo_stroke (cr);

	if (draw_bullet)
	{
		if (inconsistent)
		{
			cairo_save (cr);
			cairo_set_line_width (cr, 2.0);
			cairo_move_to (cr, 3, height*0.5);
			cairo_line_to (cr, width-3, height*0.5);
			cairo_restore (cr);
		}
		else
		{
			cairo_translate (cr, -1, 0);
			cairo_move_to (cr, 4, 8);
			cairo_rel_line_to (cr, 5, 4);
			cairo_rel_curve_to (cr, 1.4, -5, -1, -1, 5.7, -12.5);
			cairo_rel_curve_to (cr, -4, 4, -4, 4, -6.7, 9.3);
			cairo_rel_line_to (cr, -2.3, -2.5);
		}

		murrine_set_color_rgba (cr, dot, trans);
		cairo_fill (cr);
	}
}

static void
murrine_rgba_draw_menu_frame (cairo_t *cr,
                              const MurrineColors    *colors,
                              const WidgetParameters *widget,
                              int x, int y, int width, int height,
                              int menustyle)
{
	const MurrineRGB *border = &colors->shade[5];
	uint8 corners = (menustyle == 1 ? MRN_CORNER_BOTTOMRIGHT :
	                                  MRN_CORNER_BOTTOMLEFT | MRN_CORNER_BOTTOMRIGHT);

	cairo_translate      (cr, x, y);

	cairo_set_operator   (cr, CAIRO_OPERATOR_CLEAR);
	cairo_paint (cr);
	cairo_set_operator (cr, CAIRO_OPERATOR_OVER);

	murrine_set_color_rgba (cr, &colors->bg[0], MENU_OPACITY);
	clearlooks_rounded_rectangle (cr, 0, 0, width, height, widget->roundness > 1 ? widget->roundness+1 : 0, corners);
	cairo_fill (cr);

	murrine_set_color_rgb (cr, border);
	murrine_rounded_rectangle (cr, 0.5, 0.5, width-1, height-1, widget->roundness, corners);
	cairo_stroke          (cr);

	if (menustyle == 1)
	{
		MurrineRGB *fill = (MurrineRGB*)&colors->spot[1];
		MurrineRGB border2;
		murrine_shade (fill, 0.5, &border2);

		murrine_set_color_rgb (cr, &border2);
		cairo_rectangle (cr, 0.5, 0.5, 3, height-1);
		cairo_stroke_preserve (cr);

		murrine_set_color_rgb (cr, fill);
		cairo_fill (cr);
	}
}

static void
murrine_rgba_draw_statusbar (cairo_t *cr,
                             const MurrineColors    *colors,
                             const WidgetParameters *widget,
                             int x, int y, int width, int height)
{
	const MurrineRGB *dark = &colors->shade[4];
	const MurrineRGB *highlight = &colors->shade[0];

	cairo_translate       (cr, x, y+0.5);

	murrine_set_color_rgb (cr, dark);
	cairo_move_to         (cr, 0, 0);
	cairo_line_to         (cr, width, 0);
	cairo_stroke          (cr);

	murrine_set_color_rgba (cr, highlight, 0.5);
	cairo_move_to          (cr, 0, 1);
	cairo_line_to          (cr, width, 1);
	cairo_stroke           (cr);
}

void
murrine_register_style_rgba (MurrineStyleFunctions *functions)
{
	functions->draw_button             = murrine_rgba_draw_button;
	functions->draw_entry              = murrine_rgba_draw_entry;
	functions->draw_scale_trough       = murrine_rgba_draw_scale_trough;
	functions->draw_progressbar_trough = murrine_rgba_draw_progressbar_trough;
	functions->draw_progressbar_fill   = murrine_rgba_draw_progressbar_fill;
	functions->draw_menubar            = murrine_rgba_draw_menubar;
	functions->draw_toolbar            = murrine_rgba_draw_toolbar;
	functions->draw_frame              = murrine_rgba_draw_frame;
	functions->draw_separator          = murrine_rgba_draw_separator;
	functions->draw_tab                = murrine_rgba_draw_tab;
	functions->draw_scrollbar_trough   = murrine_rgba_draw_scrollbar_trough;
	functions->draw_scrollbar_stepper  = murrine_rgba_draw_scrollbar_stepper;
	functions->draw_scrollbar_slider   = murrine_rgba_draw_scrollbar_slider;
	functions->draw_handle             = murrine_rgba_draw_handle;
	functions->draw_tooltip            = murrine_rgba_draw_tooltip;
	functions->draw_radiobutton        = murrine_rgba_draw_radiobutton;
	functions->draw_checkbox           = murrine_rgba_draw_checkbox;
	functions->draw_menu_frame         = murrine_rgba_draw_menu_frame;
	functions->draw_statusbar          = murrine_rgba_draw_statusbar;
}
