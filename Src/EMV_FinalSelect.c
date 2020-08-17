/**
 * \file
 * \brief This module implements the EMV API functionalities related to the Final Selection.
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
#include "EMV_ServicesEmv_AppSel.h"
#include "EMV_ServicesEmv_FinalSelect.h"

#include "globals.h"
#include "Sqlite.h"

//// Macros & preprocessor definitions //////////////////////////

//// Types //////////////////////////////////////////////////////

//// Static function definitions ////////////////////////////////

static void __EMV_ServicesEmv_GetAidData(TLV_TREE_NODE outputTlvTree);
static void __EMV_ServicesEmv_MenuChooseLanguage(TLV_TREE_NODE inputTlvTree);

//// Global variables ///////////////////////////////////////////

////  variables ///////////////////////////////////////////

//// Functions //////////////////////////////////////////////////
static int get_Bin_AID_Data(char * DBFieldName,unsigned char * BinData){
	char AID_selected[20];
	char Statement[256];
	char DataResponse[256];
	int aid_len = 0;
	int ret = 0;

	memset(Statement, 0, sizeof(Statement));
	memset(DataResponse, 0, sizeof(DataResponse));
	memset(AID_selected, 0, sizeof(AID_selected));

	AID_selected[0] = '%';
	mapGet(traAID, &AID_selected[1], lenAID);
	aid_len = strlen(AID_selected);
	AID_selected[aid_len] = '%';

	Telium_Sprintf(Statement, "SELECT %s FROM aid WHERE emvAid LIKE '%s';", DBFieldName, AID_selected);

	Sqlite_Run_Statement(Statement,DataResponse);

	if (strlen(DataResponse) < 1) {
		/// ----  here we use partial AID matching
		char AID_selected_Temp[20];

		memset(Statement, 0, sizeof(Statement));
		memset(DataResponse, 0, sizeof(DataResponse));
		memset(AID_selected, 0, sizeof(AID_selected));
		memset(AID_selected_Temp, 0, sizeof(AID_selected_Temp));

		AID_selected[0] = '%';
		mapGet(traAID, AID_selected_Temp, lenAID);
		strncat(&AID_selected[1], AID_selected_Temp, 10);
		aid_len = strlen(AID_selected);
		AID_selected[aid_len] = '%';

		Telium_Sprintf(Statement, "SELECT %s FROM aid WHERE emvAid LIKE '%s';", DBFieldName, AID_selected);

		Sqlite_Run_Statement(Statement,DataResponse);

	}

	ret = strlen(DataResponse);

	if(ret == 0)
		return 0;
	ret = (ret - 2) / 2;

	hex2bin(BinData, &DataResponse[2], ret);

	return ret;
}


static int get_Hex_AID_Data(char * DBFieldName, char * HexData, word SaveTo){
	char AID_selected[20];
	char Statement[256];
	char DataResponse[256];
	int aid_len = 0;
	int ret = 0;

	memset(Statement, 0, sizeof(Statement));
	memset(DataResponse, 0, sizeof(DataResponse));
	memset(AID_selected, 0, sizeof(AID_selected));

	AID_selected[0] = '%';
	mapGet(traAID, &AID_selected[1], lenAID);
	aid_len = strlen(AID_selected);
	AID_selected[aid_len] = '%';

	Telium_Sprintf(Statement, "SELECT %s FROM aid WHERE emvAid LIKE '%s';", DBFieldName, AID_selected);

	Sqlite_Run_Statement(Statement,DataResponse);

	if (strlen(DataResponse) < 1) {
		/// ----  here we use partial AID matching
		char AID_selected_Temp[20];

		memset(Statement, 0, sizeof(Statement));
		memset(DataResponse, 0, sizeof(DataResponse));
		memset(AID_selected, 0, sizeof(AID_selected));
		memset(AID_selected_Temp, 0, sizeof(AID_selected_Temp));

		AID_selected[0] = '%';
		mapGet(traAID, AID_selected_Temp, lenAID);
		strncat(&AID_selected[1], AID_selected_Temp, 10);
		aid_len = strlen(AID_selected);
		AID_selected[aid_len] = '%';

		Telium_Sprintf(Statement, "SELECT %s FROM aid WHERE emvAid LIKE '%s';", DBFieldName, AID_selected);

		Sqlite_Run_Statement(Statement,DataResponse);

	}

	mapPut(SaveTo, DataResponse, strlen(DataResponse));
	ret = strlen(DataResponse);

	if(ret == 0)
		return 0;

	strncpy(HexData, DataResponse, ret);

	return ret;
}

int mapGet_AID_Data(word emvkey ,unsigned char * BinData){
	char AID_selected[lenAID + 3];
	char Statement[256];
	char DataResponse[256];
	int aid_len = 0;
	char DBFieldName[100];

	memset(DBFieldName, 0, sizeof(DBFieldName));

	switch (emvkey) {
	case emvAid:
		strcpy(DBFieldName,"emvAid");
		break;
	case emvTACDft:
		strcpy(DBFieldName,"emvTACDft");
		break;
	case emvTACDen:
		strcpy(DBFieldName,"emvTACDen");
		break;
	case emvTACOnl:
		strcpy(DBFieldName,"emvTACOnl");
		break;
	case emvThrVal:
		strcpy(DBFieldName,"emvThrVal");
		break;
	case emvTarPer:
		strcpy(DBFieldName,"emvTarPer");
		break;
	case emvMaxTarPer:
		strcpy(DBFieldName,"emvMaxTarPer");
		break;
	case emvDftValDDOL:
		strcpy(DBFieldName,"emvDftValDDOL");
		break;
	case emvDftValTDOL:
		strcpy(DBFieldName,"emvDftValTDOL");
		break;
	case emvTrmAvn:
		strcpy(DBFieldName,"emvTrmAvn");
		break;
	case emvAcqId:
		strcpy(DBFieldName,"emvAcqId");
		break;
	case emvTrmFlrLim:
		strcpy(DBFieldName,"emvTrmFlrLim");
		break;
	case emvTCC:
		strcpy(DBFieldName,"emvTCC");
		break;
	case emvAidTxnType:
		strcpy(DBFieldName,"emvAidTxnType");
	default:
		break;
	}

	memset(Statement, 0, sizeof(Statement));
	memset(DataResponse, 0, sizeof(DataResponse));
	memset(AID_selected, 0, sizeof(AID_selected));

	AID_selected[0] = '%';
	mapGet(traAID, &AID_selected[1], lenAID);
	aid_len = strlen(AID_selected);
	AID_selected[aid_len] = '%';

	Telium_Sprintf(Statement, "SELECT %s FROM aid WHERE emvAid LIKE '%s';", DBFieldName, AID_selected);

	Sqlite_Run_Statement(Statement,DataResponse);

	if (strlen(DataResponse) < 1) {
		/// ----  here we use partial AID matching
		char AID_selected_Temp[lenAID + 3];

		memset(Statement, 0, sizeof(Statement));
		memset(DataResponse, 0, sizeof(DataResponse));
		memset(AID_selected, 0, sizeof(AID_selected));
		memset(AID_selected_Temp, 0, sizeof(AID_selected_Temp));

		AID_selected[0] = '%';
		mapGet(traAID, AID_selected_Temp, lenAID);
		strncat(&AID_selected[1], AID_selected_Temp, 10);
		aid_len = strlen(AID_selected);
		AID_selected[aid_len] = '%';

		Telium_Sprintf(Statement, "SELECT %s FROM aid WHERE emvAid LIKE '%s';", DBFieldName, AID_selected);

		Sqlite_Run_Statement(Statement,DataResponse);

	}

	hex2bin(BinData, DataResponse, 0);

	return strlen(DataResponse) / 2;

}

//! \brief Retrieves the parameters linked with an AID.
//! \param[out] outputTlvTree Output TlvTree that must be filled with the AID parameters.
static void __EMV_ServicesEmv_GetAidData(TLV_TREE_NODE outputTlvTree) {
	static unsigned char versionNumberTerminal[2] = { 0x00, 0x00 };
	static unsigned char terminalFloorLimit[4] = { 0x00, 0x00, 0x00, 0x00 };				// 100.00
	static unsigned char treshValueForBiasedRandSel[4] = { 0x00, 0x00, 0x07, 0xD0 };		// 20.00
	static unsigned char targPercForBiasedRandSel[1] = { 20 };							    // 20%
	static unsigned char maxTargPercForBiasedRandSel[1] = { 80 };							// 80%
	static unsigned char terminalActionCodeDenial[5] = { 0x00, 0x00, 0x00, 0x00, 0x00 };
	static unsigned char terminalActionCodeOnline[5] = { 0x00, 0x00, 0x00, 0x00, 0x00 };
	static unsigned char terminalActionCodeDefault[5] = { 0x00, 0x00, 0x00, 0x00, 0x00 };
	static unsigned char DefaultDDOL[3] = { 0x9F, 0x37, 0x04 };
	static unsigned char DefaultTDOL[15] = { 0x9F, 0x02, 0x06, 0x5F, 0x2A, 0x02, 0x9A, 0x03, 0x9C, 0x01, 0x95, 0x05, 0x9F, 0x37, 0x04 };
	char TempHex[32];

	ASSERT(outputTlvTree != NULL);

	memset(versionNumberTerminal, 0, sizeof(versionNumberTerminal));
	get_Bin_AID_Data("emvTrmAvn",(unsigned char *)versionNumberTerminal);
	VERIFY(TlvTree_AddChild(outputTlvTree, TAG_EMV_APPLI_VERSION_NUMBER_TERM, versionNumberTerminal, sizeof(versionNumberTerminal)) != NULL);

	memset(terminalFloorLimit, 0, sizeof(terminalFloorLimit));
	get_Bin_AID_Data("emvTrmFlrLim",(unsigned char *)terminalFloorLimit);
	VERIFY(TlvTree_AddChild(outputTlvTree, TAG_EMV_TERMINAL_FLOOR_LIMIT, terminalFloorLimit, sizeof(terminalFloorLimit)) != NULL);

	memset(treshValueForBiasedRandSel, 0, sizeof(treshValueForBiasedRandSel));
	get_Bin_AID_Data("emvThrVal",(unsigned char *)treshValueForBiasedRandSel);
	VERIFY(TlvTree_AddChild(outputTlvTree, TAG_EMV_INT_THRESHOLD_VALUE_BIASED_RAND_SEL, treshValueForBiasedRandSel, sizeof(treshValueForBiasedRandSel)) != NULL);

	memset(targPercForBiasedRandSel, 0, sizeof(targPercForBiasedRandSel));
	get_Bin_AID_Data("emvTarPer",(unsigned char *)targPercForBiasedRandSel);
	VERIFY(TlvTree_AddChild(outputTlvTree, TAG_EMV_INT_TARGET_PERC_RAND_SEL, targPercForBiasedRandSel, sizeof(targPercForBiasedRandSel)) != NULL);

	memset(maxTargPercForBiasedRandSel, 0, sizeof(maxTargPercForBiasedRandSel));
	get_Bin_AID_Data("emvMaxTarPer",(unsigned char *)maxTargPercForBiasedRandSel);
	VERIFY(TlvTree_AddChild(outputTlvTree, TAG_EMV_INT_MAX_TARGET_PERC_BIASED_RAND_SEL, maxTargPercForBiasedRandSel, sizeof(maxTargPercForBiasedRandSel)) != NULL);

	memset(terminalActionCodeDenial, 0, sizeof(terminalActionCodeDenial));
	get_Bin_AID_Data("emvTACDen",(unsigned char *)terminalActionCodeDenial);
	VERIFY(TlvTree_AddChild(outputTlvTree, TAG_EMV_INT_TAC_DENIAL, terminalActionCodeDenial, sizeof(terminalActionCodeDenial)) != NULL);

	memset(terminalActionCodeOnline, 0, sizeof(terminalActionCodeOnline));
	get_Bin_AID_Data("emvTACOnl",(unsigned char *)terminalActionCodeOnline);
	VERIFY(TlvTree_AddChild(outputTlvTree, TAG_EMV_INT_TAC_ONLINE, terminalActionCodeOnline, sizeof(terminalActionCodeOnline)) != NULL);

	memset(terminalActionCodeDefault, 0, sizeof(terminalActionCodeDefault));
	get_Bin_AID_Data("emvTACDft",(unsigned char *)terminalActionCodeDefault);
	VERIFY(TlvTree_AddChild(outputTlvTree, TAG_EMV_INT_TAC_DEFAULT, terminalActionCodeDefault, sizeof(terminalActionCodeDefault)) != NULL);

	memset(DefaultDDOL, 0, sizeof(DefaultDDOL));
	get_Bin_AID_Data("emvDftValDDOL",(unsigned char *)DefaultDDOL);
	VERIFY(TlvTree_AddChild(outputTlvTree, TAG_EMV_INT_DEFAULT_DDOL, DefaultDDOL, sizeof(DefaultDDOL)) != NULL);

	memset(DefaultTDOL, 0, sizeof(DefaultTDOL));
	get_Bin_AID_Data("emvDftValTDOL",(unsigned char *)DefaultTDOL);
	VERIFY(TlvTree_AddChild(outputTlvTree, TAG_EMV_INT_DEFAULT_TDOL, DefaultTDOL, sizeof(DefaultTDOL)) != NULL);

	//Data for processing Later on from db
	memset(TempHex, 0, sizeof(TempHex));
	get_Hex_AID_Data("emvAidName",TempHex, appCardName);


}


//! \brief Called when the cardholder must select the language to use.
//! \param[in] inputTlvTree Input TlvTree that contains the TAG_EMV_INT_CARDHOLDER_LANGUAGES tag.
static void __EMV_ServicesEmv_MenuChooseLanguage(TLV_TREE_NODE inputTlvTree) {
	// TODO Select the language to use
}

//! \brief Called after the whole application select process.
//! \param[out] outputTlvTree Output TlvTree.
EMV_Status_t EMV_ServicesEmv_GetAmount(TLV_TREE_NODE outputTlvTree) {
	EMV_Status_t emvStatus;
	unsigned long amount;

	// TODO: Customise if required
	emvStatus = EMV_STATUS_SUCCESS;

	// Is the amount known?
	if (!EMV_ServicesEmv_AmountIsSet()) {
		// Neither Amount Binary nor Amount Numeric is set
		// => Ask for the amount
		if (EMV_UI_AmountEntry(&amount)) {
			if (!EMV_ServicesEmv_AmountSet(amount, outputTlvTree)) {
				// Cannot set the amount (not enough memory)
				emvStatus = EMV_STATUS_NOT_ENOUGH_MEMORY;
			}
		} else {
			// The amount entry has been cancelled
			emvStatus = EMV_STATUS_CANCEL;
		}
	}

	// Set the transaction status
	EMV_ServicesEmv_TransacStatusSet(emvStatus, NULL);
	return emvStatus;
}

//! \brief Called when the cardholder must select the account to use.
//! \param[out] outputTlvTree Output TlvTree that must contain the selected account type in TAG_EMV_ACCOUNT_TYPE.
EMV_Status_t EMV_ServicesEmv_MenuAccountType(TLV_TREE_NODE outputTlvTree) {
	unsigned char accountType = 0x00;
	EMV_Status_t emvStatus;

	// To avoid warnings because 'outputTlvTree' is not used
	(void)outputTlvTree;

	ASSERT(outputTlvTree != NULL);

	//	// Select the account type
	//	if (EMV_UI_MenuSelectAccountType(&accountType)) {
	//		// Set tag TAG_ACCOUNT_TYPE
	//		VERIFY(TlvTree_AddChild(outputTlvTree, TAG_EMV_ACCOUNT_TYPE, &accountType, sizeof(accountType)) != NULL);
	//		emvStatus = EMV_STATUS_SUCCESS;
	//	} else {
	//		emvStatus = EMV_STATUS_CANCEL;
	//	}

	EMV_UI_MenuSelectAccountType(&accountType);

	/// skipped selection process
	// Set tag TAG_ACCOUNT_TYPE
	VERIFY(TlvTree_AddChild(outputTlvTree, TAG_EMV_ACCOUNT_TYPE, &accountType, sizeof(accountType)) != NULL);
	emvStatus = EMV_STATUS_SUCCESS;


	return emvStatus;
}

//! \brief Execute the final select step of an EMV transaction.
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
EMV_Status_t EMV_ServicesEmv_FinalSelect(EMV_Object_t EMV_object, TLV_TREE_NODE inputTlvTree, TLV_TREE_NODE *tagsKernelToAppli, TLV_TREE_NODE *tagsRequestedByKernel)
{
	EMV_Status_t emvStatus;
	static const EPSTOOL_Tag_t tagsToGet[] = { TAG_EMV_INT_NEXT_STEP, TAG_EMV_INT_CARDHOLDER_LANGUAGES };

	emvStatus = EMV_STATUS_UNKNOWN;

	// Display message "Please wait"
	EMV_UI_MessagePleaseWait();

	// Get the EMV global data (terminal type, ...)
	EMV_ServicesEmv_GetGlobalParam(inputTlvTree);

	// Get the transaction data (amount, date, ...)
	if(EMV_ServicesEmv_GetTransactionData(inputTlvTree)) {
		// Get the parameters linked with an AID
		__EMV_ServicesEmv_GetAidData(inputTlvTree);

		// Call the EMV API to perform final selection
		emvStatus = EMV_ExecuteStep(EMV_object, EMV_STEP_FINAL_SELECT, inputTlvTree, sizeof(tagsToGet) / sizeof(tagsToGet[0]), tagsToGet, tagsKernelToAppli, tagsRequestedByKernel);
		if(emvStatus == EMV_STATUS_MISSING_MANDATORY_TERM_DATA) {
			mapPutByte(appFallback, 1);
		}else if (emvStatus == EMV_STATUS_SELECT_ANOTHER_AID) {
			mapPutByte(appFallback, 1);
		}
		if(emvStatus == EMV_STATUS_SUCCESS)
			__EMV_ServicesEmv_MenuChooseLanguage(*tagsKernelToAppli);
	} else {
		emvStatus = EMV_STATUS_UNEXPECTED_ERROR;
	}


	return emvStatus;
}
