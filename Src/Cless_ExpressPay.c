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

const unsigned char ARC_00[]={0x30, 0x30};
const unsigned char ARC_08[]={0x30, 0x38};
const unsigned char ARC_10[]={0x31, 0x30};
const unsigned char ARC_11[]={0x31, 0x31};
const unsigned char ARC_01[]={0x30, 0x31}; // Expresspay 3.1 section 22.3.7 "Issuer Requested Referral"
const unsigned char ARC_02[]={0x30, 0x32}; // Expresspay 3.1 section 22.3.7 "Issuer Requested Referral"

// Period of time the contactless reader field is deactivated between the two taps of the Mobile CVM (Time in milliseconds)
#define	EXPRESSPAY_DOUBLE_TAP_TIME_DEFAULT		0x5DC		// Default value: : 1,5 seconds
#define	EXPRESSPAY_DOUBLE_TAP_TIME_MIN			0x3E8		// Min value: : 1 second
#define	EXPRESSPAY_DOUBLE_TAP_TIME_MAX			0xBB8		// Max value: : 3 seconds

#define C_TIME_100MS					10

#define SAMPLE_EXPRESSPAY_GET_ALL_DATA_SIZE				(32 * 1024)	// Default size for shared buffer used for getting all data


/////////////////////////////////////////////////////////////////
//// Global data definition /////////////////////////////////////

static unsigned char gs_bMobileCVMperformed = FALSE;		// Global variable that indicates if Mobile CVM has been performed and if transaction must be restarted.
static unsigned char gs_bIsExpresspayGuiMode = FALSE;		// Set to TRUE if the Expresspay GUI must be used
static unsigned char gs_IsAnotherPaymentRequested = FALSE;	// Set to TRUE if another payment is requested (Expresspay 3.1 section 18.2.3)


/////////////////////////////////////////////////////////////////
//// Static functions definition ////////////////////////////////

static int __Cless_ExpressPay_AddExpressPaySpecificData (T_SHARED_DATA_STRUCT * pDataStruct);
static int __Cless_ExpressPay_RetreiveCvmToApply (T_SHARED_DATA_STRUCT * pResultDataStruct, unsigned char * pCvm);
static T_Bool __Cless_ExpressPay_OnlinePinManagement (T_SHARED_DATA_STRUCT * pStructureForOnlineData, int nCardHolderLang);

static void __Cless_ExpressPay_GetDoubleTapTime (unsigned long *pDoubleTapTime);
static void __Cless_ExpressPay_CloseFieldBefore2ndTap (unsigned long *pDoubleTapTime);
static void __Cless_ExpressPay_SetTxnRestartedIndicator (T_SHARED_DATA_STRUCT *pTransactionData);


/////////////////////////////////////////////////////////////////
//// Functions //////////////////////////////////////////////////


//! \brief Unset the Mobile CVM global variable.

void Cless_ExpressPay_UnsetMobileCVM (void)
{
	gs_bMobileCVMperformed = FALSE;
}


//! \brief Get the Mobile CVM global variable.

int Cless_ExpressPay_IsMobileCVMInProgress (void)
{
	return ((int) gs_bMobileCVMperformed);
}


//! \brief Get the ExpressPay CVM to perform.
//! \param[out] pCvm Retreived transaction CVM :
//!		- \a EXPRESSPAY_CVM_NO_CVM No CVM to be performed.
//!		- \a EXPRESSPAY_CVM_SIGNATURE if signature shall be performed.
//!		- \a EXPRESSPAY_CVM_ONLINE_PIN if online PIN shall be performed.
//! \return
//!		- \ref TRUE if correctly retreived.
//!		- \ref FALSE if an error occured.

static int __Cless_ExpressPay_RetreiveCvmToApply (T_SHARED_DATA_STRUCT * pResultDataStruct, unsigned char * pCvm)
{
	int nResult = TRUE;
	int nPosition, cr;
	unsigned long ulReadLength;
	const unsigned char * pReadValue;
	
	nPosition = SHARED_EXCHANGE_POSITION_NULL;
	* pCvm = EXPRESSPAY_CVM_NO_CVM; // Default result
	
	cr = GTL_SharedExchange_FindNext(pResultDataStruct, &nPosition, TAG_EXPRESSPAY_TRANSACTION_CVM, &ulReadLength, &pReadValue);
	
	if (cr != STATUS_SHARED_EXCHANGE_OK) {
		nResult = FALSE;
		goto End;
	}
	
	// Get the transaction outcome
	* pCvm = pReadValue[0];
	
End:	
	return (nResult);
}


//! \brief Fill buffer with specific ExpressPay DRL for transaction.
//! \param[out] pDataStruct Shared exchange structure filled with the specific ExpressPay data.
//! \return
//!		- \ref TRUE if correctly performed.
//!		- \ref FALSE if an error occured.

