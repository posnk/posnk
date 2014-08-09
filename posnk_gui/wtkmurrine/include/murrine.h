
#define WTK_STATE_NORMAL	(0)
#define WTK_STATE_HOVER		(1)
#define WTK_STATE_SELECTED	(2)
#define WTK_STATE_DISABLED	(3)
#define WTK_STATE_ACTIVE	(4)

void murrine_draw_btn(cairo_t *ctx, int focused, int is_default, int state, int w, int h);

void murrine_draw_textbox(cairo_t *ctx, int w, int h, int focused);

void murrine_draw_menubar(cairo_t *ctx, int w, int h, int style);

void murrine_draw_menubar_item(cairo_t *ctx, int w, int h, int focused);
