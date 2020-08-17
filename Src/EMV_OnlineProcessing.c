/**
 * \file
 * \brief This module implements the EMV API functionalities related to online processing
 * (Authorisation, Referral, Online Processing, Issuer Scripts and the 2nd Generate AC).
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
#include "EMV_ServicesEmv_OnlineProcessing.h"

#include <globals.h>
#include "EMV_Support.h"


//// Macros & preprocessor definitions //////////////////////////


#define CHK CHECK(ret>=0,lblKO)

//// Types //////////////////////////////////////////////////////

//// Static function definitions ////////////////////////////////

//// Global variables ///////////////////////////////////////////

////  variables ///////////////////////////////////////////

////  variables ///////////////////////////////////////////

//// Functions //////////////////////////////////////////////////
/** Save all tag values qV into data base.
 * \param    qV (I)  Queue containing TLVs to be saved into data base.
 * \source log\\emv.c
 */
void ServicesEmv_GetData(TLV_TREE_NODE *ptr_Tree) {
	TLV_TREE_NODE pTree;
	EPSTOOL_Data_t Elt;
	int processed = 0;
	char tagValue[999];
	card key = 0;
	int ret = 0;

	memset(tagValue,0,sizeof(tagValue));

	pTree = TlvTree_GetFirstChild(*ptr_Tree);

	while (pTree!=0) {
		EPSTOOL_TlvTree_GetData(pTree,&Elt);

		////get length and value
		memset(tagValue,0,sizeof(tagValue));

		key = mapKeyTag(Elt.tag);
		if ((key > emvAid) && (key < emvEnd)) {
			// No need to update the DB in this case these are static Data elements
		} else {
			num2hex(tagValue,Elt.length,2);
			bin2hex(&tagValue[2],Elt.value,Elt.length);

			ret = mapPut(key, tagValue, ((Elt.length)*2) + 2);
			CHK;
			///-----------  sequence number management patch  --------------
			if (key == emvPANSeq) {// 5F34 PAN sequence number
				ret = mapPut(traCrdSeq, tagValue, ((Elt.length)*2) + 2);
			}
			///-------------------------------------------------------------
		}
		lblKO://suppress error check in CHK

		pTree = TlvTree_GetNext(pTree);
		processed++;
	}

}

//! \brief Retrieve the last transaction performed with the same card.
//! \param[in] EMV_object The EMV transaction object to use.
//! \param[out] outputTlvTree Output TlvTree. If a transaction is found, it must be filled with TAG_EMV_INT_LAST_TRANSACTION_AMOUNT.
void servicesEmv_GetTransactionDetails(EMV_Object_t EMV_object){
	static const EPSTOOL_Tag_t tagsToGet[] = {
			TAG_EMV_TRACK_2_EQU_DATA,
			TAG_EMV_APPLI_PREFERRED_NAME,
			TAG_EMV_AID_TERMINAL,
			TAG_EMV_ISSUER_AUTHENTICATION_DATA,
			TAG_EMV_CRYPTOGRAM_INFO_DATA,
			TAG_EMV_ISSUER_APPLI_DATA,
			TAG_EMV_TRANSACTION_CURRENCY_CODE,
			TAG_EMV_TSI,
			TAG_EMV_AIP,
			TAG_EMV_DF_NAME,
			TAG_EMV_TRANSACTION_SEQUENCE_COUNTER,
			TAG_EMV_TVR,
			TAG_EMV_TRANSACTION_DATE,
			TAG_EMV_APPLICATION_CRYPTOGRAM,
			TAG_EMV_CVM_RESULTS,
			//TAG_EMV_APPLI_VERSION_NUMBER_TERM,
			TAG_EMV_TERMINAL_COUNTRY_CODE,
			TAG_EMV_TERMINAL_CAPABILITIES,
			TAG_EMV_TERMINAL_TYPE,
			TAG_EMV_TRANSACTION_TYPE,
			TAG_EMV_APPLI_PAN_SEQUENCE_NUMBER,
			TAG_EMV_AMOUNT_AUTH_NUM,
			TAG_EMV_AMOUNT_OTHER_NUM,
			TAG_EMV_APPLI_PAN,
			TAG_EMV_ATC,
			TAG_EMV_UNPREDICTABLE_NUMBER,
			TAG_EMV_APPLI_EXPIRATION_DATE,
			TAG_EMV_CARDHOLDER_NAME,
			TAG_EMV_AUTHORISATION_RESPONSE_CODE,
			tagMrcPrcCry,
	};
	TLV_TREE_NODE tlvTree;

	// Retrieve all the tags as in list above
	EMV_GetTags(EMV_object, sizeof(tagsToGet) / sizeof(tagsToGet[0]), tagsToGet, &tlvTree);
	if(tlvTree != NULL) {
		ServicesEmv_GetData(&tlvTree);
		EPSTOOL_TlvTree_Release(&tlvTree);
	}
}