static int __Cless_ExpressPay_AddExpressPayDrlData (T_SHARED_DATA_STRUCT * pDataStruct)
{
	TLV_TREE_NODE pDrlSets, pDrl, pDrlTag;
	T_SHARED_DATA_STRUCT *tSetsDrlStruct = NULL, *tDrlStruct = NULL;
	int cr, nResult = TRUE;


	tDrlStruct = GTL_SharedExchange_InitLocal(128);
	if(tDrlStruct == NULL)
	{
		GTL_Traces_TraceDebug("__Cless_ExpressPay_AddExpressPayDrlData : GTL_SharedExchange_InitLocal failed");
		nResult = FALSE;
		goto End;
	}

	// First get Defaults DRL (if present) and add it to the kernel's data
	pDrl = TlvTree_Find(pTreeCurrentParam, TAG_EXPRESSPAY_DEFAULT_DYNAMIC_READER_LIMITS, 0);
	if (pDrl != 0)
	{
		pDrlTag = TlvTree_GetFirstChild(pDrl);

		while(pDrlTag != NULL)
		{
			// Add all floor limits from the Default DRL
			cr = GTL_SharedExchange_AddTag(tDrlStruct, TlvTree_GetTag(pDrlTag), TlvTree_GetLength(pDrlTag), (const unsigned char *)TlvTree_GetData(pDrlTag));
			if (cr != STATUS_SHARED_EXCHANGE_OK)
			{
				GTL_Traces_TraceDebug("__Cless_ExpressPay_AddExpressPayDrlData : Unable to add tag (%lx) from Default DRL in shared buffer (cr=%02x)", (long)TlvTree_GetTag(pDrlTag), cr);
				nResult = FALSE;
				goto End;
			}

			pDrlTag = TlvTree_GetNext(pDrlTag);
		}

		// Add the TAG_EXPRESSPAY_DEFAULT_DYNAMIC_READER_LIMITS tag to the kernel's data
		cr = GTL_SharedExchange_AddTag(pDataStruct, TAG_EXPRESSPAY_DEFAULT_DYNAMIC_READER_LIMITS, tDrlStruct->ulDataLength, tDrlStruct->nPtrData);
		if (cr != STATUS_SHARED_EXCHANGE_OK)
		{
			GTL_Traces_TraceDebug("__Cless_ExpressPay_AddExpressPayDrlData : Unable to add TAG_EXPRESSPAY_DEFAULT_DYNAMIC_READER_LIMITS in shared buffer (cr=%02x)", cr);
			nResult = FALSE;
			goto End;
		}
	}

	GTL_SharedExchange_DestroyLocal(tDrlStruct);
	tDrlStruct = NULL;

	// Then get DRL sets(if present) and add it to the kernel's data
	tSetsDrlStruct = GTL_SharedExchange_InitLocal(1024);
	if(tSetsDrlStruct == NULL)
	{
		GTL_Traces_TraceDebug("__Cless_ExpressPay_AddExpressPayDrlData : GTL_SharedExchange_InitLocal failed");
		nResult = FALSE;
		goto End;
	}

	pDrlSets = TlvTree_Find(pTreeCurrentParam, TAG_EXPRESSPAY_SETS_OF_DYNAMIC_READER_LIMITS, 0);
	if (pDrlSets != 0)
	{
		pDrl = TlvTree_GetFirstChild(pDrlSets);

		while(pDrl != NULL)
		{
			tDrlStruct = GTL_SharedExchange_InitLocal(128);
			if(tDrlStruct == NULL)
			{
				GTL_Traces_TraceDebug("__Cless_ExpressPay_AddExpressPayDrlData : GTL_SharedExchange_InitLocal failed");
				nResult = FALSE;
				goto End;
			}

			pDrlTag = TlvTree_GetFirstChild(pDrl);

			while(pDrlTag != NULL)
			{
				// Add all floor limits from the DRL
				cr = GTL_SharedExchange_AddTag(tDrlStruct, TlvTree_GetTag(pDrlTag), TlvTree_GetLength(pDrlTag), (const unsigned char *)TlvTree_GetData(pDrlTag));
				if (cr != STATUS_SHARED_EXCHANGE_OK)
				{
					GTL_Traces_TraceDebug("__Cless_ExpressPay_AddExpressPayDrlData : Unable to add tag (%lx) from DRL set in shared buffer (cr=%02x)", (long)TlvTree_GetTag(pDrlTag), cr);
					nResult = FALSE;
					goto End;
				}

				pDrlTag = TlvTree_GetNext(pDrlTag);
			}

			// Add the TAG_EXPRESSPAY_SET_OF_DYNAMIC_READER_LIMITS tag the structure
			GTL_SharedExchange_AddTag(tSetsDrlStruct, TAG_EXPRESSPAY_SET_OF_DYNAMIC_READER_LIMITS, tDrlStruct->ulDataLength, tDrlStruct->nPtrData);
			if (cr != STATUS_SHARED_EXCHANGE_OK)
			{
				GTL_Traces_TraceDebug("__Cless_ExpressPay_AddExpressPayDrlData : Unable to add TAG_EXPRESSPAY_SET_OF_DYNAMIC_READER_LIMITS in shared buffer (cr=%02x)", cr);
				nResult = FALSE;
				goto End;
			}

			GTL_SharedExchange_DestroyLocal(tDrlStruct);
			tDrlStruct = NULL;

			pDrl = TlvTree_GetNext(pDrl);
		}

		// Add the TAG_EXPRESSPAY_SETS_OF_DYNAMIC_READER_LIMITS tag to the kernel's data
		cr = GTL_SharedExchange_AddTag(pDataStruct, TAG_EXPRESSPAY_SETS_OF_DYNAMIC_READER_LIMITS, tSetsDrlStruct->ulDataLength, tSetsDrlStruct->nPtrData);
		if (cr != STATUS_SHARED_EXCHANGE_OK)
		{
			GTL_Traces_TraceDebug("__Cless_ExpressPay_AddExpressPayDrlData : Unable to add TAG_EXPRESSPAY_SETS_OF_DYNAMIC_READER_LIMITS in shared buffer (cr=%02x)", cr);
			nResult = FALSE;
			goto End;
		}
	}

End:
	if(tDrlStruct != NULL)
		GTL_SharedExchange_DestroyLocal(tDrlStruct);
	if(tSetsDrlStruct != NULL)
		GTL_SharedExchange_DestroyLocal(tSetsDrlStruct);

	return(nResult);
}


//! \brief Fill buffer with specific ExpressPay for transaction.
//! \param[out] pDataStruct Shared exchange structure filled with the specific ExpressPay data.
//! \return
//!		- \ref TRUE if correctly performed.
//!		- \ref FALSE if an error occured.

static int __Cless_ExpressPay_AddExpressPaySpecificData (T_SHARED_DATA_STRUCT * pDataStruct)
{
	int cr, nResult;
	object_info_t ObjectInfo;
	T_KERNEL_TRANSACTION_FLOW_CUSTOM sTransactionFlowCustom;
	unsigned char StepInterruption[KERNEL_PAYMENT_FLOW_STOP_LENGTH];// Bit field to stop payment flow,
																	// if all bit set to 0 => no stop during payment process
									                                // if right bit set to 1 : stop after payment step number 1
	unsigned char StepCustom[KERNEL_PAYMENT_FLOW_CUSTOM_LENGTH]; 	// Bit field to custom payment flow,
																	// if all bit set to 0 => no stop during payment process
									                                // if right bit set to 1 : stop after payment step number 1
	
	if (pDataStruct == NULL)
	{
		GTL_Traces_TraceDebug("__Cless_ExpressPay_AddExpressPaySpecificData : Invalid Input data");
		nResult = FALSE;
		goto End;
	}
	
    // Init parameteters
	memset(StepInterruption, 0, sizeof(StepInterruption)); // Default Value : not stop on process
	memset(StepCustom, 0, sizeof(StepCustom)); // Default Value : not stop on process
	ObjectGetInfo(OBJECT_TYPE_APPLI, ApplicationGetCurrent(), &ObjectInfo);
	nResult = TRUE;

	
	// Add a tag for Do_Txn management
	cr = GTL_SharedExchange_AddTag(pDataStruct, TAG_KERNEL_PAYMENT_FLOW_STOP, KERNEL_PAYMENT_FLOW_STOP_LENGTH, (const unsigned char *)StepInterruption);
	if (cr != STATUS_SHARED_EXCHANGE_OK)
	{
		GTL_Traces_TraceDebug("__Cless_ExpressPay_AddExpressPaySpecificData : Unable to add TAG_KERNEL_PAYMENT_FLOW_STOP in shared buffer (cr=%02x)", cr);
		nResult = FALSE;
		goto End;
	}
		
	
	// Customize steps
	ADD_STEP_CUSTOM(STEP_EXPRESSPAY_REMOVE_CARD,StepCustom); // To do GUI when ExpressPay card has been read
	ADD_STEP_CUSTOM(STEP_EXPRESSPAY_GET_CERTIFICATE, StepCustom); // To provide the CA key data for ODA

	//ADD_STEP_CUSTOM(STEP_EXPRESSPAY_MEMBERSHIP_DATA_PROCESSING, StepCustom); // To perform Membership-Related Data Processing
	
	if (Cless_IsBlackListPresent())	
		ADD_STEP_CUSTOM(STEP_EXPRESSPAY_EXCEPTION_FILE_GET_DATA, StepCustom); // To check if PAN is in the blacklist
	
	memcpy ((void*)&sTransactionFlowCustom, (void*)StepCustom, KERNEL_PAYMENT_FLOW_CUSTOM_LENGTH);
	sTransactionFlowCustom.usApplicationType = ObjectInfo.application_type; // Kernel will call this application for customisation 
	sTransactionFlowCustom.usServiceId = SERVICE_CUSTOM_KERNEL; // Kernel will call SERVICE_CUSTOM_KERNEL service id for customisation 

    cr = GTL_SharedExchange_AddTag(pDataStruct, TAG_KERNEL_PAYMENT_FLOW_CUSTOM, sizeof(T_KERNEL_TRANSACTION_FLOW_CUSTOM), (const unsigned char *)&sTransactionFlowCustom);
	if (cr != STATUS_SHARED_EXCHANGE_OK)
	{
		GTL_Traces_TraceDebug("__Cless_ExpressPay_AddExpressPaySpecificData : Unable to add TAG_KERNEL_PAYMENT_FLOW_CUSTOM in shared buffer (cr=%02x)", cr);
		nResult = FALSE;
		goto End;
	}
	
	// Add DRL data
	__Cless_ExpressPay_AddExpressPayDrlData(pDataStruct);

End:
	return (nResult);
}


