/***************************************************************************
                          interface.cpp  -  description
                             -------------------
    begin                : Tue Jan 15 2002
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
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "drvfr.h"
#include "interface.h"
#include "options.h"
#include "conn.h"
#include "errdefs.h"

/**********************************************************
 * Local functions & procedures                           *
 **********************************************************/

int     errhand(void);
int     evalint(unsigned char*, int);
int64_t evalint64(unsigned char*, int);
void    evaldate(unsigned char*, struct tm*);
void    DefineECRModeDescription(void);

/**********************************************************
 * Local static variables                                 *
 **********************************************************/

static int initialized;
static int connected;
static fr_prop fr;
static fr_func fn =
{
  Connect,
  DisConnect,
  Beep,
  Buy,
  CancelCheck,
  CashIncome,
  CashOutcome,
  Charge,
  CheckSubTotal,
  CloseCheck,
  ConfirmDate,
  ContinuePrinting,
  Correction,
  CutCheck,
  DampRequest,
  Discount,
  DozeOilCheck,
  Draw,
  EjectSlipDocument,
  EKLZDepartmentReportInDatesRange,
  EKLZDepartmentReportInSessionsRange,
  EKLZJournalOnSessionNumber,
  EKLZSessionReportInDatesRange,
  EKLZSessionReportInSessionRange,
  FeedDocument,
  Fiscalization,
  FiscalReportForDatesRange,
  FiscalReportForSessionRange,
  GetData,
  GetDeviceMetrics,
  GetExchangeParam,
  GetFieldStruct,
  GetFiscalizationParameters,
  GetFMRecordsSum,
  GetECRStatus,
  GetLastFMRecordDate,
  GetLiterSumCounter,
  GetCashReg,
  GetOperationReg,
  GetRangeDatesAndSessions,
  GetRKStatus,
  GetTableStruct,
  InitFM,
  InitTable,
  InterruptDataStream,
  InterruptFullReport,
  InterruptTest,
  LaunchRK,
  LoadLineData,
  OilSale,
  OpenDrawer,
  PrintBarCode,
  PrintDocumentTitle,
  PrintOperationReg,
  PrintReportWithCleaning,
  PrintReportWithoutCleaning,
  PrintString,
  PrintWideString,
  ReadEKLZDocumentOnKPK,
  ReadEKLZSessionTotal,
  ReadLicense,
  ReadTable,
  ResetAllTRK,
  ResetRK,
  ResetSettings,
  ResetSummary,
  ReturnBuy,
  ReturnSale,
  Sale,
  SetDate,
  SetDozeInMilliliters,
  SetDozeInMoney,
  SetExchangeParam,
  SetPointPosition,
  SetRKParameters,
  SetSerialNumber,
  SetTime,
  StopEKLZDocumentPrinting,
  StopRK,
  Storno,
  StornoCharge,
  StornoDiscount,
  SummOilCheck,
  Test,
  WriteLicense,
  WriteTable,
  &fr
};
//-----------------------------------------------------------------------------
fr_func *drvfrInitialize(void)
{
  if(!initialized)
  {
    fr.SerialNumber   = (char*)malloc(13);
    fr.License        = (char*)malloc(13);
    fr.RNM            = (char*)malloc(13);
    fr.INN            = (char*)malloc(13);
    fr.ECRSoftVersion = (char*)malloc(4);
    fr.FMSoftVersion  = (char*)malloc(4);
    fr.DataBlock      = (unsigned char*)malloc(32);
    fr.ValueOfFieldString = (char*)malloc(41);
    fr.FieldName      = (char*)malloc(41);
    fr.LineData       = (unsigned char*)malloc(41);
    fr.ResultCodeDescription = "";
    initialized = true;
    if(readoptions() == -1) perror("Failed reading drvfrrc file");
  };
  return &fn;
}
//-----------------------------------------------------------------------------
int errhand(answer *a)
{
  fr.ResultCode = a->buff[1];
  fr.ResultCodeDescription = errmsg[fr.ResultCode];
  return fr.ResultCode;
}
//-----------------------------------------------------------------------------
int evalint(unsigned char *str, int len)
{
 int result = 0;
 while(len--)
 {
   result <<= 8;
   result += str[len];
 };
 return result;
}
//-----------------------------------------------------------------------------
int64_t evalint64(unsigned char *str, int len)
{
 int64_t result = 0;
 while(len--)
 {
   result <<= 8;
   result += str[len];
 };
 return result;
}
//-----------------------------------------------------------------------------
void evaldate(unsigned char *str, struct tm *date)
{
  date->tm_mday = evalint(str    , 1);
  date->tm_mon  = evalint(str + 1, 1) - 1;
  date->tm_year = evalint(str + 2, 1) + 100;
  mktime(date);
}
//-----------------------------------------------------------------------------
void evaltime(unsigned char *str, struct tm *time)
{
  time->tm_hour = evalint(str    , 1);
  time->tm_min  = evalint(str + 1, 1);
  time->tm_sec  = evalint(str + 2, 1);
  mktime(time);
}
//-----------------------------------------------------------------------------
void DefineECRModeDescription(void)
{
  fr.ECRMode8Status = fr.ECRMode >> 4;
  if((fr.ECRMode & 8) == 8)
  {
    fr.ECRModeDescription = ecrmode8desc[fr.ECRMode8Status];
    return;
  };
  fr.ECRModeDescription = ecrmodedesc[fr.ECRMode];
};

/**********************************************************
 *       Implementation of the interface functions        *
 **********************************************************/
