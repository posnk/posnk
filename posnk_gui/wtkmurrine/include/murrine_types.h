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

#ifndef MURRINE_TYPES_H
#define MURRINE_TYPES_H

#include <cairo.h>

typedef unsigned char boolean;
typedef unsigned char uint8;
typedef struct _MurrineStyleFunctions MurrineStyleFunctions;

typedef enum
{
	MRN_PROFILE_MURRINE = 0,
	MRN_PROFILE_NODOKA = 1,
	MRN_PROFILE_MIST = 2,
	MRN_PROFILE_CANDIDO = 3,
	MRN_PROFILE_CLEARLOOKS = 4,
	MRN_NUM_PROFILES = 5
} MurrineProfiles;

typedef enum
{
	MRN_STYLE_MURRINE = 0,
	MRN_STYLE_RGBA = 1,
	MRN_NUM_DRAW_STYLES = 2
} MurrineStyles;

typedef enum
{
	MRN_STATE_NORMAL,
	MRN_STATE_ACTIVE,
	MRN_STATE_SELECTED,
	MRN_STATE_INSENSITIVE
} MurrineStateType;

typedef enum
{
	MRN_CORNER_NONE        = 0,
	MRN_CORNER_TOPLEFT     = 1,
	MRN_CORNER_TOPRIGHT    = 2,
	MRN_CORNER_BOTTOMLEFT  = 4,
	MRN_CORNER_BOTTOMRIGHT = 8,
	MRN_CORNER_ALL         = 15
} MurrineCorners;

typedef enum
{
	MRN_JUNCTION_NONE      = 0,
	MRN_JUNCTION_BEGIN     = 1,
	MRN_JUNCTION_END       = 2
} MurrineJunction;

typedef enum
{
	MRN_CONT_NONE          = 0,
	MRN_CONT_LEFT          = 1 << 0,
	MRN_CONT_RIGHT         = 1 << 1
} MurrineContinue;

typedef enum
{
	MRN_STEPPER_UNKNOWN    = 0,
	MRN_STEPPER_A          = 1,
	MRN_STEPPER_B          = 2,
	MRN_STEPPER_C          = 4,
	MRN_STEPPER_D          = 8
} MurrineStepper;

typedef enum
{
	MRN_ORDER_FIRST = 1 << 0,
	MRN_ORDER_LAST = 1 << 1
} MurrineOrder;

typedef enum
{
	MRN_ORIENTATION_LEFT_TO_RIGHT,
	MRN_ORIENTATION_RIGHT_TO_LEFT,
	MRN_ORIENTATION_BOTTOM_TO_TOP,
	MRN_ORIENTATION_TOP_TO_BOTTOM
} MurrineOrientation;

typedef enum
{
	MRN_GAP_LEFT,
	MRN_GAP_RIGHT,
	MRN_GAP_TOP,
	MRN_GAP_BOTTOM
} MurrineGapSide;

typedef enum
{
	MRN_SHADOW_NONE,
	MRN_SHADOW_IN,
	MRN_SHADOW_OUT,
	MRN_SHADOW_ETCHED_IN,
	MRN_SHADOW_ETCHED_OUT,
	MRN_SHADOW_FLAT
} MurrineShadowType;

typedef enum
{
	MRN_HANDLE_TOOLBAR,
	MRN_HANDLE_SPLITTER
} MurrineHandleType;

typedef enum
{
	MRN_ARROW_NORMAL,
	MRN_ARROW_COMBO
} MurrineArrowType;

typedef enum
{
	MRN_FOCUS_BUTTON,
	MRN_FOCUS_BUTTON_FLAT,
	MRN_FOCUS_LABEL,
	MRN_FOCUS_TREEVIEW,
	MRN_FOCUS_TREEVIEW_HEADER,
	MRN_FOCUS_TREEVIEW_ROW,
	MRN_FOCUS_TREEVIEW_DND,
	MRN_FOCUS_SCALE,
	MRN_FOCUS_TAB,
	MRN_FOCUS_COLOR_WHEEL_DARK,
	MRN_FOCUS_COLOR_WHEEL_LIGHT,
	MRN_FOCUS_ICONVIEW,
	MRN_FOCUS_UNKNOWN
} MurrineFocusType;

