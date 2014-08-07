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

#include <math.h>

#include "cairo-support.h"
#include "support.h"
#include "murrine_types.h"

void
murrine_rgb_to_hls (double *r,
                    double *g,
                    double *b)
{
	double min;
	double max;
	double red;
	double green;
	double blue;
	double h, l, s;
	double delta;

	red = *r;
	green = *g;
	blue = *b;

	if (red > green)
	{
		if (red > blue)
			max = red;
		else
			max = blue;

		if (green < blue)
			min = green;
		else
			min = blue;
	}
	else
	{
		if (green > blue)
			max = green;
		else
			max = blue;

		if (red < blue)
			min = red;
		else
			min = blue;
	}

	l = (max+min)/2;
	if (fabs (max-min) < 0.0001)
	{
		h = 0;
		s = 0;
	}
	else
	{
		if (l <= 0.5)
			s = (max-min)/(max+min);
		else
			s = (max-min)/(2-max-min);

		delta = max -min;
		if (red == max)
			h = (green-blue)/delta;
		else if (green == max)
			h = 2+(blue-red)/delta;
		else if (blue == max)
			h = 4+(red-green)/delta;

		h *= 60;
		if (h < 0.0)
			h += 360;
	}

	*r = h;
	*g = l;
	*b = s;
}

void
murrine_hls_to_rgb (double *h,
                    double *l,
                    double *s)
{
	double hue;
	double lightness;
	double saturation;
	double m1, m2;
	double r, g, b;

	lightness = *l;
	saturation = *s;

	if (lightness <= 0.5)
		m2 = lightness*(1+saturation);
	else
		m2 = lightness+saturation-lightness*saturation;

	m1 = 2*lightness-m2;

	if (saturation == 0)
	{
		*h = lightness;
		*l = lightness;
		*s = lightness;
	}
	else
	{
		hue = *h+120;
		while (hue > 360)
			hue -= 360;
		while (hue < 0)
			hue += 360;

		if (hue < 60)
			r = m1+(m2-m1)*hue/60;
		else if (hue < 180)
			r = m2;
		else if (hue < 240)
			r = m1+(m2-m1)*(240-hue)/60;
		else
			r = m1;

		hue = *h;
		while (hue > 360)
			hue -= 360;
		while (hue < 0)
			hue += 360;

		if (hue < 60)
			g = m1+(m2-m1)*hue/60;
		else if (hue < 180)
			g = m2;
		else if (hue < 240)
			g = m1+(m2-m1)*(240-hue)/60;
		else
			g = m1;

		hue = *h-120;
		while (hue > 360)
			hue -= 360;
		while (hue < 0)
			hue += 360;

		if (hue < 60)
			b = m1+(m2-m1)*hue/60;
		else if (hue < 180)
			b = m2;
		else if (hue < 240)
			b = m1+(m2-m1)*(240-hue)/60;
		else
			b = m1;

		*h = r;
		*l = g;
		*s = b;
	}
}

void
murrine_shade (const MurrineRGB *a, float k, MurrineRGB *b)
{
	double red;
	double green;
	double blue;

	red   = a->r;
	green = a->g;
	blue  = a->b;

	if (k == 1.0)
	{
		b->r = red;
		b->g = green;
		b->b = blue;
		return;
	}

	murrine_rgb_to_hls (&red, &green, &blue);

	green *= k;
	if (green > 1.0)
		green = 1.0;
	else if (green < 0.0)
		green = 0.0;

	blue *= k;
	if (blue > 1.0)
		blue = 1.0;
	else if (blue < 0.0)
		blue = 0.0;

	murrine_hls_to_rgb (&red, &green, &blue);

	b->r = red;
	b->g = green;
	b->b = blue;
}

void
murrine_mix_color (const MurrineRGB *color1, const MurrineRGB *color2,
                   double mix_factor, MurrineRGB *composite)
{
	g_return_if_fail (color1 && color2 && composite);

	composite->r = color1->r*(1-mix_factor)+color2->r*mix_factor;
	composite->g = color1->g*(1-mix_factor)+color2->g*mix_factor;
	composite->b = color1->b*(1-mix_factor)+color2->b*mix_factor;
}

