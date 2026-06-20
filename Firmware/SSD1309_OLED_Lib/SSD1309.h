#ifndef SSD1309_H_
#define SSD1309_H_

#include <stdint.h>

#define SSD1309_BLACK 0
#define SSD1309_WHITE 1
#define SSD1309_INVERSE 2

#define SSD1309_SETCONTRAST 0x81
#define SSD1309_DISPLAYALLON_RESUME 0xA4
#define SSD1309_DISPLAYALLON 0xA5
#define SSD1309_NORMALDISPLAY 0xA6
#define SSD1309_INVERTDISPLAY 0xA7
#define SSD1309_DISPLAYOFF 0xAE
#define SSD1309_DISPLAYON 0xAF
#define SSD1309_NOP 0xE3

#define SSD1309_MEMORYMODE 0x20
#define SSD1309_COLUMNADDR 0x21
#define SSD1309_PAGEADDR 0x22

#define SSD1309_SEGREMAP 0xA1
#define SSD1309_SETMULTIPLEX 0xA8
#define SSD1309_COMSCANINC 0xC0
#define SSD1309_COMSCANDEC 0xC8
#define SSD1309_SETDISPLAYOFFSET 0xD3
#define SSD1309_SETDISPLAYCLOCKDIV 0xD5
#define SSD1309_SETPRECHARGE 0xD9
#define SSD1309_SETCOMPINS 0xDA
#define SSD1309_SETVCOMDETECT 0xDB
#define SSD1309_SETSTARTLINE 0x40
#define SSD1309_SETLOWCOLUMN    0x00
#define SSD1309_SETHIGHCOLUMN   0x10

#define SSD1309_RIGHT_HORIZONTAL_SCROLL 0x26
#define SSD1309_LEFT_HORIZONTAL_SCROLL 0x27
#define SSD1309_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29
#define SSD1309_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL 0x2A
#define SSD1309_DEACTIVATE_SCROLL 0x2E
#define SSD1309_ACTIVATE_SCROLL 0x2F
#define SSD1309_SET_VERTICAL_SCROLL_AREA 0xA3

// ---------------- API ----------------
void oled_init(uint8_t refresh);
void oled_update(void);
void oled_clear(void);
void oled_dat(uint8_t dat);
void oled_cmd(uint8_t cmd);

void oled_setCursor(uint8_t x, uint8_t y);
void oled_print(const char *str);
void oled_printChar(char c);

void oled_drawPixel(uint8_t x, uint8_t y, uint8_t color);
void oled_drawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t color);
void oled_drawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color);
void oled_fillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color);

void oled_drawBitmap(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *bitmap);

// icons
void oled_battery(uint8_t x, uint8_t y, uint8_t level); // 0–100%
void oled_iconEnvelope(uint8_t x, uint8_t y);
void oled_iconThermo(uint8_t x, uint8_t y);

#endif
