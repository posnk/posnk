#include <cairo.h>
#include <stdint.h>	
#include <ft2build.h>
#include <cairo-ft.h>
#include <assert.h>

FT_Library 	 	 wtk_font_lib;
FT_Face 	 	 wtk_normal_font_ft;
cairo_font_face_t	*wtk_normal_font;
cairo_surface_t		*wtk_button_images[3][3];

const char *wtk_button_states[3] = {"default","hover","pressed"};
const char *wtk_button_parts[3] = {"left","mid","right"};

void wtk_resources_init()
{
	int st,p,s;
	char pathbuf[80];
	FT_Init_FreeType( &wtk_font_lib );
	st = FT_New_Face( wtk_font_lib, "/share/oswin/title.ttf", 0, &wtk_normal_font_ft );
	assert(st == 0);
	wtk_normal_font =  cairo_ft_font_face_create_for_ft_face(wtk_normal_font_ft,0);
	assert(wtk_normal_font != 0);
	for (s = 0; s < 3; s++)
		for (p = 0; p < 3; p++) {
			sprintf(pathbuf, "/share/wtk/button_%s_%s.png",wtk_button_states[s],wtk_button_parts[p]);
			wtk_button_images[s][p] = cairo_image_surface_create_from_png (pathbuf);
		}
}

cairo_font_face_t *wtk_get_normal_font()
{
	if(!wtk_normal_font) 
		wtk_resources_init();
	return wtk_normal_font;
}


