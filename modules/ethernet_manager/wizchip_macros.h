#ifndef WIZCHIP_MACROS_H
#define WIZCHIP_MACROS_H

#include "w5500_config.h"

// WIZCHIP macro definitions for W5500
#define WIZCHIP_READ(AddrSel)                       w5500_spi_readbyte()
#define WIZCHIP_WRITE(AddrSel, wb)                  w5500_spi_writebyte(wb)
#define WIZCHIP_READ_BUF(AddrSel, pBuf, len)        w5500_spi_readburst(pBuf, len)
#define WIZCHIP_WRITE_BUF(AddrSel, pBuf, len)       w5500_spi_writeburst(pBuf, len)

#endif // WIZCHIP_MACROS_H
