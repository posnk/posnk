#include "driver/console/vgacon/vgacon.h"
#include <string.h>
#include <stdint.h>
#include "kernel/console.h"

vgacon_vc_info_t vgacon_early_vc;

volatile vgacon_screen_character_t *vgacon_early_get_text_video_memory(){
	return (volatile vgacon_screen_character_t *)(0xC00B8000); //TODO: Check for paging init
}

/**
 * Write character to text buffer and move cursor
*/
void vgacon_early_putch(char c){
	int offset = (vgacon_early_vc.cursor_x + (vgacon_early_vc.cursor_y * 80/*get_BDA()->columns_per_row*/))*2;
	volatile vgacon_screen_character_t *location;
	location = (vgacon_screen_character_t *) ( ((int)vgacon_early_vc.video_buffer) + offset);
	location->attribute = vgacon_early_vc.attrib;
	location->character = c;
	vgacon_early_vc.cursor_x++;
	if (vgacon_early_vc.cursor_x >= 80){
		vgacon_early_vc.cursor_x = 0;
		vgacon_early_vc.cursor_y++;
	}
}

static void vgacon_early_scroll()
{
	unsigned short *fb_adr;
	unsigned blank, temp;
	if (vgacon_early_vc.no_scroll)
		return;
	blank = 0x0;
	fb_adr = (unsigned short *)vgacon_early_vc.video_buffer;
/* scroll up */
	if(vgacon_early_vc.cursor_y >= 25)
	{
		temp = vgacon_early_vc.cursor_y - 25 + 1;
		memcpy(fb_adr, fb_adr + temp * 80,
			(25 - temp) * 80 * 2);
/* blank the bottom line of the screen */
		memset(fb_adr + (25 - temp) * 80,
			blank, 160);
		vgacon_early_vc.cursor_y = 25 - 1;
	}
}

void vgacon_early_write_crtc_register(char id,char val){
	i386_outb(0x3D4,id);
	i386_outb(0x3D5,val);
}

unsigned char vgacon_early_read_crtc_register(char id){
	i386_outb(0x3D4,id);
	return i386_inb(0x3D5);
}


void vgacon_early_move_cursor()
{
	int offset = (vgacon_early_vc.cursor_x + (vgacon_early_vc.cursor_y * 80/*get_BDA()->columns_per_row*/));
	vgacon_early_write_crtc_register(0x0E,offset >> 8);
	vgacon_early_write_crtc_register(0x0F,offset);
}

void vgacon_early_get_cursor()
{
	int offset = (vgacon_early_read_crtc_register(0x0e) << 8) | vgacon_early_read_crtc_register(0x0f);
	vgacon_early_vc.cursor_y = offset / 80;
	vgacon_early_vc.cursor_x = offset % 80;
}

void  vgacon_early_putc(char c)
{
	switch (vgacon_early_vc.escape){
		case 0:
			break;
		case 1:
			if (c == '['){
				vgacon_early_vc.escape = 2;
				return;
			} else
				vgacon_early_vc.escape = 0;
			break;
		case 2:
			break;//vgacon_early_handle_escape(c);
	}
	if (c == 0x1B)
		vgacon_early_vc.escape = 1;
	else if (c == '\b'){
		if (vgacon_early_vc.cursor_x > 0){
			vgacon_early_vc.cursor_x--;
			vgacon_early_putch(' ');
			vgacon_early_vc.cursor_x--;
		} else if (vgacon_early_vc.cursor_y > 0){
			vgacon_early_vc.cursor_x = 79;
			vgacon_early_vc.cursor_y--;
			vgacon_early_putch(' ');
			vgacon_early_vc.cursor_x = 79;
			vgacon_early_vc.cursor_y--;
		}
	} else if (c == '\t')
		vgacon_early_vc.cursor_x = (vgacon_early_vc.cursor_x + 8) % 8;
	else if ((c == '\n') || (c == '\r')){
		vgacon_early_vc.cursor_x = 0;
		vgacon_early_vc.cursor_y = vgacon_early_vc.cursor_y + 1;
	} else if(c >= ' ')
		vgacon_early_putch(c);
	vgacon_early_scroll();
	vgacon_early_move_cursor();
}

void vgacon_early_clear_video(){
	unsigned short *fb_adr;
	unsigned blank;
	blank = 0x20 | ((unsigned)vgacon_early_vc.attrib << 8);
	fb_adr = (unsigned short *) vgacon_early_vc.video_buffer;
	memset(fb_adr,blank, 160*25);
}

void vgacon_early_clear_line()
{
	unsigned short *fb_adr;
	unsigned blank;
	blank = 0x20 | ((unsigned)vgacon_early_vc.attrib << 8);
	fb_adr = (unsigned short *) ( ((int) vgacon_early_vc.video_buffer) + ( 80 * vgacon_early_vc.cursor_y * 2 ) );
	memset(fb_adr,blank, 160);
}

void vgacon_early_position(int x,int y,int move_cursor_)
{
	vgacon_early_vc.cursor_x = x;
	vgacon_early_vc.cursor_y = y;
	if (move_cursor_)
		vgacon_early_move_cursor();
}

void vgacon_early_puts(
	__attribute__((__unused__)) int sink,
	__attribute__((__unused__)) int flags, const char *str)
{
	int i = 0;
   	while (str[i])
   	{
		vgacon_early_putc(str[i++]);
   	}
}

void vgacon_early_set_attrib(int foreground,int background)
{
	vgacon_early_vc.attrib = ( char ) ( ( background << 4 ) | foreground );
}

void vgacon_early_disable_scroll(int scroll){
	vgacon_early_vc.no_scroll = scroll;
}

void vgacon_early_init(){}

void earlycon_init() {
	vgacon_early_vc.attrib = 0x07;
	vgacon_early_vc.cursor_x = 0;
	vgacon_early_vc.cursor_y = 0;
	vgacon_early_vc.no_scroll = 0;
	vgacon_early_vc.page_id =0;
	vgacon_early_vc.escape = 0;
	vgacon_early_vc.video_buffer = (vgacon_screen_character_t *)( ((uint32_t)vgacon_early_get_text_video_memory()));
	vgacon_early_get_cursor();
  	con_register_sink_s( "vgacon_early", CON_SINK_EARLY, vgacon_early_puts );
}

