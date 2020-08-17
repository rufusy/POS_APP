/**
 * \file
 * \brief This module implements main the EMV API functionalities.
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
#include "TlvTree.h"
#include "GTL_Assert.h"
#include "_emvdctag_.h"
#include "def_tag.h"
#include "EngineInterface.h"
#include "EngineInterfaceLib.h"

#include "EPSTOOL_Convert.h"
#include "EPSTOOL_TlvTree.h"

#include "EmvLib_Tags.h"
#include "EMV_Status.h"
#include "EMV_ApiTags.h"
#include "EMV_Api.h"

#include "EMV_ServicesEmv.h"
#include "EMV_ServicesEmv_AppSel.h"
#include "EMV_ServicesEmv_FinalSelect.h"
#include "EMV_ServicesEmv_Init.h"
#include "EMV_ServicesEmv_CVM.h"
#include "EMV_ServicesEmv_ActionAnalysis.h"
#include "EMV_ServicesEmv_OnlineProcessing.h"
#include "EMV_ServicesEmv_Stop.h"

#include "globals.h"

#include "Cless_Term.h"
#include "MessagesDefinitions.h"
#include "MSGlib.h"

//// Macros & preprocessor definitions //////////////////////////

extern T_GL_HGRAPHIC_LIB hGoal; // Handle of the graphics object library

//// Types //////////////////////////////////////////////////////

//// Static function definitions ////////////////////////////////

static EMV_Status_t __EMV_ServicesEmv_GetNextStep(TLV_TREE_NODE tagsFromDc, EMV_TransactionStep_t *stepToExecute);
static EMV_Status_t __EMV_ServicesEmv_PerformTransaction(EMV_Object_t EMV_object);
static EMV_Status_t  __EMV_ServicesEmv_StartTransaction(const char *cardReader, unsigned int cardReaderBufferSize, int doEmvAppliSelection);

//// Global variables ///////////////////////////////////////////

////  variables ///////////////////////////////////////////

static TLV_TREE_NODE __EMV_ServicesEmv_TransacInfoTlvTree = NULL;											//!< Store the EMV transaction data (amount, date and time, ...).
static EMV_ServicesEmv_TransactionStatus_e __EMV_ServicesEmv_TransacStatus = EMV_TR_STATUS_UNKNOWN;	//!< Store the EMV transaction status (transaction approved, ...).

//// Functions //////////////////////////////////////////////////

//! \brief Get the next transaction step to perform.
//! \param[in] tagsFromDc Transaction data that contain the TAG_EMV_INT_NEXT_STEP tag that indicates the next step to execute.
//! \param[out] stepToExecute Next step to execute.
//! \return EMV API status code. \a EMV_STATUS_SUCCESS if successful.
static EMV_Status_t __EMV_ServicesEmv_GetNextStep(TLV_TREE_NODE tagsFromDc, EMV_TransactionStep_t *stepToExecute) {
	EPSTOOL_Data_t dataStep;

	if(tagsFromDc != NULL) {
		if (EPSTOOL_TlvTree_FindFirstData(tagsFromDc, TAG_EMV_INT_NEXT_STEP, &dataStep) != NULL) {
			if ((dataStep.length == 1) && (dataStep.value != NULL)) {
				*stepToExecute = dataStep.value[0];
			} else {
				return EMV_STATUS_UNEXPECTED_ERROR;
			}
		} else {
			return EMV_STATUS_UNEXPECTED_ERROR;
		}
	}

	return EMV_STATUS_SUCCESS;
}


//! \brief Perform an EMV transaction by calling EMV API from Final Select to the end of the transaction.
//! \param[in] EMV_object The EMV transaction object.
//! \return EMV API status code. \a EMV_STATUS_SUCCESS if successful.
static EMV_Status_t __EMV_ServicesEmv_PerformTransaction(EMV_Object_t EMV_object) {
	EMV_Status_t emvStatus;
	TLV_TREE_NODE inputTlvTree;
	TLV_TREE_NODE tagsFromDc;
	TLV_TREE_NODE tagsToKernel;
	EMV_TransactionStep_t stepToExecute;
	int endTransaction;

	emvStatus = EMV_STATUS_UNKNOWN;
	endTransaction = FALSE;
	stepToExecute = EMV_STEP_INIT;

	// Initialise a tlvtree that will contain the input data
	inputTlvTree = TlvTree_New(0);
	if (inputTlvTree == NULL) {
		ASSERT(FALSE);
		return EMV_STATUS_NOT_ENOUGH_MEMORY;
	}

	// Call EMV API to perform each steps of the transaction
	do {
		switch (stepToExecute) {
		case EMV_STEP_INIT:
			emvStatus = EMV_ServicesEmv_Init(EMV_object, inputTlvTree, &tagsFromDc, &tagsToKernel);
			break;
		case EMV_STEP_FINAL_SELECT:
			emvStatus = EMV_ServicesEmv_FinalSelect(EMV_object, inputTlvTree, &tagsFromDc, &tagsToKernel);
			break;
		case EMV_STEP_INITIATE_APPLICATION_PROCESSING:
			emvStatus = EMV_ServicesEmv_InitiateApplicationSelection(EMV_object, inputTlvTree, &tagsFromDc, &tagsToKernel);
			break;
		case EMV_STEP_READ_APPLICATION_DATA:
			emvStatus = EMV_ServicesEmv_ReadRecords(EMV_object, inputTlvTree, &tagsFromDc, &tagsToKernel);
			break;
		case EMV_STEP_OFFLINE_DATA_AUTHENTICATION:
			emvStatus = EMV_ServicesEmv_OfflineDataAuthentication(EMV_object, inputTlvTree, &tagsFromDc, &tagsToKernel);
			break;
		case EMV_STEP_PROCESSING_RESTRICTION:
			emvStatus = EMV_ServicesEmv_ProcessingRestriction(EMV_object, inputTlvTree, &tagsFromDc, &tagsToKernel);
			break;
		case EMV_STEP_CARDHOLDER_VERIFICATION:
			emvStatus = EMV_ServicesEmv_CardholderVerification(EMV_object, inputTlvTree, &tagsFromDc, &tagsToKernel);
			break;
		case EMV_STEP_TERMINAL_RISK_MANAGEMENT:
			emvStatus = EMV_ServicesEmv_TerminalRiskManagement(EMV_object, inputTlvTree, &tagsFromDc, &tagsToKernel);
			break;
		case EMV_STEP_TERMINAL_ACTION_ANALYSIS:
			emvStatus = EMV_ServicesEmv_TerminalActionAnalysis(EMV_object, inputTlvTree, &tagsFromDc, &tagsToKernel);
			break;
		case EMV_STEP_CARD_ACTION_ANALYSIS:
			emvStatus = EMV_ServicesEmv_CardActionAnalysis(EMV_object, inputTlvTree, &tagsFromDc, &tagsToKernel);
			break;
		case EMV_STEP_ONLINE_PROCESSING:
			emvStatus = EMV_ServicesEmv_OnlineProcessing(EMV_object, inputTlvTree, &tagsFromDc, &tagsToKernel);
			break;
		case EMV_STEP_ISSUER_TO_CARD_SCRIPTS_PROCESSING1:
			emvStatus = EMV_ServicesEmv_ISP1(EMV_object, inputTlvTree, &tagsFromDc, &tagsToKernel);
			break;
		case EMV_STEP_COMPLETION:
			emvStatus = EMV_ServicesEmv_Completion(EMV_object, inputTlvTree, &tagsFromDc, &tagsToKernel);
			break;
		case EMV_STEP_ISSUER_TO_CARD_SCRIPTS_PROCESSING2:
			emvStatus = EMV_ServicesEmv_ISP2(EMV_object, inputTlvTree, &tagsFromDc, &tagsToKernel);
			break;
		case EMV_STEP_END:
			emvStatus = EMV_ServicesEmv_End(EMV_object, inputTlvTree, &tagsFromDc, &tagsToKernel);
			break;
		default:
			endTransaction = TRUE;
			break;
		}

		if((emvStatus == EMV_STATUS_SUCCESS) && (!endTransaction)) {
			// Release the input data
			EPSTOOL_TlvTree_Release(&inputTlvTree);

			// Create a new tlvtree for the next call
			inputTlvTree = TlvTree_New(0);
			if (inputTlvTree == NULL) {
				ASSERT(FALSE);
				EPSTOOL_TlvTree_Release(&tagsFromDc);
				EPSTOOL_TlvTree_Release(&tagsToKernel);
				return EMV_STATUS_NOT_ENOUGH_MEMORY;
			}

			// Fill input data for the next call with some data returned by the kernel
			if(stepToExecute == EMV_STEP_READ_APPLICATION_DATA) {
				// Perform the Read Records post processing
				EMV_ServicesEmv_StepReadApplicationData(tagsFromDc, inputTlvTree);
			} else if(stepToExecute == EMV_STEP_CARDHOLDER_VERIFICATION) {
				// Perform the Cardholder Verification post processing
				emvStatus = EMV_ServicesEmv_StepCardholderVerification(tagsFromDc, inputTlvTree);

				if (emvStatus != EMV_STATUS_SUCCESS){
					GL_Dialog_Message(hGoal, NULL, "Transaction Cancelled", GL_ICON_ERROR, GL_BUTTON_NONE, 2 * GL_TIME_SECOND);
				}
			}

			// Get the next step to perform
			if (emvStatus == EMV_STATUS_SUCCESS)
				emvStatus = __EMV_ServicesEmv_GetNextStep(tagsFromDc, &stepToExecute);
		}

		EMV_ServicesEmv_TransacStatusSet(emvStatus, tagsFromDc);

		// Release output data
		EPSTOOL_TlvTree_Release(&tagsFromDc);
		EPSTOOL_TlvTree_Release(&tagsToKernel);
	}while((emvStatus == EMV_STATUS_SUCCESS) && (!endTransaction));


	// Release all the allocated tlvtree
	EPSTOOL_TlvTree_Release(&inputTlvTree);
	EPSTOOL_TlvTree_Release(&tagsFromDc);
	EPSTOOL_TlvTree_Release(&tagsToKernel);

	return emvStatus;
}

//! \brief Start an EMV transaction by calling EMV API.
//! \param[in] cardReader The card reader to use.
//! \param[in] cardReaderBufferSize Size of the cardReader buffer.
//! \param[in] doEmvAppliSelection \a TRUE if the application selection must be done (transaction not started by the Telium Manager),
//! or \a FALSE if the application selection has already been done (transaction is started by the Telium Manager, usually called from \a GIVE_AID).
//! \return EMV API status code. \a EMV_STATUS_SUCCESS if successful.
static EMV_Status_t __EMV_ServicesEmv_StartTransaction(const char *cardReader, unsigned int cardReaderBufferSize, int doEmvAppliSelection) {
	EMV_Status_t emvStatus;
	EMVAS_Object_t AS_object;
	EMV_Object_t EMV_object;
	int AS_object_created = 0;

	emvStatus = EMV_STATUS_SUCCESS;

	if(doEmvAppliSelection) {
		// Create the application selection object
		emvStatus = EMVAS_Create(&AS_object, cardReader, cardReaderBufferSize);

		// Perform application selection
		if(emvStatus == EMV_STATUS_SUCCESS) {
			AS_object_created = 1;
			emvStatus = EMV_ServicesEmv_ApplicationSelection(AS_object);

			// If the apllication selection has not been performed by the manager, ask the cardholder to select the AID to use.
			// Add the AID to select in the transaction input data
			if (emvStatus == EMV_STATUS_SUCCESS) {
				EMV_ServicesEmv_MenuSelect(AS_object, __EMV_ServicesEmv_TransacInfoTlvTree);
			}

			if (emvStatus == EMV_STATUS_INVALID_CARD) {
				GL_Dialog_Message(hGoal, NULL, "Invalid card", GL_ICON_ERROR, GL_BUTTON_NONE, 2 * GL_TIME_SECOND);
			}
		}
	}

	if(emvStatus == EMV_STATUS_SUCCESS) {
		// Create the EMV transaction object
		if(EMV_Create(&EMV_object, cardReader, cardReaderBufferSize) == EMV_STATUS_SUCCESS) {
			// Perform the transaction
			emvStatus = __EMV_ServicesEmv_PerformTransaction(EMV_object);

			// Destroy the EMV transaction object
			VERIFY(EMV_Destroy(EMV_object) == EMV_STATUS_SUCCESS);
		}
	}

	// Destroy the application selection object
	if((doEmvAppliSelection) && (AS_object_created))
		VERIFY(EMVAS_Destroy(AS_object) == EMV_STATUS_SUCCESS);

	return emvStatus;
}


//! \brief Perform an EMV transaction by calling EMV API.
//! \param[in] cardReader The card reader to use.
//! \param[in] cardReaderBufferSize Size of the cardReader buffer.
//! \param[in] doEmvAppliSelection \a TRUE if the application selection must be done (transaction not started by the Telium Manager),
//! or \a FALSE if the application selection has already been done (transaction is started by the Telium Manager, usually called from \a GIVE_AID).
//! \param[out] inputTlvTree Input TlvTree. Usually contains transaction data (amount, date and time, ...).
//! \return EMV API status code. \a EMV_STATUS_SUCCESS if successful.
//! \remarks If date and time is not set in \a inputTlvTree, the current date and time is automatically used.
EMV_Status_t EMV_ServicesEmv_DoTransaction(const char *cardReader, unsigned int cardReaderBufferSize, int doEmvAppliSelection, TLV_TREE_NODE inputTlvTree) {
	//	MSGinfos tDisplayMsg;
	EMV_Status_t emvStatus;
	Telium_Date_t currentDate;
	int dateMissing;
	int timeMissing;
	unsigned char buffer[3];
	//	char ucBuffer[4];
	//	int nCardholderLanguage, merchLang;

	ASSERT(inputTlvTree != NULL);

	//	merchLang = PSQ_Give_Language();
	//	nCardholderLanguage = merchLang; // By default, cardholder language is set to default language

	// Initialise the transaction outcome
	__EMV_ServicesEmv_TransacStatus = EMV_TR_STATUS_UNKNOWN;

	EPSTOOL_TlvTree_Release(&__EMV_ServicesEmv_TransacInfoTlvTree);
	__EMV_ServicesEmv_TransacInfoTlvTree = TlvTree_New(0);
	if (__EMV_ServicesEmv_TransacInfoTlvTree == NULL) {
		ASSERT(FALSE);
		return EMV_STATUS_NOT_ENOUGH_MEMORY;
	}

	// Add date and time if they are missing
	dateMissing = (EPSTOOL_TlvTree_FindFirstData(inputTlvTree, TAG_EMV_TRANSACTION_DATE, NULL) == NULL);
	timeMissing = (EPSTOOL_TlvTree_FindFirstData(inputTlvTree, TAG_EMV_TRANSACTION_TIME, NULL) == NULL);
	if ((dateMissing) || (timeMissing)) {
		// The date and/or time is missing

		// Get the current date and time
		if (Telium_Read_date(&currentDate) == 0) {
			// Add the date if it is missing
			if (dateMissing) {
				dateTimeRefresh();
				buffer[0] = ((currentDate.year[0] - '0') << 4) | (currentDate.year[1] - '0');
				buffer[1] = ((currentDate.month[0] - '0') << 4) | (currentDate.month[1] - '0');
				buffer[2] = ((currentDate.day[0] - '0') << 4) | (currentDate.day[1] - '0');
				if (TlvTree_AddChild(__EMV_ServicesEmv_TransacInfoTlvTree, TAG_EMV_TRANSACTION_DATE, buffer, 3) == NULL) {
					ASSERT(FALSE);
					EPSTOOL_TlvTree_Release(&__EMV_ServicesEmv_TransacInfoTlvTree);
					return EMV_STATUS_NOT_ENOUGH_MEMORY;
				}
			}

			// Add the time if it is missing
			if (timeMissing) {
				dateTimeRefresh();
				buffer[0] = ((currentDate.hour[0] - '0') << 4) | (currentDate.hour[1] - '0');
				buffer[1] = ((currentDate.minute[0] - '0') << 4) | (currentDate.minute[1] - '0');
				buffer[2] = ((currentDate.second[0] - '0') << 4) | (currentDate.second[1] - '0');
				if (TlvTree_AddChild(__EMV_ServicesEmv_TransacInfoTlvTree, TAG_EMV_TRANSACTION_TIME, buffer, 3) == NULL) {
					ASSERT(FALSE);
					EPSTOOL_TlvTree_Release(&__EMV_ServicesEmv_TransacInfoTlvTree);
					return EMV_STATUS_NOT_ENOUGH_MEMORY;
				}
			}
		}
	}

	// Store the transaction input data
	if (!EPSTOOL_TlvTree_CopyChildren(__EMV_ServicesEmv_TransacInfoTlvTree, inputTlvTree)) {
		EPSTOOL_TlvTree_Release(&__EMV_ServicesEmv_TransacInfoTlvTree);
		ASSERT(FALSE);
		return EMV_STATUS_NOT_ENOUGH_MEMORY;
	}

	// Start the transaction
	emvStatus = __EMV_ServicesEmv_StartTransaction(cardReader, cardReaderBufferSize, doEmvAppliSelection);

	EMV_ServicesEmv_StepStop();

	// Force an error if transaction is not approved
	if ((emvStatus == EMV_STATUS_SUCCESS) && (__EMV_ServicesEmv_TransacStatus != EMV_TR_STATUS_APPROVED)) {
		emvStatus = EMV_STATUS_UNKNOWN;
	}

	// Reset globals
	EPSTOOL_TlvTree_Release(&__EMV_ServicesEmv_TransacInfoTlvTree);
	__EMV_ServicesEmv_TransacStatus = EMV_TR_STATUS_UNKNOWN;

	return emvStatus;
}

//! \brief Retrieve the transaction data (amount, date and time, ...).
//! \param[out] outputTlvTree TlvTree that will be filled with the transaction data.
//! \return \a TRUE if successful, \a FALSE if an error occurs (usually because not enough memory).
int EMV_ServicesEmv_GetTransactionData(TLV_TREE_NODE outputTlvTree) {
	ASSERT(outputTlvTree != NULL);
	ASSERT(__EMV_ServicesEmv_TransacInfoTlvTree != NULL);

	// Get the transaction data
	return EPSTOOL_TlvTree_CopyChildren(outputTlvTree, __EMV_ServicesEmv_TransacInfoTlvTree);
}

//! \brief Determine if the amount is known or not.
//! \return \a TRUE if the amount is known, \a FALSE if not.
int EMV_ServicesEmv_AmountIsSet(void) {
	TLV_TREE_NODE nodeAmountBin;
	TLV_TREE_NODE nodeAmountNum;
	EPSTOOL_Data_t dataAmountBin;
	EPSTOOL_Data_t dataAmountNum;

	ASSERT(__EMV_ServicesEmv_TransacInfoTlvTree != NULL);

	nodeAmountBin = EPSTOOL_TlvTree_FindFirstData(__EMV_ServicesEmv_TransacInfoTlvTree, TAG_EMV_AMOUNT_AUTH_BIN, &dataAmountBin);
	nodeAmountNum = EPSTOOL_TlvTree_FindFirstData(__EMV_ServicesEmv_TransacInfoTlvTree, TAG_EMV_AMOUNT_AUTH_NUM, &dataAmountNum);
	if (((nodeAmountBin != NULL) && (dataAmountBin.length > 0)) || ((nodeAmountNum != NULL) && (dataAmountNum.length > 0))) {
		return TRUE;
	} else {
		return FALSE;
	}
}

//! \brief Set the transaction amount.
//! \param[in] amount The amount of the transaction.
//! \param[out] outputTlvTree A TlvTree that will contain the amounts tags (binary and numeric).
//! \return \a TRUE if the amount is set, \a FALSE if an error occurs.
int EMV_ServicesEmv_AmountSet(unsigned long long amount, TLV_TREE_NODE outputTlvTree) {
	unsigned char amountBinary[4];
	unsigned char amountNumeric[6];
	unsigned char amountOthBinary[4];
	unsigned char amountOthNumeric[6];
	unsigned long long amountOth = 0l;
	unsigned long OthAmt;
	char OtherAmount[lenAmt + 1];

	memset(OtherAmount, 0, sizeof(OtherAmount));

	mapGet(traOtherAmt, OtherAmount, lenAmt);
	EPSTOOL_Convert_AsciiToUl(OtherAmount, -1, &OthAmt);
	amountOth = OthAmt;

	ASSERT(__EMV_ServicesEmv_TransacInfoTlvTree != NULL);

	if (amount <= 0xffffffff) {
		// Convert the amount into a 'EMV binary' number
		EPSTOOL_Convert_ULongToEmvBin((unsigned long)amount, amountBinary);
		if (TlvTree_AddChild(__EMV_ServicesEmv_TransacInfoTlvTree, TAG_EMV_AMOUNT_AUTH_BIN, amountBinary, sizeof(amountBinary)) == NULL)
			return FALSE;
		if (outputTlvTree != NULL) {
			if (TlvTree_AddChild(outputTlvTree, TAG_EMV_AMOUNT_AUTH_BIN, amountBinary, sizeof(amountBinary)) == NULL)
				return FALSE;

			EPSTOOL_Convert_ULongToEmvBin((unsigned long)amountOth, amountOthBinary);
			TlvTree_AddChild(outputTlvTree, TAG_EMV_AMOUNT_OTHER_BIN, amountOthBinary, sizeof(amountOthBinary));
		}
	}

	if (amount <= 999999999999ULL) {
		// Convert the amount into a 'EMV numeric' number
		EPSTOOL_Convert_UllToDcbNumber(amount, amountNumeric, 6);
		if (TlvTree_AddChild(__EMV_ServicesEmv_TransacInfoTlvTree, TAG_EMV_AMOUNT_AUTH_NUM, amountNumeric, sizeof(amountNumeric)) == NULL)
			return FALSE;
		if (outputTlvTree != NULL) {

			if (TlvTree_AddChild(outputTlvTree, TAG_EMV_AMOUNT_AUTH_NUM, amountNumeric, sizeof(amountNumeric)) == NULL)
				return FALSE;

			EPSTOOL_Convert_UllToDcbNumber(amountOth, amountOthNumeric, 6);
			TlvTree_AddChild(outputTlvTree, TAG_EMV_AMOUNT_OTHER_NUM, amountOthNumeric, sizeof(amountOthNumeric));
		}
	} else {
		// Amount is too big
		return FALSE;
	}

	return TRUE;
}


//! \brief Retrieve the transaction status.
//! \return The transaction status (from the global variable \ref __EMV_ServicesEmv_TransacStatus).
EMV_ServicesEmv_TransactionStatus_e EMV_ServicesEmv_TransacStatusGet(void) {
	ASSERT((__EMV_ServicesEmv_TransacStatus >= 0) && (__EMV_ServicesEmv_TransacStatus < EMV_TR_STATUS_END));
	return __EMV_ServicesEmv_TransacStatus;
}

//! \brief Change the transaction status with a new value.
//! \param[in] status The new transaction status.
//! \remarks This function sets the global variable \ref __EMV_ServicesEmv_TransacStatus.
void EMV_ServicesEmv_TransacStatusChange(EMV_ServicesEmv_TransactionStatus_e status) {
	ASSERT((status >= 0) && (status < EMV_TR_STATUS_END));
	__EMV_ServicesEmv_TransacStatus = status;
}

//! \brief Set the transaction status (if needed) depending on the status code from EMV API.
//! \param[in] emvStatus The status code from EMV API.
//! \param[in] transactionData Transaction data that can contain the TAG_EMV_INT_TRANSACTION_STATUS tag.
//! \remarks This function sets the global variable \ref __EMV_ServicesEmv_TransacStatus.
void EMV_ServicesEmv_TransacStatusSet(EMV_Status_t emvStatus, TLV_TREE_NODE transactionData) {
	EPSTOOL_Data_t dataStatus;

	switch(emvStatus) {
	case EMV_STATUS_SUCCESS:
		// Do not change the status code!

		// Retrieve the transaction status
		if(transactionData != NULL) {
			if (EPSTOOL_TlvTree_FindFirstData(transactionData, TAG_EMV_INT_TRANSACTION_STATUS, &dataStatus) != NULL) {
				if (dataStatus.length == 1) {
					if (dataStatus.value[0] == 0x00) {
						__EMV_ServicesEmv_TransacStatus = EMV_TR_STATUS_DECLINED;
					} else if (dataStatus.value[0] == 0x01) {
						__EMV_ServicesEmv_TransacStatus = EMV_TR_STATUS_APPROVED;
					}
				}
			}
		}
		break;
	case EMV_STATUS_SELECT_ANOTHER_AID:
		__EMV_ServicesEmv_TransacStatus = EMV_TR_STATUS_APPROVED;			//Added by Kev
		break;
		// Normal status that stops the transaction
	case EMV_STATUS_SERVICE_NOT_ALLOWED:
		__EMV_ServicesEmv_TransacStatus = EMV_TR_STATUS_SERVICE_NOT_ALLOWED;
		break;
	case EMV_STATUS_CANCEL:
		__EMV_ServicesEmv_TransacStatus = EMV_TR_STATUS_CANCELLED;
		break;
	case EMV_STATUS_CARD_BLOCKED:
		__EMV_ServicesEmv_TransacStatus = EMV_TR_STATUS_CARD_BLOCKED;
		break;
	case EMV_STATUS_CARD_REMOVED:
		__EMV_ServicesEmv_TransacStatus = EMV_TR_STATUS_CARD_REMOVED;
		break;

		// Terminal errors
	case EMV_STATUS_INVALID_OBJECT:
	case EMV_STATUS_CARD_READER_UNKOWN:
	case EMV_STATUS_NOT_ENOUGH_MEMORY:
	case EMV_STATUS_MISSING_MANDATORY_TERM_DATA:
	case EMV_STATUS_NOT_ALLOWED:
	case EMV_STATUS_INVALID_PARAMETER:
		__EMV_ServicesEmv_TransacStatus = EMV_TR_STATUS_TERMINAL_ERROR;
		break;

		// Card errors
	case EMV_STATUS_INVALID_CARD:
	case EMV_STATUS_CARD_PROCESSING_ERROR:
	case EMV_STATUS_CARD_DATA_ERROR:
	case EMV_STATUS_CARD_ANSWER:
		__EMV_ServicesEmv_TransacStatus = EMV_TR_STATUS_CARD_ERROR;
		break;

		// Unknown error
	default:
		if (emvStatus >= EMV_STATUS_LAST) {
			// Error
			__EMV_ServicesEmv_TransacStatus = EMV_TR_STATUS_UNKNOWN;
		} else {
			__EMV_ServicesEmv_TransacStatus = EMV_TR_STATUS_TERMINAL_ERROR;
		}
		break;
	}
}

//! \brief Retrieves the EMV global parameters.
//! \param[out] outputTlvTree Output TlvTree that must be filled with the EMV global parameters.
void EMV_ServicesEmv_GetGlobalParam(TLV_TREE_NODE outputTlvTree) {
	// Get the global parameters
	const char terminalId[8 + 1] = "00000001";
	char TxnCurrCode[6 + 1] = "0834";
	char BinCurrCode[2 + 1] = "\x08\x34";
	NO_SERIE serial;
	// ICS
	static const unsigned char usePse[1] = { 0x01 };
	static const unsigned char terminalType[1] = { 0x22 };
	static const unsigned char terminalCapabilities[3] = { 0xE0, 0xF0, 0xC8 }; // All pin types supported & signature
	static const unsigned char addTerminalCapabilities[5] = { 0xFF, 0x00, 0xF0, 0xA0, 0x01 }; // { 0xF9, 0x00, 0xFF, 0xA0, 0x01 };
	static const unsigned char terminalCountryCode[2] = { 0x08, 0x34 };
	static const unsigned char trmOverpassAip[1] = { 0x01 };
	static const unsigned char merchantCategoryCode[2] = { 0x53, 0x11 };
	// Supported languages
	static const unsigned char supportedLanguages[] = "enfr";

	ASSERT(outputTlvTree != NULL);

	/////////////////////////////////////////////////////////////////////////
	// Set required mandatory tags (shall not be modified)
	/////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////
	// TODO: Get the global parameters
	/////////////////////////////////////////////////////////////////////////
	mapGet(appTID, (char *)terminalId, lenTid);
	mapGet(emvTrnCurCod, TxnCurrCode, 6);

	//Set the TAG_EMV_TERMINAL_IDENTIFICATION
	ASSERT(strlen(terminalId) == 8);
	VERIFY(TlvTree_AddChild(outputTlvTree, TAG_EMV_TERMINAL_IDENTIFICATION, terminalId, 8) != NULL);

	//Set the TAG_EMV_MERCHANT_CATEGORY_CODE
	VERIFY(TlvTree_AddChild(outputTlvTree, TAG_EMV_MERCHANT_CATEGORY_CODE, merchantCategoryCode, 2) != NULL);

	// Set the TAG_EMV_IFD_SERIAL_NUMBER
	PSQ_Give_Serial_Number(serial);
	ASSERT((serial[0] >= '0') && (serial[0] <= '9')
			&& (serial[1] >= '0') && (serial[1] <= '9')
			&& (serial[2] >= '0') && (serial[2] <= '9')
			&& (serial[3] >= '0') && (serial[3] <= '9')
			&& (serial[4] >= '0') && (serial[4] <= '9')
			&& (serial[5] >= '0') && (serial[5] <= '9')
			&& (serial[6] >= '0') && (serial[6] <= '9')
			&& (serial[7] >= '0') && (serial[7] <= '9'));
	VERIFY(TlvTree_AddChild(outputTlvTree, TAG_EMV_IFD_SERIAL_NUMBER, serial, 8) != NULL);

	//////////////////////////////////////////////////////////////
	// Terminal Currency and transaction Currency code
	/////////////////////////////////////////////////////////////
	hex2bin((byte *)BinCurrCode, &TxnCurrCode[2], 2);
	VERIFY(TlvTree_AddChild(outputTlvTree, TAG_EMV_TRANSACTION_CURRENCY_CODE, BinCurrCode, 2) != NULL);

	/////////////////////////////////////////////////////////////////////////
	// TODO: Get the ICS data (Kev-Code) check
	/////////////////////////////////////////////////////////////////////////
	VERIFY(TlvTree_AddChild(outputTlvTree, TAG_USE_PSE, usePse, sizeof(usePse)) != NULL);

	VERIFY(TlvTree_AddChild(outputTlvTree, TAG_EMV_TERMINAL_TYPE, terminalType, sizeof(terminalType)) != NULL);
	VERIFY(TlvTree_AddChild(outputTlvTree, TAG_EMV_TERMINAL_CAPABILITIES, terminalCapabilities, sizeof(terminalCapabilities)) != NULL);
	VERIFY(TlvTree_AddChild(outputTlvTree, TAG_EMV_ADD_TERMINAL_CAPABILITIES, addTerminalCapabilities, sizeof(addTerminalCapabilities)) != NULL);

	VERIFY(TlvTree_AddChild(outputTlvTree, TAG_EMV_TERMINAL_COUNTRY_CODE, terminalCountryCode, sizeof(terminalCountryCode)) != NULL);
	VERIFY(TlvTree_AddChild(outputTlvTree, TAG_EMV_OPTION_TRM_IRRESPECTIVE_OF_AIP, trmOverpassAip, sizeof(trmOverpassAip)) != NULL);
	VERIFY(TlvTree_AddChild(outputTlvTree, TAG_EMV_OPTION_FAIL_CVM, trmOverpassAip, sizeof(trmOverpassAip)) != NULL);

	/////////////////////////////////////////////////////////////////////////
	//Get the supported language list
	/////////////////////////////////////////////////////////////////////////
	// Get the supported language list
	VERIFY(TlvTree_AddChild(outputTlvTree, TAG_EMV_INT_APPLICATION_LANGUAGES, supportedLanguages, sizeof(supportedLanguages) - 1) != NULL);
}
