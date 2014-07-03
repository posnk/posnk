#include "driver/console/vgacon/vga_vterm.h"
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include "kernel/tty.h"
#include "driver/console/vterm/vterm.h"
#include "driver/console/vterm/vterm_private.h"

void vterm_tty_setup(char *name, dev_t major, int minor_count, int rows, int cols);

vga_vterm_vc_t vterm_vga_all_vcs[9];

int vterm_vga_current_vc = 0;

int vga_vterm_colour_map[8] = {0,4,2,6,1,5,3,7};

volatile vga_vterm_screen_character_t *vga_vterm_get_text_video_memory(){
	return (volatile vga_vterm_screen_character_t *)(0xC00B8000); //TODO: Check for paging init
}

void vga_vterm_move_cursor(vga_vterm_vc_t *vc)
{
	int offset = (vc->cursor_x + (vc->cursor_y * 80));
	vterm_vga_write_crtc_register(0x0E,offset >> 8);
	vterm_vga_write_crtc_register(0x0F,offset);
}

void vterm_vga_position(vga_vterm_vc_t *vc, int x, int y,int move_cursor_)
{
	vc->cursor_x = x;
	vc->cursor_y = y;
	if (move_cursor_)
		vga_vterm_move_cursor(vc);
}

void vterm_vga_write_crtc_register(char id,char val){
	i386_outb(0x3D4,id);
	i386_outb(0x3D5,val);
}

inline char vga_vterm_map_attr(int attr)
{
	char fg, bg;
	fg = (char) vga_vterm_colour_map[attr & 0xF];
	bg = (char) vga_vterm_colour_map[(attr & 0xF0) >> 4];
	if (attr & A_BOLD)
		fg |= 8;
	//if (attr & A_BOLD)
	//	bg |= 8;
	return fg | (bg << 4);
}

void vterm_invalidate_screen(vterm_t *vt)
{
	int row, col;
	vga_vterm_vc_t *vc = &vterm_vga_all_vcs[MINOR(vt->device_id)];
	volatile vga_vterm_screen_character_t *cpt = vc->video_buffer;
	for (row = 0; row < vt->rows; row++)
		for (col = 0; col < vt->cols; col++) {
			cpt = (void *)(((uintptr_t)vc->video_buffer) + (col + row * 80)*2);
			cpt->character = (char) vt->cells[row][col].ch;
			cpt->attribute = (char) vga_vterm_map_attr(vt->cells[row][col].attr);
		}
	vterm_vga_position(vc, vt->ccol, vt->crow, 1);
}

void vterm_invalidate_cell(vterm_t *vt, int row, int col)
{
	vga_vterm_vc_t *vc = &vterm_vga_all_vcs[MINOR(vt->device_id)];
	volatile vga_vterm_screen_character_t *cpt = vc->video_buffer;
	cpt = (void *)(((uintptr_t)vc->video_buffer) + (col + row * 80)*2);
	cpt->character = (char) vt->cells[row][col].ch;
	cpt->attribute = (char) vga_vterm_map_attr(vt->cells[row][col].attr);
}

void vterm_invalidate_cursor(vterm_t *vt)
{
	vga_vterm_vc_t *vc = &vterm_vga_all_vcs[MINOR(vt->device_id)];	
	volatile vga_vterm_screen_character_t *cpt = vc->video_buffer;
	cpt = (void *)(((uintptr_t)vc->video_buffer) + (vt->ccol + vt->crow * 80)*2);
	cpt->attribute = (char) vga_vterm_map_attr(vt->curattr);
	if (MINOR(vt->device_id) == vterm_vga_current_vc)
		vterm_vga_position(vc, vt->ccol, vt->crow, 1);
}

void vterm_handle_bell(vterm_t *vt)
{
}

void vterm_tty_invalidate_screen(dev_t dev);
void vterm_vga_switch_vc(int vc)
{
	short offset = (short)(vterm_vga_all_vcs[vc].video_buffer - vga_vterm_get_text_video_memory());

	vterm_vga_current_vc = vc;
	vterm_tty_invalidate_screen(MAKEDEV(2, vc));
        vterm_vga_write_crtc_register(0x0C,offset >> 8);
        vterm_vga_write_crtc_register(0x0D,(char) offset & 0xFF);

}

void vterm_post_key_tty(dev_t dev, int keycode);
void con_handle_key(int keycode)
{
	if (keycode > KEY_VT0){
		vterm_vga_switch_vc(keycode - KEY_VT(1));
	} else
		vterm_post_key_tty(MAKEDEV(2,vterm_vga_current_vc), keycode);
}

void vterm_vga_init(){
	int vc_id;
        for (vc_id = 0;vc_id < 9;vc_id++){
                vterm_vga_all_vcs[vc_id].cursor_x = 0;
                vterm_vga_all_vcs[vc_id].cursor_y = 0;  
                vterm_vga_all_vcs[vc_id].page_id = vc_id;
                vterm_vga_all_vcs[vc_id].video_buffer = (vga_vterm_screen_character_t *)
			( ((uint32_t)vga_vterm_get_text_video_memory()) + 80 * 25 * 2 * vc_id);
        }
	vterm_tty_setup("vgacon", 2, 9, 25,80);
	vterm_vga_switch_vc(0);
	kb_initialize();
}
