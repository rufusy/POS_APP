/**
 * \file	CLESS_SAMPLE_GuiState.h
 * \brief 	Gui state management functions.
 *
 * \author	Ingenico
 * \author	Copyright (c) 2010 Ingenico, rue claude Chappe,\n
 *			07503 Guilherand-Granges, France, All Rights Reserved.
 *
 * \author	Ingenico has intellectual property rights relating to the technology embodied \n
 *			in this software. In particular, and without limitation, these intellectual property rights may\n
 *			include one or more patents.\n
 *			This software is distributed under licenses restricting its use, copying, distribution, and\n
 *			and decompilation. No part of this software may be reproduced in any form by any means\n
 *			without prior written authorization of Ingenico.
 **/


#ifndef __CLESS_SAMPLE_GUISTATE_H__INCLUDED__
#define __CLESS_SAMPLE_GUISTATE_H__INCLUDED__


/////////////////////////////////////////////////////////////////////////////////////
//// Macros & preprocessor definitions //////////////////////////////////////////////


#define SDSA_KERNEL_IDENTIFIER_UNKNOWN					0x00		/*!< Unknown kernel. */
#define SDSA_KERNEL_IDENTIFIER_PAYPASS					0x01		/*!< Indicates that PayPass Kernel was used to perform the transaction. */
#define SDSA_KERNEL_IDENTIFIER_PAYWAVE					0x02		/*!< Indicates that payWave Kernel was used to perform the transaction. */
#define SDSA_KERNEL_IDENTIFIER_VISAWAVE					0x03		/*!< Indicates that VisaWave Kernel was used to perform the transaction. */
#define SDSA_KERNEL_IDENTIFIER_EXPRESSPAY				0x04		/*!< Indicates that ExpressPay Kernel was used to perform the transaction. */



// Screen definitions
#define CLESS_SAMPLE_SCREEN_IDLE								(1)				/*!<  */
#define CLESS_SAMPLE_SCREEN_PRESENT_CARD						(2)				/*!<  */
#define CLESS_SAMPLE_SCREEN_RETRY								(3)				/*!<  */
#define CLESS_SAMPLE_SCREEN_USE_CONTACT							(4)				/*!<  */
#define CLESS_SAMPLE_SCREEN_REMOVE_CARD							(5)				/*!<  */
#define CLESS_SAMPLE_SCREEN_WAIT_CARD_REMOVAL					(6)				/*!<  */
#define CLESS_SAMPLE_SCREEN_TIMEOUT_ELAPSED						(7)				/*!<  */
#define CLESS_SAMPLE_SCREEN_CANCELLED							(8)				/*!<  */
#define CLESS_SAMPLE_SCREEN_PROCESSING							(9)				/*!<  */
#define CLESS_SAMPLE_SCREEN_COLLISION							(10)			/*!<  */
#define CLESS_SAMPLE_SCREEN_ONLINE_APPROVED						(11)			/*!<  */
#define CLESS_SAMPLE_SCREEN_ONLINE_PIN_REQUIRED					(12)			/*!<  */
#define CLESS_SAMPLE_SCREEN_SIGNATURE_REQUIRED					(13)			/*!<  */
#define CLESS_SAMPLE_SCREEN_ERROR           					(14)			/*!<  */
#define CLESS_SAMPLE_SCREEN_CARD_BLOCKED				        (15)			/*!<  */
#define CLESS_SAMPLE_SCREEN_APPLICATION_BLOCKED                 (16)            /*!<  */
#define CLESS_SAMPLE_SCREEN_OFFLINE_APPROVED			        (17)			/*!<  */
#define CLESS_SAMPLE_SCREEN_OFFLINE_DECLINED			        (18)			/*!<  */
#define CLESS_SAMPLE_SCREEN_ONLINE_PROCESSING			        (19)			/*!<  */
#define CLESS_SAMPLE_SCREEN_ONLINE_DECLINED				        (20)			/*!<  */
#define CLESS_SAMPLE_SCREEN_PIN_CANCEL					        (21)			/*!<  */
#define CLESS_SAMPLE_SCREEN_PIN_ERROR					        (22)			/*!<  */
#define CLESS_SAMPLE_SCREEN_ERASE_CUSTOMER				        (23)			/*!<  */
#define CLESS_SAMPLE_SCREEN_USER                		        (24)			/*!<  */
#define CLESS_SAMPLE_SCREEN_KEYBOARD                            (25)			/*!<  */
#define CLESS_SAMPLE_SCREEN_ERROR_STATUS				        (26)			/*!<  */
#define CLESS_SAMPLE_SCREEN_BATCH_ERROR        		            (27)			/*!<  */
#define CLESS_SAMPLE_SCREEN_REPRESENT_CARD						(28)			/*!<  */
#define CLESS_SAMPLE_SCREEN_ONLINE_UNABLE				        (29)			/*!<  */
#define CLESS_SAMPLE_SCREEN_REMOVE_CARD_TWO_LINES				(30)			/*!<  */
#define CLESS_SAMPLE_SCREEN_TRY_ANOTHER_CARD					(31)			/*!<  */
#define CLESS_SAMPLE_SCREEN_PHONE_INSTRUCTIONS					(32)			/*!<  */
#define CLESS_SAMPLE_SCREEN_PHONE_INSTRUCTIONS_RETRY			(33)			/*!<  */

