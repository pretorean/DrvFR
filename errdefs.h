/***************************************************************************
                          errdefs.h  -  description
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
#ifndef ERRDEFS_H
#define ERRDEFS_H

static char *errmsg[] =
{
  "No errors found",													//00
  "Fiscal memory or timer error",											//01
  "Fiscal memory 1 is missing",												//02
  "Fiscal memory 2 is missing",												//03
  "Invalid arguments",													//04
  "There is no queried data",												//05
  "Output mode",													//06
  "Invalid arguments for the particular device",									//07
  "Invalid command for the particular device",										//08
  "Invalid command lengh",												//09
  "Invalid data format",												//0a
  "",															//0b
  "",															//0c
  "",															//0d
  "",															//0e
  "",															//0f
  "",															//10
  "No License",														//11
  "Factory number is already entered",											//12
  "Current date is less then last operation date",									//13
  "Subtotal memory is full",												//14
  "Shift ia opened",													//15
  "Shift is not opened",												//16
  "The number of the first shift larger then the last shift number",							//17
  "The date of the first shift is later then the last shift date",							//18
  "There is no queried data",												//19
  "",															//1a
  "Factory number is not entered",											//1b
  "There is corrupt data in the sertain range",										//1c
  "The last subtotal record is corrupt",										//1d
  "Registration limit is achieved",											//1e
  "Register memory is not present",											//1f
  "Money register overflow while adding data",										//20
  "Register value is less",												//21
  "Invalid date",													//22
  "",															//23
  "",															//24
  "",															//25
  "",															//26
  "",															//27
  "",															//28
  "",															//29
  "",															//2a
  "",															//2b
  "",															//2c
  "",															//2d
  "",															//2e
  "",															//2f
  "",															//30
  "",															//31
  "",															//32
  "Invalid command parameters",												//33
  "There is no queried data",												//34
  "Invalid command parameter for the current settings",									//35
  "Invalid command parameters for the particular device",								//36
  "Invalid command for the particular device",										//37
  "User memory error",													//38
  "Internal software error",												//39
  "",															//3a
  "",															//3b
  "Operation is not completed. Shift is opened",									//3c
  "Operation is not completed. Shift is not opened",									//3d
  "",															//3e
  "",															//3f
  "",															//40
  "",															//41
  "",															//42
  "",															//43
  "",															//44
  "",															//45
  "",															//46
  "",															//47
  "Subtotal overflow",													//48
  "Operation is not completed with this type of opened bill",								//49
  "Operation is not completed. Bill is not opened",									//4a
  "Bill's buffer is full",												//4b
  "",															//4c
  "Non cash summ is greater of bill summ",										//4d
  "Shift is active more then 24 hours",											//4e
  "Invalid password",													//4f
  "Printing the previous command",											//50
  "",															//51
  "",															//52
  "",															//53
  "",															//54
  "Operation is not completed. Bill is closed",										//55
  "",															//56
  "",															//57
  "Waiting for printing command continuing",										//58
  "Document is opened by another operator",										//59
  "Discount is greater of the proffit",											//5a
  "",															//5b
  "",															//5c
  "Table is not defined",												//5d
  "",															//5e
  "",															//5f
  "",															//60
  "",															//61
  "",															//62
  "",															//63
  "Fiscal memory is not present",											//64
  "",															//65
  "",															//66
  "Error connecting to the device",											//67
  "",															//68
  "",															//69
  "",															//6a
  "Bill tape is out",													//6b
  "Controll tape is out",												//6c
  "",															//6d
  "",															//6e
  "",															//6f
  "Fiscal memory overflow",												//70
  "Knife error",													//71
  "Invalid command in this submode",											//72
  "Invalid command in this mode",											//73
  "Base memory error"													//74
};

static char *ecrmodedesc[] =
{
  "",
  "Data output",													//0
  "Shift is opened less then 24 hours",											//1
  "Shift is opened more then 24 hours",											//2
  "Shift is closed",													//3
  "Blocked due to wrong Tax Inspector password entry",									//4
  "Waiting for date entry confirmation",										//5
  "Changing decimal point position mode",										//6
  "Document is opened",													//7
  "Tech. reset & tables init. mode",											//8
  "Test feed",														//9
  "Full fiscal report printing",											//10
  "EKLZ report printing"												//11
};

static char *ecrmode8desc[] =
{
  "Sale check is opened",
  "Buy check is opened",
  "Sale return check is opened",
  "Buy return check is opened"
};

static char *ecrsubmodedesc[] =
{
  "",
  "Passive out of paper",
  "Active out of paper",
  "Waiting for print continue",
  "Printing operation",
  "Full printing operation"
};

static char *devcodedesc[] =
{
  "FM1 flash",
  "FM2 flash",
  "Clock",
  "Powerindependent memory",
  "FM processor",
  "FR usr memory",
  "FR base memory"
};

#endif
