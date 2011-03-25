/***************************************************************************
                          drvfr.h  -  description
                             -------------------
    begin                : Sun Jan 20 2002
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

#ifndef _DRVFR_H
#define _DRVFR_H

#include <time.h>

/****************************************************
 * FR connamds                                      *
 ****************************************************/

#define DUMP_REQUEST			0x01
#define GET_DATA			0x02
#define INTERRUPT_DATA_STREAM		0x03
#define GET_ECR_STATUS			0x11
#define PRINT_WIDE_STRING		0x12
#define BEEP				0x13
#define SET_EXCHANGE_PARAM		0x14
#define GET_EXCHANGE_PARAM		0x15
#define RESET_SETTINGS			0x16
#define PRINT_STRING			0x17
#define PRINT_DOCUMENT_TITLE		0x18
#define TEST				0x19
#define GET_CASH_REG			0x1a
#define GET_OPERATION_REG		0x1b
#define WRITE_LICENSE			0x1c
#define READ_LICENSE			0x1d
#define WRITE_TABLE			0x1e
#define READ_TABLE			0x1f
#define SET_POINT_POSITION		0x20
#define SET_TIME			0x21
#define SET_DATE			0x22
#define CONFIRM_DATE			0x23
#define INIT_TABLE			0x24
#define CUT_CHECK			0x25
#define RESET_SUMMARY			0x27
#define OPEN_DRAWER			0x28
#define FEED_DOCUMENT			0x29
#define EJECT_SLIP_DOC			0x2a
#define INTERRUPT_TEST			0x2b
#define PRINT_OPERATION_REG		0x2c
#define GET_TABLE_STRUCT		0x2d
#define GET_FIELD_STRUCT		0x2e
#define PRINT_REPORT_WITHOUT_CLEANING	0x40
#define PRINT_REPORT_WITH_CLEANING	0x41
#define CASH_INCOME			0x50
#define CASH_OUTCOME			0x51
#define SET_SERIAL_NUMBER		0x60
#define INIT_FM				0x61
#define GET_FM_RECORDS_SUM		0x62
#define GET_LAST_FM_RECORD_DATE		0x63
#define GET_RANGE_DATES_AND_SESSIONS	0x64
#define FISCALIZATION			0x65
#define FISCAL_REPORT_FOR_DATES_RANGE	0x66
#define FISCAL_REPORT_FOR_SESSION_RANGE	0x67
#define INTERRUPT_FULL_REPORT		0x68
#define GET_FISCALIZATION_PARAMETERS	0x69
#define FISCAL_PRINT_SLIP_DOC		0x70
#define PRINT_SLIP_DOC			0x71
#define SALE				0x80
#define BUY				0x81
#define RETURN_SALE			0x82
#define RETURN_BUY			0x83
#define STORNO				0x84
#define CLOSE_CHECK			0x85
#define DISCOUNT			0x86
#define CHARGE				0x87
#define CANCEL_CHECK			0x88
#define CHECK_SUBTOTAL			0x89
#define STORNO_DISCOUNT			0x8a
#define STORNO_CHARGE			0x8b
#define DOZE_OIL_CHECK			0x90
#define SUMM_OIL_CHECK			0x91
#define CORRECTION			0x92
#define SET_DOZE_IN_MILLILITERS		0x93
#define SET_DOZE_IN_MONEY		0x94
#define OIL_SALE			0x95
#define STOP_RK				0x96
#define LAUNCH_RK			0x97
#define RESET_RK			0x98
#define RESET_ALL_TRK			0x99
#define SET_RK_PARAMETERS		0x9a
#define GET_LITER_SUM_COUNTER		0x9b
#define GET_CURRENT_DOZE		0x9e
#define GET_RK_STATUS			0x9f
#define ECT_DEP_DATE_REP		0xa0
#define ECT_DEP_SHIFT_REP		0xa1
#define ECT_SHIFT_DATE_REP		0xa2
#define ECT_SHIFT_SHIFT_REP		0xa3
#define ECT_SHIFT_TOTAL			0xa4
#define ECT_PAY_DOC_BY_NUMBER		0xa5
#define ECT_BY_SHIFT_NUMBER		0xa6
#define ECT_REPORT_INTR			0xa7
#define CONTINUE_PRINTING		0xb0
#define LOAD_LINE_DATA			0xc0
#define DRAW				0xc1
#define PRINT_BARCODE			0xc2
#define GET_DEVICE_METRICS		0xfc
#define CTRL_ADD_DEVICE			0xfd