#define CLESS_SAMPLE_PAYWAVE_SCREEN_STOPPED                     (34)			/*!<  */
#define CLESS_SAMPLE_PAYWAVE_SCREEN_PIN_CANCEL                  (35)            /*!<  */
#define CLESS_SAMPLE_PAYWAVE_SCREEN_PIN_ERROR                   (36)            /*!<  */


#define CLESS_SAMPLE_VISAWAVE_SCREEN_STATUS_ERROR				(40)			/*!<  */
#define CLESS_SAMPLE_VISAWAVE_SCREEN_OFFLINE_APPROVED			(41)			/*!<  */
#define CLESS_SAMPLE_VISAWAVE_SCREEN_OFFLINE_DECLINED			(42)			/*!<  */
#define CLESS_SAMPLE_VISAWAVE_SCREEN_SIGNATURE_REQUIRED			(43)			/*!<  */
#define CLESS_SAMPLE_VISAWAVE_SCREEN_SIGNATURE_OK				(44)			/*!<  */
#define CLESS_SAMPLE_VISAWAVE_SCREEN_SIGNATURE_KO				(45)			/*!<  */
#define CLESS_SAMPLE_VISAWAVE_SCREEN_ONLINE_APPROVED			(46)			/*!<  */
#define CLESS_SAMPLE_VISAWAVE_SCREEN_ONLINE_DECLINED			(47)			/*!<  */
#define CLESS_SAMPLE_VISAWAVE_SCREEN_ONLINE_PROCESSING			(48)			/*!<  */
#define CLESS_SAMPLE_VISAWAVE_SCREEN_PIN_KO						(49)			/*!<  */
#define CLESS_SAMPLE_VISAWAVE_SCREEN_PROCESSING					(50)			/*!<  */
#define CLESS_SAMPLE_VISAWAVE_SCREEN_ERASE_CUSTOMER				(51)			/*!<  */
#define CLESS_SAMPLE_VISAWAVE_SCREEN_TRY_AGAIN				    (52)			/*!<  */
#define CLESS_SAMPLE_VISAWAVE_SCREEN_TRY_AGAIN_NO_ERASE		    (53)			/*!<  */
#define CLESS_SAMPLE_VISAWAVE_SCREEN_COLLISION                  (54)            /*!<  */
#define CLESS_SAMPLE_VISAWAVE_SCREEN_USE_CONTACT                (55)            /*!<  */


#define CLESS_SAMPLE_PAYPASS_SCREEN_SIGNATURE_REQUIRED			(63)			/*!<  */
#define CLESS_SAMPLE_PAYPASS_SCREEN_OFFLINE_APPROVED_REFUND		(66)			/*!<  */
#define CLESS_SAMPLE_PAYPASS_SCREEN_ERROR_REFUND				(67)			/*!<  */
#define CLESS_SAMPLE_PAYPASS_SCREEN_APPROVED					(68)			/*!<  */
#define CLESS_SAMPLE_PAYPASS_SCREEN_SIGNATURE_OK				(69)			/*!<  */
#define CLESS_SAMPLE_PAYPASS_SCREEN_SIGNATURE_KO				(70)			/*!<  */
#define CLESS_SAMPLE_PAYPASS_SCREEN_AUTHORISING					(71)			/*!<  */
#define CLESS_SAMPLE_PAYPASS_SCREEN_DECLINED					(72)			/*!<  */
#define CLESS_SAMPLE_PAYPASS_SCREEN_REMOVE						(73)			/*!<  */
#define CLESS_SAMPLE_PAYPASS_SCREEN_REMOVE_CARD_DS_OPERATION	(74)			/*!<  */
#define CLESS_SAMPLE_PAYPASS_SCREEN_WAIT_CARD_REMOVAL			(75)			/*!<  */


#define CLESS_SAMPLE_EXPRESSPAY_SCREEN_SIGNATURE_REQUIRED       (80)			/*!<  */
#define CLESS_SAMPLE_EXPRESSPAY_SCREEN_OFFLINE_APPROVED			(81)			/*!<  */
#define CLESS_SAMPLE_EXPRESSPAY_SCREEN_OFFLINE_DECLINED			(82)			/*!<  */
#define CLESS_SAMPLE_EXPRESSPAY_SCREEN_FULL_ONLINE_PROCESSING	(83)			/*!<  */
#define CLESS_SAMPLE_EXPRESSPAY_SCREEN_ONLINE_PROCESSING	    (84)			/*!<  */
#define CLESS_SAMPLE_EXPRESSPAY_SCREEN_ONLINE_APPROVED			(85)			/*!<  */
#define CLESS_SAMPLE_EXPRESSPAY_SCREEN_ONLINE_DECLINED			(86)			/*!<  */
#define CLESS_SAMPLE_EXPRESSPAY_SCREEN_SIGNATURE_OK				(87)			/*!<  */
#define CLESS_SAMPLE_EXPRESSPAY_SCREEN_ONLINE_SIGNATURE_OK      (88)            /*!<  */
#define CLESS_SAMPLE_EXPRESSPAY_SCREEN_SIGNATURE_KO				(89)			/*!<  */
#define CLESS_SAMPLE_EXPRESSPAY_SCREEN_NOT_PERMITTED    		(90)			/*!<  */
#define CLESS_SAMPLE_EXPRESSPAY_SCREEN_PIN_CANCEL        		(91)			/*!<  */
#define CLESS_SAMPLE_EXPRESSPAY_SCREEN_PIN_ERROR        		(92)			/*!<  */

