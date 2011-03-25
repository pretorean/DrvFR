/***************************************************************************
                          options.cpp  -  description
                             -------------------
    begin                : Fri Jan 11 2002
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

#include <string.h>
#include <stdlib.h>
#include "drvfr.h"
#include "options.h"

const char *LineSpeed[] =
{
  "2400",
  "4800",
  "9600",
  "19200",
  "38400",
  "57600",
  "115200"
};

/**********************************************************
 * Local functions & procedures                           *
 **********************************************************/

int  readrcfile(char*);
void parse(char*);
void setdrvconf(char*, char*);

/**********************************************************
 * Local static variables                                 *
 **********************************************************/

static int t_devnum    = 0;
static int t_passwd    = 0;
static int t_timeout   = 0;
static int t_linespeed = 0;
static fr_func *fr;

/**********************************************************
 * Implementation of procedures                           *
 **********************************************************/

int readrcfile(char *fname)
{
  int rcfile;
  ssize_t bytesread;
  char *filebuff;
  filebuff = (char*)malloc(MAX_LEN);

  if((rcfile = open(fname, O_RDONLY)) == -1)	return -1;
  if((bytesread = read(rcfile,filebuff,MAX_LEN)) > 0)	parse(filebuff);
  close(rcfile);
  free(filebuff);
  return 1;
}
//-----------------------------------------------------------------------------
void setdrvconf(char *param, char *val)
{
  int tmp;

  if(strcmp(param,"portnum") == 0)
  {
    if((tmp = strtol(val,NULL,10)) > 0) t_devnum = tmp;
    return;
  };
  if(strcmp(param,"passwd") == 0)
  {
    if((tmp = strtol(val,NULL,10)) > 0) t_passwd = tmp;
    return;
  };
  if(strcmp(param,"timeout") == 0)
  {
    if((tmp = strtol(val,NULL,10)) > 0 && tmp < 256) t_timeout = tmp;
    return;
  };
  if(strcmp(param,"boudrate") == 0)
  {
    for(int tmp=0; tmp<7; tmp++)
    {
      if(strcmp(val,LineSpeed[tmp]) == 0)
      {
        t_linespeed = tmp;
        break;
      };
    };
    return;
  };
}
//-----------------------------------------------------------------------------
void parse(char *str)
{
  char *param, *val, *word, *tmp, *tmp1;
  char delim[] = " .,;:!-\n";

  tmp = strdup(str);
  word = strtok(tmp,delim);
  do
  {
    tmp1  = strdup(word);
    param = strsep(&tmp1,"=");
    val   = strsep(&tmp1,"=");
    setdrvconf(param, val);
  } while ((word = strtok(NULL,delim)) != NULL);
}
//-----------------------------------------------------------------------------
int readoptions (void)
{
  char *homedir;
  int  etc_res;
  int  home_res;

  fr = drvfrInitialize();
  homedir  = getenv("HOME");
  etc_res  = readrcfile("/etc/drvfrrc");
  home_res = readrcfile(strcat(homedir,"/.drvfrrc"));
  if(etc_res == -1 && home_res == -1) return -1;
  fr->prop->ComPortNumber = t_devnum;
  fr->prop->Password = t_passwd;
  fr->prop->Timeout  = t_timeout;
  fr->prop->BaudRate = t_linespeed;
  return 1;
}
//-----------------------------------------------------------------------------
