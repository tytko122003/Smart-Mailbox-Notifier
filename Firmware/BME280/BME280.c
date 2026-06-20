#define F_CPU 16000000UL

#include "BME280.h"
#include <util/delay.h>

bme280_calib_t cal;
int32_t t_fine;

// ----------------- Funkcje pomocnicze -----------------
void BME280_WriteReg(uint8_t reg, uint8_t value) {
	reg &= ~(1 << 7);
	BME_CS_SELECT();
	SPI_TransmitReceive(reg);
	SPI_TransmitReceive(value);
	BME_CS_DESELECT();
}

uint8_t BME280_ReadReg(uint8_t reg) {
	uint8_t val;
	BME_CS_SELECT();
	SPI_TransmitReceive(reg | 0x80);
	_delay_us(20);
	val = SPI_TransmitReceive(0xFF);
	BME_CS_DESELECT();
	_delay_us(5);
	return val;
}
// ---------------------------------------------------

// Odczyt danych kalibracyjnych
static void BME280_ReadCalibration(void) {
	uint8_t tp[24];
	BME_CS_SELECT();
	SPI_TransmitReceive(0x88 | 0x80);
	for(int i = 0; i < 24; i++) tp[i] = SPI_TransmitReceive(0xFF);
	BME_CS_DESELECT();

	cal.dig_T1 = (uint16_t)((tp[1] << 8) | tp[0]);
	cal.dig_T2 = (int16_t)((tp[3] << 8) | tp[2]);
	cal.dig_T3 = (int16_t)((tp[5] << 8) | tp[4]);
	cal.dig_P1 = (uint16_t)((tp[7] << 8) | tp[6]);
	cal.dig_P2 = (int16_t)((tp[9] << 8) | tp[8]);
	cal.dig_P3 = (int16_t)((tp[11] << 8) | tp[10]);
	cal.dig_P4 = (int16_t)((tp[13] << 8) | tp[12]);
	cal.dig_P5 = (int16_t)((tp[15] << 8) | tp[14]);
	cal.dig_P6 = (int16_t)((tp[17] << 8) | tp[16]);
	cal.dig_P7 = (int16_t)((tp[19] << 8) | tp[18]);
	cal.dig_P8 = (int16_t)((tp[21] << 8) | tp[20]);
	cal.dig_P9 = (int16_t)((tp[23] << 8) | tp[22]);
}

// Kompensacja Temp z noty kat
static float BME280_CompensateT(int32_t adc_T) {
	float v1 = (((float)adc_T) / 16384.0 - ((float)cal.dig_T1) / 1024.0) * ((float)cal.dig_T2);
	float v2 = ((((float)adc_T) / 131072.0 - ((float)cal.dig_T1) / 8192.0) * (((float)adc_T) / 131072.0 - ((float)cal.dig_T1) / 8192.0)) * ((float)cal.dig_T3);
	t_fine = (int32_t)(v1 + v2);
	return (v1 + v2) / 5120.0;
}

// Kompensacja Cis z noty kat
static float BME280_CompensateP(int32_t adc_P) {
	float v1 = ((float)t_fine / 2.0) - 64000.0;
	float v2 = v1 * v1 * ((float)cal.dig_P6) / 32768.0;
	v2 = v2 + v1 * ((float)cal.dig_P5) * 2.0;
	v2 = (v2 / 4.0) + (((float)cal.dig_P4) * 65536.0);
	v1 = (((float)cal.dig_P3) * v1 * v1 / 524288.0 + ((float)cal.dig_P2) * v1) / 524288.0;
	v1 = (1.0 + v1 / 32768.0) * ((float)cal.dig_P1);
	if (v1 == 0.0) return 0;
	float p = 1048576.0 - (float)adc_P;
	p = (p - (v2 / 4096.0)) * 6250.0 / v1;
	v1 = ((float)cal.dig_P9) * p * p / 2147483648.0;
	v2 = p * ((float)cal.dig_P8) / 32768.0;
	return (p + (v1 + v2 + ((float)cal.dig_P7)) / 16.0) / 100.0;
}

void BME280_Init(void) {
	BME_CS_DIR |= (1 << BME_CS_PIN);
	BME_CS_DESELECT();
	_delay_ms(50);
	BME280_WriteReg(0xE0, 0xB6);
	_delay_ms(150);
	BME280_ReadCalibration();
	// Konfiguracja tylko dla Temp i Cis (tryb Normal)
	BME280_WriteReg(0xF4, 0x27);
	_delay_ms(50);
}

uint8_t BME280_ReadID(void) {
	return BME280_ReadReg(BME280_REG_ID);
}

void BME280_ReadFixed(bme280_values_t *results) {
	uint8_t buffer[6];
	BME_CS_SELECT();
	SPI_TransmitReceive(BME280_REG_PRESS_MSB | (1 << 7));
	for(uint8_t i = 0; i < 6; i++) buffer[i] = SPI_TransmitReceive(0xFF);
	BME_CS_DESELECT();

	int32_t adc_P = ((uint32_t)buffer[0] << 12) | ((uint32_t)buffer[1] << 4) | (buffer[2] >> 4);
	int32_t adc_T = ((uint32_t)buffer[3] << 12) | ((uint32_t)buffer[4] << 4) | (buffer[5] >> 4);
	
	results->temperature = BME280_CompensateT(adc_T) - 3; // Korekta temp
	results->pressure    = BME280_CompensateP(adc_P);
}