#define CLESS_SAMPLE_VISA_SCREEN_SIGNATURE_ASIA                 (100)			/*!<  */
#define CLESS_SAMPLE_VISA_SCREEN_SIGNATURE                      (101)			/*!<  */
#define CLESS_SAMPLE_VISA_APPROVED_ASIA                         (102)			/*!<  */
#define CLESS_SAMPLE_VISA_OFFLINE_APPROVED						(103)			/*!<  */
#define CLESS_SAMPLE_VISA_ONLINE_APPROVED						(104)			/*!<  */
#define CLESS_SAMPLE_VISA_SIGNATURE_OK						    (105)			/*!<  */
#define CLESS_SAMPLE_VISA_SIGNATURE_OK_ASIA					    (106)			/*!<  */
#define CLESS_SAMPLE_VISA_SIGNATURE_KO							(107)			/*!<  */
#define CLESS_SAMPLE_VISA_SIGNATURE_KO_ASIA						(108)			/*!<  */
#define CLESS_SAMPLE_VISA_PIN_KO								(109)			/*!<  */
#define CLESS_SAMPLE_VISA_PIN_KO_ASIA							(110)			/*!<  */
#define CLESS_SAMPLE_VISA_DECLINED_ASIA							(111)			/*!<  */
#define CLESS_SAMPLE_VISA_ONLINE_DECLINED						(112)			/*!<  */
#define CLESS_SAMPLE_VISA_OFFLINE_DECLINED						(113)			/*!<  */
#define CLESS_SAMPLE_VISA_ONLINE								(114)			/*!<  */
#define CLESS_SAMPLE_VISA_ONLINE_ASIA							(115)			/*!<  */
#define CLESS_SAMPLE_VISA_ERROR									(116)			/*!<  */
#define CLESS_SAMPLE_VISA_ERROR_ASIA							(117)			/*!<  */

#define CLESS_SAMPLE_INTERAC_SCREEN_COLLISION					(121)           /*!<  */
#define CLESS_SAMPLE_INTERAC_SCREEN_NOT_SUPPORTED				(122)           /*!<  */
#define CLESS_SAMPLE_INTERAC_SCREEN_CONTACT						(123)           /*!<  */

#define CLESS_SAMPLE_SCREEN_CARD_NOT_SUPPORTED					(131)           /*!<  */
#define CLESS_SAMPLE_SCREEN_EMPTY_UPP							(132)			/*!<  */


/////////////////////////////////////////////////////////////////////////////////////
//// Types //////////////////////////////////////////////////////////////////////////

typedef struct
{
	int nMerchantLanguage;			/*!< Merchant language. */
	int nCardholderLanguage;		/*!< Cardholder language. */
	int bCardholderLanguageDefined;	/*!< Indicates if the cardholder language had been defined. */
} T_SAMPLE_TRANSACTION_LANGUAGE;


/////////////////////////////////////////////////////////////////////////////////////
//// Global data definitions ////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////
//// Functions //////////////////////////////////////////////////////////////////////

//! \brief Initialise the language structure.

void Cless_GuiState_InitLanguageStructure (void);



//! \brief Get the merchant language.
//! \return The merchant language.

int Cless_GuiState_GetMerchantLanguage (void);



//! \brief Get the cardholder language.
//! \return The cardholder language.

int Cless_GuiState_GetCardholderLanguage (void);



//! \brief Set the application language with the cardholder language if present, else with the merchant language.
//! \param[in] pPreferredLanguage Indicates the preferred language if already extracted.

void Cless_GuiState_SetCardholderLanguage (unsigned char * pPreferredLanguage);



//! \brief This function displays a screen on the display.
//! \param[in] ulScreenIdentifier Screen identifier.

void Cless_GuiState_DisplayScreen (unsigned long ulScreenIdentifier, int nMerchantLang, int nCardholderLang);



//! \brief This function displays a screen on the display.
//! \param[in] ulScreenIdentifier Screen identifier.
//! \param[in] nMerchantLang Merchant language. \a -1 if unknown.
//! \param[in] nCardholderLang Cardholder language. \a -1 if unknown.

void Cless_GuiState_DisplayScreenText (unsigned long ulScreenIdentifier, int nMerchantLang, int nCardholderLang);


#endif
