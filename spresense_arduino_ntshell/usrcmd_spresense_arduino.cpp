/**
 * @file usrcmd.c
 * @author CuBeatSystems
 * @author Shinichiro Nakamura
 * @copyright
 * ===============================================================
 * Natural Tiny Shell (NT-Shell) Version 0.3.1
 * ===============================================================
 * Copyright (c) 2010-2016 Shinichiro Nakamura
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include "ntopt.h"
#include "ntlibc.h"
#include "ntstdio.h"

#include <Arduino.h>
#include "host_if_spi.h""

#define uart_puts Serial.print

typedef int (*USRCMDFUNC)(int argc, char **argv);

static int usrcmd_ntopt_callback(int argc, char **argv, void *extobj);
static int usrcmd_help(int argc, char **argv);
static int usrcmd_info(int argc, char **argv);
static int usrcmd_mainboard_leds(int argc, char **argv);
static int usrcmd_hostif_get_bufsize(int argc, char **argv);

typedef struct {
    char *cmd;
    char *desc;
    USRCMDFUNC func;
} cmd_table_t;

static const cmd_table_t cmdlist[] = {
    { "help", "This is a description text string for help command.", usrcmd_help },
    { "info", "This is a description text string for info command.", usrcmd_info },
    { "led",  "led [led no = 0..3] [led turn on = on or off]\r\nex) led 0 on\r\n", usrcmd_mainboard_leds },
    { "hostif_get_bufsize",  "hostif_get_bufsize [buffer ID = 0..31]\r\nex) hostif_get_bufsize 0\r\n", usrcmd_hostif_get_bufsize },
};

enum {
  COMMAND_HELP,
  COMMAND_INFO,
  COMMAND_LED,
  COMMAND_HOSTIF_GET_BUFSIZE,
  COMMAND_MAX
};

int usrcmd_execute(const char *text)
{
    return ntopt_parse(text, usrcmd_ntopt_callback, 0);
}

static int usrcmd_ntopt_callback(int argc, char **argv, void *extobj)
{
    if (argc == 0) {
        return 0;
    }
    const cmd_table_t *p = &cmdlist[0];
    for (int i = 0; i < sizeof(cmdlist) / sizeof(cmdlist[0]); i++) {
        if (ntlibc_strcmp((const char *)argv[0], p->cmd) == 0) {
            return p->func(argc, argv);
        }
        p++;
    }
    uart_puts("Unknown command found.\r\n");
    return 0;
}

static int print_cmd_description(const cmd_table_t* msg)
{
  uart_puts(msg->cmd);
  uart_puts("\t:");
  uart_puts(msg->desc);
  uart_puts("\r\n");

  return 0;
}

static int usrcmd_help(int argc, char **argv)
{
    const cmd_table_t *p = &cmdlist[0];
    for (int i = 0; i < sizeof(cmdlist) / sizeof(cmdlist[0]); i++) {
        uart_puts(p->cmd);
        uart_puts("\t:");
        uart_puts(p->desc);
        uart_puts("\r\n");
        p++;
    }
    return 0;
}

static int usrcmd_info(int argc, char **argv)
{
    if (argc != 2) {
        uart_puts("info sys\r\n");
        uart_puts("info ver\r\n");
        return 0;
    }
    if (ntlibc_strcmp(argv[1], "sys") == 0) {
        uart_puts("NXP LPC824 Monitor\r\n");
        return 0;
    }
    if (ntlibc_strcmp(argv[1], "ver") == 0) {
        uart_puts("Version 0.0.0\r\n");
        return 0;
    }
    uart_puts("Unknown sub command found\r\n");
    return -1;
}

const uint8_t leds_list[] = {
  LED0, LED1, LED2, LED3
};
static int usrcmd_mainboard_leds(int argc, char **argv)
{
  uint8_t led = 0;
  uint8_t level = LOW;

  if (argc != 3) {
    print_cmd_description(&cmdlist[COMMAND_LED]);
    return -1;
  }

  led = atoi(argv[1]);
  if (!(0 <= led && led <= 3)) {
    uart_puts("Error: led no is 0..3.\r\n");
    return -1;
  }

  if (ntlibc_strcmp(argv[2], "on") != 0 && ntlibc_strcmp(argv[2], "off") != 0) {
    uart_puts("Error: led level is on or off.\r\n");
    return 1;
  }

  if (ntlibc_strcmp(argv[2], "on") == 0) {
    level = HIGH;    
  }
  digitalWrite(leds_list[led], level);

  return 0;
}

static int usrcmd_hostif_get_bufsize(int argc, char **argv)
{
  uint8_t bufid = 0;
  uint16_t buf_size;

  if (argc != 2) {
    print_cmd_description(&cmdlist[COMMAND_HOSTIF_GET_BUFSIZE]);
    return -1;
  }

  bufid = atoi(argv[1]);
  if (!(0 <= bufid && bufid <= 31)) {
    uart_puts("Error: Buffer ID is 0..31.\r\n");
    return -1;
  }

  if (hostif_get_bufsize(bufid, &buf_size) != 0) {
    uart_puts("Error: status.\r\n");
    return -1;
  }

  String strbufid = String(bufid, DEC);
  String strbufsize = String(buf_size, DEC);
  String stroutmsg = String("bufid[" + strbufid + "] size = " + strbufsize + "\r\n");
  uart_puts(stroutmsg);

  return 0;
}