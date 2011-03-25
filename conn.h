/***************************************************************************
                          conn.h  -  description
                             -------------------
    begin                : Thu Jan 10 2002
    copyright            : (C) 2002 by Igor V. Youdytsky
    email                : Pitcher@gw.tander.ru
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef _CONN_H
#define _CONN_H
#include <termios.h>
#include "drvfr.h"

#define MAX_TRIES	10

#define ENQ 			0x05
#define STX 			0x02
#define ACK 			0x06
#define NAK 			0x15


/**********************************************************
 *  The length of commands                                *
 **********************************************************/

const unsigned commlen[0x100] =
{
   0,  6,  5,  5,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 0x00 - 0x0f
   0,  5, 26,  5,  8,  6,  1, 46, 37,  6,  6,  6, 10,  5,255,  9, // 0x10 - 0x1f
   6,  8,  8,  8,  5,  6,  0,  5,  6,  7,  5,  5,  5,  6,  7,  0, // 0x20 - 0x2f
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 0x30 - 0x3f
   5,  5,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 0x40 - 0x4f
  10, 10,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 0x50 - 0x5f
   9,  1,  6,  5,  5, 20, 12, 10,  5,  6,  0,  0,  0,  0,  0,  0, // 0x60 - 0x6f
 255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 0x70 - 0x7f
  60, 60, 60, 60, 60, 71, 54, 54,  5,  5, 54, 54,  0,  0,  0,  0, // 0x80 - 0x8f
  61, 57, 52, 11, 12, 52,  7,  7,  7,  5, 13,  7,  0,  0,  7,  7, // 0x90 - 0x9f
  13, 11, 12, 10, 10,  8,  7,  5,  0,  0,  0,  0,  0,  0,  0,  0, // 0xa0 - 0xaf
   5,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 0xb0 - 0xbf
  46,  7, 10,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 0xc0 - 0xcf
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 0xd0 - 0xdf
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 0xe0 - 0xef
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,255,  0,  0  // 0xf0 - 0xff
};

enum { s2400 = 0, s4800, s9600, s19200, s38400, s57600, s115200};

enum { WAIT_READ = 0x01, WAIT_WRITE};

const speed_t LineSpeedVal[7] =
{ B2400, B4800, B9600, B19200, B38400, B57600, B115200};

typedef struct
{
  int len;
  unsigned char buff[260];
} command;

typedef struct
{
  int len;
  unsigned char buff[260];
} answer;

typedef struct
{
    int len;
    unsigned char buff[260];
} parameter;

int opendev(fr_prop*);
int closedev(void);
int readanswer(answer*);
int sendcommand(int, int, parameter*);
int checkstate(void);
int clearanswer(void);

#endif
