#include <_stdlib.h>
#include <string.h>

char intbuf[32];
char bchars[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

void utoa(unsigned int value,int base,char *str){
   int pos = 0;
   int opos = 0;
   int top = 0;

   if (value == 0 || base > 16) {
      str[0] = '0';
      str[1] = '\0';
      return;
   }

   while (value != 0) {
      intbuf[pos] = bchars[value % base];
      pos++;
      value /= base;
   }
   top=pos--;
   for (opos=0; opos<top; pos--,opos++) {
      str[opos] = intbuf[pos];
   }
   str[opos] = 0;
}
void itoa( int value,int base,char *str){
   int pos = 0;
   int opos = 0;
   int top = 0;

   if (value == 0 || base > 16) {
      str[0] = '0';
      str[1] = '\0';
      return;
   }

   while (value != 0) {
      intbuf[pos] = bchars[value % base];
      pos++;
      value /= base;
   }
   top=pos--;
   for (opos=0; opos<top; pos--,opos++) {
      str[opos] = intbuf[pos];
   }
   str[opos] = 0;
}

void itoa_s(int value,int base,char *str){
	if (value < 0)
		*str++ = '-';
	itoa(value,base,str);
}
void utoa_s(int value,int base,char *str){
	if (value < 0)
		*str++ = '-';
	utoa(value,base,str);
}