//! \brief Perform the Online PIN input and encipher PIN.
//! \param[out] pStructureForOnlineData Data returned by the kernel in which the enciphered online PIN would be added.
//! \return
//!		- \ref TRUE if correctly performed.
//!		- \ref FALSE if an error occured.

static T_Bool __Cless_ExpressPay_OnlinePinManagement (T_SHARED_DATA_STRUCT * pStructureForOnlineData, int nCardHolderLang)
{
	T_SHARED_DATA_STRUCT * pDataRequest;
	T_Bool nResult = B_TRUE;
	int cr;
	int nPosition;
	const unsigned char * pPan;
	unsigned long ulPanLength;
	const unsigned char * pAmount;
	unsigned char aucDymmyMsg[] = "";
	unsigned long ulAmountLength;
	unsigned long ulAmount = 0;
	BUFFER_SAISIE buffer_saisie;
	MSGinfos tMsg;
	
	pDataRequest = GTL_SharedExchange_InitShared (128);

	if (pDataRequest != NULL)
	{
		// Clear shared buffer
		GTL_SharedExchange_ClearEx (pDataRequest, FALSE);

		// Indicate tag to be requested
		GTL_SharedExchange_AddTag (pDataRequest, TAG_EMV_APPLI_PAN, 0, NULL);
		GTL_SharedExchange_AddTag (pDataRequest, TAG_EMV_AMOUNT_AUTH_NUM, 0, NULL);

		cr = ExpressPay3_GetData (pDataRequest);

		if (cr != KERNEL_STATUS_OK)
		{
			GTL_Traces_TraceDebug("__Cless_ExpressPay_OnlinePinManagement : An error occured when getting tags from the ExpressPay kernel (cr=%02x)", cr);
			nResult = B_FALSE;
			goto End;
		}

		// Tags have been got (if present), get the PAN
		nPosition = SHARED_EXCHANGE_POSITION_NULL;
		if (GTL_SharedExchange_FindNext (pDataRequest, &nPosition, TAG_EMV_APPLI_PAN, &ulPanLength, &pPan) != STATUS_SHARED_EXCHANGE_OK)
		{
			GTL_Traces_TraceDebug ("__Cless_ExpressPay_OnlinePinManagement : Missing PAN for Online PIN");
			nResult = B_FALSE;
			goto End;
		}

		// Get the transaction amount (numeric)
		nPosition = SHARED_EXCHANGE_POSITION_NULL;
		if (GTL_SharedExchange_FindNext (pDataRequest, &nPosition, TAG_EMV_AMOUNT_AUTH_NUM, &ulAmountLength, &pAmount) != STATUS_SHARED_EXCHANGE_OK)
		{
			GTL_Traces_TraceDebug ("__Cless_ExpressPay_OnlinePinManagement : Missing Amount for Online PIN");
			nResult = B_FALSE;
			goto End;
		}
		
		// Convert amount
		GTL_Convert_DcbNumberToUl(pAmount, &ulAmount, ulAmountLength);
		
		// Warning, erase display must be made only if Pin input will be made on customer screen
		if (Cless_IsPinpadPresent())
		{
			// Display dummy message to erase display
			tMsg.message = (char*)aucDymmyMsg;
			tMsg.coding = _ISO8859_;
			tMsg.file = GetCurrentFont();

			Helper_DisplayTextCustomer(ERASE, HELPERS_CUSTOMER_LINE_1, &tMsg, Cless_ALIGN_CENTER, LEDSOFF);
		}
		
		// Request online PIN entry
		cr = Cless_PinManagement_OnLinePinManagement ((unsigned char*)pPan, 1, ulAmount, 30000, 10000, nCardHolderLang, &buffer_saisie);

		if (cr == INPUT_PIN_ON)
		{
			cr = GTL_SharedExchange_AddTag(pStructureForOnlineData, TAG_SAMPLE_ENCIPHERED_PIN_CODE, buffer_saisie.nombre , (const unsigned char *)buffer_saisie.donnees);

			if (cr != STATUS_SHARED_EXCHANGE_OK)
			{
				GTL_Traces_TraceDebug ("__Cless_ExpressPay_OnlinePinManagement : Unable to add TAG_SAMPLE_ENCIPHERED_PIN_CODE in the shared buffer (cr = %02x)", cr);
				nResult = B_FALSE;
				goto End;
			}
		}
		else if (cr == CANCEL_INPUT)
		{
				GTL_Traces_TraceDebug ("__Cless_ExpressPay_OnlinePinManagement : input pin Cancelled");
				nResult = B_NON_INIT;
				goto End;
		}
	}

End:
	// Destroy the shared buffer if created
	if (pDataRequest != NULL)
		GTL_SharedExchange_DestroyShare (pDataRequest);

	return (nResult);
}


//! \brief Manage the debug mode for ExpressPay kernel
//! \param[in] bActivate \a TRUE to activate the debug features. \a FALSE to deactivate features.

void Cless_ExpressPay_DebugActivation (int bActivate)
{
	T_SHARED_DATA_STRUCT * pSharedStructure;
	int nResult;
	unsigned char ucDebugMode = 0x00;
	
	if (bActivate)
		ucDebugMode = KERNEL_DEBUG_MASK_TRACES | KERNEL_DEBUG_MASK_APDU;
	
	pSharedStructure = GTL_SharedExchange_InitShared(256);
	
	if (pSharedStructure != NULL)
	{
		nResult = GTL_SharedExchange_AddTag(pSharedStructure, TAG_KERNEL_DEBUG_ACTIVATION, 1, &ucDebugMode);
		
		if (nResult != STATUS_SHARED_EXCHANGE_OK)
		{
			GTL_Traces_TraceDebug("Cless_ExpressPay_DebugActivation : Unable to add TAG_KERNEL_DEBUG_ACTIVATION (nResult = %02x)", nResult);
		}
		else
		{
			nResult = ExpressPay3_DebugManagement(pSharedStructure);
			
			if (nResult != KERNEL_STATUS_OK)
			{
				GTL_Traces_TraceDebug("Cless_ExpressPay_DebugActivation : Error occured during ExpressPay Debug activation (nResult = %02x)", nResult);
			}
		}
		
		// Destroy the shared buffer
		GTL_SharedExchange_DestroyShare(pSharedStructure);
	}
}



