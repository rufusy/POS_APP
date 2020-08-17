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
#include "Globals.h"

/////////////////////////////////////////////////////////////////
//// Macros & preprocessor definitions //////////////////////////
#ifndef TAG_PAYWAVE_TERMINAL_TRANSACTION_QUALIFIERS
#define TAG_PAYWAVE_TERMINAL_TRANSACTION_QUALIFIERS 0x9F66
#endif // TAG_PAYWAVE_TERMINAL_TRANSACTION_QUALIFIERS

#define C_TIME_1S							100
#define C_CPT_TIME_20S						20

/////////////////////////////////////////////////////////////////
//// Define the transaction card provider type //////////////////


/////////////////////////////////////////////////////////////////
//// Global variables ///////////////////////////////////////////

static const char gsTerminalSupportedLanguage[] =
{
		"en"
		"fr"
};
static unsigned char gs_ucTransactionSeqCounter[4];		/*!< Generic transaction data : Payment Interface serial number. */

static int nCardInBlackList = 0;
static int gs_ErrorAmountTooHigth = FALSE;			    /*!< Global data indicating if an error is due to amount graeter than TAG_EP_CLESS_TRANSACTION_LIMIT. */


/////////////////////////////////////////////////////////////////
//// Static functions definition ////////////////////////////////

static int __Cless_QuickPass_AddQuickPassSpecificData (T_SHARED_DATA_STRUCT * pDataStruct);
static int __Cless_QuickPass_RetrieveSignature (T_SHARED_DATA_STRUCT * pResultDataStruct, unsigned char * pSignature);
static int __Cless_QuickPass_PinOnLineRequiredState (T_SHARED_DATA_STRUCT * pResultDataStruct, unsigned char * pPinOnLineRequiredState);
static void __Cless_QuickPass_AddRecordToBatch (T_SHARED_DATA_STRUCT * pSharedData);
static int __Cless_QuickPass_ManageTransactionLog(int nMerchLang);



/////////////////////////////////////////////////////////////////
//// Functions //////////////////////////////////////////////////

//! \brief Fill buffer with specific QuickPass for transaction.
//! \param[out] pDataStruct Shared exchange structure filled with the specific QuickPass data.
//! \return
//!		- \ref TRUE if correctly performed.
//!		- \ref FALSE if an error occurred.

static int __Cless_QuickPass_AddQuickPassSpecificData (T_SHARED_DATA_STRUCT * pDataStruct)
{
	int cr, nResult;
	card InvNum = 0;
	T_KERNEL_TRANSACTION_FLOW_CUSTOM sTransactionFlowCustom;
	object_info_t ObjectInfo;
	unsigned char StepInterruption[KERNEL_PAYMENT_FLOW_STOP_LENGTH];// Bit field to stop payment flow,
	// if all bit set to 0 => no stop during payment process
	// if right bit set to 1 : stop after payment step number 1
	unsigned char StepCustom[KERNEL_PAYMENT_FLOW_CUSTOM_LENGTH]; 	// Bit field to custom payment flow,
	// if all bit set to 0 => no stop during payment process
	// if right bit set to 1 : stop after payment step number 1
	if (pDataStruct == NULL){
		GTL_Traces_TraceDebug("__Cless_QuickPass_AddQuickPassSpecificData : Invalid Input data");
		nResult = FALSE;
		goto End;
	}

	// Init parameteters
	memset(StepInterruption, 0, sizeof(StepInterruption)); // Default Value : not stop on process
	memset(StepCustom, 0, sizeof(StepCustom)); // Default Value : not stop on process
	nResult = TRUE;

	// Customize steps :
	ADD_STEP_CUSTOM(STEP_QUICKPASS_QVSDC_REMOVE_CARD,StepCustom); 		// To do GUI when MChip card has been read
	ADD_STEP_CUSTOM(STEP_QUICKPASS_QVSDC_GET_CERTIFICATE,StepCustom); 	// To get the certificate for ODA step
	ADD_STEP_CUSTOM(STEP_QUICKPASS_QVSDC_GET_CERTIFICATE_FOR_ONLINE, StepCustom); // To get the certificate for ODA for on line step

	if (Cless_IsBlackListPresent())	
		ADD_STEP_CUSTOM(STEP_QUICKPASS_QVSDC_BLACK_LIST_CONTROL,StepCustom);	// To check Pan in Black list

	ObjectGetInfo(OBJECT_TYPE_APPLI, ApplicationGetCurrent(), &ObjectInfo);


	// Add a tag for Do_Txn management
	cr = GTL_SharedExchange_AddTag(pDataStruct, TAG_KERNEL_PAYMENT_FLOW_STOP, KERNEL_PAYMENT_FLOW_STOP_LENGTH, (const unsigned char *)StepInterruption);
	if (cr != STATUS_SHARED_EXCHANGE_OK) {
		GTL_Traces_TraceDebug("__Cless_QuickPass_AddQuickPassSpecificData : Unable to add TAG_KERNEL_PAYMENT_FLOW_STOP in shared buffer (cr=%02x)", cr);
		nResult = FALSE;
		goto End;
	}

	// Add a tag for Do_Txn management
	memcpy ((void*)&sTransactionFlowCustom, (void*)StepCustom, KERNEL_PAYMENT_FLOW_CUSTOM_LENGTH);
	sTransactionFlowCustom.usApplicationType = ObjectInfo.application_type; // Kernel will call this application for customisation 
	sTransactionFlowCustom.usServiceId = SERVICE_CUSTOM_KERNEL; // Kernel will call SERVICE_CUSTOM_KERNEL service id for customisation

	cr = GTL_SharedExchange_AddTag(pDataStruct, TAG_KERNEL_PAYMENT_FLOW_CUSTOM, sizeof(T_KERNEL_TRANSACTION_FLOW_CUSTOM), (const unsigned char *)&sTransactionFlowCustom);
	if (cr != STATUS_SHARED_EXCHANGE_OK) {
		GTL_Traces_TraceDebug("__Cless_QuickPass_AddQuickPassSpecificData : Unable to add TAG_KERNEL_PAYMENT_FLOW_CUSTOM in shared buffer (cr=%02x)", cr);
		nResult = FALSE;
		goto End;
	}

	// Add Tag TAG_KERNEL_TERMINAL_SUPPORTED_LANGUAGES
	cr = GTL_SharedExchange_AddTag(pDataStruct, TAG_KERNEL_TERMINAL_SUPPORTED_LANGUAGES, sizeof(gsTerminalSupportedLanguage), (const unsigned char *)&gsTerminalSupportedLanguage);
	if (cr != STATUS_SHARED_EXCHANGE_OK) {
		GTL_Traces_TraceDebug("__Cless_QuickPass_AddQuickPassSpecificData : Unable to add TAG_KERNEL_TERMINAL_SUPPORTED_LANGUAGES in shared buffer (cr=%02x)", cr);
		nResult = FALSE;
		goto End;
	}

	/// ----------------------------------------------------=---- UPI ADDED DATA ----------------------------------------------------------------

	///Get the Transaction Sequence counter
	mapGetCard(appInvNum, InvNum);
	memset (gs_ucTransactionSeqCounter, 0, sizeof (gs_ucTransactionSeqCounter));
	// Convert the TSC
	//	num2dec(gs_ucTransactionSeqCounter, InvNum, 2 * lenTrnSeqCnt); // ulong -> "NNNN"
	GTL_Convert_UlToDcbNumber (InvNum, gs_ucTransactionSeqCounter, 4);

	// Add the Transaction sequence counter maintained in terminal
	cr = GTL_SharedExchange_AddTag(pDataStruct, TAG_EMV_TRANSACTION_SEQUENCE_COUNTER, sizeof(gs_ucTransactionSeqCounter), gs_ucTransactionSeqCounter);

	if (cr != STATUS_SHARED_EXCHANGE_OK) {
		GTL_Traces_TraceDebug("Cless_Fill_TransactionGenericData : Unable to add TAG_EMV_TRANSACTION_SEQUENCE_COUNTER in shared buffer (cr=%02x)", cr);
		nResult = FALSE;
		goto End;
	}

	/// -----------------------------------------------------------------------------------------------------------------------------------------


	End:
	return (nResult);
}

