#include <stdint.h>
#define KBD_CPSLCK_BIT 1
#define KBD_SHIFT_BIT 2
#define KBD_CTRL_BIT 4
#define KBD_ALT_BIT 8
#define KBD_WIN_BIT 16
#define KBD_MENU_BIT 32
#define KBD_SCLLCK_BIT 64
#define KBD_NUMLCK_BIT 128

#include <glib.h>
int kbdus_reg[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',     /* 9 */
  '9', '0', '-', '=', KEY_BACKSPACE,     /* Backspace */
  '\t',                 /* Tab */
  'q', 'w', 'e', 'r',   /* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\r', /* Enter key */
    0,                  /* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',     /* 39 */
 '\'', '`',   0,                /* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',                    /* 49 */
  'm', ',', '.', '/',   0,                              /* Right shift */
  '*',
    0,  /* Alt */
  ' ',  /* Space bar */
    0,  /* Caps lock */
    KEY_F(1),   KEY_F(2),   KEY_F(3),   KEY_F(4),   KEY_F(5),   KEY_F(6),   KEY_F(7),   KEY_F(8),   KEY_F(9),
    KEY_F(10),  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    KEY_HOME,  /* Home key */
    KEY_UP,  /* Up Arrow */
    KEY_PPAGE,  /* Page Up */
  '-',
    KEY_LEFT,  /* Left Arrow */
    0,
    KEY_RIGHT,  /* Right Arrow */
  '+',
    KEY_END,  /* 79 - End key*/
    KEY_DOWN,  /* Down Arrow */
    KEY_NPAGE,  /* Page Down */
    KEY_IC,  /* Insert Key */
    KEY_DC,  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */
};              
int kbdus_caps[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',     /* 9 */
  '9', '0', '-', '=', KEY_BACKSPACE,     /* Backspace */
  '\t',                 /* Tab */
  'Q', 'W', 'E', 'R',   /* 19 */
  'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '\r', /* Enter key */
    0,                  /* 29   - Control */
  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';',     /* 39 */
 '\'', '`',   0,                /* Left shift */
 '\\', 'Z', 'X', 'C', 'V', 'B', 'N',                    /* 49 */
  'M', ',', '.', '/',   0,                              /* Right shift */
  '*',
    0,  /* Alt */
  ' ',  /* Space bar */
    0,  /* Caps lock */
    KEY_F(1),   KEY_F(2),   KEY_F(3),   KEY_F(4),   KEY_F(5),   KEY_F(6),   KEY_F(7),   KEY_F(8),   KEY_F(9),
    KEY_F(10),  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    KEY_HOME,  /* Home key */
    KEY_UP,  /* Up Arrow */
    KEY_PPAGE,  /* Page Up */
  '-',
    KEY_LEFT,  /* Left Arrow */
    0,
    KEY_RIGHT,  /* Right Arrow */
  '+',
    KEY_END,  /* 79 - End key*/
    KEY_DOWN,  /* Down Arrow */
    KEY_NPAGE,  /* Page Down */
    KEY_IC,  /* Insert Key */
    KEY_DC,  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */
};  
            
int kbdus_shift[128] =
{
        0,  27, '!', '@', '#', '$', '%', '^', '&', '*', /* 9 */
  '(', ')', '_', '+', KEY_BACKSPACE,     /* Backspace */
  '\t',                 /* Tab */
  'Q', 'W', 'E', 'R',   /* 19 */
  'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\r', /* Enter key */
    0,                  /* 29   - Control */
        'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',       /* 39 */
 '\"', '~',   0,                /* Left shift */
 '|', 'Z', 'X', 'C', 'V', 'B', 'N',                     /* 49 */
  'M', '<', '>', '?',   0,                              /* Right shift */
  '*',
    0,  /* Alt */
  ' ',  /* Space bar */
    0,  /* Caps lock */
    KEY_F(1),   KEY_F(2),   KEY_F(3),   KEY_F(4),   KEY_F(5),   KEY_F(6),   KEY_F(7),   KEY_F(8),   KEY_F(9),
    KEY_F(10),  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    KEY_HOME,  /* Home key */
    KEY_UP,  /* Up Arrow */
    KEY_PPAGE,  /* Page Up */
  '-',
    KEY_LEFT,  /* Left Arrow */
    0,
    KEY_RIGHT,  /* Right Arrow */
  '+',
    KEY_END,  /* 79 - End key*/
    KEY_DOWN,  /* Down Arrow */
    KEY_NPAGE,  /* Page Down */
    KEY_IC,  /* Insert Key */
    KEY_DC,  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */
};  
              
int kbdus_ctrl[128] =
{
        0,  27, '!', '@', '#', '$', '%', '^', '&', '*', /* 9 */
  '(', ')', '_', '+', KEY_BACKSPACE,     /* Backspace */
  '\t',                 /* Tab */
  'Q', 'W', 'E', 'R',   /* 19 */
  'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', /* Enter key */
    0,                  /* 29   - Control */
        'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',       /* 39 */
 '\"', '~',   0,                /* Left shift */
 '|', KEY_SUSPEND, 'X', 3, 'V', 'B', 'N',                     /* 49 */
  'M', '<', '>', '?',   0,                              /* Right shift */
  '*',
    0,  /* Alt */
  ' ',  /* Space bar */
    0,  /* Caps lock */
    KEY_VT(1),   KEY_VT(2),   KEY_VT(3),   KEY_VT(4),   KEY_VT(5),   KEY_VT(6),   KEY_VT(7),   KEY_VT(8),   KEY_VT(9),
    KEY_VT(10),  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    KEY_HOME,  /* Home key */
    KEY_UP,  /* Up Arrow */
    KEY_PPAGE,  /* Page Up */
  '-',
    KEY_LEFT,  /* Left Arrow */
    0,
    KEY_RIGHT,  /* Right Arrow */
  '+',
    KEY_END,  /* 79 - End key*/
    KEY_DOWN,  /* Down Arrow */
    KEY_NPAGE,  /* Page Down */
    KEY_IC,  /* Insert Key */
    KEY_DC,  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */
};     

int decode_keystroke(uint8_t scancode,uint8_t modifier){
        int result;
        if (modifier & KBD_CPSLCK_BIT){
                result = kbdus_caps[scancode];
        } else if (modifier & KBD_SHIFT_BIT){
                result = kbdus_shift[scancode];
        } else if (modifier & KBD_CTRL_BIT){
                result = kbdus_ctrl[scancode];
        }  else {
                result = kbdus_reg[scancode];
        }
        return result;
}