/*
void
murrine_get_parent_bg (const GtkWidget *widget, MurrineRGB *color)
{
	GtkStateType state_type;
	const GtkWidget *parent;
	GdkColor *gcolor;
	gboolean stop;

	if (widget == NULL)
		return;

	parent = widget->parent;
	stop = FALSE;

	while (parent && !stop)
	{
		stop = FALSE;

		stop |= !GTK_WIDGET_NO_WINDOW (parent);
		stop |= GTK_IS_NOTEBOOK (parent) &&
		        gtk_notebook_get_show_tabs (GTK_NOTEBOOK (parent)) &&
		        gtk_notebook_get_show_border (GTK_NOTEBOOK (parent));

		if (GTK_IS_TOOLBAR (parent))
		{
			GtkShadowType shadow = GTK_SHADOW_OUT;
			gtk_widget_style_get (GTK_WIDGET (parent), "shadow-type", &shadow, NULL);

			stop |= (shadow != GTK_SHADOW_NONE);
		}

		if (!stop)
			parent = parent->parent;
	}

	if (parent == NULL)
		return;

	state_type = GTK_WIDGET_STATE (parent);

	gcolor = &parent->style->bg[state_type];

	murrine_gdk_color_to_rgb (gcolor, &color->r, &color->g, &color->b);
}*/

void
murrine_set_color_rgb (cairo_t *cr, const MurrineRGB *color)
{
	g_return_if_fail (cr && color);

	cairo_set_source_rgb (cr, color->r, color->g, color->b);
}

void
murrine_set_color_rgba (cairo_t *cr, const MurrineRGB *color, double alpha)
{
	g_return_if_fail (cr && color);

	cairo_set_source_rgba (cr, color->r, color->g, color->b, alpha);
}

void
murrine_pattern_add_color_stop_rgb (cairo_pattern_t *pat, double pos,
                                    const MurrineRGB *color)
{
	g_return_if_fail (pat && color);

	cairo_pattern_add_color_stop_rgb (pat, pos, color->r, color->g, color->b);
}

void
murrine_pattern_add_color_stop_rgba (cairo_pattern_t *pat, double pos,
                                     const MurrineRGB *color, double alpha)
{
	g_return_if_fail (pat && color);

	cairo_pattern_add_color_stop_rgba (pat, pos, color->r, color->g, color->b, alpha);
}

void
murrine_rounded_corner (cairo_t *cr,
                        double   x,
                        double   y,
                        int radius, uint8 corner)
{
	if (radius < 1)
	{
		cairo_line_to (cr, x, y);
	}
	else
	{
		switch (corner)
		{
			case MRN_CORNER_NONE:
				cairo_line_to (cr, x, y);
				break;
			case MRN_CORNER_TOPLEFT:
				cairo_arc (cr, x+radius, y+radius, radius, G_PI, G_PI*3/2);
				break;
			case MRN_CORNER_TOPRIGHT:
				cairo_arc (cr, x-radius, y+radius, radius, G_PI*3/2, G_PI*2);
				break;
			case MRN_CORNER_BOTTOMRIGHT:
				cairo_arc (cr, x-radius, y-radius, radius, 0, G_PI*1/2);
				break;
			case MRN_CORNER_BOTTOMLEFT:
				cairo_arc (cr, x+radius, y-radius, radius, G_PI*1/2, G_PI);
				break;

			default:
				/* A bitfield and not a sane value ... */
				assert(0);
				cairo_line_to (cr, x, y);
				return;
		}
	}
}

