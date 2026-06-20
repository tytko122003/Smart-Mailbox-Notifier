#include "SPI.h"


void SPI_Init(spi_role_t role, spi_mode_t mode, spi_clock_t clock){
	// domyslnie wylaczone
	SPCR = 0;
	SPSR = 0;

	/* tryb - master/slave */
	if(role == SPI_MASTER){
		// ustaw pin jako master
		DDRB |= (1 << PB1) | (1 << PB2) | (1 << PB0); // SCK, MOSI, SS
		DDRB &= ~(1 << PB3); // MISO
		
		PORTB |= (1 << PB0);
		
		// wybor master
		SPCR |= (1 << MSTR);

		// ustaw prescaler
		switch(clock){
			case SPI_DIV4:   break;
			case SPI_DIV16:  SPCR |= (1 << SPR0); break;
			case SPI_DIV64:  SPCR |= (1 << SPR1); break;
			case SPI_DIV128: SPCR |= (1 << SPR1) | (1 << SPR0); break;
			case SPI_DIV2:   SPSR |= (1 << SPI2X); break;
			case SPI_DIV8:   SPCR |= (1 << SPR0); SPSR |= (1 << SPI2X); break;
			case SPI_DIV32:  SPCR |= (1 << SPR1); SPSR |= (1 << SPI2X); break;
		}
	// wybor slave
	}else {	
		DDRB |= (1 << PB3); // MISO
		DDRB &= ~((1 << PB2) | (1 << PB1) | (1 << PB0)); // SS, SCK, MOSI
	}

	// tryb SPI 
	switch(mode){
		case SPI_MODE0: break;
		case SPI_MODE1: SPCR |= (1 << CPHA); break;
		case SPI_MODE2: SPCR |= (1 << CPOL); break;
		case SPI_MODE3: SPCR |= (1 << CPOL) | (1 << CPHA); break;
	}

	// wlacz SPI
	SPCR |= (1 << SPE);
}

void SPI_Enable(void){
	SPCR |= (1 << SPE);
}

void SPI_Disable(void){
	SPCR &= ~(1 << SPE);
}


uint8_t SPI_Transmit(uint8_t data){
	SPDR = data;
	while(!(SPSR & (1 << SPIF)));
	return SPDR;
}

uint8_t SPI_Receive(void){
	while(!(SPSR & (1 << SPIF)));
	return SPDR;
}

uint8_t SPI_TransmitReceive(uint8_t data){
	SPDR = data;
	while(!(SPSR & (1 << SPIF)));
	return SPDR;
}
