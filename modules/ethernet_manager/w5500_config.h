#ifndef W5500_CONFIG_H
#define W5500_CONFIG_H

#include "pico/stdlib.h"
#include "hardware/spi.h"

// Definição do tipo datasize_t usado pela ioLibrary
typedef int16_t datasize_t;

// Funções de interface SPI para W5500
void w5500_cs_select(void);
void w5500_cs_deselect(void);
uint8_t w5500_spi_readbyte(void);
void w5500_spi_writebyte(uint8_t wb);
void w5500_spi_readburst(uint8_t* pBuf, uint16_t len);
void w5500_spi_writeburst(uint8_t* pBuf, uint16_t len);

// Macros para controle do CS
#define WIZCHIP_CS_SELECT() w5500_cs_select()
#define WIZCHIP_CS_DESELECT() w5500_cs_deselect()

#endif // W5500_CONFIG_H