void
murrine_rounded_rectangle_fast (cairo_t *cr,
                                double x, double y, double w, double h,
                                uint8 corners)
{
	const float RADIUS_CORNERS = 0.35;

	if (corners & MRN_CORNER_TOPLEFT)
		cairo_move_to (cr, x+RADIUS_CORNERS, y);
	else
		cairo_move_to (cr, x, y);

	if (corners & MRN_CORNER_TOPRIGHT)
	{
		cairo_line_to (cr, x+w-RADIUS_CORNERS, y);
		cairo_move_to (cr, x+w, y+RADIUS_CORNERS);
	}
	else
		cairo_line_to (cr, x+w, y);

	if (corners & MRN_CORNER_BOTTOMRIGHT)
	{
		cairo_line_to (cr, x+w, y+h-RADIUS_CORNERS);
		cairo_move_to (cr, x+w-RADIUS_CORNERS, y+h);
	}
	else
		cairo_line_to (cr, x+w, y+h);

	if (corners & MRN_CORNER_BOTTOMLEFT)
	{
		cairo_line_to (cr, x+RADIUS_CORNERS, y+h);
		cairo_move_to (cr, x, y+h-RADIUS_CORNERS);
	}
	else
		cairo_line_to (cr, x, y+h);

	if (corners & MRN_CORNER_TOPLEFT)
		cairo_line_to (cr, x, y+RADIUS_CORNERS);
	else
	{
		if (corners == MRN_CORNER_NONE)
			cairo_close_path (cr);
		else
			cairo_line_to (cr, x, y);
	}
}

void
clearlooks_rounded_rectangle (cairo_t *cr,
                              double x, double y, double w, double h,
                              int radius, uint8 corners)
{
	if (radius < 1)
	{
		cairo_rectangle (cr, x, y, w, h);
		return;
	}
	
	radius = MIN (radius, MIN (w/2.0, h/2.0));

	if (corners & MRN_CORNER_TOPLEFT)
		cairo_move_to (cr, x+radius, y);
	else
		cairo_move_to (cr, x, y);

	if (corners & MRN_CORNER_TOPRIGHT)
		cairo_arc (cr, x+w-radius, y+radius, radius, M_PI*1.5, M_PI*2);
	else
		cairo_line_to (cr, x+w, y);

	if (corners & MRN_CORNER_BOTTOMRIGHT)
		cairo_arc (cr, x+w-radius, y+h-radius, radius, 0, M_PI*0.5);
	else
		cairo_line_to (cr, x+w, y+h);

	if (corners & MRN_CORNER_BOTTOMLEFT)
		cairo_arc (cr, x+radius,   y+h-radius, radius, M_PI*0.5, M_PI);
	else
		cairo_line_to (cr, x, y+h);

	if (corners & MRN_CORNER_TOPLEFT)
		cairo_arc (cr, x+radius,   y+radius,   radius, M_PI, M_PI*1.5);
	else
		cairo_line_to (cr, x, y);
}

void
murrine_rounded_rectangle (cairo_t *cr,
                           double x, double y, double w, double h,
                           int radius, uint8 corners)
{
	radius < 2 ? radius < 1 ? cairo_rectangle (cr, x, y, w, h) :
	                          murrine_rounded_rectangle_fast (cr, x, y, w, h, corners) :
	             clearlooks_rounded_rectangle (cr, x, y, w, h, radius, corners);
}

void
murrine_rounded_rectangle_closed (cairo_t *cr,
                                  double x, double y, double w, double h,
                                  int radius, uint8 corners)
{
	radius < 2 ? cairo_rectangle (cr, x, y, w, h) :
	             clearlooks_rounded_rectangle (cr, x, y, w, h, radius, corners);
}

static void
murrine_draw_flat_highlight (cairo_t *cr,
                             int x, int y, int width, int height)
{
	cairo_rectangle (cr, x, y, width, height/2);
}

static void
murrine_draw_curved_highlight (cairo_t *cr,
                               int x, int y, int width, int height)
{
	int w = width+x*2;
	int h = height+y*2;

	cairo_move_to (cr, x, h-y);
	cairo_curve_to (cr, x, h/2+h/5, h/5, h/2, h/2, h/2);
	cairo_line_to (cr, w-h/2, h/2);
	cairo_curve_to (cr, w-x-h/5, h/2, w-x-0.5, h/2-h/5, w-x, y);
	cairo_line_to (cr, x, y);
	cairo_line_to (cr, x, h-y);
	cairo_close_path (cr);
}

