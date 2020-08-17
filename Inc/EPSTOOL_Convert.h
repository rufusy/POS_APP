/**
* \file
* \brief This module implements conversion functions.
*
* \author Ingenico
* \author Copyright (c) 2012 Ingenico, 28-32, boulevard de Grenelle,\n
* 75015 Paris, France, All Rights Reserved.
*
* \author Ingenico has intellectual property rights relating to the technology embodied\n
* in this software. In particular, and without limitation, these intellectual property rights may\n
* include one or more patents.\n
* This software is distributed under licenses restricting its use, copying, distribution, and\n
* and decompilation. No part of this software may be reproduced in any form by any means\n
* without prior written authorisation of Ingenico.
**/

#ifndef EPSTOOL_CONVERT_H_INCLUDED
#define EPSTOOL_CONVERT_H_INCLUDED

/////////////////////////////////////////////////////////////////
//// Macros & preprocessor definitions //////////////////////////

#define EPSTOOL_Convert_ULongToEmvBin			EPSTOOL_Convert_ULongToBigEndian		//!< Convert an unsigned long number into an EMV binary number.
#define EPSTOOL_Convert_UShortToEmvBin			EPSTOOL_Convert_UShortToBigEndian		//!< Convert an unsigned short number into an EMV binary number.
#define EPSTOOL_Convert_EmvBinToULong			EPSTOOL_Convert_BigEndianToULong		//!< Convert an EMV binary number into an unsigned long number.
#define EPSTOOL_Convert_EmvBinToUShort			EPSTOOL_Convert_BigEndianToUShort		//!< Convert an EMV binary number into an unsigned short number.

//// Types //////////////////////////////////////////////////////

//// Global variables ///////////////////////////////////////////

//// Functions //////////////////////////////////////////////////

void EPSTOOL_Convert_ULongToBigEndian(unsigned long number, unsigned char *bigEndian);
void EPSTOOL_Convert_ULongToLittleEndian(unsigned long number, unsigned char *littleEndian);
void EPSTOOL_Convert_UShortToBigEndian(unsigned short number, unsigned char *bigEndian);
void EPSTOOL_Convert_UShortToLittleEndian(unsigned short number, unsigned char *littleEndian);

unsigned long int EPSTOOL_Convert_BigEndianToULong(const unsigned char *bigEndian);
unsigned long int EPSTOOL_Convert_LittleEndianToULong(const unsigned char *littleEndian);
unsigned short int EPSTOOL_Convert_BigEndianToUShort(const unsigned char *bigEndian);
unsigned short int EPSTOOL_Convert_LittleEndianToUShort(const unsigned char *littleEndian);

int EPSTOOL_Convert_AsciiToUl(const char* asciiString, int length, unsigned long* number);

void EPSTOOL_Convert_UllToDcbNumber(unsigned long long value, void *dcb, int dcbLength);

#endif // EPSTOOL_CONVERT_H_INCLUDED
