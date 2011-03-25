/***************************************************************************
                          conn.cpp  -  description
                             -------------------
    begin                : Sat Jan 12 2002
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

#define _GNU_SOURCE

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include "conn.h"
#include "options.h"
#include "drvfr.h"

/**********************************************************
 * Local functions & procedures                           *
 **********************************************************/

int sendNAK(void);
int sendACK(void);
int sendENQ(void);
int determsp(int);
int set_up_tty (int);
int input_timeout(int);
int composecomm(command*, int, unsigned char*);
void settimeout(int);
char* devname(int number);
unsigned short int readbyte(int);
unsigned short int LRC(unsigned char*, int, int);

void PrintComm(command*);

/**********************************************************
 * Local static variables                                 *
 **********************************************************/

static int devfile, fdflags, connected;
static fd_set set;
static fr_prop *prop;
static struct timeval timeout;

/**********************************************************
 * Implementation of procedures                           *
 **********************************************************/
#ifdef DEBUG
void PrintComm(command* cmd)
{
  char *s;
  char *t;

  s = (char*)malloc(cmd->len*3*3);
  t = (char*)malloc(5);
  memset(s, 0, cmd->len*3*3);
  for(int i = 0; i < cmd->len; i++)
  {
    sprintf(t, "%02X|",(int)cmd->buff[i]);
    s = strcat(s,t);
  }
  s = strcat(s,"\n");
  perror(s);
};
#endif
//--------------------------------------------------------------------------------------
void settimeout(int msec)
{
  if(msec <= 1000)
  {
    timeout.tv_sec = 0;
    timeout.tv_usec = msec;
  }
  else
  {
    timeout.tv_sec = msec/1000;
    timeout.tv_usec = msec - timeout.tv_sec * 1000;
  };
}
//--------------------------------------------------------------------------------------
int opendev(fr_prop *f)
{
  prop = f;
  while ((devfile = open(devname(prop->ComPortNumber), O_NONBLOCK | O_RDWR, 0)) < 0)
  if (errno != EINTR) return -1;
  if ((fdflags = fcntl(devfile, F_GETFL)) == -1 || fcntl(devfile, F_SETFL, fdflags & ~O_NONBLOCK) < 0) return -1;
  set_up_tty(devfile);
  FD_ZERO (&set);
  FD_SET (devfile, &set);
  settimeout(prop->Timeout);
  return 1;
}
//--------------------------------------------------------------------------------------
int set_up_tty (int tty_fd)
{
  int speed;
  struct termios tios;

  if (tcgetattr(tty_fd, &tios) < 0) return -1;

  tios.c_cflag     &= ~(CSIZE | CSTOPB | PARENB | CLOCAL);
  tios.c_cflag     |= CS8 | CREAD | HUPCL;

  tios.c_iflag      = IGNBRK | IGNPAR;
  tios.c_oflag      = 0;
  tios.c_lflag      = 0;
  tios.c_cc[VMIN]   = 1;
  tios.c_cc[VTIME]  = 0;

  tios.c_cflag &= ~CRTSCTS;

  speed = LineSpeedVal[prop->BaudRate];
  if (speed)
    {
      cfsetospeed (&tios, speed);
      cfsetispeed (&tios, speed);
    }

  if (tcsetattr(tty_fd, TCSAFLUSH, &tios) < 0) return -1;

  return 1;
}
//--------------------------------------------------------------------------------------
int determsp(int spval)
{
  for(int i=0; i<7; i++) if((unsigned)spval == LineSpeedVal[i]) return i;
  return 0;
}
//--------------------------------------------------------------------------------------
int closedev(void)
{
  return close(devfile);
}
//--------------------------------------------------------------------------------------
int input_timeout(int msec)
{
  if(msec > 0 ) settimeout(msec);
  connected = TEMP_FAILURE_RETRY (select (FD_SETSIZE, &set, NULL, NULL, &timeout));
  return connected;
}
//--------------------------------------------------------------------------------------
unsigned short int LRC(unsigned char *str, int len, int offset = 0)
{
  int i;
  unsigned char *ptr;
  unsigned char ch = 0;

  ptr = str + offset;
  for(i=0; i<len; i++)ch ^= ptr[i];
  return ch;
}
//--------------------------------------------------------------------------------------
int sendNAK(void)
{
  char buff[2];
  buff[0] = NAK;
  return write(devfile,buff,1);
}
//--------------------------------------------------------------------------------------
int sendACK(void)
{
  char buff[2];
  buff[0] = ACK;
  return write(devfile,buff,1);
}
//--------------------------------------------------------------------------------------
int sendENQ(void)
{
  char buff[2];
  buff[0] = ENQ;
  return write(devfile,buff,1);
}
//--------------------------------------------------------------------------------------
int composecomm(command *cmd, int comm, int pass, parameter *param)
{
    int len;

    len = commlen[comm];
    if(len >= 5 && param->len > (len - 5)) param->len = len - 5;
    cmd->buff[0] = STX;
    cmd->buff[1] = len;
    cmd->buff[2] = comm;
    if(len >= 5)
      {
        memcpy(cmd->buff + 3, &pass, sizeof(int));
        if(param->len > 0) memcpy(cmd->buff + 7, param->buff, param->len);
      };
    cmd->buff[len + 2] = LRC(cmd->buff, len + 1, 1);
    cmd->len = len + 3;
    return 1;
}
//--------------------------------------------------------------------------------------
unsigned short int readbyte(int msec = prop->Timeout)
{
  unsigned char readbuff[2] = "";
  if(input_timeout(msec) == 0) return 0;
  if(read(devfile, readbuff, 1) > 0) return (unsigned int) readbuff[0];
  else return 0;
}
//--------------------------------------------------------------------------------------
int readbytes(unsigned char *buff, int len)
{
  int i;
  for(i = 0; i < len; i++)
    {
      if(input_timeout(prop->Timeout) == 1)
         if(read(devfile, buff+i, 1) == -1) return -1;
    };
  return len;
}
//--------------------------------------------------------------------------------------
int checkstate(void)
{
  short int repl;

  sendENQ();
  repl = readbyte(1000);
  if(connected == 0)return -1;
  switch(repl)
    {
      case NAK:
        return NAK;
      case ACK:
        return ACK;
      default:
        return -1;
    };
};
//--------------------------------------------------------------------------------------
int sendcommand(int comm, int pass, parameter *param)
{
  short int repl, tries;
  command cmd;
  composecomm(&cmd, comm, pass, param);
#ifdef DEBUG
  PrintComm(&cmd);
#endif
  for(tries = 1; tries <= MAX_TRIES; tries++)
    {
      if(write(devfile,cmd.buff,cmd.len) != -1)
        {
          repl = readbyte(prop->Timeout * 100);
          if(connected != 0)
            {
              if(repl == ACK) return 1;
            };
        };
    };
  return -1;
};
//--------------------------------------------------------------------------------------
int readanswer(answer *ans)
{
  short int  len, crc, tries, repl;
  for(tries = 0; tries < MAX_TRIES; tries++)
  {
    repl = readbyte(prop->Timeout * 100);
    if(connected == 1)
    {
      if(repl == STX)
      {
        len = readbyte();
        if(connected == 1)
        {
          if(readbytes(ans->buff, len) == len)
          {
            crc = readbyte();
            if(connected != 0)
            {
              if(crc == (LRC(ans->buff, len, 0) ^ len))
              {
                sendACK();
                ans->len = len;
                return len;
              }
            else sendNAK();
            };
          };
        };
      };
    };
    sendENQ();
    repl = readbyte(prop->Timeout * 2);
    if(connected != 1 || repl != ACK) tries++;
  };
  return -1;
}
//--------------------------------------------------------------------------------------
int clearanswer(void)
{
  short int len;
  unsigned char buf[0x100];

  sendENQ();
  len = readbytes(buf, 0x100);
  if(connected == 1 && len > 0)
  {
    if(buf[0] != NAK)
    {
      sendACK();
      return 1;
    };
  };
  return 0;
}
//--------------------------------------------------------------------------------------
char* devname(int number)
{
  static char *devn[5] =
  {
    "/dev/null",
    "/dev/ttyS0",
    "/dev/ttyS1",
    "/dev/ttyS2",
    "/dev/ttyS3"
  };
  if(number > 0 && number < 5) return devn[number];
  return devn[0];
}
//--------------------------------------------------------------------------------------