static void
murrine_draw_curved_highlight_top (cairo_t *cr,
                                   int x, int y, int width, int height)
{
	int w = width+x*2;
	int h = height+y*2;

	cairo_move_to (cr, x, y);
	cairo_curve_to (cr, x, h/2-h/5, h/5, h/2, h/2, h/2);
	cairo_line_to (cr, w-h/2, h/2);
	cairo_curve_to (cr, w-x-h/5, h/2, w-x-0.5, h/2-h/5, w-x, y);
	cairo_close_path (cr);
}

static void
murrine_draw_curved_highlight_bottom (cairo_t *cr,
                                      int x, int y, int width, int height)
{
	int w = width+x*2;
	int h = height+y*2;

	cairo_move_to (cr, x, h-y);
	cairo_curve_to (cr, x, h/2+h/5, h/5, h/2, h/2, h/2);
	cairo_line_to (cr, w-h/2, h/2);
	cairo_curve_to (cr, w-x-h/5, h/2, w-x-0.5, h/2+h/5, w-x, h-y);
	cairo_close_path (cr);
}

/*
 * Test new glazestyles here :)
 */
static void
murrine_draw_new_glossy_highlight (cairo_t *cr,
                                   int x, int y, int width, int height)
{
	double gloss_height = .5;
	int gloss_angle = 3;
	int local_gloss_height = (int) (height*gloss_height);

	cairo_move_to (cr, x, y);
	cairo_line_to (cr, x+width, y);
	cairo_line_to (cr, x+width, local_gloss_height);
	cairo_curve_to (cr, x+2*width/3, local_gloss_height+gloss_angle, x+width/3,
	                    local_gloss_height+gloss_angle, x, local_gloss_height);
	cairo_close_path (cr);
}

static void
murrine_draw_bottom_glow (cairo_t *cr,
                          const MurrineRGB *glow,
                          int x, int y, int width, int height)
{
	cairo_pattern_t *pat;
	double           scaling_factor = (double)1.2*width/height;

	cairo_rectangle (cr, x, y, width, height);
	cairo_save (cr);
	cairo_scale (cr, scaling_factor, 1);
	pat = cairo_pattern_create_radial ((x+width/2.0)/scaling_factor, y+height, 0,
	                                   (x+width/2.0)/scaling_factor, y+height, height/2);
	murrine_pattern_add_color_stop_rgba (pat, 0.0, glow, 0.5);
	murrine_pattern_add_color_stop_rgba (pat, 1.0, glow, 0.0);
	cairo_set_source (cr, pat);
	cairo_pattern_destroy (pat);
	cairo_fill (cr);
	cairo_restore (cr);
}

static void
murrine_draw_centered_glow (cairo_t *cr,
                            const MurrineRGB *glow,
                            int x, int y, int width, int height)
{
	cairo_pattern_t *pat;
	double           scaling_factor = (double)1.2*width/height;

	cairo_rectangle (cr, x, y, width, height);
	cairo_save (cr);
	cairo_scale (cr, scaling_factor, 1);
	pat = cairo_pattern_create_radial ((x+width/2.0)/scaling_factor, y+height/2, 0,
	                                   (x+width/2.0)/scaling_factor, y+height/2, height/2);
	murrine_pattern_add_color_stop_rgba (pat, 0.0, glow, 0.5);
	murrine_pattern_add_color_stop_rgba (pat, 1.0, glow, 0.0);
	cairo_set_source (cr, pat);
	cairo_pattern_destroy (pat);
	cairo_fill (cr);
	cairo_restore (cr);
}