//! \brief Perform the ExpressPay kernel customisation.
//! \param[in,out] pSharedData Shared buffer used for customisation.
//! \param[in] ucCustomisationStep Step to be customised.
//! \return
//!		- \a KERNEL_STATUS_CONTINUE always.

int Cless_ExpressPay_CustomiseStep (T_SHARED_DATA_STRUCT * pSharedData, const unsigned char ucCustomisationStep)
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
	
    switch (ucCustomisationStep) // Steps to customise
    {
    case (STEP_EXPRESSPAY_REMOVE_CARD):
		HelperRemoveCardSequence(pSharedData);
		GTL_SharedExchange_ClearEx (pSharedData, FALSE);
		nResult = KERNEL_STATUS_CONTINUE;
		break;
	
	case (STEP_EXPRESSPAY_GET_CERTIFICATE):
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
			GTL_Traces_TraceDebug ("Cless_ExpressPay_CustomiseStep : CUPAR_GetCaKeyData failed");
		}
		
		nResult = KERNEL_STATUS_CONTINUE;
		break;
	
	case (STEP_EXPRESSPAY_EXCEPTION_FILE_GET_DATA):
		// Get the PAN
		nPosition = SHARED_EXCHANGE_POSITION_NULL;
		if (GTL_SharedExchange_FindNext (pSharedData, &nPosition, TAG_EMV_APPLI_PAN, &ulPanLength, &pPan) != STATUS_SHARED_EXCHANGE_OK)
		{
			// Pan parameters is missing, we cannot check BlackList
			GTL_Traces_TraceDebug ("Cless_ExpressPay_CustomiseStep : PAN is missing for exception file checking");
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
			// Add TAG_KERNEL_PAN_IN_BLACK_LIST tag in the exchange buffer to indicate ExpressPay kernel the PAN is in the black list
			if (GTL_SharedExchange_AddTag (pSharedData, TAG_KERNEL_PAN_IN_BLACK_LIST, 1, &bPanInExceptionFile) != STATUS_SHARED_EXCHANGE_OK)
			{
				GTL_SharedExchange_ClearEx (pSharedData, FALSE);
				GTL_Traces_TraceDebug ("Cless_ExpressPay_CustomiseStep : Unable to add TAG_KERNEL_PAN_IN_BLACK_LIST in the shared buffer");
			}
		}

		nResult = KERNEL_STATUS_CONTINUE;
		break;

	case (STEP_EXPRESSPAY_MEMBERSHIP_DATA_PROCESSING):
			// Get the Membership Product Identifier
			nPosition = SHARED_EXCHANGE_POSITION_NULL;
			if (GTL_SharedExchange_FindNext (pSharedData, &nPosition, TAG_EXPRESSPAY_MEMBERSHIP_PRODUCT_IDENTIFIER,  &ulReadLength, (const unsigned char **)&pReadValue) != STATUS_SHARED_EXCHANGE_OK)
			{
				// Membership Product Identifier is missing
				GTL_Traces_TraceDebug ("Cless_ExpressPay_CustomiseStep : Membership Product Identifier is missing");
			}

			// Get the Product Membership Number
			nPosition = SHARED_EXCHANGE_POSITION_NULL;
			if (GTL_SharedExchange_FindNext (pSharedData, &nPosition, TAG_EXPRESSPAY_PRODUCT_MEMBERSHIP_NUMBER,  &ulReadLength, (const unsigned char **)&pReadValue) != STATUS_SHARED_EXCHANGE_OK)
			{
				// Product Membership Number is missing
				GTL_Traces_TraceDebug ("Cless_ExpressPay_CustomiseStep : Product Membership Number is missing");
			}

			nResult = KERNEL_STATUS_CONTINUE;
			break;

	// Other customisation steps could be defined if necessary
		
    default:
    	GTL_Traces_TraceDebug ("Cless_ExpressPay_CustomiseStep : Step to customise (unknown) = %02x\n", ucCustomisationStep);
    	break;
    }
    
    return (nResult);
}



#ifdef __TELIUM3__
word g_MainTask = 0;
word g_DoTransactionTaskIdTask = 0;
t_topstack *g_DoTransactionTask = NULL;
T_SHARED_DATA_STRUCT * g_pDataStruct = NULL;
int g_DoTransactionCr = KERNEL_STATUS_INTERNAL_ERROR;

static word Cless_ExpressPay_DoTransactionTask (void)
{
	do
	{
		g_DoTransactionCr = ExpressPay3_DoTransaction(g_pDataStruct);
		Telium_SignalEvent(g_MainTask, E_USER_EVENT);

		Telium_Ttestall(USER_EVENT_START, 0);
	} while(1);
	return 0;
}

static int Cless_ExpressPay_StartDoTransaction (T_SHARED_DATA_STRUCT * pDataStruct)
{
	g_DoTransactionCr = KERNEL_STATUS_INTERNAL_ERROR;
	if (g_DoTransactionTask == NULL)
	{
		// Get the main task id
		g_MainTask = Telium_CurrentTask();

		Telium_ClearEvents(USER_EVENT);

		// Launch the task
		g_pDataStruct = pDataStruct;
		g_DoTransactionTask = Telium_Fork (&Cless_ExpressPay_DoTransactionTask, NULL, -1);

		// The task cannot be created
		if (g_DoTransactionTask != NULL)
		{
			g_DoTransactionTaskIdTask = Telium_GiveNoTask(g_DoTransactionTask);
			return OK;
		}
		else
		{
			g_DoTransactionTaskIdTask = 0;
			return KO;
		}
	}
	else
	{
		g_pDataStruct = pDataStruct;
		Telium_SignalEvent(g_DoTransactionTaskIdTask, E_USER_EVENT_START);
		return OK;
	}
}
#endif

int g_nCardHolderLang = 0;
word g_RemovalTaskIdTask = 0;
t_topstack *g_RemovalTask = NULL;

static word Cless_ExpressPay_RemovalDisplayTask (void)
{
	int nEvent;
	MSGinfos tMsg;

	do
	{
		nEvent = 0;
		Telium_ClearEvents(USER_EVENT_DO_NOT_DISPLAY);

		// If the card has not been removed after 1 second, a message 'Please Remove Card' will be displayed
		nEvent = Telium_Ttestall(USER_EVENT_DO_NOT_DISPLAY, 100);
		if ((nEvent & USER_EVENT_DO_NOT_DISPLAY) == 0)
		{
			// Event not received -> display the message
			Cless_Term_Read_Message(STD_MESS_REMOVE_CARD, g_nCardHolderLang, &tMsg);
			Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
			Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
		}

		Telium_Ttestall(USER_EVENT_START_DISPLAY, 0);
	} while(1);
	return 0;
}

