#ifndef ICONS_H_
#define ICONS_H_

#include <stdint.h>

// Battery outline: 24x10
extern const uint8_t icon_battery_outline[];

// Envelope icon 16x12
extern const uint8_t icon_envelope[];

// Thermometer 5x7
extern const uint8_t icon_thermo[];

void oled_battery(uint8_t x, uint8_t y, uint8_t level);
void oled_iconEnvelope(uint8_t x, uint8_t y);
void oled_iconThermo(uint8_t x, uint8_t y);

#endif