////! \brief Called when the application must request an authorisation from the acquirer.
////! \param[in] inputTlvTree Input TlvTree that usually contains data for authorisation message.
////! \param[out] outputTlvTree Output TlvTree to be filled with TAG_EMV_INT_AUTHORISATION_RESULT tag.
//void EMV_ServicesEmv_Authorisation( TLV_TREE_NODE outputTlvTree) {
//	static const unsigned char authorisationResultApproved[1] = { 0x01 };
//	static const unsigned char authorisationResultDeclined[1] = { 0x00 };
//	static const unsigned char authorisationResultUnableToGoOnline[1] = { 0x02 };
//	char autCod[lenAutCod + 1];
//	char HexData[999 + 1];
//	char cardtype[3];
//	char rspEMV[6 + 1];
//	byte bindata[256];
//	int ret = 0,inputLen = 0;
//
//	memset(rspEMV, 0, sizeof(rspEMV));
//	memset(bindata, 0, sizeof(bindata));
//	memset(cardtype, 0, sizeof(cardtype));
//
//	ASSERT(outputTlvTree != NULL);
//
//	// TODO: Perform the authorisation
//	if (EMV_UI_MessageAuthorisation()) {
//
//		if(isApproved()){
//			VERIFY(TlvTree_AddChild(outputTlvTree, TAG_EMV_INT_AUTHORISATION_RESULT, authorisationResultApproved, sizeof(authorisationResultApproved)) != NULL);
//
//			memset(HexData, 0, sizeof(HexData));
//			mapGet(appCardName, cardtype, 3);
//			if (strncmp(cardtype, "UPI", 3) != 0) { //If its not UPI
//				mapGet(traRspCod, HexData, lenRspCod);
//				bindata[0] = HexData[0];
//				bindata[1] = HexData[1];
//			} else {
//				mapGet(emvRspCod, rspEMV, lenRspCod);
//				hex2bin(bindata, &rspEMV[2], ((strlen(rspEMV))/2) - 1);
//			}
//			VERIFY(TlvTree_AddChild(outputTlvTree, TAG_EMV_AUTHORISATION_RESPONSE_CODE, bindata, 2) != NULL);
//
//			memset(bindata, 0, sizeof(bindata));
//			memset(HexData, 0, sizeof(HexData));
//			MAPGET(emvIssAutDta, HexData, lblKO);
//			inputLen = strlen(HexData);
//			if (inputLen>4) {
//				if (strncmp(cardtype, "UPI", 3) == 0) { //If its UPI edit the response in IAD
//					HexData[inputLen-4] = rspEMV[2];
//					HexData[inputLen-3] = rspEMV[3];
//					HexData[inputLen-2] = rspEMV[4];
//					HexData[inputLen-1] = rspEMV[5];
//				}
//				hex2bin(bindata, &HexData[2], ((strlen(HexData))/2) - 1);
//				VERIFY(TlvTree_AddChild(outputTlvTree, TAG_EMV_ISSUER_AUTHENTICATION_DATA, bindata, ((strlen(HexData))/2) - 1) != NULL);
//			}
//
//			lblKO:
//			memset(autCod, 0, sizeof(autCod));
//			mapGet(traAutCod, autCod, 6);
//			VERIFY(TlvTree_AddChild(outputTlvTree, TAG_EMV_AUTHORISATION_CODE, autCod, 6) != NULL);
//
//		} else {
//			VERIFY(TlvTree_AddChild(outputTlvTree, TAG_EMV_INT_AUTHORISATION_RESULT, authorisationResultDeclined, sizeof(authorisationResultDeclined)) != NULL);
//		}
//
//	} else {
//		// User cancel
//		VERIFY(TlvTree_AddChild(outputTlvTree, TAG_EMV_INT_AUTHORISATION_RESULT, authorisationResultUnableToGoOnline, sizeof(authorisationResultUnableToGoOnline)) != NULL);
//	}
//
//}



