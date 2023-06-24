#ifndef SPI5_WRAP_H
#define SPI5_WRAP_H
#include <Arduino.h>

void spi5_start(void);
void spi5_write_and_read(uint8_t* rw_buf, size_t count);

#endif