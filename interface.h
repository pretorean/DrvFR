/***************************************************************************
                          interface.h  -  description
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

#ifndef _INTERFACE_H
#define _INTERFACE_H

#include <time.h>
#define false			0
#define true			1

#define ENQ			0x05
#define STX			0x02
#define ACK			0x06
#define NAK			0x15


/****************************************************
 * FR Interface Functions Prototypes                *
 ****************************************************/

int Connect (void);
int DisConnect (void);
int Beep (void);
int Buy (void);
int CancelCheck (void);
int CashIncome (void);
int CashOutcome (void);
int Charge (void);
int CheckSubTotal (void);
int CloseCheck (void);
int ConfirmDate (void);
int ContinuePrinting (void);
int Correction (void);
int CutCheck (void);
int DampRequest (void);
int Discount (void);
int DozeOilCheck (void);
int Draw (void);
int EjectSlipDocument (void);
int EKLZDepartmentReportInDatesRange (void);
int EKLZDepartmentReportInSessionsRange (void);
int EKLZJournalOnSessionNumber (void);
int EKLZSessionReportInDatesRange (void);
int EKLZSessionReportInSessionRange (void);
int FeedDocument (void);
int Fiscalization (void);
int FiscalReportForDatesRange (void);
int FiscalReportForSessionRange (void);
int GetData (void);
int GetDeviceMetrics(void);
int GetExchangeParam (void);
int GetFieldStruct (void);
int GetFiscalizationParameters (void);
int GetFMRecordsSum (void);
int GetECRStatus (void);
int GetLastFMRecordDate (void);
int GetLiterSumCounter (void);
int GetCashReg (void);
int GetOperationReg (void);
int GetRangeDatesAndSessions (void);
int GetRKStatus (void);
int GetTableStruct (void);
int InitFM (void);
int InitTable (void);
int InterruptDataStream (void);
int InterruptFullReport (void);
int InterruptTest (void);
int LaunchRK (void);
int LoadLineData(void);
int OilSale (void);
int OpenDrawer (void);
int PrintBarCode(void);
int PrintDocumentTitle (void);
int PrintOperationReg (void);
int PrintReportWithCleaning (void);
int PrintReportWithoutCleaning (void);
int PrintString (void);
int PrintWideString (void);
int ReadEKLZDocumentOnKPK (void);
int ReadEKLZSessionTotal (void);
int ReadLicense (void);
int ReadTable (void);
int ResetAllTRK (void);
int ResetRK (void);
int ResetSettings (void);
int ResetSummary (void);
int ReturnBuy (void);
int ReturnSale (void);
int Sale (void);
int SetDate (void);
int SetDozeInMilliliters (void);
int SetDozeInMoney (void);
int SetExchangeParam (void);
int SetPointPosition (void);
int SetRKParameters (void);
int SetSerialNumber (void);
int SetTime (void);
int StopEKLZDocumentPrinting (void);
int StopRK (void);
int Storno (void);
int StornoCharge (void);
int StornoDiscount (void);
int SummOilCheck (void);
int Test (void);
int WriteLicense (void);
int WriteTable (void);

#endif