//! \brief Called when the application must request an authorisation from the acquirer.
//! \param[in] inputTlvTree Input TlvTree that usually contains data for authorisation message.
//! \param[out] outputTlvTree Output TlvTree to be filled with TAG_EMV_INT_AUTHORISATION_RESULT tag.
void EMV_ServicesEmv_Authorisation( TLV_TREE_NODE outputTlvTree) {
	static const unsigned char authorisationResultApproved[1] = { 0x01 };
	static const unsigned char authorisationResultDeclined[1] = { 0x00 };
	static const unsigned char authorisationResultUnableToGoOnline[1] = { 0x02 };
	char autCod[lenAutCod + 1];
	char HexData[999 + 1];
	char cardtype[3];
	char rspEMV[6 + 1];
	byte bindata[256];
	int ret = 0,inputLen = 0;

	memset(rspEMV, 0, sizeof(rspEMV));
	memset(bindata, 0, sizeof(bindata));
	memset(cardtype, 0, sizeof(cardtype));

	ASSERT(outputTlvTree != NULL);

	// TODO: Perform the authorisation
	if (EMV_UI_MessageAuthorisation()) {

		if(isApproved()){
			VERIFY(TlvTree_AddChild(outputTlvTree, TAG_EMV_INT_AUTHORISATION_RESULT, authorisationResultApproved, sizeof(authorisationResultApproved)) != NULL);

			memset(HexData, 0, sizeof(HexData));
			mapGet(traRspCod, HexData, 2);
			VERIFY(TlvTree_AddChild(outputTlvTree, TAG_EMV_AUTHORISATION_RESPONSE_CODE, HexData, 2) != NULL);


			memset(bindata, 0, sizeof(bindata));
			memset(HexData, 0, sizeof(HexData));
			MAPGET(emvIssAutDta, HexData, lblKO);
			inputLen = strlen(HexData);
			if (inputLen>4) {
				hex2bin(bindata, &HexData[2], ((strlen(HexData))/2) - 1);
				VERIFY(TlvTree_AddChild(outputTlvTree, TAG_EMV_ISSUER_AUTHENTICATION_DATA, bindata, ((strlen(HexData))/2) - 1) != NULL);
			}

			lblKO:
			memset(autCod, 0, sizeof(autCod));
			mapGet(traAutCod, autCod, 6);
			VERIFY(TlvTree_AddChild(outputTlvTree, TAG_EMV_AUTHORISATION_CODE, autCod, 6) != NULL);

		} else {
			VERIFY(TlvTree_AddChild(outputTlvTree, TAG_EMV_INT_AUTHORISATION_RESULT, authorisationResultDeclined, sizeof(authorisationResultDeclined)) != NULL);
		}

	} else {
		// User cancel
		VERIFY(TlvTree_AddChild(outputTlvTree, TAG_EMV_INT_AUTHORISATION_RESULT, authorisationResultUnableToGoOnline, sizeof(authorisationResultUnableToGoOnline)) != NULL);
	}

}

//! \brief Called when the application must request a voice referral from the acquirer.
//! \param[in] inputTlvTree Input TlvTree.
//! \param[out] outputTlvTree Output TlvTree.
void EMV_ServicesEmv_VoiceReferral(TLV_TREE_NODE inputTlvTree, TLV_TREE_NODE outputTlvTree)
{
	// To avoid warnings because 'inputTlvTree' is not used
	(void)inputTlvTree;
	// To avoid warnings because 'outputTlvTree' is not used
	(void)outputTlvTree;

	ASSERT(inputTlvTree != NULL);
	ASSERT(outputTlvTree != NULL);

	// TODO: Perform the voice referral
}

