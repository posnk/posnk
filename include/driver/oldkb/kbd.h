#include <string.h>
#include <stdint.h>
#define KBD_CPSLCK_BIT 1
#define KBD_SHIFT_BIT 2
#define KBD_CTRL_BIT 4
#define KBD_ALT_BIT 8
#define KBD_WIN_BIT 16
#define KBD_MENU_BIT 32
#define KBD_SCLLCK_BIT 64
#define KBD_NUMLCK_BIT 128

int decode_keystroke(uint8_t scancode,uint8_t modifier);
void kb_initialize();
void kb_set_leds(uint8_t leds);
void kb_toggle_modifier(uint8_t mask);
void kb_set_modifier(uint8_t mask);
void kb_clear_modifier(uint8_t mask);
void kb_pressed(uint8_t scan);
void kb_typed(uint8_t scan);
void kb_released(uint8_t scan);


