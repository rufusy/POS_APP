/**
 * \file
 * \brief This module implements the EMV API functionalities related to action analysis
 * (Terminal Risk Management, Terminal Action Analysis and Card Action Analysis).
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
#include "EMV_ServicesEmv_ActionAnalysis.h"

#include "globals.h"

//// Macros & preprocessor definitions //////////////////////////

//// Types //////////////////////////////////////////////////////

//// Static function definitions ////////////////////////////////

static void __EMV_ServicesEmv_GetLastTransaction(EMV_Object_t EMV_object, TLV_TREE_NODE outputTlvTree);

//// Global variables ///////////////////////////////////////////

////  variables ///////////////////////////////////////////

//// Functions //////////////////////////////////////////////////



/** Retrieve from data base PAN and Expiration Date in EMV format.
 * \n Then save it into data base in Ascii format.
 * \source log\\emv.c
 */
static void __EMV_ServicesEmv_ProcessEmvData() {
	char Track2Data[lenTrk2*2];
	char Pan[lenPan*2],ExpDat[lenExpDat*2];
	char srvCd[lenSrvCd*2];
	char *pTrk2;        // Buffer to accept the input
	int ret;


	memset(Pan, 0, sizeof(Pan));
	memset(ExpDat, 0, sizeof(ExpDat));
	memset(Track2Data, 0, sizeof(Track2Data));

	MAPGET(traTrk2, Track2Data, lblEnd);


	pTrk2 = &Track2Data[0];

	ret = fmtTok(0, pTrk2, "=");
	if(ret > lenPan){ ///system is using "D" as separator
		ret = fmtTok(0, pTrk2, "D");
		CHECK(ret <= lenPan, lblEnd);  // Search separator '='

		ret = fmtTok(Pan, pTrk2, "D");    // Retrieve Pan from track 2
		VERIFY(ret <= lenPan);
	}else{///system is using "=" as separator
		CHECK(ret <= lenPan, lblEnd);  // Search separator '='

		ret = fmtTok(Pan, pTrk2, "=");    // Retrieve Pan from track 2
		VERIFY(ret <= lenPan);
	}
	//ret = mapPutStr(traPan, Pan);

	pTrk2 += ret;         //Skip the pan
	pTrk2++;              //skip the separator  and Continue to analyse track 2

	ret = fmtSbs(ExpDat, pTrk2, 0, lenExpDat);    // Retrieve expiration date from track 2
	VERIFY(ret == 4);
	pTrk2 += ret;         //Skip the Expiry date
	MAPPUTSTR(traExpDat,ExpDat,lblEnd);


	ret = fmtSbs(srvCd, pTrk2, 0, lenSrvCd);    // Retrieve expiration date from track 2
	VERIFY(ret == 3);
	MAPPUTSTR(traSrvCd,srvCd,lblEnd);

	lblEnd:;
}