//! \brief Execute the online processing step of an EMV transaction.
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
EMV_Status_t EMV_ServicesEmv_OnlineProcessing(EMV_Object_t EMV_object, TLV_TREE_NODE inputTlvTree, TLV_TREE_NODE *tagsKernelToAppli, TLV_TREE_NODE *tagsRequestedByKernel) {
	EMV_Status_t emvStatus;
	static const EPSTOOL_Tag_t tagsToGet[] = { TAG_EMV_INT_NEXT_STEP, TAG_EMV_INT_TRANSACTION_STATUS};

	//Get All tags from kernel into buffer
	servicesEmv_GetTransactionDetails(EMV_object);

	// Perform online authorisation. inputTlvTree is filled with TAG_EMV_INT_AUTHORISATION_RESULT tag.
	EMV_ServicesEmv_Authorisation( inputTlvTree);

	// Call the EMV API to perform Online Processing
	emvStatus = EMV_ExecuteStep(EMV_object, EMV_STEP_ONLINE_PROCESSING, inputTlvTree, sizeof(tagsToGet) / sizeof(tagsToGet[0]), tagsToGet, tagsKernelToAppli, tagsRequestedByKernel);

	return emvStatus;
}


static void ProcessEmv_Script1(TLV_TREE_NODE outputTlvTree){
	byte bindata[(lenIssSc1 * 2) + 3];
	char hexdata[(lenIssSc1 * 2) + 3];
	int inputLen = 0;
	int ret = 0;

	memset(bindata, 0, sizeof bindata);
	memset(hexdata, 0, sizeof hexdata);
	MAPGET(emvIssSc1, hexdata, lblKO);

	inputLen = strlen(hexdata);
	if (inputLen > 3) {
		hex2bin(bindata, &hexdata[2], (inputLen/2)-1);
		TlvTree_AddChild(outputTlvTree, TAG_EMV_ISSUER_SCRIPT_TEMPLATE_1, bindata,  (inputLen/2) - 1);
	}

	lblKO:;
}


//! \brief Execute the issuer script step of an EMV transaction.
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
EMV_Status_t EMV_ServicesEmv_ISP1(EMV_Object_t EMV_object, TLV_TREE_NODE inputTlvTree, TLV_TREE_NODE *tagsKernelToAppli, TLV_TREE_NODE *tagsRequestedByKernel) {
	EMV_Status_t emvStatus;
	static const EPSTOOL_Tag_t tagsToGet[] = { TAG_EMV_INT_NEXT_STEP, TAG_EMV_INT_TRANSACTION_STATUS };

	//TAG_EMV_ISSUER_SCRIPT_TEMPLATE_1 tags
	ProcessEmv_Script1(inputTlvTree);

	// Call the EMV API to perform issuer scripts
	emvStatus = EMV_ExecuteStep(EMV_object, EMV_STEP_ISSUER_TO_CARD_SCRIPTS_PROCESSING1, inputTlvTree, sizeof(tagsToGet) / sizeof(tagsToGet[0]), tagsToGet, tagsKernelToAppli, tagsRequestedByKernel);

	return emvStatus;
}

//
//void ProcessEmv_CompleteScript(TLV_TREE_NODE outputTlvTree){
//	byte bindata[256];
//	char HexData[256];
//	char tmp[4 + 1];
//	char rspEMV[6 + 1];
//	int inputLen = 0;
//	TLV_TREE_NODE Result;
//	int ret = 0;
//
//	//----- process tag 8A (Authorization Response code) ------
//	Result = TlvTree_New(0);
//	memset(tmp, 0, sizeof(tmp));
//	memset(bindata, 0, sizeof(bindata));
//	memset(rspEMV, 0, sizeof(rspEMV));
//	MAPGET(emvRspCod, rspEMV, lblKO);
//	inputLen = strlen(rspEMV);
//	if (inputLen > 3) {
//		hex2bin(bindata, &rspEMV[2], (inputLen/2));
//		Result = TlvTree_AddChild(outputTlvTree, TAG_EMV_AUTHORISATION_RESPONSE_CODE, bindata,(inputLen/2));
//		VERIFY(Result != NULL);
//	}
//	EPSTOOL_TlvTree_Release(&Result);
//
//	//----- process tag 91 (Issuer Authentication Data) ------
//	Result = TlvTree_New(0);
//	memset(bindata, 0, sizeof(bindata));
//	memset(HexData, 0, sizeof(HexData));
//	MAPGET(emvIssAutDta, HexData, lblKO);
//	inputLen = strlen(HexData);
//	if (inputLen > 3) {
//
//		mapGet(appCardName, tmp, 3);
//		if (strncmp(tmp, "UPI", 3) == 0) { //If its UPI edit the response in IAD
//			HexData[inputLen-4] = rspEMV[2];
//			HexData[inputLen-3] = rspEMV[3];
//			HexData[inputLen-2] = rspEMV[4];
//			HexData[inputLen-1] = rspEMV[5];
//		}
//		hex2bin(bindata, HexData, (inputLen/2));
//		Result = TlvTree_AddChild(outputTlvTree, TAG_EMV_ISSUER_AUTHENTICATION_DATA, bindata, (inputLen/2));
//		VERIFY(Result != NULL);
//	}
//	EPSTOOL_TlvTree_Release(&Result);
//
//	//----- process tag 89 (Authorization Code)------
//	Result = TlvTree_New(0);
//	memset(bindata, 0, sizeof(bindata));
//	memset(HexData, 0, sizeof(HexData));
//	MAPGET(traAutCod, HexData, lblKO);
//	inputLen = strlen(HexData);
//	if (inputLen > 3) {
//		Result = TlvTree_AddChild(outputTlvTree, TAG_EMV_AUTHORISATION_CODE, HexData, inputLen);
//		VERIFY(Result != NULL);
//	}
//	EPSTOOL_TlvTree_Release(&Result);
//
//	lblKO:;
//}

