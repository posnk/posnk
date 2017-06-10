#include "driver/console/vgacon/vgacon.h"
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include "kernel/tty.h"

extern vgacon_vc_info_t vgacon_early_vc;

int vgacon_colour_map[8] = {0,4,2,6,1,5,3,7};

#define vgacon_vc vgacon_early_vc

volatile vgacon_screen_character_t *vgacon_get_text_video_memory(){
	return (volatile vgacon_screen_character_t *)(0xC00B8000); //TODO: Check for paging init
}

/**
 * Write character to text buffer and move cursor
*/
void vgacon_putch(char c){
	int offset = (vgacon_vc.cursor_x + (vgacon_vc.cursor_y * 80))*2;	
	volatile vgacon_screen_character_t *location;
	location = (vgacon_screen_character_t *) ( ((int)vgacon_vc.video_buffer) + offset);
	location->attribute = vgacon_vc.attrib;
	location->character = c;
	vgacon_vc.cursor_x++;
	if (vgacon_vc.cursor_x >= 80){
		vgacon_vc.cursor_x = 0;
		vgacon_vc.cursor_y++;
	}		
}

static void vgacon_scroll()
{
	unsigned short *fb_adr;
	unsigned blank, temp;
	if (vgacon_vc.no_scroll)
		return;
	blank = 0x0;
	fb_adr = (unsigned short *)vgacon_vc.video_buffer;
/* scroll up */
	if(vgacon_vc.cursor_y >= 25)
	{
		temp = vgacon_vc.cursor_y - 25 + 1;
		memcpy(fb_adr, fb_adr + temp * 80,
			(25 - temp) * 80 * 2);
/* blank the bottom line of the screen */
		memset(fb_adr + (25 - temp) * 80,
			blank, 160);
		vgacon_vc.cursor_y = 25 - 1;
	}
}

void vgacon_move_cursor()
{
	int offset = (vgacon_vc.cursor_x + (vgacon_vc.cursor_y * 80/*get_BDA()->columns_per_row*/));
	vgacon_write_crtc_register(0x0E,offset >> 8);
	vgacon_write_crtc_register(0x0F,offset);
}

void vgacon_handle_sgr(int par){
//	debugcon_aprintf("SGR : %i\n",par);
	if (par == 0) {
		vgacon_vc.attrib = 0x07;
 	} else if ((par >=30) && (par <= 37)) {
		par -= 30;
		vgacon_vc.attrib &= 0xF0;
		vgacon_vc.attrib |= vgacon_colour_map[par];
	} else if ((par >=40) && (par <= 47)) {
		par -= 40;
		vgacon_vc.attrib &= 0x0F;
		vgacon_vc.attrib |= vgacon_colour_map[par] << 4;
	}
}

char vgacon_escape_buf[20];
int vgacon_escape_ptr = 0;
int vgacon_escape_spt = 0;
int vgacon_escape_ppt = 0;
int vgacon_escape_pars[10];

int vgacon_parse_par(char *str)
{
	size_t l = strlen(str);
	size_t n;
	char c; 
	int acc = 0;
	for (n = 0; n < l; n++) {
		c = str[n];
		if ((c >= '0') && (c <= '9'))
			c -= '0';
		//else if ((c >= 'A') && (c <= 'F'))
		//	c -= 'A' - 10;
		//else if ((c >= 'a') && (c <= 'f'))
		//	c -= 'a' - 10;
		else
			break; 
		acc *= 10;
		acc += ((int)c);
	}
	return acc;
}



