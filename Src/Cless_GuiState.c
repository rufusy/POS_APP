/**
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
 */


/////////////////////////////////////////////////////////////////
//// Includes ///////////////////////////////////////////////////

#include "Cless_Implementation.h"
extern T_GL_HWIDGET TM_DisplayClessTarget  ( T_GL_HGRAPHIC_LIB  graphiclib );

/////////////////////////////////////////////////////////////////
//// Macros & preprocessor definitions //////////////////////////

#define LENGTH_OF_GUI_STATE_TO_SET			4		/*!< Length of the tag that contains the GUI state to be set */

#ifndef TERM_ISC480
#define TERM_ISC480	0x506
#endif
#ifndef TERM_ICM122
#define TERM_ICM122	0x112
#endif
#ifndef TERM_IWB220
#define TERM_IWB220	0x10E
#endif
#ifndef TERM_IMP550
#define TERM_IMP550	0x11B
#endif
#ifndef TERM_IDE280
#define TERM_IDE280	0x508
#endif

/////////////////////////////////////////////////////////////////
//// Global data definition /////////////////////////////////////

static T_SAMPLE_TRANSACTION_LANGUAGE gs_tTransactionLanguage;


/////////////////////////////////////////////////////////////////
//// Static functions definition ////////////////////////////////

static void __Cless_GuiState_GetDisplayInfoWithLogo (int * pDisplayClessLogoInternal, int * pDisplayClessLogoExternal, int * pInstructionLineMerchant, int * pInstructionLineCardholder, int * pAmountLineMerchant, int * pAmountLineCardholder, int * pFontSizeMerchant, int * pFontSizeCustomer);


/////////////////////////////////////////////////////////////////
//// Functions //////////////////////////////////////////////////

//! \brief Initialise the language structure.

void Cless_GuiState_InitLanguageStructure (void) {
	gs_tTransactionLanguage.nMerchantLanguage = PSQ_Give_Language();
	gs_tTransactionLanguage.nCardholderLanguage = gs_tTransactionLanguage.nMerchantLanguage; // By default, cardholder language is equal to merchant language
	gs_tTransactionLanguage.bCardholderLanguageDefined = FALSE;
}


//! \brief Get the merchant language.
//! \return The merchant language.

int Cless_GuiState_GetMerchantLanguage (void) {
	return (gs_tTransactionLanguage.nMerchantLanguage);
}



//! \brief Get the cardholder language.
//! \return The cardholder language.

int Cless_GuiState_GetCardholderLanguage (void) {
	return (gs_tTransactionLanguage.nCardholderLanguage);
}



//! \brief Set the application language with the cardholder language if present, else with the merchant language.
//! \param[in] pPreferredLanguage Indicates the preferred language if already extracted.

void Cless_GuiState_SetCardholderLanguage (unsigned char * pPreferredLanguage)
{
	int ret;
	T_SHARED_DATA_STRUCT * pStruct = NULL;
	unsigned char * pInfo;
	int nKernelIdentifier;

	// Get the used payment scheme
	nKernelIdentifier = Cless_Customisation_GetUsedPaymentScheme();

	if (nKernelIdentifier != CLESS_SAMPLE_CUST_UNKNOWN)
	{
		if (pPreferredLanguage == NULL)
		{
			// Init a shared buffer to get the prefered selected language
			pStruct = GTL_SharedExchange_InitShared (128);

			if (pStruct == NULL)
			{
				// An error occurred when creating the shared buffer
				GTL_Traces_TraceDebug ("Cless_UserInterface_SetCardholderLanguage : An error occurred when creating the shared buffer");
				goto End;
			}

			// Add tag in the shared buffer to request it
			ret = GTL_SharedExchange_AddTag (pStruct, TAG_KERNEL_SELECTED_PREFERED_LANGUAGE, 0, NULL);

			if (ret != STATUS_SHARED_EXCHANGE_OK)
			{
				// An error occurred when adding the TAG_KERNEL_SELECTED_PREFERED_LANGUAGE tag in the structure
				GTL_Traces_TraceDebug ("Cless_UserInterface_SetCardholderLanguage : An error occurred when adding the TAG_KERNEL_SELECTED_PREFERED_LANGUAGE tag in the structure (ret = %02x)", ret);
				goto End;
			}

			// Request data to the kernel
			switch (nKernelIdentifier)
			{
			case CLESS_SAMPLE_CUST_PAYPASS :
				ret = PayPass3_GetData(pStruct);
				break;
#ifndef DISABLE_OTHERS_KERNELS
			case CLESS_SAMPLE_CUST_PAYWAVE :
				ret = payWave_GetData(pStruct);
				break;

			case CLESS_SAMPLE_CUST_VISAWAVE :
				ret = VisaWave_GetData(pStruct);
				break;

			case CLESS_SAMPLE_CUST_EXPRESSPAY :
				//				ret = ExpressPay_GetData(pStruct);
				break;
#endif
			default:
				ret = KERNEL_STATUS_UNKNOWN;
				GTL_Traces_TraceDebug("Cless_UserInterface_SetCardholderLanguage : Unknown value for kernel identifier parameter : %x", nKernelIdentifier);
				goto End;
				break;
			}

			if (ret != KERNEL_STATUS_OK)
			{
				// An error occurred when getting data from the kernel
				GTL_Traces_TraceDebug ("Cless_UserInterface_SetCardholderLanguage : An error occurred when getting data from the kernel (ret = %02x)", ret);
				goto End;
			}
		}
		else
		{
			pInfo = pPreferredLanguage;
		}

		// Search the tag in the kernel response structure
		if (Cless_Common_RetrieveInfo (pStruct, TAG_KERNEL_SELECTED_PREFERED_LANGUAGE, &pInfo))
		{
			gs_tTransactionLanguage.nCardholderLanguage = Cless_Term_GiveLangNumber(pInfo);
			if (gs_tTransactionLanguage.nCardholderLanguage == -1)
				gs_tTransactionLanguage.nCardholderLanguage = gs_tTransactionLanguage.nMerchantLanguage;
		}

		End:
		// Destroy the shared buffer if created
		if (pStruct != NULL)
			GTL_SharedExchange_DestroyShare (pStruct);
	}
}



//! \brief This function displays a screen on the display.
//! \param[in] ulScreenIdentifier Screen identifier.
//! \param[in] nMerchantLang Merchant language. \a -1 if unknown.
//! \param[in] nCardholderLang Cardholder language. \a -1 if unknown.

