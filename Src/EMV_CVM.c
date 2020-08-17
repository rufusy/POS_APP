/**
 * \file
 * \brief This module implements the EMV API functionalities related to Cardholder Verification.
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

#include "EPSTOOL_Convert.h"
#include "EPSTOOL_TlvTree.h"
#include "EPSTOOL_PinEntry.h"

#include "EmvLib_Tags.h"
#include "EMV_Status.h"
#include "EMV_ApiTags.h"
#include "EMV_Api.h"

#include "EMV_UserInterfaceDisplay.h"
#include "EMV_ServicesEmv.h"
#include "EMV_ServicesEmv_CVM.h"
#include "globals.h"


//// Macros & preprocessor definitions //////////////////////////

//// Types //////////////////////////////////////////////////////

//// Static function definitions ////////////////////////////////

static void __EMV_ServicesEmv_PinEntry(int pinType, EMV_Status_t *emvStatus, int *methodResult, TLV_TREE_NODE inputTlvTree, TLV_TREE_NODE outputTlvTree);

//// Global variables ///////////////////////////////////////////

////  variables ///////////////////////////////////////////

//// Functions //////////////////////////////////////////////////

//! \brief Ask for a PIN entry and analyse the status.
//! \param[in] pinType The type of the PIN entry: \a EMV_INT_CV_STATUS_OFFLINE_PIN or \a EMV_INT_CV_STATUS_ONLINE_PIN.
//! \param[in,out] emvStatus The status code, \a EMV_STATUS_SUCCESS if success.
//! \param[out] methodResult The result of the PIN entry. To be set in tag \a TAG_EMV_INT_CV_RESULT.
//! \param[in] inputTlvTree Transaction data that can contain the tag TAG_EMV_INT_REMAINING_PIN.
//! \param[in,out] outputTlvTree Will be filled with all necessary data for the PIN entry process (PIN bypass indicator ...).
static void __EMV_ServicesEmv_PinEntry(int pinType, EMV_Status_t *emvStatus, int *methodResult, TLV_TREE_NODE inputTlvTree, TLV_TREE_NODE outputTlvTree) {
	int pinLength;
	int pinTryCounter;
	EPSTOOL_Data_t dataPinTryCounter;

	ASSERT((pinType == EMV_INT_CV_STATUS_OFFLINE_PIN) || (pinType == EMV_INT_CV_STATUS_ONLINE_PIN));
	ASSERT(emvStatus != NULL);
	ASSERT(methodResult != NULL);
	ASSERT(outputTlvTree != NULL);

	pinTryCounter = -1;
	if (pinType == EMV_INT_CV_STATUS_OFFLINE_PIN) {
		// Retrieve the PIN Try Counter
		if (inputTlvTree != NULL) {
			if (EPSTOOL_TlvTree_FindFirstData(inputTlvTree, TAG_EMV_INT_REMAINING_PIN, &dataPinTryCounter) != NULL) {
				if ((dataPinTryCounter.length == 1) && (dataPinTryCounter.value != NULL)) {
					pinTryCounter = *dataPinTryCounter.value;
				}
			}
		}
	}

	// Ask for the PIN entry
	switch(EMV_UI_PinEntry(pinType, pinTryCounter, &pinLength)) {
	case EPSTOOL_PINENTRY_SUCCESS:
		// PIN has been entered => give the length of the PIN in TAG_EMV_INT_PIN_ENTRY_LENGTH
		VERIFY(TlvTree_AddChild(outputTlvTree, TAG_EMV_INT_PIN_ENTRY_LENGTH, &pinLength, 1) != NULL);
		*methodResult = EMV_INT_CV_RESULT_SUCCESS;
		break;
	case EPSTOOL_PINENTRY_BYPASS:
		// PIN bypass is requested by the cardholder
		*methodResult = EMV_INT_CV_RESULT_BYPASS;
		break;
	case EPSTOOL_PINENTRY_TIMEOUT:
		// Timeout
		*methodResult = EMV_INT_CV_RESULT_TIMEOUT;
		break;
	case EPSTOOL_PINENTRY_ERROR:
		// Error => the pinpad is out of order
		*methodResult = EMV_INT_CV_RESULT_PINPAD_OUT_OF_ORDER;
		break;
	case EPSTOOL_PINENTRY_EVENT:
		// An external event occurred, in our case it is because the card has been removed
		*emvStatus = EMV_STATUS_CARD_REMOVED;
		*methodResult = EMV_INT_CV_RESULT_CANCEL;
		break;
	case EPSTOOL_PINENTRY_CANCEL:
		// PIN entry has been cancelled by the cardholder
		*methodResult = EMV_INT_CV_RESULT_CANCEL;
		break;
	default:
		// PIN entry has been cancelled by the cardholder, or an unknown error occurred => stop
		*emvStatus = EMV_STATUS_UNKNOWN;
		*methodResult = EMV_INT_CV_RESULT_CANCEL;
		break;
	}
}

//! \brief Called when the cardholder must be authenticated.
//! \param[in] inputTlvTree Input TlvTree that especially contains the method to use in TAG_EMV_INT_CV_STATUS.
//! \param[out] outputTlvTree Output TlvTree must be filled with at least TAG_EMV_INT_CV_RESULT (set it to EMV_INT_CV_RESULT_SUCCESS to terminate CVM).
//! Others tags may be given depending on the method such as TAG_EMV_INT_PIN_ENTRY_LENGTH for PIN.
//! \return  Can return any value of EMV_Status_t.
EMV_Status_t EMV_ServicesEmv_StepCardholderVerification(TLV_TREE_NODE inputTlvTree, TLV_TREE_NODE outputTlvTree) {
	EMV_Status_t emvStatus;
	EPSTOOL_Data_t dataCvStatus;
	unsigned char cvStatus;
	int signature;
	int methodResult;
//	card CardSelected;

	ASSERT(inputTlvTree != NULL);
	ASSERT(outputTlvTree != NULL);

	emvStatus = EMV_STATUS_SUCCESS;

	signature = FALSE;

	// Retrieve TAG_EMV_INT_CV_STATUS
	if (EPSTOOL_TlvTree_FindFirstData(inputTlvTree, TAG_EMV_INT_CV_STATUS, &dataCvStatus) != NULL) {

		if ((dataCvStatus.length == 2) && (dataCvStatus.value != NULL)) {

			cvStatus = dataCvStatus.value[0];
			if((cvStatus & EMV_INT_CV_STATUS_SIGNATURE) != 0) {
				signature = TRUE;
			}

			if((cvStatus & EMV_INT_CV_STATUS_END) != 0) {

				methodResult = EMV_INT_CV_RESULT_SUCCESS;

				if((cvStatus & EMV_INT_CV_STATUS_OFFLINE_PIN_OK) != 0) {
					// The previous entered PIN is correct => display message
					EMV_UI_MessageCorrectPin();
				} /*else if((cvStatus & EMV_INT_CV_STATUS_OFFLINE_PIN_WRONG) != 0) {
					mapGetCard(traCardKernel, CardSelected);
					if (CardSelected == DEFAULT_EP_KERNEL_QUICKPASS) {
						// The previous entered PIN is wrong
						methodResult = EMV_INT_CV_RESULT_CANCEL;
					}
				}*/

				if (signature) {
					// Only signature is required
					// TODO: Request signature ?
					EMV_UI_RequestSignature();
				}

			} else if((cvStatus & EMV_INT_CV_STATUS_OFFLINE_PIN) != 0) {
				if ((cvStatus & EMV_INT_CV_STATUS_OFFLINE_PIN_WRONG ) != 0) {
					// The previous entered PIN is wrong => display message
					EMV_UI_MessageWrongPin();
				}
				if((cvStatus & EMV_INT_CV_STATUS_LAST_ATTEMPT ) != 0) {
					// It is the last attempt of the offline PIN entry.
					EMV_UI_MessagePinLastAttempt();
				}

				// Ask for an offline PIN
				__EMV_ServicesEmv_PinEntry(EMV_INT_CV_STATUS_OFFLINE_PIN, &emvStatus, &methodResult, inputTlvTree, outputTlvTree);

				// Check if PIN entry was done successfully
				if((methodResult != EMV_INT_CV_RESULT_SUCCESS) && (methodResult != EMV_INT_CV_RESULT_BYPASS)) {
					emvStatus = EMV_STATUS_UNEXPECTED_ERROR;
				}
			} else if((cvStatus & EMV_INT_CV_STATUS_ONLINE_PIN) != 0) {
				// Ask for an online PIN
				__EMV_ServicesEmv_PinEntry(EMV_INT_CV_STATUS_ONLINE_PIN, &emvStatus, &methodResult, inputTlvTree, outputTlvTree);

				// Request a signature if requested and if PIN entry was done successfully
				if ((signature) && ((methodResult == EMV_INT_CV_RESULT_SUCCESS) || (methodResult == EMV_INT_CV_RESULT_BYPASS))) {
					// Request signature
					// TODO: Request signature ?
					EMV_UI_RequestSignature();
				}
				if((methodResult != EMV_INT_CV_RESULT_SUCCESS) && (methodResult != EMV_INT_CV_RESULT_BYPASS)) {
					emvStatus = EMV_STATUS_UNEXPECTED_ERROR;
				}
			} else if((cvStatus & EMV_INT_CV_STATUS_PROPRIETARY_METHOD) != 0) {
				// The selected method is a proprietary one

				methodResult = EMV_INT_CV_RESULT_SUCCESS;
			} else {
				// Unknown method
				methodResult = EMV_INT_CV_RESULT_SUCCESS;
			}

			// Set TAG_EMV_INT_CV_RESULT to indicate the result to the EMV API
			VERIFY(TlvTree_AddChild(outputTlvTree, TAG_EMV_INT_CV_RESULT, &methodResult, 1) != NULL);
		} else {
			emvStatus = EMV_STATUS_UNEXPECTED_ERROR;
		}
	} else {
		emvStatus = EMV_STATUS_UNEXPECTED_ERROR;
	}

	if(emvStatus != EMV_STATUS_SUCCESS) {
		EMV_ServicesEmv_TransacStatusSet(emvStatus, NULL);
	}

	return emvStatus;
}

