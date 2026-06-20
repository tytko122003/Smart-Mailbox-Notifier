#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

#include "SPI.h"
#include "SSD1309.h"
#include "font5x7.h"
#include "icons.h"


// ============================= OLED 128x64 =============================
#define OLED_WIDTH 128
#define OLED_HEIGHT 64

uint8_t oled_buffer[OLED_WIDTH * OLED_HEIGHT / 8];
uint8_t curX = 0;
uint8_t curY = 0;


// ============================= DEFINICJE PORTOW =============================
#define OLED_DDR		DDRC
#define OLED_DC_PORT	PORTC
#define OLED_DC_PIN		PC1

#define OLED_RST_PORT	PORTC
#define OLED_RST_PIN	PC0

#define OLED_CS_PORT	PORTC
#define OLED_CS_PIN		PC2


// ============================= MAKRODEFINICJE =============================
#define SetBit(port, pin) ( (port) |=  (1 << (pin)) )
#define ClrBit(port, pin) ( (port) &= ~(1 << (pin)) )

#define OLED_ENABLE		ClrBit(OLED_CS_PORT, OLED_CS_PIN)
#define OLED_DISABLE	SetBit(OLED_CS_PORT, OLED_CS_PIN)

#define OLED_COMMAND	ClrBit(OLED_DC_PORT, OLED_DC_PIN)
#define OLED_DATA		SetBit(OLED_DC_PORT, OLED_DC_PIN)

#define RESET_ON		ClrBit(OLED_RST_PORT, OLED_RST_PIN)
#define RESET_OFF		SetBit(OLED_RST_PORT, OLED_RST_PIN)



// -------------------------------- COMMAND SENDER --------------------------------
void oled_cmd(uint8_t cmd){
	OLED_ENABLE;
	OLED_COMMAND;
	SPI_Transmit(cmd);
	OLED_DISABLE;
}


// -------------------------------- COMMAND SENDER --------------------------------
void oled_dat(uint8_t dat){
	OLED_ENABLE;
	OLED_DATA;
	SPI_Transmit(dat);
	OLED_DISABLE;
}


// -------------------------------- RESET + INIT --------------------------------
void oled_init(uint8_t refresh){
	
	OLED_DDR |= 0x0F;
	
	//RESET_ON;
	//_delay_ms(50);
	//RESET_OFF;

	oled_cmd(SSD1309_DISPLAYOFF); // OFF
	oled_cmd(SSD1309_SETDISPLAYCLOCKDIV); oled_cmd(refresh);
	oled_cmd(SSD1309_SETDISPLAYOFFSET); oled_cmd(0x0);
	oled_cmd(SSD1309_SETSTARTLINE | 0x0);
	oled_cmd(SSD1309_MEMORYMODE); oled_cmd(0x00);
	oled_cmd(SSD1309_SETLOWCOLUMN); // horizontal addressing
	oled_cmd(SSD1309_SETMULTIPLEX); oled_cmd(0x3F);
	oled_cmd(SSD1309_SETCOMPINS); oled_cmd(0x12);
	oled_cmd(SSD1309_SEGREMAP);
	oled_cmd(SSD1309_COMSCANDEC);
	oled_cmd(SSD1309_SETCONTRAST); oled_cmd(0x7F);
	oled_cmd(SSD1309_SETVCOMDETECT); oled_cmd(0x35);
	oled_cmd(SSD1309_DISPLAYALLON_RESUME);
	oled_cmd(SSD1309_NORMALDISPLAY);
	oled_cmd(SSD1309_DISPLAYON);
}


// -------------------------------- CLEAR --------------------------------
void oled_clear(void){
	for(uint16_t i=0;i<sizeof(oled_buffer);i++)
	oled_buffer[i]=0x00;
}


// -------------------------------- UPDATE --------------------------------
void oled_update(void){
	for(uint8_t page=0; page<8; page++){
		oled_cmd(0xB0 + page);
		oled_cmd(0x00);
		oled_cmd(0x10);
		
		OLED_ENABLE;
		OLED_DATA;
		for(uint8_t col=0; col<OLED_WIDTH; col++){
			SPI_Transmit(oled_buffer[page*OLED_WIDTH + col]);
		}
		OLED_DISABLE;
	}
}


// -------------------------------- PIXEL --------------------------------
void oled_drawPixel(uint8_t x, uint8_t y, uint8_t color){
	if(x>=OLED_WIDTH || y>=OLED_HEIGHT) return;
	
	if(color) {
		oled_buffer[(y >> 3)*OLED_WIDTH + x] |=  (1<<(y & 7));
	}else{
		oled_buffer[(y >> 3)*OLED_WIDTH + x] &= ~(1<<(y & 7));
	}
}


// -------------------------------- LINE (Bresenham) --------------------------------
void oled_drawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t c){
	int dx = (x1>x0)? x1-x0 : x0-x1;
	int sx = (x0<x1)? 1 : -1;
	int dy = (y1>y0)? y0-y1 : y1-y0;
	int sy = (y0<y1)? 1 : -1;
	int err = dx + dy;

	while(1){
		oled_drawPixel(x0,y0,c);
		if(x0 == x1 && y0 == y1) break;
		int e2 = 2*err;
		if(e2 >= dy){ err += dy; x0 += sx; }
		if(e2 <= dx){ err += dx; y0 += sy; }
	}
}


// -------------------------------- PROSTOKAT --------------------------------
void oled_drawRect(uint8_t x,uint8_t y,uint8_t w,uint8_t h,uint8_t c){
	oled_drawLine(x,y, x+w-1,y, c);
	oled_drawLine(x,y+h-1, x+w-1,y+h-1, c);
	oled_drawLine(x,y, x,y+h-1, c);
	oled_drawLine(x+w-1,y, x+w-1,y+h-1, c);
}

void oled_fillRect(uint8_t x,uint8_t y,uint8_t w,uint8_t h,uint8_t c){
	for(uint8_t i=0;i<h;i++)
	oled_drawLine(x,y+i, x+w-1,y+i, c);
}

// -------------------------------- CURSOR --------------------------------
void oled_setCursor(uint8_t x, uint8_t y){
	curX = x;
	curY = y;
}


// -------------------------------- CHAR --------------------------------
void oled_printChar(char c){
	if(c < 32 || c > 126) c = '?';
	const uint8_t *f = font5x7[c-32];
	for(uint8_t i=0;i<5;i++){
		uint8_t col = f[i];
		for(uint8_t j=0;j<7;j++)
		oled_drawPixel(curX+i, curY+j, (col & (1<<j))?1:0);
	}
	curX += 6;
}


// -------------------------------- PRINT --------------------------------
void oled_print(const char* s){
	while(*s){
		if(*s=='\n'){ curY+=8; curX=0; s++; continue; }
		oled_printChar(*s++);
	}
}

// -------------------------------- BITMAP --------------------------------
void oled_drawBitmap(uint8_t x,uint8_t y,uint8_t w,uint8_t h,const uint8_t *bmp){
	for(uint8_t yy=0; yy<h; yy++){
		for(uint8_t xx=0; xx<w; xx++){
			uint8_t byte = bmp[(yy*(w>>3)) + (xx>>3)];
			if(byte & (0x80 >> (xx&7))){
				oled_drawPixel(x+xx, y+yy, SSD1309_WHITE);
			}else{
				oled_drawPixel(x+xx, y+yy, SSD1309_BLACK);
			}
		}
	}
}
