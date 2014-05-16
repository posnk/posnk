#include <string.h>
#include <stdint.h>

void kb_initialize();
void kb_set_leds(uint8_t leds);
void kb_toggle_modifier(uint8_t mask);
void kb_set_modifier(uint8_t mask);
void kb_clear_modifier(uint8_t mask);
void kb_pressed(uint8_t scan);
void kb_typed(uint8_t scan);
void kb_released(uint8_t scan);