//! \brief Execute the cardholder verification step of an EMV transaction.
//! \param[in] EMV_object The EMV transaction object to use.
//! \param[in] inputTlvTree The input transaction data.
//! \param[out] tagsKernelToAppli If not \a NULL, the function creates a new TlvTree object and fill it
//! with the tags and values requested by the application.
//! If no tag is requested by the application or if the EMV kernel does not know any value of the requested tag,
//!  no TlvTree is created (so value is set to \a NULL).
//! \param[out] tagsRequestedByKernel If not \a NULL, the function creates a new TlvTree object and fill it
//! with the tags (with empty values) that the application shall provide to the next transaction step.
//! If no tag is requested by the EMV kernel, no TlvTree is created (so value is set to \a NULL).
//! \return  Can return any value of EMV_Status_t.
//! note The application is in charge to destroy the created TlvTree \a tagsKernelToAppli and \a tagsRequestedByKernel.
EMV_Status_t EMV_ServicesEmv_CardholderVerification(EMV_Object_t EMV_object, TLV_TREE_NODE inputTlvTree, TLV_TREE_NODE *tagsKernelToAppli, TLV_TREE_NODE *tagsRequestedByKernel) {
	EMV_Status_t emvStatus;
	static const EPSTOOL_Tag_t tagsToGet[] = { TAG_EMV_INT_NEXT_STEP, TAG_EMV_INT_CV_STATUS };

	// Call the EMV API to perform cardholder verification
	emvStatus = EMV_ExecuteStep(EMV_object, EMV_STEP_CARDHOLDER_VERIFICATION, inputTlvTree, sizeof(tagsToGet) / sizeof(tagsToGet[0]), tagsToGet, tagsKernelToAppli, tagsRequestedByKernel);

	return emvStatus;
}