void vgacon_handle_escape(char c)
{
	int par;
	vgacon_escape_buf[vgacon_escape_ptr++] = c;
	vgacon_escape_buf[vgacon_escape_ptr] = 0;
	if ((c >= 'a') && (c <= 'z')) {
		vgacon_escape_pars[vgacon_escape_ppt++] = vgacon_parse_par(&vgacon_escape_buf[vgacon_escape_spt]);
		vgacon_escape_spt = vgacon_escape_ptr;
		switch (c) {
			case 'm': //SGR
				for (par = 0; par < vgacon_escape_ppt; par++)
					vgacon_handle_sgr(vgacon_escape_pars[par]);
				break;
			case 'f':
				
				vgacon_vc.cursor_y = vgacon_escape_pars[0];
				vgacon_vc.cursor_x = vgacon_escape_pars[1];
				break;
			case 'h':
				break;
			default:
				break;
		}
		vgacon_escape_ptr = 0;
		vgacon_escape_spt = 0;
		vgacon_escape_ppt = 0;
		vgacon_vc.escape = 0;
		vgacon_escape_pars[0] = 0;
		vgacon_escape_pars[1] = 0;
	} else if ((c >= 'A') && (c <= 'Z')) {
		vgacon_escape_pars[vgacon_escape_ppt++] = vgacon_parse_par(&vgacon_escape_buf[vgacon_escape_spt]);
		vgacon_escape_spt = vgacon_escape_ptr;
		switch (c) {
			case 'H':
				vgacon_vc.cursor_y = vgacon_escape_pars[0];
				vgacon_vc.cursor_x = vgacon_escape_pars[1];
				break;
			case 'M':
				vgacon_vc.cursor_y--;
				break;
			case 'D':
				vgacon_vc.cursor_y++;
				break;
			case 'E':
				vgacon_vc.cursor_y++;
				break;
			default:
				break;
		}
		vgacon_escape_ptr = 0;
		vgacon_escape_spt = 0;
		vgacon_escape_ppt = 0;
		vgacon_vc.escape = 0;
		vgacon_escape_pars[0] = 0;
		vgacon_escape_pars[1] = 0;
	} else if (c == ';') {		
		vgacon_escape_pars[vgacon_escape_ppt++] = vgacon_parse_par(&vgacon_escape_buf[vgacon_escape_spt]);
		vgacon_escape_spt = vgacon_escape_ptr;
	}
}

int vgacon_putc(__attribute__((__unused__)) dev_t dev, char c)
{
	switch (vgacon_vc.escape){
		case 0:
			break;
		case 1:
			if (c == '['){
				vgacon_vc.escape = 2;
				return 0;
			} else
				vgacon_vc.escape = 0;
			break;
		case 2:
			//debugcon_putc(c);
			vgacon_handle_escape(c);
			return 0;
	}
	if (c == 0x1B)
		vgacon_vc.escape = 1;
	else if (c == '\b'){
		if (vgacon_vc.cursor_x > 0){
			vgacon_vc.cursor_x--;
		} else if (vgacon_vc.cursor_y > 0){
			vgacon_vc.cursor_x = 79;			
			vgacon_vc.cursor_y--;
		}
	} else if (c == '\t')
		vgacon_vc.cursor_x += 8;
	else if ((c == '\n') || (c == '\r')){
		vgacon_vc.cursor_x = 0;
		vgacon_vc.cursor_y = vgacon_vc.cursor_y + 1;
	} else if(c >= ' ')
		vgacon_putch(c);
	if (vgacon_vc.cursor_x >= 80){
		vgacon_vc.cursor_x = 0;
		vgacon_vc.cursor_y = vgacon_vc.cursor_y + 1;
	}		
	vgacon_scroll();
	vgacon_move_cursor();
	return 0;
}

void vgacon_clear_video(){
	unsigned short *fb_adr;
	unsigned blank;
	blank = 0x00;
	fb_adr = (unsigned short *) vgacon_vc.video_buffer;
	memset(fb_adr,blank, 160*25);
}

void vgacon_clear_line()
{
	unsigned short *fb_adr;
	unsigned blank;
	blank = 0x20 | ((unsigned)vgacon_vc.attrib << 8);
	fb_adr = (unsigned short *) ( ((int) vgacon_vc.video_buffer) + ( 80 * vgacon_vc.cursor_y * 2 ) );
	memset(fb_adr,blank, 160);
}

void vgacon_position(int x,int y,int move_cursor_)
{
	vgacon_vc.cursor_x = x;
	vgacon_vc.cursor_y = y;
	if (move_cursor_)
		vgacon_move_cursor();
}

void vgacon_set_attrib(int foreground,int background)
{
	vgacon_vc.attrib = ( char ) ( ( background << 4 ) | foreground );
}

void vgacon_disable_scroll(int scroll){
	vgacon_vc.no_scroll = scroll;
}

void vgacon_write_crtc_register(char id,char val){
	i386_outb(0x3D4,id);
	i386_outb(0x3D5,val);
}

void vgacon_init(){
	/*vgacon_vc.attrib = 0x07;
	vgacon_vc.cursor_x = 0;
	vgacon_vc.cursor_y = 0;		
	vgacon_vc.no_scroll = 0;
	vgacon_vc.page_id =0;
	vgacon_vc.escape = 0;
	vgacon_vc.video_buffer = (vgacon_screen_character_t *)( ((uint32_t)vgacon_get_text_video_memory()));*/
	tty_register_driver("vgacon", 12, 1, &vgacon_putc);
}
