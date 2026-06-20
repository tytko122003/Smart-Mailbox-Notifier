#ifndef BME280_H_
#define BME280_H_

#include <avr/io.h>
#include <stdint.h>
#include "SPI.h"

// Pin CS (Chip Select) - PB6
#define BME_CS_PORT PORTB
#define BME_CS_DIR  DDRB
#define BME_CS_PIN  PB6

#define BME_CS_SELECT()   BME_CS_PORT &= ~(1 << BME_CS_PIN)
#define BME_CS_DESELECT() BME_CS_PORT |= (1 << BME_CS_PIN)

// Rejestry
#define BME280_REG_ID           0xD0
#define BME280_REG_RESET        0xE0
#define BME280_REG_CTRL_MEAS    0xF4
#define BME280_REG_CONFIG       0xF5
#define BME280_REG_PRESS_MSB    0xF7

// Struktura na parametry kalibracyjne (Tylko T i P)
typedef struct {
	uint16_t dig_T1; int16_t dig_T2; int16_t dig_T3;
	uint16_t dig_P1; int16_t dig_P2; int16_t dig_P3; int16_t dig_P4;
	int16_t  dig_P5; int16_t dig_P6; int16_t dig_P7; int16_t dig_P8; int16_t dig_P9;
} bme280_calib_t;

// Struktura na surowe dane (ADC)
typedef struct {
	int32_t temperature;
	uint32_t pressure;
} bme280_data_t;

// Struktura na przeliczone dane (float)
typedef struct {
	float temperature;
	float pressure;
} bme280_values_t;

extern bme280_calib_t cal;

// Konstruktory
void BME280_Init(void);
uint8_t BME280_ReadID(void);
void BME280_ReadFixed(bme280_values_t *results);

#endif