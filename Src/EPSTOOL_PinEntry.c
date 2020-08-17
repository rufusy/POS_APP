/**
 * \file
 * \brief This module implements the PIN entry common treatments.
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
#include "SEC_interface.h"
#include "schVar_def.h"
#include "GL_GraphicLib.h"
#include "GTL_Assert.h"

#include "EPSTOOL_PinEntry.h"
#include "globals.h"
#include "dukpt.h"

//// Macros & preprocessor definitions //////////////////////////

//// Types //////////////////////////////////////////////////////

extern T_GL_HGRAPHIC_LIB hGoal; // Handle of the graphics object library

//! \brief Context of a PIN entry.
typedef struct
{
	EPSTOOL_PinEntry_Infos_t *infos;		//!< PIN entry parameters.

	int firstRefresh;						//!< First refresh flag.
	int pinLength;							//!< Current length of the entered PIN.
	EPSTOOL_PinEntry_Status_e status;		//!< Status of the PIN entry.
} __EPSTOOL_PinEntry_Context_t;

//// Static function definitions ////////////////////////////////

static unsigned long __EPSTOOL_PinEntry_Open(T_GL_HSCHEME_INTERFACE interface);
static void __EPSTOOL_PinEntry_Close(T_GL_HSCHEME_INTERFACE interface);
static unsigned long __EPSTOOL_PinEntry_Refresh(T_GL_HSCHEME_INTERFACE interface, T_GL_HWIDGET label);

//// Global variables ///////////////////////////////////////////

//// Functions //////////////////////////////////////////////////

//! \brief Start a GOAL PIN entry.
//! \param[in] interface GOAL PIN entry context.
//! \return \a GL_RESULT_SUCCESS if PIN entry can be performed or \a GL_RESULT_FAILED to abort PIN entry.
static unsigned long __EPSTOOL_PinEntry_Open(T_GL_HSCHEME_INTERFACE interface) {
	__EPSTOOL_PinEntry_Context_t *pinEntryContext;
	T_SEC_ENTRYCONF pinEntryConfig;
	int result;

	ASSERT(interface != NULL);
	pinEntryContext = (__EPSTOOL_PinEntry_Context_t*)interface->privateData;

	ASSERT(pinEntryContext != NULL);
	ASSERT(pinEntryContext->infos != NULL);

	if (pinEntryContext->infos->pinBypassAllowed) {
		// Force the scheme min PIN length to 0
		memcpy(&pinEntryConfig, &pinEntryContext->infos->pinEntryConfig, sizeof(pinEntryConfig));
		pinEntryConfig.ucMinDigits = 0;
		// Initialise the PIN entry
		result = SEC_PinEntryInit(&pinEntryConfig, pinEntryContext->infos->secureType);
	} else {
		// Initialise the PIN entry
		result = SEC_PinEntryInit(&pinEntryContext->infos->pinEntryConfig, pinEntryContext->infos->secureType);
	}

	if (result == OK) {
		return GL_RESULT_SUCCESS;
	} else {
		return GL_RESULT_FAILED;
	}
}

//! \brief End a GOAL PIN entry.
//! \param[in] interface GOAL PIN entry context.
static void __EPSTOOL_PinEntry_Close(T_GL_HSCHEME_INTERFACE interface)
{
	unsigned int eventsToWait;
	unsigned char key;
	int continuePinEntry;

	ASSERT(interface != NULL);

	// Stop the PIN entry
	eventsToWait = 0;
	key = 0;
	continuePinEntry = FALSE;
	SEC_PinEntry(&eventsToWait, &key, &continuePinEntry);
}

//! \brief Refresh the display of a GOAL PIN entry.
//! \param[in] interface GOAL PIN entry context.
//! \param[in] label Handle of the GOAL widget that displays the PIN entry.
//! \return \a GL_RESULT_SUCCESS if success or any other status code to abort PIN entry.
static unsigned long __EPSTOOL_PinEntry_Refresh(T_GL_HSCHEME_INTERFACE interface, T_GL_HWIDGET label) {
	unsigned long result;
	__EPSTOOL_PinEntry_Context_t *pinEntryContext;
	unsigned int eventsToWait;
	unsigned char key;
	int continuePinEntry;
	int updateDisplay;
	unsigned long events;

	ASSERT(interface != NULL);
	pinEntryContext = (__EPSTOOL_PinEntry_Context_t*)interface->privateData;

	ASSERT(pinEntryContext != NULL);
	ASSERT(pinEntryContext->infos != NULL);
	ASSERT(pinEntryContext->infos->refresh != NULL);
	ASSERT(pinEntryContext->pinLength >= 0);
	ASSERT(pinEntryContext->pinLength <= pinEntryContext->infos->pinEntryConfig.ucMaxDigits);

	if (pinEntryContext->firstRefresh) {
		// First call of this function
		//  => Call the user 'refresh' function once before starting PIN entry
		ASSERT(pinEntryContext->pinLength == 0);
		pinEntryContext->infos->refresh(pinEntryContext->infos, label, pinEntryContext->pinLength);
		pinEntryContext->firstRefresh = FALSE;

		// Return to let GOAL update the display
		return GL_RESULT_SUCCESS;
	}

	result = GL_RESULT_SUCCESS;

	// Manage PIN entry
	eventsToWait = pinEntryContext->infos->eventsToWait;
	key = 0;
	continuePinEntry = TRUE;
	updateDisplay = FALSE;
	switch(SEC_PinEntry(&eventsToWait, &key, &continuePinEntry)) {
	case OK:
		if (key == pinEntryContext->infos->pinEntryConfig.ucEchoChar) {
			// A numeric key has been pressed
			//  => Increase the PIN length (unless max digits has already been entered)
			if (pinEntryContext->pinLength < pinEntryContext->infos->pinEntryConfig.ucMaxDigits) {
				pinEntryContext->pinLength++;
				updateDisplay = TRUE;
			}
		} else if (key == T_CORR) {
			// The correction key has been pressed
			//  => Decrease the PIN length (unless no digits has already been entered)
			if (pinEntryContext->pinLength > 0)
			{
				pinEntryContext->pinLength--;
				updateDisplay = TRUE;
			}
		} else if (key == T_ANN) {
			// The cancel key has been pressed
			pinEntryContext->status = EPSTOOL_PINENTRY_CANCEL;
			result = GL_KEY_CANCEL;
		} else if (key == T_VAL) {

			mapPutCard(traOnlinePinLen, pinEntryContext->pinLength);
			// The cancel key has been pressed
			if ((pinEntryContext->pinLength == 0) && (pinEntryContext->infos->pinBypassAllowed)) {
				// PIN bypass
				pinEntryContext->status = EPSTOOL_PINENTRY_BYPASS;
				// Stop the PIN entry
				result = GL_KEY_CANCEL;
			} else if (pinEntryContext->pinLength >= pinEntryContext->infos->pinEntryConfig.ucMinDigits) {
				// Terminate the PIN entry
				pinEntryContext->status = EPSTOOL_PINENTRY_SUCCESS;
				result = GL_KEY_VALID;
			} else {
				// Invalid PIN (too short)
				pinEntryContext->status = EPSTOOL_PINENTRY_INVALID;
				result = GL_KEY_CANCEL;
			}
		} else if (key == 0) {
			// No custom 'user event' function => Stop the PIN entry
			pinEntryContext->status = EPSTOOL_PINENTRY_TIMEOUT;
			result = GL_RESULT_INACTIVITY;
		}

		// Update the display
		if (updateDisplay) {
			ASSERT(pinEntryContext->infos->refresh != NULL);
			pinEntryContext->infos->refresh(pinEntryContext->infos, label, pinEntryContext->pinLength);
		}
		break;

	case ERR_TIMEOUT:
		events = (eventsToWait & pinEntryContext->infos->eventsToWait);
		if (events != 0)
		{
			// User event
			if (pinEntryContext->infos->userEvents != NULL)
			{
				if (pinEntryContext->infos->userEvents(pinEntryContext->infos, label, events, pinEntryContext->pinLength))
				{
					// Continue the PIN entry
					result = GL_RESULT_SUCCESS;
				}
				else
				{
					// Stop the PIN entry
					pinEntryContext->status = EPSTOOL_PINENTRY_EVENT;
					result = GL_KEY_CANCEL;
				}
			}
			else
			{
				// No custom 'user event' function => Stop the PIN entry
				pinEntryContext->status = EPSTOOL_PINENTRY_EVENT;
				result = GL_KEY_CANCEL;
			}
		}
		else
		{
			// Timeout from scheme => pinpad error
			pinEntryContext->status = EPSTOOL_PINENTRY_ERROR;
			result = GL_KEY_CANCEL;
		}
		break;

	case C_SEC_ERR_PINENTRY_CURRENT:
	default:
		// Error => stop the PIN entry
		pinEntryContext->status = EPSTOOL_PINENTRY_ERROR;
		result = GL_KEY_CANCEL;
		break;
	}

	key = 0;

	return result;
}

int EnterPin(void){
	byte bdk[32 + 1];
	int ret = 0;
	char pinCode[7];
	byte panBis[17];
	char Pin[16+1];
	char Pan[19+1];
	byte PinBlok[9];
	byte pcc[8]; //pin
	byte acc[8]; //account extracted from pan
	byte blk[8]; //pinblock calculated by cryptomodule
	byte vlPin[3];
	byte Padding[11]="FFFFFFFFFF";
	int  Plen;
	int i;
	int lenP;
	char curr[5 + 1];
	char mDisp[32 + 1];
	char amount[lenAmt+1];
	char formattedAmount[lenAmt+5];
	card amt;
	ulong result;
	byte ksn[32 + 1];
	byte IPEK[32 + 1];
	char bdkSTR[32 + 1];

	memset(formattedAmount, 0, sizeof(formattedAmount));
	memset(PinBlok, 0, sizeof(PinBlok));
	memset(amount, 0, sizeof(amount));
	memset(pinCode,0,sizeof(pinCode));
	memset(panBis, 0, sizeof(panBis));
	memset(bdkSTR, 0, sizeof(bdkSTR));
	memset(vlPin,0,sizeof(vlPin));
	memset(mDisp,0,sizeof(mDisp));
	memset(IPEK,0,sizeof(IPEK));
	memset(curr, 0, sizeof(curr));
	memset(Pan, 0, sizeof(Pan));
	memset(Pin, 0, sizeof(Pin));
	memset(pcc, 0, sizeof(pcc));
	memset(ksn, 0, sizeof(ksn));
	memset(bdk, 0, sizeof(bdk));
	memset(blk, 0, sizeof(blk));

//	T_GL_HGRAPHIC_LIB goalHandle = GL_GraphicLib_Create();
	//============ Enter PIN ==============
	_clrscr();

	MAPGET(traCtx,mDisp,lblDBA);
	fmtPad(mDisp, 18, ' ');

	//	MAPGET(traAmt,amount,lblDBA);
	MAPGET(traTotAmt,amount,lblDBA);
	dec2num(&amt, amount, 0);
	if(amt > 0)
		fmtAmt(formattedAmount, amount, 2,".,");
	strcat(mDisp, formattedAmount);

	curr[0] = ' ';
	MAPGET(traCurrencyLabel,&curr[1],lblDBA);
	strcat(mDisp, curr);

	result = GL_Dialog_Password(hGoal, mDisp, "Enter PIN:", "/d/d/d/d/d/d/d/d/d/d/d", pinCode, sizeof(pinCode), GL_TIME_MINUTE);
	if(result == GL_KEY_CANCEL || result == GL_RESULT_INACTIVITY)
		goto lblKO;

	Plen=strlen(pinCode);
	CHECK(Plen>=4,lblKO);
	ret= num2dec((char *)vlPin,Plen,2);

	//Format PIN
	memcpy(&Pin[0],vlPin,2);
	memcpy(&Pin[2],&pinCode[0],Plen);
	memcpy(&Pin[2+Plen],Padding,(14-Plen));
	hex2bin(pcc,Pin,8);

	//Format PAN
	ret= mapGet(traPan,Pan,19+1); CHECK(ret>=0,lblKO);
	memcpy(&panBis[0],"0000",4);
	lenP= strlen(Pan);

	memcpy(&panBis[4],&Pan[lenP-13],12);
	hex2bin(acc,(char *)panBis,8);

	//CALCULATE CRYPTOMODULE
	for ( i = 0 ; i < 8 ; i++)
		PinBlok[i]  = pcc[i] ^ acc[i];

	//Encrypt PinBlok for feild 52
	memset(IPEK, 0, sizeof(IPEK));
	memset(ksn, 0, sizeof(ksn));
	MAPGET(appDUKPT_BDK, bdkSTR, lblKO);
	hex2bin(bdk, bdkSTR, 16);
	DukptIncrementKsn();
	PAR_PADDED_KSN_Get(ksn);
	DukptGenerateIpek(IPEK, bdk, ksn);

	DukptData_t data_dec = {
			.Data = PinBlok,
			.Len = 8,
	};

	DukptData_t data_enc = {
			.Data = blk,
			.Len = 8,
	};

	DukptEncryptDataIpek( &data_enc, &data_dec, ksn, IPEK );
	//	DukptEncryptDataBdk( &data_enc, &data_dec, ksn, bdk );


	// Save feild 52
	ret= mapPut(traPinBlk,blk,8);
	CHECK(ret>=0,lblDBA);

//	GL_GraphicLib_Destroy(goalHandle);
	return 1;

	lblDBA:
//	GL_GraphicLib_Destroy(goalHandle);
	return -2;

	lblKO :
//	GL_GraphicLib_Destroy(goalHandle);
	return -1;
}

//! \brief Ask for a PIN.
//! \param[in] goalHandle The GOAL handle.
//! \param[in] title Title of the message box (or null if no title).
//! \param[in] text Text displayed (null hides text).
//! \param[in] help Help displayed (null hides text).
//! \param[in] pinEntryInfo PIN entry parameters.
//! \param[out] pinLength Length of the entered PIN.
//! \return Any value of \ref EPSTOOL_PinEntry_Status_e.
EPSTOOL_PinEntry_Status_e EPSTOOL_PinEntry(T_GL_HGRAPHIC_LIB goalHandle, const char *title, const char *text, const char *help, EPSTOOL_PinEntry_Infos_t *pinEntryInfo, int *pinLength) {
	int ret = 0;
	char CvmSelected[5];
	char amount[lenAmt + 1];
	char amountFormatted[128];
	T_GL_SCHEME_INTERFACE pinEntryInterface;
	__EPSTOOL_PinEntry_Context_t pinEntryContext;
	int Result = 0;
	card MnuItem = 0;
	char MnuStr[5];
	card outputPINLen = 0;
	char CardAID[100];

	ASSERT(goalHandle != NULL);
	ASSERT(pinEntryInfo != NULL);
	ASSERT(pinEntryInfo->refresh != NULL);

	memset(CvmSelected, 0, sizeof(CvmSelected));
	memset(amount, 0, sizeof(amount));
	memset(MnuStr, 0, sizeof(MnuStr));
	memset(amountFormatted, 0, sizeof(amountFormatted));
	memclr(&pinEntryInterface, sizeof(pinEntryInterface));
	memclr(&pinEntryContext, sizeof(pinEntryContext));

	MAPGET(traCVMused, CvmSelected, lblKO);
	MAPGET(traTotAmt, amount, lblKO);
	fmtAmt(amountFormatted, amount, 2, ".,");


	// Set PIN entry parameters
	pinEntryContext.infos = pinEntryInfo;
	pinEntryContext.firstRefresh = TRUE;
	pinEntryContext.pinLength = 4;
	pinEntryContext.status = EPSTOOL_PINENTRY_SUCCESS;

	// Ask for a PIN entry
	pinEntryInterface.open = &__EPSTOOL_PinEntry_Open;
	pinEntryInterface.close = &__EPSTOOL_PinEntry_Close;
	pinEntryInterface.refresh = &__EPSTOOL_PinEntry_Refresh;
	pinEntryInterface.privateData = &pinEntryContext;

	MAPGET(traMnuItm, MnuStr, lblKO);
	dec2num(&MnuItem, MnuStr,0);

	if (strncmp(CvmSelected,"ONL", 3) == 0) {

		RenterPin:

#ifdef INGENICO_PIN
		Result = FUN_PinEntry();
		if (Result < 0) {
			pinEntryContext.pinLength = 0;
			pinEntryContext.status = EPSTOOL_PINENTRY_ERROR;
		}
#else
		Result = EnterPin();
		if ((Result == GL_RESULT_INACTIVITY) || (Result == GL_KEY_CANCEL)) {
			pinEntryContext.pinLength = 0;
			pinEntryContext.status = EPSTOOL_PINENTRY_ERROR;
		}
#endif
	} else {

		lblKO:
		pinEntryContext.pinLength = 0;
		pinEntryContext.status = EPSTOOL_PINENTRY_ERROR;
		if ((MnuItem == mnuBalanceEnquiry) || MnuItem == mnuMiniStatement) {
			GL_Dialog_Scheme(goalHandle, title, NULL, help, &pinEntryInterface);
		} else {
			GL_Dialog_Scheme(goalHandle, title, amountFormatted, help, &pinEntryInterface);
		}
	}
	// Return the PIN length and PIN entry status
	if (pinLength != NULL) {
		*pinLength = pinEntryContext.pinLength;
		mapGetCard(traOnlinePinLen, outputPINLen);
		if ((outputPINLen == 0) && (pinEntryContext.infos->pinBypassAllowed == TRUE)) {
			pinEntryContext.status = EPSTOOL_PINENTRY_BYPASS;
			goto RenterPin;
		} else if (outputPINLen == 0) {

			memset(CardAID, 0, sizeof(CardAID));
			mapGet(traAID, CardAID, 1);
			if (strncmp(CardAID, "A000000333010101", 16) == 0) {
			}
			goto RenterPin;
		}
	}
	return pinEntryContext.status;
}
