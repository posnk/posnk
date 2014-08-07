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

#define G_PI    3.1415926535897932384626433832795028841971693993751
#define G_PI_2  1.5707963267948966192313216916397514420985846996876
#define G_PI_4  0.78539816339744830961566084581987572104929234984378
#define G_SQRT2 1.4142135623730950488016887242096980785696718753769
#define G_LOG_2_BASE_10		(0.30102999566398119521)

#include "murrine_types.h"

#define g_return_if_fail(CoNd) do {if (!(CoNd)) return;} while(0)

 void murrine_shade (const MurrineRGB *a, float k, MurrineRGB *b);

 void murrine_mix_color (const MurrineRGB *color1, const MurrineRGB *color2,
                                       double mix_factor, MurrineRGB *composite);

/* void murrine_get_parent_bg (const GtkWidget *widget,
                                            MurrineRGB *color);*/

 void murrine_set_color_rgb (cairo_t *cr,
                                            const MurrineRGB *color);

 void murrine_set_color_rgba (cairo_t *cr,
                                             const MurrineRGB *color,
                                             double alpha);

 void murrine_pattern_add_color_stop_rgb (cairo_pattern_t *pat, double pos,
                                                         const MurrineRGB *color);

 void murrine_pattern_add_color_stop_rgba (cairo_pattern_t *pat, double pos,
                                                          const MurrineRGB *color, double alpha);

 void rotate_mirror_translate (cairo_t *cr,
                                              double radius, double x, double y,
                                              boolean mirror_horizontally, boolean mirror_vertically);

 double get_decreased_shade (double old, double factor);

 double get_increased_shade (double old, double factor);

 double get_contrast (double old, double factor);

 MurrineGradients get_decreased_gradient_shades (MurrineGradients mrn_gradient, double factor);

 void murrine_exchange_axis (cairo_t  *cr,
                                            int     *x,
                                            int     *y,
                                            int     *width,
                                            int     *height);

 void murrine_rounded_corner (cairo_t *cr,
                                             double x, double y,
                                             int radius, uint8 corner);

 void clearlooks_rounded_rectangle (cairo_t *cr,
                                                   double x, double y, double w, double h,
                                                   int radius, uint8 corners);

 void murrine_rounded_rectangle (cairo_t *cr,
                                                double x, double y, double w, double h,
                                                int radius, uint8 corners);

 void murrine_rounded_rectangle_closed (cairo_t *cr,
                                                       double x, double y, double w, double h,
                                                       int radius, uint8 corners);

 void murrine_rounded_rectangle_fast (cairo_t *cr,
                                                     double x, double y, double w, double h,
                                                     uint8 corners);

 void murrine_set_gradient (cairo_t *cr,
                                           const MurrineRGB *color,
                                           MurrineGradients mrn_gradient,
                                           int x, int y, int width, int height,
                                           boolean gradients, boolean alpha);

 void murrine_draw_glaze (cairo_t *cr,
                                         const MurrineRGB *fill,
                                         double glow_shade,
                                         double highlight_shade,
                                         double lightborder_shade,
                                         MurrineGradients mrn_gradient,
                                         const WidgetParameters *widget,
                                         int x, int y, int width, int height,
                                         int radius, uint8 corners, boolean horizontal);