static void
murrine_draw_horizontal_glow (cairo_t *cr,
                              const MurrineRGB *glow,
                              int x, int y, int width, int height)
{
	cairo_pattern_t *pat;

	cairo_rectangle (cr, x, y, width, height);
	pat = cairo_pattern_create_linear (x, y, width, 0);
	murrine_pattern_add_color_stop_rgba (pat, 0.0, glow, 0.0);
	murrine_pattern_add_color_stop_rgba (pat, 0.5, glow, 0.5);
	murrine_pattern_add_color_stop_rgba (pat, 1.0, glow, 0.0);
	cairo_set_source (cr, pat);
	cairo_pattern_destroy (pat);
	cairo_fill (cr);
}

static void
murrine_draw_top_glow (cairo_t *cr,
                       const MurrineRGB *glow,
                       int x, int y, int width, int height)
{
	cairo_pattern_t *pat;
	double           scaling_factor = (double)1.2*width/height;

	cairo_rectangle (cr, x, y, width, height);
	cairo_save (cr);
	cairo_scale (cr, scaling_factor, 1);
	pat = cairo_pattern_create_radial ((x+width/2.0)/scaling_factor, y, 0,
	                                   (x+width/2.0)/scaling_factor, y, height/2);
	murrine_pattern_add_color_stop_rgba (pat, 0.0, glow, 0.5);
	murrine_pattern_add_color_stop_rgba (pat, 1.0, glow, 0.0);
	cairo_set_source (cr, pat);
	cairo_pattern_destroy (pat);
	cairo_fill (cr);
	cairo_restore (cr);
}

static void
murrine_draw_lightborder (cairo_t *cr,
                          const MurrineRGB *highlight_color,
                          const MurrineRGB *fill,
                          MurrineGradients mrn_gradient,
                          double x, double y, int width, int height,
                          boolean gradients,
                          int glazestyle, int lightborderstyle,
                          int radius, uint8 corners)
{
	cairo_pattern_t *pat;
	MurrineRGB shade1, shade2, shade3, shade4;
	double alpha_value = mrn_gradient.use_rgba ? mrn_gradient.rgba_opacity : 1.0;
	radius = MIN (radius, MIN ((double)width/2.0, (double)height/2.0));

	murrine_shade (highlight_color, mrn_gradient.gradient_shades[0], &shade1);
	murrine_shade (highlight_color, mrn_gradient.gradient_shades[1], &shade2);
	murrine_shade (highlight_color, mrn_gradient.gradient_shades[2], &shade3);
	murrine_shade (highlight_color, mrn_gradient.gradient_shades[3], &shade4);

	cairo_save (cr);

	double fill_pos = 1.0-1.0/(double)height;
	if (corners == MRN_CORNER_ALL && radius > 2)
		fill_pos = 1.0-(((double)radius-1.0)/2.0)/(double)height;

	radius < 2 ? cairo_rectangle (cr, x, y, width, height) :
	             clearlooks_rounded_rectangle (cr, x, y, width, height, radius-1, corners);

	pat = cairo_pattern_create_linear (x, y, x, height+y);
	murrine_pattern_add_color_stop_rgba (pat, 0.00,     &shade1, 0.5*alpha_value);
	murrine_pattern_add_color_stop_rgba (pat, 0.49,     &shade2, 0.5*alpha_value);
	murrine_pattern_add_color_stop_rgba (pat, 0.49,     &shade3, 0.5*alpha_value);
	murrine_pattern_add_color_stop_rgba (pat, fill_pos, &shade4, 0.5*alpha_value);
	murrine_pattern_add_color_stop_rgba (pat, fill_pos, &shade4,
	                                     lightborderstyle > 0 ? 0.5*alpha_value : 0.0);
	murrine_pattern_add_color_stop_rgba (pat, 1.00,     &shade4,
	                                     lightborderstyle > 0 ? 0.5*alpha_value : 0.0);
	cairo_set_source (cr, pat);
	cairo_pattern_destroy (pat);

	cairo_stroke (cr);

	if (glazestyle == 2)
	{
		murrine_set_gradient (cr, fill, mrn_gradient, x, y, 0, height, mrn_gradient.gradients, FALSE);
		cairo_move_to (cr, width+x, y+0.5);
		cairo_line_to (cr, width+x, height+y);
		cairo_line_to (cr, x-0.5, height+y);
		cairo_stroke (cr);
	}

	cairo_restore (cr);
}