static int Cless_ExpressPay_StartRemovalDisplay (int nCardHolderLang)
{
	if (g_RemovalTask == NULL)
	{
		Telium_ClearEvents(USER_EVENT_START_DISPLAY);
		Telium_ClearEvents(USER_EVENT_DO_NOT_DISPLAY);

		g_nCardHolderLang = nCardHolderLang;

		// Launch the task
		g_RemovalTask = Telium_Fork (&Cless_ExpressPay_RemovalDisplayTask, NULL, -1);

		if (g_RemovalTask != NULL)
		{
			g_RemovalTaskIdTask = Telium_GiveNoTask(g_RemovalTask);
			return OK;
		}
		else
		{
			// The task cannot be created
			g_RemovalTaskIdTask = 0;
			return KO;
		}
	}
	else
	{
		g_nCardHolderLang = nCardHolderLang;
		Telium_SignalEvent(g_RemovalTaskIdTask, E_USER_EVENT_START_DISPLAY);
		return OK;
	}
}

//! \brief Calls the ExpressPay kernel to perform the transaction.
//! \param[in] pDataStruct Data buffer to be filled and used for ExpressPay transaction.
//!	\return
//!		- ExpressPay kernel result.

int Cless_ExpressPay_PerformTransaction (T_SHARED_DATA_STRUCT * pDataStruct)
{
	unsigned char bLoopMode;
	int nResult = CLESS_CR_MANAGER_END;
	int cr, nPosition;
	unsigned long ulReadLength;
	const unsigned char * pReadValue;
	unsigned char ucCvm;
	unsigned char auCustomerDisplayAvailable;
	unsigned char * pInfo;
	int merchLang, nCardHolderLang;
	T_Bool bContinueWithOnlineAuthorisation = B_TRUE;
	MSGinfos tMsg;
	unsigned char bUnableToGoOnline = FALSE;
	unsigned char bCommunicationResult;
	T_SHARED_DATA_STRUCT * pGetAllDataStruct;
	unsigned long ulDoubleTapTime;		// Double Tap time (in milliseconds)
	long remainingTime;


	// Loop mode information
	bLoopMode = Cless_Menu_IsTransactionLoopOn();
	
	merchLang = Cless_Term_GiveLangNumber((unsigned char *)GetManagerLanguage());
	auCustomerDisplayAvailable = Helper_IsClessCustomerDisplayAvailable();
	
	// Indicate ExpressPay kernel is going to be used (for customisation purposes)
	Cless_Customisation_SetUsedPaymentScheme (CLESS_SAMPLE_CUST_EXPRESSPAY);
	
	if (!__Cless_ExpressPay_AddExpressPaySpecificData(pDataStruct))
	{
	    GTL_Traces_TraceDebug("Cless_ExpressPay_PerformTransaction : Cless_Fill_ExpressPayTransaction failed\n");
	}
	else
	{		
		// Debug purpose : if you need to dump the data provided to the kernel
		///Cless_DumpData_DumpSharedBuffer(pDataStruct, 0);

		// If Mobile CVM has been performed, an indicator is set to identify that a transaction has been Restarted.
		if(gs_bMobileCVMperformed)
			__Cless_ExpressPay_SetTxnRestartedIndicator(pDataStruct);
		
#ifndef __TELIUM3__
		cr = ExpressPay3_DoTransaction(pDataStruct);
#else
		// Launch a task  that calls the ExpressPay kernel
		cr = Cless_ExpressPay_StartDoTransaction(pDataStruct);
		if (cr == OK)
		{
			int nEvent = 0;
			int bWaitEndTr = TRUE;
			do
			{
				nEvent = Telium_Ttestall (USER_EVENT, 0);
				if ((nEvent & USER_EVENT) != 0)
				{
					// It indicates the payment is done
					bWaitEndTr = FALSE;
				}
			} while(bWaitEndTr);
		}
		cr = g_DoTransactionCr;
#endif

		// Debug purpose : if you need to dump the data provided by the kernel
		if (Cless_Menu_IsTransactionDataDumpingOn())
			Cless_DumpData_DumpSharedBufferWithTitle (pDataStruct, "DATA RECORD");
		
		// Get prefered card language (defaulty returned by the ExpressPay kernel (if present in the kernel database)
		nCardHolderLang = merchLang; // By default, cardholder language is set to default language
		if (Cless_Common_RetrieveInfo (pDataStruct, TAG_KERNEL_SELECTED_PREFERED_LANGUAGE, &pInfo))
			nCardHolderLang = Cless_Term_GiveLangNumber(pInfo);

		// Get the TAG_EXPRESSPAY_TRANSACTION_CVM to identify the CVM to be performed :
		// 	- EXPRESSPAY_CVM_NO_CVM (0x01) : "No CVM" method has been applied.
		// 	- EXPRESSPAY_CVM_SIGNATURE (0x02) : "Signature" method has been applied.
		// 	- EXPRESSPAY_CVM_ONLINE_PIN (0x04) : "Online PIN" method has been applied.
		if (!__Cless_ExpressPay_RetreiveCvmToApply (pDataStruct, &ucCvm))
			ucCvm = EXPRESSPAY_CVM_NO_CVM;

		switch (cr)
		{
		case (KERNEL_STATUS_OFFLINE_APPROVED):

			// Warning : on iWE280 Cless field must be stopped before using the printer
			ClessEmv_CloseDriver();

			if (auCustomerDisplayAvailable) // If a customer display is available
			{
				Cless_Term_Read_Message(STD_MESS_OFFLINE_APPROVED, merchLang, &tMsg);
				Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, LEDSOFF);
				if (ucCvm == EXPRESSPAY_CVM_SIGNATURE)
				{
					Cless_Term_Read_Message(STD_MESS_SIGNATURE_CHECK_L1, merchLang, &tMsg);
					Helper_DisplayTextMerchant(NO_ERASE, HELPERS_MERCHANT_LINE_4, &tMsg, NOLEDSOFF);
				}
				Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
			}
			Cless_Term_Read_Message(STD_MESS_OFFLINE_APPROVED, nCardHolderLang, &tMsg);
			Helper_DisplayTextCustomer(ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, LEDSOFF);
			if (ucCvm == EXPRESSPAY_CVM_SIGNATURE)
			{
				Cless_Term_Read_Message(STD_MESS_SIGNATURE_CHECK_L1, nCardHolderLang, &tMsg);
				Helper_DisplayTextCustomer(NO_ERASE, HELPERS_CUSTOMER_LINE_3, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
			}
			Helper_RefreshScreen(WAIT, HELPERS_CUSTOMER_SCREEN);
			
			if (bLoopMode == 0)
            {
				pGetAllDataStruct = GTL_SharedExchange_InitShared(SAMPLE_EXPRESSPAY_GET_ALL_DATA_SIZE);
				if (pGetAllDataStruct != NULL)
				{
					// Get all data
					ExpressPay3_GetAllData(pGetAllDataStruct); // Get all the kernel data to print the receipt

					// Print the receipt
					Cless_Term_Read_Message(STD_MESS_PRINT_RECEIPT, merchLang, &tMsg);
//					if ((Cless_DumpData_DumpGetOutputId() != CUSTOM_OUTPUT_NONE) && ((ucCvm == EXPRESSPAY_CVM_SIGNATURE) || (HelperQuestionYesNo (&tMsg, 30, merchLang))))
//	            		Cless_Receipt_PrintTransaction(pGetAllDataStruct, BATCH_TICKET_AMEX, APPROVED_TICKED, (ucCvm == EXPRESSPAY_CVM_SIGNATURE), nCardHolderLang, WITH_AOSA);


					char responseCode[lenRspCod + 1]; memset(responseCode, 0, sizeof(responseCode));
															strcpy(responseCode, "00");
															mapPut(traRspCod, responseCode,2);

					GTL_SharedExchange_DestroyLocal (pGetAllDataStruct);
					pGetAllDataStruct = NULL;
				}
            }

			break;

		case (KERNEL_STATUS_OFFLINE_DECLINED):

			// Warning : on iWE280 Cless field must be stopped before using the printer
			ClessEmv_CloseDriver();

		if (auCustomerDisplayAvailable) // If a customer display is available
			{
				Cless_Term_Read_Message(STD_MESS_OFFLINE_DECLINED, merchLang, &tMsg);
				Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, LEDSOFF);
				Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
			}
			Cless_Term_Read_Message(STD_MESS_OFFLINE_DECLINED, nCardHolderLang, &tMsg);
			Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, LEDSOFF);
			Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
			HelperErrorSequence (WITHBEEP);
			Helper_WaitUserDisplay();
			break;
		
		case (KERNEL_STATUS_ONLINE_AUTHORISATION):
			if (ucCvm == EXPRESSPAY_CVM_ONLINE_PIN)
			{
				bContinueWithOnlineAuthorisation = __Cless_ExpressPay_OnlinePinManagement (pDataStruct, nCardHolderLang);
			}

			if (bContinueWithOnlineAuthorisation == B_TRUE)
			{
				if (auCustomerDisplayAvailable) // If a customer display is available
				{
					Cless_Term_Read_Message(STD_MESS_ONLINE_REQUEST, merchLang, &tMsg);
					Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, LEDSOFF);
					Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
				}
				Cless_Term_Read_Message(STD_MESS_ONLINE_REQUEST, nCardHolderLang, &tMsg);
				Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, LEDSOFF);
				Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);

				bCommunicationResult = Cless_FinancialCommunication_ManageAuthorisation (pDataStruct);
				
				if (bCommunicationResult)
				{
					// Warning : on iWE280 Cless field must be stopped before using the printer
					// ClessEmv_CloseDriver();

					nPosition = SHARED_EXCHANGE_POSITION_NULL;

					if (GTL_SharedExchange_FindNext(pDataStruct, &nPosition, TAG_EMV_AUTHORISATION_RESPONSE_CODE, &ulReadLength, &pReadValue) == STATUS_SHARED_EXCHANGE_OK) {
						if((memcmp(pReadValue, ARC_00, 2) == 0) || (memcmp(pReadValue, ARC_08, 2) == 0) || (memcmp(pReadValue, ARC_10, 2) == 0) || (memcmp(pReadValue, ARC_11, 2) == 0)) {
							if (auCustomerDisplayAvailable) {// If a customer display is available
								Cless_Term_Read_Message(STD_MESS_ONLINE_APPROVED, merchLang, &tMsg);
								Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, LEDSOFF);
								if (ucCvm == EXPRESSPAY_CVM_SIGNATURE) {
									Cless_Term_Read_Message(STD_MESS_SIGNATURE_CHECK_L1, merchLang, &tMsg);
									Helper_DisplayTextMerchant(NO_ERASE, HELPERS_MERCHANT_LINE_4, &tMsg, NOLEDSOFF);
								}
								Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
							}
							Cless_Term_Read_Message(STD_MESS_ONLINE_APPROVED, nCardHolderLang, &tMsg);
							Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, LEDSOFF);
							if (ucCvm == EXPRESSPAY_CVM_SIGNATURE) {
								Cless_Term_Read_Message(STD_MESS_SIGNATURE_CHECK_L1, nCardHolderLang, &tMsg);
								Helper_DisplayTextCustomer (NO_ERASE, HELPERS_CUSTOMER_LINE_3, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
							}
							Helper_RefreshScreen(WAIT, HELPERS_CUSTOMER_SCREEN);

							if (bLoopMode == 0) {
								pGetAllDataStruct = GTL_SharedExchange_InitShared(SAMPLE_EXPRESSPAY_GET_ALL_DATA_SIZE);
								if (pGetAllDataStruct != NULL) {
									// Get all data
									ExpressPay3_GetAllData(pGetAllDataStruct); // Get all the kernel data to print the receipt

									// Print the receipt
									Cless_Term_Read_Message(STD_MESS_PRINT_RECEIPT, merchLang, &tMsg);
//									if ((Cless_DumpData_DumpGetOutputId() != CUSTOM_OUTPUT_NONE) && ((ucCvm == EXPRESSPAY_CVM_SIGNATURE) || (HelperQuestionYesNo (&tMsg, 30, merchLang))))
//										Cless_Receipt_PrintTransaction(pGetAllDataStruct, BATCH_TICKET_AMEX, APPROVED_TICKED, (ucCvm == EXPRESSPAY_CVM_SIGNATURE), nCardHolderLang, WITH_AOSA);

									GTL_SharedExchange_DestroyLocal (pGetAllDataStruct);
									pGetAllDataStruct = NULL;
								}
							}
						} else if((memcmp(pReadValue, ARC_01, 2) == 0) || (memcmp(pReadValue, ARC_02, 2) == 0)) {
							// Expresspay 3.1 section 22.3.7 "Issuer Requested Referral"
							if (auCustomerDisplayAvailable) {// If a customer display is available
								Cless_Term_Read_Message(STD_MESS_CALL_YOUR_BANK, merchLang, &tMsg);
								Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, LEDSOFF);
								Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
							}
							Cless_Term_Read_Message(STD_MESS_CALL_YOUR_BANK, nCardHolderLang, &tMsg);
							Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, LEDSOFF);
							Helper_RefreshScreen(WAIT, HELPERS_CUSTOMER_SCREEN);
						} else {
							// Authorisation response code do not indicates an Approved transaction
							if (auCustomerDisplayAvailable) { // If a customer display is available

								Cless_Term_Read_Message(STD_MESS_ONLINE_DECLINED, merchLang, &tMsg);
								Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, LEDSOFF);
								Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
							}
							Cless_Term_Read_Message(STD_MESS_ONLINE_DECLINED, nCardHolderLang, &tMsg);
							Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, LEDSOFF);
							Helper_RefreshScreen(WAIT, HELPERS_CUSTOMER_SCREEN);

							if (bLoopMode == 0)
							{
								pGetAllDataStruct = GTL_SharedExchange_InitShared(SAMPLE_EXPRESSPAY_GET_ALL_DATA_SIZE);
								if (pGetAllDataStruct != NULL)
								{
									// Get all data
									ExpressPay3_GetAllData(pGetAllDataStruct); // Get all the kernel data to print the receipt

									// Print the receipt
									Cless_Term_Read_Message(STD_MESS_PRINT_RECEIPT, merchLang, &tMsg);
//									if ((Cless_DumpData_DumpGetOutputId() != CUSTOM_OUTPUT_NONE) && (HelperQuestionYesNo (&tMsg, 30, merchLang)))
//										Cless_Receipt_PrintTransaction(pGetAllDataStruct, BATCH_TICKET_AMEX, DECLINED_TICKED, WITHOUT_SIGNATURE, nCardHolderLang, WITH_AOSA);

									GTL_SharedExchange_DestroyLocal (pGetAllDataStruct);
									pGetAllDataStruct = NULL;
								}
							}
						}
					}
					else
					{
						bUnableToGoOnline = TRUE;
					}
				}
				else
				{
					bUnableToGoOnline = TRUE;
				}

				// Warning : on iWE280 Cless field must be stopped before using the printer
				if(ClessEmv_IsDriverOpened())
					ClessEmv_CloseDriver();

				if(bUnableToGoOnline)
				{
					// If terminal is unable to go online, transaction is declined.
					// Default Action Code match, transaction is decline
					if (auCustomerDisplayAvailable) // If a customer display is available
					{
						Cless_Term_Read_Message(STD_MESS_OFFLINE_DECLINED, merchLang, &tMsg);
						Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, LEDSOFF);
						Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
					}
					Cless_Term_Read_Message(STD_MESS_OFFLINE_DECLINED, nCardHolderLang, &tMsg);
					Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, LEDSOFF);
					Helper_RefreshScreen(WAIT, HELPERS_CUSTOMER_SCREEN);
				}
			}
			else
			{
				if(ClessEmv_IsDriverOpened())
					ClessEmv_CloseDriver();

				if (auCustomerDisplayAvailable) // If a customer display is available
				{
					if (bContinueWithOnlineAuthorisation == B_NON_INIT)
						Cless_Term_Read_Message(STD_MESS_PIN_CANCEL, merchLang, &tMsg);
					else
						Cless_Term_Read_Message(STD_MESS_ONLINE_PIN_ERROR, merchLang, &tMsg);
					Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, LEDSOFF);
					Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
				}
				