//! \brief Retrieve the last transaction performed with the same card.
//! \param[in] EMV_object The EMV transaction object to use.
//! \param[out] outputTlvTree Output TlvTree. If a transaction is found, it must be filled with TAG_EMV_INT_LAST_TRANSACTION_AMOUNT.
static void __EMV_ServicesEmv_GetLastTransaction(EMV_Object_t EMV_object, TLV_TREE_NODE outputTlvTree) {
	//EMV_Status_t emvStatus;
	static const EPSTOOL_Tag_t tagsToGet[] = { TAG_EMV_APPLI_PAN, TAG_EMV_APPLI_PAN_SEQUENCE_NUMBER, TAG_EMV_TRACK_2_EQU_DATA, TAG_EMV_APPLICATION_LABEL, TAG_EMV_CARDHOLDER_NAME, TAG_EMV_APPLICATION_CRYPTOGRAM, TAG_EMV_TSI, TAG_EMV_TVR, TAG_EMV_CVM_RESULTS };
	TLV_TREE_NODE tlvTree;

	TLV_TREE_NODE nodePan;
	EPSTOOL_Data_t dataPan;

	TLV_TREE_NODE nodePanSeqNumber;
	EPSTOOL_Data_t dataPanSeqNumber;

	TLV_TREE_NODE nodeApplicationLabel;
	EPSTOOL_Data_t dataApplicationLabel;

	TLV_TREE_NODE nodeCardHolderName;
	EPSTOOL_Data_t dataCardHolderName;

	TLV_TREE_NODE nodeEmvTrk2;
	EPSTOOL_Data_t dataTrk2;

	TLV_TREE_NODE nodeIssTrnCrt;
	EPSTOOL_Data_t dataIssTrnCrt;

	TLV_TREE_NODE nodeTVR;
	EPSTOOL_Data_t dataTVR;

	TLV_TREE_NODE nodeTSI;
	EPSTOOL_Data_t dataTSI;

	TLV_TREE_NODE nodeCVMR;
	EPSTOOL_Data_t dataCVMR;

	char tagValue[100];
	char tagTemp[100];


	// To avoid warnings because 'outputTlvTree' is not used
	(void)outputTlvTree;

	// Retrieve the PAN and the PAN Sequence Counter
	/*emvStatus =*/ EMV_GetTags(EMV_object, sizeof(tagsToGet) / sizeof(tagsToGet[0]), tagsToGet, &tlvTree);
	if(tlvTree != NULL) {
		nodePan = EPSTOOL_TlvTree_FindFirstData(tlvTree, TAG_EMV_APPLI_PAN, &dataPan);
		if (nodePan != NULL) {
			memset(tagValue, 0, sizeof(tagValue));
			bin2hex(tagValue,dataPan.value,dataPan.length);
			mapPut(traPan, tagValue, lenPan);
		}

		nodePanSeqNumber = EPSTOOL_TlvTree_FindFirstData(tlvTree, TAG_EMV_APPLI_PAN_SEQUENCE_NUMBER, &dataPanSeqNumber);
		if (nodePanSeqNumber != NULL) {
			memset(tagValue, 0, sizeof(tagValue));
			bin2hex(tagValue,dataPanSeqNumber.value,dataPanSeqNumber.length);
			mapPut(traCrdSeq, tagValue, lenCrdSeq);// 5F34 PAN sequence number
		}

		nodeEmvTrk2 = EPSTOOL_TlvTree_FindFirstData(tlvTree, TAG_EMV_TRACK_2_EQU_DATA, &dataTrk2);
		if (nodeEmvTrk2 != NULL) {
			memset(tagValue, 0, sizeof(tagValue));
			bin2hex(tagValue,dataTrk2.value,dataTrk2.length);
			mapPut(traTrk2, tagValue, (dataTrk2.length * 2));
		}

		nodeApplicationLabel = EPSTOOL_TlvTree_FindFirstData(tlvTree, TAG_EMV_APPLICATION_LABEL, &dataApplicationLabel);
		if (nodeApplicationLabel != NULL) {
			memset(tagValue, 0, sizeof(tagValue));
			bin2hex(tagValue,dataApplicationLabel.value,dataApplicationLabel.length);
			mapPut(traApplicationLabel, tagValue, (dataApplicationLabel.length * 2));
		}

		nodeCardHolderName = EPSTOOL_TlvTree_FindFirstData(tlvTree, TAG_EMV_CARDHOLDER_NAME, &dataCardHolderName);
		if (nodeCardHolderName != NULL) {
			memset(tagValue, 0, sizeof(tagValue));
			memset(tagTemp, 0, sizeof(tagTemp));
			bin2hex(tagValue,dataCardHolderName.value,dataCardHolderName.length);
			mapPut(traCardHolderName, tagValue, (dataCardHolderName.length * 2));
			Hex2Text(&tagTemp[0],tagValue,0);
			mapPut(traTrk1, tagTemp, strlen(tagTemp));
		}

		nodeIssTrnCrt = EPSTOOL_TlvTree_FindFirstData(tlvTree, TAG_EMV_APPLICATION_CRYPTOGRAM, &dataIssTrnCrt);
		if (nodeIssTrnCrt != NULL) {
			memset(tagValue, 0, sizeof(tagValue));
			bin2hex(tagValue,dataIssTrnCrt.value,dataIssTrnCrt.length);
			mapPut(traIssTrnCrt, tagValue, (dataIssTrnCrt.length * 2));
		}

		nodeTVR = EPSTOOL_TlvTree_FindFirstData(tlvTree, TAG_EMV_TVR, &dataTVR);
		if (nodeTVR != NULL) {
			memset(tagValue, 0, sizeof(tagValue));
			bin2hex(tagValue,dataTVR.value+1,dataTVR.length);
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

		__EMV_ServicesEmv_ProcessEmvData();

		//		 TODO: Get the last transaction amount
		//		 Set tag TAG_EMV_INT_LAST_TRANSACTION_AMOUNT

		// Free the memory
		EPSTOOL_TlvTree_Release(&tlvTree);
	}
}



//! \brief Execute the terminal risk management step of an EMV transaction.
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
EMV_Status_t EMV_ServicesEmv_TerminalRiskManagement(EMV_Object_t EMV_object, TLV_TREE_NODE inputTlvTree, TLV_TREE_NODE *tagsKernelToAppli, TLV_TREE_NODE *tagsRequestedByKernel) {
	EMV_Status_t emvStatus;
	static const EPSTOOL_Tag_t tagsToGet[] = { TAG_EMV_INT_NEXT_STEP };

	// Display message "Please wait"
	EMV_UI_MessagePleaseWait();

	__EMV_ServicesEmv_GetLastTransaction(EMV_object, inputTlvTree);

	// Call the EMV API to perform Terminal Risk Management
	emvStatus = EMV_ExecuteStep(EMV_object, EMV_STEP_TERMINAL_RISK_MANAGEMENT, inputTlvTree, sizeof(tagsToGet) / sizeof(tagsToGet[0]), tagsToGet, tagsKernelToAppli, tagsRequestedByKernel);

	return emvStatus;
}

//! \brief Execute the terminal action analysis step of an EMV transaction.
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
EMV_Status_t EMV_ServicesEmv_TerminalActionAnalysis(EMV_Object_t EMV_object, TLV_TREE_NODE inputTlvTree, TLV_TREE_NODE *tagsKernelToAppli, TLV_TREE_NODE *tagsRequestedByKernel) {
	EMV_Status_t emvStatus;
	static const EPSTOOL_Tag_t tagsToGet[] = { TAG_EMV_INT_NEXT_STEP };

	// Call the EMV API to perform Terminal Action Analysis
	emvStatus = EMV_ExecuteStep(EMV_object, EMV_STEP_TERMINAL_ACTION_ANALYSIS, inputTlvTree, sizeof(tagsToGet) / sizeof(tagsToGet[0]), tagsToGet, tagsKernelToAppli, tagsRequestedByKernel);

	return emvStatus;
}

//! \brief Execute the card action analysis step of an EMV transaction.
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
EMV_Status_t EMV_ServicesEmv_CardActionAnalysis(EMV_Object_t EMV_object, TLV_TREE_NODE inputTlvTree, TLV_TREE_NODE *tagsKernelToAppli, TLV_TREE_NODE *tagsRequestedByKernel) {
	EMV_Status_t emvStatus;
	static const EPSTOOL_Tag_t tagsToGet[] = { TAG_EMV_INT_NEXT_STEP, TAG_EMV_ISSUER_APPLI_DATA, TAG_EMV_INT_TRANSACTION_STATUS};

	// Call the EMV API to perform Card Action Analysis
	emvStatus = EMV_ExecuteStep(EMV_object, EMV_STEP_CARD_ACTION_ANALYSIS, inputTlvTree, sizeof(tagsToGet) / sizeof(tagsToGet[0]), tagsToGet, tagsKernelToAppli, tagsRequestedByKernel);

	return emvStatus;
}