int Connect(void)
{
  int tries = 0;
  int state = 0;
  answer      a;

  if(connected) closedev();

  if(opendev(&fr) == -1)
  {
    connected = false;
    return -1;
  };
  while(tries < MAX_TRIES)
  {
    state = checkstate();
    switch(state)
    {
      case NAK:
        connected = true;
        return 1;
      case ACK:
        readanswer(&a);
        connected = true;
        return 1;
      case -1:
        tries++;
      };
  };
  connected = false;
  return -1;
}
//-----------------------------------------------------------------------------
int DisConnect(void)
{
  if(initialized)
  {
    free(fr.SerialNumber);
    free(fr.License);
    free(fr.RNM);
    free(fr.INN);
    free(fr.ECRSoftVersion);
    free(fr.FMSoftVersion);
    free(fr.DataBlock);
    free(fr.ValueOfFieldString);
    free(fr.FieldName);
    free(fr.LineData);
    if(fr.UDescription) free(fr.UDescription);
  };
  initialized = false;
  connected = false;
  return closedev();
}
//-----------------------------------------------------------------------------
int Beep(void)
{
  parameter  p;
  answer     a;

  if(!connected) return -1;

  if(sendcommand(BEEP, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != BEEP) return -1;

  if(errhand(&a) != 0) return fr.ResultCode;

  fr.OperatorNumber = a.buff[2];

  return 0;
}
//-----------------------------------------------------------------------------
int CutCheck(void)
{
  parameter  p;
  answer     a;
  p.len    = 1;

  if(!connected) return -1;

  p.buff[0] = fr.CutType;

  if(sendcommand(CUT_CHECK, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != CUT_CHECK) return -1;

  if(errhand(&a) != 0) return fr.ResultCode;

  fr.OperatorNumber = a.buff[2];

  return 0;
}
//-----------------------------------------------------------------------------
int PrintString(void)
{
  parameter  p;
  answer     a;
  p.len	  = 41;

  if(!connected) return -1;

  p.buff[0]  = (fr.UseJournalRibbon == true) ? 1 : 0;
  p.buff[0] |= (fr.UseReceiptRibbon == true) ? 2 : 0;

  strncpy((char*)&p.buff+1, (char*)fr.StringForPrinting, 40);

  if(sendcommand(PRINT_STRING, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != PRINT_STRING) return -1;

  if(errhand(&a) != 0) return  fr.ResultCode;;

  fr.OperatorNumber = a.buff[2];

  return 0;
}
//-----------------------------------------------------------------------------
int PrintWideString(void)
{
  parameter  p;
  answer     a;
  p.len	  = 21;

  if(!connected) return -1;

  p.buff[0]  = (fr.UseJournalRibbon == true) ? 1 : 0;
  p.buff[0] |= (fr.UseReceiptRibbon == true) ? 2 : 0;

  strncpy((char*)&p.buff+1, (char*)fr.StringForPrinting, 20);

  if(sendcommand(PRINT_WIDE_STRING, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != PRINT_WIDE_STRING) return -1;

  if(errhand(&a) != 0) return  fr.ResultCode;;

  fr.OperatorNumber = a.buff[2];

  return 0;
}
//-----------------------------------------------------------------------------
int FeedDocument(void)
{
  parameter  p;
  answer     a;
  p.len    = 2;

  if(!connected) return -1;

  p.buff[0]  = (fr.UseJournalRibbon == true) ? 1 : 0;
  p.buff[0] |= (fr.UseReceiptRibbon == true) ? 2 : 0;
  p.buff[0] |= (fr.UseSlipDocument  == true) ? 4 : 0;

  p.buff[1] = fr.StringQuantity;

  if(sendcommand(FEED_DOCUMENT, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != FEED_DOCUMENT) return -1;

  if(errhand(&a) != 0) return  fr.ResultCode;;

  fr.OperatorNumber = a.buff[2];

  return 0;
}
//-----------------------------------------------------------------------------
int SetExchangeParam(void)
{
  parameter  p;
  answer     a;
  p.len    = 3;

  if(!connected) return -1;

  p.buff[0] = fr.PortNumber;
  p.buff[1] = fr.BaudRate;
  p.buff[2] = fr.Timeout;

  if(sendcommand(SET_EXCHANGE_PARAM, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != SET_EXCHANGE_PARAM) return -1;

  return errhand(&a);
}
//-----------------------------------------------------------------------------
int GetExchangeParam(void)
{
  parameter  p;
  answer     a;
  p.len    = 1;

  if(!connected) return -1;

  p.buff[0] = fr.PortNumber;

  if(sendcommand(GET_EXCHANGE_PARAM, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != GET_EXCHANGE_PARAM) return -1;

  if(errhand(&a) != 0) return fr.ResultCode;

  fr.BaudRate = a.buff[2];
  fr.Timeout  = a.buff[3];
  return 0;
}
//-----------------------------------------------------------------------------
int GetECRStatus(void)
{
  parameter  p;
  answer     a;

  if(!connected) return -1;

  if(sendcommand(GET_ECR_STATUS, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != GET_ECR_STATUS) return -1;
  if(errhand(&a))	return  fr.ResultCode;

  fr.OperatorNumber = a.buff[2];

  fr.ECRSoftVersion[0] = a.buff[3];
  fr.ECRSoftVersion[1] = 0x2e;
  fr.ECRSoftVersion[2] = a.buff[4];
  fr.ECRSoftVersion[3] = 0;

  fr.ECRBuild = evalint((unsigned char*)&a.buff + 5, 2);
  evaldate((unsigned char*)&a.buff + 7, &fr.ECRSoftDate);
  fr.LogicalNumber = evalint((unsigned char*)&a.buff + 10, 1);
  fr.OpenDocumentNumber = evalint((unsigned char*)&a.buff + 11, 2);
  fr.ReceiptRibbonIsPresent     = (a.buff[13] &   1)  ==   1; //0
  fr.JournalRibbonIsPresent     = (a.buff[13] &   2)  ==   2; //1
  fr.SlipDocumentIsPresent      = (a.buff[13] &   4)  ==   4; //2
  fr.SlipDocumentIsMoving       = (a.buff[13] &   8)  ==   8; //3
  fr.PointPosition              = (a.buff[13] &  16)  ==  16; //4
  fr.EKLZIsPresent              = (a.buff[13] &  32)  ==  32; //5
  fr.JournalRibbonOpticalSensor = (a.buff[13] &  64)  ==  64; //6
  fr.ReceiptRibbonOpticalSensor = (a.buff[13] & 128)  == 128; //6

  fr.JournalRibbonLever         = (a.buff[14] &   1)  ==   1; //0
  fr.ReceiptRibbonLever         = (a.buff[14] &   2)  ==   2; //1
  fr.LidPositionSensor          = (a.buff[14] &   4)  ==   4; //2

  fr.ECRMode  = evalint((unsigned char*)&a.buff + 15, 1);
  DefineECRModeDescription();

  fr.ECRAdvancedMode = evalint((unsigned char*)&a.buff + 16, 1);
  fr.ECRAdvancedModeDescription = ecrsubmodedesc[fr.ECRAdvancedMode];

  fr.PortNumber  = evalint((unsigned char*)&a.buff + 17, 1);
  fr.FMSoftVersion[0] = a.buff[18];
  fr.FMSoftVersion[1] = 0x2e;
  fr.FMSoftVersion[2] = a.buff[19];
  fr.FMSoftVersion[3] = 0;
  fr.FMBuild = evalint((unsigned char*)&a.buff + 20, 2);
  evaldate((unsigned char*)&a.buff + 22, &fr.FMSoftDate);
  evaldate((unsigned char*)&a.buff + 25, &fr.Date);
  evaltime((unsigned char*)&a.buff + 28, &fr.Time);
  fr.FM1IsPresent = (a.buff[31] & 1) == 1;
  fr.FM2IsPresent = (a.buff[31] & 2) == 2;
  fr.LicenseIsPresent = (a.buff[31] & 4) == 4;
  fr.FMOverflow = (a.buff[31] & 8) == 8;
  fr.BatteryCondition = (a.buff[31] & 16) == 16;
  sprintf(fr.SerialNumber, "%d", evalint((unsigned char*)&a.buff + 32, 4));
  fr.SessionNumber = evalint((unsigned char*)&a.buff + 36, 2);
  fr.FreeRecordInFM = evalint((unsigned char*)&a.buff + 38, 2);
  fr.RegistrationNumber = evalint((unsigned char*)&a.buff + 40, 1);
  fr.FreeRegistration = evalint((unsigned char*)&a.buff + 41, 1);
  sprintf(fr.INN, "%.0lg", (double)evalint64((unsigned char*)&a.buff + 42, 6));

  return 0;
}
//-----------------------------------------------------------------------------
int GetDeviceMetrics(void)
{
  int len  = 0;
  parameter  p;
  answer     a;
  p.len    = 1;

  if(!connected) return -1;

  if(sendcommand(GET_DEVICE_METRICS, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != GET_DEVICE_METRICS) return -1;
  if(errhand(&a)) return  fr.ResultCode;
  len = a.len - 7;
  fr.UMajorType            = evalint((unsigned char*)&a.buff + 2, 1);
  fr.UMinorType            = evalint((unsigned char*)&a.buff + 3, 1);
  fr.UMajorProtocolVersion = evalint((unsigned char*)&a.buff + 4, 1);
  fr.UMinorProtocolVersion = evalint((unsigned char*)&a.buff + 5, 1);
  fr.UModel                = evalint((unsigned char*)&a.buff + 6, 1);
  fr.UCodePage             = evalint((unsigned char*)&a.buff + 7, 1);
  fr.UDescription          = (char*)malloc(len + 1);
  strncpy(fr.UDescription, (char*)a.buff + 8, len);

  return 0;
}
//-----------------------------------------------------------------------------
int Test(void)
{
  parameter  p;
  answer     a;
  p.len    = 1;

  if(!connected) return -1;

  p.buff[0] = fr.RunningPeriod;

  if(sendcommand(TEST, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != TEST) return -1;

  if(errhand(&a) != 0) return fr.ResultCode;

  fr.OperatorNumber = a.buff[2];

  return 0;
}
//-----------------------------------------------------------------------------
int InterruptTest(void)
{
  parameter  p;
  answer     a;

  if(!connected) return -1;

  if(sendcommand(INTERRUPT_TEST, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != INTERRUPT_TEST) return -1;

  if(errhand(&a) != 0) return fr.ResultCode;

  fr.OperatorNumber = a.buff[2];

  return 0;
}
//-----------------------------------------------------------------------------
int ContinuePrinting(void)
{
  parameter  p;
  answer     a;

  if(!connected) return -1;

  if(sendcommand(CONTINUE_PRINTING, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != CONTINUE_PRINTING) return -1;

  if(errhand(&a) != 0) return fr.ResultCode;

  fr.OperatorNumber = a.buff[2];

  return 0;
}
//-----------------------------------------------------------------------------
int OpenDrawer(void)
{
  parameter  p;
  answer     a;
  p.len    = 1;

  if(!connected) return -1;

  p.buff[0] = fr.DrawerNumber;

  if(sendcommand(OPEN_DRAWER, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != OPEN_DRAWER) return -1;

  if(errhand(&a) != 0) return fr.ResultCode;

  fr.OperatorNumber = a.buff[2];

  return 0;
}
//-----------------------------------------------------------------------------
int PrintDocumentTitle(void)
{
  parameter  p;
  answer     a;
  p.len	  = 32;

  if(!connected) return -1;

  strncpy((char*)p.buff, (char*)fr.DocumentName, 30);
  memcpy(&p.buff+30, &fr.DocumentNumber, 2);

  if(sendcommand(PRINT_DOCUMENT_TITLE, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != PRINT_DOCUMENT_TITLE) return -1;

  if(errhand(&a) != 0) return  fr.ResultCode;;

  fr.OperatorNumber = a.buff[2];

  return 0;
}
//-----------------------------------------------------------------------------
int ResetSettings(void)
{
  parameter  p;

  if(!connected) return -1;

  if(sendcommand(RESET_SETTINGS, fr.Password, &p) < 0) return -1;

  return 0;
}
//-----------------------------------------------------------------------------
int ResetSummary(void)
{
  parameter  p;
  answer     a;

  if(!connected) return -1;

  if(sendcommand(RESET_SUMMARY, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != RESET_SUMMARY) return -1;

  return errhand(&a);
}
//-----------------------------------------------------------------------------
int Buy(void)
{
  parameter  p;
  answer     a;
  p.len   = 55;

  if(!connected) return -1;

  int64_t quant = llround(fr.Quantity * 1000);
  int64_t price = llround(fr.Price * 100);

  memcpy(p.buff,    &quant, 5);
  memcpy(p.buff+5,  &price, 5);
  p.buff[10] = fr.Department;

  p.buff[11] = fr.Tax1;
  p.buff[12] = fr.Tax2;
  p.buff[13] = fr.Tax3;
  p.buff[14] = fr.Tax4;

  strncpy((char*)p.buff+15, (char*)fr.StringForPrinting, 40);

  if(sendcommand(BUY, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != BUY) return -1;

  if(errhand(&a) != 0) return fr.ResultCode;

  fr.OperatorNumber = a.buff[2];

  return 0;
}
//-----------------------------------------------------------------------------
int ReturnBuy(void)
{
  parameter  p;
  answer     a;
  p.len   = 55;

  if(!connected) return -1;

  int64_t quant = llround(fr.Quantity * 1000);
  int64_t price = llround(fr.Price * 100);

  memcpy(p.buff,    &quant, 5);
  memcpy(p.buff+5,  &price, 5);
  p.buff[10] = fr.Department;

  p.buff[11] = fr.Tax1;
  p.buff[12] = fr.Tax2;
  p.buff[13] = fr.Tax3;
  p.buff[14] = fr.Tax4;

  strncpy((char*)p.buff+15, (char*)fr.StringForPrinting, 40);

  if(sendcommand(RETURN_BUY, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != RETURN_BUY) return -1;

  if(errhand(&a) != 0) return fr.ResultCode;

  fr.OperatorNumber = a.buff[2];

  return 0;
}
//-----------------------------------------------------------------------------
int Sale(void)
{
  parameter  p;
  answer     a;
  p.len   = 55;

  if(!connected) return -1;

  int64_t quant = llround(fr.Quantity * 1000);
  int64_t price = llround(fr.Price * 100);

  memcpy(p.buff,    &quant, 5);
  memcpy(p.buff+5,  &price, 5);
  p.buff[10] = fr.Department;

  p.buff[11] = fr.Tax1;
  p.buff[12] = fr.Tax2;
  p.buff[13] = fr.Tax3;
  p.buff[14] = fr.Tax4;

  strncpy((char*)p.buff+15, (char*)fr.StringForPrinting, 40);

  if(sendcommand(SALE, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != SALE) return -1;

  if(errhand(&a) != 0) return fr.ResultCode;

  fr.OperatorNumber = a.buff[2];

  return 0;
}
//-----------------------------------------------------------------------------
int ReturnSale(void)
{
  parameter  p;
  answer     a;
  p.len   = 55;

  if(!connected) return -1;

  int64_t quant = llround(fr.Quantity * 1000);
  int64_t price = llround(fr.Price * 100);

  memcpy(p.buff,    &quant, 5);
  memcpy(p.buff+5,  &price, 5);
  p.buff[10] = fr.Department;

  p.buff[11] = fr.Tax1;
  p.buff[12] = fr.Tax2;
  p.buff[13] = fr.Tax3;
  p.buff[14] = fr.Tax4;

  strncpy((char*)p.buff+15, (char*)fr.StringForPrinting, 40);

  if(sendcommand(RETURN_SALE, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != RETURN_SALE) return -1;

  if(errhand(&a) != 0) return fr.ResultCode;

  fr.OperatorNumber = a.buff[2];

  return 0;
}
//-----------------------------------------------------------------------------
int CancelCheck(void)
{
  parameter  p;
  answer     a;

  if(!connected) return -1;

  if(sendcommand(CANCEL_CHECK, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != CANCEL_CHECK) return -1;

  if(errhand(&a) != 0) return fr.ResultCode;

  fr.OperatorNumber = a.buff[2];

  return 0;
}
//-----------------------------------------------------------------------------
int CashIncome(void)
{
  parameter  p;
  answer     a;
  p.len    = 5;

  if(!connected) return -1;

  int64_t  sum = llround(fr.Summ1 * 100);

  memcpy(p.buff, &sum, 5);

  if(sendcommand(CASH_INCOME, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != CASH_INCOME) return -1;

  if(errhand(&a) != 0) return fr.ResultCode;

  fr.OperatorNumber = a.buff[2];
  fr.OpenDocumentNumber = evalint((unsigned char*)&a.buff+3, 2);

  return 0;
}
//-----------------------------------------------------------------------------
int CashOutcome(void)
{
  parameter  p;
  answer     a;
  p.len    = 5;

  if(!connected) return -1;

  int64_t  sum = llround(fr.Summ1 * 100);

  memcpy(p.buff, &sum, 5);

  if(sendcommand(CASH_OUTCOME, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != CASH_OUTCOME) return -1;

  if(errhand(&a) != 0) return fr.ResultCode;

  fr.OperatorNumber = a.buff[2];
  fr.OpenDocumentNumber = evalint((unsigned char*)&a.buff+3, 2);

  return 0;
}
//-----------------------------------------------------------------------------
int Charge(void)
{
  parameter  p;
  answer     a;
  p.len   = 49;

  if(!connected) return -1;

  int64_t  sum = llround(fr.Summ1 * 100);

  memcpy(p.buff, &sum, 5);

  p.buff[5] = fr.Tax1;
  p.buff[6] = fr.Tax2;
  p.buff[7] = fr.Tax3;
  p.buff[8] = fr.Tax4;

  strncpy((char*)p.buff+9, (char*)fr.StringForPrinting, 40);

  if(sendcommand(CHARGE, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != CHARGE) return -1;

  if(errhand(&a) != 0) return fr.ResultCode;

  fr.OperatorNumber = a.buff[2];

  return 0;
}
//-----------------------------------------------------------------------------
int StornoCharge(void)
{
  parameter  p;
  answer     a;
  p.len   = 49;

  if(!connected) return -1;

  int64_t  sum = llround(fr.Summ1 * 100);

  memcpy(p.buff, &sum, 5);

  p.buff[5] = fr.Tax1;
  p.buff[6] = fr.Tax2;
  p.buff[7] = fr.Tax3;
  p.buff[8] = fr.Tax4;

  strncpy((char*)p.buff+9, (char*)fr.StringForPrinting, 40);

  if(sendcommand(STORNO_CHARGE, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != STORNO_CHARGE) return -1;

  if(errhand(&a) != 0) return fr.ResultCode;

  fr.OperatorNumber = a.buff[2];

  return 0;
}
//-----------------------------------------------------------------------------
int Discount(void)
{
  parameter  p;
  answer     a;
  p.len   = 49;

  if(!connected) return -1;

  int64_t  sum = llround(fr.Summ1 * 100);

  memcpy(p.buff, &sum, 5);

  p.buff[5] = fr.Tax1;
  p.buff[6] = fr.Tax2;
  p.buff[7] = fr.Tax3;
  p.buff[8] = fr.Tax4;

  strncpy((char*)p.buff+9, (char*)fr.StringForPrinting, 40);

  if(sendcommand(DISCOUNT, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != DISCOUNT) return -1;

  if(errhand(&a) != 0) return fr.ResultCode;

  fr.OperatorNumber = a.buff[2];

  return 0;
}
//-----------------------------------------------------------------------------
int StornoDiscount(void)
{
  parameter  p;
  answer     a;
  p.len   = 49;

  if(!connected) return -1;

  int64_t sum = llround(fr.Summ1 * 100);

  memcpy(p.buff, &sum, 5);

  p.buff[5] = fr.Tax1;
  p.buff[6] = fr.Tax2;
  p.buff[7] = fr.Tax3;
  p.buff[8] = fr.Tax4;

  strncpy((char*)p.buff+9, (char*)fr.StringForPrinting, 40);

  if(sendcommand(STORNO_DISCOUNT, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != STORNO_DISCOUNT) return -1;

  if(errhand(&a) != 0) return fr.ResultCode;

  fr.OperatorNumber = a.buff[2];

  return 0;
}
//-----------------------------------------------------------------------------
int CheckSubTotal(void)
{
  parameter  p;
  answer     a;

  if(!connected) return -1;

  if(sendcommand(CHECK_SUBTOTAL, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != CHECK_SUBTOTAL) return -1;

  if(errhand(&a) != 0) return fr.ResultCode;

  fr.OperatorNumber = a.buff[2];
  fr.Summ1 = evalint64((unsigned char*)&a.buff+3, 5);
  fr.Summ1 /= 100;

  return 0;
}
//-----------------------------------------------------------------------------
int CloseCheck(void)
{
  parameter  p;
  answer     a;

  if(!connected) return -1;

  int64_t  sum;
  p.len   = 67;

  sum = llround(fr.Summ1 * 100);
  memcpy(p.buff,    &sum, 5);			// 0-4
  sum = llround(fr.Summ2 * 100);
  memcpy(p.buff+ 5, &sum, 5);			// 5-9
  sum = llround(fr.Summ3 * 100);
  memcpy(p.buff+10, &sum, 5);			//10-14
  sum = llround(fr.Summ4 * 100);
  memcpy(p.buff+15, &sum, 5);			//15-19

  sum = llround(fr.DiscountOnCheck * 100);
  memcpy(p.buff+20, &sum, 3);			//20-22

  p.buff[23] = fr.Tax1;				//23
  p.buff[24] = fr.Tax2;				//24
  p.buff[25] = fr.Tax3;				//25
  p.buff[26] = fr.Tax4;				//26

  strncpy((char*)p.buff+27, (char*)fr.StringForPrinting, 40);
  if(sendcommand(CLOSE_CHECK, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != CLOSE_CHECK) return -1;

  if(errhand(&a) != 0) return fr.ResultCode;

  fr.OperatorNumber = a.buff[2];
  fr.Change = evalint64((unsigned char*)&a.buff+3, 5);
  fr.Change /= 100;

  return 0;
}
//-----------------------------------------------------------------------------
int Storno(void)
{
  parameter  p;
  answer     a;

  if(!connected) return -1;

  int64_t quant = llround(fr.Quantity * 1000);
  int64_t price = llround(fr.Price * 100);
  p.len   = 55;

  memcpy(p.buff,    &quant, 5);
  memcpy(p.buff+5,  &price, 5);
  p.buff[10] = fr.Department;

  p.buff[11] = fr.Tax1;
  p.buff[12] = fr.Tax2;
  p.buff[13] = fr.Tax3;
  p.buff[14] = fr.Tax4;

  strncpy((char*)p.buff+15, (char*)fr.StringForPrinting, 40);

  if(sendcommand(STORNO, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != STORNO) return -1;

  if(errhand(&a) != 0) return fr.ResultCode;

  fr.OperatorNumber = a.buff[2];

  return 0;
}
//-----------------------------------------------------------------------------
int PrintReportWithCleaning(void)
{
  parameter  p;
  answer     a;

  if(!connected) return -1;

  if(sendcommand(PRINT_REPORT_WITH_CLEANING, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != PRINT_REPORT_WITH_CLEANING) return -1;

  if(errhand(&a) != 0) return fr.ResultCode;

  fr.OperatorNumber = a.buff[2];
  fr.SessionNumber = evalint((unsigned char*)&a.buff+3, 2);

  return 0;
}
//-----------------------------------------------------------------------------
int PrintReportWithoutCleaning(void)
{
  parameter  p;
  answer     a;

  if(!connected) return -1;

  if(sendcommand(PRINT_REPORT_WITHOUT_CLEANING, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != PRINT_REPORT_WITHOUT_CLEANING) return -1;

  if(errhand(&a) != 0) return fr.ResultCode;

  fr.OperatorNumber = a.buff[2];
  fr.SessionNumber = evalint((unsigned char*)&a.buff+3, 2);

  return 0;
}
//-----------------------------------------------------------------------------
int PrintOperationReg(void)
{
  parameter  p;
  answer     a;

  if(!connected) return -1;

  if(sendcommand(PRINT_OPERATION_REG, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != PRINT_OPERATION_REG) return -1;

  return errhand(&a);
}
//-----------------------------------------------------------------------------
int DampRequest(void)
{
  parameter  p;
  answer     a;

  if(!connected) return -1;

  p.len    = 1;

  p.buff[0] = fr.DeviceCode;

  if(sendcommand(DUMP_REQUEST, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != DUMP_REQUEST) return -1;

  if(errhand(&a) != 0) return -1;

  return a.buff[2];  			//number of data blocks to return
}
//-----------------------------------------------------------------------------
int GetData(void)
{
  parameter  p;
  answer     a;

  if(!connected) return -1;

  if(sendcommand(GET_DATA, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != GET_DATA) return -1;

  if(errhand(&a) != 0) return fr.ResultCode;

  fr.DeviceCode = a.buff[2];
  fr.DeviceCodeDescription = devcodedesc[fr.DeviceCode];
  fr.DataBlockNumber = evalint((unsigned char*)&a.buff+3, 2);
  memcpy(fr.DataBlock, a.buff+5, 32);

  return 0;
}
//-----------------------------------------------------------------------------
int InterruptDataStream(void)
{
  parameter  p;
  answer     a;

  if(!connected) return -1;

  if(sendcommand(INTERRUPT_DATA_STREAM, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != INTERRUPT_DATA_STREAM) return -1;

  return errhand(&a);
}
//-----------------------------------------------------------------------------
int GetCashReg(void)
{
  parameter  p;
  answer     a;

  if(!connected) return -1;

  p.len    = 1;

  p.buff[0] = fr.RegisterNumber;

  if(sendcommand(GET_CASH_REG, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != GET_CASH_REG) return -1;

  if(errhand(&a) != 0) return fr.ResultCode;

  fr.OperatorNumber = a.buff[2];
  fr.ContentsOfCashRegister = evalint64((unsigned char*)&a.buff+3, 6);
  fr.ContentsOfCashRegister /= 100;

  return 0;
}
//-----------------------------------------------------------------------------
int GetOperationReg(void)
{
  parameter  p;
  answer     a;

  if(!connected) return -1;

  p.len    = 1;

  p.buff[0] = fr.RegisterNumber;

  if(sendcommand(GET_OPERATION_REG, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != GET_OPERATION_REG) return -1;

  if(errhand(&a) != 0) return fr.ResultCode;

  fr.OperatorNumber = a.buff[2];
  fr.ContentsOfOperationRegister = evalint((unsigned char*)&a.buff+3, 2);

  return 0;
}
//-----------------------------------------------------------------------------
int SetSerialNumber(void)
{
  parameter  p;
  answer     a;

  if(!connected) return -1;

  p.len    = 4;
  int num = atol(fr.SerialNumber);

  memcpy(p.buff, &num, sizeof(int));

  if(sendcommand(SET_SERIAL_NUMBER, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != SET_SERIAL_NUMBER) return -1;

  return errhand(&a);
}
//-----------------------------------------------------------------------------
int SetPointPosition(void)
{
  parameter  p;
  answer     a;

  if(!connected) return -1;

  p.len    = 1;

  p.buff[0] = fr.PointPosition;

  if(sendcommand(SET_POINT_POSITION, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != SET_POINT_POSITION) return -1;

  return errhand(&a);
}
//-----------------------------------------------------------------------------
int SetTime(void)
{
  parameter  p;
  answer     a;

  if(!connected) return -1;

  p.len    = 3;

  p.buff[2] = fr.Time.tm_sec;
  p.buff[1] = fr.Time.tm_min;
  p.buff[0] = fr.Time.tm_hour;

  if(sendcommand(SET_TIME, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != SET_TIME) return -1;

  return errhand(&a);
}
//-----------------------------------------------------------------------------
int SetDate(void)
{
  parameter  p;
  answer     a;

  if(!connected) return -1;

  p.len    = 3;

  p.buff[0] = fr.Date.tm_mday;
  p.buff[1] = fr.Date.tm_mon + 1;
  p.buff[2] = fr.Date.tm_year - 100;

  if(sendcommand(SET_DATE, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != SET_DATE) return -1;

  return errhand(&a);
}
//-----------------------------------------------------------------------------
int ConfirmDate(void)
{
  parameter  p;
  answer     a;

  if(!connected) return -1;

  p.len    = 3;

  p.buff[0] = fr.Date.tm_mday;
  p.buff[1] = fr.Date.tm_mon + 1;
  p.buff[2] = fr.Date.tm_year - 100;

  if(sendcommand(CONFIRM_DATE, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != CONFIRM_DATE) return -1;

  return errhand(&a);
}
//-----------------------------------------------------------------------------
int InitTable(void)
{
  parameter  p;

  if(!connected) return -1;

  if(sendcommand(INIT_TABLE, fr.Password, &p) < 0) return -1;
  return 0;
}
//-----------------------------------------------------------------------------
int WriteTable(void)
{
  parameter  p;
  answer     a;
  int      len;
  char    *tmp;

  if(!connected) return -1;

  GetFieldStruct();
  if(fr.FieldType == 1)
  {
    len = strlen(fr.ValueOfFieldString);
    tmp = fr.ValueOfFieldString;
  }
  else
  {
    len = fr.FieldSize;
    tmp = (char*)fr.ValueOfFieldInteger;
  };
  p.len = 4 + len;

  p.buff[0] = fr.TableNumber;
  memcpy(p.buff+1, &fr.FieldNumber, 2);
  p.buff[3] = fr.RowNumber;
  memcpy(p.buff+4, &tmp, len);

  if(sendcommand(WRITE_TABLE, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != WRITE_TABLE) return -1;

  return errhand(&a);
}
//-----------------------------------------------------------------------------
int ReadTable(void)
{
  parameter  p;
  answer     a;
  int      len;

  if(!connected) return -1;

  p.len    = 4;

  p.buff[0] = fr.TableNumber;
  memcpy(p.buff+1, &fr.FieldNumber, 2);
  p.buff[3] = fr.RowNumber;

  if(sendcommand(READ_TABLE, fr.Password, &p) < 0) return -1;
  if((len = readanswer(&a)) < 0) return -1;
  if(a.buff[0] != READ_TABLE) return -1;

  GetFieldStruct();

  if(fr.FieldType == 1) strncpy(fr.ValueOfFieldString, (char*)a.buff, len - 2);
  else fr.ValueOfFieldInteger = evalint64((unsigned char*)&a.buff+2, fr.FieldSize);

  return 0;
}
//-----------------------------------------------------------------------------
int GetFieldStruct(void)
{
  parameter  p;
  answer     a;

  if(!connected) return -1;

  p.len    = 2;

  p.buff[0] = fr.TableNumber;
  p.buff[1] = fr.FieldNumber;

  if(sendcommand(GET_FIELD_STRUCT, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != GET_FIELD_STRUCT) return -1;

  strncpy(fr.FieldName, (char*)&a.buff+1, 40);
  fr.FieldType = a.buff[41];
  fr.FieldSize = a.buff[42];
  fr.MINValueOfField = evalint64((unsigned char*)&a.buff+43, fr.FieldSize);
  fr.MAXValueOfField = evalint64((unsigned char*)&a.buff+43 + fr.FieldSize, fr.FieldSize);

  return 0;
}
//-----------------------------------------------------------------------------
int GetTableStruct(void)
{
  parameter  p;
  answer     a;

  if(!connected) return -1;

  p.len    = 1;

  p.buff[0] = fr.TableNumber;

  if(sendcommand(GET_TABLE_STRUCT, fr.Password, &p) < 0) return -1;
  if((readanswer(&a)) < 0) return -1;
  if(a.buff[0] != GET_TABLE_STRUCT) return -1;

  strncpy(fr.TableName, (char*)&a.buff+1, 40);
  fr.RowNumber   = evalint((unsigned char*)&a.buff+41, 2);;
  fr.FieldNumber = a.buff[43];

  return 0;
}
//-----------------------------------------------------------------------------
int WriteLicense(void)
{
  parameter  p;
  answer     a;

  if(!connected) return -1;

  int64_t num = atoll(fr.License);
  p.len    = 5;

  memcpy(p.buff, &num, 5);

  if(sendcommand(WRITE_LICENSE, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != WRITE_LICENSE) return -1;

  return errhand(&a);
}
//-----------------------------------------------------------------------------
int ReadLicense(void)
{
  parameter  p;
  answer     a;

  if(!connected) return -1;

  if(sendcommand(READ_LICENSE, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != READ_LICENSE) return -1;

  if(errhand(&a) != 0) return fr.ResultCode;

  sprintf(fr.License, "%.0lg", (double)evalint64((unsigned char*)&a.buff+2, 5));

  return 0;
}
//-----------------------------------------------------------------------------
int InitFM(void)
{
  parameter  p;
  answer     a;

  if(!connected) return -1;

  if(sendcommand(INIT_FM, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != INIT_FM) return -1;

  return errhand(&a);
}
//-----------------------------------------------------------------------------
int Fiscalization(void)
{
  parameter  p;
  answer 	   a;

  if(!connected) return -1;

  int     nti = fr.NewPasswordTI;
  int64_t rnm = atoll(fr.RNM);
  int64_t inn = atoll(fr.INN);
  p.len   = 14;

  memcpy(p.buff,   &nti, 4);
  memcpy(p.buff+4, &rnm, 5);
  memcpy(p.buff+9, &inn, 6);

  if(sendcommand(FISCALIZATION, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != FISCALIZATION) return -1;

  if(errhand(&a) != 0) return fr.ResultCode;

  fr.RegistrationNumber = evalint((unsigned char*)&a.buff+2, 1);
  fr.FreeRegistration   = evalint((unsigned char*)&a.buff+3, 1);
  fr.SessionNumber      = evalint((unsigned char*)&a.buff+4, 2);
  evaldate((unsigned char*)&a.buff+6, &fr.Date);

  return 0;
}
//-----------------------------------------------------------------------------
int FiscalReportForDatesRange(void)
{
  parameter  p;
  answer     a;

  if(!connected) return -1;

  p.len    = 7;

  p.buff[0] = fr.ReportType;

  p.buff[1] = fr.FirstSessionDate.tm_year - 100;
  p.buff[2] = fr.FirstSessionDate.tm_mon  + 1;
  p.buff[3] = fr.FirstSessionDate.tm_mday;

  p.buff[4] = fr.LastSessionDate.tm_year - 100;
  p.buff[5] = fr.LastSessionDate.tm_mon  + 1;
  p.buff[6] = fr.LastSessionDate.tm_mday;

  if(sendcommand(FISCAL_REPORT_FOR_DATES_RANGE, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != FISCAL_REPORT_FOR_DATES_RANGE) return -1;

  if(errhand(&a) != 0) return fr.ResultCode;

  evaldate((unsigned char*)&a.buff+2, &fr.FirstSessionDate);
  evaldate((unsigned char*)&a.buff+5, &fr.LastSessionDate);
  fr.FirstSessionNumber = evalint((unsigned char*)&a.buff+8, 2);
  fr.LastSessionNumber  = evalint((unsigned char*)&a.buff+10, 2);

  return 0;
}
//-----------------------------------------------------------------------------
int FiscalReportForSessionRange(void)
{
  parameter  p;
  answer     a;

  if(!connected) return -1;

  p.len    = 5;

  p.buff[0] = fr.ReportType;

  memcpy(p.buff+1, &fr.FirstSessionNumber, 2);
  memcpy(p.buff+3, &fr.LastSessionNumber, 2);

  if(sendcommand(FISCAL_REPORT_FOR_SESSION_RANGE, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != FISCAL_REPORT_FOR_SESSION_RANGE) return -1;

  if(errhand(&a) != 0) return fr.ResultCode;

  evaldate((unsigned char*)&a.buff+2, &fr.FirstSessionDate);
  evaldate((unsigned char*)&a.buff+5, &fr.LastSessionDate);
  fr.FirstSessionNumber = evalint((unsigned char*)&a.buff+8, 2);
  fr.LastSessionNumber  = evalint((unsigned char*)&a.buff+10, 2);

  return 0;
}
//-----------------------------------------------------------------------------
int InterruptFullReport(void)
{
  parameter  p;
  answer     a;

  if(!connected) return -1;

  if(sendcommand(INTERRUPT_FULL_REPORT, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != INTERRUPT_FULL_REPORT) return -1;

  return errhand(&a);
}
//-----------------------------------------------------------------------------
int GetFiscalizationParameters(void)
{
  parameter  p;
  answer     a;

  if(!connected) return -1;

  p.len    = 1;

  p.buff[0] = fr.RegistrationNumber;

  if(sendcommand(GET_FISCALIZATION_PARAMETERS, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != GET_FISCALIZATION_PARAMETERS) return -1;

  if(errhand(&a) != 0) return fr.ResultCode;

  fr.NewPasswordTI = evalint((unsigned char*)&a.buff+2, 4);
  sprintf(fr.RNM, "%.0lg", (double)evalint64((unsigned char*)&a.buff+6,  5));
  sprintf(fr.INN, "%.0lg", (double)evalint64((unsigned char*)&a.buff+11, 6));
  fr.SessionNumber = evalint((unsigned char*)&a.buff+17, 2);
  evaldate((unsigned char*)&a.buff+19, &fr.Date);

  return 0;
}
//-----------------------------------------------------------------------------
int GetFMRecordsSum(void)
{
  parameter  p;
  answer     a;

  if(!connected) return -1;

  p.len    = 1;

  p.buff[0] = fr.TypeOfSumOfEntriesFM;

  if(sendcommand(GET_FM_RECORDS_SUM, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != GET_FM_RECORDS_SUM) return -1;

  if(errhand(&a) != 0) return fr.ResultCode;

  fr.OperatorNumber = a.buff[2];
  fr.Summ1 = evalint64((unsigned char*)&a.buff+3,  8);
  fr.Summ1 /= 100;
  fr.Summ2 = evalint64((unsigned char*)&a.buff+11, 6);
  fr.Summ2 /= 100;
  fr.Summ3 = evalint64((unsigned char*)&a.buff+17, 6);
  fr.Summ3 /= 100;
  fr.Summ4 = evalint64((unsigned char*)&a.buff+23, 6);
  fr.Summ4 /= 100;

  return 0;
}
//-----------------------------------------------------------------------------
int GetLastFMRecordDate(void)
{
  parameter  p;
  answer     a;

  if(!connected) return -1;

  if(sendcommand(GET_LAST_FM_RECORD_DATE, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != GET_LAST_FM_RECORD_DATE) return -1;

  if(errhand(&a) != 0) return fr.ResultCode;

  fr.OperatorNumber   = a.buff[2];
  fr.LastFMRecordType = a.buff[3];
  evaldate((unsigned char*)&a.buff+4, &fr.Date);

  return 0;
}
//-----------------------------------------------------------------------------
int GetRangeDatesAndSessions(void)
{
  parameter  p;
  answer     a;

  if(!connected) return -1;

  if(sendcommand(GET_RANGE_DATES_AND_SESSIONS, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != GET_RANGE_DATES_AND_SESSIONS) return -1;

  if(errhand(&a) != 0) return fr.ResultCode;

  evaldate((unsigned char*)&a.buff+2, &fr.FirstSessionDate);
  evaldate((unsigned char*)&a.buff+5, &fr.LastSessionDate);
  fr.FirstSessionNumber = evalint((unsigned char*)&a.buff+8,  2);
  fr.LastSessionNumber  = evalint((unsigned char*)&a.buff+10, 2);

  return 0;
}
//-----------------------------------------------------------------------------
int EKLZDepartmentReportInDatesRange(void) {return 0;}
//-----------------------------------------------------------------------------
int EKLZDepartmentReportInSessionsRange(void) {return 0;}
//-----------------------------------------------------------------------------
int EKLZJournalOnSessionNumber(void) {return 0;}
//-----------------------------------------------------------------------------
int EKLZSessionReportInDatesRange(void) {return 0;}
//-----------------------------------------------------------------------------
int EKLZSessionReportInSessionRange(void) {return 0;}
//-----------------------------------------------------------------------------
int ReadEKLZDocumentOnKPK(void) {return 0;}
//-----------------------------------------------------------------------------
int ReadEKLZSessionTotal(void) {return 0;}
//-----------------------------------------------------------------------------
int StopEKLZDocumentPrinting(void) {return 0;}
//-----------------------------------------------------------------------------
int Correction(void) {return 0;}
//-----------------------------------------------------------------------------
int DozeOilCheck(void) {return 0;}
//-----------------------------------------------------------------------------
int SummOilCheck(void) {return 0;}
//-----------------------------------------------------------------------------
int SetDozeInMilliliters(void) {return 0;}
//-----------------------------------------------------------------------------
int SetDozeInMoney(void) {return 0;}
//-----------------------------------------------------------------------------
int OilSale(void) {return 0;}
//-----------------------------------------------------------------------------
int GetLiterSumCounter(void) {return 0;}
//-----------------------------------------------------------------------------
int GetRKStatus(void) {return 0;}
//-----------------------------------------------------------------------------
int LaunchRK(void) {return 0;}
//-----------------------------------------------------------------------------
int StopRK(void) {return 0;}
//-----------------------------------------------------------------------------
int ResetRK(void) {return 0;}
//-----------------------------------------------------------------------------
int ResetAllTRK(void) {return 0;}
//-----------------------------------------------------------------------------
int SetRKParameters(void) {return 0;}
//-----------------------------------------------------------------------------
int EjectSlipDocument(void) {return 0;}
//-----------------------------------------------------------------------------
int LoadLineData(void)
{
  parameter  p;
  answer     a;

  if(!connected) return -1;

  p.len    = 41;

  p.buff[0]  = fr.LineNumber;
  memcpy(p.buff + 1, fr.LineData, 40);

  if(sendcommand(LOAD_LINE_DATA, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != LOAD_LINE_DATA) return -1;

  if(errhand(&a) != 0) return fr.ResultCode;

  fr.OperatorNumber = a.buff[2];

  return 0;
}
//-----------------------------------------------------------------------------
int Draw(void)
{
  parameter  p;
  answer     a;

  if(!connected) return -1;

  p.len    = 2;

  p.buff[0]  = fr.FirstLineNumber;
  p.buff[1]  = fr.LastLineNumber;

  if(sendcommand(DRAW, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != DRAW) return -1;

  if(errhand(&a) != 0) return fr.ResultCode;

  fr.OperatorNumber = a.buff[2];

  return 0;
}
//-----------------------------------------------------------------------------
int PrintBarCode(void)
{
  parameter  p;
  answer     a;

  if(!connected) return -1;

  int64_t barcode = atoll(fr.BarCode);
  p.len    = 5;

  memcpy(p.buff, &barcode, 5);

  if(sendcommand(PRINT_BARCODE, fr.Password, &p) < 0) return -1;
  if(readanswer(&a) < 0) return -1;
  if(a.buff[0] != PRINT_BARCODE) return -1;

  if(errhand(&a) != 0) return fr.ResultCode;

  fr.OperatorNumber = a.buff[2];

  return 0;
}
//-----------------------------------------------------------------------------
