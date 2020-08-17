/**
* \file
* \brief This module implements functions to convert ISO8859 strings into UTF-8 strings.
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

#ifndef EPSTOOL_UNICODE_H_INCLUDED
#define EPSTOOL_UNICODE_H_INCLUDED

/////////////////////////////////////////////////////////////////
//// Macros & preprocessor definitions //////////////////////////

//// Types //////////////////////////////////////////////////////

//! \brief Type of a UNICODE character.
typedef unsigned long EPSTOOL_WideChar_t;

//! \brief List of supported charsets.
typedef enum
{
	EPSTOOL_UNICODE_UTF8 = 0,    //!< Unicode utf8
	EPSTOOL_UNICODE_ISO_8859_1,  //!< Latin-1 Western European
	EPSTOOL_UNICODE_ISO_8859_2,  //!< Latin-2 Central European
	EPSTOOL_UNICODE_ISO_8859_3,  //!< Latin-3 South European
	EPSTOOL_UNICODE_ISO_8859_4,  //!< Latin-4 North European
	EPSTOOL_UNICODE_ISO_8859_5,  //!< Latin/Cyrillic
	EPSTOOL_UNICODE_ISO_8859_6,  //!< Latin/Arabic
	EPSTOOL_UNICODE_ISO_8859_7,  //!< Latin/Greek
	EPSTOOL_UNICODE_ISO_8859_8,  //!< Latin/Hebrew
	EPSTOOL_UNICODE_ISO_8859_9,  //!< Latin-5 Turkish
	EPSTOOL_UNICODE_ISO_8859_10, //!< Latin-6 Nordic
	EPSTOOL_UNICODE_ISO_8859_11, //!< Latin/Thai
	EPSTOOL_UNICODE_ISO_8859_13, //!< Latin-7 Baltic Rim
	EPSTOOL_UNICODE_ISO_8859_14, //!< Latin-8 Celtic
	EPSTOOL_UNICODE_ISO_8859_15, //!< Latin-9
	EPSTOOL_UNICODE_ISO_8859_16, //!< Latin-10 South-Eastern European

	EPSTOOL_UNICODE_LAST
} EPSTOOL_Unicode_charset_e;

//// Static function definitions ////////////////////////////////

//// Global variables ///////////////////////////////////////////

//// Functions //////////////////////////////////////////////////

EPSTOOL_WideChar_t EPSTOOL_UnicodeChar_Decode(char character, EPSTOOL_Unicode_charset_e charset);
unsigned long EPSTOOL_UnicodeChar_Utf8Length(EPSTOOL_WideChar_t unicode);
unsigned long EPSTOOL_UnicodeChar_UnicodeToUtf8 (EPSTOOL_WideChar_t unicode, char *utf8, unsigned long size);

unsigned long EPSTOOL_Unicode_CharsetToUtf8(const char *string, unsigned long stringLength,
		EPSTOOL_Unicode_charset_e charset, char *utf8, unsigned long utf8Length);

char *EPSTOOL_Unicode_CharsetToNewUtf8(const char *string, unsigned long stringLength, EPSTOOL_Unicode_charset_e charset);

#endif // EPSTOOL_UNICODE_H_INCLUDED