//! \brief Get the QuickPass signature state.
//! \param[out] pSignature :
//!		- \a 0 No signature to made.
//!		- \a 1 Signature to made.
//! \return
//!		- \ref TRUE if correctly retrieved.
//!		- \ref FALSE if an error occurred.

static int __Cless_QuickPass_RetrieveSignature (T_SHARED_DATA_STRUCT * pResultDataStruct, unsigned char * pSignature)
{
	int nResult = TRUE;
	int nPosition, cr;
	unsigned long ulReadLength;
	const unsigned char * pReadValue;

	nPosition = SHARED_EXCHANGE_POSITION_NULL;
	* pSignature = 0; // Default result : no signature

	cr = GTL_SharedExchange_FindNext(pResultDataStruct, &nPosition, TAG_KERNEL_SIGNATURE_REQUESTED, &ulReadLength, &pReadValue);

	if (cr != STATUS_SHARED_EXCHANGE_OK)
	{
		GTL_Traces_TraceDebug("__Cless_QuickPass_RetrieveSignature : Unable to get SigantureRequested from the QuickPass kernel response (cr = %02x)", cr);
		nResult = FALSE;
		goto End;
	}

	// Get the transaction outcome
	* pSignature = pReadValue[0];

	End:
	return (nResult);
}


//! \brief Get the QuickPass PinOnLineRequired state.
//! \param[out] pPinOnLineRequiredState :
//!		- \a 0 No Pin OnLine Requested to made.
//!		- \a 1 Pin OnLine Requested to made.
//! \return
//!		- \ref TRUE if correctly retrieved.
//!		- \ref FALSE if an error occurred.

static int __Cless_QuickPass_PinOnLineRequiredState (T_SHARED_DATA_STRUCT * pResultDataStruct, unsigned char * pPinOnLineRequiredState){
	int nResult = TRUE;
	int nPosition, cr;
	unsigned long ulReadLength;
	const unsigned char * pReadValue;
	char UPI_Credit_and_QuasiCredit[3 + 1];
	char panData[19 + 1];

	nPosition = SHARED_EXCHANGE_POSITION_NULL;
	* pPinOnLineRequiredState = 0; // Default result : no PinOnLineRequired

	memset(UPI_Credit_and_QuasiCredit, 0, sizeof(UPI_Credit_and_QuasiCredit));
	mapGet(traUPI_Credit_QuasiCredit, UPI_Credit_and_QuasiCredit, mapDatLen(traUPI_Credit_QuasiCredit));

	if (strncmp(UPI_Credit_and_QuasiCredit, "1", 1) == 0){
		goto End;
	}

	cr = GTL_SharedExchange_FindNext(pResultDataStruct, &nPosition, TAG_KERNEL_ONLINE_PIN_REQUESTED, &ulReadLength, &pReadValue);

	if (cr != STATUS_SHARED_EXCHANGE_OK) {
		GTL_Traces_TraceDebug("__Cless_QuickPass_PinOnLineRequiredState : Unable to get OnLinePinRequested from the QuickPass kernel response (cr = %02x)", cr);
		nResult = FALSE;
		goto End;
	}

	// Get the transaction outcome
	* pPinOnLineRequiredState = pReadValue[0];

	if (strncmp(UPI_Credit_and_QuasiCredit, "2", 1) == 0){

		///modify only for Credit and Quasi Credi UPI cards
		memset(panData, 0, sizeof(panData));
		mapGet(traPan, panData, mapDatLen(traPan));

		if (strncmp(panData, "629260", 6) == 0) {
			* pPinOnLineRequiredState = 1; // Default result : no PinOnLineRequired
		}

	}

	End:
	return (nResult);
}

//! \brief If transaction Log has been configured and was managed by card, this function ask if Log must be printed and print it if necessary.
//! \param[in] pDataStruct share buffer with tags informations.
//! \param[in] nMerchLang lang to use for the merchant.
//! \return
//!		- nothing.
static int __Cless_QuickPass_ManageTransactionLog(int nMerchLang)
{
	int cr;
	int nResult = KERNEL_STATUS_OK;
	MSGinfos tDisplayMsg;
	T_SHARED_DATA_STRUCT * pDataStruct = NULL;

	// Clear output data buffer
	Cless_Fill_InitSharedBufferWithKernels (&pDataStruct);

	// Add a tag to read in kernel
	cr = GTL_SharedExchange_AddTag (pDataStruct, TAG_EMV_LOG_FORMAT, 0, NULL);
	if (cr != STATUS_SHARED_EXCHANGE_OK) goto End;
	cr = GTL_SharedExchange_AddTag (pDataStruct, TAG_QUICKPASS_TRANSACTION_LOG_RECORD, 0, NULL);
	if (cr != STATUS_SHARED_EXCHANGE_OK) goto End;

	QuickPass_GetData (pDataStruct);

	if (pDataStruct->ulDataLength){ // If a data has been obtained
		// Print a receipt only if signature requested or merchant need to print a receipt
		Cless_Term_Read_Message(STD_MESS_PRINT_TRANSACTION_LOG, nMerchLang, &tDisplayMsg);

		if (HelperQuestionYesNo (&tDisplayMsg, 30, nMerchLang))
			Cless_DumpData_DumpSharedBufferWithTitle (pDataStruct, "Transaction Log :");
	}

	GTL_SharedExchange_DestroyShare(pDataStruct);
	End:
	return (nResult);
}

//! \brief Manage the debug mode for QuickPass kernel
//! \param[in] bActivate \a TRUE to activate the debug features. \a FALSE to deactivate features.

void Cless_QuickPass_DebugActivation (int bActivate)
{
	T_SHARED_DATA_STRUCT * pSharedStructure;
	int nResult;
	unsigned char ucDebugMode = 0x00;

	if (bActivate)
		ucDebugMode = KERNEL_DEBUG_MASK_TRACES + KERNEL_DEBUG_MASK_APDU + KERNEL_DEBUG_MASK_TIMERS;

	pSharedStructure = GTL_SharedExchange_InitShared(256);

	if (pSharedStructure != NULL)
	{
		nResult = GTL_SharedExchange_AddTag(pSharedStructure, TAG_KERNEL_DEBUG_ACTIVATION, 1, &ucDebugMode);

		if (nResult != STATUS_SHARED_EXCHANGE_OK)
		{
			GTL_Traces_TraceDebug("Cless_QuickPass_DebugActivation : Unable to add TAG_KERNEL_DEBUG_ACTIVATION (nResult = %02x)", nResult);
		}
		else
		{
			nResult = QuickPass_DebugManagement(pSharedStructure);

			if (nResult != KERNEL_STATUS_OK)
			{
				GTL_Traces_TraceDebug("Cless_QuickPass_DebugActivation : Error occurred during QuickPass Debug activation (nResult = %02x)", nResult);
			}
		}

		// Destroy the shared buffer
		GTL_SharedExchange_DestroyShare(pSharedStructure);
	}
}



//! \brief Perform the QuickPass kernel customisation.
//! \param[in,out] pSharedData Shared buffer used for customisation.
//! \param[in] ucCustomisationStep Step to be customised.
//! \return
//!		- \a KERNEL_STATUS_CONTINUE always.

