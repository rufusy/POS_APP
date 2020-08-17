/**
 * \file
 * \brief This module implements the EMV API functionalities related to the end of the transaction.
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
#include "EMV_ServicesEmv_Stop.h"

#include "globals.h"

//// Macros & preprocessor definitions //////////////////////////

//// Types //////////////////////////////////////////////////////

//// Static function definitions ////////////////////////////////

//// Global variables ///////////////////////////////////////////

////  variables ///////////////////////////////////////////

//// Functions //////////////////////////////////////////////////

//! \brief Called when the transaction is terminated.
void EMV_ServicesEmv_StepStop(void) {
	char responseCode[lenRspCod + 1];
	int ret = 0;

	memset(responseCode, 0, sizeof(responseCode));
	MAPGET(traRspCod,responseCode,lblKO);


	// TODO: Customise if required
	// TODO: Reversal, Online Advice, display transaction status, print ticket...
	All_AfterTransaction();

	// (3) Check and Perform Reversal if required
	if ((strncmp(responseCode,"000",strlen(responseCode)) == 0) || (strncmp(responseCode,"Y1",2) == 0)){
		EMV_ServicesEmv_TransacStatusChange(EMV_TR_STATUS_APPROVED);
	} else {
		EMV_ServicesEmv_TransacStatusChange(EMV_TR_STATUS_DECLINED);
	}
	lblKO:

	// Display the transaction outcome
	switch(EMV_ServicesEmv_TransacStatusGet())
	{
	case EMV_TR_STATUS_APPROVED:
		EMV_UI_MessageApproved();
		break;
	case EMV_TR_STATUS_DECLINED:
		EMV_UI_MessageDeclined();
		break;
	case EMV_TR_STATUS_SERVICE_NOT_ALLOWED:
		EMV_UI_MessageServiceNotAllowed();
		break;

	case EMV_TR_STATUS_CANCELLED:
		EMV_UI_MessageTransactionCancelled();
		break;

	case EMV_TR_STATUS_CARD_BLOCKED:
		EMV_UI_MessageCardBlocked();
		break;
	case EMV_TR_STATUS_CARD_REMOVED:
		EMV_UI_MessageCardRemoved();
		break;

	case EMV_TR_STATUS_CARD_ERROR:
		EMV_UI_MessageCardError();
		break;

	case EMV_TR_STATUS_TERMINAL_ERROR:
	case EMV_TR_STATUS_UNKNOWN:
	default:
		EMV_UI_MessageTransactionError();
		break;
	}
}


//! \brief Retrieve the last transaction performed with the same card.
//! \param[in] EMV_object The EMV transaction object to use.
//! \param[out] outputTlvTree Output TlvTree. If a transaction is found, it must be filled with.
static void __EMV_ServicesEmv_GetResponseTransactionEMV_Data(EMV_Object_t EMV_object, TLV_TREE_NODE outputTlvTree)
{
	//EMV_Status_t emvStatus;
	static const EPSTOOL_Tag_t tagsToGet[] = { TAG_EMV_APPLICATION_CRYPTOGRAM, TAG_EMV_TSI, TAG_EMV_TVR, TAG_EMV_CVM_RESULTS };
	TLV_TREE_NODE tlvTree;

	TLV_TREE_NODE nodeIssTrnCrt;
	EPSTOOL_Data_t dataIssTrnCrt;

	TLV_TREE_NODE nodeTVR;
	EPSTOOL_Data_t dataTVR;

	TLV_TREE_NODE nodeTSI;
	EPSTOOL_Data_t dataTSI;

	TLV_TREE_NODE nodeCVMR;
	EPSTOOL_Data_t dataCVMR;

	char tagValue[100];


	// To avoid warnings because 'outputTlvTree' is not used
	(void)outputTlvTree;

	// Retrieve the PAN and the PAN Sequence Counter
	/*emvStatus =*/ EMV_GetTags(EMV_object, sizeof(tagsToGet) / sizeof(tagsToGet[0]), tagsToGet, &tlvTree);
	if(tlvTree != NULL) {

		nodeIssTrnCrt = EPSTOOL_TlvTree_FindFirstData(tlvTree, TAG_EMV_APPLICATION_CRYPTOGRAM, &dataIssTrnCrt);
		if (nodeIssTrnCrt != NULL) {
			memset(tagValue, 0, sizeof(tagValue));
			bin2hex(tagValue,dataIssTrnCrt.value,dataIssTrnCrt.length);
			mapPut(traIssTrnCrt, tagValue, (dataIssTrnCrt.length * 2));
		}

		nodeTVR = EPSTOOL_TlvTree_FindFirstData(tlvTree, TAG_EMV_TVR, &dataTVR);
		if (nodeTVR != NULL) {
			memset(tagValue, 0, sizeof(tagValue));
			bin2hex(tagValue,dataTVR.value,dataTVR.length);
			mapPut(traTVR, tagValue, (dataTVR.length * 2));
		}

		nodeTSI = EPSTOOL_TlvTree_FindFirstData(tlvTree, TAG_EMV_TSI, &dataTSI);
		if (nodeTSI != NULL) {
			memset(tagValue, 0, sizeof(tagValue));
			bin2hex(tagValue,dataTSI.value,dataTSI.length);
			mapPut(traTSI, tagValue, (dataTSI.length * 2));
		}

		nodeCVMR = EPSTOOL_TlvTree_FindFirstData(tlvTree, TAG_EMV_CVM_RESULTS, &dataCVMR);
		if (nodeCVMR != NULL) {
			memset(tagValue, 0, sizeof(tagValue));
			bin2hex(tagValue,dataCVMR.value,dataCVMR.length);
			mapPut(traCVMR, tagValue, (dataCVMR.length * 2));
		}

		//		 TODO: Get the last transaction amount
		//		 Set tag TAG_EMV_INT_LAST_TRANSACTION_AMOUNT

		// Free the memory
		EPSTOOL_TlvTree_Release(&tlvTree);
	}
}


