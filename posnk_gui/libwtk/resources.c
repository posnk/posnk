#include <cairo.h>
#include <stdint.h>	
#include <ft2build.h>
#include <cairo-ft.h>
#include <assert.h>

FT_Library 	 	 wtk_font_lib;
FT_Face 	 	 wtk_normal_font_ft;
cairo_font_face_t	*wtk_normal_font;

void wtk_resources_init()
{
	int st;
	FT_Init_FreeType( &wtk_font_lib );
	st = FT_New_Face( wtk_font_lib, "/share/oswin/title.ttf", 0, &wtk_normal_font_ft );
	assert(st == 0);
	wtk_normal_font =  cairo_ft_font_face_create_for_ft_face(wtk_normal_font_ft,0);
	assert(wtk_normal_font != 0);
}

cairo_font_face_t *wtk_get_normal_font()
{
	if(!wtk_normal_font) 
		wtk_resources_init();
	return wtk_normal_font;
}