typedef enum
{
	MRN_DIRECTION_UP,
	MRN_DIRECTION_DOWN,
	MRN_DIRECTION_LEFT,
	MRN_DIRECTION_RIGHT
} MurrineDirection;

typedef enum
{
	MRN_WINDOW_EDGE_NORTH_WEST,
	MRN_WINDOW_EDGE_NORTH,
	MRN_WINDOW_EDGE_NORTH_EAST,
	MRN_WINDOW_EDGE_WEST,
	MRN_WINDOW_EDGE_EAST,
	MRN_WINDOW_EDGE_SOUTH_WEST,
	MRN_WINDOW_EDGE_SOUTH,
	MRN_WINDOW_EDGE_SOUTH_EAST
} MurrineWindowEdge;

typedef struct
{
	double r;
	double g;
	double b;
} MurrineRGB;

typedef struct
{
	double x;
	double y;
	double width;
	double height;
} MurrineRectangle;

typedef struct
{
	MurrineRGB bg[5];
	MurrineRGB base[5];
	MurrineRGB text[5];
	MurrineRGB fg[5];

	MurrineRGB shade[9];
	MurrineRGB spot[3];
} MurrineColors;

typedef struct
{
	double  gradient_shades[4];
	double  rgba_opacity;

	boolean gradients;
	boolean use_rgba;
} MurrineGradients;

typedef struct
{
	boolean active;
	boolean prelight;
	boolean disabled;
	boolean ltr;
	boolean focus;
	boolean is_default;
	MurrineStateType state_type;
	uint8 corners;
	uint8 xthickness;
	uint8 ythickness;
	MurrineRGB parentbg;

	/* Style */
	int glazestyle;
	int glowstyle;
	int lightborderstyle;
	int reliefstyle;
	int roundness;
	double glow_shade;
	double highlight_shade;
	double lightborder_shade;
	MurrineGradients mrn_gradient;

	MurrineStyles style;
	MurrineStyleFunctions *style_functions;
} WidgetParameters;

typedef struct
{
	MurrineFocusType    type;
	MurrineContinue     continue_side;
	MurrineRGB          color;
	boolean             has_color;
	int                 line_width;
	int                 padding;
	uint8*              dash_list;
	boolean             interior;
} FocusParameters;

typedef struct
{
	boolean lower;
	boolean horizontal;
	boolean fill_level;
} SliderParameters;

typedef struct
{
	MurrineOrientation orientation;
	int style;
} ProgressBarParameters;
/*
typedef struct
{
	/ * The maximum size of the fill. Calcualted from the entries allocation,
	 * and other information. Relative to the drawn position.
	 * /
	GdkRectangle max_size;
	gboolean max_size_known;
	/ * The border around the bar. This can be used for radius calculations.
	 * /
	GtkBorder border;
} EntryProgressParameters;*/

typedef struct
{
	int linepos;
} OptionMenuParameters;

typedef struct
{
	MurrineShadowType shadow_type;
	boolean           in_cell;
	boolean           in_menu;
} CheckboxParameters;

typedef struct
{
	MurrineShadowType shadow;
	MurrineGapSide gap_side;
	int gap_x;
	int gap_width;
	MurrineRGB *border;
} FrameParameters;

typedef struct
{
	MurrineGapSide gap_side;
} TabParameters;

typedef struct
{
	MurrineCorners    corners;
	MurrineShadowType shadow;
} ShadowParameters;

typedef struct
{
	boolean horizontal;
	boolean use_rgba;
} SeparatorParameters;

typedef struct
{
	MurrineOrder   order;
	boolean        resizable;
	int            style;
} ListViewHeaderParameters;