int Cless_QuickPass_CustomiseStep (T_SHARED_DATA_STRUCT * pSharedData, const unsigned char ucCustomisationStep)
{
	int nResult = KERNEL_STATUS_CONTINUE;
	int nPosition;
	unsigned char ucCapkIndex;
	unsigned char ucRid[5];
	unsigned long ulReadLength;
	const unsigned char * pReadValue;
	const unsigned char * pPan;
	unsigned long ulLgPan;
	const unsigned char * pPanSeqNumber;
	unsigned char ucVoidPanSeqNumber = C_CLESS_VOID_PAN_SEQ_NUMBER; // Unused value for PanSeqNumber
	unsigned long ulLgSeqPan;

	switch (ucCustomisationStep) // Steps to customise
	{
	case STEP_QUICKPASS_QVSDC_REMOVE_CARD:
		perflog("MG\tpW_CUST\tSTEP_QUICKPASS_xxxx_REMOVE_CARD");
		HelperRemoveCardSequence(pSharedData);
		GTL_SharedExchange_ClearEx (pSharedData, FALSE);
		nResult = KERNEL_STATUS_CONTINUE;
		break;

	case (STEP_QUICKPASS_QVSDC_GET_CERTIFICATE):
	case (STEP_QUICKPASS_QVSDC_GET_CERTIFICATE_FOR_ONLINE):
	perflog("MG\tpW_CUST\tSTEP_QUICKPASS_QVSDC_GET_CERTIFICATE");
	memset (ucRid, 0, sizeof(ucRid));
	nPosition = SHARED_EXCHANGE_POSITION_NULL;
	if (GTL_SharedExchange_FindNext (pSharedData, &nPosition, TAG_EMV_CA_PUBLIC_KEY_INDEX_CARD, &ulReadLength, (const unsigned char **)&pReadValue) == STATUS_SHARED_EXCHANGE_OK)
		ucCapkIndex = pReadValue[0];
	else
		ucCapkIndex = 0;

	nPosition = SHARED_EXCHANGE_POSITION_NULL;
	if (GTL_SharedExchange_FindNext (pSharedData, &nPosition, TAG_EMV_DF_NAME, &ulReadLength, (const unsigned char **)&pReadValue) == STATUS_SHARED_EXCHANGE_OK)
		memcpy (ucRid, pReadValue, 5);

	GTL_SharedExchange_ClearEx (pSharedData, FALSE);

	if (!Cless_Parameters_GetCaKeyData (pTreeCurrentParam, ucCapkIndex, ucRid, pSharedData))
	{
		GTL_Traces_TraceDebug ("Cless_QuickPass_CustomiseStep : Cless_Parameters_GetCaKeyData failed");
	}

	nResult = KERNEL_STATUS_CONTINUE;
	break;

	case (STEP_QUICKPASS_QVSDC_BLACK_LIST_CONTROL):
						perflog("MG\tpW_CUST\tSTEP_QUICKPASS_QVSDC_BLACK_LIST_CONTROL");
	nResult = KERNEL_STATUS_CONTINUE; // default value

	// Take values and length of : TAG_EMV_APPLI_PAN and TAG_EMV_APPLI_PAN_SEQUENCE_NUMBER tag.

	nPosition = SHARED_EXCHANGE_POSITION_NULL;
	if (GTL_SharedExchange_FindNext (pSharedData, &nPosition, TAG_EMV_APPLI_PAN, &ulLgPan, &pPan) != STATUS_SHARED_EXCHANGE_OK)
	{
		// Pan parameters is missing, we cannot check BlackList
		break;
	}

	nPosition = SHARED_EXCHANGE_POSITION_NULL;
	// Length of this tag is 1
	if (GTL_SharedExchange_FindNext (pSharedData, &nPosition, TAG_EMV_APPLI_PAN_SEQUENCE_NUMBER, &ulLgSeqPan, &pPanSeqNumber) != STATUS_SHARED_EXCHANGE_OK)
	{
		// Pan Sequence Number is missing, we will check BlackList without PanSeqNumber
		pPanSeqNumber = &ucVoidPanSeqNumber;
	}

	// Reset output buffer
	GTL_SharedExchange_ClearEx (pSharedData, FALSE);

	if (Cless_BlackListIsPan((int)ulLgPan, pPan, (int)(*pPanSeqNumber)) == TRUE)
	{
		// Card is in BlackList
		unsigned char ucPanInBlackList = 1;
		int cr;

		// add information in exchange buffer
		cr = GTL_SharedExchange_AddTag(pSharedData, TAG_KERNEL_PAN_IN_BLACK_LIST, 1, &ucPanInBlackList);
		if (cr != STATUS_SHARED_EXCHANGE_OK)
		{
			GTL_Traces_TraceDebug("Cless_QuickPass_CustomiseStep An error occurred when adding TAG_KERNEL_PAN_IN_BLACK_LIST in the shared buffer (cr = %02x)", cr);
			GTL_SharedExchange_ClearEx (pSharedData, FALSE);
			break;
		}
		nCardInBlackList = 1;
	}
	else
		nCardInBlackList = 0;

	break;
	// Other customisation steps could be defined if necessary

	default:
		perflog("MG\tpW_CUST\tSTEP_QUICKPASS_ unknown");
		GTL_Traces_TraceDebug ("Cless_QuickPass_CustomiseStep : Step to customise (unknown) = %02x", ucCustomisationStep);
		break;
	}

	perflog("MG\tpW_CUST\tEnd STEP_QUICKPASS_xxx");
	return (nResult);
}




#ifdef __TELIUM3__
static word g_MainTask = 0;
static word g_DoTransactionTaskIdTask = 0;
static t_topstack *g_DoTransactionTask = NULL;
static T_SHARED_DATA_STRUCT * g_pDataStruct = NULL;
static int g_DoTransactionCr = KERNEL_STATUS_INTERNAL_ERROR;

static word Cless_QuickPass_DoTransactionTask (void)
{
	do
	{
		perflog("MG\tpW_CUST\tQuickPass_DoTransaction");
		g_DoTransactionCr = QuickPass_DoTransaction(g_pDataStruct);
		perflog("MG\tpW_CUST\tEnd QuickPass_DoTransaction");
		perflog("MG\tpW_CUST\tCless_QuickPass_DoTransactionTask SignalEvent");

		mapPut(appCardName, "UPI", 3);

		// Get all the kernel data to print the receipt
		QuickPass_GetAllData(g_pDataStruct);
		CLESS_Data_Save_To_DB(g_pDataStruct);

		Telium_SignalEvent(g_MainTask, E_USER_EVENT);

		Telium_Ttestall(USER_EVENT_START, 0);
	} while(1);
	return 0;
}

static int __Cless_QuickPass_StartDoTransaction (T_SHARED_DATA_STRUCT * pDataStruct) {
	perflog("MG\tpW_CUST\t__Cless_QuickPass_StartDoTransaction");

	g_DoTransactionCr = KERNEL_STATUS_INTERNAL_ERROR;
	if (g_DoTransactionTask == NULL) {
		// Get the main task id
		perflog("MG\tpW_CUST\tTelium_CurrentTask");
		g_MainTask = Telium_CurrentTask();
		perflog("MG\tpW_CUST\tExit Telium_CurrentTask");

		perflog("MG\tpW_CUST\tTelium_ClearEvents");
		Telium_ClearEvents(USER_EVENT);
		perflog("MG\tpW_CUST\tExit Telium_ClearEvents");

		// Launch the scanning task
		perflog("MG\tpW_CUST\tTelium_Fork");
		g_pDataStruct = pDataStruct;
		g_DoTransactionTask = Telium_Fork (&Cless_QuickPass_DoTransactionTask, NULL, -1);
		perflog("MG\tpW_CUST\tExit Telium_Fork");

		// The task cannot be created
		if (g_DoTransactionTask != NULL)
		{
			perflog("MG\tpW_CUST\tTelium_GiveNoTask");
			g_DoTransactionTaskIdTask = Telium_GiveNoTask(g_DoTransactionTask);
			perflog("MG\tpW_CUST\tExit Telium_GiveNoTask");
			perflog("MG\tpW_CUST\tExit __Cless_QuickPass_StartDoTransaction");
			return OK;
		}
		else
		{
			g_DoTransactionTaskIdTask = 0;
			perflog("MG\tpW_CUST\tExit __Cless_QuickPass_StartDoTransaction");
			return KO;
		}
	}
	else
	{
		g_pDataStruct = pDataStruct;
		perflog("MG\tpW_CUST\t__Cless_QuickPass_StartDoTransaction Telium_SignalEvent");
		Telium_SignalEvent(g_DoTransactionTaskIdTask, E_USER_EVENT_START);
		perflog("MG\tpW_CUST\tExit __Cless_QuickPass_StartDoTransaction Telium_SignalEvent");
		return OK;
	}
}
#endif



