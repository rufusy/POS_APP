/**
 * \file	Cless_Tags.h
 * \brief 	Define the cless sample application tags.
 * \details	Tags ranges for sample application are :
 * 				- 9F918800 to 9F91887F for simple tags.
 * 				- BF918800 to BF91887F for constructed tags.
 * \author	Ingenico
 * \author	Copyright (c) 2008 Ingenico, rue claude Chappe,\n
 *			07503 Guilherand-Granges, France, All Rights Reserved.
 *
 * \author	Ingenico has intellectual property rights relating to the technology embodied \n
 *			in this software. In particular, and without limitation, these intellectual property rights may\n
 *			include one or more patents.\n
 *			This software is distributed under licenses restricting its use, copying, distribution, and\n
 *			and decompilation. No part of this software may be reproduced in any form by any means\n
 *			without prior written authorization of Ingenico.
 **/

#ifndef __CLESS_SAMPLE_TAGS_H__INCLUDED__
#define __CLESS_SAMPLE_TAGS_H__INCLUDED__


/////////////////////////////////////////////////////////////////
//// Macros & preprocessor definitions //////////////////////////

// Simple tags
#define TAG_SAMPLE_ENCIPHERED_PIN_CODE		0x9F918800		/*!< Sample parameters : Enciphered pin code for on-line verification. */
#define TAG_SAMPLE_PARAMETERS_LABEL			0x9F918801		/*!< Sample parameters : Indicates the parameter label. */
#define TAG_SAMPLE_GENERIC_MONEY_EXTENDED	0x9F918802		/*!< Sample parameters : Defines a money. nom:3car, code:3car, decimal pos:1car, centseparator:1car, thousandseparator:1car, currencyposition:0after1before. - Format : t - Length : 10 bytes - Source : Terminal. */
#define TAG_SAMPLE_DOUBLE_DIP_TIMEOUT		0x9F918803		/*!< Sample parameters : Indicates how soon the reader can accept the same card to perform the next transaction having the same amount. Time in milliseconds. (VisaWave requirement)*/
#define TAG_SAMPLE_NO_CARD_TIMEOUT			0x9F918804		/*!< Sample parameters : Timout value for card detection process. Time in milliseconds. (VisaWave and Interac requirement)*/
#define TAG_SAMPLE_ZERO_CHECK_DEACTIVATED	0x9F918805		/*!< Sample parameters : Set to true, payWave kernel doesn't realize the zero check with the Online cryptogram */
#define TAG_SAMPLE_TRANSACTION_CASHBACK		0x9F918806		/*!< Internal transaction cashback : kernel subset. */
#define TAG_SAMPLE_INTERAC_MODE				0x9F918807		/*!< Sample parameters : Specific Interac Mode to perform Interac GUI operations. (Interac requirement)*/
#define TAG_SAMPLE_EXPRESSPAY_DOUBLE_TAP_TIME	0x9F918809		/*!< Sample parameters (Specific Expresspay): To configure the period of time the contactless reader field is deactivated between the two taps of the Mobile CVM. Time in milliseconds. Range: one to three seconds (default 1.5 seconds). (Expresspay requirement). */
#define TAG_SAMPLE_EXPRESSPAY_MODE				0x9F91880A		/*!< Sample parameters : Specific Expresspay Mode to perform Expresspay GUI operations. (Expresspay requirement)*/

// Constructed tags
#define TAG_SAMPLE_AID_PARAMETERS			0xBF918800		/*!< Sample parameters : AID subset. */
#define TAG_SAMPLE_CAKEYS_PARAMETERS		0xBF918801		/*!< Sample parameters : CAKEYS subset. */
#define TAG_SAMPLE_CAREVOK_PARAMETERS		0xBF918802		/*!< Sample parameters : CAREVOK subset. */
#define TAG_SAMPLE_ICS_PARAMETERS		    0xBF918803		/*!< Sample parameters : global parameters (ICS) subset. */
#define TAG_SAMPLE_SPECIFIC_PARAMETERS		0xBF918804		/*!< Sample parameters : kernel subset. */


/////////////////////////////////////////////////////////////////
//// Types //////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////
//// Global variables ///////////////////////////////////////////


/////////////////////////////////////////////////////////////////
//// Functions //////////////////////////////////////////////////


#endif // __CLESS_SAMPLE_TAGS_H__INCLUDED__