typedef struct
{
	MurrineRGB      color;
	MurrineJunction junction; /* On which sides the slider junctions */
	MurrineStepper  steppers; /* The visible steppers */
	boolean         horizontal;
	boolean         has_color;
	int             style;
	int             stepperstyle;
} ScrollBarParameters;

typedef struct
{
	MurrineHandleType type;
	boolean           horizontal;
} HandleParameters;

typedef struct
{
	MurrineStepper stepper; /* Which stepper to draw */
} ScrollBarStepperParameters;

typedef struct
{
	MurrineArrowType type;
	MurrineDirection direction;
} ArrowParameters;

typedef struct
{
	MurrineWindowEdge edge;
} ResizeGripParameters;

typedef struct
{
	boolean topmost;
	int style;
} ToolbarParameters;

struct _MurrineStyleFunctions
{
	void (*draw_button) (cairo_t *cr,
	                     const MurrineColors    *colors,
	                     const WidgetParameters *widget,
	                     int x, int y, int width, int height,
	                     boolean vertical);

	void (*draw_scale_trough) (cairo_t *cr,
	                           const MurrineColors    *colors,
	                           const WidgetParameters *widget,
	                           const SliderParameters *slider,
	                           int x, int y, int width, int height);

	void (*draw_slider_handle) (cairo_t *cr,
	                            const MurrineColors    *colors,
	                            const WidgetParameters *widget,
	                            int x, int y, int width, int height,
	                            boolean horizontal);

	void (*draw_progressbar_trough) (cairo_t *cr,
	                                 const MurrineColors    *colors,
	                                 const WidgetParameters *widget,
	                                 int x, int y, int width, int height);

	void (*draw_progressbar_fill) (cairo_t *cr,
	                               const MurrineColors         *colors,
	                               const WidgetParameters      *widget,
	                               const ProgressBarParameters *progressbar,
	                               int x, int y, int width, int height,
	                               int offset);

	void (*draw_entry) (cairo_t *cr,
	                    const MurrineColors    *colors,
	                    const WidgetParameters *widget,
	                    const FocusParameters  *focus,
	                    int x, int y, int width, int height);

	/*void (*draw_entry_progress)   (cairo_t *cr,
	                               const MurrineColors    *colors,
	                               const WidgetParameters *widget,
	                               const EntryProgressParameters *progress,
	                               int x, int y, int width, int height);*/

	void (*draw_spinbutton) (cairo_t *cr,
	                         const MurrineColors    *colors,
	                         const WidgetParameters *widget,
	                         int x, int y, int width, int height);

	void (*draw_spinbutton_down) (cairo_t *cr,
	                              const MurrineColors    *colors,
	                              const WidgetParameters *widget,
	                              int x, int y, int width, int height);

	void (*draw_optionmenu) (cairo_t *cr,
	                         const MurrineColors        *colors,
	                         const WidgetParameters     *widget,
	                         const OptionMenuParameters *optionmenu,
	                         int x, int y, int width, int height);

	void (*draw_menubar) (cairo_t *cr,
	                      const MurrineColors    *colors,
	                      const WidgetParameters *widget,
	                      int x, int y, int width, int height,
	                      int menubarstyle);

	void (*draw_tab) (cairo_t *cr,
	                  const MurrineColors    *colors,
	                  const WidgetParameters *widget,
	                  const TabParameters    *tab,
	                  int x, int y, int width, int height);

	void (*draw_frame) (cairo_t *cr,
	                    const MurrineColors    *colors,
	                    const WidgetParameters *widget,
	                    const FrameParameters  *frame,
	                    int x, int y, int width, int height);

	void (*draw_separator) (cairo_t *cr,
	                        const MurrineColors       *colors,
	                        const WidgetParameters    *widget,
	                        const SeparatorParameters *separator,
	                        int x, int y, int width, int height);

	void (*draw_combo_separator) (cairo_t *cr,
	                              const MurrineColors    *colors,
	                              const WidgetParameters *widget,
	                              int x, int y, int width, int height);