void
murrine_draw_glaze (cairo_t *cr,
                    const MurrineRGB *fill,
                    double glow_shade,
                    double highlight_shade,
                    double lightborder_shade,
                    MurrineGradients mrn_gradient,
                    const WidgetParameters *widget,
                    int x, int y, int width, int height,
                    int radius, uint8 corners, boolean horizontal)
{
	MurrineRGB highlight;
	murrine_shade (fill, highlight_shade, &highlight);

	murrine_set_gradient (cr, fill, mrn_gradient, x, y, 0, height, mrn_gradient.gradients, FALSE);
	cairo_set_fill_rule (cr, CAIRO_FILL_RULE_EVEN_ODD);
	switch (widget->glazestyle)
	{
		default:
		case 0:
			cairo_fill (cr);
			murrine_draw_flat_highlight (cr, x, y, width, height);
			break;
		case 1:
			cairo_fill (cr);
			murrine_draw_curved_highlight (cr, x, y, width, height);
			break;
		case 2:
			cairo_fill_preserve (cr);
			murrine_draw_curved_highlight (cr, x, y, width, height);
			break;
		case 3:
		case 4:
			cairo_fill (cr);
			murrine_draw_curved_highlight_top (cr, x, y, width, height);
			break;
		case 5:
			cairo_fill (cr);
			murrine_draw_new_glossy_highlight (cr, x, y, width, height);
			break;
	}
	murrine_set_gradient (cr, &highlight, mrn_gradient, x, y, 0, height, mrn_gradient.gradients, TRUE);
	cairo_fill (cr);
	if (widget->glazestyle == 4)
	{
		MurrineRGB shadow;
		murrine_shade (fill, 1.0/highlight_shade, &shadow);

		murrine_draw_curved_highlight_bottom (cr, x, y, width, height);
		murrine_set_gradient (cr, &shadow, mrn_gradient, x, y, 0, height, mrn_gradient.gradients, TRUE);
		cairo_fill (cr);
	}

	if (glow_shade != 1.0)
	{
		MurrineRGB glow;
		murrine_shade (fill, glow_shade, &glow);

		if (mrn_gradient.use_rgba)
			cairo_set_operator (cr, CAIRO_OPERATOR_OVER);

		switch (widget->glowstyle)
		{
			default:
			case 0:
				murrine_draw_top_glow (cr, &glow, x, y, width, height);
				break;
			case 1:
				murrine_draw_bottom_glow (cr, &glow, x, y, width, height);
				break;
			case 2:
				murrine_draw_top_glow (cr, &glow, x, y, width, height);
				murrine_draw_bottom_glow (cr, &glow, x, y, width, height);
				break;
			case 3:
				murrine_draw_horizontal_glow (cr, &glow, x, y, width, height);
				break;
			case 4:
				murrine_draw_centered_glow (cr, &glow, x, y, width, height);
				break;
		}
	}

	if (widget->glazestyle != 4 && lightborder_shade != 1.0)
	{
		murrine_shade (fill, lightborder_shade*highlight_shade, &highlight);

		if (mrn_gradient.use_rgba)
			cairo_set_operator (cr, CAIRO_OPERATOR_OVER);

		murrine_draw_lightborder (cr, &highlight, fill, mrn_gradient,
		                          x+0.5, y+0.5, width-1, height-1,
		                          mrn_gradient.gradients,
		                          widget->glazestyle, widget->lightborderstyle,
		                          radius, corners);
	}
}