//! \brief Execute the end step of an EMV transaction.
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
EMV_Status_t EMV_ServicesEmv_End(EMV_Object_t EMV_object, TLV_TREE_NODE inputTlvTree,
		TLV_TREE_NODE *tagsKernelToAppli, TLV_TREE_NODE *tagsRequestedByKernel)
{
	EMV_Status_t emvStatus;
	static const EPSTOOL_Tag_t tagsToGet[] = { TAG_EMV_INT_NEXT_STEP , TAG_EMV_INT_TRANSACTION_STATUS};

	__EMV_ServicesEmv_GetResponseTransactionEMV_Data(EMV_object, inputTlvTree);

	// Call the EMV API to perform end step
	emvStatus = EMV_ExecuteStep(EMV_object, EMV_STEP_END, inputTlvTree, sizeof(tagsToGet) / sizeof(tagsToGet[0]), tagsToGet, tagsKernelToAppli, tagsRequestedByKernel);

	return emvStatus;
}


//! \brief Wait card removal after an EMV transaction.
void EMV_ServicesEmv_RemoveCard(void){
	int bCardPresent;
	Telium_File_t* hCam;
	unsigned char ucStatus;

	hCam = NULL;


	hCam = Telium_Fopen("CAM0", "rw");

	bCardPresent = TRUE;
	if (hCam != NULL) {
		Telium_Status(hCam, &ucStatus);
		if ((ucStatus & CAM_PRESENT) == 0) {
			if (hCam != NULL) {
				Telium_Fclose(hCam);
				hCam = NULL;
			}
			// A card has been removed
			bCardPresent = FALSE;
		} else {
			// Display message "Remove card"
			EMV_UI_MessageRemoveCard();
		}


		while(bCardPresent) {
			Telium_Ttestall(CAM0, 0);
			Telium_Status(hCam, &ucStatus);

			if ((ucStatus & CAM_PRESENT) == 0) {
				if (hCam != NULL) {
					Telium_Fclose(hCam);
					hCam = NULL;
				}
				// A card has been removed
				bCardPresent = FALSE;
			}
		}
	}
}

//! \brief Wait card insertion before an EMV transaction.
void EMV_ServicesEmv_WaitCard(void){
	int bCardPresent;
	Telium_File_t* hCam;
	unsigned char ucStatus;

	hCam = NULL;


	hCam = Telium_Fopen("CAM0", "rw");

	bCardPresent = FALSE;
	if (hCam != NULL) {
		Telium_Status(hCam, &ucStatus);
		if ((ucStatus & CAM_PRESENT) == 0) {
			// Display message "Insert card"
			EMV_UI_MessageInsertCard();
		} else {
			if (hCam != NULL) {
				Telium_Fclose(hCam);
				hCam = NULL;
			}
			// A card is present
			bCardPresent = TRUE;
		}



		while(!bCardPresent) {
			Telium_Ttestall(CAM0, 0);
			Telium_Status(hCam, &ucStatus);

			if ((ucStatus & CAM_PRESENT) != 0) {
				if (hCam != NULL) {
					Telium_Fclose(hCam);
					hCam = NULL;
				}
				// A card is present
				bCardPresent = TRUE;
			}
		}
	}
}
