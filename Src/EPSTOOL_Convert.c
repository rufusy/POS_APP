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

/////////////////////////////////////////////////////////////////
//// Includes ///////////////////////////////////////////////////

#include "sdk.h"
#include "GTL_Assert.h"

#include "EPSTOOL_Convert.h"

//// Macros & preprocessor definitions //////////////////////////

//// Types //////////////////////////////////////////////////////

//// Static function definitions ////////////////////////////////

//// Global variables ///////////////////////////////////////////

//// Functions //////////////////////////////////////////////////

//! \brief Convert an unsigned long number into a big endian encoded number.
//! \param[in] number Number to convert.
//! \param[out] bigEndian Converted number. It must be a 4 bytes buffer.
void EPSTOOL_Convert_ULongToBigEndian(unsigned long number, unsigned char *bigEndian)
{
	// Check input parameters
	ASSERT(bigEndian != NULL);

	bigEndian[0] = (unsigned char)(number >> 24);
	bigEndian[1] = (unsigned char)(number >> 16);
	bigEndian[2] = (unsigned char)(number >> 8);
	bigEndian[3] = (unsigned char)(number);
}

//! \brief Convert an unsigned long number into a little endian encoded number.
//! \param[in] number Number to convert.
//! \param[out] littleEndian Converted number. It must be a 4 bytes buffer.
void EPSTOOL_Convert_ULongToLittleEndian(unsigned long number, unsigned char *littleEndian)
{
	// Check input parameters
	ASSERT(littleEndian != NULL);

	littleEndian[0] = (unsigned char)(number);
	littleEndian[1] = (unsigned char)(number >> 8);
	littleEndian[2] = (unsigned char)(number >> 16);
	littleEndian[3] = (unsigned char)(number >> 24);
}

//! \brief Convert an unsigned short number into a big endian encoded number.
//! \param[in] number Number to convert.
//! \param[out] bigEndian Converted number. It must be a 2 bytes buffer.
void EPSTOOL_Convert_UShortToBigEndian(unsigned short number, unsigned char *bigEndian)
{
	// Check input parameters
	ASSERT(bigEndian != NULL);

	bigEndian[0] = (unsigned char)(number >> 8);
	bigEndian[1] = (unsigned char)(number);
}

//! \brief Convert an unsigned short number into a little endian encoded number.
//! \param[in] number Number to convert.
//! \param[out] littleEndian Converted number. It must be a 2 bytes buffer.
void EPSTOOL_Convert_UShortToLittleEndian(unsigned short number, unsigned char *littleEndian)
{
	// Check input parameters
	ASSERT(littleEndian != NULL);

	littleEndian[0] = (unsigned char)(number);
	littleEndian[1] = (unsigned char)(number >> 8);
}

//! \brief Convert a big endian encoded number into an unsigned long number.
//! \param[in] bigEndian Big endian number to convert.
//! \return The converted number.
unsigned long int EPSTOOL_Convert_BigEndianToULong(const unsigned char *bigEndian)
{
	// Check input parameters
	ASSERT(bigEndian != NULL);

	return (((unsigned long)bigEndian[0]) << 24) | (((unsigned long)bigEndian[1]) << 16)
			| (((unsigned long)bigEndian[2]) << 8) | ((unsigned long)bigEndian[3]);
}

//! \brief Convert a little endian encoded number into an unsigned long number.
//! \param[in] littleEndian Little endian number to convert.
//! \return The converted number.
unsigned long int EPSTOOL_Convert_LittleEndianToULong(const unsigned char *littleEndian)
{
	// Check input parameters
	ASSERT(littleEndian != NULL);

	return ((unsigned long)littleEndian[0]) | (((unsigned long)littleEndian[1]) << 8)
			| (((unsigned long)littleEndian[2]) << 16) | (((unsigned long)littleEndian[3]) << 24);
}

//! \brief Convert a big endian encoded number into an unsigned short number.
//! \param[in] bigEndian Big endian number to convert.
//! \return The converted number.
unsigned short int EPSTOOL_Convert_BigEndianToUShort(const unsigned char *bigEndian)
{
	// Check input parameters
	ASSERT(bigEndian != NULL);

	return (((unsigned short)bigEndian[0]) << 8) | ((unsigned short)bigEndian[1]);
}

//! \brief Convert a little endian encoded number into an unsigned short number.
//! \param[in] littleEndian Little endian number to convert.
//! \return The converted number.
unsigned short int EPSTOOL_Convert_LittleEndianToUShort(const unsigned char *littleEndian)
{
	// Check input parameters
	ASSERT(littleEndian != NULL);

	return ((unsigned short)littleEndian[0]) | (((unsigned short)littleEndian[1]) << 8);
}

//! \brief Convert an ASCII coded number into an unsigned long number.
//! \param[in] asciiString ASCII string number to convert.
//! \param[in] length Length of \a asciiString. If set to (-1), the length of the string is calculated (so the string must be zero terminated).
//! \param[out] number Converted unsigned long number.
//! \return \a TRUE if the convertion is successful. \a FALSE if an invalid character is found.
int EPSTOOL_Convert_AsciiToUl(const char* asciiString, int length, unsigned long* number)
{
	int result;
	int index;

	ASSERT(asciiString != NULL);
	ASSERT(number != NULL);

	if (length < 0)
		length = strlen(asciiString);

	*number = 0;
	index = 0;
	result = TRUE;
	while((result) && (index < length))
	{
		if ((*asciiString >= '0') && (*asciiString <= '9'))
			*number = (*number * 10) + (unsigned long)(*asciiString - '0');
		else result = FALSE;

		index++;
		asciiString++;
	}

	return result;
}

//! \brief Convert an unsigned 64 bits number into a DCB number.
//! \param[in] value Unsigned 64 bits number.
//! \param[out] dcb Convert DCB number.
//! \param[in] dcbLength Length in bytes of \a dcb.
void EPSTOOL_Convert_UllToDcbNumber(unsigned long long value, void *dcb, int dcbLength)
{
	unsigned char* ptr;
	int firstHalf;
	unsigned long long maxValue;

	ASSERT(dcb != NULL);
	ASSERT(dcbLength > 0);

	switch(dcbLength)
	{
	case 0:		maxValue = 0ULL;					break;
	case 1:		maxValue = 99ULL;					break;
	case 2:		maxValue = 9999ULL;					break;
	case 3:		maxValue = 999999ULL;				break;
	case 4:		maxValue = 99999999ULL;				break;
	case 5:		maxValue = 9999999999ULL;			break;
	case 6:		maxValue = 999999999999ULL;			break;
	case 7:		maxValue = 99999999999999ULL;		break;
	case 8:		maxValue = 9999999999999999ULL;		break;
	case 9:		maxValue = 999999999999999999ULL;	break;
	default:	maxValue = 0xffffffffffffffffULL;	break;
	}

	// Truncate by removing extra ending digits
	while(value > maxValue)
		value /= 10;

	ptr = ((unsigned char*)dcb) + (dcbLength - 1);
	dcbLength *= 2;
	firstHalf = FALSE;
	while(dcbLength > 0)
	{
		if (firstHalf)
		{
			*ptr |= (unsigned char)((value % 10) << 4);
			ptr--;
		}
		else *ptr = (unsigned char)(value % 10);

		firstHalf = !firstHalf;

		value /= 10;
		dcbLength--;
	}
}
