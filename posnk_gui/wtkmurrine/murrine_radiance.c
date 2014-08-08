#include "murrine_style.h"
#include "support.h"
//bbase_color:#ffffff\nfg_color:#4c4c4c\ntooltip_fg_color:#ffffff\nselected_bg_color:#f07746\nselected_fg_color:#FFFFFF\ntext_color:#3C3C3C\nbg_color:#F2F1F0\ntooltip_bg_color:#000000\nlink_color:#DD4814

MurrineStyleFunctions murrine_fnc;

MurrineRGB murrine_color(int r, int g, int b)
{
	MurrineRGB c = {((double)r)/255.0, ((double)g)/255.0, ((double)b)/255.0};
	return c;
}
MurrineRGB murrine_tshade(double s, MurrineRGB _c)
{
	MurrineRGB c = {_c.r * s, _c.g * s, _c.b * s};
	return c;
}
MurrineTheme *murrine_radiance()
{
	MurrineTheme *radiance = malloc(sizeof(MurrineTheme));
	MurrineRGB fg_colour = murrine_color(0x4c, 0x4c, 0x4c);
	MurrineRGB bg_colour = murrine_color(0xF6, 0xF4, 0xF2);
	MurrineRGB selected_bg_colour = murrine_color(0xF0, 0x77, 0x46);
	MurrineRGB selected_fg_colour = murrine_color(0xFF, 0xFF, 0xFF);
	MurrineRGB text_colour = murrine_color(0x3c, 0x3c, 0x3c);
	MurrineRGB dark_bg_colour = murrine_color(0x3c, 0x3b, 0x37);
	MurrineRGB dark_fg_colour = murrine_color(0xDF, 0xDB, 0xD2);
	MurrineRGB base_colour = murrine_color(0xFF, 0xFF, 0xFF);
	MurrineRGB dark_s_fg_colour = murrine_color(0xFF, 0xFF, 0xFF);
	MurrineRGB button_colour = murrine_color(0xCD, 0xCD, 0xCD);
	radiance->default_style.colors.bg[WTK_STATE_NORMAL] = bg_colour;
	radiance->default_style.colors.bg[WTK_STATE_HOVER] = murrine_tshade(1.02, bg_colour);
	radiance->default_style.colors.bg[WTK_STATE_SELECTED] = selected_bg_colour;
	radiance->default_style.colors.bg[WTK_STATE_DISABLED] = murrine_tshade(0.95, bg_colour);
	radiance->default_style.colors.bg[WTK_STATE_ACTIVE] = murrine_tshade(0.9, bg_colour);
	radiance->default_style.colors.fg[WTK_STATE_NORMAL] = fg_colour;
	radiance->default_style.colors.fg[WTK_STATE_HOVER] = fg_colour;
	radiance->default_style.colors.fg[WTK_STATE_SELECTED] = selected_fg_colour;
	radiance->default_style.colors.fg[WTK_STATE_DISABLED] = murrine_tshade(0.7, bg_colour);
	radiance->default_style.colors.fg[WTK_STATE_ACTIVE] = fg_colour;
	radiance->default_style.colors.text[WTK_STATE_NORMAL] = text_colour;
	radiance->default_style.colors.text[WTK_STATE_HOVER] = text_colour;
	radiance->default_style.colors.text[WTK_STATE_SELECTED] = selected_fg_colour;
	radiance->default_style.colors.text[WTK_STATE_DISABLED] = murrine_tshade(0.8, bg_colour);
	radiance->default_style.colors.text[WTK_STATE_ACTIVE] = murrine_tshade(0.7, text_colour);
	radiance->default_style.colors.base[WTK_STATE_NORMAL] = base_colour;
	radiance->default_style.colors.base[WTK_STATE_HOVER] = murrine_tshade(0.98, bg_colour);
	radiance->default_style.colors.base[WTK_STATE_SELECTED] = selected_bg_colour;
	radiance->default_style.colors.base[WTK_STATE_DISABLED] = murrine_tshade(0.97, bg_colour);
	radiance->default_style.colors.base[WTK_STATE_ACTIVE] = murrine_tshade(0.94, bg_colour);
	radiance->default_style.contrast = 0.6;
	//radiance->default_style.arrowstyle = 2;
	radiance->default_style.reliefstyle = 3;
	radiance->default_style.highlight_shade = 1.0;
	radiance->default_style.glazestyle = 0;
	radiance->default_style.gradients = TRUE;
	//radiance->default_style.default_button_color = shade (1.1, @selected_bg_color)
	radiance->default_style.gradient_shades[0] = 1.1;
	radiance->default_style.gradient_shades[1] = 1.0;
	radiance->default_style.gradient_shades[2] = 1.0;
	radiance->default_style.gradient_shades[3] = 0.9;
	radiance->default_style.roundness = 3;
	radiance->default_style.lightborder_shade = 1.26;
	radiance->default_style.lightborderstyle = 1;
	radiance->default_style.listviewstyle = 2;
	radiance->default_style.progressbarstyle = 0;
	radiance->default_style.colorize_scrollbar = FALSE;
	radiance->default_style.menubaritemstyle = 1;
	radiance->default_style.menubarstyle = 1;
	radiance->default_style.menustyle = 0;
	//radiance->default_style.focusstyle = 3;
	//radiance->default_style.handlestyle = 1;
	radiance->default_style.sliderstyle = 3;
	radiance->default_style.scrollbarstyle = 2;
	radiance->default_style.stepperstyle = 3;
	radiance->default_style.profile = MRN_PROFILE_MURRINE;

	memcpy(&radiance->dark_style, &radiance->default_style, sizeof(MurrineStyle));
	radiance->dark_style.colors.fg[WTK_STATE_NORMAL] = dark_fg_colour;
	radiance->dark_style.colors.fg[WTK_STATE_HOVER] = murrine_tshade(1.15, dark_fg_colour);
	radiance->dark_style.colors.fg[WTK_STATE_SELECTED] = dark_s_fg_colour;
	radiance->dark_style.colors.fg[WTK_STATE_DISABLED] = murrine_tshade(0.5, dark_fg_colour);
	radiance->dark_style.colors.fg[WTK_STATE_ACTIVE] = dark_fg_colour;

	radiance->dark_style.colors.bg[WTK_STATE_NORMAL] = dark_bg_colour;
	radiance->dark_style.colors.bg[WTK_STATE_HOVER] = murrine_color(0x4d,0x4c,0x48);
	radiance->dark_style.colors.bg[WTK_STATE_SELECTED] = selected_bg_colour;
	radiance->dark_style.colors.bg[WTK_STATE_DISABLED] = murrine_tshade(0.85, dark_bg_colour);
	radiance->dark_style.colors.bg[WTK_STATE_ACTIVE] = murrine_tshade(0.8, dark_bg_colour);

	radiance->dark_style.colors.text[WTK_STATE_NORMAL] = dark_fg_colour;
	radiance->dark_style.colors.text[WTK_STATE_HOVER] = murrine_tshade(1.15, dark_fg_colour);
	radiance->dark_style.colors.text[WTK_STATE_SELECTED] = dark_s_fg_colour;
	radiance->dark_style.colors.text[WTK_STATE_DISABLED] = murrine_tshade(0.85, dark_fg_colour);
	radiance->dark_style.colors.text[WTK_STATE_ACTIVE] = dark_fg_colour;
	radiance->dark_style.colors.shade[5] = radiance->dark_style.colors.text[WTK_STATE_DISABLED];
	radiance->dark_style.colors.shade[6] = radiance->dark_style.colors.text[WTK_STATE_ACTIVE];

	memcpy(&radiance->button_style, &radiance->default_style, sizeof(MurrineStyle));
	radiance->button_style.colors.bg[WTK_STATE_NORMAL] = murrine_tshade(1.07, button_colour);
	radiance->button_style.colors.bg[WTK_STATE_HOVER] = murrine_tshade(1.09, button_colour);
	radiance->button_style.colors.bg[WTK_STATE_ACTIVE] = murrine_tshade(1.0, button_colour);
	radiance->button_style.colors.bg[WTK_STATE_DISABLED] = murrine_color(0xE2, 0xE1, 0xE1);
	radiance->button_style.colors.fg[WTK_STATE_DISABLED] = murrine_color(0x9c, 0x9c, 0x9c);
	//radiance->dark_style.border_shades = {1.04, 0.82}
	radiance->button_style.reliefstyle = 5;
	//radiance->dark_style.shadow_shades = {1.02, 1.1}
	//radiance->dark_style.textstyle = 1;
	radiance->button_style.glowstyle = 5;
	radiance->button_style.glow_shade = 1.1;
	radiance->button_style.xthickness = 3;
	radiance->button_style.ythickness = 3;

	memcpy(&radiance->notebook_button_style, &radiance->default_style, sizeof(MurrineStyle));
	radiance->notebook_button_style.colors.bg[WTK_STATE_NORMAL] = bg_colour;
	radiance->notebook_button_style.colors.bg[WTK_STATE_HOVER] = murrine_tshade(1.04, bg_colour);
	radiance->notebook_button_style.colors.bg[WTK_STATE_ACTIVE] = murrine_tshade(0.96, bg_colour);
	radiance->notebook_button_style.colors.bg[WTK_STATE_DISABLED] = bg_colour;
	//radiance->dark_style.border_shades = {1.01, 0.8}
	radiance->notebook_button_style.reliefstyle = 5;
	//radiance->dark_style.shadow_shades = {1.0, 1.1}
	//radiance->dark_style.textstyle = 1;
	radiance->notebook_button_style.glowstyle = 5;
	radiance->notebook_button_style.glow_shade = 1.02;
	radiance->notebook_button_style.xthickness = 3;
	radiance->notebook_button_style.ythickness = 3;
	radiance->notebook_button_style.lightborder_shade = 1.26;

	memcpy(&radiance->spinbutton_style, &radiance->notebook_button_style, sizeof(MurrineStyle));
	radiance->spinbutton_style.xthickness = 5;

	memcpy(&radiance->wide_style, &radiance->default_style, sizeof(MurrineStyle));
	radiance->wide_style.xthickness = 2;
	radiance->wide_style.ythickness = 2;

	memcpy(&radiance->wider_style, &radiance->default_style, sizeof(MurrineStyle));
	radiance->wider_style.xthickness = 3;
	radiance->wider_style.ythickness = 3;

	memcpy(&radiance->entry_style, &radiance->default_style, sizeof(MurrineStyle));
	radiance->entry_style.xthickness = 3;
	radiance->entry_style.ythickness = 3;

	memcpy(&radiance->scrollbar_style, &radiance->button_style, sizeof(MurrineStyle));
	radiance->scrollbar_style.colors.bg[WTK_STATE_NORMAL] = bg_colour;
	radiance->scrollbar_style.colors.bg[WTK_STATE_HOVER] = murrine_tshade(1.04, bg_colour);
	radiance->scrollbar_style.colors.bg[WTK_STATE_ACTIVE] = murrine_tshade(0.96, bg_colour);
	//radiance->dark_style.border_shades = {0.95, 0.90}
	radiance->scrollbar_style.reliefstyle = 5;
	//radiance->dark_style.shadow_shades = {1.0, 1.1}
	//radiance->trough_shades = {0.92, 0.98}
	//radiance->dark_style.textstyle = 1;
	radiance->scrollbar_style.glowstyle = 5;
	radiance->scrollbar_style.glow_shade = 1.02;
	radiance->scrollbar_style.xthickness = 2;
	radiance->scrollbar_style.ythickness = 2;
	radiance->scrollbar_style.roundness = 20;
	radiance->scrollbar_style.contrast = 1.0;
	radiance->scrollbar_style.lightborder_shade = 1.3;
	radiance->scrollbar_style.gradient_shades[0] = 1.2;
	radiance->scrollbar_style.gradient_shades[1] = 1.0;
	radiance->scrollbar_style.gradient_shades[2] = 1.0;
	radiance->scrollbar_style.gradient_shades[3] = 0.86;

	memcpy(&radiance->overlay_scrollbar_style, &radiance->default_style, sizeof(MurrineStyle));
	radiance->overlay_scrollbar_style.colors.bg[WTK_STATE_SELECTED] = selected_bg_colour;
	radiance->overlay_scrollbar_style.colors.bg[WTK_STATE_DISABLED] = murrine_tshade(0.85, bg_colour);
	radiance->overlay_scrollbar_style.colors.bg[WTK_STATE_ACTIVE] = murrine_tshade(0.6, bg_colour);

	memcpy(&radiance->scale_style, &radiance->button_style, sizeof(MurrineStyle));
	radiance->scale_style.colors.bg[WTK_STATE_NORMAL] = bg_colour;
	radiance->scale_style.colors.bg[WTK_STATE_HOVER] = murrine_tshade(1.06, bg_colour);
	radiance->scale_style.colors.bg[WTK_STATE_ACTIVE] = murrine_tshade(0.94, bg_colour);
	radiance->scale_style.glowstyle = 5;
	//radiance->scale_style.handlestyle = 2;
	radiance->scale_style.glow_shade = 1.0;
	radiance->scale_style.roundness = 5;
	radiance->scale_style.contrast = 0.6;
	radiance->scale_style.lightborder_shade = 1.32;
	radiance->scale_style.gradient_shades[0] = 1.1;
	radiance->scale_style.gradient_shades[1] = 1.0;
	radiance->scale_style.gradient_shades[2] = 1.0;
	radiance->scale_style.gradient_shades[3] = 0.8;

	memcpy(&radiance->notebook_bg_style, &radiance->default_style, sizeof(MurrineStyle));
	radiance->notebook_bg_style.colors.bg[WTK_STATE_NORMAL] = murrine_tshade(1.02, bg_colour);
	radiance->notebook_bg_style.colors.bg[WTK_STATE_ACTIVE] = murrine_tshade(0.97, bg_colour);
	radiance->notebook_bg_style.colors.fg[WTK_STATE_ACTIVE] = murrine_tshade(1.3, bg_colour);

	memcpy(&radiance->notebook_style, &radiance->default_style, sizeof(MurrineStyle));
	//radiance->notebook_style.focusstyle = 2;
	radiance->notebook_style.xthickness = 2;
	radiance->notebook_style.ythickness = 2;
	radiance->notebook_style.roundness = 3;
	radiance->notebook_style.contrast = 0.8;
	radiance->notebook_style.lightborder_shade = 1.16;
	radiance->notebook_style.gradient_shades[0] = 1.1;
	radiance->notebook_style.gradient_shades[1] = 1.0;
	radiance->notebook_style.gradient_shades[2] = 1.0;
	radiance->notebook_style.gradient_shades[3] = 0.68;

	memcpy(&radiance->statusbar_style, &radiance->default_style, sizeof(MurrineStyle));
	radiance->statusbar_style.contrast = 1.2;

	memcpy(&radiance->comboboxentry_style, &radiance->notebook_button_style, sizeof(MurrineStyle));
	radiance->comboboxentry_style.glowstyle = 5;
	radiance->comboboxentry_style.xthickness = 3;
	radiance->comboboxentry_style.ythickness = 3;
	radiance->comboboxentry_style.glow_shade = 1.02;

	memcpy(&radiance->menubar_style, &radiance->dark_style, sizeof(MurrineStyle));
	radiance->menubar_style.gradient_shades[0] = 1.0;
	radiance->menubar_style.gradient_shades[1] = 1.0;
	radiance->menubar_style.gradient_shades[2] = 1.0;
	radiance->menubar_style.gradient_shades[3] = 1.0;
	radiance->menubar_style.lightborder_shade = 1.0;

	memcpy(&radiance->toolbar_style, &radiance->default_style, sizeof(MurrineStyle));
	radiance->toolbar_style.lightborder_shade = 1.0;

	memcpy(&radiance->toolbar_button_style, &radiance->notebook_button_style, sizeof(MurrineStyle));

	memcpy(&radiance->menu_style, &radiance->default_style, sizeof(MurrineStyle));
	radiance->menu_style.xthickness = 0;
	radiance->menu_style.ythickness = 0;
	radiance->menu_style.roundness = 0;

	memcpy(&radiance->menu_item_style, &radiance->menu_style, sizeof(MurrineStyle));
	radiance->menu_item_style.xthickness = 2;
	radiance->menu_item_style.ythickness = 3;
	radiance->menu_item_style.colors.fg[WTK_STATE_HOVER] = selected_fg_colour;
	radiance->menu_item_style.glowstyle = 5;
	radiance->menu_item_style.glow_shade = 1.1;

	memcpy(&radiance->menubar_item_style, &radiance->menu_style, sizeof(MurrineStyle));
	radiance->menubar_item_style.xthickness = 2;
	radiance->menubar_item_style.ythickness = 3;
	radiance->menubar_item_style.colors.fg[WTK_STATE_HOVER] = selected_fg_colour;
	radiance->menubar_item_style.glowstyle = 5;
	radiance->menubar_item_style.glow_shade = 1.0;
	radiance->menubar_item_style.lightborder_shade = 1.26;
	radiance->menubar_item_style.lightborderstyle = 3;
	radiance->menubar_item_style.gradient_shades[0] = 1.1;
	radiance->menubar_item_style.gradient_shades[1] = 1.0;
	radiance->menubar_item_style.gradient_shades[2] = 1.0;
	radiance->menubar_item_style.gradient_shades[3] = 0.88;
	

	return radiance;
}