#ifndef DISABLE_UNATTENDED
				if (Cless_Unattended_IsPinPadUPPwithUCMC())
				{
					if (bContinueWithOnlineAuthorisation == B_NON_INIT)
						Cless_Term_Read_Message(STD_MESS_PIN_CANCEL, merchLang, &tMsg);
					else
						Cless_Term_Read_Message(STD_MESS_ONLINE_PIN_ERROR, merchLang, &tMsg);

					Helper_DisplayPinText(ERASE, HELPERS_MERCHANT_LINE_3,&tMsg,Cless_ALIGN_CENTER,NOLEDSOFF);
				}
#endif

				Cless_Term_Read_Message(STD_MESS_OFFLINE_DECLINED, nCardHolderLang, &tMsg);
				Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, LEDSOFF);
				Helper_RefreshScreen(WAIT, HELPERS_CUSTOMER_SCREEN);
			}
			break;
		
		case (KERNEL_STATUS_USE_CONTACT_INTERFACE):
			nResult = CLESS_CR_MANAGER_RESTART_WO_CLESS;
			break;
		
		case (KERNEL_STATUS_COMMUNICATION_ERROR):
			nResult = CLESS_CR_MANAGER_RESTART;
			break;

		case (KERNEL_STATUS_MOBILE):
			// ExpressPay 3.1 Terminal Specification Section 18.2.2 Try Again
			// The cardholder is notified to withdraw and/or consult the mobile device (Card)
			if (auCustomerDisplayAvailable) // If a customer display is available
			{
				Cless_Term_Read_Message(STD_MESS_PHONE_INSTRUCTIONS_L1, merchLang, &tMsg);
				Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_2, &tMsg, LEDSOFF);
				Cless_Term_Read_Message(STD_MESS_PHONE_INSTRUCTIONS_L2, merchLang, &tMsg);
				Helper_DisplayTextMerchant(NO_ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, LEDSOFF);
				Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
			}
			Cless_Term_Read_Message(STD_MESS_PHONE_INSTRUCTIONS_L1, nCardHolderLang, &tMsg);
			Helper_DisplayTextCustomer(ERASE, HELPERS_CUSTOMER_LINE_1, &tMsg, Cless_ALIGN_CENTER, LEDSOFF);
			Cless_Term_Read_Message(STD_MESS_PHONE_INSTRUCTIONS_L2, nCardHolderLang, &tMsg);
			Helper_DisplayTextCustomer(NO_ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, LEDSOFF);
			Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);

			// The contactless reader field must be deactivated for a period of time (configurable in the range of one to three seconds, default value to be 1.5 seconds).
			__Cless_ExpressPay_CloseFieldBefore2ndTap(&ulDoubleTapTime);

			// After 1 second (bulletin 09), change the message. The cardholder is notified to re-present the mobile device.
			if (auCustomerDisplayAvailable) // If a customer display is available
			{
				Cless_Term_Read_Message(STD_MESS_REPRESENT_CARD, merchLang, &tMsg);
				Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
				Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
			}
			Cless_Term_Read_Message(STD_MESS_REPRESENT_CARD, nCardHolderLang, &tMsg);
			Helper_DisplayTextCustomer(ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
			Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);

			// Wait the remaining time (ulDoubleTapTime - 1 second) before turning on the contactless field
			remainingTime = (ulDoubleTapTime/10) - (10 * C_TIME_100MS);
			remainingTime -= 80; // remove 800 ms to be sure to be less than 3 seconds
			if(remainingTime <= 0)
				remainingTime = 1; // to avoid infinite Telium_Ttestall(0,0)
			Telium_Ttestall(0, remainingTime);

			// The Dynamic Reader Limits must not be preserved (ExpressPay3_Clear() at the end of the function)
		
			// An indicator is set to identify that a transaction has been Restarted.
			gs_bMobileCVMperformed = TRUE;

			// Turn on the first LED
			TPass_LedsOff(TPASS_LED_2 | TPASS_LED_3 | TPASS_LED_4);
			TPass_LedsOn (TPASS_LED_1);

			//nResult = CLESS_CR_MANAGER_RESTART_DOUBLE_TAP;
			nResult = CLESS_CR_MANAGER_RESTART_NO_MESSAGE_BEFORE_RETRY;

			break;

		case (EXPRESSPAY_STATUS_REQUEST_ANOTHER_PAYMENT):

			Helper_Expresspay_DisplayRequestAnotherPayment();

			Cless_Expresspay_SetAnotherPaymentRequested(TRUE);

			nResult = CLESS_CR_MANAGER_RESTART_NO_MESSAGE_BEFORE_RETRY;
			break;

		case (KERNEL_STATUS_CARD_BLOCKED):
		case (KERNEL_STATUS_APPLICATION_BLOCKED):
		case (KERNEL_STATUS_REMOVE_AID):
			nResult = CLESS_CR_MANAGER_REMOVE_AID;
			break;		

		default: // Error case
			GTL_Traces_TraceDebug ("ExpressPay3_DoTransaction result = %02x", cr);
			if (auCustomerDisplayAvailable) // If a customer display is available
			{
				Cless_Term_Read_Message(STD_MESS_ERROR, merchLang, &tMsg);
				Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, LEDSOFF);
				Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
			}
			Cless_Term_Read_Message(STD_MESS_ERROR, nCardHolderLang, &tMsg);
			Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, LEDSOFF);
			Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
			HelperErrorSequence(WITHBEEP);
			Helper_WaitUserDisplay();
			break;
		}

		// Cless field must be stopped only if we don't try to work with an another AID
		if ((nResult != CLESS_CR_MANAGER_REMOVE_AID) && (nResult != CLESS_CR_MANAGER_RESTART_WO_CLESS) && (nResult != CLESS_CR_MANAGER_RESTART_NO_MESSAGE_BEFORE_RETRY))
		{
			if(ClessEmv_IsDriverOpened())
			{
				// Start a task that will display a message 'Please Remove Card' if the card has not been removed after 1 second,
				Cless_ExpressPay_StartRemovalDisplay(nCardHolderLang);
				// Wait card removal
				ClessEmv_DeselectCard(0, TRUE, TRUE);
				// Send message to not display the message if the card has already been removed (before 1 second)
				Telium_SignalEvent(g_RemovalTaskIdTask, E_USER_EVENT_DO_NOT_DISPLAY);
			}
		}

		// If the transaction does not restart from the begining, set the LEDs into the idle state
		if ((nResult != CLESS_CR_MANAGER_RESTART) && (nResult != CLESS_CR_MANAGER_REMOVE_AID) && (nResult != CLESS_CR_MANAGER_RESTART_NO_MESSAGE_BEFORE_RETRY))
		{
			// Increment 
			Cless_Batch_IncrementTransactionSeqCounter();
		}
	}

	// If activated, dump all the kernel database even if restart
	if (Cless_Menu_IsTransactionDatabaseDumpingOn())
	{
		ExpressPay3_GetAllData(pDataStruct); // Get all the kernel data to print the receipt
		Cless_DumpData_DumpSharedBufferWithTitle (pDataStruct, "DATABASE DUMP");
	}

	// Transaction is completed, clear ExpressPay kernel transaction data
	ExpressPay3_Clear ();
	
	// Return result
	return (nResult);
}



