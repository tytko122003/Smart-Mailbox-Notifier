#include "icons.h"
#include "SSD1309.h"

// --- BATT OUTLINE (24x10, 1bpp, row packed)
const uint8_t icon_battery_outline[] = {
	0xFF,0x81,0x81,0x81,0xFF,
	0x07,0x07
};

// --- ENVELOPE (16x12)
const uint8_t icon_envelope[] = {
	0xFF,0x81,0xBD,0xA5,0xA5,0xBD,0x81,0xFF,
	0x03,0x03
};

// --- THERMO (5x7)
const uint8_t icon_thermo[] = {
	0x0E,
	0x0A,
	0x0A,
	0x0E,
	0x0A,
	0x0A,
	0x0E
};


void oled_battery(uint8_t x, uint8_t y, uint8_t level){
	if(level>100) level=100;

	oled_drawRect(x, y, 22, 10, SSD1309_WHITE);
	oled_drawRect(x+22, y+3, 2, 4, SSD1309_WHITE);

	uint8_t fillWidth = (20 * level) / 100;
	if(fillWidth>0)
	oled_fillRect(x+1, y+1, fillWidth, 8, SSD1309_WHITE);
}

void oled_iconEnvelope(uint8_t x, uint8_t y){
	oled_drawBitmap(x,y,16,12,icon_envelope);
}

void oled_iconThermo(uint8_t x, uint8_t y){
	for(uint8_t i=0;i<7;i++){
		uint8_t col = icon_thermo[i];
		for(uint8_t j=0;j<5;j++){
			oled_drawPixel(x+j, y+i, (col & (1<<j))?1:0);
		}
	}
}
