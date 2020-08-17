/**
 * \author	Ingenico
 * \author	Copyright (c) 2009 Ingenico, rue claude Chappe,\n
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
#include "globals.h"
#include "EPSTOOL_Convert.h"



/////////////////////////////////////////////////////////////////
//// Macros & preprocessor definitions //////////////////////////

#define SAMPLE_PAYPASS_ONLINE_BUFFER_SIZE				2048		// Default size for shared buffer used for online authorisation
#define SAMPLE_PAYPASS_GET_ALL_DATA_SIZE				(32 * 1024)	// Default size for shared buffer used for getting all data


/////////////////////////////////////////////////////////////////
//// Global data definition /////////////////////////////////////

static T_SHARED_DATA_STRUCT * gs_pTransactionDataLog;			// Structure used to log the transaction data
static int gDataExchangeInProgress;								// Indication that the application is still performing Data Exchange processing


/////////////////////////////////////////////////////////////////
//// Static functions definition ////////////////////////////////

static int __Cless_PayPass_IsReceiptRequired (T_SHARED_DATA_STRUCT * pResultDataStruct, int * pIsReceiptRequired);
static int __Cless_PayPass_RetreiveTransactionOutcome (T_SHARED_DATA_STRUCT * pResultDataStruct, int * pTransactionOutcome);
static int __Cless_PayPass_RetrieveStart (T_SHARED_DATA_STRUCT * pResultDataStruct, int * pStart);
static int __Cless_PayPass_RetreiveUirdMessage (T_SHARED_DATA_STRUCT * pResultDataStruct, int * pUirdMessage);
static int __Cless_PayPass_RetreiveCvmToApply (T_SHARED_DATA_STRUCT * pResultDataStruct, unsigned char * pCvm);
static int __Cless_PayPass_RetreiveCardType (T_SHARED_DATA_STRUCT * pResultDataStruct, unsigned short * pCardType);
static int __Cless_PayPass_RetreiveErrorIndicationByte (T_SHARED_DATA_STRUCT * pResultDataStruct, const unsigned int nByteNumber, unsigned char * pErrorIndicationByte);

static int __Cless_PayPass_AddPayPassSpecificData (T_SHARED_DATA_STRUCT * pDataStruct);
static T_Bool __Cless_PayPass_OnlinePinManagement (T_SHARED_DATA_STRUCT * pStructureForOnlineData, int nCardHolderLang);
static void __Cless_PayPass_AddRecordToBatch (T_SHARED_DATA_STRUCT * pSharedData, unsigned short usCardType);
static int __Cless_PayPass_FillBufferForOnlineAuthorisation (T_SHARED_DATA_STRUCT * pTransactionData, T_SHARED_DATA_STRUCT * pOnlineAuthorisationData);
static void __Cless_PayPass_FormatAmount (unsigned char ucFormat, char *ucCurrencyLabel, unsigned char ucPosition, const unsigned char * pAmount, unsigned int nAmountLength, char *pFormattedAmountMessage);
static void __Cless_PayPass_DisplayUirdMsg (T_SHARED_DATA_STRUCT * pDataStruct, const int bPhoneMessage);
//static void __Cless_PayPass_DumpDataExchangeInformation (T_SHARED_DATA_STRUCT * pDataStruct);
//static void __Cless_PayPass_PrintMCWFormat (unsigned char * pReadValue, unsigned long ulReadLength);
static int __Cless_PayPass_RetreiveFieldOffValue (T_SHARED_DATA_STRUCT * pResultDataStruct, unsigned char * pFieldOff);


/////////////////////////////////////////////////////////////////
//// Functions //////////////////////////////////////////////////

//! \brief Dump the Outcome Parameter Set.
//!	\param[in] pOpsValue Outcome Parameter Set value.
//! \param[in] ulOpsLength Outcome Parameter Set length.

static void __Cless_PayPass_DumpOps (unsigned char * pOpsValue, unsigned long ulOpsLength)
{
	(void) ulOpsLength;

	// Open the output driver
	if (Cless_DumpData_DumpOpenOutputDriver())
	{
		Cless_DumpData ("OUTCOME PARAMETER SET (OUT)");

		if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER)
			Cless_DumpData ("\x0f");

		Cless_DumpData_DumpNewLine();

		// Dump the status
		switch (pOpsValue[PAYPASS_OPS_STATUS_BYTE])
		{
		case PAYPASS_OPS_STATUS_APPROVED:
			Cless_DumpData (" - Status : APPROVED");
			break;
		case PAYPASS_OPS_STATUS_DECLINED:
			Cless_DumpData (" - Status : DECLINED");
			break;
		case PAYPASS_OPS_STATUS_ONLINE_REQUEST:
			Cless_DumpData (" - Status : ONLINE REQUEST");
			break;
		case PAYPASS_OPS_STATUS_END_APPLICATION:
			Cless_DumpData (" - Status : END APPLICATION");
			break;
		case PAYPASS_OPS_STATUS_SELECT_NEXT:
			Cless_DumpData (" - Status : SELECT NEXT");
			break;
		case PAYPASS_OPS_STATUS_TRY_AGAIN:
			Cless_DumpData (" - Status : TRY AGAIN");
			break;
		case PAYPASS_OPS_STATUS_TRY_ANOTHER_INTERFACE:
			Cless_DumpData (" - Status : TRY ANOTHER INTERFACE");
			break;
		case PAYPASS_OPS_STATUS_NA:
			Cless_DumpData (" - Status : N/A");
			break;
		default:
			Cless_DumpData (" - Status : RFU = %02X", pOpsValue[PAYPASS_OPS_STATUS_BYTE]);
			break;
		}
		Cless_DumpData_DumpNewLine();

		// Dump the start
		switch (pOpsValue[PAYPASS_OPS_START_BYTE])
		{
		case PAYPASS_OPS_START_A:
			Cless_DumpData (" - Start : START A");
			break;
		case PAYPASS_OPS_START_B:
			Cless_DumpData (" - Start : START B");
			break;
		case PAYPASS_OPS_START_C:
			Cless_DumpData (" - Start : START C");
			break;
		case PAYPASS_OPS_START_D:
			Cless_DumpData (" - Start : START D");
			break;
		case PAYPASS_OPS_START_NA:
			Cless_DumpData (" - Start : N/A");
			break;
		default:
			Cless_DumpData (" - Start : RFU = %02X", pOpsValue[PAYPASS_OPS_START_BYTE]);
			break;
		}
		Cless_DumpData_DumpNewLine();

		// Dump the online response data
		switch (pOpsValue[PAYPASS_OPS_ONLINE_RESPONSE_DATA_BYTE])
		{
		case PAYPASS_OPS_ONLINE_RESPONSE_DATA_NA:
			Cless_DumpData (" - Online Resp Data : N/A");
			break;
		default:
			Cless_DumpData (" - Online Resp Data : RFU = %02X", pOpsValue[PAYPASS_OPS_ONLINE_RESPONSE_DATA_BYTE]);
			break;
		}
		Cless_DumpData_DumpNewLine();

		// Dump the CVM
		switch (pOpsValue[PAYPASS_OPS_CVM_BYTE])
		{
		case PAYPASS_OPS_CVM_NO_CVM:
			Cless_DumpData (" - CVM : NO CVM");
			break;
		case PAYPASS_OPS_CVM_SIGNATURE:
			Cless_DumpData (" - CVM : SIGNATURE");
			break;
		case PAYPASS_OPS_CVM_ONLINE_PIN:
			Cless_DumpData (" - CVM : ONLINE PIN");
			break;
		case PAYPASS_OPS_CVM_CONFIRMATION_CODE_VERIFIED:
			Cless_DumpData (" - CVM : CONFIRMATION CODE VERIFIED");
			break;
		case PAYPASS_OPS_CVM_NA:
			Cless_DumpData (" - CVM : N/A");
			break;
		default:
			Cless_DumpData (" - CVM : RFU = %02X", pOpsValue[PAYPASS_OPS_CVM_BYTE]);
			break;
		}
		Cless_DumpData_DumpNewLine();

		// Dump the presence
		Cless_DumpData (" - Presence :");
		Cless_DumpData_DumpNewLine();

		if (pOpsValue[PAYPASS_OPS_DATA_PRESENCE_BYTE] & PAYPASS_OPS_DATA_PRESENCE_MASK_UIR_ON_OUTCOME)
			Cless_DumpData ("   - UIR on outome : YES");
		else
			Cless_DumpData ("   - UIR on outome : NO");
		Cless_DumpData_DumpNewLine();

		if (pOpsValue[PAYPASS_OPS_DATA_PRESENCE_BYTE] & PAYPASS_OPS_DATA_PRESENCE_MASK_UIR_ON_RESTART)
			Cless_DumpData ("   - UIR on restart : YES");
		else
			Cless_DumpData ("   - UIR on restart : NO");
		Cless_DumpData_DumpNewLine();

		if (pOpsValue[PAYPASS_OPS_DATA_PRESENCE_BYTE] & PAYPASS_OPS_DATA_PRESENCE_MASK_DATA_RECORD)
			Cless_DumpData ("   - Data Record : YES");
		else
			Cless_DumpData ("   - Data Record : NO");
		Cless_DumpData_DumpNewLine();

		if (pOpsValue[PAYPASS_OPS_DATA_PRESENCE_BYTE] & PAYPASS_OPS_DATA_PRESENCE_MASK_DISCRET_DATA)
			Cless_DumpData ("   - Discretionary Data : YES");
		else
			Cless_DumpData ("   - Discretionary Data : NO");
		Cless_DumpData_DumpNewLine();

		if (pOpsValue[PAYPASS_OPS_DATA_PRESENCE_BYTE] & PAYPASS_OPS_DATA_PRESENCE_MASK_RECEIPT)
			Cless_DumpData ("   - Receipt : YES");
		else
			Cless_DumpData ("   - Receipt : NO");
		Cless_DumpData_DumpNewLine();

		// Dump the alternative interface preference
		switch (pOpsValue[PAYPASS_OPS_ALTERNATIVE_INTERFACE_PREF_BYTE])
		{
		case PAYPASS_OPS_ALTERNATIVE_INTERFACE_PREF_NA:
			Cless_DumpData (" - Alt Interface pref : N/A");
			break;
		default:
			Cless_DumpData (" - Alt Interface pref : %02X", pOpsValue[PAYPASS_OPS_ALTERNATIVE_INTERFACE_PREF_BYTE]);
			break;
		}
		Cless_DumpData_DumpNewLine();

		// Dump the field off request
		switch (pOpsValue[PAYPASS_OPS_FIELD_OFF_REQUEST_BYTE])
		{
		case PAYPASS_OPS_FIELD_OFF_REQUEST_NA:
			Cless_DumpData (" - Field off req : N/A");
			break;
		default:
			Cless_DumpData (" - Field off req : %02X", pOpsValue[PAYPASS_OPS_FIELD_OFF_REQUEST_BYTE]);	
			break;
		}
		Cless_DumpData_DumpNewLine();

		// Dump the removal timeout
		Cless_DumpData (" - Removal TO : %02X", pOpsValue[PAYPASS_OPS_REMOVAL_TIMEOUT_BYTE]);
		Cless_DumpData_DumpNewLine();

		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();

		if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER)
			Cless_DumpData ("\x1b" "@");

		// Close the output driver
		Cless_DumpData_DumpCloseOutputDriver();
	}
}



//! \brief Dump the Error Indication.
//!	\param[in] pEiValue Outcome Parameter Set value.
//! \param[in] ulEiLength Outcome Parameter Set length.

static void __Cless_PayPass_DumpEi (unsigned char * pEiValue, unsigned long ulEiLength)
{
	(void) ulEiLength;

	// Open the output driver
	if (Cless_DumpData_DumpOpenOutputDriver())
	{
		Cless_DumpData ("ERROR INDICATION");

		if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER)
			Cless_DumpData ("\x0f");

		Cless_DumpData_DumpNewLine();

		// Dump L1 error
		switch (pEiValue[PAYPASS_EI_L1_BYTE])
		{
		case PAYPASS_EI_L1_OK:
			Cless_DumpData (" - L1 : OK");
			break;
		case PAYPASS_EI_L1_TIMEOUT_ERROR:
			Cless_DumpData (" - L1 : TIMEOUT ERROR");
			break;
		case PAYPASS_EI_L1_TRANSMISSION_ERROR:
			Cless_DumpData (" - L1 : TRANSMISSION ERROR");
			break;
		case PAYPASS_EI_L1_PROTOCOL_ERROR:
			Cless_DumpData (" - L1 : PROTOCOL ERROR");
			break;
		default:
			Cless_DumpData (" - L1 : RFU = %02X", pEiValue[PAYPASS_EI_L1_BYTE]);
			break;
		}
		Cless_DumpData_DumpNewLine();

		// Dump L2 error
		switch (pEiValue[PAYPASS_EI_L2_BYTE])
		{
		case PAYPASS_EI_L2_OK:
			Cless_DumpData (" - L2 : OK");
			break;
		case PAYPASS_EI_L2_CARD_DATA_MISSING:
			Cless_DumpData (" - L2 : CARD DATA MISSING");
			break;
		case PAYPASS_EI_L2_CAM_FAILED:
			Cless_DumpData (" - L2 : CAM FAILED");
			break;
		case PAYPASS_EI_L2_STATUS_BYTES:
			Cless_DumpData (" - L2 : STATUS BYTES");
			break;
		case PAYPASS_EI_L2_PARSING_ERROR:
			Cless_DumpData (" - L2 : PARSING ERROR");
			break;
		case PAYPASS_EI_L2_MAX_LIMIT_EXCEEDED:
			Cless_DumpData (" - L2 : MAX LIMIT EXCEEDED");
			break;
		case PAYPASS_EI_L2_CARD_DATA_ERROR:
			Cless_DumpData (" - L2 : CARD DATA ERROR");
			break;
		case PAYPASS_EI_L2_MAGSTRIPE_NOT_SUPPORTED:
			Cless_DumpData (" - L2 : MAGSTRIPE NOT SUPPORTED");
			break;
		case PAYPASS_EI_L2_NO_PPSE:
			Cless_DumpData (" - L2 : NO PPSE");
			break;
		case PAYPASS_EI_L2_PPSE_FAULT:
			Cless_DumpData (" - L2 : PPSE FAULT");
			break;
		case PAYPASS_EI_L2_EMPTY_CANDIDATE_LIST:
			Cless_DumpData (" - L2 : EMPTY CANDIDATE LIST");
			break;
		case PAYPASS_EI_L2_IDS_READ_ERROR:
			Cless_DumpData (" - L2 : IDS READ ERROR");
			break;
		case PAYPASS_EI_L2_IDS_WRITE_ERROR:
			Cless_DumpData (" - L2 : IDS WRITE ERROR");
			break;
		case PAYPASS_EI_L2_IDS_DATA_ERROR:
			Cless_DumpData (" - L2 : IDS DATA ERROR");
			break;
		case PAYPASS_EI_L2_IDS_NO_MATCHING_AC:
			Cless_DumpData (" - L2 : IDS NO MATCHING AC");
			break;
		case PAYPASS_EI_L2_TERMINAL_DATA_ERROR:
			Cless_DumpData (" - L2 : TERMINAL DATA ERROR");
			break;
		default:
			Cless_DumpData (" - L2 : RFU = %02X", pEiValue[PAYPASS_EI_L2_BYTE]);
			break;
		}
		Cless_DumpData_DumpNewLine();

		// Dump L3 error
		switch (pEiValue[PAYPASS_EI_L3_BYTE])
		{
		case PAYPASS_EI_L3_OK:
			Cless_DumpData (" - L3 : OK");
			break;
		case PAYPASS_EI_L3_TIMEOUT:
			Cless_DumpData (" - L3 : TIMEOUT");
			break;
		case PAYPASS_EI_L3_STOP:
			Cless_DumpData (" - L3 : STOP");
			break;
		case PAYPASS_EI_L3_AMOUNT_NOT_PRESENT:
			Cless_DumpData (" - L3 : AMOUNT NOT PRESENT");
			break;
		default:
			Cless_DumpData (" - L3 : RFU = %02X", pEiValue[PAYPASS_EI_L3_BYTE]);
			break;
		}
		Cless_DumpData_DumpNewLine();

		// Dump SW
		Cless_DumpData (" - SW : %02X%02X", pEiValue[PAYPASS_EI_SW12_OFFSET], pEiValue[PAYPASS_EI_SW12_OFFSET+1]);
		Cless_DumpData_DumpNewLine();

		// Dump MSG on error
		switch (pEiValue[PAYPASS_EI_MSG_ON_ERROR_BYTE])
		{
		case PAYPASS_UIRD_MESSAGE_ID_CARD_READ_OK:
			Cless_DumpData (" - Msg on error : CARD READ OK");
			break;
		case PAYPASS_UIRD_MESSAGE_ID_TRY_AGAIN:
			Cless_DumpData (" - Msg on error : TRY AGAIN");
			break;
		case PAYPASS_UIRD_MESSAGE_ID_APPROVED:
			Cless_DumpData (" - Msg on error : APPROVED");
			break;
		case PAYPASS_UIRD_MESSAGE_ID_APPROVED_SIGN:
			Cless_DumpData (" - Msg on error : APPROVED SIGN");
			break;
		case PAYPASS_UIRD_MESSAGE_ID_DECLINED:
			Cless_DumpData (" - Msg on error : DECLINED");
			break;
		case PAYPASS_UIRD_MESSAGE_ID_ERROR_OTHER_CARD:
			Cless_DumpData (" - Msg on error : ERROR OTHER CARD");
			break;
		case PAYPASS_UIRD_MESSAGE_ID_SEE_PHONE:
			Cless_DumpData (" - Msg on error : SEE PHONE");
			break;
		case PAYPASS_UIRD_MESSAGE_ID_AUTHORISING_PLEASE_WAIT:
			Cless_DumpData (" - Msg on error : AUTHORISING PLEASE WAIT");
			break;
		case PAYPASS_UIRD_MESSAGE_ID_INSERT_CARD:
			Cless_DumpData (" - Msg on error : INSERT CARD");
			break;
		case PAYPASS_UIRD_MESSAGE_ID_CLEAR_DISPLAY:
			Cless_DumpData (" - Msg on error : CLEAR DISPLAY");
			break;
		case PAYPASS_UIRD_MESSAGE_ID_NA:
			Cless_DumpData (" - Msg on error : N/A");
			break;
		default:
			Cless_DumpData (" - Msg on error : RFU = %02X", pEiValue[PAYPASS_EI_MSG_ON_ERROR_BYTE]);
			break;
		}
		Cless_DumpData_DumpNewLine();

		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();

		if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER)
			Cless_DumpData ("\x1b" "@");

		// Close the output driver
		Cless_DumpData_DumpCloseOutputDriver();
	}
}



//! \brief Dump the User Interface Request Data.
//!	\param[in] pUirdValue User Interface Request Data value.
//! \param[in] ulUirdLength User Interface Request Data length.

static void __Cless_PayPass_DumpUird (unsigned char * pUirdValue, unsigned long ulUirdLength, const int bIsOutUird)
{
	(void) ulUirdLength;

	// Open the output driver
	if (Cless_DumpData_DumpOpenOutputDriver())
	{
		if (bIsOutUird)
			Cless_DumpData ("USER INTERFACE REQ DATA (OUT)");
		else
			Cless_DumpData ("USER INTERFACE REQ DATA (MSG)");

		if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER)
			Cless_DumpData ("\x0f");

		Cless_DumpData_DumpNewLine();

		// Dump Message Identifier
		switch (pUirdValue[PAYPASS_UIRD_MESSAGE_ID_BYTE])
		{
		case PAYPASS_UIRD_MESSAGE_ID_CARD_READ_OK:
			Cless_DumpData (" - Msg Id : CARD READ OK");
			break;
		case PAYPASS_UIRD_MESSAGE_ID_TRY_AGAIN:
			Cless_DumpData (" - Msg Id : TRY AGAIN");
			break;
		case PAYPASS_UIRD_MESSAGE_ID_APPROVED:
			Cless_DumpData (" - Msg Id : APPROVED");
			break;
		case PAYPASS_UIRD_MESSAGE_ID_APPROVED_SIGN:
			Cless_DumpData (" - Msg Id : APPROVED SIGN");
			break;
		case PAYPASS_UIRD_MESSAGE_ID_DECLINED:
			Cless_DumpData (" - Msg Id : DECLINED");
			break;
		case PAYPASS_UIRD_MESSAGE_ID_ERROR_OTHER_CARD:
			Cless_DumpData (" - Msg Id : ERROR OTHER CARD");
			break;
		case PAYPASS_UIRD_MESSAGE_ID_SEE_PHONE:
			Cless_DumpData (" - Msg Id : SEE PHONE");
			break;
		case PAYPASS_UIRD_MESSAGE_ID_AUTHORISING_PLEASE_WAIT:
			Cless_DumpData (" - Msg Id : AUTHORISING PLEASE WAIT");
			break;
		case PAYPASS_UIRD_MESSAGE_ID_INSERT_CARD:
			Cless_DumpData (" - Msg Id : INSERT CARD");
			break;
		case PAYPASS_UIRD_MESSAGE_ID_CLEAR_DISPLAY:
			Cless_DumpData (" - Msg Id : CLEAR DISPLAY");
			break;
		case PAYPASS_UIRD_MESSAGE_ID_NA:
			Cless_DumpData (" - Msg Id : N/A");
			break;
		default:
			Cless_DumpData (" - Msg Id : RFU = %02X", pUirdValue[PAYPASS_UIRD_MESSAGE_ID_BYTE]);
			break;
		}
		Cless_DumpData_DumpNewLine();

		// Dump Status
		switch (pUirdValue[PAYPASS_UIRD_STATUS_BYTE])
		{
		case PAYPASS_UIRD_STATUS_NOT_READY:
			Cless_DumpData (" - Status : NOT READY");
			break;
		case PAYPASS_UIRD_STATUS_IDLE:
			Cless_DumpData (" - Status : IDLE");
			break;
		case PAYPASS_UIRD_STATUS_READY_TO_READ:
			Cless_DumpData (" - Status : READY TO READ");
			break;
		case PAYPASS_UIRD_STATUS_PROCESSING:
			Cless_DumpData (" - Status : PROCESSING");
			break;
		case PAYPASS_UIRD_STATUS_CARD_READ_SUCCESSFULLY:
			Cless_DumpData (" - Status : CARD READ SUCCESSFULLY");
			break;
		case PAYPASS_UIRD_STATUS_PROCESSING_ERROR:
			Cless_DumpData (" - Status : PROCESSING ERROR");
			break;
		case PAYPASS_UIRD_STATUS_NA:
			Cless_DumpData (" - Status : N/A");
			break;
		default:
			Cless_DumpData (" - Status : RFU = %02X", pUirdValue[PAYPASS_UIRD_STATUS_BYTE]);
			break;
		}
		Cless_DumpData_DumpNewLine();

		// Dump Hold Time
		Cless_DumpData (" - Hold Time : %02X%02X%02X", pUirdValue[PAYPASS_UIRD_HOLD_TIME_OFFSET], pUirdValue[PAYPASS_UIRD_HOLD_TIME_OFFSET+1], pUirdValue[PAYPASS_UIRD_HOLD_TIME_OFFSET+2]);
		Cless_DumpData_DumpNewLine();

		// Dump language preference
		Cless_DumpData (" - Lang Pref : %02X%02X%02X%02X%02X%02X%02X%02X", pUirdValue[PAYPASS_UIRD_LANGUAGE_PREFERENCE_OFFSET], pUirdValue[PAYPASS_UIRD_LANGUAGE_PREFERENCE_OFFSET+1], pUirdValue[PAYPASS_UIRD_LANGUAGE_PREFERENCE_OFFSET+2], pUirdValue[PAYPASS_UIRD_LANGUAGE_PREFERENCE_OFFSET+3], pUirdValue[PAYPASS_UIRD_LANGUAGE_PREFERENCE_OFFSET+4], pUirdValue[PAYPASS_UIRD_LANGUAGE_PREFERENCE_OFFSET+5], pUirdValue[PAYPASS_UIRD_LANGUAGE_PREFERENCE_OFFSET+6], pUirdValue[PAYPASS_UIRD_LANGUAGE_PREFERENCE_OFFSET+7]);
		Cless_DumpData_DumpNewLine();

		// Dump value qualifier
		switch (pUirdValue[PAYPASS_UIRD_VALUE_QUALIFIER_BYTE])
		{
		case PAYPASS_UIRD_VALUE_QUALIFIER_NONE:
			Cless_DumpData (" - Value qual : NONE");
			break;
		case PAYPASS_UIRD_VALUE_QUALIFIER_AMOUNT:
			Cless_DumpData (" - Value qual : AMOUNT");
			break;
		case PAYPASS_UIRD_VALUE_QUALIFIER_BALANCE:
			Cless_DumpData (" - Value qual : BALANCE");
			break;
		default:
			Cless_DumpData (" - Value qual : RFU = %02X", pUirdValue[PAYPASS_UIRD_VALUE_QUALIFIER_BYTE]);
			break;
		}
		Cless_DumpData_DumpNewLine();

		// Dump value
		Cless_DumpData (" - Value : %02X%02X%02X%02X%02X%02X", pUirdValue[PAYPASS_UIRD_VALUE_OFFSET], pUirdValue[PAYPASS_UIRD_VALUE_OFFSET+1], pUirdValue[PAYPASS_UIRD_VALUE_OFFSET+2], pUirdValue[PAYPASS_UIRD_VALUE_OFFSET+3], pUirdValue[PAYPASS_UIRD_VALUE_OFFSET+4], pUirdValue[PAYPASS_UIRD_VALUE_OFFSET+5]);
		Cless_DumpData_DumpNewLine();

		// Dump currency code
		Cless_DumpData (" - Currency code : %02X%02X", pUirdValue[PAYPASS_UIRD_CURRENCY_CODE_OFFSET], pUirdValue[PAYPASS_UIRD_CURRENCY_CODE_OFFSET+1]);
		Cless_DumpData_DumpNewLine();

		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();

		if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER)
			Cless_DumpData ("\x1b" "@");

		// Close the output driver
		Cless_DumpData_DumpCloseOutputDriver();
	}
}



//! \brief Dump the Data Record.
//!	\param[in] pDataRecordValue Data Record value.
//! \param[in] ulDataRecordLength Data Record length.

static void __Cless_PayPass_DumpDataRecord (unsigned char * pDataRecordValue, unsigned long ulDataRecordLength) {
	T_SHARED_DATA_STRUCT tDataRecord;

	if (GTL_SharedExchange_InitEx (&tDataRecord, ulDataRecordLength+1, ulDataRecordLength, pDataRecordValue) == STATUS_SHARED_EXCHANGE_OK) {
		// Dump the data record
		if (Cless_DumpData_DumpOpenOutputDriver()) {
			Cless_DumpData ("DATA RECORD");
			Cless_DumpData_DumpNewLine();

			Cless_DumpData_DumpCloseOutputDriver();
		}

		// Dump the shared buffer
		Cless_DumpData_DumpSharedBuffer (&tDataRecord, 0);
	} else {
		// An error occurred when initialising the structure
		GTL_Traces_TraceDebug ("__Cless_PayPass_DumpDataRecord : Data record structure initialisation failed");
	}
}



//! \brief Dump the Discretionary Data.
//!	\param[in] pDiscretionaryDataValue Discretionary Data value.
//! \param[in] ulDiscretionaryDataLength Discretionary Data length.

static void __Cless_PayPass_DumpDiscretionaryData (unsigned char * pDiscretionaryDataValue, unsigned long ulDiscretionaryDataLength)
{
	T_SHARED_DATA_STRUCT tDiscretionaryData;

	if (GTL_SharedExchange_InitEx (&tDiscretionaryData, ulDiscretionaryDataLength+1, ulDiscretionaryDataLength, pDiscretionaryDataValue) == STATUS_SHARED_EXCHANGE_OK) {
		// Dump the data record
		if (Cless_DumpData_DumpOpenOutputDriver()) {
			Cless_DumpData ("DISCRETIONARY DATA");
			Cless_DumpData_DumpNewLine();

			Cless_DumpData_DumpCloseOutputDriver();
		}

		// Dump the shared buffer
		Cless_DumpData_DumpSharedBuffer (&tDiscretionaryData, 0);
	}
	else
	{
		// An error occurred when initialising the structure
		GTL_Traces_TraceDebug ("__Cless_PayPass_DumpDiscretionaryData : Discretionary data structure initialisation failed");
	}
}


//! \brief Dump the RRP Processing Time.
//!	\param[in] pRrpProcessingTimeValue RRP Processing Time value.
//! \param[in] pRrpProcessingTimeValue RRP Processing Time length.

static void __Cless_PayPass_DumpRrpProcessingTime (unsigned char * pRrpProcessingTimeValue, unsigned long ulRrpProcessingTimeLength)
{
	unsigned long ulRrpTime = 0;
	// Dump the data record
	if (Cless_DumpData_DumpOpenOutputDriver())
	{
		GTL_Convert_BinNumberToUl(pRrpProcessingTimeValue, &ulRrpTime, ulRrpProcessingTimeLength);
		Cless_DumpData ("###################");
		Cless_DumpData_DumpNewLine();
		Cless_DumpData ("RRP PROCESSING TIME");
		Cless_DumpData_DumpNewLine();
		Cless_DumpData ("Decimal: %08lu", ulRrpTime);
		Cless_DumpData_DumpNewLine();
		Cless_DumpData ("Hex: %08lX", ulRrpTime);
		Cless_DumpData_DumpNewLine();
		Cless_DumpData ("###################");
		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();

		Cless_DumpData_DumpCloseOutputDriver();
	}
}



//! \brief Request a tag to the PayPass kernel.
//! \param[in,out] pDataRequest Structure already initialised and allocated.
//! \param[in] ulTag tag to be requested.
//! \return
//!	- \a TRUE if correctly required.
//!	- \a FALSE if an error occurred.

static int __Cless_PayPass_RequestDataToKernel (T_SHARED_DATA_STRUCT * pDataRequest, unsigned long ulTag)
{
	int nResult = TRUE;
	int cr;

	if (pDataRequest != NULL)
	{
		// Clear shared buffer
		GTL_SharedExchange_ClearEx (pDataRequest, FALSE);

		// Indicate tag to be requested
		GTL_SharedExchange_AddTag (pDataRequest, ulTag, 0, NULL);

		// Request the data to the kernel
		cr = PayPass3_GetData (pDataRequest);

		if (cr != KERNEL_STATUS_OK)
		{
			// An error occurred
			GTL_Traces_TraceDebug("__Cless_PayPass_RequestDataToKernel : An error occured when getting tag to the PayPass kernel (cr=%02x)", cr);
			nResult = FALSE;
		}
	}
	else
	{
		// An error occurred
		GTL_Traces_TraceDebug("__Cless_PayPass_RequestDataToKernel : Provided structure is not initialised");
		nResult = FALSE;
	}

	return (nResult);
}



//! \brief Dump a specific PayPass tag.
//! \param[in] ulTag Specific tag to be dumped.
//! \param[in] bRequestToKernelIfNotPresent Boolean that indicates if the tag must requested or not to the kernel if it is not present in pExchangeStruct.
//!	- \ref TAG_PAYPASS_OUTCOME_PARAMETER_SET
//!	- \ref TAG_PAYPASS_ERROR_INDICATION
//!	- \ref TAG_PAYPASS_USER_INTERFACE_REQUEST_DATA
//!	- \ref TAG_PAYPASS_DATA_RECORD
//!	- \ref TAG_PAYPASS_DISCRETIONARY_DATA

void Cless_PayPass_DumpSpecificStructure (T_SHARED_DATA_STRUCT * pExchangeStruct, unsigned long ulTag, int bRequestToKernelIfNotPresent) {
	unsigned long ulReadLength;
	unsigned char * pReadValue;
	T_SHARED_DATA_STRUCT * pRequestData = NULL;
	int nPosition;
	int bTagFound = FALSE;

	if (gs_pTransactionDataLog == NULL)
		gs_pTransactionDataLog = GTL_SharedExchange_InitLocal (10240);

	if (gs_pTransactionDataLog != NULL) {
		if ((ulTag == TAG_PAYPASS_OUTCOME_PARAMETER_SET) || (ulTag == TAG_PAYPASS_ERROR_INDICATION) || (ulTag == TAG_PAYPASS_USER_INTERFACE_REQUEST_DATA) || (ulTag == TAG_PAYPASS_DATA_RECORD) || (ulTag == TAG_PAYPASS_DISCRETIONARY_DATA) || (ulTag == TAG_PAYPASS_RRP_MEASURED_PROCESSING_TIME))
		{
			// Check if the tag is present in pExchangeStruct
			nPosition = SHARED_EXCHANGE_POSITION_NULL;

			if (GTL_SharedExchange_FindNext (pExchangeStruct, &nPosition, ulTag, &ulReadLength, (const unsigned char **)&pReadValue) != STATUS_SHARED_EXCHANGE_OK) {
				if (bRequestToKernelIfNotPresent) {
					// Allocate the shared buffer
					pRequestData = GTL_SharedExchange_InitShared (1024);

					if (pRequestData != NULL)
					{
						// Tag not found, request it to the kernel
						if (__Cless_PayPass_RequestDataToKernel (pRequestData, ulTag))
						{
							// Check if the tag is present in pExchangeStruct
							nPosition = SHARED_EXCHANGE_POSITION_NULL;

							if (GTL_SharedExchange_FindNext (pRequestData, &nPosition, ulTag, &ulReadLength, (const unsigned char **)&pReadValue) == STATUS_SHARED_EXCHANGE_OK)
							{
								bTagFound = TRUE;
							}
						}
					}
				}
			}
			else
			{
				bTagFound = TRUE;
			}

			if (bTagFound)
			{
				GTL_SharedExchange_AddTag (gs_pTransactionDataLog, ulTag, ulReadLength, pReadValue);
			}
		}
	}

	// Destroy allocated structure (if allocated)
	if (pRequestData != NULL)
		GTL_SharedExchange_DestroyShare (pRequestData);
}



//! \brief Dump the PayPass transaction data (for debug purpose).

void Cless_PayPass_DumpTransationDataLog (void)
{
	unsigned long ulReadTag, ulReadLength;
	unsigned char * pReadValue;
	unsigned long ulReadTagTmp, ulReadLengthTmp;
	unsigned char * pReadValueTmp;
	int nPosition;
	int nPositionTmp;
	int nResult;
	int nResultTmp;
	int bIsOutUird = FALSE;

	if (gs_pTransactionDataLog != NULL)
	{
		// Check if the tag is present in pExchangeStruct
		nPosition = SHARED_EXCHANGE_POSITION_NULL;

		do
		{
			bIsOutUird = FALSE;

			nResult = GTL_SharedExchange_GetNext (gs_pTransactionDataLog, &nPosition, &ulReadTag, &ulReadLength, (const unsigned char **)&pReadValue);

			if (nResult == STATUS_SHARED_EXCHANGE_OK)
			{
				switch (ulReadTag)
				{
				case TAG_PAYPASS_OUTCOME_PARAMETER_SET:
					__Cless_PayPass_DumpOps (pReadValue, ulReadLength);
					break;
				case TAG_PAYPASS_ERROR_INDICATION:
					__Cless_PayPass_DumpEi (pReadValue, ulReadLength);
					break;
				case TAG_PAYPASS_USER_INTERFACE_REQUEST_DATA:
					nPositionTmp = nPosition;
					nResultTmp = GTL_SharedExchange_GetNext (gs_pTransactionDataLog, &nPositionTmp, &ulReadTagTmp, &ulReadLengthTmp, (const unsigned char **)&pReadValueTmp);
					if (nResultTmp == STATUS_SHARED_EXCHANGE_OK)
					{
						if (ulReadTagTmp == TAG_PAYPASS_OUTCOME_PARAMETER_SET)
						{
							if (((pReadValueTmp[PAYPASS_OPS_DATA_PRESENCE_BYTE] & PAYPASS_OPS_DATA_PRESENCE_MASK_UIR_ON_RESTART) == PAYPASS_OPS_DATA_PRESENCE_MASK_UIR_ON_RESTART) ||
									((pReadValueTmp[PAYPASS_OPS_DATA_PRESENCE_BYTE] & PAYPASS_OPS_DATA_PRESENCE_MASK_UIR_ON_OUTCOME) == PAYPASS_OPS_DATA_PRESENCE_MASK_UIR_ON_OUTCOME))
							{
								bIsOutUird = TRUE;
							}
						}
					}
					__Cless_PayPass_DumpUird (pReadValue, ulReadLength, bIsOutUird);
					break;
				case TAG_PAYPASS_DATA_RECORD:
					__Cless_PayPass_DumpDataRecord (pReadValue, ulReadLength);
					break;
				case TAG_PAYPASS_DISCRETIONARY_DATA:
					__Cless_PayPass_DumpDiscretionaryData (pReadValue, ulReadLength);
					break;
				case TAG_PAYPASS_RRP_MEASURED_PROCESSING_TIME:
					__Cless_PayPass_DumpRrpProcessingTime (pReadValue, ulReadLength);
					break;
				default:
					break;
				}
			}			
		}
		while (nResult == STATUS_SHARED_EXCHANGE_OK);

		GTL_SharedExchange_DestroyLocal (gs_pTransactionDataLog);
		gs_pTransactionDataLog = NULL;
	}
}


//
////! \brief Dump the PayPass data exchange and APDU data (for debug purpose).
//void __Cless_PayPass_DumpDataExchangeInformation (T_SHARED_DATA_STRUCT * pDataStruct) {
//	unsigned long ulReadTag, ulReadLength, character;
//	unsigned char * pReadValue;
//	int nPosition;
//	int nResult = FALSE;
//	unsigned char bDriverOpened, bDriverAlreadyOpened;
//	TLV_TREE_NODE hFileNameNode;
//	unsigned int nLength;
//	unsigned int nIndex;
//	int nMissingDET=FALSE;
//
//
//	if (!Cless_DumpData_DumpOpenOutputDriver())
//		return;
//
//	if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER)
//		Cless_DumpData("\x1b" "@\x1b" "E");
//
//	Cless_DumpData ("DEK/DET/APDU");
//	Cless_DumpData_DumpNewLine();
//	Cless_DumpData_DumpNewLine();
//
//	if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER)
//		Cless_DumpData("\x1b" "@");
//
//	Cless_DumpData_DumpCloseOutputDriver();
//
//	// Check if the tag is present in pExchangeStruct
//	nPosition = SHARED_EXCHANGE_POSITION_NULL;
//
//	if (Cless_DumpData_DumpGetOutputHandle() == NULL) {
//		bDriverOpened = Cless_DumpData_DumpOpenOutputDriver();
//		bDriverAlreadyOpened = FALSE;
//	} else {
//		bDriverOpened = TRUE;
//		bDriverAlreadyOpened = TRUE;
//	}
//
//	if (bDriverOpened) {
//		if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER)
//			Cless_DumpData ("\x0f");
//
//		if (pDataStruct != NULL) {
//			// Dump the File Name
//			hFileNameNode = TlvTree_Find (pTreeCurrentDataExchange, MCW_XML_TAG_FILE_NAME, 0);
//
//			if (hFileNameNode != NULL) {
//				// Get the node data
//				pReadValue = NULL;
//				nLength = TlvTree_GetLength (hFileNameNode);
//				if (nLength > 0)
//					pReadValue = TlvTree_GetData (hFileNameNode);
//
//				Cless_DumpData ("MCW XML FILE: ");
//				for (nIndex=0; nIndex<nLength; nIndex++)
//					Cless_DumpData ("%c", pReadValue[nIndex]);
//				Cless_DumpData_DumpNewLine();
//				Cless_DumpData ("LL: See DEKDET.PAR for MCW format");
//				Cless_DumpData_DumpNewLine();
//				Cless_DumpData_DumpNewLine();
//			}
//
//			do {
//				character = 0;
//				nResult = GTL_SharedExchange_GetNext (pDataStruct, &nPosition, &ulReadTag, &ulReadLength, (const unsigned char **)&pReadValue);
//
//				if (nResult == STATUS_SHARED_EXCHANGE_OK) {
//					switch (ulReadTag) {
//					case TAG_PAYPASS_INT_SENT_APDU:
//						Cless_DumpData ("C-APDU: ");
//						Cless_DumpData_DumpNewLine();
//						for (nIndex=0;nIndex<ulReadLength; nIndex++) {
//							Cless_DumpData ("%02X", pReadValue[nIndex]);
//
//							if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER) {
//								character++;
//
//								// Limit the print output to 24 characters to ensure a new line is printed
//								if (character == 24) {
//									character=0;
//									Cless_DumpData_DumpNewLine();
//								}
//							}
//						}
//						Cless_DumpData_DumpNewLine();
//						Cless_DumpData_DumpNewLine();
//						break;
//
//
//					case TAG_PAYPASS_INT_CARD_RESPONSE:
//						Cless_DumpData ("R-APDU: ");
//						Cless_DumpData_DumpNewLine();
//						for (nIndex=0;nIndex<ulReadLength; nIndex++) {
//							Cless_DumpData ("%02X", pReadValue[nIndex]);
//							if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER) {
//								character++;
//
//								// Limit the print output to 24 characters to ensure a new line is printed
//								if (character == 24) {
//									character=0;
//									Cless_DumpData_DumpNewLine();
//								}
//							}
//						}
//						Cless_DumpData_DumpNewLine();
//						Cless_DumpData_DumpNewLine();
//						break;
//
//					case TAG_PAYPASS_INT_DEK_SIGNAL:
//
//						nMissingDET = TRUE;
//						Cless_DumpData ("DEK SIGNAL: ");
//						Cless_DumpData_DumpNewLine();
//						for (nIndex=0;nIndex<ulReadLength; nIndex++) {
//							Cless_DumpData ("%02X", pReadValue[nIndex]);
//							if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER) {
//								character++;
//
//								// Limit the print output to 24 characters to ensure a new line is printed
//								if (character == 24) {
//									character=0;
//									Cless_DumpData_DumpNewLine();
//								}
//							}
//						}
//						Cless_DumpData_DumpNewLine();
//						Cless_DumpData_DumpNewLine();
//
//						Cless_DumpData ("DEK SIGNAL (MCW): ");
//						Cless_DumpData_DumpNewLine();
//
//						// Print the data in MCW format
//						__Cless_PayPass_PrintMCWFormat (pReadValue, ulReadLength);
//
//						break;
//
//					case TAG_PAYPASS_INT_DET_SIGNAL:
//
//						nMissingDET = FALSE;
//						Cless_DumpData ("DET SIGNAL: ");
//						Cless_DumpData_DumpNewLine();
//						for (nIndex=0;nIndex<ulReadLength; nIndex++) {
//							Cless_DumpData ("%02X", pReadValue[nIndex]);
//							if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER) {
//								character++;
//
//								// Limit the print output to 24 characters to ensure a new line is printed
//								if (character == 24) {
//									character=0;
//									Cless_DumpData_DumpNewLine();
//								}
//							}
//						}
//						Cless_DumpData_DumpNewLine();
//						Cless_DumpData_DumpNewLine();
//
//						Cless_DumpData ("DET SIGNAL (MCW): ");
//						Cless_DumpData_DumpNewLine();
//
//						// Print the data in MCW format
//						__Cless_PayPass_PrintMCWFormat (pReadValue, ulReadLength);
//
//						break;
//
//					default:
//						break;
//					}
//				}
//			}
//			while (nResult == STATUS_SHARED_EXCHANGE_OK);
//
//			// indicate if there was a DEK Signal mismatching
//			if ((nMissingDET) && (!Cless_DataExchange_GetDEKSignalStatus())) {
//				Cless_DumpData_DumpNewLine();
//				Cless_DumpData ("Unexpected DEK Signal");
//				Cless_DumpData_DumpNewLine();
//			}
//		}
//
//
//		if (!bDriverAlreadyOpened) {
//			Cless_DumpData_DumpNewLine();
//			Cless_DumpData_DumpNewLine();
//			Cless_DumpData_DumpNewLine();
//			Cless_DumpData_DumpNewLine();
//
//			if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER)
//				Cless_DumpData("\x1b" "@");
//			Cless_DumpData_DumpCloseOutputDriver();
//		}
//	}
//}



//! \brief Check if a receipt is required or not (read from the Outcome Parameter Set).
//! \param[in] pResultDataStruct Structure containing the C2 kernel output.
//! \param[out] pIsReceiptRequired \a TRUE if the receipt is required, \a FALSE else.
//! \return
//!		- \ref TRUE if correctly retreived.
//!		- \ref FALSE if an error occured.

static int __Cless_PayPass_IsReceiptRequired (T_SHARED_DATA_STRUCT * pResultDataStruct, int * pIsReceiptRequired) {
	int nResult = TRUE;
	int nPosition, cr;
	unsigned long ulReadLength;
	const unsigned char * pReadValue;

	// Init position
	nPosition = SHARED_EXCHANGE_POSITION_NULL;

	// Init output data
	if (pIsReceiptRequired != NULL)
		*pIsReceiptRequired = FALSE;

	// Get the Outcome Parameter Set
	cr = GTL_SharedExchange_FindNext(pResultDataStruct, &nPosition, TAG_PAYPASS_OUTCOME_PARAMETER_SET, &ulReadLength, &pReadValue);

	if (cr != STATUS_SHARED_EXCHANGE_OK) {
		GTL_Traces_TraceDebug("__Cless_PayPass_IsReceiptRequired : Unable to get the outcome parameter set from the C2 kernel response (cr = %02x)", cr);
		nResult = FALSE;
		goto End;
	}

	// Get the transaction outcome
	if (pIsReceiptRequired != NULL)
		*pIsReceiptRequired = ((pReadValue[PAYPASS_OPS_DATA_PRESENCE_BYTE] & PAYPASS_OPS_DATA_PRESENCE_MASK_RECEIPT) == PAYPASS_OPS_DATA_PRESENCE_MASK_RECEIPT);

	End:
	return (nResult);
}



//! \brief Get the PayPass transaction outcome from the Outcome Parameter Set.
//! \param[in] pResultDataStruct Structure containing the C2 kernel output.
//! \param[out] pTransactionOutcome Retreived transaction outcome :
//!		- \a PAYPASS_OPS_STATUS_APPROVED if transaction is approved.
//!		- \a PAYPASS_OPS_STATUS_ONLINE_REQUEST if an online authorisation is requested.
//!		- \a PAYPASS_OPS_STATUS_DECLINED if the transaction is declined.
//!		- \a PAYPASS_OPS_STATUS_SELECT_NEXT if next AID shall be selected.
//!		- \a PAYPASS_OPS_STATUS_TRY_AGAIN transaction shall be restarted from the begining..
//!		- \a PAYPASS_OPS_STATUS_END_APPLICATION if the transaction is terminated.
//! \return
//!		- \ref TRUE if correctly retreived.
//!		- \ref FALSE if an error occured.

static int __Cless_PayPass_RetreiveTransactionOutcome (T_SHARED_DATA_STRUCT * pResultDataStruct, int * pTransactionOutcome) {
	int nResult = TRUE;
	int nPosition, cr;
	unsigned long ulReadLength;
	const unsigned char * pReadValue;

	// Init position
	nPosition = SHARED_EXCHANGE_POSITION_NULL;

	// Init output data
	if (pTransactionOutcome != NULL)
		*pTransactionOutcome = PAYPASS_OPS_STATUS_END_APPLICATION;

	// Get the Outcome Parameter Set
	cr = GTL_SharedExchange_FindNext(pResultDataStruct, &nPosition, TAG_PAYPASS_OUTCOME_PARAMETER_SET, &ulReadLength, &pReadValue);

	if (cr != STATUS_SHARED_EXCHANGE_OK) {
		GTL_Traces_TraceDebug("__Cless_PayPass_RetreiveTransactionOutcome : Unable to get the outcome parameter set from the C2 kernel response (cr = %02x)", cr);
		nResult = FALSE;
		goto End;
	}

	// Get the transaction outcome
	if (pTransactionOutcome != NULL)
		*pTransactionOutcome = pReadValue[PAYPASS_OPS_STATUS_BYTE];

	End:
	return (nResult);
}



//! \brief Get the start byte from the Outcome Parameter Set.
//! \param[in] pResultDataStruct Structure containing the C2 kernel output.
//! \param[out] pStart Retrieved start value :
//!		- \a PAYPASS_OPS_START_A if transaction needs to start at A.
//!		- \a PAYPASS_OPS_START_B if transaction needs to start at B.
//!		- \a PAYPASS_OPS_START_C if transaction needs to start at C.
//!		- \a PAYPASS_OPS_START_D if transaction needs to start at D.
//! \return
//!		- \ref TRUE if correctly retreived.
//!		- \ref FALSE if an error occured.

static int __Cless_PayPass_RetrieveStart (T_SHARED_DATA_STRUCT * pResultDataStruct, int * pStart) {
	int nResult = TRUE;
	int nPosition, cr;
	unsigned long ulReadLength;
	const unsigned char * pReadValue;

	// Init position
	nPosition = SHARED_EXCHANGE_POSITION_NULL;

	// Init output data
	if (pStart != NULL)
		*pStart = PAYPASS_OPS_START_NA;

	// Get the Outcome Parameter Set
	cr = GTL_SharedExchange_FindNext(pResultDataStruct, &nPosition, TAG_PAYPASS_OUTCOME_PARAMETER_SET, &ulReadLength, &pReadValue);

	if (cr != STATUS_SHARED_EXCHANGE_OK) {
		GTL_Traces_TraceDebug("__Cless_PayPass_RetrieveStart : Unable to get the outcome parameter set from the C2 kernel response (cr = %02x)", cr);
		nResult = FALSE;
		goto End;
	}

	// Get the start value
	if (pStart != NULL)
		*pStart = pReadValue[PAYPASS_OPS_START_BYTE];

	End:
	return (nResult);
}



//! \brief Get the PayPass Error Indication required byte (extracted from the Discretionary Data).
//! \param[in] pResultDataStruct Structure containing the C2 kernel output.
//! \param[in] nByteNumber byte number to be returned (shall be in [0;5]).
//! \param[out] pErrorIndicationByte Retreived byte.Retreived transaction outcome :
//!		- Value if correctly retreived.
//!		- \a 0xFF if not.
//! \return
//!		- \ref TRUE if correctly retreived.
//!		- \ref FALSE if an error occured.

static int __Cless_PayPass_RetreiveErrorIndicationByte (T_SHARED_DATA_STRUCT * pResultDataStruct, const unsigned int nByteNumber, unsigned char * pErrorIndicationByte)
{
	int nResult = TRUE;
	int nPosition, cr;
	unsigned long ulReadLength;
	unsigned char * pReadValue;

	unsigned long ulEiReadLength;
	const unsigned char * pEiReadValue;
	T_SHARED_DATA_STRUCT tDiscretionaryData;

	// Init position
	nPosition = SHARED_EXCHANGE_POSITION_NULL;

	// Init output data
	if (pErrorIndicationByte != NULL)
		*pErrorIndicationByte = 0xFF;

	if (nByteNumber > PAYPASS_EI_MSG_ON_ERROR_BYTE) {
		GTL_Traces_TraceDebug("__Cless_PayPass_RetreiveErrorIndicationByte : Invalid nBytesNumber = %02X", nByteNumber);
		nResult = FALSE;
		goto End;
	}

	// Get the Outcome Parameter Set
	cr = GTL_SharedExchange_FindNext(pResultDataStruct, &nPosition, TAG_PAYPASS_DISCRETIONARY_DATA, &ulReadLength, (const unsigned char **)&pReadValue);

	if (cr != STATUS_SHARED_EXCHANGE_OK) {
		GTL_Traces_TraceDebug("__Cless_PayPass_RetreiveErrorIndicationByte : Unable to get the Discretionary data set from the C2 kernel response (cr = %02x)", cr);
		nResult = FALSE;
		goto End;
	}

	// Init the shared buffer with the discretionary data buffer
	cr = GTL_SharedExchange_InitEx (&tDiscretionaryData, ulReadLength, ulReadLength, pReadValue);

	if (cr != STATUS_SHARED_EXCHANGE_OK) {
		GTL_Traces_TraceDebug("__Cless_PayPass_RetreiveErrorIndicationByte : Unable to initialise the shared buffer with the Discretionary data set from the C2 kernel response (cr = %02x)", cr);
		nResult = FALSE;
		goto End;
	}

	// Init position
	nPosition = SHARED_EXCHANGE_POSITION_NULL;

	cr = GTL_SharedExchange_FindNext (&tDiscretionaryData, &nPosition, TAG_PAYPASS_ERROR_INDICATION, &ulEiReadLength, &pEiReadValue);

	if (cr != STATUS_SHARED_EXCHANGE_OK) {
		GTL_Traces_TraceDebug("__Cless_PayPass_RetreiveErrorIndicationByte : Unable to find the error indication in the Discretionary data set from the C2 kernel response (cr = %02x)", cr);
		nResult = FALSE;
		goto End;
	}


	// Get the byte number from the EI
	if (pErrorIndicationByte != NULL)
		*pErrorIndicationByte = pEiReadValue[nByteNumber];

	End:
	return (nResult);
}



//! \brief Get the PayPass message to be displayed from the UIRD.
//! \param[in] pResultDataStruct Structure containing the C2 kernel output.
//! \param[out] pUirdMessage Retreived transaction outcome :
//!		- \a PAYPASS_UIRD_MESSAGE_ID_CARD_READ_OK Indicates the card has been read.
//!		- \a PAYPASS_UIRD_MESSAGE_ID_TRY_AGAIN Indicates the card has to be presented again.
//!		- \a PAYPASS_UIRD_MESSAGE_ID_APPROVED Indicates the transaction is approved.
//!		- \a PAYPASS_UIRD_MESSAGE_ID_APPROVED_SIGN Indicates the transaction is approved but signature required.
//!		- \a PAYPASS_UIRD_MESSAGE_ID_DECLINED Indicates the transaction is declined.
//!		- \a PAYPASS_UIRD_MESSAGE_ID_ERROR_OTHER_CARD Indicates the an error occured and the cardholder shall use another card.
//!		- \a PAYPASS_UIRD_MESSAGE_ID_SEE_PHONE Indicates the cardholder shall see his phone for instructions.
//!		- \a PAYPASS_UIRD_MESSAGE_ID_CLEAR_DISPLAY Indicates the display shall be cleared.
//!		- \a PAYPASS_UIRD_MESSAGE_ID_NA Message identifier is not applicable.
//! \return
//!		- \ref TRUE if correctly retreived.
//!		- \ref FALSE if an error occured.

static int __Cless_PayPass_RetreiveUirdMessage (T_SHARED_DATA_STRUCT * pResultDataStruct, int * pUirdMessage)
{
	int nResult = TRUE;
	int nPosition, cr;
	unsigned long ulReadLength;
	const unsigned char * pReadValue;

	// Init position
	nPosition = SHARED_EXCHANGE_POSITION_NULL;

	// Init output data
	if (pUirdMessage != NULL)
		*pUirdMessage = PAYPASS_UIRD_MESSAGE_ID_NA;

	// Get the Outcome Parameter Set
	cr = GTL_SharedExchange_FindNext(pResultDataStruct, &nPosition, TAG_PAYPASS_USER_INTERFACE_REQUEST_DATA, &ulReadLength, &pReadValue);

	if (cr != STATUS_SHARED_EXCHANGE_OK)
	{
		GTL_Traces_TraceDebug("__Cless_PayPass_RetreiveUirdMessage : Unable to get the UIRD from the C2 kernel response (cr = %02x)", cr);
		nResult = FALSE;
		goto End;
	}

	// Get the UIRD message
	if (pUirdMessage != NULL)
		*pUirdMessage = pReadValue[PAYPASS_UIRD_MESSAGE_ID_BYTE];

	End:
	return (nResult);
}



//! \brief Get the CVM to apply (read from the Outcome Parameter Set).
//! \param[in] pResultDataStruct Structure containing the C2 kernel output.
//! \param[out] pCvm Retreived transaction outcome :
//!		- \a PAYPASS_OPS_CVM_NO_CVM No CVM to be performed.
//!		- \a PAYPASS_OPS_CVM_SIGNATURE if signature shall be performed.
//!		- \a PAYPASS_OPS_CVM_ONLINE_PIN if online PIN shall be performed.
//!		- \a PAYPASS_OPS_CVM_CONFIRMATION_CODE_VERIFIED if if confirmation code has been verified.
//!		- \a PAYPASS_OPS_CVM_NA if CVM is not applicable to the case.
//! \return
//!		- \ref TRUE if correctly retreived.
//!		- \ref FALSE if an error occured.

static int __Cless_PayPass_RetreiveCvmToApply (T_SHARED_DATA_STRUCT * pResultDataStruct, unsigned char * pCvm)
{
	int nResult = TRUE;
	int nPosition, cr;
	unsigned long ulReadLength;
	const unsigned char * pReadValue;

	// Init position
	nPosition = SHARED_EXCHANGE_POSITION_NULL;

	// Init output data
	if (pCvm != NULL)
		*pCvm = PAYPASS_OPS_CVM_NA;

	// Get the Outcome Parameter Set
	cr = GTL_SharedExchange_FindNext(pResultDataStruct, &nPosition, TAG_PAYPASS_OUTCOME_PARAMETER_SET, &ulReadLength, &pReadValue);

	if (cr != STATUS_SHARED_EXCHANGE_OK)
	{
		GTL_Traces_TraceDebug("__Cless_PayPass_RetreiveCvmToApply : Unable to get the outcome parameter set from the C2 kernel response (cr = %02x)", cr);
		nResult = FALSE;
		goto End;
	}

	// Get the CVM to apply
	if (pCvm != NULL)
		*pCvm = pReadValue[PAYPASS_OPS_CVM_BYTE];

	End:
	return (nResult);
}



//! \brief Get the Field Off value (read from the Outcome Parameter Set).
//! \param[in] pResultDataStruct Structure containing the C2 kernel output.
//! \param[out] pFieldOff Retreived field off value (in units of 100 ms).
//!		- \a PAYPASS_OPS_FIELD_OFF_REQUEST_NA if Field Off is not applicable to the case.
//! \return
//!		- \ref TRUE if correctly retreived.
//!		- \ref FALSE if an error occured.

static int __Cless_PayPass_RetreiveFieldOffValue (T_SHARED_DATA_STRUCT * pResultDataStruct, unsigned char * pFieldOff)
{
	int nResult = TRUE;
	int nPosition, cr;
	unsigned long ulReadLength;
	const unsigned char * pReadValue;

	// Init position
	nPosition = SHARED_EXCHANGE_POSITION_NULL;

	// Init output data
	if (pFieldOff != NULL)
		*pFieldOff = PAYPASS_OPS_FIELD_OFF_REQUEST_NA;

	// Get the Outcome Parameter Set
	cr = GTL_SharedExchange_FindNext(pResultDataStruct, &nPosition, TAG_PAYPASS_OUTCOME_PARAMETER_SET, &ulReadLength, &pReadValue);

	if (cr != STATUS_SHARED_EXCHANGE_OK)
	{
		GTL_Traces_TraceDebug("__Cless_PayPass_RetreiveFieldOffValue : Unable to get the outcome parameter set from the C2 kernel response (cr = %02x)", cr);
		nResult = FALSE;
		goto End;
	}

	// Get the Field Off value
	if (pFieldOff != NULL)
		*pFieldOff = pReadValue[PAYPASS_OPS_FIELD_OFF_REQUEST_BYTE];

	End:
	return (nResult);
}



//! \brief Get the card type.
//! \param[in] pResultDataStruct Structure containing the C2 kernel output.
//! \param[out] pCardType Retreived card type
//!		- \a 0 If card type not found.
//!		- \a 0x8501 for MStripe card.
//!		- \a 0x8502 for MChip card.
//! \return
//!		- \ref TRUE if correctly retreived.
//!		- \ref FALSE if an error occured.

static int __Cless_PayPass_RetreiveCardType (T_SHARED_DATA_STRUCT * pResultDataStruct, unsigned short * pCardType)
{
	int nResult = TRUE;
	int nPosition, cr;
	unsigned long ulReadLength;
	const unsigned char * pReadValue;

	nPosition = SHARED_EXCHANGE_POSITION_NULL;

	if (pCardType != NULL)
		*pCardType = 0; // Default result

	cr = GTL_SharedExchange_FindNext(pResultDataStruct, &nPosition, TAG_KERNEL_CARD_TYPE, &ulReadLength, &pReadValue);

	if (cr != STATUS_SHARED_EXCHANGE_OK)
	{
		nResult = FALSE;
		goto End;
	}

	// Get the transaction outcome
	if (pCardType != NULL)
		*pCardType = (pReadValue[0] << 8) + pReadValue[1];

	End:
	return (nResult);
}



//! \brief Fill buffer with specific PayPass for transaction.
//! \param[out] pDataStruct Shared exchange structure filled with the specific PayPass data.
//! \return
//!		- \ref TRUE if correctly performed.
//!		- \ref FALSE if an error occured.

static int __Cless_PayPass_AddPayPassSpecificData (T_SHARED_DATA_STRUCT * pDataStruct)
{
	int cr, nResult;
	object_info_t ObjectInfo;
	unsigned char bMerchantForcedTrOnline = 0x01;
	unsigned char ucTornRecordNumber;
	T_KERNEL_TRANSACTION_FLOW_CUSTOM sTransactionFlowCustom;
	T_PAYPASS_DATA_EXCHANGE_CUSTOM sDataExchangeCustom;
	///const T_PAYMENT_DATA_STRUCT * pTransactionData;
	unsigned char StepInterruption[KERNEL_PAYMENT_FLOW_STOP_LENGTH];// Bit field to stop payment flow,
	// if all bit set to 0 => no stop during payment process
	// if right bit set to 1 : stop after payment step number 1
	unsigned char StepCustom[KERNEL_PAYMENT_FLOW_CUSTOM_LENGTH]; 	// Bit field to custom payment flow,
	// if all bit set to 0 => no stop during payment process
	// if right bit set to 1 : stop after payment step number 1
	int nPosition;
	T_TI_LENGTH ReadLength;
	T_TI_VALUE ReadValue;
	unsigned char ucTransactionType;
	char MENU[lenMnu + 1];
	card mnuitem = 0;
	int ret = 0;


	// Check the input data are correctly provided
	if (pDataStruct == NULL) {
		GTL_Traces_TraceDebug("__Cless_PayPass_AddPayPassSpecificData : Invalid Input data");
		nResult = FALSE;
		goto End;
	}


	// Init parameteters
	memset(StepInterruption, 0, sizeof(StepInterruption)); // Default Value : not stop on process
	memset(StepCustom, 0, sizeof(StepCustom)); // Default Value : not stop on process
	ObjectGetInfo(OBJECT_TYPE_APPLI, ApplicationGetCurrent(), &ObjectInfo);
	nResult = TRUE;

	// Add a tag indicating where the transaction has to be stopped (default value, i.e. all bytes set to 0, is strongly recommanded)
	cr = GTL_SharedExchange_AddTag(pDataStruct, TAG_KERNEL_PAYMENT_FLOW_STOP, KERNEL_PAYMENT_FLOW_STOP_LENGTH, (const unsigned char *)StepInterruption);
	if (cr != STATUS_SHARED_EXCHANGE_OK) {
		GTL_Traces_TraceDebug("__Cless_PayPass_AddPayPassSpecificData : Unable to add TAG_KERNEL_PAYMENT_FLOW_STOP in shared buffer (cr=%02x)", cr);
		nResult = FALSE;
		goto End;
	}


	// Customize steps
	ADD_STEP_CUSTOM(STEP_PAYPASS_MSTRIPE_REMOVE_CARD,StepCustom); // To do GUI when MStripe card has been read
	ADD_STEP_CUSTOM(STEP_PAYPASS_MCHIP_REMOVE_CARD,StepCustom); // To do GUI when MChip card has been read
	ADD_STEP_CUSTOM(STEP_PAYPASS_MCHIP_GET_CERTIFICATE, StepCustom); // To provide the CA key data for ODA
	ADD_STEP_CUSTOM(STEP_PAYPASS_MCHIP_SEND_PHONE_MSG, StepCustom);		// To do GUI when MChip phone has been read 
	ADD_STEP_CUSTOM(STEP_PAYPASS_MSTRIPE_SEND_PHONE_MSG, StepCustom);	// To do GUI when MStripe phone has been read 
	ADD_STEP_CUSTOM(STEP_PAYPASS_CANCEL_DE_PROCESSING, StepCustom); // To allow the application to cancel outstanding DE processing

	MAPGET(traMnuItm, MENU, lblSKIP);
	dec2num(&mnuitem, MENU,0);
	if (mnuitem == mnuRefund) { //Only for the terminal transaction refund to kill the transaction
		ADD_STEP_CUSTOM(STEP_PAYPASS_MCHIP_TERMINAL_ACTION_ANALYSIS, StepCustom);
	}
	lblSKIP:
	//	ADD_STEP_CUSTOM(STEP_PAYPASS_MSTRIPE_COMPUTE_CRYPTO_CHECKSUM, StepCustom); //Testing CVC3 calculation by Kev

	if (Cless_IsBlackListPresent())
		ADD_STEP_CUSTOM(STEP_PAYPASS_MCHIP_EXCEPTION_FILE_GET_DATA, StepCustom); // To check if PAN is in the blacklist

	if(Cless_Torn_IsRecoverySupported()) {
		ADD_STEP_CUSTOM(STEP_PAYPASS_MCHIP_IS_TORN_RECORD, StepCustom);		// To check if transaction is in the torn transaction log
		ADD_STEP_CUSTOM(STEP_PAYPASS_MCHIP_ADD_TORN_RECORD, StepCustom);	// To add a transaction in the torn transaction log
		ADD_STEP_CUSTOM(STEP_PAYPASS_MCHIP_REMOVE_TORN_RECORD, StepCustom);	// To remove a transaction from the torn transaction log

		// Search in the shared buffer, the maximum number of records that could be stored in the torn transaction log and replace it with the maximum authorized value if exceeded.
		nPosition = SHARED_EXCHANGE_POSITION_NULL;
		cr = GTL_SharedExchange_FindNext (pDataStruct, &nPosition, TAG_PAYPASS_MAX_NUMBER_OF_TORN_TXN_LOG_RECORDS, &ReadLength, (const unsigned char **)&ReadValue);
		if (cr == STATUS_SHARED_EXCHANGE_OK) {
			ReadValue[0] = Cless_Torn_GetMaxNumberOfTornTxnLogRecords();
		}

		// Add data indicating the number of records in the torn transaction log.
		ucTornRecordNumber = Cless_Torn_GetNumberOfTornTxnLogRecords();
		if (ucTornRecordNumber > 0) {
			cr = GTL_SharedExchange_AddTag(pDataStruct, TAG_PAYPASS_NUMBER_OF_TORN_TXN_LOG_RECORDS, 1, &ucTornRecordNumber);

			if (cr != STATUS_SHARED_EXCHANGE_OK) {
				GTL_Traces_TraceDebug("__Cless_PayPass_AddPayPassSpecificData : Unable to add TAG_PAYPASS_NUMBER_OF_TORN_TXN_LOG_RECORDS in shared buffer (cr=%02x)", cr);
				nResult = FALSE;
				goto End;
			}
		}
	}

	memcpy ((void*)&sTransactionFlowCustom, (void*)StepCustom, KERNEL_PAYMENT_FLOW_CUSTOM_LENGTH);
	sTransactionFlowCustom.usApplicationType = ObjectInfo.application_type; // Kernel will call this application for customisation 
	sTransactionFlowCustom.usServiceId = SERVICE_CUSTOM_KERNEL; // Kernel will call SERVICE_CUSTOM_KERNEL service id for customisation 

	cr = GTL_SharedExchange_AddTag(pDataStruct, TAG_KERNEL_PAYMENT_FLOW_CUSTOM, sizeof(T_KERNEL_TRANSACTION_FLOW_CUSTOM), (const unsigned char *)&sTransactionFlowCustom);
	if (cr != STATUS_SHARED_EXCHANGE_OK)
	{
		GTL_Traces_TraceDebug("__Cless_PayPass_AddPayPassSpecificData : Unable to add TAG_KERNEL_PAYMENT_FLOW_CUSTOM in shared buffer (cr=%02x)", cr);
		nResult = FALSE;
		goto End;
	}

	// Add the Data Exchange Custom call service id for DEK and DET signals
	sDataExchangeCustom.usApplicationType = ObjectInfo.application_type; // Kernel will call this application for customisation 
	sDataExchangeCustom.usServiceId = SERVICE_DE_KERNEL; // Kernel will call SERVICE_DE_KERNEL service id for data exchange processing 

	cr = GTL_SharedExchange_AddTag(pDataStruct, TAG_PAYPASS_DATA_EXCHANGE_CUSTOM, sizeof(T_PAYPASS_DATA_EXCHANGE_CUSTOM), (const unsigned char *)&sDataExchangeCustom);
	if (cr != STATUS_SHARED_EXCHANGE_OK) {
		GTL_Traces_TraceDebug("__Cless_PayPass_AddPayPassSpecificData : Unable to add TAG_PAYPASS_DATA_EXCHANGE_CUSTOM in shared buffer (cr=%02x)", cr);
		nResult = FALSE;
		goto End;
	}

	// Add data indicating if merchant forced transaction online or not
	if (Cless_Menu_IsMerchantForcedOnline()) {
		cr = GTL_SharedExchange_AddTag(pDataStruct, TAG_PAYPASS_INT_MERCHANT_FORCE_ONLINE, 1, &bMerchantForcedTrOnline);

		if (cr != STATUS_SHARED_EXCHANGE_OK) {
			GTL_Traces_TraceDebug("__Cless_PayPass_AddPayPassSpecificData : Unable to add TAG_PAYPASS_INT_MERCHANT_FORCE_ONLINE in shared buffer (cr=%02x)", cr);
			nResult = FALSE;
			goto End;
		}
	}

	// Change the internal transaction type to EMVLIB_TRANSACTION_TYPE_PURCHASE_WITH_CASHBACK if transaction with caskback is performed and amount other is different from 0
	///pTransactionData = Cless_Fill_GetTransactionDataStructure ();

	///if (pTransactionData != NULL)
	///{
	if (CS_GlobalTransactionData_GetTransactionType(&ucTransactionType)) {
		// View from PayPass kernel side, Manual Cash transactions are viewed as a Cash transaction
		///if ((pTransactionData->bIsTransacTypePresent == SAMPLE_TAG_PRESENT) &&
		///	(pTransactionData->ucTransactionType == CLESS_SAMPLE_TRANSACTION_TYPE_MANUAL_CASH))
		if ((ucTransactionType == CLESS_SAMPLE_TRANSACTION_TYPE_MANUAL_CASH) || (ucTransactionType == CLESS_SAMPLE_TRANSACTION_TYPE_CASH_DISBURSEMENT)) {
			// Search and replace internal transaction type in the shared buffer
			nPosition = SHARED_EXCHANGE_POSITION_NULL;

			cr = GTL_SharedExchange_FindNext (pDataStruct, &nPosition, TAG_EMV_INT_TRANSACTION_TYPE, &ReadLength, (const unsigned char **)&ReadValue);

			if (cr == STATUS_SHARED_EXCHANGE_OK){
				ReadValue[0] = CLESS_SAMPLE_TRANSACTION_TYPE_CASH;
			}
		}
	}
	///}

	End:
	return (nResult);
}



//! \brief Perform the Online PIN input and encipher PIN.
//! \param[out] pStructureForOnlineData Data returned by the kernel in which the enciphered online PIN would be added.
//! \return
//!		- \ref TRUE if correctly performed.
//!		- \ref FALSE if an error occured.

static T_Bool __Cless_PayPass_OnlinePinManagement (T_SHARED_DATA_STRUCT * pStructureForOnlineData, int nCardHolderLang) {
	//T_SHARED_DATA_STRUCT * pDataRequest;
	T_Bool nResult = B_TRUE;
	int nPosition, cr;
	const unsigned char * pPan;
	unsigned long ulPanLength;
	const unsigned char * pTrack2Data;
	unsigned long ulTrack2DataLength;
	char aucDymmyMsg[] = "";
	BUFFER_SAISIE buffer_saisie;
	MSGinfos tMsg;
	unsigned long ulReadDataRecordLength;
	unsigned char * pReadDataRecordValue;
	T_SHARED_DATA_STRUCT tDataRecord;
	char TempAmount[lenAmt + 1];
	int Len = 0;
	char Temp[256];
	unsigned long ulAmount;

	// Get amount and copy all the data in the structure to be sent to the HOST
	memset(TempAmount, 0, sizeof(TempAmount));
	mapGet(traAmt, TempAmount, lenAmt);
	EPSTOOL_Convert_AsciiToUl(TempAmount, -1, &ulAmount);

	// Get the DATA RECORD and copy all the data in the structure to be sent to the HOST
	nPosition = SHARED_EXCHANGE_POSITION_NULL;
	cr = GTL_SharedExchange_FindNext (pStructureForOnlineData, &nPosition, TAG_PAYPASS_DATA_RECORD, &ulReadDataRecordLength, (const unsigned char **)&pReadDataRecordValue);

	if (cr == STATUS_SHARED_EXCHANGE_OK) {
		if (GTL_SharedExchange_InitEx (&tDataRecord, ulReadDataRecordLength, ulReadDataRecordLength, pReadDataRecordValue) != STATUS_SHARED_EXCHANGE_OK) {
			GTL_Traces_TraceDebug ("__Cless_PayPass_OnlinePinManagement : Unable to init the DATA RECORD structure");
			nResult = B_FALSE;
			goto End;
		}
	}


	// Tags have been got (if present), get the PAN
	nPosition = SHARED_EXCHANGE_POSITION_NULL;
	if (GTL_SharedExchange_FindNext (&tDataRecord, &nPosition, TAG_EMV_APPLI_PAN, &ulPanLength, &pPan) != STATUS_SHARED_EXCHANGE_OK) {
		// Maybe it is a MStripe card, so try to extract PAN from Track2 Data
		nPosition = SHARED_EXCHANGE_POSITION_NULL;

		if (GTL_SharedExchange_FindNext (&tDataRecord, &nPosition, TAG_PAYPASS_TRACK2_DATA, &ulTrack2DataLength, &pTrack2Data) != STATUS_SHARED_EXCHANGE_OK) {
			GTL_Traces_TraceDebug ("__Cless_PayPass_OnlinePinManagement : Missing PAN for Online PIN");
			nResult = B_FALSE;
			goto End;
		} else {
			// Continue with the PAN extracted from the track2 data
			pPan = pTrack2Data;

			Len = (int)ulTrack2DataLength;
			memset(Temp, 0, sizeof(Temp));
			bin2hex(Temp, pPan, Len);
			mapPut(traPan, &Temp[0], lenPan);
		}
	}else{
		Len = (int)ulPanLength;
		memset(Temp, 0, sizeof(Temp));
		bin2hex(Temp, pPan, Len);
		mapPut(traPan, &Temp[0], Len*2);
	}

	// Warning, erase display must be made only if Pin input will be made on customer screen
	if (Cless_IsPinpadPresent()) {
		// Display dummy message to erase display
		tMsg.message = aucDymmyMsg;
		tMsg.coding = _ISO8859_;
		tMsg.file = GetCurrentFont();

		Helper_DisplayTextCustomer(ERASE, HELPERS_CUSTOMER_LINE_1, &tMsg, Cless_ALIGN_CENTER, LEDSOFF);
	}

	// Request online PIN entry
	cr = Cless_PinManagement_OnLinePinManagement ((unsigned char*)pPan, 1, ulAmount, 30000, 10000, nCardHolderLang, &buffer_saisie);
	//	cr = Cless_PinManagement_OnLinePinManagement ((unsigned char*)pPan, 1, 0, 30000, 10000, nCardHolderLang, &buffer_saisie);
	if (cr == INPUT_PIN_ON) {
		cr = GTL_SharedExchange_AddTag(pStructureForOnlineData, TAG_SAMPLE_ENCIPHERED_PIN_CODE, buffer_saisie.nombre , (const unsigned char *)buffer_saisie.donnees);

		if (cr != STATUS_SHARED_EXCHANGE_OK) {
			GTL_Traces_TraceDebug ("__Cless_PayPass_OnlinePinManagement : Unable to add TAG_SAMPLE_ENCIPHERED_PIN_CODE in the shared buffer (cr = %02x)", cr);
			nResult = B_FALSE;
			goto End;
		}
	} else if (cr == CANCEL_INPUT) {
		GTL_Traces_TraceDebug ("__Cless_PayPass_OnlinePinManagement : input pin Cancelled");
		nResult = B_NON_INIT;
		goto End;
	}

	End:
	return (nResult);
}



//! \brief Manage the debug mode for PayPass kernel
//! \param[in] bActivate \a TRUE to activate the debug features. \a FALSE to deactivate features.

void Cless_PayPass_DebugActivation (int bActivate) {
	T_SHARED_DATA_STRUCT * pSharedStructure;
	int nResult;
	unsigned char ucDebugMode = 0x00;

	if (bActivate)
		ucDebugMode = KERNEL_DEBUG_MASK_TRACES|KERNEL_DEBUG_MASK_APDU|PAYPASS_DEBUG_MASK_DE_LOG;

	// Init the shared buffer
	pSharedStructure = GTL_SharedExchange_InitShared(256);

	// Check the structure correctly created
	if (pSharedStructure != NULL) {
		// Add the debug information into the structure
		nResult = GTL_SharedExchange_AddTag(pSharedStructure, TAG_KERNEL_DEBUG_ACTIVATION, 1, &ucDebugMode);

		// Check if data correctly added
		if (nResult != STATUS_SHARED_EXCHANGE_OK) {
			// An error occurred when adding the tag in the structure
			GTL_Traces_TraceDebug("Cless_PayPass_DebugActivation : Unable to add TAG_KERNEL_DEBUG_ACTIVATION (nResult = %02x)", nResult);
		} else {
			// Data correctly added, call the kernel to activate the debug feature
			nResult = PayPass3_DebugManagement(pSharedStructure);

			// Check if debug activation correctly performed
			if (nResult != KERNEL_STATUS_OK) {
				// An error occurred when enabling the debug feature in the kernel
				GTL_Traces_TraceDebug("Cless_PayPass_DebugActivation : Error occured during PayPass Debug activation (nResult = %02x)", nResult);
			}
		}

		// Destroy the shared buffer
		GTL_SharedExchange_DestroyShare(pSharedStructure);
	}
}



//! \brief Display a specific message according to the UIRD.
//! \param[in] pDataStruct Shared buffer contianing the UIRD, containing itself the message to be displayed.
//! \param[in] bPhoneMessage \a TRUE if function called when displaying phone message, \a FALSE else.

static void __Cless_PayPass_DisplayUirdMsg (T_SHARED_DATA_STRUCT * pDataStruct, const int bPhoneMessage)
{
	//int auCustomerDisplayAvailable;
	int nMerchantLanguage, nCardHolderLanguage;
	unsigned char * pInfo;
	int nUirdMessage;

	if (bPhoneMessage) {
		UIH_RemoveCardTask_WaitEnd();
		HelperErrorSequence(WITHBEEP);
	}

	// Init languages
	nMerchantLanguage = PSQ_Give_Language();

	if (Cless_Common_RetrieveInfo (pDataStruct, TAG_KERNEL_SELECTED_PREFERED_LANGUAGE, &pInfo))
		nCardHolderLanguage = Cless_Term_GiveLangNumber(pInfo);
	else
		nCardHolderLanguage = nMerchantLanguage;

	// Check if a cardholder device is available
	//auCustomerDisplayAvailable = Helper_IsClessCustomerDisplayAvailable();

	// Get the UIRD message to be displayed
	if (!__Cless_PayPass_RetreiveUirdMessage (pDataStruct, &nUirdMessage))
		nUirdMessage = PAYPASS_UIRD_MESSAGE_ID_NA;

	switch (nUirdMessage)
	{
	case PAYPASS_UIRD_MESSAGE_ID_CARD_READ_OK:
		Cless_GuiState_DisplayScreenText (CLESS_SAMPLE_SCREEN_REMOVE_CARD, nMerchantLanguage, nCardHolderLanguage);
		break;
	case PAYPASS_UIRD_MESSAGE_ID_TRY_AGAIN:
		Cless_GuiState_DisplayScreenText (CLESS_SAMPLE_SCREEN_RETRY, nMerchantLanguage, nCardHolderLanguage);
		break;
	case PAYPASS_UIRD_MESSAGE_ID_APPROVED:
		Cless_GuiState_DisplayScreenText (CLESS_SAMPLE_PAYPASS_SCREEN_APPROVED, nMerchantLanguage, nCardHolderLanguage);
		break;
	case PAYPASS_UIRD_MESSAGE_ID_APPROVED_SIGN:
		Cless_GuiState_DisplayScreenText (CLESS_SAMPLE_PAYPASS_SCREEN_SIGNATURE_OK, nMerchantLanguage, nCardHolderLanguage);
		break;
	case PAYPASS_UIRD_MESSAGE_ID_DECLINED:
		Cless_GuiState_DisplayScreenText (CLESS_SAMPLE_PAYPASS_SCREEN_DECLINED, nMerchantLanguage, nCardHolderLanguage);
		break;
	case PAYPASS_UIRD_MESSAGE_ID_ERROR_OTHER_CARD:
		Cless_GuiState_DisplayScreenText (CLESS_SAMPLE_SCREEN_ERROR, nMerchantLanguage, nCardHolderLanguage);
		break;
	case PAYPASS_UIRD_MESSAGE_ID_SEE_PHONE:	
		// Request cardholder to follow his phone for instructions
		Cless_GuiState_DisplayScreenText (CLESS_SAMPLE_SCREEN_PHONE_INSTRUCTIONS, nMerchantLanguage, nCardHolderLanguage);

		if (bPhoneMessage) {
			// Indicate double tap is in progress
			Cless_Customisation_SetDoubleTapInProgress (TRUE);
		}
		break;
	case PAYPASS_UIRD_MESSAGE_ID_AUTHORISING_PLEASE_WAIT:
		Cless_GuiState_DisplayScreenText (CLESS_SAMPLE_PAYPASS_SCREEN_AUTHORISING, nMerchantLanguage, nCardHolderLanguage);
		break;
	case PAYPASS_UIRD_MESSAGE_ID_INSERT_CARD:
		Cless_GuiState_DisplayScreenText (CLESS_SAMPLE_SCREEN_USE_CONTACT, nMerchantLanguage, nCardHolderLanguage);
		break;
	case PAYPASS_UIRD_MESSAGE_ID_CLEAR_DISPLAY:
		UIH_ClearScreen();
		break;
	default:
		break;
	}
}



//! \brief Perform the C2 kernel customisation.
//! \param[in,out] pSharedData Shared buffer used for customisation.
//! \param[in] ucCustomisationStep Step to be customised.
//! \return
//!		- \a KERNEL_STATUS_CONTINUE always.

int Cless_PayPass_CustomiseStep (T_SHARED_DATA_STRUCT * pSharedData, const unsigned char ucCustomisationStep)
{
	int nResult = KERNEL_STATUS_CONTINUE;
	unsigned char ucCapkIndex;
	unsigned char ucRid[5];
	unsigned long ulReadLength;
	int nPosition;
	const unsigned char * pReadValue;
	const unsigned char * pPan;
	unsigned long ulPanLength;
	const unsigned char * pPanSeqNumber;
	unsigned char ucVoidPanSeqNumber = C_CLESS_VOID_PAN_SEQ_NUMBER; // Unused value for PanSeqNumber
	unsigned long ulPanSeqNbLength;
	unsigned char bPanInExceptionFile = FALSE;
	unsigned char ucTmp = 0x00;

	char AID_selected[lenAID  + 1];


	switch (ucCustomisationStep) // Steps to customise
	{
	case STEP_PAYPASS_MSTRIPE_REMOVE_CARD:
	case STEP_PAYPASS_MCHIP_REMOVE_CARD:
		perflog_process("MG\tPP_CUST_PROC\tSTEP_PAYPASS_XXX_REMOVE_CARD");
		perflog("MG\tPP_CUST\tSTEP_PAYPASS_XXX_REMOVE_CARD");
		// Indicate the card shall be removed
		UIH_RemoveCardTask_Launch();

		if (Cless_Menu_IsTransactionDataDumpingOn() && (!Cless_Menu_IsTransactionLoopOn()))
			Cless_PayPass_DumpSpecificStructure (pSharedData, TAG_PAYPASS_USER_INTERFACE_REQUEST_DATA, FALSE);

		// Display the message located in the UIRD, if available
		__Cless_PayPass_DisplayUirdMsg (pSharedData, FALSE);

		GTL_SharedExchange_ClearEx (pSharedData, FALSE);
		nResult = KERNEL_STATUS_CONTINUE;
		break;

	case STEP_PAYPASS_MCHIP_GET_CERTIFICATE:
		perflog("MG\tPP_CUST\tSTEP_PAYPASS_MCHIP_GET_CERTIFICATE");
		// Init RID value
		memset (ucRid, 0, sizeof(ucRid));

		// Get the CA public key index (card)
		nPosition = SHARED_EXCHANGE_POSITION_NULL;
		if (GTL_SharedExchange_FindNext (pSharedData, &nPosition, TAG_EMV_CA_PUBLIC_KEY_INDEX_CARD, &ulReadLength, (const unsigned char **)&pReadValue) == STATUS_SHARED_EXCHANGE_OK)
			ucCapkIndex = pReadValue[0];
		else
			ucCapkIndex = 0;

		// Get the DF Name returned by the card
		nPosition = SHARED_EXCHANGE_POSITION_NULL;
		if (GTL_SharedExchange_FindNext (pSharedData, &nPosition, TAG_PAYPASS_INT_RID, &ulReadLength, (const unsigned char **)&pReadValue) == STATUS_SHARED_EXCHANGE_OK)
			memcpy (ucRid, pReadValue, 5);
		else {
			memset(AID_selected, 0, sizeof(AID_selected));
			mapGet(traAID, AID_selected, lenAID);
			hex2bin((byte *)pReadValue, AID_selected, strlen(AID_selected));
			memcpy (ucRid, pReadValue, 5);
		}

		// Clear the output structure
		GTL_SharedExchange_ClearEx (pSharedData, FALSE);

		// Get the CA public key data (Modulus, exponent, etc) in the parameters
		if (!Cless_Parameters_GetCaKeyData (pTreeCurrentParam, ucCapkIndex, ucRid, pSharedData)) {
			// An error occurred when retreiving the CA Public Key data in the parameters
			GTL_Traces_TraceDebug ("Cless_PayPass_CustomiseStep : Cless_Parameters_GetCaKeyData failed");
		}

		nResult = KERNEL_STATUS_CONTINUE;
		break;

	case STEP_PAYPASS_MCHIP_EXCEPTION_FILE_GET_DATA:
		perflog("MG\tPP_CUST\tSTEP_PAYPASS_MCHIP_EXCEPTION_FILE_GET_DATA");
		// Get the PAN
		nPosition = SHARED_EXCHANGE_POSITION_NULL;
		if (GTL_SharedExchange_FindNext (pSharedData, &nPosition, TAG_EMV_APPLI_PAN, &ulPanLength, &pPan) != STATUS_SHARED_EXCHANGE_OK) {
			// Pan parameters is missing, we cannot check BlackList
			GTL_Traces_TraceDebug ("Cless_PayPass_CustomiseStep : PAN is missing for excpetion file checking");
			break;
		}

		// Get the PAN Sequence Number
		nPosition = SHARED_EXCHANGE_POSITION_NULL;
		if (GTL_SharedExchange_FindNext (pSharedData, &nPosition, TAG_EMV_APPLI_PAN_SEQUENCE_NUMBER, &ulPanSeqNbLength, &pPanSeqNumber) != STATUS_SHARED_EXCHANGE_OK)
		{
			// Pan Sequence Number is missing, we will check BlackList without PanSeqNumber
			pPanSeqNumber = &ucVoidPanSeqNumber;
		}

		// Check if PAN is in the exception file
		bPanInExceptionFile = Cless_BlackListIsPan((int)ulPanLength, pPan, (int)(pPanSeqNumber[0]));

		GTL_SharedExchange_ClearEx (pSharedData, FALSE);

		if (bPanInExceptionFile)
		{
			// Add TAG_KERNEL_PAN_IN_BLACK_LIST tag in the exchange buffer to indicate PayPass kernel the PAN is in the black list
			if (GTL_SharedExchange_AddTag (pSharedData, TAG_KERNEL_PAN_IN_BLACK_LIST, 1, &bPanInExceptionFile) != STATUS_SHARED_EXCHANGE_OK)
			{
				GTL_SharedExchange_ClearEx (pSharedData, FALSE);
				GTL_Traces_TraceDebug ("Cless_PayPass_CustomiseStep : Unable to add TAG_KERNEL_PAN_IN_BLACK_LIST in the shared buffer");
			}
		}

		nResult = KERNEL_STATUS_CONTINUE;
		break;

	case STEP_PAYPASS_MCHIP_SEND_PHONE_MSG:
	case STEP_PAYPASS_MSTRIPE_SEND_PHONE_MSG:
		perflog("MG\tPP_CUST\tSTEP_PAYPASS_XXX_SEND_PHONE_MSG");
		if (Cless_Menu_IsTransactionDataDumpingOn() && (!Cless_Menu_IsTransactionLoopOn()))
			Cless_PayPass_DumpSpecificStructure (pSharedData, TAG_PAYPASS_USER_INTERFACE_REQUEST_DATA, FALSE);

		__Cless_PayPass_DisplayUirdMsg (pSharedData, TRUE);

		GTL_SharedExchange_ClearEx (pSharedData, FALSE);

		nResult = KERNEL_STATUS_CONTINUE;
		break;

	case STEP_PAYPASS_MCHIP_IS_TORN_RECORD:
		perflog("MG\tPP_CUST\tSTEP_PAYPASS_MCHIP_IS_TORN_RECORD");
		if(!Cless_Torn_IsTornTxn(pSharedData))
			GTL_SharedExchange_ClearEx (pSharedData, FALSE);

		nResult = KERNEL_STATUS_CONTINUE;
		break;

	case STEP_PAYPASS_MCHIP_ADD_TORN_RECORD:
		perflog("MG\tPP_CUST\tSTEP_PAYPASS_MCHIP_ADD_TORN_RECORD");
		if(!Cless_Torn_AddRecord(pSharedData))
			GTL_SharedExchange_ClearEx (pSharedData, FALSE);

		nResult = KERNEL_STATUS_CONTINUE;
		break;

	case STEP_PAYPASS_MCHIP_REMOVE_TORN_RECORD:
		perflog("MG\tPP_CUST\tSTEP_PAYPASS_MCHIP_REMOVE_TORN_RECORD");
		Cless_Torn_RemoveRecord();

		GTL_SharedExchange_ClearEx (pSharedData, FALSE);

		nResult = KERNEL_STATUS_CONTINUE;
		break;

	case STEP_PAYPASS_CANCEL_DE_PROCESSING:
		perflog("MG\tPP_CUST\tSTEP_PAYPASS_CANCEL_DE_PROCESSING");

		// Cancel Data Exchange processing
		GTL_Traces_TraceDebug ("Cless_PayPass_CustomiseStep : Cancel Data Exchange processing\n");
		Cless_PayPass_SetDataExchangeInProgress(FALSE);

		// Ensure DET processing is finished
		nResult = KERNEL_STATUS_CONTINUE;
		break;
	case STEP_PAYPASS_MCHIP_TERMINAL_ACTION_ANALYSIS:

		GTL_SharedExchange_ClearEx (pSharedData, FALSE);
		GTL_SharedExchange_AddTag (pSharedData, TAG_PAYPASS_INT_MCHIP_GENAC_REF_CTRL_PARAMETER, 1, &ucTmp);
		nResult = KERNEL_STATUS_CONTINUE;

		break;
		//	case STEP_PAYPASS_MSTRIPE_COMPUTE_CRYPTO_CHECKSUM:
		//
		//		GTL_SharedExchange_ClearEx (pSharedData, FALSE);
		//
		//		nPosition = SHARED_EXCHANGE_POSITION_NULL;
		//		if (GTL_SharedExchange_FindNext (pSharedData, &nPosition, TAG_PAYPASS_MSTRIPE_UNPREDICTABLE_NUMBER, &ulReadLength, (const unsigned char **)&pReadValue) == STATUS_SHARED_EXCHANGE_OK)
		//			GTL_SharedExchange_AddTag (pSharedData, TAG_PAYPASS_MSTRIPE_UNPREDICTABLE_NUMBER, 4, pReadValue);
		//
		//		nResult = KERNEL_STATUS_CONTINUE;
		//
		//		break;

	default:
		perflog("MG\tPP_CUST\tSTEP_PAYPASS_???????????????????????????");
		GTL_Traces_TraceDebug ("Cless_PayPass_CustomiseStep : Step to customise (unknown) = %02x\n", ucCustomisationStep);
		break;
	}

	return (nResult);
}



//! \brief Format a string with the transaction amount.
//! \param[in] ucFormat : the display format
//! \param[in] ucCurrency : transaction currency
//! \param[in] ucPosition : the position of the currency
//! \param[in] ulAmount : the transaction amount
//! \param[out] pFormattedAmountMessage : the formatted message to display

static void __Cless_PayPass_FormatAmount (unsigned char ucFormat, char *ucCurrencyLabel, unsigned char ucPosition, const unsigned char * pAmount, unsigned int nAmountLength, char *pFormattedAmountMessage)
{
	char str[64];
	char * ptr;
	int index;
	int nLgStr;

	(void)ucFormat;
	(void)ucPosition;

	if (pFormattedAmountMessage == NULL)
		return;

	if ((pAmount == NULL) || (nAmountLength == 0))
	{
		*pFormattedAmountMessage = 0; // String end...
		return;
	}

	ptr = str;
	GTL_Convert_DcbNumberToAscii((const unsigned char*)pAmount, ptr, nAmountLength, nAmountLength*2);

	index = 0;
	while ((ptr[index] == '0') && (index < 10))
		index++;
	ptr+=index;

	nLgStr = strlen(ptr);
	strcpy (pFormattedAmountMessage, ucCurrencyLabel);
	if (nLgStr > 2)
	{
		strcat (pFormattedAmountMessage, " ");
		strncat (pFormattedAmountMessage, ptr, nLgStr - 2);
		pFormattedAmountMessage[strlen(ucCurrencyLabel) + nLgStr-1] = 0;
		strcat (pFormattedAmountMessage, ",");
		strncat (pFormattedAmountMessage, &ptr[nLgStr-2], 2);
	}
	else
	{
		strcat (pFormattedAmountMessage, " 0,");
		strcat (pFormattedAmountMessage, ptr);
	}
}



//! \brief Display Balance amount (if available from UIRD).
//! \param[in] pResultDataStruct Structure containing the C2 kernel output.
//! \return
//!		- \ref TRUE if no problem occured.
//!		- \ref FALSE if an error occured.

static int __Cless_PayPass_DisplayBalanceAmount(T_SHARED_DATA_STRUCT * pResultDataStruct)
{
	int nResult = TRUE;
	int nPosition, cr;
	unsigned long ulReadLength;
	const unsigned char * pReadValue;
	static char ucBalanceAmountMessage[64];
	char ucCurrencyLabel[4];
	unsigned char ucFormat, ucPosition;
	unsigned char * pCurrencyCode = NULL;
	char * pCurrencyLabel = NULL;
	MSGinfos tDisplayMsgL1, tDisplayMsgL2;

	// Init position
	nPosition = SHARED_EXCHANGE_POSITION_NULL;

	// Get the Outcome Parameter Set
	cr = GTL_SharedExchange_FindNext(pResultDataStruct, &nPosition, TAG_PAYPASS_USER_INTERFACE_REQUEST_DATA, &ulReadLength, &pReadValue);
	if (cr != STATUS_SHARED_EXCHANGE_OK) {
		GTL_Traces_TraceDebug("__Cless_PayPass_DisplayBalanceAmount : Unable to get the UIRD from the C2 kernel response (cr = %02x)", cr);
		nResult = FALSE;
		goto End;
	}

	// Get the Balance value and format the message  to display
	if(pReadValue[PAYPASS_UIRD_VALUE_QUALIFIER_BYTE] == PAYPASS_UIRD_VALUE_QUALIFIER_BALANCE) {
		pCurrencyCode = (unsigned char *) (&pReadValue[PAYPASS_UIRD_CURRENCY_CODE_OFFSET]);

		// Get the currency label and exponent from the parameters. If not found, indicate an invalid parameter as the currency code provided is unknown from the application.
		if(!Cless_Parameters_GetCurrencyFromParameters(pCurrencyCode, &pCurrencyLabel)) {
			GTL_Traces_TraceDebug ("__Cless_PayPass_DisplayBalanceAmount, Currency code provided is unknown from the application (%02x %02x)", pCurrencyCode[0], pCurrencyCode[1]);
			nResult = FALSE;
			goto End;
		}

		// Retrieve the format of the money (currency position, separator, ...)
		Cless_Parameters_GetCurrencyFormatFromParameters(pCurrencyLabel,&ucFormat,&ucPosition);

		memset(ucCurrencyLabel,0,sizeof(ucCurrencyLabel));
		memcpy(ucCurrencyLabel, pCurrencyLabel, 3);

		// Create a message that contains the balance amount, the currency ...
		memset (ucBalanceAmountMessage, 0, sizeof(ucBalanceAmountMessage));
		__Cless_PayPass_FormatAmount (ucFormat, ucCurrencyLabel, ucPosition, &(pReadValue[PAYPASS_UIRD_VALUE_OFFSET]), PAYPASS_UIRD_VALUE_LENGTH, ucBalanceAmountMessage);

		Cless_Term_Read_Message(STD_MESS_AVAILABLE, Cless_GuiState_GetCardholderLanguage(), &tDisplayMsgL1);

		tDisplayMsgL2.coding  = tDisplayMsgL1.coding;
		tDisplayMsgL2.file    = tDisplayMsgL1.file;
		tDisplayMsgL2.message = ucBalanceAmountMessage;

		// Display messages
		Helper_DisplayTextCustomer(NO_ERASE, HELPERS_CUSTOMER_LINE_2, &tDisplayMsgL1, Cless_ALIGN_CENTER, NOLEDSOFF);
		Helper_DisplayTextCustomer(NO_ERASE, HELPERS_CUSTOMER_LINE_3, &tDisplayMsgL2, Cless_ALIGN_CENTER, NOLEDSOFF);
	}

	End:
	return (nResult);
}



//! \brief Prepare the data to be sent in the online authorisation message.
//! \param[in] pTransactionData Kernel transaction data (including the Data Record as well as the Discretionary Data. It could also contain the Ciphered Online PIN.
//! \param[out] pOnlineAuthorisationData Filled with the data to be sent in the online authorisation message.
//! \return
//!	- \a TRUE if correctly performed.
//!	- \a FALSE if an error occurred.

static int __Cless_PayPass_FillBufferForOnlineAuthorisation (T_SHARED_DATA_STRUCT * pTransactionData, T_SHARED_DATA_STRUCT * pOnlineAuthorisationData)
{
	int nPosition;
	unsigned long ulReadLength;
	unsigned char * pReadValue;
	T_SHARED_DATA_STRUCT tTempStruct;
	int bErrorDetected = FALSE;


	// Check input data consistence
	if ((pTransactionData != NULL) && (pOnlineAuthorisationData != NULL)) {
		////////////////////////////////////////////////////////////////////////////////////////////////
		// Get the Data Record
		nPosition = SHARED_EXCHANGE_POSITION_NULL;
		if (GTL_SharedExchange_FindNext (pTransactionData, &nPosition, TAG_PAYPASS_DATA_RECORD, &ulReadLength, (const unsigned char **)&pReadValue) == STATUS_SHARED_EXCHANGE_OK) {
			// Copy data record data in the local buffer
			if (GTL_SharedExchange_InitEx (&tTempStruct, ulReadLength, ulReadLength, pReadValue) == STATUS_SHARED_EXCHANGE_OK) {
				if (GTL_SharedExchange_AddSharedBufferContent (pOnlineAuthorisationData, &tTempStruct) != STATUS_SHARED_EXCHANGE_OK) {
					GTL_Traces_TraceDebug ("__Cless_PayPass_FillBufferForOnlineAuthorisation : Unable to copy the local structure with data record");
					bErrorDetected = TRUE;
				}
			} else {
				GTL_Traces_TraceDebug ("__Cless_PayPass_FillBufferForOnlineAuthorisation : Unable to init the local structure with data record");
				bErrorDetected = TRUE;
			}
		} else {
			GTL_Traces_TraceDebug ("__Cless_PayPass_FillBufferForOnlineAuthorisation : Unable to retreive Data Record");
			bErrorDetected = TRUE;
		}

		////////////////////////////////////////////////////////////////////////////////////////////////
		// Get the Discretionary Data
		if (!bErrorDetected) {
			nPosition = SHARED_EXCHANGE_POSITION_NULL;
			if (GTL_SharedExchange_FindNext (pTransactionData, &nPosition, TAG_PAYPASS_DISCRETIONARY_DATA, &ulReadLength, (const unsigned char **)&pReadValue) == STATUS_SHARED_EXCHANGE_OK) {
				// Copy data record data in the local buffer
				if (GTL_SharedExchange_InitEx (&tTempStruct, ulReadLength, ulReadLength, pReadValue) == STATUS_SHARED_EXCHANGE_OK) {
					if (GTL_SharedExchange_AddSharedBufferContent (pOnlineAuthorisationData, &tTempStruct) != STATUS_SHARED_EXCHANGE_OK) {
						GTL_Traces_TraceDebug ("__Cless_PayPass_FillBufferForOnlineAuthorisation : Unable to copy the local structure with discretionary data");
						bErrorDetected = TRUE;
					}
				} else {
					GTL_Traces_TraceDebug ("__Cless_PayPass_FillBufferForOnlineAuthorisation : Unable to init the local structure with discretionary data");
					bErrorDetected = TRUE;
				}
			} else {
				GTL_Traces_TraceDebug ("__Cless_PayPass_FillBufferForOnlineAuthorisation : Unable to retreive discretionary Data");
				bErrorDetected = TRUE;
			}
		}

		////////////////////////////////////////////////////////////////////////////////////////////////
		// Get the ciphered PIN if present
		if (!bErrorDetected) {
			// Get the Discretionary Data
			nPosition = SHARED_EXCHANGE_POSITION_NULL;
			if (GTL_SharedExchange_FindNext (pTransactionData, &nPosition, TAG_SAMPLE_ENCIPHERED_PIN_CODE, &ulReadLength, (const unsigned char **)&pReadValue) == STATUS_SHARED_EXCHANGE_OK) {
				if (GTL_SharedExchange_AddTag (pOnlineAuthorisationData, TAG_SAMPLE_ENCIPHERED_PIN_CODE, ulReadLength, pReadValue) != STATUS_SHARED_EXCHANGE_OK) {
					GTL_Traces_TraceDebug ("__Cless_PayPass_FillBufferForOnlineAuthorisation : An error occurred when adding the enciphered PIN in the structure");
					bErrorDetected = TRUE;
				}
			}
		}
	} else {
		if (pTransactionData == NULL)
			GTL_Traces_TraceDebug ("__Cless_PayPass_FillBufferForOnlineAuthorisation : pTransactionData is NULL");

		if (pOnlineAuthorisationData == NULL)
			GTL_Traces_TraceDebug ("__Cless_PayPass_FillBufferForOnlineAuthorisation : pOnlineAuthorisationData is NULL");

		bErrorDetected = TRUE;
	}

	return (!bErrorDetected);
}



//! \brief Calls the PayPass kernel to perform the transaction.
//! \param[in] pDataStruct Data buffer to be filled and used for PayPass transaction.
//!	\return
//!		- PayPass kernel result.

int Cless_PayPass_PerformTransaction (T_SHARED_DATA_STRUCT * pDataStruct) {
	unsigned char bLoopMode;
	int nResult = CLESS_CR_MANAGER_END;
	int cr, nPosition, nTransactionOutcome, nTransactionStart;
	int bReceiptRequired;
	unsigned long ulReadLength;
	const unsigned char * pReadValue;
	unsigned char ucCvm, ucFieldOff;
	//unsigned char bCustomerDisplayAvailable;
	unsigned char bSaveInBatch = FALSE;
	unsigned short usCardType = 0;
	int merchLang, nCardHolderLang;
	//	int bMobileTransaction;
	int nUirdMessage;
	T_Bool bContinueWithOnlineAuthorisation = B_TRUE;
	MSGinfos tMsg;
	unsigned char ucErrorIndicationByte;
	T_SHARED_DATA_STRUCT * pOnlineAuthorisationData;
	int bPerformDeselect = TRUE;
	T_SHARED_DATA_STRUCT * pGetAllDataStruct;


	perflog("MG\tPP_CUST\tCless_PayPass_PerformTransaction");
	// Indicate PayPass kernel is going to be used (for customisation purposes)
	Cless_Customisation_SetUsedPaymentScheme (CLESS_SAMPLE_CUST_PAYPASS);

	// Loop mode information
	bLoopMode = Cless_Menu_IsTransactionLoopOn();

	// Get the merchant language
	merchLang = PSQ_Give_Language();

	// Check if an external customer display is available
	//bCustomerDisplayAvailable = Helper_IsClessCustomerDisplayAvailable();

	perflog("MG\tPP_CUST\t__Cless_PayPass_AddPayPassSpecificData");
	// Get the PayPass
	if (!__Cless_PayPass_AddPayPassSpecificData(pDataStruct)) {
		GTL_Traces_TraceDebug("Cless_PayPass_PerformTransaction : Cless_Fill_PayPassTransaction failed\n");
	} else {
		// Debug purpose : if you need to dump the data provided to the kernel
		///Cless_DumpData_DumpSharedBufferWithTitle(pDataStruct, "KERNEL INPUT DATA");

		if (Cless_Common_GetTransactionMode () != CLESS_SAMPLE_TRANSACTION_MODE_EXPLICIT) {
			// Launch task that scans the peripheral to be checks for cancel (keyboard, chip, swipe)
			Cless_Scan_LaunchScanningTask ();
			g_ScanningTask = Telium_GiveNoTask (g_tsScanning_task_handle);

			// Waiting task start before to call do_transaction
			Telium_Ttestall (USER_EVENT_START, 2); // Wait scan task start
		}

		mapPut(appCardName, "MASTERCARD", 10);

		// Call the PayPass kernel to perform the transaction
		perflog("MG\tPP_CUST\tPayPass3_DoTransaction");
		cr = PayPass3_DoTransaction(pDataStruct);
		perflog("MG\tPP_CUST\tExit PayPass3_DoTransaction");

		// Get all the kernel data to print the receipt
		PayPass3_GetAllData(pDataStruct);
		CLESS_Data_Save_To_DB(pDataStruct);

#ifdef __PERFORMANCE_LOG__
		Telium_Ttestall(0, 10);
		perflog_dump();
#endif

		/// Cless_DumpData_DumpSharedBufferWithTitle (pDataStruct, "KERNEL OUTPUT");

		if (Cless_Common_GetTransactionMode () != CLESS_SAMPLE_TRANSACTION_MODE_EXPLICIT) {
			// Kill the scanning task
			Cless_Scan_KillScanningTask();
		}


		// Debug purpose : if you need to dump the data provided by the kernel
		if (Cless_Menu_IsTransactionDataDumpingOn() && (!Cless_Menu_IsTransactionLoopOn())) {
			Cless_PayPass_DumpSpecificStructure (pDataStruct, TAG_PAYPASS_USER_INTERFACE_REQUEST_DATA, FALSE);
			Cless_PayPass_DumpSpecificStructure (pDataStruct, TAG_PAYPASS_OUTCOME_PARAMETER_SET, FALSE);
			Cless_PayPass_DumpSpecificStructure (pDataStruct, TAG_PAYPASS_ERROR_INDICATION, TRUE);
			Cless_PayPass_DumpSpecificStructure (pDataStruct, TAG_PAYPASS_DATA_RECORD, FALSE);
			Cless_PayPass_DumpSpecificStructure (pDataStruct, TAG_PAYPASS_DISCRETIONARY_DATA, FALSE);
			Cless_PayPass_DumpSpecificStructure (pDataStruct, TAG_PAYPASS_RRP_MEASURED_PROCESSING_TIME, TRUE);
		}

		// Get prefered card language (a request shall be done to the PayPass kernel as the TAG_KERNEL_SELECTED_PREFERED_LANGUAGE tag is not defaulty returned by the PayPass kernel).
		Cless_GuiState_SetCardholderLanguage (NULL);

		// Get the transaction outcome
		//	- PAYPASS_OPS_STATUS_APPROVED if transaction is approved.
		//	- PAYPASS_OPS_STATUS_ONLINE_REQUEST if an online authorisation is requested.
		//	- PAYPASS_OPS_STATUS_DECLINED if the transaction is declined.
		//	- PAYPASS_OPS_STATUS_SELECT_NEXT if next AID shall be selected.
		//	- PAYPASS_OPS_STATUS_TRY_AGAIN transaction shall be restarted from the begining..
		//	- PAYPASS_OPS_STATUS_END_APPLICATION if the transaction is terminated.
		if (!__Cless_PayPass_RetreiveTransactionOutcome (pDataStruct, &nTransactionOutcome))
			nTransactionOutcome = PAYPASS_OPS_STATUS_END_APPLICATION;

		merchLang = Cless_GuiState_GetMerchantLanguage();
		nCardHolderLang = Cless_GuiState_GetCardholderLanguage();

		// Get the TAG_PAYPASS_OUTCOME_PARAMETER_SET to identify the CVM to be performed
		// 	- PAYPASS_OPS_CVM_NO_CVM : "No CVM" method has been applied.
		// 	- PAYPASS_OPS_CVM_SIGNATURE : "Signature" method has been applied.
		// 	- PAYPASS_OPS_CVM_ONLINE_PIN : "Online PIN" method has been applied.
		//	- PAYPASS_OPS_CVM_CONFIRMATION_CODE_VERIFIED : confirmation code has been verified.
		//	- PAYPASS_OPS_CVM_NA : if CVM is not applicable.
		if (!__Cless_PayPass_RetreiveCvmToApply (pDataStruct, &ucCvm))
			ucCvm = PAYPASS_OPS_CVM_NA;

#ifndef __TEST_VERSION__
		if(ucCvm == PAYPASS_OPS_CVM_CONFIRMATION_CODE_VERIFIED)
			mapPut(traCVMused, "COD", 3);
		else if(ucCvm == PAYPASS_OPS_CVM_NO_CVM)
			mapPut(traCVMused, "NON", 3);
		else if(ucCvm == PAYPASS_OPS_CVM_NO_CVM)
			mapPut(traCVMused, "NON", 3);
#endif

		if (!__Cless_PayPass_RetreiveFieldOffValue (pDataStruct, &ucFieldOff))
			ucFieldOff = PAYPASS_OPS_FIELD_OFF_REQUEST_NA;

		// Retreive the card type
		if (!__Cless_PayPass_RetreiveCardType (pDataStruct, &usCardType))
			usCardType = 0;

		//!		- \a 0 If card type not found.
		//!		- \a 0x8501 for MStripe card.
		//!		- \a 0x8502 for MChip card.
		switch (usCardType) {
		case 0x8501: //MStripe card.
			mapPutByte(appClessMagMode, 1);
			//CLess PIN online forced
			FUN_PinEntry();
			break;
		case 0x8502: //MChip card.
			mapPutByte(appClessMagMode, 0);
			break;
		default:
			break;
		}

		// Check if the receipt has to be printed or not
		if (!__Cless_PayPass_IsReceiptRequired (pDataStruct, &bReceiptRequired))
			bReceiptRequired = FALSE;

		// Get the UIRD message to be displayed
		if (!__Cless_PayPass_RetreiveUirdMessage (pDataStruct, &nUirdMessage))
			nUirdMessage = PAYPASS_UIRD_MESSAGE_ID_NA;

		// Retrieve the start value in the OPS
		if (!__Cless_PayPass_RetrieveStart (pDataStruct, &nTransactionStart))
			nTransactionStart = PAYPASS_OPS_START_NA;


		// Additional possible processing :
		//	- Perform an online authorisation if necessary
		//	- Save the transaction in the batch if transaction is accepted
		//	- Perform CVM processing if necessary

		//		// Check if a mobile transaction has been performed
		//		bMobileTransaction = ((usCardType & PAYPASS_B2_MASK_ON_DEVICE_CVM_SUPPORTED) == PAYPASS_B2_MASK_ON_DEVICE_CVM_SUPPORTED);

		switch (nTransactionOutcome) {
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		case PAYPASS_OPS_STATUS_APPROVED:
		{
			if (Cless_Common_GetTransactionMode () == CLESS_SAMPLE_TRANSACTION_MODE_EXPLICIT) {
				// Kill the scanning task
				Cless_Scan_KillScanningTask();
			}


			if (ucCvm == PAYPASS_OPS_CVM_SIGNATURE) {
				Cless_GuiState_DisplayScreen (CLESS_SAMPLE_PAYPASS_SCREEN_SIGNATURE_REQUIRED, -1, -1);
#ifndef __TEST_VERSION__
				mapPut(traCVMused, "SIG", 3);
#endif
			} else {
				// Display the message
				Cless_GuiState_DisplayScreen (CLESS_SAMPLE_PAYPASS_SCREEN_APPROVED, -1, -1);

				__Cless_PayPass_DisplayBalanceAmount(pDataStruct);
				Helper_RefreshScreen(WAIT, HELPERS_CUSTOMER_SCREEN);

				// Indicate the transaction shall be stored in the batch
				bSaveInBatch = TRUE;
			}

			// Read the "Receipt" message
			Cless_Term_Read_Message(STD_MESS_PRINT_RECEIPT, merchLang, &tMsg);

			// If loop mode is not activated
			if (!bLoopMode) {
				// Create a specific buffer for online authorization
				pGetAllDataStruct = GTL_SharedExchange_InitShared(SAMPLE_PAYPASS_GET_ALL_DATA_SIZE);
				if (pGetAllDataStruct != NULL) {
					// Get all data
					PayPass3_GetAllData(pGetAllDataStruct); // Get all the kernel data to print the receipt
#ifndef __TEST_VERSION__
					CLESS_Data_Save_To_DB(pGetAllDataStruct);
#endif
					//TODO: NEW-CLESS Pick data for printing here

					//					// Print the receipt
					//					//if ((Cless_DumpData_DumpGetOutputId() != CUSTOM_OUTPUT_NONE) && ((bReceiptRequired) || (ucCvm == PAYPASS_OPS_CVM_SIGNATURE) || (HelperQuestionYesNo (&tMsg, 30, merchLang))))
					//					if ((Cless_DumpData_DumpGetOutputId() != CUSTOM_OUTPUT_NONE) && ((bReceiptRequired) || (ucCvm == PAYPASS_OPS_CVM_SIGNATURE)))
					//						Cless_Receipt_PrintPayPass (pGetAllDataStruct, bMobileTransaction, (unsigned int)ucCvm, nCardHolderLang);

					GTL_SharedExchange_DestroyLocal (pGetAllDataStruct);
					pGetAllDataStruct = NULL;
				}
			}

			// If the selected CVM is signature
			if (ucCvm == PAYPASS_OPS_CVM_SIGNATURE) {
				// Read the message
				Cless_Term_Read_Message(STD_MESS_SIGNATURE_OK, merchLang, &tMsg);

				// Request the merchant to check the signature
				if (HelperQuestionYesNo (&tMsg, 30, merchLang)) {
					// Signature is correct
					Cless_GuiState_DisplayScreen (CLESS_SAMPLE_PAYPASS_SCREEN_SIGNATURE_OK, -1, -1);
					bSaveInBatch = TRUE;
				} else {
					// Signature is incorrect
					Cless_GuiState_DisplayScreen (CLESS_SAMPLE_PAYPASS_SCREEN_SIGNATURE_KO, -1, -1);
				}
			}
		}
		break;

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		case PAYPASS_OPS_STATUS_DECLINED:
		{

			if (Cless_Common_GetTransactionMode () == CLESS_SAMPLE_TRANSACTION_MODE_EXPLICIT) {
				// Kill the scanning task
				Cless_Scan_KillScanningTask();
			}

			// Display the message located in the UIRD (DECLINED, INSERT CARD or CLEAR DISPLAY)
			__Cless_PayPass_DisplayUirdMsg (pDataStruct, FALSE);

			if (!UIH_IsRemoveCardTaskRunning())
				HelperLedsOff();
			Helper_WaitUserDisplay();
		}
		break;

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		case PAYPASS_OPS_STATUS_TRY_ANOTHER_INTERFACE:
		{
			if (Cless_Common_GetTransactionMode () == CLESS_SAMPLE_TRANSACTION_MODE_EXPLICIT) {
				// Kill the scanning task
				Cless_Scan_KillScanningTask();
			}

			// Display the message located in the UIRD
			__Cless_PayPass_DisplayUirdMsg (pDataStruct, FALSE);

			if (!UIH_IsRemoveCardTaskRunning())
				HelperLedsOff();
			Helper_WaitUserDisplay();
		}
		break;

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		case PAYPASS_OPS_STATUS_ONLINE_REQUEST:
		{
			if (Cless_Common_GetTransactionMode () == CLESS_SAMPLE_TRANSACTION_MODE_EXPLICIT) {
				// Kill the scanning task
				Cless_Scan_KillScanningTask();
			}

			// Create a specific buffer for online authorisation
			pOnlineAuthorisationData = NULL;
			pOnlineAuthorisationData = GTL_SharedExchange_InitLocal (SAMPLE_PAYPASS_ONLINE_BUFFER_SIZE);

			if (pOnlineAuthorisationData != NULL) {
				// Check if the required CVM is Online PIN
				if (ucCvm == PAYPASS_OPS_CVM_ONLINE_PIN) {
					mapPut(traCVMused, "ONL", 3);
					UIH_RemoveCardTask_WaitEnd();
					bContinueWithOnlineAuthorisation = __Cless_PayPass_OnlinePinManagement (pDataStruct, nCardHolderLang);
				}

				// If PIN online successfully enetered
				if (bContinueWithOnlineAuthorisation == B_TRUE) {
					// Prepare data for online authorisation
					if (__Cless_PayPass_FillBufferForOnlineAuthorisation (pDataStruct, pOnlineAuthorisationData)) {
						// Display the screen indicating the transaction has been sent online for authorisation
						Cless_GuiState_DisplayScreen (CLESS_SAMPLE_SCREEN_ONLINE_PROCESSING, -1, -1);

						// Simulate the online authorisation
						if (__CLESS_CallAuthorisationHost (pOnlineAuthorisationData)) {
							// Get the authorisation response code retreived from the server
							nPosition = SHARED_EXCHANGE_POSITION_NULL;
							if (GTL_SharedExchange_FindNext(pOnlineAuthorisationData, &nPosition, TAG_EMV_AUTHORISATION_RESPONSE_CODE, &ulReadLength, &pReadValue) == STATUS_SHARED_EXCHANGE_OK) {
								// Authorisation code is present, check it is 3030 (OK)
								if ((pReadValue[0] == 0x30) && (pReadValue[1] == 0x30)) {
									// Authorisation code is 3030 (OK), so transaction has been approved by the server
									// Check if the required CVM is signature
									if (ucCvm == PAYPASS_OPS_CVM_SIGNATURE) {
										// Indicate signature is required on the receipt
										Cless_GuiState_DisplayScreen (CLESS_SAMPLE_PAYPASS_SCREEN_SIGNATURE_REQUIRED, -1, -1);
									} else {
										// Signature is not required, indicate the transaction has been approved online
										Cless_GuiState_DisplayScreen (CLESS_SAMPLE_SCREEN_ONLINE_APPROVED, -1, -1);

										// Transaction is approved, so it shall be added to the batch
										bSaveInBatch = TRUE;
									}

									// Get all the kernel data to print the receipt
									PayPass3_GetAllData(pDataStruct);
#ifndef __TEST_VERSION__
									CLESS_Data_Save_To_DB(pDataStruct);
#endif
									// Read the "receipt" message
									Cless_Term_Read_Message(STD_MESS_PRINT_RECEIPT, merchLang, &tMsg);

									// If loop mode is not activated, print the receipt
									if (!bLoopMode) {

										//TODO: NEW-CLESS Pick data for printing here

										//										// Print the receipt
										//										//if ((Cless_DumpData_DumpGetOutputId() != CUSTOM_OUTPUT_NONE) && ((bReceiptRequired) || (ucCvm == PAYPASS_OPS_CVM_SIGNATURE) || (HelperQuestionYesNo (&tMsg, 30, merchLang))))
										//										if ((Cless_DumpData_DumpGetOutputId() != CUSTOM_OUTPUT_NONE) && ((bReceiptRequired) || (ucCvm == PAYPASS_OPS_CVM_SIGNATURE)))
										//											Cless_Receipt_PrintPayPass (pDataStruct, bMobileTransaction, (unsigned int)ucCvm, nCardHolderLang);

									}

									// If signature has been required
									if (ucCvm == PAYPASS_OPS_CVM_SIGNATURE) {
										// Read the message
										Cless_Term_Read_Message(STD_MESS_SIGNATURE_OK, merchLang, &tMsg);

										// If loop mode is activated or merchant validate the signature, approve the transaction
										if ((bLoopMode) || (HelperQuestionYesNo (&tMsg, 30, merchLang))) {
											// Indicate the transaction is approved
											Cless_GuiState_DisplayScreen (CLESS_SAMPLE_PAYPASS_SCREEN_SIGNATURE_OK, -1, -1);

											// Indicate the transaction shall be added to the batch
											bSaveInBatch = TRUE;
										} else {
											// Signature is not correct, transaction is declined (by the merchant)
											Cless_GuiState_DisplayScreen (CLESS_SAMPLE_PAYPASS_SCREEN_SIGNATURE_KO, -1, -1);
										}
									}
								} else {
									// Indicate the transaction has been online declined
									Cless_GuiState_DisplayScreen (CLESS_SAMPLE_SCREEN_ONLINE_DECLINED, -1, -1);
								}
							} else {
								// Indicate the transaction has been online declined (no host answer)
								Cless_GuiState_DisplayScreen (CLESS_SAMPLE_SCREEN_ONLINE_DECLINED, -1, -1);
							}
						} else {
							// Indicate the transaction has been online declined (unable to go online)
							Cless_GuiState_DisplayScreen (CLESS_SAMPLE_PAYPASS_SCREEN_DECLINED, -1, -1);
						}
					} else {
						// Trace the error
						GTL_Traces_TraceDebug ("An error occurred when creating the online data for online authorisation");

						// Indicate an error occurred (transaction is terminated)
						Cless_GuiState_DisplayScreen (CLESS_SAMPLE_SCREEN_ERROR, -1, -1);
						UIH_RemoveCardTask_WaitEnd();
						HelperErrorSequence(WITHBEEP);
						Helper_WaitUserDisplay();
					}
				} else {
					if (bContinueWithOnlineAuthorisation == B_NON_INIT)
						// PIN Entry has been cancelled
						Cless_GuiState_DisplayScreen (CLESS_SAMPLE_SCREEN_PIN_CANCEL, -1, -1);
					else
						// An error occurred during the online PIN entry
						Cless_GuiState_DisplayScreen (CLESS_SAMPLE_SCREEN_PIN_ERROR, -1, -1);
				}

				if (pOnlineAuthorisationData != NULL) {
					GTL_SharedExchange_DestroyLocal (pOnlineAuthorisationData);
					pOnlineAuthorisationData = NULL;
				}
			} else {
				// Trace the error
				GTL_Traces_TraceDebug ("Unable to create a buffer for online authorisation");

				// Indicate an error occurred (transaction is terminated)
				Cless_GuiState_DisplayScreen (CLESS_SAMPLE_SCREEN_ERROR, -1, -1);
				UIH_RemoveCardTask_WaitEnd();
				HelperErrorSequence(WITHBEEP);
				Helper_WaitUserDisplay();
			}
		}
		break;


		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		case PAYPASS_OPS_STATUS_TRY_AGAIN:
		{
			// Transaction shall be restarted from the begining
			nResult = CLESS_CR_MANAGER_RESTART_NO_MESSAGE_BEFORE_RETRY;

			// Do not perform deselect when a silent retart needs to be performed
			bPerformDeselect = FALSE;
		}
		break;


		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		case PAYPASS_OPS_STATUS_SELECT_NEXT:
		{
			// The next AID shall be selected in the candidate list
			nResult = CLESS_CR_MANAGER_REMOVE_AID;
		}
		break;


		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		default: // Error case
		{
			// Check if the transaction must be restarted
			if ((nTransactionOutcome == PAYPASS_OPS_STATUS_END_APPLICATION) && (nTransactionStart == PAYPASS_OPS_START_B)) {
				// Do not perform deselect in the generic case, as all is managed here
				bPerformDeselect = FALSE;

				__Cless_PayPass_RetreiveErrorIndicationByte (pDataStruct, PAYPASS_EI_L1_BYTE, &ucErrorIndicationByte);

				// Check if the restart is due to a communication error with the card
				if((nUirdMessage == PAYPASS_UIRD_MESSAGE_ID_TRY_AGAIN) && (ucErrorIndicationByte != PAYPASS_EI_L1_OK)) {
					// Display a message and wait card removal
					if(ucFieldOff == PAYPASS_OPS_FIELD_OFF_REQUEST_NA) {
						Cless_GuiState_DisplayScreen (CLESS_SAMPLE_PAYPASS_SCREEN_WAIT_CARD_REMOVAL, -1, -1); 
						///ClessEmv_DeselectCard(0, TRUE, TRUE);
						ClessEmv_CloseDriver();
					}

					if (Cless_Customisation_GetDoubleTapInProgress())
						Cless_GuiState_DisplayScreen (CLESS_SAMPLE_SCREEN_PHONE_INSTRUCTIONS_RETRY, -1, -1);
					else {
						if (Cless_Common_GetTransactionMode() == CLESS_SAMPLE_TRANSACTION_MODE_EXPLICIT)
							Cless_GuiState_DisplayScreen (CLESS_SAMPLE_SCREEN_RETRY, -1, -1); 
					}

					HelperErrorSequence(WITHBEEP);

					// Transaction shall be restarted from the begining
					nResult = CLESS_CR_MANAGER_RESTART;
				} else {
					// Message has already been displayed in the customisation of steps STEP_PAYPASS_MCHIP_SEND_PHONE_MSG and STEP_PAYPASS_MSTRIPE_SEND_PHONE_MSG

					// Wait card removal
					if(ucFieldOff == PAYPASS_OPS_FIELD_OFF_REQUEST_NA)
						///ClessEmv_DeselectCard(0, TRUE, TRUE);
						ClessEmv_CloseDriver();

					if (Cless_Customisation_GetDoubleTapInProgress())
						nResult = CLESS_CR_MANAGER_RESTART_DOUBLE_TAP;
					else
						nResult = CLESS_CR_MANAGER_RESTART;
				}
			} else {// Standard error case

				if (Cless_Common_GetTransactionMode () == CLESS_SAMPLE_TRANSACTION_MODE_EXPLICIT) {
					// Kill the scanning task
					Cless_Scan_KillScanningTask();
				}

				GTL_Traces_TraceDebug ("PayPass3_DoTransaction result = %02X, OPS Status = %02X", cr, nTransactionOutcome);

				__Cless_PayPass_RetreiveErrorIndicationByte (pDataStruct, PAYPASS_EI_L3_BYTE, &ucErrorIndicationByte);

				if (!((nTransactionOutcome == PAYPASS_OPS_STATUS_END_APPLICATION) && ((ucErrorIndicationByte == PAYPASS_EI_L3_STOP) || (nUirdMessage == PAYPASS_UIRD_MESSAGE_ID_CLEAR_DISPLAY))))
					///{
					///Cless_GuiState_DisplayScreen (CLESS_SAMPLE_SCREEN_CANCELLED, -1, -1);
					///}
					///else
				{
					// Indicate an error occurred (transaction is terminated)
					Cless_GuiState_DisplayScreen (CLESS_SAMPLE_SCREEN_ERROR, -1, -1);
					UIH_RemoveCardTask_WaitEnd();
					HelperErrorSequence(WITHBEEP);
					Helper_WaitUserDisplay();
				}

				// Indicate double tap is not in progress
				Cless_Customisation_SetDoubleTapInProgress(FALSE);
			}
		}
		break;
		}

		if(nResult != CLESS_CR_MANAGER_REMOVE_AID) {
			// After the transaction, turn off the field during the time requested in the OPS
			if(ucFieldOff != PAYPASS_OPS_FIELD_OFF_REQUEST_NA) {
				ClessEmv_CloseDriver();
				if(ucFieldOff != 0)
					Telium_Ttestall(0, ucFieldOff * 10);
			} else {
				if (nTransactionStart == PAYPASS_OPS_START_NA) {
					if (bPerformDeselect)
						ClessEmv_DeselectCard(0, TRUE, FALSE);
					ClessEmv_CloseDriver();
				}
			}

			// Turn on the 1st LED only
			if ((nResult == CLESS_CR_MANAGER_RESTART) || (nResult == CLESS_CR_MANAGER_RESTART_DOUBLE_TAP)) {
				HelperWaitLedsTime();
				HelperCardWait();
			}
		}

		// If transaction shall be save in the batch, save it
		if (bSaveInBatch)
			__Cless_PayPass_AddRecordToBatch (pDataStruct, usCardType);

		//				// If activated, dump the APDU and the PayPass 3.0 Data Exchange (DEK and DET signals)
		//				if (Cless_Menu_IsApduDataExchangeDumpingOn()) {
		//		PayPass3_GetAllData(pDataStruct); // Get all the kernel data
		//		__Cless_PayPass_DumpDataExchangeInformation (pDataStruct);
		//				}

		// If the transaction does not restart from the begining, set the LEDs into the idle state
		if ((nResult != CLESS_CR_MANAGER_RESTART) && (nResult != CLESS_CR_MANAGER_RESTART_NO_MESSAGE_BEFORE_RETRY) && (nResult != CLESS_CR_MANAGER_REMOVE_AID) && (nResult != CLESS_CR_MANAGER_RESTART_DOUBLE_TAP)) {
			// Increment the transaction sequence counter
			Cless_Batch_IncrementTransactionSeqCounter();
		}

		// If activated, dump all the kernel database
		if (Cless_Menu_IsTransactionDatabaseDumpingOn()) {
			PayPass3_GetAllData(pDataStruct); // Get all the kernel data
			Cless_DumpData_DumpSharedBufferWithTitle (pDataStruct, "DATABASE DUMP");
		}
	}

	// Transaction is completed, clear PayPass kernel transaction data
	PayPass3_Clear ();

	// In any case, wait and kill the UI task
	UIH_RemoveCardTask_WaitEnd();

	switch (usCardType) {
	case 0x8501: //MStripe card.
		ClosePeripherals();
		break;
	default:
		break;
	}

	// Return result
	return (nResult);
}



//! \brief Add the transaction data record to the batch.
//! \param[in] pSharedData Shared buffer to be used to retreive the data record.
//! \param[in] usCardType Card type.

static void __Cless_PayPass_AddRecordToBatch (T_SHARED_DATA_STRUCT * pSharedData, unsigned short usCardType) {
	// Tags required for Offline and Online approved transactions
	T_TI_TAG tRequestedTags[] = {TAG_PAYPASS_DATA_RECORD, TAG_PAYPASS_DISCRETIONARY_DATA};

	unsigned int nIndex;
	int nPosition;
	int nResult, bErrorDetected;
	int merchLang;
	MSGinfos tMsg;
	T_SHARED_DATA_STRUCT * pBatchData;
	T_SHARED_DATA_STRUCT tTempStruct;
	unsigned long ulReadLength;
	unsigned char * pReadValue;

	(void) usCardType;

	// Clear the shared exchange buffer
	GTL_SharedExchange_ClearEx (pSharedData, FALSE);
	nIndex = 0;
	bErrorDetected = FALSE;


	// Allocate the sharde buffer
	pBatchData = GTL_SharedExchange_InitLocal (2048);

	if (pBatchData != NULL) {
		// Request Data Record and Discretionary Data to the kernel
		while (nIndex < NUMBER_OF_ITEMS(tRequestedTags)) {
			GTL_SharedExchange_AddTag (pSharedData, tRequestedTags[nIndex], 0, NULL);
			nIndex ++;
		}

		// Get the tags from the kernel
		nResult = PayPass3_GetData (pSharedData);

		if (nResult == KERNEL_STATUS_OK) {
			// Get the Data Record
			nPosition = SHARED_EXCHANGE_POSITION_NULL;
			if (GTL_SharedExchange_FindNext (pSharedData, &nPosition, TAG_PAYPASS_DATA_RECORD, &ulReadLength, (const unsigned char **)&pReadValue) == STATUS_SHARED_EXCHANGE_OK) {
				// Copy data record data in the local buffer
				if (GTL_SharedExchange_InitEx (&tTempStruct, ulReadLength, ulReadLength, pReadValue) == STATUS_SHARED_EXCHANGE_OK) {
					if (GTL_SharedExchange_AddSharedBufferContent (pBatchData, &tTempStruct) != STATUS_SHARED_EXCHANGE_OK) {
						GTL_Traces_TraceDebug ("__Cless_PayPass_AddRecordToBatch : Unable to copy the local structure with data record");
						bErrorDetected = TRUE;
					}
				} else {
					GTL_Traces_TraceDebug ("__Cless_PayPass_AddRecordToBatch : Unable to init the local structure with data record");
					bErrorDetected = TRUE;
				}
			} else {
				GTL_Traces_TraceDebug ("__Cless_PayPass_AddRecordToBatch : Unable to retreive Data Record");
				bErrorDetected = TRUE;
			}

			if (!bErrorDetected)
			{
				// Get the Discretionary Data
				nPosition = SHARED_EXCHANGE_POSITION_NULL;
				if (GTL_SharedExchange_FindNext (pSharedData, &nPosition, TAG_PAYPASS_DISCRETIONARY_DATA, &ulReadLength, (const unsigned char **)&pReadValue) == STATUS_SHARED_EXCHANGE_OK)
				{
					// Copy data record data in the local buffer
					if (GTL_SharedExchange_InitEx (&tTempStruct, ulReadLength, ulReadLength, pReadValue) == STATUS_SHARED_EXCHANGE_OK)
					{
						if (GTL_SharedExchange_AddSharedBufferContent (pBatchData, &tTempStruct) != STATUS_SHARED_EXCHANGE_OK)
						{
							GTL_Traces_TraceDebug ("__Cless_PayPass_AddRecordToBatch : Unable to copy the local structure with discretionary data");
							bErrorDetected = TRUE;
						}
					}
					else
					{
						GTL_Traces_TraceDebug ("__Cless_PayPass_AddRecordToBatch : Unable to init the local structure with discretionary data");
						bErrorDetected = TRUE;
					}
				}
				else
				{
					GTL_Traces_TraceDebug ("__Cless_PayPass_AddRecordToBatch : Unable to retreive discretionary Data");
					bErrorDetected = TRUE;
				}
			}

			if (!bErrorDetected)
			{
				if (!Cless_Batch_AddTransactionToBatch (pBatchData))
				{
					GTL_Traces_TraceDebug ("__Cless_PayPass_AddRecordToBatch : Save transaction in batch failed");
					bErrorDetected = TRUE;
				}
			}
		}
		else
		{
			GTL_Traces_TraceDebug ("__Cless_PayPass_AddRecordToBatch : Unable to get PayPass data (nResult = %02x)\n", nResult);
			bErrorDetected = TRUE;
		}

		// Destroy allocated buffer
		GTL_SharedExchange_DestroyLocal (pBatchData);
	}
	else
	{
		// An error occurred when initialising the local data exchange
		GTL_Traces_TraceDebug ("__Cless_PayPass_AddRecordToBatch : An error occurred when initialising the local data exchange");
		bErrorDetected = TRUE;
	}

	if (bErrorDetected)
	{
		merchLang = PSQ_Give_Language();
		Cless_Term_Read_Message(STD_MESS_BATCH_ERROR, merchLang, &tMsg);
		Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, LEDSOFF);
		Helper_RefreshScreen (WAIT, HELPERS_MERCHANT_SCREEN);
	}
}

//! \brief Set if Data Exchange is in progress or not.
//! \param[in] bInProgress Data Exchange in progress or not.

void Cless_PayPass_SetDataExchangeInProgress (const int bInProgress)
{
	gDataExchangeInProgress = bInProgress;
}



//! \brief Check if Data Exchange is in progress.
//! \return TRUE In progress, FALSE No Data Exchange in progress.

int Cless_PayPass_GetDataExchangeInProgress (void) {
	return (gDataExchangeInProgress);
}



//! \brief Print the buffer in MCW tag format 
//! \param[in] pReadValue buffer with Ingenico tag formats.
//! \param[in] nPosition position in current buffer.
//! \return none

//static void __Cless_PayPass_PrintMCWFormat (unsigned char * pReadValue, unsigned long ulReadLength) {
//	unsigned long loop, character;
//	int nFinished = FALSE, cr;
//	T_BER_TLV_DECODE_STRUCT tBerTlvStruct;
//	int nReadBytes;
//	unsigned long ulTagMCW;
//	BER_TLV_TAG ReadTag;
//	BER_TLV_LENGTH ReadLength;
//	BER_TLV_VALUE ReadValue;
//	unsigned long ulConstructedReadLength;
//	unsigned char * pConstructedReadValue;
//
//	// Get the data to be parse in the correct format
//	GTL_BerTlvDecode_Init (&tBerTlvStruct, pReadValue, ulReadLength);
//
//	do {
//		character =0;
//		cr = Cless_DataExchange_ParseTlv (&tBerTlvStruct, &ReadTag, &ReadLength, &ReadValue, &nReadBytes);
//
//		if (cr == STATUS_BER_TLV_END) {
//			nFinished = TRUE;		// completed
//		} else if (cr == STATUS_BER_TLV_OK) {
//			// convert Ingenico tag to MCW tag
//			ulTagMCW = Cless_DataExchange_McwTagConversion (ReadTag);
//
//			// Print the Tag
//			Cless_DumpData ("%lX", ulTagMCW);
//
//			// check if tag constructed (use Ingenico tag as MCW DF tag is not a correctly formatted constructed tag)
//			// and there is sufficient data for a constructed tag
//			if ((GTL_BerTlv_IsTagConstructedObject(ReadTag)) && (ReadLength > 4)) {
//				Cless_DumpData ("LL");
//				Cless_DumpData_DumpNewLine();
//
//				pConstructedReadValue = (unsigned char *) ReadValue;
//				ulConstructedReadLength = ReadLength;
//
//				// Call this fucntion to print the constructed data
//				__Cless_PayPass_PrintMCWFormat (pConstructedReadValue, ulConstructedReadLength);
//			} else {
//				// Print the Length
//				if (ReadLength<=0x7F)
//					Cless_DumpData ("%02lX", ReadLength);
//				else if (ReadLength<=0xFF)
//					Cless_DumpData ("81%02lX", ReadLength);
//				else if (ReadLength<=0xFFFF)
//					Cless_DumpData ("82%04lX", ReadLength);
//
//				Cless_DumpData_DumpNewLine();
//
//				// Print the tag data
//				for (loop=0;loop<ReadLength; loop++) {
//					Cless_DumpData ("%02X", ReadValue[loop]);
//					if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER) {
//						character++;
//
//						// Limit the print output to 24 characters to ensure a new line is printed
//						if (character == 24) {
//							character=0;
//							Cless_DumpData_DumpNewLine();
//						}
//					}
//				}
//
//				if (character != 0)
//					Cless_DumpData_DumpNewLine();
//			}
//		} else {
//			// Error, continue
//			nFinished = TRUE;
//			GTL_Traces_TraceDebug("__Cless_PayPass_PrintMCWForma: Error parsing Data Exchange Data (cr = %02x)", cr);
//		}
//	} while (!nFinished);
//
//	Cless_DumpData_DumpNewLine();
//}