void murrine_set_widget_parameters (MurrineStyle *murrine_style, int     state_type, int focused, int is_default,
                               WidgetParameters *params)
{
	

	params->active     = (state_type == WTK_STATE_ACTIVE);
	params->prelight   = (state_type == WTK_STATE_HOVER);
	params->disabled   = (state_type == WTK_STATE_DISABLED);
	params->state_type = (MurrineStateType)state_type;
	params->corners    = MRN_CORNER_ALL;
	params->ltr        = TRUE;
	params->focus      = focused;
	params->is_default = is_default;

	//if (!params->active && widget && MRN_IS_TOGGLE_BUTTON (widget))
	//	params->active = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget));

	params->xthickness = murrine_style->xthickness;
	params->ythickness = murrine_style->ythickness;

	params->glazestyle        = murrine_style->glazestyle;
	params->glow_shade        = murrine_style->glow_shade;
	params->glowstyle         = murrine_style->glowstyle;
	params->highlight_shade   = murrine_style->highlight_shade;
	params->lightborder_shade = murrine_style->lightborder_shade;
	params->lightborderstyle  = murrine_style->lightborderstyle;
	params->reliefstyle       = murrine_style->reliefstyle;
	params->roundness         = murrine_style->roundness;

	MurrineGradients mrn_gradient;
	if (murrine_style->gradients)
	{
		mrn_gradient.gradient_shades[0] = murrine_style->gradient_shades[0];
		mrn_gradient.gradient_shades[1] = murrine_style->gradient_shades[1];
		mrn_gradient.gradient_shades[2] = murrine_style->gradient_shades[2];
		mrn_gradient.gradient_shades[3] = murrine_style->gradient_shades[3];
	}
	else
	{
		mrn_gradient.gradient_shades[0] = 1.0;
		mrn_gradient.gradient_shades[1] = 1.0;
		mrn_gradient.gradient_shades[2] = 1.0;
		mrn_gradient.gradient_shades[3] = 1.0;
	}
	mrn_gradient.gradients = murrine_style->gradients;
	mrn_gradient.use_rgba = FALSE;//(murrine_widget_is_rgba ((GtkWidget*) widget) &&
	                      //   murrine_style->rgba);
	mrn_gradient.rgba_opacity = GRADIENT_OPACITY;

	MurrineStyles mrn_style = MRN_STYLE_MURRINE;
	if (mrn_gradient.use_rgba)
	{
		mrn_style = MRN_STYLE_RGBA;
	}
	params->mrn_gradient = mrn_gradient;
	params->style = mrn_style;
	murrine_register_style_murrine (&murrine_fnc);
	params->style_functions = &murrine_fnc;

	/* I want to avoid to have to do this. I need it for GtkEntry, unless I
	   find out why it doesn't behave the way I expect it to. */
	params->parentbg = murrine_style->colors.bg[state_type];
}