/****************************************************
 * FR properties structure                          *
 * Will be included into class declaration in future*
 ****************************************************/

typedef struct
{
  int    BatteryCondition;
  int    BaudRate;
  char*  BarCode;
  double Change;
  int    CheckIsClosed;
  int    CheckIsMadeOut;
  int    ComPortNumber;
  double ContentsOfCashRegister;
  int    ContentsOfOperationRegister;
  int    CurrentDozeInMilliliters;
  double CurrentDozeInMoney;
  int    CutType;
  unsigned char* DataBlock;
  int    DataBlockNumber;
  struct tm Date;
  int    Department;
  int    DeviceCode;
  char*  DeviceCodeDescription;
  double DiscountOnCheck;
  char*  DocumentName;
  int    DocumentNumber;
  int    OpenDocumentNumber;
  int    DozeInMilliliters;
  double DozeInMoney;
  int    DrawerNumber;
  int    ECRAdvancedMode;
  char*  ECRAdvancedModeDescription;
  int    ECRBuild;
  int    ECRMode;
  int    ECRMode8Status;
  char*  ECRModeDescription;
  struct tm ECRSoftDate;
  char*  ECRSoftVersion;
  int    EKLZIsPresent;
  int    EmergencyStopCode;
  char*  EmergencyStopCodeDescription;
  char*  FieldName;
  int    FieldNumber;
  int    FieldSize;
  int    FieldType;
  int    FirstLineNumber;
  struct tm FirstSessionDate;
  int    FirstSessionNumber;
  int    FM1IsPresent;
  int    FM2IsPresent;
  int    FMBuild;
  int    FMOverflow;
  struct tm FMSoftDate;
  char*  FMSoftVersion;
  int    FreeRecordInFM;
  int    FreeRegistration;
  char*  INN;
  int    JournalRibbonIsPresent;
  int    JournalRibbonOpticalSensor;
  int    JournalRibbonLever;
  int    KPKNumber;
  int    LastFMRecordType;
  int    LastLineNumber;
  struct tm LastSessionDate;
  int    LastSessionNumber;
  char*  License;
  int    LicenseIsPresent;
  int    LidPositionSensor;
  int    LogicalNumber;
  int    LineNumber;
  unsigned char* LineData;
  int    MAXValueOfField;
  int    MINValueOfField;
  int    Motor;
  char*  NameCashReg;
  char*  NameOperationReg;
  int    NewPasswordTI;
  int    OperatorNumber;
  int    Password;
  int    Pistol;
  int    PointPosition;
  int    PortNumber;
  double Price;
  double Quantity;
  int    ReceiptRibbonIsPresent;
  int    ReceiptRibbonOpticalSensor;
  int    ReceiptRibbonLever;
  int    Timeout;
  int    RegisterNumber;
  int    ReportType;
  int    RegistrationNumber;
  int    ResultCode;
  char*  ResultCodeDescription;
  int    RKNumber;
  char*  RNM;
  int    RoughValve;
  int    RowNumber;
  int    RunningPeriod;
  char*  SerialNumber;
  int    SessionNumber;
  int    SlipDocumentIsMoving;
  int    SlipDocumentIsPresent;
  int    SlowingInMilliliters;
  int    SlowingValve;
  int    StatusRK;
  char*  StatusRKDescription;
  char*  StringForPrinting;
  int    StringQuantity;
  double Summ1;
  double Summ2;
  double Summ3;
  double Summ4;
  char*  TableName;
  int    TableNumber;
  int    Tax1;
  int    Tax2;
  int    Tax3;
  int    Tax4;
  struct tm Time;
  int    TRKNumber;
  int    TypeOfSumOfEntriesFM;
  int    UseJournalRibbon;
  int    UseReceiptRibbon;
  int    UseSlipDocument;
  int    UModel;
  int    UMajorType;
  int    UMinorType;
  int    UMajorProtocolVersion;
  int    UMinorProtocolVersion;
  int    UCodePage;
  char*  UDescription;
  int    ValueOfFieldInteger;
  char*  ValueOfFieldString;
} fr_prop;