void Cless_GuiState_DisplayScreen (unsigned long ulScreenIdentifier, int nMerchantLang, int nCardholderLang) {
	///const T_PAYMENT_DATA_STRUCT * pTransactionData;
	unsigned long ulBeforeDeselectDisplayTime = 0;
	int nTimeout;
	//	int nMerchantLanguage;
	//	int nCardholderLanguage;
	//
	//	if (nMerchantLang == -1)
	//		nMerchantLanguage = gs_tTransactionLanguage.nMerchantLanguage;
	//	else
	//		nMerchantLanguage = nMerchantLang;
	//
	//	if (nCardholderLang == -1)
	//		nCardholderLanguage = gs_tTransactionLanguage.nCardholderLanguage;
	//	else
	//		nCardholderLanguage = nCardholderLang;

	// Get the global transaction data
	///pTransactionData = Cless_Fill_GetTransactionDataStructure();

	// Display the screen
	switch (ulScreenIdentifier)
	{
	/*	case (CLESS_SAMPLE_SCREEN_IDLE):
		break;*/

	case (CLESS_SAMPLE_SCREEN_PRESENT_CARD):
			{
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
		if (!UIH_IsRemoveCardTaskRunning())
			HelperLedsOff();
			}
	break;

	case (CLESS_SAMPLE_SCREEN_RETRY):
			{
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
		if (!UIH_IsRemoveCardTaskRunning())
			HelperLedsOff();
			}
	break;

	case (CLESS_SAMPLE_SCREEN_REMOVE_CARD):
	case (CLESS_SAMPLE_SCREEN_REMOVE_CARD_TWO_LINES):
	case (CLESS_SAMPLE_PAYPASS_SCREEN_REMOVE):
	{
		ulBeforeDeselectDisplayTime = GTL_StdTimer_GetCurrent();

		// Turn on the buzzer
		TPass_Buzzer(C_DEFAULT_BEEP_OK_FREQUENCY, (unsigned char)C_DEFAULT_BEEP_VOLUME);

#ifndef DISABLE_OTHERS_KERNELS
		// Turn on the 2nd LED
		if(!Cless_VisaWave_IsVisaAsiaGuiMode())
#endif
			TPass_LedsOn(TPASS_LED_1 | TPASS_LED_2);

#ifndef DISABLE_OTHERS_KERNELS
		// Display the "Remove card" message
		if(!Cless_VisaWave_IsVisaAsiaGuiMode())
#endif
		{
			Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);

			// Wait before turning on third LED
			nTimeout = GTL_StdTimer_GetRemaining(ulBeforeDeselectDisplayTime, 13);
			if (nTimeout > 0)
				Telium_Ttestall(0, nTimeout);

			// Turn on third LED
			TPass_LedsOn (TPASS_LED_1 | TPASS_LED_2 | TPASS_LED_3);

			// Wait before turning on fourth LED
			nTimeout = GTL_StdTimer_GetRemaining(ulBeforeDeselectDisplayTime, 25);
			if (nTimeout > 0)
				Telium_Ttestall(0, nTimeout);

			// Turn on fourth LED
			TPass_LedsOn (TPASS_LED_1 | TPASS_LED_2 | TPASS_LED_3 | TPASS_LED_4);

			// Wait display time
			nTimeout = GTL_StdTimer_GetRemaining(ulBeforeDeselectDisplayTime, C_DEFAULT_BEEP_OK_DELAY);
			if (nTimeout > 0) {
				Telium_Ttestall(0, nTimeout);
				nTimeout = 0;
			}

			TPass_Buzzer (0, 0);
		}
#ifndef DISABLE_OTHERS_KERNELS
		else {
			Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
			if (!UIH_IsRemoveCardTaskRunning())
				HelperLedsOff();

			TPass_LedsOn (TPASS_LED_GREEN);

			// Wait display time
			nTimeout = GTL_StdTimer_GetRemaining(ulBeforeDeselectDisplayTime, C_DEFAULT_BEEP_OK_DELAY);
			if (nTimeout > 0) {
				Telium_Ttestall(0, nTimeout);
				nTimeout = 0;
			}

			TPass_Buzzer (0, 0);

			// Wait display time
			nTimeout = GTL_StdTimer_GetRemaining(ulBeforeDeselectDisplayTime, 200);
			if (nTimeout > 0) {
				Telium_Ttestall(0, nTimeout);
				nTimeout = 0;
			}
		}
#endif
	}
	break;

	/*	case (CLESS_SAMPLE_SCREEN_TIMEOUT_ELAPSED):
		break;*/

	case (CLESS_SAMPLE_PAYPASS_SCREEN_WAIT_CARD_REMOVAL):
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	if (!UIH_IsRemoveCardTaskRunning())
		HelperLedsOff();
	break;

	case (CLESS_SAMPLE_SCREEN_CANCELLED):
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	if (!UIH_IsRemoveCardTaskRunning())
		HelperLedsOff();
	Helper_WaitUserDisplay();
	break;

	/*	case (CLESS_SAMPLE_SCREEN_PROCESSING):
		break;*/

	case (CLESS_SAMPLE_SCREEN_COLLISION):
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	if (!UIH_IsRemoveCardTaskRunning())
		HelperLedsOff();

	// Error tone
	HelperErrorSequence (WITHBEEP);
	break;

	/*	case (CLESS_SAMPLE_SCREEN_ONLINE_APPROVED):
		break;*/

	/*	case (CLESS_SAMPLE_SCREEN_ONLINE_PIN_REQUIRED):
		break;*/

	/*	case (CLESS_SAMPLE_SCREEN_SIGNATURE_REQUIRED):
		break;*/

	case (CLESS_SAMPLE_SCREEN_ERROR):
		{
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
		if (!UIH_IsRemoveCardTaskRunning())
			HelperLedsOff();
		}
	break;

	case (CLESS_SAMPLE_SCREEN_CARD_BLOCKED):
		{
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
		if (!UIH_IsRemoveCardTaskRunning())
			HelperLedsOff();
		}
	break;

	case (CLESS_SAMPLE_SCREEN_APPLICATION_BLOCKED):
		{
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
		if (!UIH_IsRemoveCardTaskRunning())
			HelperLedsOff();
		}
	break;

	case (CLESS_SAMPLE_SCREEN_OFFLINE_DECLINED):
		{
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
		if (!UIH_IsRemoveCardTaskRunning())
			HelperLedsOff();
		Helper_WaitUserDisplay();
		}
	break;

	case (CLESS_SAMPLE_SCREEN_OFFLINE_APPROVED):
		{
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
		if (!UIH_IsRemoveCardTaskRunning())
			HelperLedsOff();
		Helper_WaitUserDisplay();
		}
	break;

	case (CLESS_SAMPLE_SCREEN_ONLINE_PROCESSING):
		{
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
		if (!UIH_IsRemoveCardTaskRunning())
			HelperLedsOff();
		Helper_WaitUserDisplay();
		}
	break;

	case (CLESS_SAMPLE_SCREEN_ONLINE_APPROVED):
		{
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
		if (!UIH_IsRemoveCardTaskRunning())
			HelperLedsOff();
		Helper_WaitUserDisplay();
		}
	break;

	case (CLESS_SAMPLE_SCREEN_ONLINE_DECLINED):
		{
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
		if (!UIH_IsRemoveCardTaskRunning())
			HelperLedsOff();
		Helper_WaitUserDisplay();
		}
	break;

	case (CLESS_SAMPLE_SCREEN_PIN_CANCEL):
		{
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
		if (!UIH_IsRemoveCardTaskRunning())
			HelperLedsOff();
		Helper_WaitUserDisplay();
		}
	break;

	case (CLESS_SAMPLE_SCREEN_PIN_ERROR):
		{
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
		if (!UIH_IsRemoveCardTaskRunning())
			HelperLedsOff();
		Helper_WaitUserDisplay();
		}
	break;

	case (CLESS_SAMPLE_SCREEN_EMPTY_UPP):
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	break;

	case (CLESS_SAMPLE_SCREEN_ERASE_CUSTOMER):
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	if (!UIH_IsRemoveCardTaskRunning())
		HelperLedsOff();
	break;

	case (CLESS_SAMPLE_SCREEN_USER):
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	Helper_WaitUserDisplay();
	break;

	case (CLESS_SAMPLE_SCREEN_KEYBOARD):
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	Helper_WaitUserDisplay();
	break;

	case (CLESS_SAMPLE_SCREEN_ERROR_STATUS):
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	if (!UIH_IsRemoveCardTaskRunning())
		HelperLedsOff();
	break;

	case (CLESS_SAMPLE_SCREEN_BATCH_ERROR):
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	if (!UIH_IsRemoveCardTaskRunning())
		HelperLedsOff();
	Helper_WaitUserDisplay();
	break;

	case (CLESS_SAMPLE_SCREEN_REPRESENT_CARD):
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	break;

	case (CLESS_SAMPLE_SCREEN_PHONE_INSTRUCTIONS):
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	if (!UIH_IsRemoveCardTaskRunning())
		HelperLedsOff();
	break;

	case (CLESS_SAMPLE_SCREEN_PHONE_INSTRUCTIONS_RETRY):
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	if (!UIH_IsRemoveCardTaskRunning())
		HelperLedsOff();
	break;

	case (CLESS_SAMPLE_SCREEN_CARD_NOT_SUPPORTED):
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	if (!UIH_IsRemoveCardTaskRunning())
		HelperLedsOff();
	break;

	//////////////////////////////
	// PAYWAVE SPECIFIC SCREENS //
	//////////////////////////////

	case (CLESS_SAMPLE_PAYWAVE_SCREEN_STOPPED):
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	break;

	case (CLESS_SAMPLE_PAYWAVE_SCREEN_PIN_CANCEL):
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	if (!UIH_IsRemoveCardTaskRunning())
		HelperLedsOff();
	Helper_WaitUserDisplay();
	break;

	case (CLESS_SAMPLE_PAYWAVE_SCREEN_PIN_ERROR):
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	if (!UIH_IsRemoveCardTaskRunning())
		HelperLedsOff();
	Helper_WaitUserDisplay();
	break;

	///////////////////////////////
	// VISAWAVE SPECIFIC SCREENS //
	///////////////////////////////

	case (CLESS_SAMPLE_VISAWAVE_SCREEN_ERASE_CUSTOMER):
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	break;

	/*	case (CLESS_SAMPLE_VISAWAVE_SCREEN_STATUS_ERROR):
		break;*/

	/*	case (CLESS_SAMPLE_VISAWAVE_SCREEN_OFFLINE_APPROVED):
		break;*/

	/*	case (CLESS_SAMPLE_VISAWAVE_SCREEN_OFFLINE_DECLINED):
		break;*/

	/*	case (CLESS_SAMPLE_VISAWAVE_SCREEN_SIGNATURE_REQUIRED):
		break;*/

	/*	case (CLESS_SAMPLE_VISAWAVE_SCREEN_SIGNATURE_OK):
		break;*/

	/*	case (CLESS_SAMPLE_VISAWAVE_SCREEN_SIGNATURE_KO):
		break;*/

	/*	case (CLESS_SAMPLE_VISAWAVE_SCREEN_ONLINE_APPROVED):
		break;*/

	/*	case (CLESS_SAMPLE_VISAWAVE_SCREEN_ONLINE_DECLINED):
		break;*/

	/*	case (CLESS_SAMPLE_VISAWAVE_SCREEN_ONLINE_PROCESSING):
		break;*/

	/*	case (CLESS_SAMPLE_VISAWAVE_SCREEN_PIN_KO):
		break;*/

	case (CLESS_SAMPLE_VISAWAVE_SCREEN_PROCESSING):
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	if (!UIH_IsRemoveCardTaskRunning())
		HelperLedsOff();
	break;

	case (CLESS_SAMPLE_VISAWAVE_SCREEN_TRY_AGAIN):
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	break;

	case (CLESS_SAMPLE_VISAWAVE_SCREEN_TRY_AGAIN_NO_ERASE):
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	break;

	case (CLESS_SAMPLE_VISAWAVE_SCREEN_COLLISION):
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	break;

	case (CLESS_SAMPLE_VISAWAVE_SCREEN_USE_CONTACT):
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	if (!UIH_IsRemoveCardTaskRunning())
		HelperLedsOff();
	break;

	case (CLESS_SAMPLE_SCREEN_TRY_ANOTHER_CARD):
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	break;

	//////////////////////////////
	// PAYPASS SPECIFIC SCREENS //
	//////////////////////////////

	case (CLESS_SAMPLE_PAYPASS_SCREEN_SIGNATURE_REQUIRED):
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	if (!UIH_IsRemoveCardTaskRunning())
		HelperLedsOff();
	Helper_WaitUserDisplay();
	break;

	case (CLESS_SAMPLE_PAYPASS_SCREEN_SIGNATURE_OK):
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	if (!UIH_IsRemoveCardTaskRunning())
		HelperLedsOff();
	Helper_WaitUserDisplay();
	break;

	case (CLESS_SAMPLE_PAYPASS_SCREEN_SIGNATURE_KO):
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	if (!UIH_IsRemoveCardTaskRunning())
		HelperLedsOff();
	Helper_WaitUserDisplay();
	break;

	case (CLESS_SAMPLE_PAYPASS_SCREEN_DECLINED):
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	if (!UIH_IsRemoveCardTaskRunning())
		HelperLedsOff();
	HelperErrorSequence(WITHBEEP);
	Helper_WaitUserDisplay();
	break;

	case (CLESS_SAMPLE_SCREEN_ONLINE_UNABLE):
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	if (!UIH_IsRemoveCardTaskRunning())
		HelperLedsOff();
	Helper_WaitUserDisplay();
	break;

	case (CLESS_SAMPLE_PAYPASS_SCREEN_AUTHORISING):
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	if (!UIH_IsRemoveCardTaskRunning())
		HelperLedsOff();
	Helper_WaitUserDisplay();
	break;

	case (CLESS_SAMPLE_PAYPASS_SCREEN_APPROVED):
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	if (!UIH_IsRemoveCardTaskRunning())
		HelperLedsOff();
	Helper_WaitUserDisplay();
	break;

	/////////////////////////////////
	// EXPRESSPAY SPECIFIC SCREENS //
	/////////////////////////////////

	case (CLESS_SAMPLE_EXPRESSPAY_SCREEN_SIGNATURE_REQUIRED):
    											Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	if (!UIH_IsRemoveCardTaskRunning())
		HelperLedsOff();
	Helper_WaitUserDisplay();
	break;

	case (CLESS_SAMPLE_EXPRESSPAY_SCREEN_OFFLINE_APPROVED):
    											Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	if (!UIH_IsRemoveCardTaskRunning())
		HelperLedsOff();
	Helper_WaitUserDisplay();
	break;

	case (CLESS_SAMPLE_EXPRESSPAY_SCREEN_ONLINE_APPROVED):
    											Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	if (!UIH_IsRemoveCardTaskRunning())
		HelperLedsOff();
	Helper_WaitUserDisplay();
	break;

	case (CLESS_SAMPLE_EXPRESSPAY_SCREEN_SIGNATURE_OK):
    											Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	if (!UIH_IsRemoveCardTaskRunning())
		HelperLedsOff();
	Helper_WaitUserDisplay();
	break;

	case (CLESS_SAMPLE_EXPRESSPAY_SCREEN_ONLINE_SIGNATURE_OK):
    											Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	if (!UIH_IsRemoveCardTaskRunning())
		HelperLedsOff();
	Helper_WaitUserDisplay();
	break;

	case (CLESS_SAMPLE_EXPRESSPAY_SCREEN_SIGNATURE_KO):
    											Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	if (!UIH_IsRemoveCardTaskRunning())
		HelperLedsOff();
	Helper_WaitUserDisplay();
	break;

	case (CLESS_SAMPLE_EXPRESSPAY_SCREEN_OFFLINE_DECLINED):
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	if (!UIH_IsRemoveCardTaskRunning())
		HelperLedsOff();
	break;

	case (CLESS_SAMPLE_EXPRESSPAY_SCREEN_ONLINE_DECLINED):
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	if (!UIH_IsRemoveCardTaskRunning())
		HelperLedsOff();
	break;

	case (CLESS_SAMPLE_EXPRESSPAY_SCREEN_NOT_PERMITTED):
    											Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	if (!UIH_IsRemoveCardTaskRunning())
		HelperLedsOff();
	HelperErrorSequence (WITHBEEP);
	Helper_WaitUserDisplay();
	break;

	case (CLESS_SAMPLE_EXPRESSPAY_SCREEN_FULL_ONLINE_PROCESSING):
    											Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	break;

	case (CLESS_SAMPLE_EXPRESSPAY_SCREEN_ONLINE_PROCESSING):
    											Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	if (!UIH_IsRemoveCardTaskRunning())
		HelperLedsOff();
	break;

	case (CLESS_SAMPLE_EXPRESSPAY_SCREEN_PIN_CANCEL):
    											Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	if (!UIH_IsRemoveCardTaskRunning())
		HelperLedsOff();
	Helper_WaitUserDisplay();
	break;

	case (CLESS_SAMPLE_EXPRESSPAY_SCREEN_PIN_ERROR):
    											Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	if (!UIH_IsRemoveCardTaskRunning())
		HelperLedsOff();
	Helper_WaitUserDisplay();
	break;

	//////////////////////////////
	// INTERAC SPECIFIC SCREENS //
	//////////////////////////////

	case (CLESS_SAMPLE_INTERAC_SCREEN_COLLISION):
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	break;

	case (CLESS_SAMPLE_INTERAC_SCREEN_NOT_SUPPORTED):
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	break;

	case (CLESS_SAMPLE_INTERAC_SCREEN_CONTACT):
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	break;

	///////////////////////////
	// VISA SPECIFIC SCREENS //
	///////////////////////////

	case (CLESS_SAMPLE_VISA_SCREEN_SIGNATURE_ASIA):
    											Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	break;

	case (CLESS_SAMPLE_VISA_SCREEN_SIGNATURE):
    											Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	if (!UIH_IsRemoveCardTaskRunning())
		HelperLedsOff();
	Helper_WaitUserDisplay();
	break;

	case (CLESS_SAMPLE_VISA_APPROVED_ASIA):
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	break;

	case (CLESS_SAMPLE_VISA_OFFLINE_APPROVED):
	case (CLESS_SAMPLE_VISA_ONLINE_APPROVED):
	Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	if (!UIH_IsRemoveCardTaskRunning())
		HelperLedsOff();

	//		if (TPass_GetDeviceType() == TPASS_CLESS_TYPE_IPP2XX) // Two lines display
	if ((TPass_GetDeviceType() == TPASS_CLESS_TYPE_IPP2XX) && (!Cless_UseGraphicalFunctionOnPPAD()))
		Helper_WaitUserDisplay();
	break;

	case (CLESS_SAMPLE_VISA_SIGNATURE_OK):
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	if (!UIH_IsRemoveCardTaskRunning())
		HelperLedsOff();

	//		if (TPass_GetDeviceType() == TPASS_CLESS_TYPE_IPP2XX) // Two lines display
	if ((TPass_GetDeviceType() == TPASS_CLESS_TYPE_IPP2XX) && (!Cless_UseGraphicalFunctionOnPPAD()))
		Helper_WaitUserDisplay();
	break;

	case (CLESS_SAMPLE_VISA_SIGNATURE_OK_ASIA):
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	break;

	case (CLESS_SAMPLE_VISA_SIGNATURE_KO):
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	if (!UIH_IsRemoveCardTaskRunning())
		HelperLedsOff();

	//		if (TPass_GetDeviceType() == TPASS_CLESS_TYPE_IPP2XX) // Two lines display
	if ((TPass_GetDeviceType() == TPASS_CLESS_TYPE_IPP2XX) && (!Cless_UseGraphicalFunctionOnPPAD()))
		Helper_WaitUserDisplay();
	break;

	case (CLESS_SAMPLE_VISA_SIGNATURE_KO_ASIA):
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	if (!UIH_IsRemoveCardTaskRunning())
		HelperLedsOff();
	break;

	case (CLESS_SAMPLE_VISA_PIN_KO):
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	if (!UIH_IsRemoveCardTaskRunning())
		HelperLedsOff();
	Helper_WaitUserDisplay();
	break;

	case (CLESS_SAMPLE_VISA_PIN_KO_ASIA):
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	if (!UIH_IsRemoveCardTaskRunning())
		HelperLedsOff();
	break;

	case (CLESS_SAMPLE_VISA_DECLINED_ASIA):
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	if (!UIH_IsRemoveCardTaskRunning())
		HelperLedsOff();
	break;

	case (CLESS_SAMPLE_VISA_OFFLINE_DECLINED):
	case (CLESS_SAMPLE_VISA_ONLINE_DECLINED):
	Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	if (!UIH_IsRemoveCardTaskRunning())
		HelperLedsOff();

	//		if (TPass_GetDeviceType() == TPASS_CLESS_TYPE_IPP2XX) // Two lines display
	if ((TPass_GetDeviceType() == TPASS_CLESS_TYPE_IPP2XX) && (!Cless_UseGraphicalFunctionOnPPAD()))
		Helper_WaitUserDisplay();
	break;

	case (CLESS_SAMPLE_VISA_ONLINE):
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	if (!UIH_IsRemoveCardTaskRunning())
		HelperLedsOff();

	//		if (TPass_GetDeviceType() == TPASS_CLESS_TYPE_IPP2XX) // Two lines display
	if ((TPass_GetDeviceType() == TPASS_CLESS_TYPE_IPP2XX) && (!Cless_UseGraphicalFunctionOnPPAD()))
		Helper_WaitUserDisplay();
	break;

	case (CLESS_SAMPLE_VISA_ONLINE_ASIA):
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	break;

	case (CLESS_SAMPLE_VISA_ERROR):
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	if (!UIH_IsRemoveCardTaskRunning())
		HelperLedsOff();
	break;

	case (CLESS_SAMPLE_VISA_ERROR_ASIA):
		Cless_GuiState_DisplayScreenText (ulScreenIdentifier, nMerchantLang, nCardholderLang);
	if (!UIH_IsRemoveCardTaskRunning())
		HelperLedsOff();
	break;

	default:
		// Unexpected screen identifier
		GTL_Traces_TraceDebug ("Cless_GuiState_DisplayScreen : Unexpected screen identifier (%02lx)", ulScreenIdentifier);
		break;
	}
}