void
murrine_set_gradient (cairo_t *cr,
                      const MurrineRGB *color,
                      MurrineGradients mrn_gradient,
                      int x, int y, int width, int height,
                      boolean gradients, boolean alpha)
{
	double alpha_value = 1.0;
	if (mrn_gradient.use_rgba)
	{
		alpha_value = mrn_gradient.rgba_opacity;
	}
	else if (alpha)
	{
		alpha_value *= 0.8;
	}

	if (gradients)
	{
		cairo_pattern_t *pat;
		MurrineRGB shade1, shade2, shade3, shade4;

		murrine_shade (color, mrn_gradient.gradient_shades[0], &shade1);
		murrine_shade (color, mrn_gradient.gradient_shades[1], &shade2);
		murrine_shade (color, mrn_gradient.gradient_shades[2], &shade3);
		murrine_shade (color, mrn_gradient.gradient_shades[3], &shade4);

		pat = cairo_pattern_create_linear (x, y, width+x, height+y);

		murrine_pattern_add_color_stop_rgba (pat, 0.00, &shade1, alpha_value);
		murrine_pattern_add_color_stop_rgba (pat, 0.49, &shade2, alpha_value);
		murrine_pattern_add_color_stop_rgba (pat, 0.49, &shade3, alpha_value);
		murrine_pattern_add_color_stop_rgba (pat, 1.00, &shade4, alpha_value);

		cairo_set_source (cr, pat);
		cairo_pattern_destroy (pat);
	}
	else
	{
		murrine_set_color_rgba (cr, color, alpha_value);
	}
}

void
rotate_mirror_translate (cairo_t *cr,
                         double radius, double x, double y,
                         boolean mirror_horizontally, boolean mirror_vertically)
{
	cairo_matrix_t matrix_rotate;
	cairo_matrix_t matrix_mirror;
	cairo_matrix_t matrix_result;

	double r_cos = cos(radius);
	double r_sin = sin(radius);

	cairo_matrix_init (&matrix_rotate, r_cos, r_sin, r_sin, r_cos, x, y);

	cairo_matrix_init (&matrix_mirror,
	                   mirror_horizontally ? -1 : 1, 0, 0,
	                   mirror_vertically ? -1 : 1, 0, 0);

	cairo_matrix_multiply (&matrix_result, &matrix_mirror, &matrix_rotate);

	cairo_set_matrix (cr, &matrix_result);
}

void
murrine_exchange_axis (cairo_t  *cr,
                       int     *x,
                       int     *y,
                       int     *width,
                       int     *height)
{
	int tmp;
	cairo_matrix_t matrix;

	cairo_translate (cr, *x, *y);
	cairo_matrix_init (&matrix, 0, 1, 1, 0, 0, 0);

	cairo_transform (cr, &matrix);

	/* swap width/height */
	tmp = *width;
	*x = 0;
	*y = 0;
	*width = *height;
	*height = tmp;
}

double
get_decreased_shade (double old, double factor)
{
	if (old > 1.0)
		return ((old-1.0)/factor+1.0);
	else if (old < 1.0)
		return (1.0-(1.0-old)/factor);

	return old;
}

double
get_increased_shade (double old, double factor)
{
	if (old > 1.0)
		return ((old-1.0)*factor+1.0);
	else if (old < 1.0)
		return (1.0-(1.0-old)*factor);

	return old;
}

double
get_contrast (double old, double factor)
{
	if (factor == 1.0)
		return old;

	if (factor < 1.0)
	{
		if (old < 1.0)
			return old+(1.0-old)*(1.0-factor);
		else
			return old-(old-1.0)*(1.0-factor);
	}
	else
	{
		if (old < 1.0)
			return old-old*(factor-1.0);
		else
			return old+(old-1.0)*(factor-1.0);
	}
}

MurrineGradients
get_decreased_gradient_shades (MurrineGradients mrn_gradient, double factor)
{
	MurrineGradients mrn_gradient_custom = mrn_gradient;

	mrn_gradient_custom.gradient_shades[0] = get_decreased_shade (mrn_gradient.gradient_shades[0], factor);
	mrn_gradient_custom.gradient_shades[1] = get_decreased_shade (mrn_gradient.gradient_shades[1], factor);
	mrn_gradient_custom.gradient_shades[2] = get_decreased_shade (mrn_gradient.gradient_shades[2], factor);
	mrn_gradient_custom.gradient_shades[3] = get_decreased_shade (mrn_gradient.gradient_shades[3], factor);

	return mrn_gradient_custom;
}