void murrine_draw_btn(cairo_t *ctx, int focused, int is_default, int state, int w, int h)
{
	MurrineTheme *thm = murrine_radiance();
	WidgetParameters params;
	murrine_set_widget_parameters(&(thm->button_style), state, focused, is_default, &params);	
	params.style_functions->draw_button(ctx, &(thm->button_style.colors),&params, 0, 0, w, h,TRUE);
	free(thm);
}

void murrine_draw_menubar(cairo_t *ctx, int w, int h, int style)
{
	MurrineTheme *thm = murrine_radiance();
	WidgetParameters params;
	murrine_set_widget_parameters(&(thm->menubar_style), WTK_STATE_NORMAL, FALSE, FALSE, &params);	
	params.style_functions->draw_menubar(ctx, &(thm->menubar_style.colors),&params, 0, 0, w, h, style);
	free(thm);
}

void murrine_draw_textbox(cairo_t *ctx, int w, int h, int focused)
{
	MurrineTheme *thm = murrine_radiance();
	WidgetParameters params;
	FrameParameters  frame;
	frame.shadow    = WTK_SHADOW_ETCHED_IN;
	frame.gap_side  = MRN_GAP_LEFT;
	frame.gap_x     = 5;
	frame.gap_width = 0;
	frame.border    = &thm->notebook_style.colors.shade[4];
	murrine_set_widget_parameters(&(thm->notebook_style), WTK_STATE_NORMAL, focused, FALSE, &params);	
	params.style_functions->draw_frame(ctx, &(thm->notebook_style.colors), &params, &frame, 0, 0, w, h);
	free(thm);
}