int Cless_QuickPass_PerformTransaction (T_SHARED_DATA_STRUCT * pDataStruct)
{
	unsigned char bLoopMode;
	int cr, nFound;
	int nResult = C_CLESS_CR_END;
	int nPosition;
	int nCurrencyCode;
	unsigned long ulReadLength;
	const unsigned char * pReadValue;
	unsigned char ucOnLinePinRequested;
	unsigned char bOnlinePinError = FALSE;
	BUFFER_SAISIE buffer_saisie;

	unsigned char cr_pin_online = 0;
	unsigned char ucOnLineDeclined;
	const unsigned char * pPan;
	unsigned long ulLgPan;
	unsigned char ucSignature;
	unsigned char auCustomerDisplayAvailable;
	unsigned long ulAmount = 0;
	unsigned long ulLgAmount;
	const unsigned char * pAmount;
	unsigned char * pInfo;
	unsigned char * pCurrencyCode;
	unsigned char * pttq;
	int bSaveInBatch = FALSE;
	int merchLang, nCardHolderLang;
	MSGinfos tDisplayMsg;	

	int Len = 0;
	char Temp[256];

	perflog("MG\tpW_CUST\tCless_QuickPass_PerformTransaction");
	// Loop mode information
	bLoopMode = Cless_Menu_IsTransactionLoopOn();

	// The contactless transaction can be stopped by a contact card (swipe or chip)
	auCustomerDisplayAvailable = Helper_IsClessCustomerDisplayAvailable();

	merchLang = PSQ_Give_Language();
	perflog("MG\tpW_CUST\tCless_QuickPass_PerformTransaction 01");

	// Indicate QuickPass kernel is going to be used (for customisation purposes)
	Cless_Customisation_SetUsedPaymentScheme (CLESS_SAMPLE_CUST_QUICKPASS);
	perflog("MG\tpW_CUST\tCless_QuickPass_PerformTransaction 02");

	if (!__Cless_QuickPass_AddQuickPassSpecificData(pDataStruct)) {
		GTL_Traces_TraceDebug("Cless_QuickPass_PerformTransaction : __Cless_QuickPass_AddQuickPassSpecificData error\n");
	} else {
		perflog("MG\tpW_CUST\tCless_QuickPass_PerformTransaction 03");
		// Dump all the data sent to the QuickPass kernel
		////Cless_DumpData_DumpSharedBuffer(pDataStruct, 0);

		// For later use
		ulAmount = 0;
		nCurrencyCode = 0;

		nFound = Cless_Common_RetrieveInfo (pDataStruct, TAG_EMV_AMOUNT_AUTH_BIN, &pInfo);

		if (nFound){
			ulLgAmount = 4;
			GTL_Convert_BinNumberToUl(pInfo, &ulAmount, ulLgAmount);
		}				
		perflog("MG\tpW_CUST\tCless_QuickPass_PerformTransaction 04");

		nFound = Cless_Common_RetrieveInfo (pDataStruct, TAG_EMV_TRANSACTION_CURRENCY_CODE, &pCurrencyCode);
		if (nFound) {
			nCurrencyCode = ((*pCurrencyCode)<< 8) + *(pCurrencyCode+1);
		}				
		// End for later use

		perflog("MG\tpW_CUST\tCless_QuickPass_PerformTransaction 05");

#ifndef __TELIUM3__
		Cless_Scan_TransacOpenDrivers();
#endif
		perflog("MG\tpW_CUST\tCless_QuickPass_PerformTransaction 06");

		// Launch task that scans the peripheral to be checks for cancel (keyboard, chip, swipe)
#ifndef __TELIUM3__
		Cless_Scan_LaunchScanningTask ();
		g_ScanningTask = Telium_GiveNoTask (g_tsScanning_task_handle);
#endif
		perflog("MG\tpW_CUST\tCless_QuickPass_PerformTransaction 07");

		//Telium_Ttestall (USER_EVENT_START, 0); // Wait scan task start
		perflog("MG\tpW_CUST\tCless_QuickPass_PerformTransaction 08");

		// Perform the QuickPass transaction
		//Telium_Ttestall (0, 400);
#ifndef __TELIUM3__
		perflog("MG\tpW_CUST\tQuickPass_DoTransaction");
		cr = QuickPass_DoTransaction(pDataStruct);

		perflog("MG\tpW_CUST\tEnd QuickPass_DoTransaction");
#else
		cr = __Cless_QuickPass_StartDoTransaction(pDataStruct);
		if (cr == OK) {
			//			Telium_File_t * hKeyboard = NULL;
			Telium_File_t * hSwipe2 = NULL;
			Telium_File_t * hCam0 = NULL;
			unsigned char StatusSwipe = 0;
			unsigned char StatusCam = 0;
			int nEvent = 0;
			int bSendCancel;
			int bWaitEndTr = TRUE;

			//			perflog("MG\tpW_CUST\tTelium_Fopen(KEYBOARD)");
			//			hKeyboard = Telium_Fopen("KEYBOARD", "r*");
			//			perflog("MG\tpW_CUST\tExit Telium_Fopen(KEYBOARD)");

			perflog("MG\tpW_CUST\tTelium_Stdperif(SWIPE2)");
			hSwipe2 = Telium_Stdperif("SWIPE2", NULL);
			perflog("MG\tpW_CUST\tExit Telium_Stdperif(SWIPE2)");
			if (hSwipe2 != NULL)
			{
				perflog("MG\tpW_CUST\tTelium_Status(SWIPE2)");
				Telium_Status(hSwipe2, &StatusSwipe);
				perflog("MG\tpW_CUST\tExit Telium_Status(SWIPE2)");
				if ((StatusSwipe & TRACK_READ) != 0)
				{
					nEvent = SWIPE2;
					g_ListOfEvent = SWIPE2;
				}
			}

			if (nEvent == 0)
			{
				perflog("MG\tpW_CUST\tTelium_Stdperif(CAM0)");
				hCam0 = Telium_Stdperif("CAM0", NULL);
				perflog("MG\tpW_CUST\tExit Telium_Stdperif(CAM0)");
				if (hCam0 != NULL)
				{
					perflog("MG\tpW_CUST\tTelium_Status(CAM0)");
					Telium_Status(hCam0, &StatusCam);
					perflog("MG\tpW_CUST\tExit Telium_Status(CAM0)");
					if ((StatusCam & CAM_PRESENT) != 0)
					{
						nEvent = CAM0;
						g_ListOfEvent = CAM0;
					}
				}
			}

			if (nEvent == 0)
			{
				bSendCancel = FALSE;
				do
				{
					nEvent = Telium_Ttestall (KEYBOARD | CAM0 | SWIPE2 | USER_EVENT, 0);
					if ((nEvent & USER_EVENT) != 0)
					{
						// It indicates the payment is done
						bWaitEndTr = FALSE;
					}
					else if ((nEvent & KEYBOARD) != 0)
					{
						if (Telium_Getchar() == T_ANN) // Cancel only if red key pressed
						{
							bSendCancel = TRUE;
							g_ListOfEvent = KEYBOARD;
						}
					}
					else if ((nEvent & CAM0) != 0)
					{
						bSendCancel = TRUE;
						g_ListOfEvent = CAM0;
					}
					else
					{
						bSendCancel = TRUE;
						g_ListOfEvent = SWIPE2;
					}
				} while((!bSendCancel) && (bWaitEndTr));
			}
			else bSendCancel = TRUE;

			if (bSendCancel)
				QuickPass_Cancel();
			//			if (hKeyboard != NULL)
			//				Telium_Fclose(hKeyboard);
			if (bWaitEndTr)
				Telium_Ttestall (USER_EVENT, 0);
		}
		cr = g_DoTransactionCr;
#endif

#ifdef __PERFORMANCE_LOG__
		Telium_Ttestall(0, 10);
		perflog_dump();
#endif


		// Kill the scanning task
#ifndef __TELIUM3__
		Cless_Scan_KillScanningTask();
		Cless_Scan_TransacCloseDrivers();
#endif


		// Debug purpose : if you need to dump the data provided by the kernel
		if (Cless_Menu_IsTransactionDataDumpingOn())
			Cless_DumpData_DumpSharedBufferWithTitle (pDataStruct, "QUICKPASS DATA");

		// Get prefered card language (defaulty returned by the QuickPass kernel (if present in the kernel database)
		nCardHolderLang = merchLang; // By default, cardholder language is set to default language
		if (Cless_Common_RetrieveInfo (pDataStruct, TAG_KERNEL_SELECTED_PREFERED_LANGUAGE, &pInfo))
			nCardHolderLang = Cless_Term_GiveLangNumber(pInfo);

		// Specific treatment for on-line/Pin management
		ucOnLinePinRequested = 0; // Default value : no Online Pin requested

#ifndef __TEST_VERSION__
		mapPut(traCVMused, "NON", 3);
#endif
		if ((cr == KERNEL_STATUS_OFFLINE_APPROVED) || (cr == KERNEL_STATUS_ONLINE_AUTHORISATION)) {
			__Cless_QuickPass_PinOnLineRequiredState (pDataStruct, &ucOnLinePinRequested);
			if (ucOnLinePinRequested) // If pin asked
				cr = KERNEL_STATUS_ONLINE_AUTHORISATION; // => mandatory to go on-line
		}

		// Check if signature is requested or not
		ucSignature = 0;
		__Cless_QuickPass_RetrieveSignature (pDataStruct, &ucSignature);

#ifndef __TEST_VERSION__
		if (ucOnLinePinRequested){
			mapPut(traCVMused, "ONL", 3);
		} else if (ucSignature) {
			mapPut(traCVMused, "SIG", 3);
		}
#endif

		// CR analyse
		if ((cr & KERNEL_STATUS_STOPPED_BY_APPLICATION_MASK) ||	(cr == KERNEL_STATUS_CANCELLED)) // If mask has been set
		{
			switch (g_ListOfEvent) {
			case CAM0:			// Chip card
				if (Cless_Scan_TransacCam0(ulAmount, nCurrencyCode) == FALSE)
					GTL_Traces_TraceDebug ("Cless_QuickPass_PerformTransaction Pb Transac Cam 0");

				break;

			case SWIPE2:		// Swipe
				if (Cless_Scan_TransacSwipe2(ulAmount, nCurrencyCode) == FALSE)
					GTL_Traces_TraceDebug ("Cless_QuickPass_PerformTransaction Pb Transac Swipe 2");

				break;

			case USER_EVENT:	// User event (from main task), it indicates the task is going to be killed (because g_bScanning_task_to_be_killed has been set to TRUE by the custom application
				Cless_Term_Read_Message(STD_MESS_USER, merchLang, &tDisplayMsg);
				Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tDisplayMsg, NOLEDSOFF);
				Helper_RefreshScreen(WAIT, HELPERS_MERCHANT_SCREEN);
				break;

			case KEYBOARD:		// Keyboard event
				Cless_Term_Read_Message(STD_MESS_KEYBOARD, merchLang, &tDisplayMsg);
				Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tDisplayMsg, NOLEDSOFF);
				Helper_RefreshScreen(WAIT, HELPERS_MERCHANT_SCREEN);
				break;

			default:
				Cless_Term_Read_Message(STD_MESS_STOPPED, merchLang, &tDisplayMsg);
				Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tDisplayMsg, NOLEDSOFF);
				Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
				HelperErrorSequence (WITHBEEP);
				Helper_WaitUserDisplay();
				break;
			}
		}
		else 
		{
			// VisaWave 2.1 chapter 5.19
			// Note: An AAC returned by the card application for refunds and credits simply indicates completion of card action analysis,
			// and should not be treated as a "decline" of the refund.
			if (cr == KERNEL_STATUS_OFFLINE_DECLINED)
			{
				// Specific case :
				if (Cless_Common_RetrieveInfo (pDataStruct, TAG_EMV_TRANSACTION_TYPE, &pInfo))
					// if ((CR == KERNEL_STATUS_OFFLINE_DECLINED) && (transaction_type == Refund) && (DECLINED_BY_CARD == 1))
					if (pInfo[0] == 0x20)
						if (Cless_Common_RetrieveInfo (pDataStruct, TAG_QUICKPASS_DECLINED_BY_CARD, &pInfo))
						{
							// => transaction must be accepted
							// cr = KERNEL_STATUS_OFFLINE_APPROVED;
							cr = KERNEL_STATUS_OFFLINE_APPROVED;
							QuickPass_GetAllData(pDataStruct); // Get all the kernel data to print the receipt
						}
			}

			switch (cr)
			{
			case KERNEL_STATUS_OK:
				// A good transaction state must be finished by a approved, declined, ...
				if (auCustomerDisplayAvailable) // If a customer display is available
				{
					Cless_Term_Read_Message(STD_MESS_ERROR_STATUS, merchLang, &tDisplayMsg);
					Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tDisplayMsg, LEDSOFF);
					Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
				}
				Cless_Term_Read_Message(STD_MESS_ERROR_STATUS, nCardHolderLang, &tDisplayMsg);
				Helper_DisplayTextCustomer(ERASE, HELPERS_CUSTOMER_LINE_1, &tDisplayMsg, CLESS_ALIGN_CENTER, LEDSOFF);
				Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
				HelperErrorSequence (WITHBEEP);
				Helper_WaitUserDisplay();
				break;

			case (KERNEL_STATUS_OFFLINE_APPROVED):
						// Warning : on iWL3XX
						// Touch screen can be processing only with Cless field stopped
						if(ClessEmv_IsDriverOpened())
							ClessEmv_CloseDriver();

			if (ucSignature) {
				if(!ucOnLinePinRequested){
					mapPut(traCVMused, "SIG", 3);
				}
				Helper_Visa_SignatureCVMIHM(auCustomerDisplayAvailable,merchLang,nCardHolderLang);
			}
			else
			{
				Helper_Visa_ApprovedIHM(HELPERS_PAYWAVE, OFFLINE,pDataStruct,auCustomerDisplayAvailable,merchLang,nCardHolderLang, WITH_AOSA);

				// Transaction shall be added to the batch
				bSaveInBatch = TRUE;
			}

			// Print a receipt only if signature requested or merchant need to print a receipt
			Cless_Term_Read_Message(STD_MESS_PRINT_RECEIPT, merchLang, &tDisplayMsg);

			//				if (bLoopMode == 0)
			//				{
			//					if ((Cless_DumpData_DumpGetOutputId() != CUSTOM_OUTPUT_NONE) && ((ucSignature) || (HelperQuestionYesNo (&tDisplayMsg, 30, merchLang))))
			//						Cless_Receipt_PrintTransaction(pDataStruct, BATCH_TICKET_QUICKPASS, APPROVED_TICKED, ucSignature, nCardHolderLang, WITH_AOSA);
			//				}

			if (ucSignature) {
				if(!ucOnLinePinRequested){
					mapPut(traCVMused, "SIG", 3);
				}
				Cless_Term_Read_Message(STD_MESS_SIGNATURE_OK, merchLang, &tDisplayMsg);
				if (HelperQuestionYesNo (&tDisplayMsg, 30, merchLang)) {
					Helper_Visa_SignatureOKIHM(HELPERS_PAYWAVE, pDataStruct,auCustomerDisplayAvailable,merchLang,nCardHolderLang);

					bSaveInBatch = TRUE;
				} else {
					Helper_Visa_SignatureKOIHM(HELPERS_PAYWAVE, pDataStruct,auCustomerDisplayAvailable,merchLang,nCardHolderLang);
				}
			}

			__Cless_QuickPass_ManageTransactionLog(merchLang);

			break;

			case (KERNEL_STATUS_OFFLINE_DECLINED):
						// Warning : on iWL3XX
						// Touch screen can be processing only with Cless field stopped
						if(ClessEmv_IsDriverOpened())
							ClessEmv_CloseDriver();

			Helper_Visa_DeclinedIHM(HELPERS_PAYWAVE, OFFLINE, pDataStruct, auCustomerDisplayAvailable,merchLang,nCardHolderLang);

			if (bLoopMode == 0)
			{
				// Print a receipt only if signature requested or merchant need to print a receipt
				Cless_Term_Read_Message(STD_MESS_PRINT_RECEIPT, merchLang, &tDisplayMsg);

				//					if ((Cless_DumpData_DumpGetOutputId() != CUSTOM_OUTPUT_NONE) && (HelperQuestionYesNo (&tDisplayMsg, 30, merchLang)))
				//						Cless_Receipt_PrintTransaction(pDataStruct, BATCH_TICKET_QUICKPASS, DECLINED_TICKED, WITHOUT_SIGNATURE, nCardHolderLang, WITH_AOSA);
			}

			__Cless_QuickPass_ManageTransactionLog(merchLang);

			break;

			case (KERNEL_STATUS_ONLINE_AUTHORISATION):

						// Warning : on iWL3XX
						// Touch screen can be processing only with Cless field stopped
						if(ClessEmv_IsDriverOpened())
							ClessEmv_CloseDriver();

			// Is Online Pin asked and possible ?
			if (ucOnLinePinRequested) // If OnLine Pin Requested
			{
				ucSignature = 0;

				nPosition = SHARED_EXCHANGE_POSITION_NULL;
				if (GTL_SharedExchange_FindNext (pDataStruct, &nPosition, TAG_EMV_APPLI_PAN, &ulLgPan, &pPan) != STATUS_SHARED_EXCHANGE_OK)
				{
					GTL_Traces_TraceDebug ("Cless_QuickPass_PerformTransaction : Missing PAN for Online PIN");
					bOnlinePinError = TRUE; // On-line Pin cannot be made
				}else{
					Len = (int)ulLgPan;
					memset(Temp, 0, sizeof(Temp));
					bin2hex(Temp, pPan, Len);

					mapPut(traPan, &Temp[0], Len*2);
				}

				ulAmount = 0;
				nPosition = SHARED_EXCHANGE_POSITION_NULL;
				if (GTL_SharedExchange_FindNext (pDataStruct, &nPosition, TAG_EMV_AMOUNT_AUTH_NUM, &ulLgAmount, &pAmount) != STATUS_SHARED_EXCHANGE_OK)
				{
					GTL_Traces_TraceDebug ("Cless_QuickPass_PerformTransaction : Missing Amount auth for Online PIN");
					bOnlinePinError = TRUE; // On-line Pin cannot be made : amount to display not available
				}
				else
				{
					GTL_Convert_DcbNumberToUl(pAmount, &ulAmount, ulLgAmount);
				}

				if (!bOnlinePinError) {
					char acDummyMsg[] = "";

					// Warning, erase display must be made only if Pin input will be made on customer screen
					if (Cless_IsPinpadPresent()) {
						// Display dummy message to erase display
						tDisplayMsg.message = acDummyMsg;
						tDisplayMsg.coding = _ISO8859_;
						tDisplayMsg.file = GetCurrentFont();
						Helper_DisplayTextCustomer(ERASE, HELPERS_CUSTOMER_LINE_1, &tDisplayMsg, CLESS_ALIGN_CENTER, LEDSOFF);
						Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
					}

					cr_pin_online = Cless_PinManagement_OnLinePinManagement ((unsigned char*)pPan, 1, ulAmount, 30000, 10000, nCardHolderLang, &buffer_saisie);

					if (cr_pin_online == INPUT_PIN_ON) {
						// Add a tag for on-line authorisation
						cr = GTL_SharedExchange_AddTag(pDataStruct, TAG_SAMPLE_ENCIPHERED_PIN_CODE, buffer_saisie.nombre , (const unsigned char *)buffer_saisie.donnees);
						if (cr != STATUS_SHARED_EXCHANGE_OK) {
							GTL_Traces_TraceDebug("Cless_QuickPass_PerformTransaction : Unable to add TAG__ENCIPHERED_PIN_CODE in shared buffer (cr=%02x)", cr);
							bOnlinePinError = TRUE;
						}
					} else {
						bOnlinePinError = TRUE;
					}
				}
			}

			ucOnLineDeclined = FALSE;

			if (!bOnlinePinError)
			{
				Helper_Visa_OnlineProcessingIHM(HELPERS_PAYWAVE, pDataStruct, auCustomerDisplayAvailable,merchLang,nCardHolderLang);

				T_SHARED_DATA_STRUCT* tmpShared;

				tmpShared = GTL_SharedExchange_InitShared(2048);
				payWave_GetAllData(tmpShared); // Get all the kernel data to be able to send all data
				CLESS_Data_Save_To_DB(tmpShared);
				// Destroy the shared buffer
				GTL_SharedExchange_DestroyShare(tmpShared);

				///////////////////////////////////////////////
				//////////// ON-LINE AUTHORISATION ////////////
				///////////////////////////////////////////////
				if(__CLESS_CallAuthorisationHost(pDataStruct)){
					//				if (Cless_FinancialCommunication_ManageAuthorisation (pDataStruct)){

					nPosition = SHARED_EXCHANGE_POSITION_NULL;

					if (GTL_SharedExchange_FindNext(pDataStruct, &nPosition, TAG_EMV_AUTHORISATION_RESPONSE_CODE, &ulReadLength, &pReadValue) == STATUS_SHARED_EXCHANGE_OK)
					{
						if ((pReadValue[0] == 0x30) && (pReadValue[1] == 0x30))
						{
							if (ucSignature)
							{
								if(!ucOnLinePinRequested){
									mapPut(traCVMused, "SIG", 3);
								}
								Helper_Visa_SignatureCVMIHM(auCustomerDisplayAvailable,merchLang,nCardHolderLang);
							}
							else
							{
								Helper_Visa_ApprovedIHM(HELPERS_PAYWAVE, ONLINE,pDataStruct,auCustomerDisplayAvailable,merchLang,nCardHolderLang, WITH_AOSA);

								// Transaction shall be added to the batch
								bSaveInBatch = TRUE;
							}

							// Print a receipt only if signature requested or merchant need to print a receipt
							Cless_Term_Read_Message(STD_MESS_PRINT_RECEIPT, merchLang, &tDisplayMsg);

							if (bLoopMode == 0)
							{
								if ((Cless_DumpData_DumpGetOutputId() != CUSTOM_OUTPUT_NONE) && ((ucSignature) || (HelperQuestionYesNo (&tDisplayMsg, 30, merchLang))))
								{
									//										Cless_Receipt_PrintTransaction(pDataStruct, BATCH_TICKET_QUICKPASS, APPROVED_TICKED, ucSignature, nCardHolderLang, WITH_AOSA);
								}
							}

							if (ucSignature)
							{
								Cless_Term_Read_Message(STD_MESS_SIGNATURE_OK, merchLang, &tDisplayMsg);
								if ((bLoopMode) || (HelperQuestionYesNo (&tDisplayMsg, 30, merchLang)))
								{
									Helper_Visa_SignatureOKIHM(HELPERS_PAYWAVE, pDataStruct,auCustomerDisplayAvailable,merchLang,nCardHolderLang);
									bSaveInBatch = TRUE;
								}
								else
								{
									Helper_Visa_SignatureKOIHM(HELPERS_PAYWAVE, pDataStruct,auCustomerDisplayAvailable,merchLang,nCardHolderLang);
								}
							}
						}
						else
						{
							// No AfterTxn
							ucOnLineDeclined = TRUE;
						}
					}
					else
					{
						// No Authorisation response code
						ucOnLineDeclined = TRUE;
					}
				}
				else
				{
					// Unable to go online, offline decline
					ucOnLineDeclined = TRUE;
				}
			}
			else
			{
				unsigned int nUsePinPad = FALSE;

				if (Cless_IsPinpadPresent())
					if(IsPPSConnected() == 0)
						// pinpad is here and can be used
						nUsePinPad = TRUE;

				if (auCustomerDisplayAvailable || nUsePinPad) // If a customer display is available
				{
					if (cr_pin_online == CANCEL_INPUT) // Specifc case : input PIN has been canceled
						Cless_Term_Read_Message(STD_MESS_PIN_CANCEL, merchLang, &tDisplayMsg);
					else
						Cless_Term_Read_Message(STD_MESS_PIN_ERROR, merchLang, &tDisplayMsg);

					Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tDisplayMsg, LEDSOFF);
					Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
				}

				if (cr_pin_online == CANCEL_INPUT) // Specific case : input PIN has been canceled
					Cless_Term_Read_Message(STD_MESS_PIN_CANCEL, nCardHolderLang, &tDisplayMsg);
				else
					Cless_Term_Read_Message(STD_MESS_PIN_ERROR, nCardHolderLang, &tDisplayMsg);
				Helper_DisplayTextCustomer(ERASE, HELPERS_CUSTOMER_LINE_1, &tDisplayMsg, CLESS_ALIGN_CENTER, LEDSOFF);
				Helper_RefreshScreen(WAIT, HELPERS_CUSTOMER_SCREEN);

				GTL_Traces_TraceDebug ("Cless_QuickPass_PerformTransaction : An error occurred for PIN Entry");
			}

			if (ucOnLineDeclined) // Problem with communication or transaction rejected
			{
				Helper_Visa_DeclinedIHM(HELPERS_PAYWAVE, ONLINE, pDataStruct, auCustomerDisplayAvailable,merchLang,nCardHolderLang);

				if (bLoopMode == 0)
				{
					// Print a receipt only if signature requested or merchant need to print a receipt
					Cless_Term_Read_Message(STD_MESS_PRINT_RECEIPT, merchLang, &tDisplayMsg);

					//						if ((Cless_DumpData_DumpGetOutputId() != CUSTOM_OUTPUT_NONE) && (HelperQuestionYesNo (&tDisplayMsg, 30, merchLang)))
					//							Cless_Receipt_PrintTransaction(pDataStruct, BATCH_TICKET_QUICKPASS, DECLINED_TICKED, WITHOUT_SIGNATURE, nCardHolderLang, WITH_AOSA);
				}
			}

			__Cless_QuickPass_ManageTransactionLog(merchLang);

			break;

			case (KERNEL_STATUS_USE_CONTACT_INTERFACE):
						nResult = CLESS_CR_MANAGER_RESTART_WO_CLESS;
			break;

			case (KERNEL_STATUS_COMMUNICATION_ERROR):
						nResult = CLESS_CR_MANAGER_RESTART;
			break;

			case (KERNEL_STATUS_REMOVE_AID):
						nResult = CLESS_CR_MANAGER_REMOVE_AID;
			break;

			case (KERNEL_STATUS_MOBILE):
						if (Cless_Common_RetrieveInfo (pDataStruct, TAG_QUICKPASS_TERMINAL_TRANSACTION_QUALIFIERS, &pttq))
						{
							if (pttq[0] & 0x20) // If qVSDC supported
							{
								if (auCustomerDisplayAvailable) // If a customer display is available
								{
									Cless_Term_Read_Message(STD_MESS_PHONE_INSTRUCTIONS_L1, merchLang, &tDisplayMsg);
									Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_2, &tDisplayMsg, LEDSOFF);
									Cless_Term_Read_Message(STD_MESS_PHONE_INSTRUCTIONS_L2, merchLang, &tDisplayMsg);
									Helper_DisplayTextMerchant(NO_ERASE, HELPERS_MERCHANT_LINE_3, &tDisplayMsg, LEDSOFF);
									Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
								}
								Cless_Term_Read_Message(STD_MESS_PHONE_INSTRUCTIONS_L1, nCardHolderLang, &tDisplayMsg);
								Helper_DisplayTextCustomer(ERASE, HELPERS_CUSTOMER_LINE_1, &tDisplayMsg, CLESS_ALIGN_CENTER, LEDSOFF);
								Cless_Term_Read_Message(STD_MESS_PHONE_INSTRUCTIONS_L2, nCardHolderLang, &tDisplayMsg);
								Helper_DisplayTextCustomer(NO_ERASE, HELPERS_CUSTOMER_LINE_2, &tDisplayMsg, CLESS_ALIGN_CENTER, LEDSOFF);
								Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);

								// QuickPass Specification 2.1 Req 5.61
								// After a duration of between 1000ms and 1500ms, the reader shall power up the contactless interface and return
								// to Discovery Processing. Any message displayed to the cardholder shall continue to be displayed during the subsequent
								// Discovery Processing.
								if(ClessEmv_IsDriverOpened())
									ClessEmv_CloseDriver(); // Stop Cless Field

								HelperErrorSequence (WITHBEEP);
								Helper_QuickWaitUserDisplay();

								nResult = CLESS_CR_MANAGER_RESTART_DOUBLE_TAP;
							}
							else
							{
								if (auCustomerDisplayAvailable) // If a customer display is available
								{
									Cless_Term_Read_Message(STD_MESS_ERROR_STATUS, merchLang, &tDisplayMsg);
									Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tDisplayMsg, LEDSOFF);
									Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
								}
								Cless_Term_Read_Message(STD_MESS_ERROR_STATUS, nCardHolderLang, &tDisplayMsg);
								Helper_DisplayTextCustomer(ERASE, HELPERS_CUSTOMER_LINE_1, &tDisplayMsg, CLESS_ALIGN_CENTER, LEDSOFF);
								Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);

								HelperErrorSequence (WITHBEEP);
								Helper_WaitUserDisplay();

								// display error
								nResult = CLESS_CR_MANAGER_END;
							}
						}
						else
						{
							nResult = CLESS_CR_MANAGER_RESTART_DOUBLE_TAP;
						}
			break;

			default:
				GTL_Traces_TraceDebug ("Cless_QuickPass_PerformTransaction result = %02x", cr);

				// For Visa Europe, no error should be displayed.
				// If the Custom application is configured in Visa Europe mode, transaction shall be conducted over another interface
				if (!Cless_Menu_IsVisaEuropeModeOn())
				{
					Helper_Visa_ErrorIHM(auCustomerDisplayAvailable,merchLang,nCardHolderLang);
				}
				else
					nResult = CLESS_CR_MANAGER_RESTART_WO_CLESS;
				break;
			}
		}

		// Cless field must be stopped only if we don't try to work with an another AID
		if ((nResult != CLESS_CR_MANAGER_REMOVE_AID) && (nResult != CLESS_CR_MANAGER_RESTART_WO_CLESS) && (nResult != CLESS_CR_MANAGER_RESTART_DOUBLE_TAP))
		{
			if(ClessEmv_IsDriverOpened())	// FT 14414, because in case of OFFLINE_DECLINED driver has been closed
			{	// Deselect the card
				ClessEmv_DeselectCard(0, TRUE, FALSE);
			}
		}

		// If the transaction does not restart from the begining, set the LEDs into the idle state
		if ((nResult != CLESS_CR_MANAGER_RESTART) && (nResult != CLESS_CR_MANAGER_REMOVE_AID))
		{
			// Check if transaction shall be saved in the batch
			if (bSaveInBatch)
				__Cless_QuickPass_AddRecordToBatch (pDataStruct);

			// Increment 
			Cless_Batch_IncrementTransactionSeqCounter();

			// If activated, dump all the kernel database
			if (Cless_Menu_IsTransactionDatabaseDumpingOn())
			{
				QuickPass_GetAllData(pDataStruct); // Get all the kernel data to print the receipt

				if (Cless_DumpData_DumpOpenOutputDriver())
				{
					if (Cless_Common_RetrieveInfo (pDataStruct, TAG_QUICKPASS_EXEC_FUNCTIONS_INFO, &pInfo)) // TAG_QUICKPASS_EXEC_FUNCTIONS_INFO
					{
						// ODA : SDA for online started
						if (pInfo[0] & C_QUICKPASS_EXEC_SDA_FOR_ONLINE_STARTED)
						{
							Cless_DumpData ("%s", "SDA for online started");
							Cless_DumpData_DumpNewLine();
						}
						// ODA : DDA for online started
						if (pInfo[0] & C_QUICKPASS_EXEC_DDA_FOR_ONLINE_STARTED)
						{
							Cless_DumpData ("%s", "DDA for online started");
							Cless_DumpData_DumpNewLine();
						}
						// ODA : SDA (offline) started
						if (pInfo[0] & C_QUICKPASS_EXEC_SDA_OFFLINE_STARTED)
						{
							Cless_DumpData ("%s", "SDA (offline) started");
							Cless_DumpData_DumpNewLine();
						}
						// ODA : DDA (offline) started
						if (pInfo[0] & C_QUICKPASS_EXEC_DDA_OFFLINE_STARTED)
						{
							Cless_DumpData ("%s", "DDA (offline) started");
							Cless_DumpData_DumpNewLine();
						}
						// ODA : SDA for online successfully executed
						if (pInfo[0] & C_QUICKPASS_EXEC_SDA_FOR_ONLINE_OK)
						{
							Cless_DumpData ("%s", "SDA for online OK");
							Cless_DumpData_DumpNewLine();
						}
						// ODA : DDA for online successfully executed
						if (pInfo[0] & C_QUICKPASS_EXEC_DDA_FOR_ONLINE_OK)
						{
							Cless_DumpData ("%s", "DDA for online OK");
							Cless_DumpData_DumpNewLine();
						}
						// ODA : SDA (offline) successfully executed
						if (pInfo[0] & C_QUICKPASS_EXEC_SDA_OFFLINE_OK)
						{
							Cless_DumpData ("%s", "SDA (offline) OK");
							Cless_DumpData_DumpNewLine();
						}
						// ODA : DDA (offline) successfully executed
						if (pInfo[0] & C_QUICKPASS_EXEC_DDA_OFFLINE_OK)
						{
							Cless_DumpData ("%s", "DDA (offline) OK");
							Cless_DumpData_DumpNewLine();
						}
					}
					else
						Cless_DumpData ("%s", "No EXEC_FUNCTIONS_INFO");

					if (Cless_Common_RetrieveInfo (pDataStruct, TAG_QUICKPASS_ODA_FAIL, &pInfo)) // TAG_QUICKPASS_ODA_FAIL
					{
						Cless_DumpData ("%s", "ODA failed");
					}
					Cless_DumpData_DumpNewLine();

					Cless_DumpData_DumpCloseOutputDriver();
				}
				Cless_DumpData_DumpSharedBufferWithTitle (pDataStruct, "DATABASE DUMP.");

			}
		}
	}

	// Additional possible processing :
	//	- Perform an online authorisation if necessary
	//	- Save the transaction in the batch if transaction is accepted
	//	- Perform CVM processing if necessary


	// Transaction is completed, clear QuickPass kernel transaction data
	QuickPass_Clear ();


	return (nResult);
}


