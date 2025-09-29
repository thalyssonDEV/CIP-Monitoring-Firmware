#include "w5500_config.h"
#include "wizchip_macros.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"

// Configuração dos pinos SPI para W5500
#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_MOSI 19
#define PIN_RST  20

// Implementação das funções de callback para SPI
void w5500_cs_select(void) {
    gpio_put(PIN_CS, 0);
}

void w5500_cs_deselect(void) {
    gpio_put(PIN_CS, 1);
}

uint8_t w5500_spi_readbyte(void) {
    uint8_t rb = 0;
    spi_read_blocking(SPI_PORT, 0, &rb, 1);
    return rb;
}

void w5500_spi_writebyte(uint8_t wb) {
    spi_write_blocking(SPI_PORT, &wb, 1);
}

void w5500_spi_readburst(uint8_t* pBuf, uint16_t len) {
    spi_read_blocking(SPI_PORT, 0, pBuf, len);
}

void w5500_spi_writeburst(uint8_t* pBuf, uint16_t len) {
    spi_write_blocking(SPI_PORT, pBuf, len);
}
