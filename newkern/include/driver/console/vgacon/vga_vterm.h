#ifndef __text_video_h__
#define __text_video_h__
#include "arch/i386/x86.h"

typedef struct vga_vterm_screen_character {
	char character;
	char attribute;
} vga_vterm_screen_character_t;

typedef struct vgacon_vc {
	short cursor_x;
	short cursor_y;
	char page_id;
	volatile vga_vterm_screen_character_t *video_buffer;
} vga_vterm_vc_t;

void vga_vterm_write_crtc_register(char id,char val);
#endif
