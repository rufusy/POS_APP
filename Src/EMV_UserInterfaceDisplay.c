/**
 * \file
 * \brief This module manages the user interface.
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
#include "GL_GraphicLib.h"
#include "TlvTree.h"
#include "GTL_Assert.h"
#include "GTL_Convert.h"

#include "EPSTOOL_Convert.h"
#include "EPSTOOL_TlvTree.h"
#include "EPSTOOL_PinEntry.h"
#include "EPSTOOL_Unicode.h"

#include "EMV_Status.h"
#include "EMV_ApiTags.h"
#include "EMV_Api.h"

#include "EMV_UserInterface.h"
#include "EMV_UserInterfaceDisplay.h"

#include <globals.h>

//// Macros & preprocessor definitions //////////////////////////

// Account Type Selection
#define  ACCOUNT_TYPE_DEFAULT						0x00 	//!< Default account type
#define  ACCOUNT_TYPE_SAVINGS						0x10	//!< Saving account
#define  ACCOUNT_TYPE_CHEQUE_DEBIT					0x20	//!< Check or debit account
#define  ACCOUNT_TYPE_CREDIT						0x30	//!< Credit account

//// Types //////////////////////////////////////////////////////

//// Static function definitions ////////////////////////////////

static void __EMV_UI_PinEntry_Refresh(struct EPSTOOL_PinEntry_Infos_t *interface, T_GL_HWIDGET label, int pinLength);

//// Global variables ///////////////////////////////////////////

//// Functions //////////////////////////////////////////////////

//! \brief Setup the user interface environment to perform a transaction.
int EMV_UI_TransactionBegin(void) {
	int result;

	result = 0;
	if (DisplayHeader(_OFF_) == _ON_)
		result |= 0x01;
	if (DisplayFooter(_OFF_) == _ON_)
		result |= 0x02;

	GL_GraphicLib_OpenApplicationWindow(EMV_UI_GoalHandle());

	return result;
}

//! \brief Restore the user interface environment as it was before the transaction.
void EMV_UI_TransactionEnd(int previous)
{
	DisplayHeader((previous & 0x01) ? _ON_ : _OFF_);
	DisplayFooter((previous & 0x02) ? _ON_ : _OFF_);

	GL_GraphicLib_CloseApplicationWindow(EMV_UI_GoalHandle());
}

//! \brief Display the "Insert card" message.
void EMV_UI_MessageInsertCard(void) {
	EMV_UI_GoalOpen();
	GL_Dialog_Message(EMV_UI_GoalHandle(), NULL, "Insert, Swipe or\n tap card", GL_ICON_INFORMATION, GL_BUTTON_NONE, 0);
	EMV_UI_GoalClose();
}


//! \brief Display the "Remove card" message.
void EMV_UI_MessageRemoveCard(void)
{
	EMV_UI_GoalOpen();
	GL_Dialog_Message(EMV_UI_GoalHandle(), NULL, "Remove card", GL_ICON_INFORMATION, GL_BUTTON_NONE, 0);
	EMV_UI_GoalClose();
}

//! \brief Display the "Please wait" message.
void EMV_UI_MessagePleaseWait(void)
{
	EMV_UI_GoalOpen();
	GL_Dialog_Message(EMV_UI_GoalHandle(), NULL, "Please wait", GL_ICON_INFORMATION, GL_BUTTON_NONE, 0);
	EMV_UI_GoalClose();
}

//! \brief Display the "No remaining PIN" message.
void EMV_UI_MessageNoRemainingPin(void)
{
	EMV_UI_GoalOpen();
	GL_Dialog_Message(EMV_UI_GoalHandle(), NULL, "No remaining PIN", GL_ICON_WARNING, GL_BUTTON_NONE, GL_TIME_SECOND);
	EMV_UI_GoalClose();
}

//! \brief Display the "Wrong PIN" message.
void EMV_UI_MessageWrongPin(void)
{
	EMV_UI_GoalOpen();
	GL_Dialog_Message(EMV_UI_GoalHandle(), NULL, "Wrong PIN", GL_ICON_WARNING, GL_BUTTON_NONE, 2 * GL_TIME_SECOND);
	EMV_UI_GoalClose();
}

//! \brief Display the "Last attempt" message.
void EMV_UI_MessagePinLastAttempt(void)
{
	EMV_UI_GoalOpen();
	GL_Dialog_Message(EMV_UI_GoalHandle(), NULL, "Last attempt", GL_ICON_WARNING, GL_BUTTON_NONE, 2 * GL_TIME_SECOND);
	EMV_UI_GoalClose();
}

//! \brief Display the "Correct PIN" message.
void EMV_UI_MessageCorrectPin(void)
{
	EMV_UI_GoalOpen();
	GL_Dialog_Message(EMV_UI_GoalHandle(), NULL, "Correct PIN", GL_ICON_INFORMATION, GL_BUTTON_NONE, 2 * GL_TIME_SECOND);
	EMV_UI_GoalClose();
}

//! \brief Display the "Approved" message.
void EMV_UI_MessageApproved(void)
{
	EMV_UI_GoalOpen();
	GL_Dialog_Message(EMV_UI_GoalHandle(), NULL, "Approved", GL_ICON_INFORMATION, GL_BUTTON_NONE, 2 * GL_TIME_SECOND);
	EMV_UI_GoalClose();
}

//! \brief Display the "Declined" message.
void EMV_UI_MessageDeclined(void)
{
	EMV_UI_GoalOpen();
	GL_Dialog_Message(EMV_UI_GoalHandle(), NULL, "Declined", GL_ICON_ERROR, GL_BUTTON_NONE, GL_TIME_SECOND);
	EMV_UI_GoalClose();
}

//! \brief Display the "Service not allowed" message.
void EMV_UI_MessageServiceNotAllowed(void)
{
	EMV_UI_GoalOpen();
	GL_Dialog_Message(EMV_UI_GoalHandle(), NULL, "Service\nnot allowed", GL_ICON_ERROR, GL_BUTTON_NONE, 2 * GL_TIME_SECOND);
	EMV_UI_GoalClose();
}

//! \brief Display the "Transaction Cancelled" message.
void EMV_UI_MessageTransactionCancelled(void)
{
	EMV_UI_GoalOpen();
	GL_Dialog_Message(EMV_UI_GoalHandle(), NULL, "Transaction\ncancelled", GL_ICON_ERROR, GL_BUTTON_NONE, 2 * GL_TIME_SECOND);
	EMV_UI_GoalClose();
}

//! \brief Display the "Card blocked" message.
void EMV_UI_MessageCardBlocked(void)
{
	EMV_UI_GoalOpen();
	GL_Dialog_Message(EMV_UI_GoalHandle(), NULL, "Card blocked", GL_ICON_ERROR, GL_BUTTON_NONE, 2 * GL_TIME_SECOND);
	EMV_UI_GoalClose();
}

//! \brief Display the "Card removed" message.
void EMV_UI_MessageCardRemoved(void)
{
	EMV_UI_GoalOpen();
	GL_Dialog_Message(EMV_UI_GoalHandle(), NULL, "Card removed", GL_ICON_ERROR, GL_BUTTON_NONE, 2 * GL_TIME_SECOND);
	EMV_UI_GoalClose();
}

//! \brief Display the "Invalid card" message.
void EMV_UI_MessageCardError(void)
{
	EMV_UI_GoalOpen();
	GL_Dialog_Message(EMV_UI_GoalHandle(), NULL, "Invalid card", GL_ICON_ERROR, GL_BUTTON_NONE, 2 * GL_TIME_SECOND);
	EMV_UI_GoalClose();
}

//! \brief Display the "Invalid card" message.
void EMV_UI_MessageDisplayError(char * DisplayData){
	EMV_UI_GoalOpen();
	GL_Dialog_Message(EMV_UI_GoalHandle(), NULL, "Invalid card", GL_ICON_ERROR, GL_BUTTON_NONE, 2 * GL_TIME_SECOND);
	EMV_UI_GoalClose();
}

//! \brief Display the "Transaction error" message.
void EMV_UI_MessageTransactionError(void)
{
	EMV_UI_GoalOpen();
	GL_Dialog_Message(EMV_UI_GoalHandle(), NULL, "Transaction\nerror", GL_ICON_ERROR, GL_BUTTON_NONE, 2 * GL_TIME_SECOND);
	EMV_UI_GoalClose();
}


//! \brief Display the "Authorisation in progress" message with a progress bar.
//! \return \a TRUE if successful, \a FALSE if it has been cancelled.
//! \remarks This function is used to simulate an authorisation of 1 second.
int EMV_UI_MessageAuthorisation(void) {
	int ret = FALSE;

	ret = performOlineTransaction();

	return ret;
}


//! \brief Perform the signature capture and stores the signature in a file.
void EMV_UI_RequestSignature(void)
{
	int previousHeader;
	int previousFooter;

	if (GL_GraphicLib_IsTouchPresent(EMV_UI_GoalHandle()))
	{
		EMV_UI_GoalOpen();

		// Remove the header and footer if present
		previousHeader = DisplayHeader(_OFF_);
		previousFooter = DisplayFooter(_OFF_);

		// Request for signature
		// TODO: Store the signature with the transaction data
		GL_Dialog_Signature(EMV_UI_GoalHandle(), NULL, "Signature", "file://flash/HOST/SIGNATURE.SIG", GL_TIME_MINUTE);
		GL_File_Delete( "file://flash/HOST/SIGNATURE.SIG" );

		// Restore the header and footer context
		DisplayFooter(previousFooter);
		DisplayHeader(previousHeader);

		EMV_UI_GoalClose();
	}
}

//! \brief Display a screen to select an AID.
//! \param[in] numOfAids Number of AIDs.
//! \param[in] aids Possible AIDs.
//! \return Index of the selected AID (starting from 0), or (-1) if the input is cancelled, or (-2) if there is not enough memory.
int EMV_UI_MenuSelectAid(int numOfAids, const EMV_UI_SelectAidItem_t *aids) {
	int result;
	char **menu;
	int index;
	int usePreferredName;

	ASSERT(numOfAids > 0);
	ASSERT(aids != NULL);

	// Allocate memory to fill with the menu data
	menu = umalloc((numOfAids + 1) * sizeof(char*));
	if (menu != NULL) {
		memclr(menu, (numOfAids + 1) * sizeof(char*));

		// Set the menu item with the Application Label or with to Application Preferred Name
		result = 0;
		index = 0;
		while((index < numOfAids) && (result >= 0)) {
			// Check if label or preferred name must be used
			usePreferredName = FALSE;
			if (aids[index].preferredName[0] != '\0') {
				// There is a Preferred Name

				// TODO: Check if the Issuer Code Table Index is supported or not.
				// By default, GOAL support ISO8859 1, 2, 3, 4, 9, 10, 13, 14, 15, 16.
				// But EMVCo limits it from ISO8859 1 to 10.
				switch(aids[index].issuerCodeTableIndex) {
				case 1:
				case 2:
				case 3:
				case 4:
				case 9:
				case 10:
					usePreferredName = TRUE;
					break;
				}
			}

			if (usePreferredName) {
				// Use the Application Preferred Name
				// Convert 'aids[index].preferredName' into UTF-8
				menu[index] = EPSTOOL_Unicode_CharsetToNewUtf8(aids[index].preferredName,
						sizeof(aids[index].preferredName),
						aids[index].issuerCodeTableIndex - 1 + EPSTOOL_UNICODE_ISO_8859_1);
				if (menu[index] == NULL) {
					result = -2;
				}
			} else if (aids[index].applicationLabel[0] != '\0') {
				// Use the Application Label (there is no need to convert it in UTF-8 as it is ASCII with no special characters)
				menu[index] = umalloc(strlen(aids[index].applicationLabel) + 1);
				if (menu[index] != NULL) {
					strcpy(menu[index], aids[index].applicationLabel);
				} else {
					result = -2;
				}
			} else {
				// There is neither Application Label nor Application Preferred Name
				// Use the AID instead
				menu[index] = umalloc(2 * aids[index].aidLength + 1);
				if (menu[index] != NULL) {
					// Convert the AID into an ASCII string
					GTL_Convert_DcbToAscii(aids[index].aid, menu[index], 0, 2 * aids[index].aidLength);
				} else {
					result = -2;
				}
			}
			index++;
		}

		if (index >= 0) {
			EMV_UI_GoalOpen();

			// Display the menu
			// Note that last item 'menu[numOfAids]' is equal to NULL because of the memclr(menu)
			result = GL_Dialog_Menu(EMV_UI_GoalHandle(), "Select Application",
					(const char * const *)menu, 0, GL_BUTTON_VALID_CANCEL, GL_KEY_NONE, 2 * GL_TIME_MINUTE);
			if ((result < 0) || (result > numOfAids)) {
				// Cancelled
				result = -1;
			}

			EMV_UI_GoalClose();
		}

		// Free the memory
		for(index = 0; index < numOfAids; index++) {
			if (menu[index] != NULL) {
				ufree(menu[index]);
			}
		}
		ufree(menu);
	} else {
		result = -2;
	}

	return result;
}

//! \brief Select the account type.
//! \param[out] accountType The selected account type:
//! - \a ACCOUNT_TYPE_DEFAULT for "No choice".
//! - \a ACCOUNT_TYPE_CHEQUE_DEBIT for check or debit account.
//! - \a ACCOUNT_TYPE_CREDIT for a credit account.
//! - \a ACCOUNT_TYPE_SAVINGS for a saving account.
//! \return \a TRUE if an account is selected, \a FALSE if the input is not correct or cancelled.
int EMV_UI_MenuSelectAccountType(unsigned char *accountType) {
	//	static const char *menu[] = {
	//			"No choice",
	//			"Check / Debit",
	//			"Credit",
	//			"Saving",
	//			NULL
	//	};
	int result;

	ASSERT(accountType != NULL);

	//	EMV_UI_GoalOpen();

	result = TRUE;


	*accountType = ACCOUNT_TYPE_DEFAULT;

	//	switch(GL_Dialog_Menu(EMV_UI_GoalHandle(), "Account Type", menu, 0, GL_BUTTON_VALID_CANCEL, GL_KEY_NONE, 2 * GL_TIME_MINUTE)) {
	//	case 0:
	//		*accountType = ACCOUNT_TYPE_DEFAULT;
	//		break;
	//	case 1:
	//		*accountType = ACCOUNT_TYPE_CHEQUE_DEBIT;
	//		break;
	//	case 2:
	//		*accountType = ACCOUNT_TYPE_CREDIT;
	//		break;
	//	case 3:
	//		*accountType = ACCOUNT_TYPE_SAVINGS;
	//		break;
	//	default:
	//		*accountType = ACCOUNT_TYPE_DEFAULT;
	//		result = FALSE;
	//		break;
	//	}
	//
	//	EMV_UI_GoalClose();

	return result;
}

//! \brief Ask for an amount.
//! \param[out] amount Entered amount.
//! \return \a TRUE if the amount is entered, \a FALSE if not (cancelled ...).
int EMV_UI_AmountEntry(unsigned long *amount) {
	int result = 0,ret = 0;
	const char entryMask[] = "/d/d/d,/d/d/d,/d/d/D./D/D";
	char currencyLabel[3 + 1] ;
	char amountString[32 + 1];
	char CashBackAmtString[32 + 1];
	char MnuStr[5];
	card MnuItem = 0;

	memset(MnuStr, 0, sizeof(MnuStr));
	memset(CashBackAmtString, 0, sizeof(CashBackAmtString));

	MAPGET(traMnuItm, MnuStr, lblKO);
	dec2num(&MnuItem, MnuStr,0);
	switch (MnuItem) {
	case mnuBalanceEnquiry:
	case mnuMiniStatement:
		memset(amountString, 0, sizeof(amountString));
		strcpy(amountString, "000000000000");
		EPSTOOL_Convert_AsciiToUl(amountString, -1 , amount);

		MAPPUTSTR(traAmt, "000000000000", lblKO);
		MAPPUTSTR(traOtherAmt, "000000000000", lblKO);
		result = TRUE;
		break;
	default:

		ASSERT(amount != NULL);

		memset(amountString, 0, sizeof(amountString));
		memset(currencyLabel, 0, sizeof(currencyLabel));

		MAPGET(traCurrencyLabel, currencyLabel, lblKO);
		MAPGET(traAmt, amountString, lblKO);

		EMV_UI_GoalOpen();

		if (strlen(amountString)>1) {
			MAPGET(traTotAmt, amountString, lblKO); //Get the whole amount totake care of the CashBack
			EPSTOOL_Convert_AsciiToUl(amountString, -1 , amount);
			result = TRUE;
		} else {

			result = (GL_Dialog_Amount(EMV_UI_GoalHandle(), NULL, "Amount:", entryMask, amountString, sizeof(amountString) - 1, currencyLabel, GL_ALIGN_RIGHT, 2 * GL_TIME_MINUTE) == GL_KEY_VALID);
			if (result) {
				MAPPUTSTR(traAmt, amountString, lblKO);

				ComputeTotAmt();

				MAPGET(traTotAmt, amountString, lblKO); //Get the whole amount totake care of the CashBack

				EPSTOOL_Convert_AsciiToUl(amountString, -1 , amount);
			}
		}
		lblKO:
		EMV_UI_GoalClose();

		break;
	}

	ComputeTotAmt();

	return result;
}

//! \brief Called when the display must be updated during a PIN entry.
//! \param[in] interface PIN entry parameters.
//! \param[in] label Handle of the GOAL widget that is used to display the PIN entry.
//! \param[in] pinLength Current length of the entered PIN.
static void __EMV_UI_PinEntry_Refresh(struct EPSTOOL_PinEntry_Infos_t *interface, T_GL_HWIDGET label, int pinLength) {
	const char defaultString[] = "- - - -";
	char string[64];
	int index;

	ASSERT(interface != NULL);
	ASSERT(label != NULL);

	if (pinLength <= 0) {
		// No PIN entered => display a default message
		GL_Widget_SetText(label, defaultString);
	} else {
		// PIN entry is in progress
		//  => Update the display

		// Check PIN length to avoid buffer overflows
		if (pinLength > (sizeof(string) / 2))
			pinLength = sizeof(string) / 2;

		// Format a string to have a star '*' and a space ' ' for each digit
		// Ex: for a 4 digits PIN, the string will be "* * * *"
		for(index = 0; index < pinLength; index++) {
			string[2 * index] = '*';
			string[(2 * index) + 1] = ' ';
		}
		string[(2 * (pinLength - 1)) + 1] = '\0';

		// Display the string that contains the stars
		GL_Widget_SetText(label, string);
	}
}

//! \brief Ask for a PIN entry.
//! \param[in] pinType \a EMV_INT_CV_STATUS_OFFLINE_PIN for an offline PIN entry or \a EMV_INT_CV_STATUS_ONLINE_PIN for an online PIN entry.
//! \param[in] pinTryCounter The PIN try counter from the card. Give (-1) if unknown.
//! \param[out] pinLength Length of entered PIN.
//! \return Any value of \ref EPSTOOL_PinEntry_Status_e.
EPSTOOL_PinEntry_Status_e EMV_UI_PinEntry(int pinType, int pinTryCounter, int *pinLength) {
	EPSTOOL_PinEntry_Infos_t pinEntryInfo;
	EPSTOOL_PinEntry_Status_e status;
	char CardAID[100];

	EMV_UI_GoalOpen();

	memset(CardAID, 0, sizeof(CardAID));
	mapGet(traAID, CardAID, 1);
	//	if (strncmp(CardAID, "A000000333010101", 16) == 0) {
	//		pinEntryInfo.pinEntryConfig.ucMinDigits = 4;
	//		pinEntryInfo.pinBypassAllowed = FALSE;
	//	} else {
	pinEntryInfo.pinEntryConfig.ucMinDigits = 0;
	pinEntryInfo.pinBypassAllowed = TRUE;
	//	}

	// Setup the PIN entry parameters
	pinEntryInfo.pinEntryConfig.ucEchoChar = '*';
	pinEntryInfo.pinEntryConfig.ucFontWidth = 0;
	pinEntryInfo.pinEntryConfig.ucFontHeight = 0;
	pinEntryInfo.pinEntryConfig.ucEchoLine = 0;
	pinEntryInfo.pinEntryConfig.ucEchoColumn = 0;
	pinEntryInfo.pinEntryConfig.ucMaxDigits = 12;
	pinEntryInfo.pinEntryConfig.iFirstCharTimeOut = 60000;
	pinEntryInfo.pinEntryConfig.iInterCharTimeOut = 10000;

	if (pinType == EMV_INT_CV_STATUS_OFFLINE_PIN) {
		mapPut(traCVMused, "OFL", 3);
		// Offline PIN goes to the card reader
		pinEntryInfo.secureType = C_SEC_CARD;
	} else {
		mapPut(traCVMused, "ONL", 3);
		// Online PIN goes to the PIN encryption module
		pinEntryInfo.secureType = C_SEC_PINCODE;
	}
	// TODO: Set here if PIN bypass is allowed or not
	// TODO: Set here some cancel events (CAM0 ...)
	pinEntryInfo.eventsToWait = CAM0 | CAM2;

	// Customisation functions
	pinEntryInfo.userEvents = NULL;
	pinEntryInfo.refresh = __EMV_UI_PinEntry_Refresh;
	pinEntryInfo.privateData = NULL;

	do {
		// Ask for the PIN
		if (pinTryCounter == 1) {
			status = EPSTOOL_PinEntry(EMV_UI_GoalHandle(), NULL, "Last Try", "Enter your PIN\nsafe from prying eyes", &pinEntryInfo, pinLength);
		} else {
			status = EPSTOOL_PinEntry(EMV_UI_GoalHandle(), NULL, "Enter PIN", "Enter your PIN\nsafe from prying eyes", &pinEntryInfo, pinLength);
		}

		// If the PIN is not valid (too short), restart the PIN entry
		if (status == EPSTOOL_PINENTRY_INVALID) {
			GL_Dialog_Message(EMV_UI_GoalHandle(), NULL, "Invalid PIN", GL_ICON_WARNING, GL_BUTTON_NONE, 2 * GL_TIME_SECOND);
		}
	} while(status == EPSTOOL_PINENTRY_INVALID);

	EMV_UI_GoalClose();

	return status;
}