//! \brief This function displays a screen on the display.
//! \param[in] ulScreenIdentifier Screen identifier.
//! \param[in] nMerchantLang Merchant language. \a -1 if unknown.
//! \param[in] nCardholderLang Cardholder language. \a -1 if unknown.

void Cless_GuiState_DisplayScreenText (unsigned long ulScreenIdentifier, int nMerchantLang, int nCardholderLang) {
	///const T_PAYMENT_DATA_STRUCT * pTransactionData;
	MSGinfos tDisplayMsg;
	MSGinfos tMsg;
	int auCustomerDisplayAvailable;
	unsigned char ucFormattedAmountMessage[32];
	unsigned char ucFormat, ucPosition;
	int nPeriph;
	int nMerchantLanguage;
	int nCardholderLanguage;
	int bDisplayClessLogoInternal = FALSE;
	int bDisplayClessLogoExternal = FALSE;
	int nInstructionLineMerchant = -1;
	int nInstructionLineCardholder = -1;
	int nAmountLineMerchant = -1;
	int nAmountLineCardholder = -1;
	int bAmountCanBeDisplayed = FALSE;
	int nFontSizeMerchant = HELPERS_FONT_SIZE_DEFAULT;
	int nFontSizeCustomer = HELPERS_FONT_SIZE_DEFAULT;
	int nIndex;
	Telium_File_t* hDisplay = NULL;
	static T_GL_HWIDGET goalTarget = NULL;

	//TODO: proprement
	ucFormat = 0;
	ucPosition = 0;
	(void)nPeriph;

	// Init the buffer
	memset (ucFormattedAmountMessage, '\0', sizeof (ucFormattedAmountMessage));

	if (nMerchantLang == -1)
		nMerchantLanguage = gs_tTransactionLanguage.nMerchantLanguage;
	else
		nMerchantLanguage = nMerchantLang;

	if (nCardholderLang == -1)
		nCardholderLanguage = gs_tTransactionLanguage.nCardholderLanguage;
	else
		nCardholderLanguage = nCardholderLang;

	if (goalTarget) {
		GL_Widget_Destroy(goalTarget);
		goalTarget = NULL;
	}

	// Get the global transaction data
	///pTransactionData = Cless_Fill_GetTransactionDataStructure();

	// Check if a cardholder device is available
	auCustomerDisplayAvailable = Helper_IsClessCustomerDisplayAvailable();

	// Get the cless peripheral
#ifndef __TELIUM3__
	nPeriph = TPass_GetCustomerDisplay(0, 0);
#else
//	nPeriph = PERIPH_DISPLAY;
#endif

	// Display the screen
	switch (ulScreenIdentifier) {
	/*	case (CLESS_SAMPLE_SCREEN_IDLE):
		Helper_DisplayMessage(STD_MESS_WELCOME, ERASE, HELPERS_CUSTOMER_LINE_2, HELPERS_ALIGN_CENTER, NOLEDSOFF);
		//Helper_WaitUserDisplay();
		break;*/

	case (CLESS_SAMPLE_SCREEN_PRESENT_CARD):
				{
		// Get the information for displaying the screen
		__Cless_GuiState_GetDisplayInfoWithLogo (&bDisplayClessLogoInternal, &bDisplayClessLogoExternal, &nInstructionLineMerchant, &nInstructionLineCardholder, &nAmountLineMerchant, &nAmountLineCardholder, &nFontSizeMerchant, &nFontSizeCustomer);

		// Determine if amount can be displayed of not
		///bAmountCanBeDisplayed = ((pTransactionData->bIsAmountAuthPresent == SAMPLE_TAG_PRESENT) &&
		///						 (pTransactionData->bIsCurrencyCodePresent == SAMPLE_TAG_PRESENT) &&
		///						 (pTransactionData->bIsCurrencyExpPresent == SAMPLE_TAG_PRESENT));

		bAmountCanBeDisplayed = (CS_GlobalTransactionData_IsTagPresentAndNotEmpty(TAG_EMV_AMOUNT_AUTH_NUM) &&
				CS_GlobalTransactionData_IsTagPresentAndNotEmpty(TAG_EMV_TRANSACTION_CURRENCY_CODE) &&
				CS_GlobalTransactionData_IsTagPresentAndNotEmpty(TAG_EMV_TRANSACTION_CURRENCY_EXPONENT));



		// If all the information is available, format the transaction amount
		if (bAmountCanBeDisplayed) {
			unsigned long ulAmountTemp;

			// Format the message that contains the amount, the currency according to ucFormat, ucPosition
			if (CS_GlobalTransactionData_GetAmountAuthorized(&ulAmountTemp)) {
				char * pCurrencyLabel = NULL;

				if (CS_GlobalTransactionData_GetCurrencyLabel(&pCurrencyLabel)) {
					// Format the message that contains the amount, the currency according to ucFormat, ucPosition
					memset (ucFormattedAmountMessage, 0, sizeof(ucFormattedAmountMessage));
					///Cless_Parameters_GetCurrencyFormatFromParameters(Cless_Fill_GiveMoneyLabel(), &ucFormat, &ucPosition);
					Cless_Parameters_GetCurrencyFormatFromParameters(pCurrencyLabel, &ucFormat, &ucPosition);
					///Cless_Common_FormatAmount(ucFormat, Cless_Fill_GiveMoneyLabel(), ucPosition, pTransactionData->ulTransactionAmount, (char*)ucFormattedAmountMessage);
					Cless_Common_FormatAmount(ucFormat, pCurrencyLabel, ucPosition, ulAmountTemp, (char*)ucFormattedAmountMessage);
					InitMSGinfos_message(&tDisplayMsg, (char*)ucFormattedAmountMessage);
				}
			}
		}

		if (auCustomerDisplayAvailable) { // If a customer display is available

			if (nInstructionLineMerchant != -1) {
				Cless_Term_Read_Message(STD_MESS_PRESENT_CARD, nMerchantLanguage, &tMsg);
				_Helper_DisplayTextMerchant (ERASE, nInstructionLineMerchant, &tMsg, NOLEDSOFF, SHORTLINE, nFontSizeMerchant);
			}

			if ((bAmountCanBeDisplayed) && (nAmountLineMerchant != -1))
				_Helper_DisplayTextMerchant(NO_ERASE, nAmountLineMerchant, &tDisplayMsg, NOLEDSOFF, SHORTLINE, nFontSizeMerchant);

			Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
		}

		// Get the STD_MESS_PRESENT_CARD message and display it
		if (nInstructionLineCardholder != -1) {
			Cless_Term_Read_Message(STD_MESS_PRESENT_CARD, nCardholderLanguage, &tMsg);
			_Helper_DisplayTextCustomer(ERASE, nInstructionLineCardholder, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF, SHORTLINE, nFontSizeCustomer);
		}

		if ((bAmountCanBeDisplayed) && (nAmountLineCardholder != -1))
			_Helper_DisplayTextCustomer(NO_ERASE, nAmountLineCardholder, &tDisplayMsg, Cless_ALIGN_CENTER, NOLEDSOFF, SHORTLINE, nFontSizeCustomer);

		if (Cless_Goal_IsAvailable()) {
			if ((bDisplayClessLogoInternal) && (nInstructionLineCardholder > nAmountLineCardholder)) {
				for (nIndex=(nAmountLineCardholder+1); nIndex<nInstructionLineCardholder; nIndex++)
					Cless_Goal_SetBlankLine (nIndex, NOCLEAR);
			}
		}

		Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);

		// Display the contactless symbol on the terminal if configured to support: Contactless Internal
		if (bDisplayClessLogoInternal) {
			if (!Cless_Goal_IsAvailable())
				DisplayTargetCless(PERIPH_DISPLAY);
			else
				goalTarget = TM_DisplayClessTarget(Cless_Goal_Handle());
		}

		if (bDisplayClessLogoExternal) {
			// check if external display is local
			if (TM_GetTerminalType () == TERM_IDE280) {
				hDisplay = Telium_Fopen ("DISPLAY", "a");
				InitContexteGraphique(PERIPH_DISPLAY_ECR);
				DisplayTargetCless(PERIPH_DISPLAY_ECR);
				InitContexteGraphique (PERIPH_DISPLAY);
				Telium_Fclose (hDisplay);
			} else {
				hDisplay = Telium_Fopen ("DISPLAY", "a");
				InitContexteGraphique (PERIPH_PPR);
				DisplayTargetCless(PERIPH_PPR);
				InitContexteGraphique (PERIPH_DISPLAY);
				Telium_Fclose (hDisplay);
			}
		}
				}
	break;

	case (CLESS_SAMPLE_SCREEN_RETRY):
		// Get the information for displaying the screen
		__Cless_GuiState_GetDisplayInfoWithLogo (&bDisplayClessLogoInternal, &bDisplayClessLogoExternal, &nInstructionLineMerchant, &nInstructionLineCardholder, &nAmountLineMerchant, &nAmountLineCardholder, &nFontSizeMerchant, &nFontSizeCustomer);

	// Determine if amount can be displayed of not
	///bAmountCanBeDisplayed = ((pTransactionData->bIsAmountAuthPresent == SAMPLE_TAG_PRESENT) &&
	///						 (pTransactionData->bIsCurrencyCodePresent == SAMPLE_TAG_PRESENT) &&
	///						 (pTransactionData->bIsCurrencyExpPresent == SAMPLE_TAG_PRESENT));

	bAmountCanBeDisplayed = (CS_GlobalTransactionData_IsTagPresentAndNotEmpty(TAG_EMV_AMOUNT_AUTH_NUM) &&
			CS_GlobalTransactionData_IsTagPresentAndNotEmpty(TAG_EMV_TRANSACTION_CURRENCY_CODE) &&
			CS_GlobalTransactionData_IsTagPresentAndNotEmpty(TAG_EMV_TRANSACTION_CURRENCY_EXPONENT));

	if (bAmountCanBeDisplayed) {
		unsigned long ulAmountTemp;

		if (CS_GlobalTransactionData_GetAmountAuthorized(&ulAmountTemp)) {
			char * pCurrencyLabel = NULL;

			if (CS_GlobalTransactionData_GetCurrencyLabel(&pCurrencyLabel)) {
				// Format the message that contains the amount, the currency according to ucFormat, ucPosition
				memset (ucFormattedAmountMessage, 0, sizeof(ucFormattedAmountMessage));
				///Cless_Parameters_GetCurrencyFormatFromParameters(Cless_Fill_GiveMoneyLabel(), &ucFormat, &ucPosition);
				Cless_Parameters_GetCurrencyFormatFromParameters(pCurrencyLabel, &ucFormat, &ucPosition);
				///Cless_Common_FormatAmount(ucFormat, Cless_Fill_GiveMoneyLabel(), ucPosition, pTransactionData->ulTransactionAmount, (char*)ucFormattedAmountMessage);
				Cless_Common_FormatAmount(ucFormat, pCurrencyLabel, ucPosition, ulAmountTemp, (char*)ucFormattedAmountMessage);
				InitMSGinfos_message(&tDisplayMsg, (char*)ucFormattedAmountMessage);
			}
		}
	}

	if (auCustomerDisplayAvailable) // If a customer display is available
	{
		if (nInstructionLineMerchant != -1) {
			Cless_Term_Read_Message(STD_MESS_TRY_AGAIN, nMerchantLanguage, &tMsg);
			_Helper_DisplayTextMerchant (ERASE, nInstructionLineMerchant, &tMsg, NOLEDSOFF, SHORTLINE, nFontSizeMerchant);
		}

		if ((bAmountCanBeDisplayed) && (nAmountLineMerchant != -1))
			_Helper_DisplayTextMerchant(NO_ERASE, nAmountLineMerchant, &tDisplayMsg, NOLEDSOFF, SHORTLINE, nFontSizeMerchant);

		Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
	}

	// Get the STD_MESS_PRESENT_CARD message and display it
	if (nInstructionLineCardholder != -1) {
		Cless_Term_Read_Message(STD_MESS_TRY_AGAIN, nCardholderLanguage, &tMsg);
		_Helper_DisplayTextCustomer(ERASE, nInstructionLineCardholder, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF, SHORTLINE, nFontSizeCustomer);
	}

	if ((bAmountCanBeDisplayed) && (nAmountLineCardholder != -1))
		_Helper_DisplayTextCustomer(NO_ERASE, nAmountLineCardholder, &tDisplayMsg, Cless_ALIGN_CENTER, NOLEDSOFF, SHORTLINE, nFontSizeCustomer);

	if (Cless_Goal_IsAvailable()) {
		if ((bDisplayClessLogoInternal) && (nInstructionLineCardholder > nAmountLineCardholder))
		{
			for (nIndex=(nAmountLineCardholder+1); nIndex<nInstructionLineCardholder; nIndex++)
				Cless_Goal_SetBlankLine (nIndex, NOCLEAR);
		}
	}

	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);

	// Display the contactless symbol on the terminal if configured to support: Contactless Internal
	if (bDisplayClessLogoInternal) {
		if (!Cless_Goal_IsAvailable())
			DisplayTargetCless(PERIPH_DISPLAY);
		else
			goalTarget = TM_DisplayClessTarget(Cless_Goal_Handle());
	}

	if (bDisplayClessLogoExternal) {
		// check if external display is local
		if (TM_GetTerminalType () == TERM_IDE280) {
			hDisplay = Telium_Fopen ("DISPLAY", "a");
			InitContexteGraphique(PERIPH_DISPLAY_ECR);
			DisplayTargetCless(PERIPH_DISPLAY_ECR);
			InitContexteGraphique (PERIPH_DISPLAY);
			Telium_Fclose (hDisplay);
		} else {
			hDisplay = Telium_Fopen ("DISPLAY", "a");
			InitContexteGraphique (PERIPH_PPR);
			DisplayTargetCless(PERIPH_PPR);
			InitContexteGraphique (PERIPH_DISPLAY);
			Telium_Fclose (hDisplay);
		}
	}
	break;

	case (CLESS_SAMPLE_SCREEN_USE_CONTACT):
		if (auCustomerDisplayAvailable) // If a customer display is available
		{
			Cless_Term_Read_Message(STD_MESS_INSERT_CARD, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
			Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
		}

	Cless_Term_Read_Message(STD_MESS_INSERT_CARD, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	case (CLESS_SAMPLE_SCREEN_REMOVE_CARD):
	case (CLESS_SAMPLE_SCREEN_REMOVE_CARD_TWO_LINES):
	case (CLESS_SAMPLE_PAYPASS_SCREEN_REMOVE):
	// Display the "Remove card" message
#ifndef DISABLE_OTHERS_KERNELS
	if(!Cless_VisaWave_IsVisaAsiaGuiMode())
#endif
	{
		if (auCustomerDisplayAvailable) // If a customer display is available
		{
			if (ulScreenIdentifier == CLESS_SAMPLE_SCREEN_REMOVE_CARD) {
				Cless_Term_Read_Message(STD_MESS_REMOVE_CARD, nMerchantLanguage, &tMsg);
				Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
			} else // (ulScreenIdentifier == CLESS_SAMPLE_SCREEN_REMOVE_CARD_TWO_LINES)
			{
				Cless_Term_Read_Message(STD_MESS_PLEASE_REMOVE, nMerchantLanguage, &tMsg);
				Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_2, &tMsg, NOLEDSOFF);
				Cless_Term_Read_Message(STD_MESS_CARD_DEVICE, nMerchantLanguage, &tMsg);
				Helper_DisplayTextMerchant(NO_ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
			}
			Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
		}

		if (ulScreenIdentifier == CLESS_SAMPLE_SCREEN_REMOVE_CARD) {
			// Display the customer message
			Cless_Term_Read_Message(STD_MESS_REMOVE_CARD, nCardholderLanguage, &tMsg);
			Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
		} else // (ulScreenIdentifier == CLESS_SAMPLE_SCREEN_REMOVE_CARD_TWO_LINES)
		{
			// Display the customer message
			Cless_Term_Read_Message(STD_MESS_PLEASE_REMOVE, nCardholderLanguage, &tMsg);
			Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
			Cless_Term_Read_Message(STD_MESS_CARD_DEVICE, nCardholderLanguage, &tMsg);
			Helper_DisplayTextCustomer (NO_ERASE, HELPERS_CUSTOMER_LINE_3, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
		}
		Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	}
#ifndef DISABLE_OTHERS_KERNELS
	else
	{
		if (ulScreenIdentifier != CLESS_SAMPLE_PAYPASS_SCREEN_REMOVE) {
			if (auCustomerDisplayAvailable) // If a customer display is available
			{
				Cless_Term_Read_Message(VISAWAVE_MESS_REMOVE_CARD, nMerchantLanguage, &tMsg);
				Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_2, &tMsg, NOLEDSOFF);
				Cless_Term_Read_Message(STD_MESS_PLEASE_WAIT, nMerchantLanguage, &tMsg);
				Helper_DisplayTextMerchant(NO_ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
				Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
			}

			// Display the customer message
			Cless_Term_Read_Message(VISAWAVE_MESS_REMOVE_CARD, nCardholderLanguage, &tMsg);
			Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
			Cless_Term_Read_Message(STD_MESS_PLEASE_WAIT, nCardholderLanguage, &tMsg);
			Helper_DisplayTextCustomer (NO_ERASE, HELPERS_CUSTOMER_LINE_3, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
			Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
		} else // if (ulScreenIdentifier == CLESS_SAMPLE_PAYPASS_SCREEN_REMOVE)
		{
			if (auCustomerDisplayAvailable) // If a customer display is available
			{
				Cless_Term_Read_Message(PAYPASS_MESS_REMOVE, nMerchantLanguage, &tMsg);
				Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_2, &tMsg, NOLEDSOFF);
				Cless_Term_Read_Message(PAYPASS_MESS_REMOVE_L2, nMerchantLanguage, &tMsg);
				Helper_DisplayTextMerchant(NO_ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
				Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
			}

			// Display the customer message
			Cless_Term_Read_Message(PAYPASS_MESS_REMOVE, nCardholderLanguage, &tMsg);
			Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
			Cless_Term_Read_Message(PAYPASS_MESS_REMOVE_L2, nCardholderLanguage, &tMsg);
			Helper_DisplayTextCustomer (NO_ERASE, HELPERS_CUSTOMER_LINE_3, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
			Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
		}
	}
#endif
	break;

	/*	case (CLESS_SAMPLE_SCREEN_TIMEOUT_ELAPSED):
		break;*/

	case (CLESS_SAMPLE_PAYPASS_SCREEN_WAIT_CARD_REMOVAL):
		if (auCustomerDisplayAvailable) // If a customer display is available
		{
			Cless_Term_Read_Message(STD_MESS_REMOVE_CARD, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_2, &tMsg, NOLEDSOFF);
			Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
		}

	// Display the customer message
	Cless_Term_Read_Message(STD_MESS_REMOVE_CARD, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	case (CLESS_SAMPLE_SCREEN_CANCELLED):
		if (auCustomerDisplayAvailable) // If a customer display is available
		{
			Cless_Term_Read_Message(STD_MESS_CANCEL, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_2, &tMsg, NOLEDSOFF);
			Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
		}

	// Display the customer message
	Cless_Term_Read_Message(STD_MESS_CANCEL, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;
	/*case (CLESS_SAMPLE_SCREEN_PROCESSING):
		break;*/

	case (CLESS_SAMPLE_SCREEN_COLLISION):
		if (auCustomerDisplayAvailable) // If a customer display is available
		{
			Cless_Term_Read_Message(STD_MESS_COLLISION_LINE_1, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_2, &tMsg, NOLEDSOFF);
			Cless_Term_Read_Message(STD_MESS_COLLISION_LINE_2, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(NO_ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
			Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
		}

	// Display the customer message
	Cless_Term_Read_Message(STD_MESS_COLLISION_LINE_1, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Cless_Term_Read_Message(STD_MESS_COLLISION_LINE_2, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer (NO_ERASE, HELPERS_CUSTOMER_LINE_3, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	case (CLESS_SAMPLE_SCREEN_ERROR):
		if (auCustomerDisplayAvailable) // If a customer display is available
		{
			Cless_Term_Read_Message(STD_MESS_ERROR, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
			Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
		}

	Cless_Term_Read_Message(STD_MESS_ERROR, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	case (CLESS_SAMPLE_SCREEN_CARD_BLOCKED):
		if (auCustomerDisplayAvailable) // If a customer display is available
		{
			Cless_Term_Read_Message(STD_MESS_CARD_BLOCKED, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
			Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
		}

	Cless_Term_Read_Message(STD_MESS_CARD_BLOCKED, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	case (CLESS_SAMPLE_SCREEN_APPLICATION_BLOCKED):
		if (auCustomerDisplayAvailable) // If a customer display is available
		{
			Cless_Term_Read_Message(STD_MESS_APPLICATION_BLOCKED, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
			Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
		}

	Cless_Term_Read_Message(STD_MESS_APPLICATION_BLOCKED, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	case (CLESS_SAMPLE_SCREEN_OFFLINE_DECLINED):
		if (auCustomerDisplayAvailable) // If a customer display is available
		{
			Cless_Term_Read_Message(STD_MESS_OFFLINE_DECLINED, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
			Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
		}

	Cless_Term_Read_Message(STD_MESS_OFFLINE_DECLINED, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	case (CLESS_SAMPLE_SCREEN_OFFLINE_APPROVED):
		if (auCustomerDisplayAvailable) // If a customer display is available
		{
			Cless_Term_Read_Message(STD_MESS_OFFLINE_APPROVED, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
			Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
		}

	Cless_Term_Read_Message(STD_MESS_OFFLINE_APPROVED, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	case (CLESS_SAMPLE_SCREEN_ONLINE_PROCESSING):
		if (auCustomerDisplayAvailable) // If a customer display is available
		{
			Cless_Term_Read_Message(STD_MESS_ONLINE_REQUEST, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
			Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
		}

	Cless_Term_Read_Message(STD_MESS_ONLINE_REQUEST, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	case (CLESS_SAMPLE_SCREEN_ONLINE_APPROVED):
		if (auCustomerDisplayAvailable) // If a customer display is available
		{
			Cless_Term_Read_Message(STD_MESS_ONLINE_APPROVED, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
			Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
		}

	Cless_Term_Read_Message(STD_MESS_ONLINE_APPROVED, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	case (CLESS_SAMPLE_SCREEN_ONLINE_DECLINED):
		if (auCustomerDisplayAvailable) // If a customer display is available
		{
			Cless_Term_Read_Message(STD_MESS_ONLINE_DECLINED, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
			Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
		}

	Cless_Term_Read_Message(STD_MESS_ONLINE_DECLINED, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	case (CLESS_SAMPLE_SCREEN_PIN_CANCEL):
		if (auCustomerDisplayAvailable) // If a customer display is available
		{
			Cless_Term_Read_Message(STD_MESS_PIN_CANCEL, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
			Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
		}

#ifndef DISABLE_UNATTENDED
	if (Cless_Unattended_IsUnattendedMechanism())
	{
		if (Cless_Unattended_IsPinPadUPPwithUCMC())
		{
			Cless_Term_Read_Message(STD_MESS_PIN_CANCEL, nMerchantLanguage, &tMsg);
			Helper_DisplayPinText(ERASE, HELPERS_MERCHANT_LINE_3,&tMsg,Cless_ALIGN_CENTER,NOLEDSOFF);
		}
	}
#endif
	Cless_Term_Read_Message(STD_MESS_OFFLINE_DECLINED, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	case (CLESS_SAMPLE_SCREEN_PIN_ERROR):
		if (auCustomerDisplayAvailable) // If a customer display is available
		{
			Cless_Term_Read_Message(STD_MESS_ONLINE_PIN_ERROR, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
			Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
		}

#ifndef DISABLE_UNATTENDED
	if (Cless_Unattended_IsUnattendedMechanism())
	{
		if (Cless_Unattended_IsPinPadUPPwithUCMC())
		{
			Cless_Term_Read_Message(STD_MESS_ONLINE_PIN_ERROR, nCardholderLanguage, &tMsg);
			Helper_DisplayPinText(ERASE, HELPERS_MERCHANT_LINE_3,&tMsg,Cless_ALIGN_CENTER,NOLEDSOFF);
		}
	}
#endif

	Cless_Term_Read_Message(STD_MESS_OFFLINE_DECLINED, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	case (CLESS_SAMPLE_SCREEN_EMPTY_UPP):
#ifndef DISABLE_UNATTENDED
		if (Cless_Unattended_IsUnattendedMechanism())
		{
			if (Cless_Unattended_IsPinPadUPPwithUCMC())
			{
				Cless_Term_Read_Message(STD_MESS_EMPTY, nCardholderLanguage, &tMsg);
				Helper_DisplayPinText(ERASE, HELPERS_MERCHANT_LINE_3,&tMsg,Cless_ALIGN_CENTER,NOLEDSOFF);
			}
		}
#endif
	break;

	case (CLESS_SAMPLE_SCREEN_ERASE_CUSTOMER):
							{
		char aucDummyMsg[] = "";
		MSGinfos tDisplayMsg;

		// Display dummy message to erase display
		tDisplayMsg.message = aucDummyMsg;
		tDisplayMsg.coding = _ISO8859_;
		tDisplayMsg.file = GetCurrentFont();
		Helper_DisplayTextCustomer(ERASE, HELPERS_CUSTOMER_LINE_1, &tDisplayMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
		Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
							}
	break;

	case (CLESS_SAMPLE_SCREEN_USER):
		Cless_Term_Read_Message(STD_MESS_USER, nMerchantLanguage, &tMsg);
	Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
	break;

	case (CLESS_SAMPLE_SCREEN_KEYBOARD):
		Cless_Term_Read_Message(STD_MESS_KEYBOARD, nMerchantLanguage, &tMsg);
	Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
	break;

	case (CLESS_SAMPLE_SCREEN_ERROR_STATUS):
		if (auCustomerDisplayAvailable) // If a customer display is available
		{
			Cless_Term_Read_Message(STD_MESS_ERROR_STATUS, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
			Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
		}

	Cless_Term_Read_Message(STD_MESS_ERROR_STATUS, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(ERASE, HELPERS_CUSTOMER_LINE_1, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	case (CLESS_SAMPLE_SCREEN_BATCH_ERROR):
		Cless_Term_Read_Message(STD_MESS_BATCH_ERROR, nMerchantLanguage, &tMsg);
	Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
	break;

	case (CLESS_SAMPLE_SCREEN_REPRESENT_CARD):
		// Display the customer message
		Cless_Term_Read_Message(STD_MESS_REPRESENT_CARD, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_1, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	case (CLESS_SAMPLE_SCREEN_PHONE_INSTRUCTIONS):
		if (auCustomerDisplayAvailable) // If a customer display is available
		{
			Cless_Term_Read_Message(VISAWAVE_MESS_PHONE_INSTRUCTIONS_L1, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_2, &tMsg, NOLEDSOFF);
			Cless_Term_Read_Message(VISAWAVE_MESS_PHONE_INSTRUCTIONS_L2, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(NO_ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
			Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
		}
	Cless_Term_Read_Message(VISAWAVE_MESS_PHONE_INSTRUCTIONS_L1, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(ERASE, HELPERS_CUSTOMER_LINE_1, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Cless_Term_Read_Message(VISAWAVE_MESS_PHONE_INSTRUCTIONS_L2, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(NO_ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_DisplayBlankLineCustomer (NO_ERASE, HELPERS_CUSTOMER_LINE_4);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	case (CLESS_SAMPLE_SCREEN_PHONE_INSTRUCTIONS_RETRY):
		if (auCustomerDisplayAvailable) // If a customer display is available
		{
			Cless_Term_Read_Message(VISAWAVE_MESS_PHONE_INSTRUCTIONS_L1, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_2, &tMsg, NOLEDSOFF);
			Cless_Term_Read_Message(VISAWAVE_MESS_PHONE_INSTRUCTIONS_L2, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(NO_ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
			Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
		}

	Cless_Term_Read_Message(VISAWAVE_MESS_PHONE_INSTRUCTIONS_L1, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(ERASE, HELPERS_CUSTOMER_LINE_1, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Cless_Term_Read_Message(VISAWAVE_MESS_PHONE_INSTRUCTIONS_L2, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(NO_ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Cless_Term_Read_Message(VISAWAVE_MESS_TRY_AGAIN, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(NO_ERASE, HELPERS_CUSTOMER_LINE_4, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;



	case (CLESS_SAMPLE_SCREEN_CARD_NOT_SUPPORTED):
		if (auCustomerDisplayAvailable) // If a customer display is available
		{
			Cless_Term_Read_Message(STD_MESS_CARD_NOT_SUPPORTED, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
			Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
		}

	Cless_Term_Read_Message(STD_MESS_CARD_NOT_SUPPORTED, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	//////////////////////////////
	// PAYWAVE SPECIFIC SCREENS //
	//////////////////////////////



	case (CLESS_SAMPLE_PAYWAVE_SCREEN_STOPPED):
		Cless_Term_Read_Message(STD_MESS_STOPPED, nMerchantLanguage, &tMsg);
	Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
	break;

	case (CLESS_SAMPLE_PAYWAVE_SCREEN_PIN_CANCEL):
		if (auCustomerDisplayAvailable) // If a customer display is available
		{
			Cless_Term_Read_Message(STD_MESS_PIN_CANCEL, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
			Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
		}

	Cless_Term_Read_Message(STD_MESS_PIN_CANCEL, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(ERASE, HELPERS_CUSTOMER_LINE_1, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	case (CLESS_SAMPLE_PAYWAVE_SCREEN_PIN_ERROR):
		if (auCustomerDisplayAvailable) // If a customer display is available
		{
			Cless_Term_Read_Message(STD_MESS_PIN_ERROR, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
			Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
		}

	Cless_Term_Read_Message(STD_MESS_PIN_ERROR, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(ERASE, HELPERS_CUSTOMER_LINE_1, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;


	///////////////////////////////
	// VISAWAVE SPECIFIC SCREENS //
	///////////////////////////////

	case (CLESS_SAMPLE_VISAWAVE_SCREEN_ERASE_CUSTOMER):
							{
		char aucDummyMsg[] = "";
		MSGinfos tDisplayMsg;

		// Display dummy message to erase display
		tDisplayMsg.message = aucDummyMsg;
		tDisplayMsg.coding = _ISO8859_;
		tDisplayMsg.file = GetCurrentFont();
		Helper_DisplayTextCustomer(ERASE, HELPERS_CUSTOMER_LINE_1, &tDisplayMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
		Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
							}
	break;

	/*
	case (CLESS_SAMPLE_VISAWAVE_SCREEN_STATUS_ERROR):
		SDSA_UserInterface_SetCardholderLanguage(SDSA_KERNEL_IDENTIFIER_VISAWAVE);
		Helper_DisplayMessage(STD_MESS_ERROR_STATUS, ERASE, HELPERS_CUSTOMER_LINE_1, HELPERS_ALIGN_CENTER, NOLEDSOFF);
		if(!Cless_VisaWave_IsVisaAsiaGuiMode())
		{
			HelperErrorSequence (WITHBEEP);
			Helper_WaitUserDisplay();
		}
		else
		{
			Helper_PerformKOSequence();
		}
		break;

	case (CLESS_SAMPLE_VISAWAVE_SCREEN_OFFLINE_APPROVED):
		SDSA_UserInterface_SetCardholderLanguage(SDSA_KERNEL_IDENTIFIER_VISAWAVE);
		Helper_Visa_ApprovedIHM(HELPERS_VISAWAVE, OFFLINE);
		break;

	case (CLESS_SAMPLE_VISAWAVE_SCREEN_OFFLINE_DECLINED):
		SDSA_UserInterface_SetCardholderLanguage(SDSA_KERNEL_IDENTIFIER_VISAWAVE);
		Helper_Visa_DeclinedIHM(HELPERS_VISAWAVE, OFFLINE);
		break;

	case (CLESS_SAMPLE_VISAWAVE_SCREEN_SIGNATURE_REQUIRED):
		SDSA_UserInterface_SetCardholderLanguage(SDSA_KERNEL_IDENTIFIER_VISAWAVE);
		Helper_Visa_SignatureCVMIHM();
		break;

	case (CLESS_SAMPLE_VISAWAVE_SCREEN_SIGNATURE_OK):
		SDSA_UserInterface_SetCardholderLanguage(SDSA_KERNEL_IDENTIFIER_VISAWAVE);
		Helper_Visa_SignatureOKIHM();
		break;

	case (CLESS_SAMPLE_VISAWAVE_SCREEN_SIGNATURE_KO):
		SDSA_UserInterface_SetCardholderLanguage(SDSA_KERNEL_IDENTIFIER_VISAWAVE);
		Helper_Visa_SignatureKOIHM();
		break;

	case (CLESS_SAMPLE_VISAWAVE_SCREEN_ONLINE_APPROVED):
		SDSA_UserInterface_SetCardholderLanguage(SDSA_KERNEL_IDENTIFIER_VISAWAVE);
		Helper_Visa_ApprovedIHM(HELPERS_VISAWAVE, ONLINE);
		break;

	case (CLESS_SAMPLE_VISAWAVE_SCREEN_ONLINE_DECLINED):
		SDSA_UserInterface_SetCardholderLanguage(SDSA_KERNEL_IDENTIFIER_VISAWAVE);
		Helper_Visa_DeclinedIHM(HELPERS_VISAWAVE, ONLINE);
		break;

	case (CLESS_SAMPLE_VISAWAVE_SCREEN_ONLINE_PROCESSING):
		SDSA_UserInterface_SetCardholderLanguage(SDSA_KERNEL_IDENTIFIER_VISAWAVE);
		Helper_Visa_OnlineProcessingIHM(HELPERS_VISAWAVE);
		break;

	case (CLESS_SAMPLE_VISAWAVE_SCREEN_PIN_KO):
		SDSA_UserInterface_SetCardholderLanguage(SDSA_KERNEL_IDENTIFIER_VISAWAVE);
		Helper_Visa_PinEntryKOIHM(FALSE);
		break;
	 */

	case (CLESS_SAMPLE_VISAWAVE_SCREEN_PROCESSING):
		if (auCustomerDisplayAvailable) // If a customer display is available
		{
			Cless_Term_Read_Message(VISAWAVE_MESS_PROCESSING, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
			Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
		}

	Cless_Term_Read_Message(VISAWAVE_MESS_PROCESSING, nMerchantLanguage, &tMsg);
	Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	case (CLESS_SAMPLE_VISAWAVE_SCREEN_TRY_AGAIN):
		Cless_Term_Read_Message(VISAWAVE_MESS_TRY_AGAIN, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	case (CLESS_SAMPLE_VISAWAVE_SCREEN_TRY_AGAIN_NO_ERASE):
		Cless_Term_Read_Message(VISAWAVE_MESS_TRY_AGAIN, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(NO_ERASE, HELPERS_CUSTOMER_LINE_3, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	case (CLESS_SAMPLE_VISAWAVE_SCREEN_COLLISION):
		Cless_Term_Read_Message(VISAWAVE_MESS_SELECT_ONE_CARD_L1, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Cless_Term_Read_Message(VISAWAVE_MESS_SELECT_ONE_CARD_L2, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(NO_ERASE, HELPERS_CUSTOMER_LINE_3, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	case (CLESS_SAMPLE_VISAWAVE_SCREEN_USE_CONTACT):
		Cless_Term_Read_Message(VISAWAVE_MESS_INSERT_OR_SWIPE, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	case (CLESS_SAMPLE_SCREEN_TRY_ANOTHER_CARD):
		// Salon Carte 2010 Integration pour ne pas afficher Inserer carte lors de la présentation d'une carte sans contact non gérée
		Cless_Term_Read_Message(STD_MESS_INSERT_OR_TRY_ANOTHER_CARD_L1, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Cless_Term_Read_Message(STD_MESS_INSERT_OR_TRY_ANOTHER_CARD_L2, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(NO_ERASE, HELPERS_CUSTOMER_LINE_3, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	//////////////////////////////
	// PAYPASS SPECIFIC SCREENS //
	//////////////////////////////

	case (CLESS_SAMPLE_PAYPASS_SCREEN_SIGNATURE_REQUIRED):
		if (auCustomerDisplayAvailable) // If a customer display is available
		{
			Cless_Term_Read_Message(STD_MESS_SIGNATURE_CHECK_L1, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
			Cless_Term_Read_Message(STD_MESS_SIGNATURE_CHECK_L2, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(NO_ERASE, HELPERS_MERCHANT_LINE_4, &tMsg, NOLEDSOFF);
			Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
		}

	Cless_Term_Read_Message(STD_MESS_SIGNATURE_CHECK_L1, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Cless_Term_Read_Message(STD_MESS_SIGNATURE_CHECK_L2, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(NO_ERASE, HELPERS_CUSTOMER_LINE_3, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	case (CLESS_SAMPLE_PAYPASS_SCREEN_SIGNATURE_OK):
		if (auCustomerDisplayAvailable) // If a customer display is available
		{
			Cless_Term_Read_Message(STD_MESS_SIGNATURE_CORRECT, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
			Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
		}

	Cless_Term_Read_Message(STD_MESS_APPROVED, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	case (CLESS_SAMPLE_PAYPASS_SCREEN_SIGNATURE_KO):
		if (auCustomerDisplayAvailable) // If a customer display is available
		{
			Cless_Term_Read_Message(STD_MESS_SIGNATURE_INCORRECT, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
			Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
		}

	Cless_Term_Read_Message(STD_MESS_OFFLINE_DECLINED, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	case (CLESS_SAMPLE_PAYPASS_SCREEN_DECLINED):
		if (auCustomerDisplayAvailable) // If a customer display is available
		{
			Cless_Term_Read_Message(VISAWAVE_MESS_TRANSACTION, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
			Cless_Term_Read_Message(PAYPASS_MESS_NOT_AUTHORISED, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(NO_ERASE, HELPERS_MERCHANT_LINE_4, &tMsg, NOLEDSOFF);
			Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
		}

	//	// NO_ERASE because amount was previously displayed
	//	Cless_Term_Read_Message(VISAWAVE_MESS_TRANSACTION, nCardholderLanguage, &tMsg);
	//	Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	//	Cless_Term_Read_Message(PAYPASS_MESS_NOT_AUTHORISED, nCardholderLanguage, &tMsg);
	//	Helper_DisplayTextCustomer (NO_ERASE, HELPERS_CUSTOMER_LINE_3, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	//	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);

	Cless_Term_Read_Message(STD_MESS_INSERT_OR_TRY_ANOTHER_CARD_L1, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Cless_Term_Read_Message(STD_MESS_INSERT_OR_TRY_ANOTHER_CARD_L2, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(NO_ERASE, HELPERS_CUSTOMER_LINE_3, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	case (CLESS_SAMPLE_SCREEN_ONLINE_UNABLE):
		if (auCustomerDisplayAvailable) // If a customer display is available
		{
			Cless_Term_Read_Message(STD_MESS_UNABLE_ON_LINE, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
			Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
		}

	Cless_Term_Read_Message(STD_MESS_ONLINE_DECLINED, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	case (CLESS_SAMPLE_PAYPASS_SCREEN_AUTHORISING):
		if (auCustomerDisplayAvailable) // If a customer display is available
		{
			Cless_Term_Read_Message(STD_MESS_AUTHORISING, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
			Cless_Term_Read_Message(STD_MESS_PLEASE_WAIT, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(NO_ERASE, HELPERS_MERCHANT_LINE_4, &tMsg, NOLEDSOFF);
			Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
		}

	Cless_Term_Read_Message(STD_MESS_AUTHORISING, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Cless_Term_Read_Message(STD_MESS_PLEASE_WAIT, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer (NO_ERASE, HELPERS_CUSTOMER_LINE_3, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	/*	case (CLESS_SAMPLE_PAYPASS_SCREEN_OFFLINE_APPROVED_REFUND):
		SDSA_UserInterface_SetCardholderLanguage(SDSA_KERNEL_IDENTIFIER_PAYPASS);
		_Helper_DisplayMessage(STD_MESS_MENU_MAIN_REFUND, ERASE, HELPERS_CUSTOMER_LINE_2, HELPERS_ALIGN_CENTER, NOLEDSOFF);
		Helper_DisplayMessage(STD_MESS_APPROVED, NO_ERASE, HELPERS_CUSTOMER_LINE_3, HELPERS_ALIGN_CENTER, NOLEDSOFF);
		Helper_WaitUserDisplay();*/

	/*	case (CLESS_SAMPLE_PAYPASS_SCREEN_ERROR_REFUND):
		SDSA_UserInterface_SetCardholderLanguage(SDSA_KERNEL_IDENTIFIER_PAYPASS);
		_Helper_DisplayMessage(STD_MESS_MENU_MAIN_REFUND, ERASE, HELPERS_CUSTOMER_LINE_2, HELPERS_ALIGN_CENTER, NOLEDSOFF);
		Helper_DisplayMessage(STD_MESS_FAILED, NO_ERASE, HELPERS_CUSTOMER_LINE_3, HELPERS_ALIGN_CENTER, NOLEDSOFF);
		Helper_WaitUserDisplay();
		break;*/

	case (CLESS_SAMPLE_PAYPASS_SCREEN_APPROVED):
		if (auCustomerDisplayAvailable) // If a customer display is available
		{
			Cless_Term_Read_Message(STD_MESS_OFFLINE_APPROVED, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
			Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
		}

	Cless_Term_Read_Message(STD_MESS_OFFLINE_APPROVED, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(ERASE, HELPERS_CUSTOMER_LINE_1, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	/*	case (CLESS_SAMPLE_PAYPASS_SCREEN_REMOVE_CARD_DS_OPERATION):

		// Turn on the buzzer
		//TPass_Buzzer(C_DEFAULT_BEEP_OK_FREQUENCY, (unsigned char)C_DEFAULT_BEEP_VOLUME);
		TPass_LedsBlink (TPASS_LED_1 | TPASS_LED_2 | TPASS_LED_3 | TPASS_LED_4, 10, 10);

		// Get the STD_MESS_PRESENT_CARD message and display it
		ulBeforeDeselectDisplayTime = GTL_StdTimer_GetCurrent();

		// Display the "Remove card" message
		if (auCustomerDisplayAvailable) // If a customer display is available
		{
			Cless_Term_Read_Message(STD_MESS_REMOVE_CARD, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(TRUE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
			Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
		}

		// Display the customer message
		Cless_Term_Read_Message(STD_MESS_REMOVE_CARD, nCardholderLanguage, &tMsg);
		Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
		Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);

		// Double beep
		TPass_Buzzer(C_DEFAULT_BEEP_OK_FREQUENCY, (unsigned char)C_DEFAULT_BEEP_VOLUME);
		ttestall(0,10);
		TPass_Buzzer (0,0);
		ttestall(0,10);
		TPass_Buzzer(C_DEFAULT_BEEP_OK_FREQUENCY, (unsigned char)C_DEFAULT_BEEP_VOLUME);
		ttestall(0,10);
		TPass_Buzzer (0,0);

		// Wait display time
		nTimeout = GTL_StdTimer_GetRemaining(ulBeforeDeselectDisplayTime, 100);

		if (nTimeout > 0)
		{
			ttestall(0, nTimeout);
			nTimeout = 0;
		}
		TPass_LedsOff (TPASS_LED_1 | TPASS_LED_2 | TPASS_LED_3 | TPASS_LED_4);

		break;*/

	/////////////////////////////////
	// EXPRESSPAY SPECIFIC SCREENS //
	/////////////////////////////////

	case (CLESS_SAMPLE_EXPRESSPAY_SCREEN_SIGNATURE_REQUIRED):
    											if (auCustomerDisplayAvailable) // If a customer display is available
    											{
    		Cless_Term_Read_Message(STD_MESS_SIGNATURE_CHECK_L1, nMerchantLanguage, &tMsg);
    		Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
    		Cless_Term_Read_Message(STD_MESS_SIGNATURE_CHECK_L2, nMerchantLanguage, &tMsg);
    		Helper_DisplayTextMerchant(NO_ERASE, HELPERS_MERCHANT_LINE_4, &tMsg, NOLEDSOFF);
    		Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
    											}

	Cless_Term_Read_Message(STD_MESS_SIGNATURE_CHECK_L1, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Cless_Term_Read_Message(STD_MESS_SIGNATURE_CHECK_L2, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(NO_ERASE, HELPERS_CUSTOMER_LINE_3, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	case (CLESS_SAMPLE_EXPRESSPAY_SCREEN_OFFLINE_APPROVED):
    											if (auCustomerDisplayAvailable) // If a customer display is available
    											{
    		Cless_Term_Read_Message(STD_MESS_OFFLINE_APPROVED, nMerchantLanguage, &tMsg);
    		Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
    		Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
    											}

	Cless_Term_Read_Message(STD_MESS_OFFLINE_APPROVED, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	case (CLESS_SAMPLE_EXPRESSPAY_SCREEN_ONLINE_APPROVED):
    											if (auCustomerDisplayAvailable) // If a customer display is available
    											{
    		Cless_Term_Read_Message(STD_MESS_ONLINE_APPROVED, nMerchantLanguage, &tMsg);
    		Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
    		Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
    											}

	Cless_Term_Read_Message(STD_MESS_ONLINE_APPROVED, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	case (CLESS_SAMPLE_EXPRESSPAY_SCREEN_SIGNATURE_OK):
    											if (auCustomerDisplayAvailable) // If a customer display is available
    											{
    		Cless_Term_Read_Message(STD_MESS_SIGNATURE_CORRECT, nMerchantLanguage, &tMsg);
    		Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
    		Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
    											}

	Cless_Term_Read_Message(STD_MESS_APPROVED, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	case (CLESS_SAMPLE_EXPRESSPAY_SCREEN_ONLINE_SIGNATURE_OK):
    											if (auCustomerDisplayAvailable) // If a customer display is available
    											{
    		Cless_Term_Read_Message(STD_MESS_SIGNATURE_CORRECT, nMerchantLanguage, &tMsg);
    		Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
    		Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
    											}

	Cless_Term_Read_Message(STD_MESS_ONLINE_APPROVED, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	case (CLESS_SAMPLE_EXPRESSPAY_SCREEN_SIGNATURE_KO):
    											if (auCustomerDisplayAvailable) // If a customer display is available
    											{
    		Cless_Term_Read_Message(STD_MESS_SIGNATURE_INCORRECT, nMerchantLanguage, &tMsg);
    		Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
    		Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
    											}

	Cless_Term_Read_Message(STD_MESS_OFFLINE_DECLINED, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	case (CLESS_SAMPLE_EXPRESSPAY_SCREEN_OFFLINE_DECLINED):

    											if (auCustomerDisplayAvailable) // If a customer display is available
    											{
    		Cless_Term_Read_Message(STD_MESS_OFFLINE_DECLINED, nMerchantLanguage, &tMsg);
    		Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
    		Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
    											}

	Cless_Term_Read_Message(STD_MESS_OFFLINE_DECLINED, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	case (CLESS_SAMPLE_EXPRESSPAY_SCREEN_ONLINE_DECLINED):

    											if (auCustomerDisplayAvailable) // If a customer display is available
    											{
    		Cless_Term_Read_Message(STD_MESS_ONLINE_DECLINED, nMerchantLanguage, &tMsg);
    		Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
    		Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
    											}

	Cless_Term_Read_Message(STD_MESS_ONLINE_DECLINED, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	case (CLESS_SAMPLE_EXPRESSPAY_SCREEN_NOT_PERMITTED):
    											if (auCustomerDisplayAvailable) // If a customer display is available
    											{
    		Cless_Term_Read_Message(EXPRESSPAY_MESS_CLESS_TRANS_NOT_PERMITTED_L1, nMerchantLanguage, &tMsg);
    		Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_2, &tMsg, NOLEDSOFF);
    		Cless_Term_Read_Message(EXPRESSPAY_MESS_CLESS_TRANS_NOT_PERMITTED_L2, nMerchantLanguage, &tMsg);
    		Helper_DisplayTextMerchant(NO_ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
    		Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
    											}

	Cless_Term_Read_Message(EXPRESSPAY_MESS_CLESS_TRANS_NOT_PERMITTED_L1, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Cless_Term_Read_Message(EXPRESSPAY_MESS_CLESS_TRANS_NOT_PERMITTED_L2, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer (NO_ERASE, HELPERS_CUSTOMER_LINE_3, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	case (CLESS_SAMPLE_EXPRESSPAY_SCREEN_FULL_ONLINE_PROCESSING):
    											if (auCustomerDisplayAvailable) // If a customer display is available
    											{
    		Cless_Term_Read_Message(EXPRESSPAY_MESS_PROCESSING, nMerchantLanguage, &tMsg);
    		Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
    		Cless_Term_Read_Message(STD_MESS_PLEASE_WAIT, nMerchantLanguage, &tMsg);
    		Helper_DisplayTextMerchant(NO_ERASE, HELPERS_MERCHANT_LINE_4, &tMsg, NOLEDSOFF);
    		Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
    											}

	Cless_Term_Read_Message(EXPRESSPAY_MESS_PROCESSING, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Cless_Term_Read_Message(STD_MESS_PLEASE_WAIT, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer (NO_ERASE, HELPERS_CUSTOMER_LINE_3, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	case (CLESS_SAMPLE_EXPRESSPAY_SCREEN_ONLINE_PROCESSING):
    											if (auCustomerDisplayAvailable) // If a customer display is available
    											{
    		Cless_Term_Read_Message(STD_MESS_ONLINE_REQUEST, nMerchantLanguage, &tMsg);
    		Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
    		Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
    											}

	Cless_Term_Read_Message(STD_MESS_ONLINE_REQUEST, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	case (CLESS_SAMPLE_EXPRESSPAY_SCREEN_PIN_CANCEL):
    											if (auCustomerDisplayAvailable) // If a customer display is available
    											{
    		Cless_Term_Read_Message(STD_MESS_PIN_CANCEL, nMerchantLanguage, &tMsg);
    		Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
    		Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
    											}

	Cless_Term_Read_Message(STD_MESS_OFFLINE_DECLINED, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	case (CLESS_SAMPLE_EXPRESSPAY_SCREEN_PIN_ERROR):
    											if (auCustomerDisplayAvailable) // If a customer display is available
    											{
    		Cless_Term_Read_Message(STD_MESS_ONLINE_PIN_ERROR, nMerchantLanguage, &tMsg);
    		Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
    		Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
    											}

	Cless_Term_Read_Message(STD_MESS_OFFLINE_DECLINED, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	//////////////////////////////
	// INTERAC SPECIFIC SCREENS //
	//////////////////////////////

	case (CLESS_SAMPLE_INTERAC_SCREEN_COLLISION):
		Cless_Term_Read_Message(INTERAC_MESS_SELECT_ONE_CARD_L1, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Cless_Term_Read_Message(INTERAC_MESS_SELECT_ONE_CARD_L2, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(NO_ERASE, HELPERS_CUSTOMER_LINE_3, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	case (CLESS_SAMPLE_INTERAC_SCREEN_NOT_SUPPORTED):
		Cless_Term_Read_Message(INTERAC_MESS_NOT_SUPPORTED_L1, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Cless_Term_Read_Message(INTERAC_MESS_NOT_SUPPORTED_L2, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(NO_ERASE, HELPERS_CUSTOMER_LINE_3, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	case (CLESS_SAMPLE_INTERAC_SCREEN_CONTACT):
		Cless_Term_Read_Message(INTERAC_MESS_INSERT_CARD_L1, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Cless_Term_Read_Message(INTERAC_MESS_INSERT_CARD_L2, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(NO_ERASE, HELPERS_CUSTOMER_LINE_3, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	///////////////////////////
	// VISA SPECIFIC SCREENS //
	///////////////////////////

	case (CLESS_SAMPLE_VISA_SCREEN_SIGNATURE_ASIA):
    											if (auCustomerDisplayAvailable) // If a customer display is available
    											{
    		Cless_Term_Read_Message(VISAWAVE_MESS_SIGNATURE_CHECK_L1, nMerchantLanguage, &tMsg);
    		Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
    		Cless_Term_Read_Message(VISAWAVE_MESS_SIGNATURE_CHECK_L2, nMerchantLanguage, &tMsg);
    		Helper_DisplayTextMerchant(NO_ERASE, HELPERS_MERCHANT_LINE_4, &tMsg, NOLEDSOFF);
    		Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
    											}

	Cless_Term_Read_Message(VISAWAVE_MESS_SIGNATURE_CHECK_L1, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Cless_Term_Read_Message(VISAWAVE_MESS_SIGNATURE_CHECK_L2, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(NO_ERASE, HELPERS_CUSTOMER_LINE_3, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	case (CLESS_SAMPLE_VISA_SCREEN_SIGNATURE):
    											if (auCustomerDisplayAvailable) // If a customer display is available
    											{
    		Cless_Term_Read_Message(STD_MESS_SIGNATURE_CHECK_L1, nMerchantLanguage, &tMsg);
    		Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
    		Cless_Term_Read_Message(STD_MESS_SIGNATURE_CHECK_L2, nMerchantLanguage, &tMsg);
    		Helper_DisplayTextMerchant(NO_ERASE, HELPERS_MERCHANT_LINE_4, &tMsg, NOLEDSOFF);
    		Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
    											}

	Cless_Term_Read_Message(STD_MESS_SIGNATURE_CHECK_L1, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Cless_Term_Read_Message(STD_MESS_SIGNATURE_CHECK_L2, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(NO_ERASE, HELPERS_CUSTOMER_LINE_3, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	case (CLESS_SAMPLE_VISA_APPROVED_ASIA):
		if (auCustomerDisplayAvailable) // If a customer display is available
		{
			Cless_Term_Read_Message(VISAWAVE_MESS_THANK_YOU, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_1, &tMsg, NOLEDSOFF);
			Cless_Term_Read_Message(VISAWAVE_MESS_TRANSACTION, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(NO_ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
			Cless_Term_Read_Message(VISAWAVE_MESS_TRANSACTION_COMPLETED, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(NO_ERASE, HELPERS_MERCHANT_LINE_4, &tMsg, NOLEDSOFF);
			Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
		}
	Cless_Term_Read_Message(VISAWAVE_MESS_THANK_YOU, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(ERASE, HELPERS_CUSTOMER_LINE_1, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Cless_Term_Read_Message(VISAWAVE_MESS_TRANSACTION, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(NO_ERASE, HELPERS_CUSTOMER_LINE_3, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Cless_Term_Read_Message(VISAWAVE_MESS_TRANSACTION_COMPLETED, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(NO_ERASE, HELPERS_CUSTOMER_LINE_4, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NO_WAIT, HELPERS_CUSTOMER_SCREEN);
	break;


	case (CLESS_SAMPLE_VISA_OFFLINE_APPROVED):
	case (CLESS_SAMPLE_VISA_ONLINE_APPROVED):
	{
		int nMsgNum;

		if (ulScreenIdentifier == CLESS_SAMPLE_VISA_OFFLINE_APPROVED)
			nMsgNum = STD_MESS_OFFLINE_APPROVED;
		else // if (ulScreenIdentifier == CLESS_SAMPLE_VISA_ONLINE_APPROVED)
			nMsgNum = STD_MESS_ONLINE_APPROVED;

		if (auCustomerDisplayAvailable) // If a customer display is available
		{
			Cless_Term_Read_Message(nMsgNum, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
			Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
		}
		Cless_Term_Read_Message(nMsgNum, nCardholderLanguage, &tMsg);
		Helper_DisplayTextCustomer(ERASE, HELPERS_CUSTOMER_LINE_1, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);

		//		if (TPass_GetDeviceType() == TPASS_CLESS_TYPE_IPP2XX) // Two lines display
		if ((TPass_GetDeviceType() == TPASS_CLESS_TYPE_IPP2XX) && (!Cless_UseGraphicalFunctionOnPPAD()))
			Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	}
	break;

	case (CLESS_SAMPLE_VISA_SIGNATURE_OK):
		if (auCustomerDisplayAvailable) // If a customer display is available
		{
			Cless_Term_Read_Message(STD_MESS_SIGNATURE_CORRECT, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
			Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
		}

	Cless_Term_Read_Message(STD_MESS_APPROVED, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(ERASE, HELPERS_CUSTOMER_LINE_1, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);

	//		if (TPass_GetDeviceType() == TPASS_CLESS_TYPE_IPP2XX) // Two lines display
	if ((TPass_GetDeviceType() == TPASS_CLESS_TYPE_IPP2XX) && (!Cless_UseGraphicalFunctionOnPPAD()))
		Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	case (CLESS_SAMPLE_VISA_SIGNATURE_OK_ASIA):
		if (auCustomerDisplayAvailable) // If a customer display is available
		{
			Cless_Term_Read_Message(STD_MESS_SIGNATURE_CORRECT, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
			Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
		}

	Cless_Term_Read_Message(VISAWAVE_MESS_THANK_YOU, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(ERASE, HELPERS_CUSTOMER_LINE_1, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Cless_Term_Read_Message(VISAWAVE_MESS_TRANSACTION, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(NO_ERASE, HELPERS_CUSTOMER_LINE_3, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Cless_Term_Read_Message(VISAWAVE_MESS_TRANSACTION_COMPLETED, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(NO_ERASE, HELPERS_CUSTOMER_LINE_4, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	case (CLESS_SAMPLE_VISA_SIGNATURE_KO):
		if (auCustomerDisplayAvailable) // If a customer display is available
		{
			Cless_Term_Read_Message(STD_MESS_SIGNATURE_INCORRECT, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
			Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
		}
	Cless_Term_Read_Message(STD_MESS_OFFLINE_DECLINED, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_1, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);

	//		if (TPass_GetDeviceType() == TPASS_CLESS_TYPE_IPP2XX) // Two lines display
	if ((TPass_GetDeviceType() == TPASS_CLESS_TYPE_IPP2XX) && (!Cless_UseGraphicalFunctionOnPPAD()))
		Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	case (CLESS_SAMPLE_VISA_SIGNATURE_KO_ASIA):
		if (auCustomerDisplayAvailable) // If a customer display is available
		{
			Cless_Term_Read_Message(VISAWAVE_MESS_SIGNATURE_REQUIRED, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_2, &tMsg, NOLEDSOFF);
			Cless_Term_Read_Message(VISAWAVE_MESS_TRANSACTION, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(NO_ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
			Cless_Term_Read_Message(VISAWAVE_MESS_TRANSACTION_NOT_COMPLETED, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(NO_ERASE, HELPERS_MERCHANT_LINE_4, &tMsg, NOLEDSOFF);
			Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
		}
	Cless_Term_Read_Message(VISAWAVE_MESS_SIGNATURE_REQUIRED, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(ERASE, HELPERS_CUSTOMER_LINE_1, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Cless_Term_Read_Message(VISAWAVE_MESS_TRANSACTION, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(NO_ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Cless_Term_Read_Message(VISAWAVE_MESS_TRANSACTION_NOT_COMPLETED, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(NO_ERASE, HELPERS_CUSTOMER_LINE_3, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	case (CLESS_SAMPLE_VISA_PIN_KO):
		if (auCustomerDisplayAvailable) // If a customer display is available
		{
			Cless_Term_Read_Message(STD_MESS_PIN_ERROR, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
			Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
		}

	Cless_Term_Read_Message(STD_MESS_PIN_ERROR, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(ERASE, HELPERS_CUSTOMER_LINE_1, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	case (CLESS_SAMPLE_VISA_PIN_KO_ASIA):
		if (auCustomerDisplayAvailable) // If a customer display is available
		{
			Cless_Term_Read_Message(VISAWAVE_MESS_PIN_REQUIRED, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_2, &tMsg, NOLEDSOFF);
			Cless_Term_Read_Message(VISAWAVE_MESS_TRANSACTION, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(NO_ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
			Cless_Term_Read_Message(VISAWAVE_MESS_TRANSACTION_NOT_COMPLETED, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(NO_ERASE, HELPERS_MERCHANT_LINE_4, &tMsg, NOLEDSOFF);
			Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
		}
	Cless_Term_Read_Message(VISAWAVE_MESS_PIN_REQUIRED, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(ERASE, HELPERS_CUSTOMER_LINE_1, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Cless_Term_Read_Message(VISAWAVE_MESS_TRANSACTION, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(NO_ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Cless_Term_Read_Message(VISAWAVE_MESS_TRANSACTION_NOT_COMPLETED, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(NO_ERASE, HELPERS_CUSTOMER_LINE_3, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	case (CLESS_SAMPLE_VISA_DECLINED_ASIA):
		if (auCustomerDisplayAvailable) // If a customer display is available
		{
			Cless_Term_Read_Message(VISAWAVE_MESS_TRANSACTION, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_1, &tMsg, NOLEDSOFF);
			Cless_Term_Read_Message(VISAWAVE_MESS_TRANSACTION_DECLINED, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(NO_ERASE, HELPERS_MERCHANT_LINE_2, &tMsg, NOLEDSOFF);
			Cless_Term_Read_Message(VISAWAVE_MESS_USE_OTHER_VISA_CARD_L1, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(NO_ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
			Cless_Term_Read_Message(VISAWAVE_MESS_USE_OTHER_VISA_CARD_L2, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(NO_ERASE, HELPERS_MERCHANT_LINE_4, &tMsg, NOLEDSOFF);
			Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
		}
	Cless_Term_Read_Message(VISAWAVE_MESS_TRANSACTION, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(ERASE, HELPERS_CUSTOMER_LINE_1, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Cless_Term_Read_Message(VISAWAVE_MESS_TRANSACTION_DECLINED, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(NO_ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Cless_Term_Read_Message(VISAWAVE_MESS_USE_OTHER_VISA_CARD_L1, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(NO_ERASE, HELPERS_CUSTOMER_LINE_3, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Cless_Term_Read_Message(VISAWAVE_MESS_USE_OTHER_VISA_CARD_L2, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(NO_ERASE, HELPERS_CUSTOMER_LINE_4, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	case (CLESS_SAMPLE_VISA_OFFLINE_DECLINED):
	case (CLESS_SAMPLE_VISA_ONLINE_DECLINED):
	{
		int nMsgNum;

		if (ulScreenIdentifier == CLESS_SAMPLE_VISA_OFFLINE_DECLINED)
			nMsgNum = STD_MESS_OFFLINE_DECLINED;
		else // if (ulScreenIdentifier == CLESS_SAMPLE_VISA_ONLINE_DECLINED)
			nMsgNum = STD_MESS_ONLINE_DECLINED;

		if (auCustomerDisplayAvailable) // If a customer display is available
		{
			Cless_Term_Read_Message(nMsgNum, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
			Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
		}

		Cless_Term_Read_Message(nMsgNum, nMerchantLanguage, &tMsg);
		Helper_DisplayTextCustomer(ERASE, HELPERS_CUSTOMER_LINE_1, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);

		//		if (TPass_GetDeviceType() == TPASS_CLESS_TYPE_IPP2XX) // Two lines display
		if ((TPass_GetDeviceType() == TPASS_CLESS_TYPE_IPP2XX) && (!Cless_UseGraphicalFunctionOnPPAD()))
			Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	}
	break;

	case (CLESS_SAMPLE_VISA_ONLINE):
		if (auCustomerDisplayAvailable) // If a customer display is available
		{
			Cless_Term_Read_Message(STD_MESS_ONLINE_REQUEST, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
			Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
		}
	Cless_Term_Read_Message(STD_MESS_ONLINE_REQUEST, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(ERASE, HELPERS_CUSTOMER_LINE_1, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);

	//		if (TPass_GetDeviceType() == TPASS_CLESS_TYPE_IPP2XX) // Two lines display
	if ((TPass_GetDeviceType() == TPASS_CLESS_TYPE_IPP2XX) && (!Cless_UseGraphicalFunctionOnPPAD()))
		Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	case (CLESS_SAMPLE_VISA_ONLINE_ASIA):
		if (auCustomerDisplayAvailable) // If a customer display is available
		{
			Cless_Term_Read_Message(STD_MESS_PLEASE_WAIT, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_2, &tMsg, NOLEDSOFF);
			Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
		}
	Cless_Term_Read_Message(STD_MESS_PLEASE_WAIT, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	case (CLESS_SAMPLE_VISA_ERROR):
		if (auCustomerDisplayAvailable) // If a customer display is available
		{
			Cless_Term_Read_Message(STD_MESS_ERROR, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
			Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
		}
	Cless_Term_Read_Message(STD_MESS_ERROR, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	case (CLESS_SAMPLE_VISA_ERROR_ASIA):
		if (auCustomerDisplayAvailable) // If a customer display is available
		{
			Cless_Term_Read_Message(VISAWAVE_MESS_TRANSACTION, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_1, &tMsg, NOLEDSOFF);
			Cless_Term_Read_Message(VISAWAVE_MESS_TRANSACTION_TERMINATED, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(NO_ERASE, HELPERS_MERCHANT_LINE_2, &tMsg, NOLEDSOFF);
			Cless_Term_Read_Message(VISAWAVE_MESS_USE_OTHER_VISA_CARD_L1, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(NO_ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
			Cless_Term_Read_Message(VISAWAVE_MESS_USE_OTHER_VISA_CARD_L2, nMerchantLanguage, &tMsg);
			Helper_DisplayTextMerchant(NO_ERASE, HELPERS_MERCHANT_LINE_4, &tMsg, NOLEDSOFF);
			Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
		}
	Cless_Term_Read_Message(VISAWAVE_MESS_TRANSACTION, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(ERASE, HELPERS_CUSTOMER_LINE_1, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Cless_Term_Read_Message(VISAWAVE_MESS_TRANSACTION_TERMINATED, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(NO_ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Cless_Term_Read_Message(VISAWAVE_MESS_USE_OTHER_VISA_CARD_L1, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(NO_ERASE, HELPERS_CUSTOMER_LINE_3, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Cless_Term_Read_Message(VISAWAVE_MESS_USE_OTHER_VISA_CARD_L2, nCardholderLanguage, &tMsg);
	Helper_DisplayTextCustomer(NO_ERASE, HELPERS_CUSTOMER_LINE_4, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	break;

	default:
		// Unexpected screen identifier
		GTL_Traces_TraceDebug ("Cless_GuiState_DisplayScreen : Unexpected screen identifier (%02lx)", ulScreenIdentifier);
		break;
	}
}


//! \brief Determines the display conditions for "PRESENT CARD" and "RETRY" screens.
//! \param[out] pDisplayClessLogoInternal \a TRUE if internal logo shall be displayed (internal cless readers), \a FALSE else.
//! \param[out] pDisplayClessLogoExternal \a TRUE if external logo shall be displayed (external cless readers), \a FALSE else.
//! \param[out] pInstructionLineMerchant Indicates the line where the merchant instruction shall be displayed. -1 if not applicable.
//! \param[out] pInstructionLineCardholder Indicates the line where the customer instruction shall be displayed. -1 if not applicable.
//! \param[out] pAmountLineMerchant Indicates the line where the merchant amount shall be displayed. -1 if not applicable.
//! \param[out] pAmountLineCardholder Indicates the line where the cardholder amount shall be displayed. -1 if not applicable.
//! \param[out] pFontSizeMerchant Indicates the font size to be used to display the messages on the merchant display. By default, it is set to \a HELPERS_FONT_SIZE_DEFAULT.
//! \param[out] pFontSizeCustomer Indicates the font size to be used to display the messages on the customer display. By default, it is set to \a HELPERS_FONT_SIZE_DEFAULT.

static void __Cless_GuiState_GetDisplayInfoWithLogo (int * pDisplayClessLogoInternal, int * pDisplayClessLogoExternal, int * pInstructionLineMerchant, int * pInstructionLineCardholder, int * pAmountLineMerchant, int * pAmountLineCardholder, int * pFontSizeMerchant, int * pFontSizeCustomer) {
	int nClessDeviceType = TPASS_CLESS_TYPE_NONE;

	// Check input data are correct (no NULL pointer)
	if ((pDisplayClessLogoInternal == NULL) || (pDisplayClessLogoExternal == NULL) || (pInstructionLineCardholder == NULL) || (pInstructionLineMerchant == NULL) ||
			(pAmountLineCardholder == NULL) || (pAmountLineMerchant == NULL) || (pFontSizeMerchant == NULL) || (pFontSizeCustomer == NULL))
		return;

	// init output data
	*pDisplayClessLogoInternal = FALSE;
	*pDisplayClessLogoExternal = FALSE;
	*pInstructionLineMerchant = -1;
	*pInstructionLineCardholder = -1;
	*pAmountLineMerchant = -1;
	*pAmountLineCardholder = -1;
	*pFontSizeMerchant = HELPERS_FONT_SIZE_DEFAULT;
	*pFontSizeCustomer = HELPERS_FONT_SIZE_DEFAULT;

	// Get the contactless device type
	nClessDeviceType = TPass_GetDeviceTypeEx();

	if (nClessDeviceType != TPASS_CLESS_TYPE_NONE)
	{
		if (nClessDeviceType == TPASS_CLESS_TYPE_EMBEDDED)
		{
			// Internal cless reader
			switch (Cless_Common_GetTerminalType())
			{
			/////////////////////////////////////////////////////////////////////////////
			case (TERM_ISC250): // iSC250 ///////////////////////////////////////////////
				*pDisplayClessLogoInternal = TRUE;
			*pInstructionLineCardholder = ((Cless_Goal_IsAvailable()) ? (HELPERS_CUSTOMER_LINE_5) : (HELPERS_CUSTOMER_LINE_4));
			*pAmountLineCardholder = HELPERS_CUSTOMER_LINE_1;
			break;
			/////////////////////////////////////////////////////////////////////////////
			case (TERM_ISC350): // iSC350 ///////////////////////////////////////////////
				*pDisplayClessLogoInternal = TRUE;
			*pInstructionLineCardholder = ((Cless_Goal_IsAvailable()) ? (HELPERS_CUSTOMER_LINE_5) : (HELPERS_CUSTOMER_LINE_4));
			*pAmountLineCardholder = HELPERS_CUSTOMER_LINE_1;
			break;
			/////////////////////////////////////////////////////////////////////////////
			case (TERM_ICT250): // iCT250 ///////////////////////////////////////////////
				*pDisplayClessLogoInternal = TRUE;
			if (IsColorDisplay())
			{
				*pInstructionLineCardholder = ((Cless_Goal_IsAvailable()) ? (HELPERS_CUSTOMER_LINE_5) : (HELPERS_CUSTOMER_LINE_4));
				*pAmountLineCardholder = HELPERS_CUSTOMER_LINE_1;
			}
			else // ICT220
			{
				*pInstructionLineCardholder = HELPERS_CUSTOMER_LINE_2;
				*pAmountLineCardholder = HELPERS_CUSTOMER_LINE_1;
				*pFontSizeCustomer = HELPERS_FONT_SIZE_MEDIUM;
			}
			break;
			/////////////////////////////////////////////////////////////////////////////
			case (TERM_IWL220): // iWL220 ///////////////////////////////////////////////
				*pDisplayClessLogoInternal = TRUE;
			*pInstructionLineCardholder = HELPERS_CUSTOMER_LINE_2;
			*pAmountLineCardholder = HELPERS_CUSTOMER_LINE_1;
			*pFontSizeCustomer = HELPERS_FONT_SIZE_MEDIUM;
			break;
			/////////////////////////////////////////////////////////////////////////////
			case (TERM_IWL250): // iWL250 ///////////////////////////////////////////////
				*pDisplayClessLogoInternal = TRUE;
			*pInstructionLineCardholder = ((Cless_Goal_IsAvailable()) ? (HELPERS_CUSTOMER_LINE_5) : (HELPERS_CUSTOMER_LINE_4));
			*pAmountLineCardholder = HELPERS_CUSTOMER_LINE_1;
			break;
			/////////////////////////////////////////////////////////////////////////////
			case (TERM_IWL280): // iWL280 ///////////////////////////////////////////////
				*pDisplayClessLogoInternal = TRUE;
			*pInstructionLineCardholder = ((Cless_Goal_IsAvailable()) ? (HELPERS_CUSTOMER_LINE_5) : (HELPERS_CUSTOMER_LINE_6));
			*pAmountLineCardholder = HELPERS_CUSTOMER_LINE_1;
			*pFontSizeCustomer = HELPERS_FONT_SIZE_MEDIUM;
			break;
			/////////////////////////////////////////////////////////////////////////////
			case (TERM_IPP320): // iPP320 ///////////////////////////////////////////////
				*pDisplayClessLogoInternal = TRUE;
			*pInstructionLineCardholder = HELPERS_CUSTOMER_LINE_2;
			*pAmountLineCardholder = HELPERS_CUSTOMER_LINE_1;
			break;
			/////////////////////////////////////////////////////////////////////////////
			case (TERM_IPP350): // iPP350 ///////////////////////////////////////////////
				*pDisplayClessLogoInternal = TRUE;
			*pInstructionLineCardholder = ((Cless_Goal_IsAvailable()) ? (HELPERS_CUSTOMER_LINE_5) : (HELPERS_CUSTOMER_LINE_4));
			*pAmountLineCardholder = HELPERS_CUSTOMER_LINE_1;
			break;
			/////////////////////////////////////////////////////////////////////////////
			case (TERM_IMP320): // iSMP /////////////////////////////////////////////////
			case (TERM_IMP350):
			*pDisplayClessLogoInternal = TRUE;
			*pInstructionLineCardholder = HELPERS_CUSTOMER_LINE_2;
			*pAmountLineCardholder = HELPERS_CUSTOMER_LINE_1;
			*pFontSizeCustomer = HELPERS_FONT_SIZE_MEDIUM;
			break;
			/////////////////////////////////////////////////////////////////////////////
			case (TERM_P300): // ML30 ///////////////////////////////////////////////////
				*pDisplayClessLogoInternal = TRUE;
			*pInstructionLineCardholder = HELPERS_CUSTOMER_LINE_2;
			*pAmountLineCardholder = HELPERS_CUSTOMER_LINE_1;
			*pFontSizeCustomer = HELPERS_FONT_SIZE_MEDIUM;
			break;
			/////////////////////////////////////////////////////////////////////////////
			case (TERM_EFT930): // EFT930 ///////////////////////////////////////////////
			case (TERM_EFT930BT):
			case (TERM_EFT930G):
			case (TERM_EFT930SGEM):
			case (TERM_EFT930W):
			case (TERM_EFT930S):
			*pDisplayClessLogoInternal = TRUE;
			*pInstructionLineCardholder = ((Cless_Goal_IsAvailable()) ? (HELPERS_CUSTOMER_LINE_5) : (HELPERS_CUSTOMER_LINE_2));
			*pAmountLineCardholder = HELPERS_CUSTOMER_LINE_1;
			*pFontSizeCustomer = HELPERS_FONT_SIZE_MEDIUM;
			break;
			/////////////////////////////////////////////////////////////////////////////
			case (TERM_SPM): // iPA280 //////////////////////////////////////////////////
				*pDisplayClessLogoInternal = FALSE;
			*pInstructionLineCardholder = ((Cless_Goal_IsAvailable()) ? (HELPERS_CUSTOMER_LINE_5) : (HELPERS_CUSTOMER_LINE_3));
			*pAmountLineCardholder = HELPERS_CUSTOMER_LINE_2;
			break;
			/////////////////////////////////////////////////////////////////////////////
			case (TERM_MR40): // CAD30 UCR //////////////////////////////////////////////
				*pDisplayClessLogoInternal = FALSE;
			*pInstructionLineCardholder = HELPERS_CUSTOMER_LINE_3;
			*pAmountLineCardholder = HELPERS_CUSTOMER_LINE_2;
			break;
			/////////////////////////////////////////////////////////////////////////////
			case (TERM_IUN180): // iUC180 ///////////////////////////////////////////////
				*pDisplayClessLogoInternal = FALSE;
			*pInstructionLineCardholder = HELPERS_CUSTOMER_LINE_3;
			*pAmountLineCardholder = HELPERS_CUSTOMER_LINE_2;
			break;
			/////////////////////////////////////////////////////////////////////////////
			case (TERM_IUN230): // iUP250 ///////////////////////////////////////////////
				*pDisplayClessLogoInternal = FALSE;
			*pInstructionLineCardholder = HELPERS_CUSTOMER_LINE_3;
			*pAmountLineCardholder = HELPERS_CUSTOMER_LINE_2;
			break;
			/////////////////////////////////////////////////////////////////////////////
			case (TERM_IUC285): // iUC285 ///////////////////////////////////////////////
				*pDisplayClessLogoInternal = FALSE;
			*pInstructionLineCardholder = HELPERS_CUSTOMER_LINE_3;
			*pAmountLineCardholder = HELPERS_CUSTOMER_LINE_2;
			break;
			/////////////////////////////////////////////////////////////////////////////
			case (TERM_IPP480): // iPP480 ///////////////////////////////////////////////
				*pDisplayClessLogoInternal = TRUE;
			*pInstructionLineCardholder = ((Cless_Goal_IsAvailable()) ? (HELPERS_CUSTOMER_LINE_5) : (HELPERS_CUSTOMER_LINE_6));
			*pAmountLineCardholder = HELPERS_CUSTOMER_LINE_1;
			*pFontSizeCustomer = HELPERS_FONT_SIZE_MEDIUM;
			break;
			/////////////////////////////////////////////////////////////////////////////
			case (TERM_IWL350): // iWL350 ///////////////////////////////////////////////
				*pDisplayClessLogoInternal = TRUE;
			*pInstructionLineCardholder = ((Cless_Goal_IsAvailable()) ? (HELPERS_CUSTOMER_LINE_5) : (HELPERS_CUSTOMER_LINE_6));
			*pAmountLineCardholder = HELPERS_CUSTOMER_LINE_1;
			*pFontSizeCustomer = HELPERS_FONT_SIZE_MEDIUM;
			break;
			/////////////////////////////////////////////////////////////////////////////
			case (TERM_ISC480): // iSC480 ///////////////////////////////////////////////
				*pDisplayClessLogoInternal = FALSE;
			*pInstructionLineCardholder = ((Cless_Goal_IsAvailable()) ? (HELPERS_CUSTOMER_LINE_5) : (HELPERS_CUSTOMER_LINE_3));
			*pAmountLineCardholder = HELPERS_CUSTOMER_LINE_2;
			break;

			/////////////////////////////////////////////////////////////////////////////
			case (TERM_ICM122): // iCMP /////////////////////////////////////////////////
				*pDisplayClessLogoInternal = TRUE;
			*pInstructionLineCardholder = ((Cless_Goal_IsAvailable()) ? (HELPERS_CUSTOMER_LINE_5) : (HELPERS_CUSTOMER_LINE_6));
			*pAmountLineCardholder = HELPERS_CUSTOMER_LINE_5;
			*pFontSizeCustomer = HELPERS_FONT_SIZE_MEDIUM;
			break;

			/////////////////////////////////////////////////////////////////////////////
			case (TERM_IWB220): // iWB220 ///////////////////////////////////////////////
				*pDisplayClessLogoInternal = TRUE;
			*pInstructionLineCardholder = HELPERS_CUSTOMER_LINE_2;
			*pAmountLineCardholder = HELPERS_CUSTOMER_LINE_1;
			*pFontSizeCustomer = HELPERS_FONT_SIZE_MEDIUM;
			break;

			/////////////////////////////////////////////////////////////////////////////
			case (TERM_IDE280): // iDE280 ///////////////////////////////////////////////
				*pDisplayClessLogoInternal = FALSE;
			*pDisplayClessLogoExternal = TRUE;
			*pAmountLineMerchant = HELPERS_CUSTOMER_LINE_2;
			*pInstructionLineMerchant = HELPERS_CUSTOMER_LINE_3;
			*pInstructionLineCardholder = HELPERS_CUSTOMER_LINE_2;
			*pAmountLineCardholder = HELPERS_CUSTOMER_LINE_1;
			*pFontSizeCustomer = HELPERS_FONT_SIZE_MEDIUM;
			break;

			/////////////////////////////////////////////////////////////////////////////
			case (TERM_IMP550): // iSMP3 /////////////////////////////////////////////////
				*pDisplayClessLogoInternal = TRUE;
			*pInstructionLineCardholder = HELPERS_CUSTOMER_LINE_2;
			*pAmountLineCardholder = HELPERS_CUSTOMER_LINE_1;
			*pFontSizeCustomer = HELPERS_FONT_SIZE_MEDIUM;
			break;

			/////////////////////////////////////////////////////////////////////////////
			default:
				*pDisplayClessLogoInternal = TRUE;
				*pInstructionLineCardholder = ((Cless_Goal_IsAvailable()) ? (HELPERS_CUSTOMER_LINE_5) : (HELPERS_CUSTOMER_LINE_6));
				*pAmountLineCardholder = HELPERS_CUSTOMER_LINE_1;
				*pFontSizeCustomer = HELPERS_FONT_SIZE_MEDIUM;
				break;
			}
		}
		else if (nClessDeviceType == TPASS_CLESS_TYPE_P30C)
		{
			// P30 /////////////////////////////////////////////////
			*pDisplayClessLogoExternal = TRUE;
			*pAmountLineMerchant = HELPERS_CUSTOMER_LINE_2;
			*pInstructionLineMerchant = HELPERS_CUSTOMER_LINE_3;
			*pAmountLineCardholder = HELPERS_CUSTOMER_LINE_1;
			*pInstructionLineCardholder = HELPERS_CUSTOMER_LINE_2;
		}
		else if (nClessDeviceType == TPASS_CLESS_TYPE_IPP3XX_EMUL_P30C)
		{
			// iPP320 in transparent mode, iPP350 in transparent mode /////////////////////////////////////////////////
			*pDisplayClessLogoExternal = TRUE;
			*pAmountLineMerchant = HELPERS_CUSTOMER_LINE_2;
			*pInstructionLineMerchant = HELPERS_CUSTOMER_LINE_3;
			*pAmountLineCardholder = HELPERS_CUSTOMER_LINE_1;
			*pInstructionLineCardholder = HELPERS_CUSTOMER_LINE_2;
			*pFontSizeCustomer = HELPERS_FONT_SIZE_MEDIUM;
		}
		else if (nClessDeviceType == TPASS_CLESS_TYPE_IPP2XX)
		{
			// iPP220, iPP280 /////////////////////////////////////////////////
			*pAmountLineMerchant = HELPERS_CUSTOMER_LINE_2;
			*pInstructionLineMerchant = HELPERS_CUSTOMER_LINE_3;
			*pAmountLineCardholder = HELPERS_CUSTOMER_LINE_2;
			*pInstructionLineCardholder = HELPERS_CUSTOMER_LINE_3;
		}
		else // TP+, VP, iUC150, iST150 /////////////////////////////////////////////////
		{
			*pAmountLineMerchant = HELPERS_CUSTOMER_LINE_2;
			*pInstructionLineMerchant = HELPERS_CUSTOMER_LINE_3;
			*pAmountLineCardholder = HELPERS_CUSTOMER_LINE_2;
			*pInstructionLineCardholder = HELPERS_CUSTOMER_LINE_3;
		}
	}
}