	void (*draw_list_view_header) (cairo_t *cr,
	                               const MurrineColors            *colors,
	                               const WidgetParameters         *widget,
	                               const ListViewHeaderParameters *header,
	                               int x, int y, int width, int height);

	void (*draw_toolbar) (cairo_t *cr,
	                      const MurrineColors    *colors,
	                      const WidgetParameters *widget,
	                      const ToolbarParameters *toolbar,
	                      int x, int y, int width, int height);

	void (*draw_menuitem) (cairo_t *cr,
	                       const MurrineColors    *colors,
	                       const WidgetParameters *widget,
	                       int x, int y, int width, int height,
	                       int menuitemstyle);

	void (*draw_scrollbar_stepper) (cairo_t *cr,
	                                const MurrineColors              *colors,
	                                const WidgetParameters           *widget,
	                                const ScrollBarParameters        *scrollbar,
	                                //const ScrollBarStepperParameters *stepper,
	                                int x, int y, int width, int height);

	void (*draw_scrollbar_slider) (cairo_t *cr,
	                               const MurrineColors       *colors,
	                               const WidgetParameters    *widget,
	                               const ScrollBarParameters *scrollbar,
	                               int x, int y, int width, int height);

	void (*draw_scrollbar_trough) (cairo_t *cr,
	                               const MurrineColors       *colors,
	                               const WidgetParameters    *widget,
	                               const ScrollBarParameters *scrollbar,
	                               int x, int y, int width, int height);

	void (*draw_selected_cell) (cairo_t *cr,
	                            const MurrineColors    *colors,
	                            const WidgetParameters *widget,
	                            int x, int y, int width, int height);

	void (*draw_statusbar) (cairo_t *cr,
	                        const MurrineColors    *colors,
	                        const WidgetParameters *widget,
	                        int x, int y, int width, int height);

	void (*draw_menu_frame) (cairo_t *cr,
	                         const MurrineColors    *colors,
	                         const WidgetParameters *widget,
	                         int x, int y, int width, int height,
	                         int menustyle);

	void (*draw_tooltip) (cairo_t *cr,
	                      const MurrineColors    *colors,
	                      const WidgetParameters *widget,
	                      int x, int y, int width, int height);

	void (*draw_handle) (cairo_t *cr,
	                     const MurrineColors    *colors,
	                     const WidgetParameters *widget,
	                     const HandleParameters *handle,
	                     int x, int y, int width, int height);

	void (*draw_arrow) (cairo_t *cr,
	                    const MurrineColors    *colors,
	                    const WidgetParameters *widget,
	                    const ArrowParameters  *arrow,
	                    int x, int y, int width, int height);

	void (*draw_checkbox) (cairo_t *cr,
	                       const MurrineColors      *colors,
	                       const WidgetParameters   *widget,
	                       const CheckboxParameters *checkbox,
	                       int x, int y, int width, int height,
	                       double trans);

	void (*draw_radiobutton) (cairo_t *cr,
	                          const MurrineColors      *colors,
	                          const WidgetParameters   *widget,
	                          const CheckboxParameters *checkbox,
	                          int x, int y, int width, int height,
	                          double trans);

	void (*draw_resize_grip) (cairo_t *cr,
	                          const MurrineColors        *colors,
	                          const WidgetParameters     *widget,
	                          const ResizeGripParameters *grip,
	                          int x, int y, int width, int height);

	void (*draw_focus) (cairo_t *cr,
	                    const MurrineColors    *colors,
	                    const WidgetParameters *widget,
	                    const FocusParameters  *focus,
	                    int x, int y, int width, int height);
};

#define MURRINE_RECTANGLE_SET(rect, _x, _y, _w, _h) rect.x      = _x; \
                                                    rect.y      = _y; \
                                                    rect.width  = _w; \
                                                    rect.height = _h;

#endif /* MURRINE_TYPES_H */
