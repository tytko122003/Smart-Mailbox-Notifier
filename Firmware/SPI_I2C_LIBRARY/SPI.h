#ifndef SPI_H_
#define SPI_H_

#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>

/* ----- SPI MODES ----- */
typedef enum {
	SPI_MODE0 = 0,  // CPOL = 0, CPHA = 0
	SPI_MODE1,      // CPOL = 0, CPHA = 1
	SPI_MODE2,      // CPOL = 1, CPHA = 0
	SPI_MODE3       // CPOL = 1, CPHA = 1
} spi_mode_t;

/* ----- MASTER / SLAVE ----- */
typedef enum {
	SPI_MASTER = 0,
	SPI_SLAVE
} spi_role_t;


/* ----- CLOCK PRESCALER ----- */
typedef enum {
	SPI_DIV4 = 0,
	SPI_DIV16,
	SPI_DIV64,
	SPI_DIV128,
	SPI_DIV2,
	SPI_DIV8,
	SPI_DIV32
} spi_clock_t;


/* ----- API ----- */
void SPI_Init(spi_role_t role, spi_mode_t mode, spi_clock_t clock);

void SPI_Enable(void);
void SPI_Disable(void);

uint8_t SPI_Transmit(uint8_t data);
uint8_t SPI_Receive(void);
uint8_t SPI_TransmitReceive(uint8_t data);


#endif /* SPI_H_ */