//! \brief Get the double tap time value from the parameter file.
//! \param[out] pDoubleTapTime the double tap time value (in milliseconds).

static void __Cless_ExpressPay_GetDoubleTapTime (unsigned long *pDoubleTapTime)
{
	TLV_TREE_NODE pTime;		// Node for Double Tap Time.
	unsigned char * pValue;
	unsigned int nDataLength;

	pValue = NULL;
	nDataLength = 0;
	
	pTime = TlvTree_Find(pTreeCurrentParam, TAG_SAMPLE_EXPRESSPAY_DOUBLE_TAP_TIME, 0);

	if (pTime != NULL) {
		pValue = TlvTree_GetData(pTime);
		nDataLength = TlvTree_GetLength(pTime);
	}

	if ((pValue != NULL) && (nDataLength != 0))
	{
		GTL_Convert_BinNumberToUl(pValue, pDoubleTapTime, nDataLength);

		if((*pDoubleTapTime < EXPRESSPAY_DOUBLE_TAP_TIME_MIN) || (*pDoubleTapTime > EXPRESSPAY_DOUBLE_TAP_TIME_MAX))
			*pDoubleTapTime = EXPRESSPAY_DOUBLE_TAP_TIME_DEFAULT;
	}
	else
	{
		*pDoubleTapTime = EXPRESSPAY_DOUBLE_TAP_TIME_DEFAULT;
	}
}


