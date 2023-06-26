#include "spi5_wrap.h"

/* ICMD defnitions */

#define ICMD_AVAILABLE_SIZE(n)  (0x10 + (n))
#define ICMD_FIXLEN_TRANS(n)    (0x80 + (n))
#define ICMD_VARLEN_TRANS(n)    (0xa0 + (n))

int hostif_get_bufsize(uint8_t bufid, size_t* p_buf_size) {
    uint8_t rw_buf[5] = {0xff, 0xff, 0xff, 0xff, 0xff};

    rw_buf[0] = ICMD_AVAILABLE_SIZE(bufid);

    spi5_write_and_read(rw_buf, sizeof(rw_buf));

    if (rw_buf[2] != 0) {
      return -1;
    }

    *p_buf_size = rw_buf[3] | (rw_buf[4] << 8);

    return 0;
}


int host_receive(int bufid, uint8_t* buffer, size_t len, bool lock) {
  size_t tx_size = len - 3;

  buffer[0] = ICMD_VARLEN_TRANS(bufid);
  buffer[1] = tx_size & 0xff;
  buffer[2] = ((tx_size >> 8) & 0x3f);
  if (lock) buffer[2] |= 0x40;

  spi5_write_and_read(buffer, len);

  if (buffer[2] != 0) {
    return -1;
  }

  return 0;
}