/****************************************************
 * FR interface functions  & properties structure   *
 * Will be removed in future                        *
 ****************************************************/

struct fr_func
  {
    int (*Connect)(void);
    int (*DisConnect)(void);
    int (*Beep)(void);
    int (*Buy)(void);
    int (*CancelCheck)(void);
    int (*CashIncome)(void);
    int (*CashOutcome)(void);
    int (*Charge)(void);
    int (*CheckSubTotal)(void);
    int (*CloseCheck)(void);
    int (*ConfirmDate)(void);
    int (*ContinuePrinting)(void);
    int (*Correction)(void);
    int (*CutCheck)(void);
    int (*DampRequest)(void);
    int (*Discount)(void);
    int (*DozeOilCheck)(void);
    int (*Draw)(void);
    int (*EjectSlipDocument)(void);
    int (*EKLZDepartmentReportInDatesRange)(void);
    int (*EKLZDepartmentReportInSessionsRange)(void);
    int (*EKLZJournalOnSessionNumber)(void);
    int (*EKLZSessionReportInDatesRange)(void);
    int (*EKLZSessionReportInSessionRange)(void);
    int (*FeedDocument)(void);
    int (*Fiscalization)(void);
    int (*FiscalReportForDatesRange)(void);
    int (*FiscalReportForSessionRange)(void);
    int (*GetData)(void);
    int (*GetDeviceMetrics)(void);
    int (*GetExchangeParam)(void);
    int (*GetFieldStruct)(void);
    int (*GetFiscalizationParameters)(void);
    int (*GetFMRecordsSum)(void);
    int (*GetECRStatus)(void);
    int (*GetLastFMRecordDate)(void);
    int (*GetLiterSumCounter)(void);
    int (*GetCashReg)(void);
    int (*GetOperationReg)(void);
    int (*GetRangeDatesAndSessions)(void);
    int (*GetRKStatus)(void);
    int (*GetTableStruct)(void);
    int (*InitFM)(void);
    int (*InitTable)(void);
    int (*InterruptDataStream)(void);
    int (*InterruptFullReport)(void);
    int (*InterruptTest)(void);
    int (*LaunchRK)(void);
    int (*LoadLineData)(void);
    int (*OilSale)(void);
    int (*OpenDrawer)(void);
    int (*PrintBarCode)(void);
    int (*PrintDocumentTitle)(void);
    int (*PrintOperationReg)(void);
    int (*PrintReportWithCleaning)(void);
    int (*PrintReportWithoutCleaning)(void);
    int (*PrintString)(void);
    int (*PrintWideString)(void);
    int (*ReadEKLZDocumentOnKPK)(void);
    int (*ReadEKLZSessionTotal)(void);
    int (*ReadLicense)(void);
    int (*ReadTable)(void);
    int (*ResetAllTRK)(void);
    int (*ResetRK)(void);
    int (*ResetSettings)(void);
    int (*ResetSummary)(void);
    int (*ReturnBuy)(void);
    int (*ReturnSale)(void);
    int (*Sale)(void);
    int (*SetDate)(void);
    int (*SetDozeInMilliliters)(void);
    int (*SetDozeInMoney)(void);
    int (*SetExchangeParam)(void);
    int (*SetPointPosition)(void);
    int (*SetRKParameters)(void);
    int (*SetSerialNumber)(void);
    int (*SetTime)(void);
    int (*StopEKLZDocumentPrinting)(void);
    int (*StopRK)(void);
    int (*Storno)(void);
    int (*StornoCharge)(void);
    int (*StornoDiscount)(void);
    int (*SummOilCheck)(void);
    int (*Test)(void);
    int (*WriteLicense)(void);
    int (*WriteTable)(void);
    /* Pointer to the interface properties structure */
    fr_prop *prop;
  };

/****************************************************
 * Returns the pointer to the properties structure. *
 * Will be removed by class constructor in future   *
 ****************************************************/

fr_func *drvfrInitialize(void);

#endif
