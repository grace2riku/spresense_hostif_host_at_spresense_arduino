#include <Arduino.h>
#include <SPI.h>

void spi5_start(void) {
  /* Start the SPI library */
  SPI5.begin();

  /* Configure the SPI port */
  SPI5.beginTransaction(SPISettings(800000, MSBFIRST, SPI_MODE1));
}


void spi5_write_and_read(uint8_t* rw_buf, size_t count) {
  SPI5.transfer(rw_buf, count);
}