//! \brief Modify several parameters before to use it.
//! \param[in] pShareStruct Share buffer given and modified by EntryPoint.
//! \param[in/out] pAidParameters parameters found to manage AID.
//!	\return
//!		- QuickPass kernel result.
void Cless_QuickPass_AidRelatedData(T_SHARED_DATA_STRUCT * pSharedStruct, T_SHARED_DATA_STRUCT * pAidParameters) {
	// Local variables
	int cr;
	int nPositionElement;
	unsigned long ulReadLengthElement, ulAidReadLengthElement, ulCVMLimitLength;
	unsigned char *pReadValueElement;
	unsigned char *pAidReadValueElement;
	unsigned char *pCVMLimit;
	unsigned long ulTransactionAmount;
	unsigned long ulCVMLimit;
	char aidData[lenAID + 1];

	// Protection against bad parameters
	if ((pSharedStruct == NULL) || (pAidParameters == NULL))
		return;

	//////////////////////////////////////////////////////////
	// TAG_PAYWAVE_TERMINAL_TRANSACTION_QUALIFIERS management
	//////////////////////////////////////////////////////////

	// Warning, Entry Point modify the TAG_PAYWAVE_TERMINAL_TRANSACTION_QUALIFIERS parameters
	// First bit managed by EntryPoint : byte 2, bit 8 : Online cryptogram required / not required
	// Second bit managed by EntryPoint : byte 2, bit 7 : CVM required / not required

	// Find the TTQ from EP buffer
	nPositionElement = SHARED_EXCHANGE_POSITION_NULL;
	cr = GTL_SharedExchange_FindNext (pSharedStruct, &nPositionElement, TAG_PAYWAVE_TERMINAL_TRANSACTION_QUALIFIERS, &ulReadLengthElement, (const unsigned char **)&pReadValueElement);
	if (cr != STATUS_SHARED_EXCHANGE_OK)  // If tag with AID informations found
		return;	// No TTQ found, what can we made ?

	// Find the TTQ in pAidParameters structure
	nPositionElement = SHARED_EXCHANGE_POSITION_NULL;
	cr = GTL_SharedExchange_FindNext (pAidParameters, &nPositionElement, TAG_PAYWAVE_TERMINAL_TRANSACTION_QUALIFIERS, &ulAidReadLengthElement, (const unsigned char **)&pAidReadValueElement);
	if (cr != STATUS_SHARED_EXCHANGE_OK)  // If tag with AID informations found
		return;	// No TTQ found, what can we made ?

	pAidReadValueElement[1] &= 0x3F; 						// 00XX XXXX erase Online cryptogram required & CVM required bit
	pAidReadValueElement[1] |= pReadValueElement[1] & 0xC0; // Take Online cryptogram required & EntryPoint CVM bit value


	///---------------------------------------------------------- ADDED IMPLEMENTATION --------------------------------------------------------

	ulTransactionAmount = Cless_Fill_GiveAmount();

	// Find the TAG_EP_CLESS_TRANSACTION_LIMIT in pAidParameters structure
	nPositionElement = SHARED_EXCHANGE_POSITION_NULL;
	cr = GTL_SharedExchange_FindNext (pAidParameters, &nPositionElement, TAG_EP_CLESS_CVM_REQUIRED_LIMIT ,&ulCVMLimitLength, (const unsigned char **)&pCVMLimit);
	if (cr != STATUS_SHARED_EXCHANGE_OK)  // If tag with AID informations found
		return;	// No AID option found, what can we made ?

	GTL_Convert_DcbNumberToUl(pCVMLimit, &ulCVMLimit, ulCVMLimitLength);

	///modify only for Credit and Quasi Credi UPI cards
	memset(aidData, 0, sizeof(aidData));
	mapGet(traAID, aidData, mapDatLen(traAID));

	//Amount is less than CVM limit
	if (ulTransactionAmount < ulCVMLimit) { //Amount is Below the CVM Limit

		if ((strncmp(aidData, "A000000333010102", 16) == 0) || (strncmp(aidData, "A000000333010103", 16) == 0)) {
			mapPut(traUPI_Credit_QuasiCredit,"1", 1);
		}else {
			mapPut(traUPI_Credit_QuasiCredit,"3", 1); //DEBIT amount UPI
		}
	} else if (ulTransactionAmount > ulCVMLimit) { //Amount is Below the CVM Limit

		if ((strncmp(aidData, "A000000333010102", 16) == 0) || (strncmp(aidData, "A000000333010103", 16) == 0)) {
			mapPut(traUPI_Credit_QuasiCredit,"2", 1);
		}else {
			mapPut(traUPI_Credit_QuasiCredit,"4", 1);  //DEBIT amount UPI
		}
	}
}