//! \brief Execute the completion step of an EMV transaction.
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
EMV_Status_t EMV_ServicesEmv_Completion(EMV_Object_t EMV_object, TLV_TREE_NODE inputTlvTree, TLV_TREE_NODE *tagsKernelToAppli, TLV_TREE_NODE *tagsRequestedByKernel) {
	EMV_Status_t emvStatus;
	static const EPSTOOL_Tag_t tagsToGet[] = { TAG_EMV_INT_NEXT_STEP, TAG_EMV_ISSUER_APPLI_DATA, TAG_EMV_INT_TRANSACTION_STATUS };

	//	// get data required for 2nd generate AC
	//	ProcessEmv_CompleteScript(inputTlvTree);

	// Call the EMV API to send the 2nd GENERATE AC command
	emvStatus = EMV_ExecuteStep(EMV_object, EMV_STEP_COMPLETION, inputTlvTree, sizeof(tagsToGet) / sizeof(tagsToGet[0]), tagsToGet, tagsKernelToAppli, tagsRequestedByKernel);

	return emvStatus;
}

static void ProcessEmv_Script2(TLV_TREE_NODE outputTlvTree){
	byte bindata[(lenIssSc2 * 2) + 3];
	char hexdata[(lenIssSc2 * 2) + 3];
	int inputLen = 0;
	int ret = 0;

	memset(bindata, 0, sizeof bindata);
	memset(hexdata, 0, sizeof hexdata);
	MAPGET(emvIssSc2, hexdata, lblKO);

	inputLen = strlen(hexdata);
	if (inputLen > 3) {
		hex2bin(bindata, &hexdata[2], (inputLen/2)-1);
		TlvTree_AddChild(outputTlvTree, TAG_EMV_ISSUER_SCRIPT_TEMPLATE_2, bindata,  sizeof(bindata));
	}

	lblKO:;
}

//! \brief Execute the issuer script step of an EMV transaction.
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
EMV_Status_t EMV_ServicesEmv_ISP2(EMV_Object_t EMV_object, TLV_TREE_NODE inputTlvTree, TLV_TREE_NODE *tagsKernelToAppli, TLV_TREE_NODE *tagsRequestedByKernel) {
	EMV_Status_t emvStatus;
	static const EPSTOOL_Tag_t tagsToGet[] = { TAG_EMV_INT_NEXT_STEP, TAG_EMV_INT_TRANSACTION_STATUS };

	// TAG_EMV_ISSUER_SCRIPT_TEMPLATE_2 tags
	ProcessEmv_Script2(inputTlvTree);

	// Call the EMV API to perform issuer scripts
	emvStatus = EMV_ExecuteStep(EMV_object, EMV_STEP_ISSUER_TO_CARD_SCRIPTS_PROCESSING2, inputTlvTree, sizeof(tagsToGet) / sizeof(tagsToGet[0]), tagsToGet, tagsKernelToAppli, tagsRequestedByKernel);

	return emvStatus;
}