//! \brief Deactivate the contactless field for 1 second between the two taps of the Mobile CVM.

static void __Cless_ExpressPay_CloseFieldBefore2ndTap (unsigned long *pDoubleTapTime)
{

	__Cless_ExpressPay_GetDoubleTapTime(pDoubleTapTime);
			
	// Stop Cless Field
	ClessEmv_CloseDriver();

	// Perform a double bip (600 ms)
	HelperErrorSequence (WITHBEEP);

	// Wait (400 ms)
	Telium_Ttestall(0, 4 * C_TIME_100MS);
}


//! \brief Add a tag in the transaction data to indicate to the Expresspay kernel that the transaction has been restarted.
//! \param[in/out] pTransactionData Transaction data.

static void __Cless_ExpressPay_SetTxnRestartedIndicator (T_SHARED_DATA_STRUCT *pTransactionData)
{
	int ret;
	unsigned char ucTxnRestarted;

	ucTxnRestarted = 0x01;
	
	ret = GTL_SharedExchange_AddTag(pTransactionData, TAG_EXPRESSPAY_MOBILE_TRANSACTION_RESTARTED, 1, &ucTxnRestarted);
	if (ret != STATUS_SHARED_EXCHANGE_OK)
		GTL_Traces_TraceDebug("__Cless_ExpressPay_SetTxnRestartedIndicator: Unable to add TAG_EXPRESSPAY_MOBILE_TRANSACTION_RESTARTED in shared buffer (ret=%02x)", ret);
}


//! \brief Check in the parameter file if the GUI mode to use is Expresspay.

void Cless_Expresspay_SetExpresspayGuiMode (void)
{
	TLV_TREE_NODE pGuiMode;
	unsigned char * pValue;
	unsigned int nDataLength;

	pValue = NULL;
	nDataLength = 0;

	pGuiMode = TlvTree_Find(pTreeCurrentParam, TAG_SAMPLE_EXPRESSPAY_MODE, 0);

	if (pGuiMode != NULL)
	{
		pValue = TlvTree_GetData(pGuiMode);
		nDataLength = TlvTree_GetLength(pGuiMode);

		if ((pValue != NULL) && (nDataLength != 0))
		{
			if (pValue[0] != 0)
			{
				gs_bIsExpresspayGuiMode = TRUE;
				return;
			}
		}
	}

	gs_bIsExpresspayGuiMode = FALSE;
}


//! \brief Check if the Expresspay GUI mode is activated.
//!	\return
//!		- TRUE if Expresspay GUI mode is used.
//!		- FALSE else.

unsigned char Cless_Expresspay_IsExpresspayGuiMode (void)
{
	return (gs_bIsExpresspayGuiMode);
}


//! \brief Set Another Payment Requested variable.
//! \param[in] ucValue TRUE or FALSE

void Cless_Expresspay_SetAnotherPaymentRequested (unsigned char ucValue)
{
	gs_IsAnotherPaymentRequested = ucValue;
}


//! \brief Check if Another Payment is Requested.
//!	\return
//!		- TRUE if Another Payment is Requested.
//!		- FALSE else.

unsigned char Cless_Expresspay_IsAnotherPaymentRequested (void)
{
	return (gs_IsAnotherPaymentRequested);
}
