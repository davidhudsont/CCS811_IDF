

#ifndef _ESP32THING_SPI_PINS_H_
#define _ESP32THING_SPI_PINS_H_


#include <stdint.h>

#define SPI_MISO    19
#define SPI_MOSI    23
#define SPI_SCK     18
#define SPI_CS      2


void SPI_begin(int8_t sck, int8_t miso, int8_t mosi, int8_t cs);
void SPI_setBitOrder(uint8_t bitOrder);
void SPI_setDataMode(uint8_t dataMode);
void SPI_setFrequency(uint32_t freq);
void SPI_setClockDivider(uint32_t clockDiv);

void SPI_select();
void SPI_unselect();
uint8_t SPI_transfer(uint8_t data);
void SPI_write8(uint8_t data);
void SPI_writeBytes(uint8_t * data, uint32_t size);

#endif