//! \brief Add the transaction record in the batch file.
//! \param[in] pSharedData Shared buffer to be used to get all the record data.
//! \note For QuickPass, \a pSharedData is the one returned by the QuickPass kernel on the DoTransaction function.

static void __Cless_QuickPass_AddRecordToBatch (T_SHARED_DATA_STRUCT * pSharedData)
{
	int merchLang;
	MSGinfos tDisplayMsg;

	if (!Cless_Batch_AddTransactionToBatch (pSharedData))
	{
		merchLang = PSQ_Give_Language();
		GTL_Traces_TraceDebug ("__Cless_QuickPass_AddRecordToBatch : Save transaction in batch failed");
		Cless_Term_Read_Message(STD_MESS_BATCH_ERROR, merchLang, &tDisplayMsg);
		Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tDisplayMsg, LEDSOFF);
		Helper_RefreshScreen(WAIT, HELPERS_MERCHANT_SCREEN);
	}
}

//! \brief Allows to known if error is due to amount greater than TAG_EP_CLESS_TRANSACTION_LIMIT
//!	\return
//!		- TRUE if error is due to amount greater than TAG_EP_CLESS_TRANSACTION_LIMIT.
//!     - FALSE else.
int Cless_QuickPass_IsQVSDCNotAllowedForAmount(void)
{
	return (gs_ErrorAmountTooHigth);
}

//! \brief Memorise if amount is greater than TAG_EP_CLESS_TRANSACTION_LIMIT.
//! \param[in] TRUE/FALSE error due to amount greater than TAG_EP_CLESS_TRANSACTION_LIMIT.
void Cless_QuickPass_SetQVSDCNotAllowedForAmount(int nAmountGreaterThanTransactionLimit)
{
	gs_ErrorAmountTooHigth = nAmountGreaterThanTransactionLimit;
}

