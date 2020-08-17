/**
 * \file   Cless_DiscoverDPAS.c
 * \brief  Discover DPAS functionality
 *
 * \author ------------------------------------------------------------------------------\n
 * \author INGENICO Technical Software Department\n
 * \author ------------------------------------------------------------------------------\n
 * \author Copyright (c) 2015 - 2015 INGENICO.\n
 * \author 28-32 boulevard de Grenelle 75015 Paris, France.\n
 * \author All rights reserved.\n
 * \author This source program is the property of the INGENICO Company mentioned above\n
 * \author and may not be copied in any form or by any means, whether in part or in whole,\n
 * \author except under license expressly granted by such INGENICO company.\n
 * \author All copies of this program, whether in part or in whole, and\n
 * \author whether modified or not, must display this and all other\n
 * \author embedded copyright and ownership notices in full.\n
 **/


/////////////////////////////////////////////////////////////////
//// Includes ///////////////////////////////////////////////////

#include "Cless_Implementation.h"
#include "Globals.h"

#ifndef DISABLE_OTHERS_KERNELS

/////////////////////////////////////////////////////////////////
//// Macros & preprocessor definitions //////////////////////////

#define SAMPLE_DISCOVER_DPAS_ONLINE_BUFFER_SIZE				2048	// Default size for shared buffer used for online authorisation
#define CPR_SWITCH_INTERFACE_ONLINE_UNABLE					0x80
#define C_CPT_TIME_20S						20



/////////////////////////////////////////////////////////////////
//// Global data definition /////////////////////////////////////

static T_SHARED_DATA_STRUCT * gs_pTransactionDataLog;			// Structure used to log the transaction data


/////////////////////////////////////////////////////////////////
//// Static functions definition ////////////////////////////////

//static int __Cless_DiscoverDPAS_IsReceiptRequired (T_SHARED_DATA_STRUCT * pResultDataStruct, int * pIsReceiptRequired);
//static int __Cless_DiscoverDPAS_RetreiveTransactionOutcome (T_SHARED_DATA_STRUCT * pResultDataStruct, int * pTransactionOutcome);
//static int __Cless_DiscoverDPAS_RetrieveStart (T_SHARED_DATA_STRUCT * pResultDataStruct, int * pStart);
//static int __Cless_DiscoverDPAS_RetreiveUirdMessage (T_SHARED_DATA_STRUCT * pResultDataStruct, int * pUirdMessage);
static int __Cless_DiscoverDPAS_RetreiveCvmToApply (T_SHARED_DATA_STRUCT * pResultDataStruct, unsigned char * pCvm);
static int __Cless_DiscoverDPAS_RetreiveCardType (T_SHARED_DATA_STRUCT * pResultDataStruct, unsigned short * pCardType);

static int __Cless_DiscoverDPAS_AddDiscoverSpecificData (T_SHARED_DATA_STRUCT * pDataStruct);
static T_Bool __Cless_DiscoverDPAS_OnlinePinManagement (T_SHARED_DATA_STRUCT * pStructureForOnlineData, int nCardHolderLang);
static void __Cless_DiscoverDPAS_AddRecordToBatch (T_SHARED_DATA_STRUCT * pSharedData, unsigned short usCardType);
static void __Cless_DiscoverDPAS_GetSelectedPreferedLanguage (int * pCardholderLanguage, int nMerchantLanguage);
static int __Cless_DiscoverDPAS_FillBufferForOnlineAuthorisation (T_SHARED_DATA_STRUCT * pTransactionData, T_SHARED_DATA_STRUCT * pOnlineAuthorisationData);
//static void __Cless_DiscoverDPAS_FormatAmount (unsigned char ucFormat, unsigned char *ucCurrencyLabel, unsigned char ucPosition, const unsigned char * pAmount, unsigned int nAmountLength, unsigned char *pFormattedAmountMessage);
//static void __Cless_DiscoverDPAS_DisplayUirdMsg (T_SHARED_DATA_STRUCT * pDataStruct);
static void __Cless_DiscoverDPAS_DumpSpecificStructure (T_SHARED_DATA_STRUCT * pExchangeStruct, unsigned long ulTag, int bRequestToKernelIfNotPresent);
//static void __Cless_DiscoverDPAS_DumpTransationDataLog (void);
static void __Cless_DiscoverDPAS_DisplayPresentCard (void);
//static void __Cless_DiscoverDPAS_DisplayRemoveCard (T_SHARED_DATA_STRUCT * pDataStruct);
static int __Cless_DiscoverDPAS_AddIssuerScripts (T_SHARED_DATA_STRUCT *pTransactionData, T_SHARED_DATA_STRUCT * pDataStruct);
static int __Cless_DiscoverDPAS_SitchInterfaceOnOnlineFailure (void);
static int __Cless_DiscoverDPAS_WaitClessCard(void);
static int __Cless_DiscoverDPAS_StopClessCard(void);

/////////////////////////////////////////////////////////////////
//// Functions //////////////////////////////////////////////////
/*


//! \brief Dump the Data Record.
//!	\param[in] pDataRecordValue Data Record value.
//! \param[in] ulDataRecordLength Data Record length.

static void __Cless_DiscoverDPAS_DumpDataRecord (unsigned char * pDataRecordValue, unsigned long ulDataRecordLength)
{
	T_SHARED_DATA_STRUCT tDataRecord;

	if (GTL_SharedExchange_InitEx (&tDataRecord, ulDataRecordLength+1, ulDataRecordLength, pDataRecordValue) == STATUS_SHARED_EXCHANGE_OK)
	{
		// Dump the data record
		if (Cless_DumpData_DumpOpenOutputDriver())
		{
			Cless_DumpData ("DATA RECORD");
			Cless_DumpData_DumpNewLine();

			Cless_DumpData_DumpCloseOutputDriver();
		}

		// Dump the shared buffer
		Cless_DumpData_DumpSharedBuffer (&tDataRecord, 0);
	}
	else
	{
		// An error occurred when initialising the structure
		GTL_Traces_TraceDebug ("__Cless_DiscoverDPAS_DumpDataRecord : Data record structure initialisation failed");
	}
}



//! \brief Dump the Discretionary Data.
//!	\param[in] pDiscretionaryDataValue Discretionary Data value.
//! \param[in] ulDiscretionaryDataLength Discretionary Data length.

static void __Cless_DiscoverDPAS_DumpDiscretionaryData (unsigned char * pDiscretionaryDataValue, unsigned long ulDiscretionaryDataLength)
{
	T_SHARED_DATA_STRUCT tDiscretionaryData;

	if (GTL_SharedExchange_InitEx (&tDiscretionaryData, ulDiscretionaryDataLength+1, ulDiscretionaryDataLength, pDiscretionaryDataValue) == STATUS_SHARED_EXCHANGE_OK)
	{
		// Dump the data record
		if (Cless_DumpData_DumpOpenOutputDriver())
		{
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
		GTL_Traces_TraceDebug ("__Cless_DiscoverDPAS_DumpDiscretionaryData : Discretionary data structure initialisation failed");
	}
}
 */

//! \brief Request a tag to the Discover kernel.
//! \param[in,out] pDataRequest Structure already initialised and allocated.
//! \param[in] ulTag tag to be requested.
//! \return
//!	- \a TRUE if correctly required.
//!	- \a FALSE if an error occurred.

static int __Cless_DiscoverDPAS_RequestDataToKernel (T_SHARED_DATA_STRUCT * pDataRequest, unsigned long ulTag)
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
		cr = DiscoverDPAS_GetData (pDataRequest);

		if (cr != KERNEL_STATUS_OK)
		{
			// An error occurred
			GTL_Traces_TraceDebug("__Cless_DiscoverDPAS_RequestDataToKernel : An error occured when getting tag to the Discover kernel (cr=%02x)", cr);
			nResult = FALSE;
		}
	}
	else
	{
		// An error occurred
		GTL_Traces_TraceDebug("__Cless_DiscoverDPAS_RequestDataToKernel : Provided structure is not initialised");
		nResult = FALSE;
	}

	return (nResult);
}



//! \brief Dump a specific Discover tag.
//! \param[in] ulTag Specific tag to be dumped.
//! \param[in] bRequestToKernelIfNotPresent Boolean that indicates if the tag must requested or not to the kernel if it is not present in pExchangeStruct.
//!	- \ref TAG_DISCOVER_DPAS_OUTCOME_PARAMETER_SET
//!	- \ref TAG_DISCOVER_DPAS_USER_INTERFACE_REQUEST_DATA
//!	- \ref TAG_DISCOVER_DPAS_DATA_RECORD
//!	- \ref TAG_DISCOVER_DPAS_DISCRETIONARY_DATA

void __Cless_DiscoverDPAS_DumpSpecificStructure (T_SHARED_DATA_STRUCT * pExchangeStruct, unsigned long ulTag, int bRequestToKernelIfNotPresent)
{
	unsigned long ulReadLength;
	unsigned char * pReadValue;
	T_SHARED_DATA_STRUCT * pRequestData = NULL;
	int nPosition;
	int bTagFound = FALSE;

	if (gs_pTransactionDataLog == NULL)
		gs_pTransactionDataLog = GTL_SharedExchange_InitLocal (10240);

	if (gs_pTransactionDataLog != NULL)
	{
		if (ulTag == TAG_DISCOVER_DPAS_DATA_RECORD)
		{
			// Check if the tag is present in pExchangeStruct
			nPosition = SHARED_EXCHANGE_POSITION_NULL;

			if (GTL_SharedExchange_FindNext (pExchangeStruct, &nPosition, ulTag, &ulReadLength, (const unsigned char **)&pReadValue) != STATUS_SHARED_EXCHANGE_OK)
			{
				if (bRequestToKernelIfNotPresent)
				{
					// Allocate the sharde buffer
					pRequestData = GTL_SharedExchange_InitShared (1024);

					if (pRequestData != NULL)
					{
						// Tag not found, request it to the kernel
						if (__Cless_DiscoverDPAS_RequestDataToKernel (pRequestData, ulTag))
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

/*

//! \brief Dump the Discover transaction data (for debug purpose).

void __Cless_DiscoverDPAS_DumpTransationDataLog (void)
{
	unsigned long ulReadTag, ulReadLength;
	unsigned char * pReadValue;
	int nPosition;
	int nResult;

	if (gs_pTransactionDataLog != NULL)
	{
		// Check if the tag is present in pExchangeStruct
		nPosition = SHARED_EXCHANGE_POSITION_NULL;

		do
		{
			nResult = GTL_SharedExchange_GetNext (gs_pTransactionDataLog, &nPosition, &ulReadTag, &ulReadLength, (const unsigned char **)&pReadValue);

			if (nResult == STATUS_SHARED_EXCHANGE_OK)
			{
				switch (ulReadTag)
				{
				case (TAG_DISCOVER_DPAS_OUTCOME_PARAMETER_SET):
					__Cless_DiscoverDPAS_DumpOps (pReadValue, ulReadLength);
					break;
				case (TAG_DISCOVER_DPAS_USER_INTERFACE_REQUEST_DATA):
					__Cless_DiscoverDPAS_DumpUird (pReadValue, ulReadLength);
					break;
				case (TAG_DISCOVER_DPAS_DATA_RECORD):
					__Cless_DiscoverDPAS_DumpDataRecord (pReadValue, ulReadLength);
					break;
				case (TAG_DISCOVER_DPAS_DISCRETIONARY_DATA):
					__Cless_DiscoverDPAS_DumpDiscretionaryData (pReadValue, ulReadLength);
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


//! \brief Check if a receipt is required or not (read from the Outcome Parameter Set).
//! \param[in] pResultDataStruct Structure containing the C2 kernel output.
//! \param[out] pIsReceiptRequired \a TRUE if the receipt is required, \a FALSE else.
//! \return
//!		- \ref TRUE if correctly retreived.
//!		- \ref FALSE if an error occured.

static int __Cless_DiscoverDPAS_IsReceiptRequired (T_SHARED_DATA_STRUCT * pResultDataStruct, int * pIsReceiptRequired)
{
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
	cr = GTL_SharedExchange_FindNext(pResultDataStruct, &nPosition, TAG_DISCOVER_DPAS_OUTCOME_PARAMETER_SET, &ulReadLength, &pReadValue);

	if (cr != STATUS_SHARED_EXCHANGE_OK)
	{
		GTL_Traces_TraceDebug("__Cless_DiscoverDPAS_IsReceiptRequired : Unable to get the outcome parameter set from the C2 kernel response (cr = %02x)", cr);
		nResult = FALSE;
		goto End;
	}

	// Get the transaction outcome
	if (pIsReceiptRequired != NULL)
 *pIsReceiptRequired = ((pReadValue[DISCOVER_DPAS_OPS_DATA_PRESENCE_BYTE] & DISCOVER_DPAS_OPS_DATA_PRESENCE_MASK_RECEIPT) == DISCOVER_DPAS_OPS_DATA_PRESENCE_MASK_RECEIPT);

End:
	return (nResult);
}



 */


//! \brief Get the CVM to apply (read from the Outcome Parameter Set).
//! \param[in] pResultDataStruct Structure containing the C2 kernel output.
//! \param[out] pCvm Retreived transaction outcome :
//!		- \a DISCOVER_DPAS_CVM_NO_CVM No CVM to be performed.
//!		- \a DISCOVER_DPAS_CVM_SIGNATURE if signature shall be performed.
//!		- \a DISCOVER_DPAS_CVM_ONLINE_PIN if online PIN shall be performed.
//!		- \a DISCOVER_DPAS_CVM_CONFIRMATION_CODE_VERIFIED if if confirmation code has been verified.
//!		- \a DISCOVER_DPAS_CVM_NA if CVM is not applicable to the case.
//! \return
//!		- \ref TRUE if correctly retreived.
//!		- \ref FALSE if an error occured.

static int __Cless_DiscoverDPAS_RetreiveCvmToApply (T_SHARED_DATA_STRUCT * pResultDataStruct, unsigned char * pCvm)
{
	int nResult = TRUE;
	int nPosition, cr;
	unsigned long ulReadLength;
	const unsigned char * pReadValue;

	// Init position
	nPosition = SHARED_EXCHANGE_POSITION_NULL;

	// Init output data
	if (pCvm != NULL)
		*pCvm = DISCOVER_DPAS_CVM_NA;

	// Get the Outcome Parameter Set
	cr = GTL_SharedExchange_FindNext(pResultDataStruct, &nPosition, TAG_DISCOVER_DPAS_TRANSACTION_CVM, &ulReadLength, &pReadValue);

	if (cr != STATUS_SHARED_EXCHANGE_OK)
	{
		GTL_Traces_TraceDebug("__Cless_DiscoverDPAS_RetreiveCvmToApply : Unable to get TAG_DISCOVER_DPAS_TRANSACTION_CVM from the kernel response (cr = %02x)", cr);
		nResult = FALSE;
		goto End;
	}

	// Get the CVM to apply
	if (pCvm != NULL)
		*pCvm = *pReadValue;

	End:
	return (nResult);
}



//! \brief Get the card type.
//! \param[in] pResultDataStruct Structure containing the C2 kernel output.
//! \param[out] pCardType Retreived card type
//!		- \a 0 If card type not found.
//!		- \a 0x8501 for MStripe card.
//!		- \a 0x8502 for DPAS card.
//! \return
//!		- \ref TRUE if correctly retreived.
//!		- \ref FALSE if an error occured.

static int __Cless_DiscoverDPAS_RetreiveCardType (T_SHARED_DATA_STRUCT * pResultDataStruct, unsigned short * pCardType)
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



//! \brief Fill buffer with specific Discover for transaction.
//! \param[out] pDataStruct Shared exchange structure filled with the specific Discover data.
//! \return
//!		- \ref TRUE if correctly performed.
//!		- \ref FALSE if an error occured.

static int __Cless_DiscoverDPAS_AddDiscoverSpecificData (T_SHARED_DATA_STRUCT * pDataStruct)
{
	int cr, nResult;
	object_info_t ObjectInfo;
	//unsigned char bMerchantForcedTrOnline = 0x01;
	T_KERNEL_TRANSACTION_FLOW_CUSTOM sTransactionFlowCustom;
	unsigned char StepInterruption[KERNEL_PAYMENT_FLOW_STOP_LENGTH];// Bit field to stop payment flow,
	// if all bit set to 0 => no stop during payment process
	// if right bit set to 1 : stop after payment step number 1
	unsigned char StepCustom[KERNEL_PAYMENT_FLOW_CUSTOM_LENGTH]; 	// Bit field to custom payment flow,
	// if all bit set to 0 => no stop during payment process
	// if right bit set to 1 : stop after payment step number 1

	// Check the input data are correctly provided
	if (pDataStruct == NULL)
	{
		GTL_Traces_TraceDebug("__Cless_DiscoverDPAS_AddDiscoverSpecificData : Invalid Input data");
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
	if (cr != STATUS_SHARED_EXCHANGE_OK)
	{
		GTL_Traces_TraceDebug("__Cless_DiscoverDPAS_AddDiscoverSpecificData : Unable to add TAG_KERNEL_PAYMENT_FLOW_STOP in shared buffer (cr=%02x)", cr);
		nResult = FALSE;
		goto End;
	}


	// Customize steps
	ADD_STEP_CUSTOM(STEP_DISCOVER_DPAS_MSTRIPE_REMOVE_CARD,StepCustom); // To do GUI when MStripe card has been read
	ADD_STEP_CUSTOM(STEP_DISCOVER_DPAS_REMOVE_CARD,StepCustom); // To do GUI when DPAS card has been read
	ADD_STEP_CUSTOM(STEP_DISCOVER_DPAS_ISSUER_SCRIPT_REMOVE_CARD,StepCustom); // To do GUI when Issuer Scripts have been performed.
	ADD_STEP_CUSTOM(STEP_DISCOVER_DPAS_GET_CERTIFICATE, StepCustom); // To provide the CA key data for ODA

	if (Cless_IsBlackListPresent())
		ADD_STEP_CUSTOM(STEP_DISCOVER_DPAS_EXCEPTION_FILE_GET_DATA, StepCustom); // To check if PAN is in the blacklist

	memcpy ((void*)&sTransactionFlowCustom, (void*)StepCustom, KERNEL_PAYMENT_FLOW_CUSTOM_LENGTH);
	sTransactionFlowCustom.usApplicationType = ObjectInfo.application_type; // Kernel will call this application for customisation
	sTransactionFlowCustom.usServiceId = SERVICE_CUSTOM_KERNEL; // Kernel will call SERVICE_CUSTOM_KERNEL service id for customisation

	cr = GTL_SharedExchange_AddTag(pDataStruct, TAG_KERNEL_PAYMENT_FLOW_CUSTOM, sizeof(T_KERNEL_TRANSACTION_FLOW_CUSTOM), (const unsigned char *)&sTransactionFlowCustom);
	if (cr != STATUS_SHARED_EXCHANGE_OK)
	{
		GTL_Traces_TraceDebug("__Cless_DiscoverDPAS_AddDiscoverSpecificData : Unable to add TAG_KERNEL_PAYMENT_FLOW_CUSTOM in shared buffer (cr=%02x)", cr);
		nResult = FALSE;
		goto End;
	}


	// Add data indicating if merchant forced transaction online or not
#if 0
	if (Cless_Menu_IsMerchantForcedOnline())
	{
		cr = GTL_SharedExchange_AddTag(pDataStruct, TAG_DISCOVER_DPAS_INT_MERCHANT_FORCE_ONLINE, 1, &bMerchantForcedTrOnline);

		if (cr != STATUS_SHARED_EXCHANGE_OK)
		{
			GTL_Traces_TraceDebug("__Cless_DiscoverDPAS_AddDiscoverSpecificData : Unable to add TAG_DISCOVER_DPAS_INT_MERCHANT_FORCE_ONLINE in shared buffer (cr=%02x)", cr);
			nResult = FALSE;
			goto End;
		}
	}
#endif

	End:
	return (nResult);
}



//! \brief Perform the Online PIN input and encipher PIN.
//! \param[out] pStructureForOnlineData Data returned by the kernel in which the enciphered online PIN would be added.
//! \return
//!		- \ref TRUE if correctly performed.
//!		- \ref FALSE if an error occured.

static T_Bool __Cless_DiscoverDPAS_OnlinePinManagement (T_SHARED_DATA_STRUCT * pStructureForOnlineData, int nCardHolderLang)
{
	//T_SHARED_DATA_STRUCT * pDataRequest;
	T_Bool nResult = B_TRUE;
	int nPosition, cr;
	const unsigned char * pPan;
	unsigned long ulPanLength;
	const unsigned char * pTrack2Data;
	unsigned long ulTrack2DataLength;
	///const unsigned char * pAmount;
	char aucDymmyMsg[] = "";
	///unsigned long ulAmountLength;
	///unsigned long ulAmount = 0;
	BUFFER_SAISIE buffer_saisie;
	MSGinfos tMsg;
	unsigned long ulReadDataRecordLength;
	unsigned char * pReadDataRecordValue;
	T_SHARED_DATA_STRUCT tDataRecord;


	// Get the DATA RECORD and copy all the data in the structure to be sent to the HOST
	nPosition = SHARED_EXCHANGE_POSITION_NULL;
	cr = GTL_SharedExchange_FindNext (pStructureForOnlineData, &nPosition, TAG_DISCOVER_DPAS_DATA_RECORD, &ulReadDataRecordLength, (const unsigned char **)&pReadDataRecordValue);

	if (cr == STATUS_SHARED_EXCHANGE_OK)
	{
		if (GTL_SharedExchange_InitEx (&tDataRecord, ulReadDataRecordLength, ulReadDataRecordLength, pReadDataRecordValue) != STATUS_SHARED_EXCHANGE_OK)
		{
			GTL_Traces_TraceDebug ("__Cless_DiscoverDPAS_OnlinePinManagement : Unable to init the DATA RECORD structure");
			nResult = B_FALSE;
			goto End;
		}
	}


	// Tags have been got (if present), get the PAN
	nPosition = SHARED_EXCHANGE_POSITION_NULL;
	if (GTL_SharedExchange_FindNext (&tDataRecord, &nPosition, TAG_EMV_APPLI_PAN, &ulPanLength, &pPan) != STATUS_SHARED_EXCHANGE_OK)
	{
		// Maybe it is a MStripe card, so try to extract PAN from Track2 Data
		nPosition = SHARED_EXCHANGE_POSITION_NULL;

		if (GTL_SharedExchange_FindNext (&tDataRecord, &nPosition, TAG_EMV_TRACK_2_EQU_DATA, &ulTrack2DataLength, &pTrack2Data) != STATUS_SHARED_EXCHANGE_OK)
		{
			GTL_Traces_TraceDebug ("__Cless_DiscoverDPAS_OnlinePinManagement : Missing PAN for Online PIN");
			nResult = B_FALSE;
			goto End;
		}
		else
		{
			// Continue with the PAN extracted from the track2 data
			pPan = pTrack2Data;
		}
	}

	// Warning, erase display must be made only if Pin input will be made on customer screen
	if (Cless_IsPinpadPresent())
	{
		// Display dummy message to erase display
		tMsg.message = aucDymmyMsg;
		tMsg.coding = _ISO8859_;
		tMsg.file = GetCurrentFont();

		Helper_DisplayTextCustomer(ERASE, HELPERS_CUSTOMER_LINE_1, &tMsg, Cless_ALIGN_CENTER, LEDSOFF);
	}

	cr = Cless_PinManagement_OnLinePinManagement ((unsigned char*)pPan, 1, 0, 30000, 10000, nCardHolderLang, &buffer_saisie);

	if (cr == INPUT_PIN_ON)
	{
		cr = GTL_SharedExchange_AddTag(pStructureForOnlineData, TAG_SAMPLE_ENCIPHERED_PIN_CODE, buffer_saisie.nombre , (const unsigned char *)buffer_saisie.donnees);

		if (cr != STATUS_SHARED_EXCHANGE_OK)
		{
			GTL_Traces_TraceDebug ("__Cless_DiscoverDPAS_OnlinePinManagement : Unable to add TAG_SAMPLE_ENCIPHERED_PIN_CODE in the shared buffer (cr = %02x)", cr);
			nResult = B_FALSE;
			goto End;
		}
	}
	else if (cr == CANCEL_INPUT)
	{
		GTL_Traces_TraceDebug ("__Cless_DiscoverDPAS_OnlinePinManagement : input pin Cancelled");
		nResult = B_NON_INIT;
		goto End;
	}

	End:
	//	if (pDataRequest != NULL)
	//		GTL_SharedExchange_DestroyShare (pDataRequest);

	return (nResult);
}



//! \brief Manage the debug mode for Discover kernel
//! \param[in] bActivate \a TRUE to activate the debug features. \a FALSE to deactivate features.

void Cless_DiscoverDPAS_DebugActivation (int bActivate)
{
	T_SHARED_DATA_STRUCT * pSharedStructure;
	int nResult;
	unsigned char ucDebugMode = 0x00;

	if (bActivate)
		ucDebugMode = 0x01;

	// Init the shared buffer
	pSharedStructure = GTL_SharedExchange_InitShared(256);

	// Check the structure correctly created
	if (pSharedStructure != NULL)
	{
		// Add the debug information into the structure
		nResult = GTL_SharedExchange_AddTag(pSharedStructure, TAG_KERNEL_DEBUG_ACTIVATION, 1, &ucDebugMode);

		// Check if data correctly added
		if (nResult != STATUS_SHARED_EXCHANGE_OK)
		{
			// An error occurred when adding the tag in the structure
			GTL_Traces_TraceDebug("Cless_DiscoverDPAS_DebugActivation : Unable to add TAG_KERNEL_DEBUG_ACTIVATION (nResult = %02x)", nResult);
		}
		else
		{
			// Data correctly added, call the kernel to activate the debug feature
			nResult = DiscoverDPAS_DebugManagement(pSharedStructure);

			// Check if debug activation correctly performed
			if (nResult != KERNEL_STATUS_OK)
			{
				// An error occurred when enabling the debug feature in the kernel
				GTL_Traces_TraceDebug("Cless_DiscoverDPAS_DebugActivation : Error occured during Discover Debug activation (nResult = %02x)", nResult);
			}
		}

		// Destroy the shared buffer
		GTL_SharedExchange_DestroyShare(pSharedStructure);
	}
}

/*

//! \brief Display a specific message according to the UIRD.
//! \param[in] pDataStruct Shared buffer contianing the UIRD, containing itself the message to be displayed.

static void __Cless_DiscoverDPAS_DisplayUirdMsg (T_SHARED_DATA_STRUCT * pDataStruct)
{
	int auCustomerDisplayAvailable;
	int nMerchantLanguage, nCardHolderLanguage;
	unsigned char * pInfo;
	int nUirdMessage;
	int nMessageNo;
	MSGinfos tMsg;


	// Init languages
	nMerchantLanguage = PSQ_Give_Language();

    if (Cless_Common_RetreiveInfo (pDataStruct, TAG_KERNEL_SELECTED_PREFERED_LANGUAGE, &pInfo))
		nCardHolderLanguage = Cless_Term_GiveLangNumber(pInfo);
	else
		nCardHolderLanguage = nMerchantLanguage;

	// Check if a cardholder device is available
	auCustomerDisplayAvailable = Helper_IsClessCustomerDisplayAvailable();

	// Get the UIRD message to be displayed
	if (!__Cless_DiscoverDPAS_RetreiveUirdMessage (pDataStruct, &nUirdMessage))
		nUirdMessage = DISCOVER_DPAS_UIRD_MESSAGE_ID_NA;

	switch (nUirdMessage)
	{
	case DISCOVER_DPAS_UIRD_MESSAGE_ID_CARD_READ_OK:
		nMessageNo = STD_MESS_REMOVE_CARD;
		break;
	case DISCOVER_DPAS_UIRD_MESSAGE_ID_TRY_AGAIN:
		nMessageNo = STD_MESS_TRY_AGAIN;
		break;
	case DISCOVER_DPAS_UIRD_MESSAGE_ID_APPROVED:
		nMessageNo = STD_MESS_APPROVED_TICKET;
		break;
	case DISCOVER_DPAS_UIRD_MESSAGE_ID_APPROVED_SIGN:
		nMessageNo = STD_MESS_SIGNATURE_OK;
		break;
	case DISCOVER_DPAS_UIRD_MESSAGE_ID_DECLINED:
		nMessageNo = STD_MESS_DECLINED_TICKET;
		break;
	case DISCOVER_DPAS_UIRD_MESSAGE_ID_ERROR_OTHER_CARD:
		nMessageNo = STD_MESS_ERROR;
		break;
	case DISCOVER_DPAS_UIRD_MESSAGE_ID_AUTHORISING_PLEASE_WAIT:
		nMessageNo = STD_MESS_ONLINE_REQUEST;
		break;
	case DISCOVER_DPAS_UIRD_MESSAGE_ID_CLEAR_DISPLAY:
		// Do something!
		break;
	default:
		break;
	}

	// Display the screen indicating the transaction has been sent online for authorisation
	if (auCustomerDisplayAvailable) // If a customer display is available
	{
		Cless_Term_Read_Message(nMessageNo, nMerchantLanguage, &tMsg);
		Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, LEDSOFF);
		Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
	}
	Cless_Term_Read_Message(nMessageNo, nCardHolderLanguage, &tMsg);
	Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, LEDSOFF);
	Helper_RefreshScreen(WAIT, HELPERS_CUSTOMER_SCREEN);


}



static void __Cless_DiscoverDPAS_DisplayRemoveCard (T_SHARED_DATA_STRUCT * pDataStruct)
{
	int auCustomerDisplayAvailable;
	int nMerchantLanguage, nCardHolderLanguage;
	unsigned char * pInfo;
	int nMessageNo;
	MSGinfos tMsg;


	// Init languages
	nMerchantLanguage = PSQ_Give_Language();

    if (Cless_Common_RetreiveInfo (pDataStruct, TAG_KERNEL_SELECTED_PREFERED_LANGUAGE, &pInfo))
		nCardHolderLanguage = Cless_Term_GiveLangNumber(pInfo);
	else
		nCardHolderLanguage = nMerchantLanguage;

	// Check if a cardholder device is available
	auCustomerDisplayAvailable = Helper_IsClessCustomerDisplayAvailable();

	nMessageNo = STD_MESS_REMOVE_CARD;

	// Display the screen indicating the card can be removed
	if (auCustomerDisplayAvailable) // If a customer display is available
	{
		Cless_Term_Read_Message(nMessageNo, nMerchantLanguage, &tMsg);
		Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, LEDSOFF);
		Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
	}
	Cless_Term_Read_Message(nMessageNo, nCardHolderLanguage, &tMsg);
	Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, LEDSOFF);
	Helper_RefreshScreen(WAIT, HELPERS_CUSTOMER_SCREEN);
}
 */
//! \brief Perform the C2 kernel customisation.
//! \param[in,out] pSharedData Shared buffer used for customisation.
//! \param[in] ucCustomisationStep Step to be customised.
//! \return
//!		- \a KERNEL_STATUS_CONTINUE always.

int Cless_DiscoverDPAS_CustomiseStep (T_SHARED_DATA_STRUCT * pSharedData, const unsigned char ucCustomisationStep)
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
	case (STEP_DISCOVER_DPAS_MSTRIPE_REMOVE_CARD):
	case (STEP_DISCOVER_DPAS_REMOVE_CARD):
	case (STEP_DISCOVER_DPAS_ISSUER_SCRIPT_REMOVE_CARD):
	perflog("MG\tpW_CUST\tSTEP_DISCOVER_DPAS_xxx_REMOVE_CARD");
	// Indicate the card shall be removed
	HelperRemoveCardSequence(pSharedData);
	GTL_SharedExchange_ClearEx (pSharedData, FALSE);
	nResult = KERNEL_STATUS_CONTINUE;
	break;


	case (STEP_DISCOVER_DPAS_GET_CERTIFICATE):
				perflog("MG\tpW_CUST\tSTEP_DISCOVER_DPAS_GET_CERTIFICATE");
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
	if (GTL_SharedExchange_FindNext (pSharedData, &nPosition, TAG_DISCOVER_DPAS_INT_RID, &ulReadLength, (const unsigned char **)&pReadValue) == STATUS_SHARED_EXCHANGE_OK)
		memcpy (ucRid, pReadValue, 5);

	// Clear the output structure
	GTL_SharedExchange_ClearEx (pSharedData, FALSE);

	// Get the CA public key data (Modulus, exponent, etc) in the parameters
	if (!Cless_Parameters_GetCaKeyData (pTreeCurrentParam, ucCapkIndex, ucRid, pSharedData))
	{
		// An error occurred when retreiving the CA Public Key data in the parameters
		GTL_Traces_TraceDebug ("Cless_DiscoverDPAS_CustomiseStep : Cless_Parameters_GetCaKeyData failed");
	}

	nResult = KERNEL_STATUS_CONTINUE;
	break;


	case (STEP_DISCOVER_DPAS_EXCEPTION_FILE_GET_DATA):
				perflog("MG\tpW_CUST\tSTEP_DISCOVER_DPAS_EXCEPTION_FILE_GET_DATA");
	// Get the PAN
	nPosition = SHARED_EXCHANGE_POSITION_NULL;
	if (GTL_SharedExchange_FindNext (pSharedData, &nPosition, TAG_EMV_APPLI_PAN, &ulPanLength, &pPan) != STATUS_SHARED_EXCHANGE_OK)
	{
		// Pan parameters is missing, we cannot check BlackList
		GTL_Traces_TraceDebug ("Cless_DiscoverDPAS_CustomiseStep : PAN is missing for excpetion file checking");
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
		// Add TAG_KERNEL_PAN_IN_BLACK_LIST tag in the exchange buffer to indicate Discover kernel the PAN is in the black list
		if (GTL_SharedExchange_AddTag (pSharedData, TAG_KERNEL_PAN_IN_BLACK_LIST, 1, &bPanInExceptionFile) != STATUS_SHARED_EXCHANGE_OK)
		{
			GTL_SharedExchange_ClearEx (pSharedData, FALSE);
			GTL_Traces_TraceDebug ("Cless_DiscoverDPAS_CustomiseStep : Unable to add TAG_KERNEL_PAN_IN_BLACK_LIST in the shared buffer");
		}
	}

	nResult = KERNEL_STATUS_CONTINUE;
	break;

	default:
		perflog("MG\tpW_CUST\tSTEP_DISCOVER_DPAS_ unknown");
		GTL_Traces_TraceDebug ("Cless_DiscoverDPAS_CustomiseStep : Step to customise (unknown) = %02x\n", ucCustomisationStep);
		break;
	}

	perflog("MG\tpW_CUST\tEnd STEP_DISCOVER_DPAS_xxx");
	return (nResult);
}

/*
//! \brief Format a string with the transaction amount.
//! \param[in] ucFormat : the display format
//! \param[in] ucCurrency : transaction currency
//! \param[in] ucPosition : the position of the currency
//! \param[in] ulAmount : the transaction amount
//! \param[out] pFormattedAmountMessage : the formatted message to display

static void __Cless_DiscoverDPAS_FormatAmount (unsigned char ucFormat, unsigned char *ucCurrencyLabel, unsigned char ucPosition, const unsigned char * pAmount, unsigned int nAmountLength, unsigned char *pFormattedAmountMessage)
{
    unsigned char str[64];
    unsigned char * ptr;
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
    GTL_Convert_DcbNumberToAscii((const unsigned char*)pAmount, (char *)ptr, nAmountLength, nAmountLength*2);

    index = 0;
    while ((ptr[index] == '0') && (index < 10))
    	index++;
    ptr+=index;

    nLgStr = strlen((const char *)ptr);
    strcpy ((char *)pFormattedAmountMessage, (const char *)ucCurrencyLabel);
    if (nLgStr > 2)
    {
	    strcat ((char*)pFormattedAmountMessage, " ");
	    strncat ((char*)pFormattedAmountMessage, (char *)ptr, nLgStr - 2);
	    pFormattedAmountMessage[strlen((char *)ucCurrencyLabel) + nLgStr-1] = 0;
	    strcat ((char*)pFormattedAmountMessage, ",");
	    strncat ((char*)pFormattedAmountMessage, (char *)&ptr[nLgStr-2], 2);
	}
	else
	{
	    strcat ((char*)pFormattedAmountMessage, " 0,");
	    strcat ((char*)pFormattedAmountMessage, (char *)ptr);
    }
}



//! \brief Display Balance amount (if available from UIRD).
//! \param[in] pResultDataStruct Structure containing the C2 kernel output.
//! \return
//!		- \ref TRUE if no problem occured.
//!		- \ref FALSE if an error occured.

static int __Cless_DiscoverDPAS_DisplayBalanceAmount(T_SHARED_DATA_STRUCT * pResultDataStruct)
{
	int nResult = TRUE;
	int nPosition, cr;
	unsigned long ulReadLength;
	const unsigned char * pReadValue;
	static unsigned char ucBalanceAmountMessage[64];
	unsigned char ucCurrencyLabel[4];
	unsigned char ucFormat, ucPosition;
	unsigned char * pCurrencyCode = NULL;
	unsigned char * pCurrencyLabel = NULL;
	MSGinfos tDisplayMsgL1, tDisplayMsgL2;

	// Init position
	nPosition = SHARED_EXCHANGE_POSITION_NULL;

	// Get the Outcome Parameter Set
	cr = GTL_SharedExchange_FindNext(pResultDataStruct, &nPosition, TAG_DISCOVER_DPAS_USER_INTERFACE_REQUEST_DATA, &ulReadLength, &pReadValue);

	if (cr != STATUS_SHARED_EXCHANGE_OK)
	{
		GTL_Traces_TraceDebug("__Cless_DiscoverDPAS_DisplayBalanceAmount : Unable to get the UIRD from the C2 kernel response (cr = %02x)", cr);
		nResult = FALSE;
		goto End;
	}

	// Get the Balance value and format the message  to display
	if(pReadValue[DISCOVER_DPAS_UIRD_VALUE_QUALIFIER_BYTE] == DISCOVER_DPAS_UIRD_VALUE_QUALIFIER_BALANCE)
	{
		pCurrencyCode = (unsigned char *) (&pReadValue[DISCOVER_DPAS_UIRD_CURRENCY_CODE_OFFSET]);

		// Get the currency label and exponent from the parameters. If not found, indicate an invalid parameter as the currency code provided is unknown from the application.
// TODO

//		if(!Cless_Parameters_GetCurrencyFromParameters(pCurrencyCode, &pCurrencyLabel, &pCurrencyExponent))
//		{
//			GTL_Traces_TraceDebug ("__Cless_DiscoverDPAS_DisplayBalanceAmount, Currency code provided is unknown from the application (%02x %02x)", pCurrencyCode[0], pCurrencyCode[1]);
//			nResult = FALSE;
//			goto End;
//		}
//
//		// Retrieve the format of the money (currency position, separator, ...)
//		Cless_Parameters_GetCurrencyFormatFromParameters(pCurrencyLabel,&ucFormat,&ucPosition);

		memset(ucCurrencyLabel,0,sizeof(ucCurrencyLabel));
		memcpy(ucCurrencyLabel, pCurrencyLabel, 3);

		// Create a message that contains the balance amount, the currency ...
		memset (ucBalanceAmountMessage, 0, sizeof(ucBalanceAmountMessage));
		__Cless_DiscoverDPAS_FormatAmount (ucFormat, ucCurrencyLabel, ucPosition, &(pReadValue[DISCOVER_DPAS_UIRD_VALUE_OFFSET]), DISCOVER_DPAS_UIRD_VALUE_LENGTH, ucBalanceAmountMessage);

		GetMessageInfos(STD_MESS_AVAILABLE, &tDisplayMsgL1);

		tDisplayMsgL2.coding  = tDisplayMsgL1.coding;
		tDisplayMsgL2.file    = tDisplayMsgL1.file;
		tDisplayMsgL2.message = (char *) ucBalanceAmountMessage;

		// Display messages
		Helper_DisplayTextCustomer(NO_ERASE, HELPERS_CUSTOMER_LINE_2, &tDisplayMsgL1, Cless_ALIGN_CENTER, NOLEDSOFF);
		Helper_DisplayTextCustomer(NO_ERASE, HELPERS_CUSTOMER_LINE_3, &tDisplayMsgL2, Cless_ALIGN_CENTER, NOLEDSOFF);
}

End:
	return (nResult);
}

 */

//! \brief Prepare the data to be sent in the online authorisation message.
//! \param[in] pTransactionData Kernel transaction data (including the Data Record as well as the Discretionary Data. It could also contain the Ciphered Online PIN.
//! \param[out] pOnlineAuthorisationData Filled with the data to be sent in the online authorisation message.
//! \return
//!	- \a TRUE if correctly performed.
//!	- \a FALSE if an error occurred.

static int __Cless_DiscoverDPAS_FillBufferForOnlineAuthorisation (T_SHARED_DATA_STRUCT * pTransactionData, T_SHARED_DATA_STRUCT * pOnlineAuthorisationData)
{
	int nPosition;
	unsigned long ulReadLength;
	unsigned char * pReadValue;
	T_SHARED_DATA_STRUCT tTempStruct;
	int bErrorDetected = FALSE;


	// Check input data consistence
	if ((pTransactionData != NULL) && (pOnlineAuthorisationData != NULL))
	{
		////////////////////////////////////////////////////////////////////////////////////////////////
		// Get the Data Record
		nPosition = SHARED_EXCHANGE_POSITION_NULL;
		if (GTL_SharedExchange_FindNext (pTransactionData, &nPosition, TAG_DISCOVER_DPAS_DATA_RECORD, &ulReadLength, (const unsigned char **)&pReadValue) == STATUS_SHARED_EXCHANGE_OK)
		{
			// Copy data record data in the local buffer
			if (GTL_SharedExchange_InitEx (&tTempStruct, ulReadLength, ulReadLength, pReadValue) == STATUS_SHARED_EXCHANGE_OK)
			{
				if (GTL_SharedExchange_AddSharedBufferContent (pOnlineAuthorisationData, &tTempStruct) != STATUS_SHARED_EXCHANGE_OK)
				{
					GTL_Traces_TraceDebug ("__Cless_DiscoverDPAS_FillBufferForOnlineAuthorisation : Unable to copy the local structure with data record");
					bErrorDetected = TRUE;
				}
			}
			else
			{
				GTL_Traces_TraceDebug ("__Cless_DiscoverDPAS_FillBufferForOnlineAuthorisation : Unable to init the local structure with data record");
				bErrorDetected = TRUE;
			}
		}
		else
		{
			GTL_Traces_TraceDebug ("__Cless_DiscoverDPAS_FillBufferForOnlineAuthorisation : Unable to retreive Data Record");
			bErrorDetected = TRUE;
		}


		////////////////////////////////////////////////////////////////////////////////////////////////
		// Get the ciphered PIN if present
		if (!bErrorDetected)
		{
			// Get the Discretionary Data
			nPosition = SHARED_EXCHANGE_POSITION_NULL;
			if (GTL_SharedExchange_FindNext (pTransactionData, &nPosition, TAG_SAMPLE_ENCIPHERED_PIN_CODE, &ulReadLength, (const unsigned char **)&pReadValue) == STATUS_SHARED_EXCHANGE_OK)
			{
				if (GTL_SharedExchange_AddTag (pOnlineAuthorisationData, TAG_SAMPLE_ENCIPHERED_PIN_CODE, ulReadLength, pReadValue) != STATUS_SHARED_EXCHANGE_OK)
				{
					GTL_Traces_TraceDebug ("__Cless_DiscoverDPAS_FillBufferForOnlineAuthorisation : An error occurred when adding the enciphered PIN in the structure");
					bErrorDetected = TRUE;
				}
			}
		}
	}
	else
	{
		if (pTransactionData == NULL)
			GTL_Traces_TraceDebug ("__Cless_DiscoverDPAS_FillBufferForOnlineAuthorisation : pTransactionData is NULL");

		if (pOnlineAuthorisationData == NULL)
			GTL_Traces_TraceDebug ("__Cless_DiscoverDPAS_FillBufferForOnlineAuthorisation : pOnlineAuthorisationData is NULL");

		bErrorDetected = TRUE;
	}

	return (!bErrorDetected);
}

//! \brief Wait Cless Card.
//! \param[in,out] pSharedData Shared buffer used for customisation.
//! \return
//!		- \a TRUE if appropriate card has been inputed on field.
//!		- \a FALSE else.
static int __Cless_DiscoverDPAS_WaitClessCard(void)
{
	unsigned int bIsOpened, nTimeout, nEvent;
	unsigned int nKey, nNumOfCards;
	int nCpt;
	int nAppropriateCardFound;
	int nResult = KERNEL_STATUS_OK; // Default result


	// Local variables initialization
	nCpt = 0;							// Time counter initialisation (step = 1s)
	nAppropriateCardFound = FALSE;		// Default result : card hasn't been found

	// Cless Field Close allows to restart it later ...
	//ClessEmv_CloseDriver();

	// Card Power-On
	bIsOpened = CL_OK;//ClessEmv_OpenDriver ();  Open contactless field

	if (bIsOpened == CL_OK)
	{
		do{ // Infinite loop to wait a Contacless Card detection
			// Synchrone Detection (bloquante)
			nTimeout = 1; // 1s
			nNumOfCards = 1;
			nResult = ClessEmv_DetectCards(CL_TYPE_AB, &nNumOfCards, nTimeout);

			if (nResult != CL_OK) // If card hasn't been detected
			{
				nTimeout = 100; // 1s
				nEvent = Telium_Ttestall (KEYBOARD, nTimeout);

				if (nEvent == KEYBOARD)
				{
					nKey = Telium_Getchar();
					if (nKey == T_ANN) // If Detection canceled
						nCpt = C_CPT_TIME_20S; // End loop
				}
			}
			nCpt ++;
		}while ((nResult != CL_OK) && (nCpt < C_CPT_TIME_20S)); // For 20s max

		if (nResult == CL_OK) // End if no card detected
		{
			if (ClessEmv_ActiveCard(0, CL_ISO14443_4) == CL_OK)
			{
				nAppropriateCardFound = TRUE;
			}
		}

	}

	return(nAppropriateCardFound);
}

//! \brief Stop Cless field.
//! \return
//!		- \a TRUE if appropriate card has been inputed on field.
//!		- \a FALSE else.
static int __Cless_DiscoverDPAS_StopClessCard(void)
{
	int nFctResult;

	// Card deselection
	nFctResult = ClessEmv_DeselectCard(0, TRUE, FALSE); // Deselect card without wait a card removal
	if (nFctResult != CL_OK)
	{
		// A problem occurs !
		// ..
		GTL_Traces_TraceDebug("__Cless_DiscoverDPAS_StopClessCard: ClessEmv_DeselectCard Failed\n");
	}


	return(TRUE);
}

//! \brief Calls the Discover kernel to perform the transaction.
//! \param[in] pDataStruct Data buffer to be filled and used for Discover transaction.
//!	\return
//!		- Discover kernel result.

int Cless_DiscoverDPAS_PerformTransaction (T_SHARED_DATA_STRUCT * pDataStruct)
{
	unsigned char bLoopMode;
	int nResult = CLESS_CR_MANAGER_END;
	int nPosition, nTransactionOutcome;
	//	int bReceiptRequired = TRUE;
	unsigned long ulReadLength;
	const unsigned char * pReadValue;
	unsigned char ucCvm;
	unsigned char auCustomerDisplayAvailable;
	unsigned char bSaveInBatch = FALSE;
	unsigned short usCardType = 0;
	int merchLang, nCardHolderLang;
	T_Bool bContinueWithOnlineAuthorisation = B_TRUE;
	MSGinfos tMsg;
	T_SHARED_DATA_STRUCT * pOnlineAuthorisationData;
	T_Bool bOnlineUnable = B_FALSE;

	perflog("MG\tpW_CUST\tCless_DiscoverDPAS_PerformTransaction");

	// Indicate Discover kernel is going to be used (for customisation purposes)
	Cless_Customisation_SetUsedPaymentScheme (CLESS_SAMPLE_CUST_DISCOVER_DPAS);

	// Loop mode information
	bLoopMode = Cless_Menu_IsTransactionLoopOn();

	// Get the merchant language
	merchLang = PSQ_Give_Language();

	// Check if an external customer display is available
	auCustomerDisplayAvailable = Helper_IsClessCustomerDisplayAvailable();

	///GTL_Traces_TraceDebug ("TEST CANCEL 6 : before calling the kernel");
	///ttestall (0, 200);

	// Get the Discover
	if (!__Cless_DiscoverDPAS_AddDiscoverSpecificData(pDataStruct))
	{
		GTL_Traces_TraceDebug("Cless_DiscoverDPAS_PerformTransaction : Cless_Fill_DiscoverTransaction failed\n");
	}
	else
	{
		// Debug purpose : if you need to dump the data provided to the kernel
		///Cless_DumpData_DumpSharedBufferWithTitle(pDataStruct, "INPUT DATA");
		// Call the Discover kernel to perform the transaction
		//cr = DiscoverDPAS_DoTransaction(pDataStruct);
		nTransactionOutcome = DiscoverDPAS_DoTransaction(pDataStruct);

		// Debug purpose : if you need to dump the data provided by the kernel
		if (Cless_Menu_IsTransactionDataDumpingOn() && (!Cless_Menu_IsTransactionLoopOn()))
		{
			__Cless_DiscoverDPAS_DumpSpecificStructure (pDataStruct, TAG_DISCOVER_DPAS_DATA_RECORD, FALSE);
		}

		// Get prefered card language (a request shall be done to the Discover kernel as the TAG_KERNEL_SELECTED_PREFERED_LANGUAGE tag is not defaulty returned by the Discover kernel).
		//		Cless_GuiState_SetCardholderLanguage (NULL);

		// Get the transaction outcome
		//	- DISCOVER_DPAS_OPS_STATUS_APPROVED if transaction is approved.
		//	- DISCOVER_DPAS_OPS_STATUS_ONLINE_REQUEST if an online authorisation is requested.
		//	- DISCOVER_DPAS_OPS_STATUS_DECLINED if the transaction is declined.
		//	- DISCOVER_DPAS_OPS_STATUS_SELECT_NEXT if next AID shall be selected.
		//	- DISCOVER_DPAS_OPS_STATUS_TRY_AGAIN transaction shall be restarted from the begining..
		//	- DISCOVER_DPAS_OPS_STATUS_END_APPLICATION if the transaction is terminated.
		//		if (!__Cless_DiscoverDPAS_RetreiveTransactionOutcome (pDataStruct, &nTransactionOutcome))
		//			nTransactionOutcome = DISCOVER_DPAS_OPS_STATUS_END_APPLICATION;


		nCardHolderLang=0x00;		// default to English

		// Get prefered card language (a request shall be done to the Discover DPAS kernel as the TAG_KERNEL_SELECTED_PREFERED_LANGUAGE tag is not returned by the DPAS kernel).
		__Cless_DiscoverDPAS_GetSelectedPreferedLanguage (&nCardHolderLang, merchLang);



		// Get the TAG_DISCOVER_DPAS_OUTCOME_PARAMETER_SET to identify the CVM to be performed
		// 	- DISCOVER_DPAS_CVM_NO_CVM : "No CVM" method has been applied.
		// 	- DISCOVER_DPAS_CVM_SIGNATURE : "Signature" method has been applied.
		// 	- DISCOVER_DPAS_CVM_ONLINE_PIN : "Online PIN" method has been applied.
		//	- DISCOVER_DPAS_CVM_CONFIRMATION_CODE_VERIFIED : confirmation code has been verified.
		//	- DISCOVER_DPAS_CVM_NA : if CVM is not applicable.
		if (!__Cless_DiscoverDPAS_RetreiveCvmToApply (pDataStruct, &ucCvm))
			ucCvm = DISCOVER_DPAS_CVM_NA;

		// Retreive the card type
		if (!__Cless_DiscoverDPAS_RetreiveCardType (pDataStruct, &usCardType))
			usCardType = 0;

		// Check if the receipt has to be printed or not
		//		if (!__Cless_DiscoverDPAS_IsReceiptRequired (pDataStruct, &bReceiptRequired))
		//			bReceiptRequired = FALSE;

		// Get the UIRD message to be displayed
		//		if (!__Cless_DiscoverDPAS_RetreiveUirdMessage (pDataStruct, &nUirdMessage))
		//			nUirdMessage = DISCOVER_DPAS_UIRD_MESSAGE_ID_NA;


		// Additional possible processing :
		//	- Perform an online authorisation if necessary
		//	- Save the transaction in the batch if transaction is accepted
		//	- Perform CVM processing if necessary
		///if ((ucCvm == DISCOVER_DPAS_OPS_CVM_ONLINE_PIN) && (cr == KERNEL_STATUS_OFFLINE_APPROVED))
		///	cr = KERNEL_STATUS_ONLINE_AUTHORISATION;
		if ((ucCvm == DISCOVER_DPAS_CVM_ONLINE_PIN) && (nTransactionOutcome == KERNEL_STATUS_OFFLINE_APPROVED))
			nTransactionOutcome = KERNEL_STATUS_ONLINE_AUTHORISATION;

		///switch (cr)
		switch (nTransactionOutcome)
		{
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		case (KERNEL_STATUS_OFFLINE_APPROVED):
				//case (DISCOVER_DPAS_OPS_STATUS_APPROVED):

					if (ucCvm == DISCOVER_DPAS_CVM_SIGNATURE)
					{
						if (auCustomerDisplayAvailable) // If a customer display is available
						{
							Cless_Term_Read_Message(STD_MESS_SIGNATURE_CHECK_L1, merchLang, &tMsg);
							Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, LEDSOFF);
							Cless_Term_Read_Message(STD_MESS_SIGNATURE_CHECK_L2, merchLang, &tMsg);
							Helper_DisplayTextMerchant(NO_ERASE, HELPERS_MERCHANT_LINE_4, &tMsg, NOLEDSOFF);
							Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
						}
						Cless_Term_Read_Message(STD_MESS_SIGNATURE_CHECK_L1, nCardHolderLang, &tMsg);
						Helper_DisplayTextCustomer(ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, LEDSOFF);
						Cless_Term_Read_Message(STD_MESS_SIGNATURE_CHECK_L2, nCardHolderLang, &tMsg);
						Helper_DisplayTextCustomer(NO_ERASE, HELPERS_CUSTOMER_LINE_3, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
						Helper_RefreshScreen(WAIT, HELPERS_CUSTOMER_SCREEN);
					}
					else
					{
						// Display the message
						if (auCustomerDisplayAvailable) // If a customer display is available
						{
							Cless_Term_Read_Message(STD_MESS_OFFLINE_APPROVED, merchLang, &tMsg);
							Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, LEDSOFF);
							Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
						}
						Cless_Term_Read_Message(STD_MESS_OFFLINE_APPROVED, nCardHolderLang, &tMsg);
						Helper_DisplayTextCustomer(ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, LEDSOFF);
						Helper_RefreshScreen(WAIT, HELPERS_CUSTOMER_SCREEN);
						bSaveInBatch = TRUE;

						char responseCode[lenRspCod + 1]; memset(responseCode, 0, sizeof(responseCode));
						strcpy(responseCode, "00");
						mapPut(traRspCod, responseCode,2);

						//				__Cless_DiscoverDPAS_DisplayBalanceAmount(pDataStruct);
						Helper_RefreshScreen(WAIT, HELPERS_CUSTOMER_SCREEN);

						// Indicate the transaction shall be stored in the batch
						bSaveInBatch = TRUE;
					}

		// Get all data
		DiscoverDPAS_GetAllData(pDataStruct); // Get all the kernel data to print the receipt

		// If loop mode is not activated
		if (!bLoopMode)
		{
			// Read the "Receipt" message
			Cless_Term_Read_Message(STD_MESS_PRINT_RECEIPT, merchLang, &tMsg);

			//				// Print the receipt
			//				if ((Cless_DumpData_DumpGetOutputId() != CUSTOM_OUTPUT_NONE) && ((bReceiptRequired) || (ucCvm == DISCOVER_DPAS_CVM_SIGNATURE) || (HelperQuestionYesNo (&tMsg, 30, merchLang))))
			//					Cless_Receipt_PrintTransaction(pDataStruct, BATCH_TICKET_DISCOVER, APPROVED_TICKED, FALSE, merchLang, WITHOUT_AOSA);
		}

		// If the selected CVM is signature
		if (ucCvm == DISCOVER_DPAS_CVM_SIGNATURE)
		{
			Cless_Term_Read_Message(STD_MESS_SIGNATURE_OK, merchLang, &tMsg);
			if (HelperQuestionYesNo (&tMsg, 30, merchLang))
			{
				if (auCustomerDisplayAvailable) // If a customer display is available
				{
					Cless_Term_Read_Message(STD_MESS_SIGNATURE_CORRECT, merchLang, &tMsg);
					Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, LEDSOFF);
					Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
				}

				Cless_Term_Read_Message(STD_MESS_APPROVED, nCardHolderLang, &tMsg);
				Helper_DisplayTextCustomer(ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, LEDSOFF);
				Helper_RefreshScreen(WAIT, HELPERS_CUSTOMER_SCREEN);
				bSaveInBatch = TRUE;
			}
			else
			{
				if (auCustomerDisplayAvailable) // If a customer display is available
				{
					Cless_Term_Read_Message(STD_MESS_SIGNATURE_INCORRECT, merchLang, &tMsg);
					Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, LEDSOFF);
					Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
				}
				Cless_Term_Read_Message(STD_MESS_OFFLINE_DECLINED, nCardHolderLang, &tMsg);
				Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, LEDSOFF);
				Helper_RefreshScreen(WAIT, HELPERS_CUSTOMER_SCREEN);
			}
		}
		break;

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		case (KERNEL_STATUS_OFFLINE_DECLINED):
				//case (DISCOVER_DPAS_OPS_STATUS_DECLINED):

				if (auCustomerDisplayAvailable) // If a customer display is available
				{
					Cless_Term_Read_Message(STD_MESS_OFFLINE_DECLINED, merchLang, &tMsg);
					Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, LEDSOFF);
					Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
				}
		Cless_Term_Read_Message(STD_MESS_OFFLINE_DECLINED, nCardHolderLang, &tMsg);
		Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, LEDSOFF);
		Helper_RefreshScreen(WAIT, HELPERS_CUSTOMER_SCREEN);
		break;

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		case (KERNEL_STATUS_ONLINE_AUTHORISATION):
				//case (DISCOVER_DPAS_OPS_STATUS_ONLINE_REQUEST):
					// Create a specific buffer for online authorisation
					pOnlineAuthorisationData = NULL;
		pOnlineAuthorisationData = GTL_SharedExchange_InitLocal (SAMPLE_DISCOVER_DPAS_ONLINE_BUFFER_SIZE);

		if (pOnlineAuthorisationData != NULL)
		{
			// Check if the required CVM is Online PIN
			if (ucCvm == DISCOVER_DPAS_CVM_ONLINE_PIN)
			{
				bContinueWithOnlineAuthorisation = __Cless_DiscoverDPAS_OnlinePinManagement (pDataStruct, nCardHolderLang);
			}

			// If PIN online successfully entered
			if (bContinueWithOnlineAuthorisation == B_TRUE)
			{
				// Prepare data for online authorisation
				if (__Cless_DiscoverDPAS_FillBufferForOnlineAuthorisation (pDataStruct, pOnlineAuthorisationData))
				{
					// Display the screen indicating the transaction has been sent online for authorisation
					if (auCustomerDisplayAvailable) // If a customer display is available
					{
						Cless_Term_Read_Message(STD_MESS_ONLINE_REQUEST, merchLang, &tMsg);
						Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, LEDSOFF);
						Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
					}
					Cless_Term_Read_Message(STD_MESS_ONLINE_REQUEST, nCardHolderLang, &tMsg);
					Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, LEDSOFF);
					Helper_RefreshScreen(WAIT, HELPERS_CUSTOMER_SCREEN);

					// Simulate the online authorisation
					if (Cless_FinancialCommunication_ManageAuthorisation (pOnlineAuthorisationData))
					{
						// determine if there are any issuer scripts to be delivered to the card

						if (__Cless_DiscoverDPAS_AddIssuerScripts (pOnlineAuthorisationData, pDataStruct))
						{
							// setup the screen
							__Cless_DiscoverDPAS_DisplayPresentCard();

							if (__Cless_DiscoverDPAS_WaitClessCard() == TRUE)
							{
								// we must send the issuer scripts to the card
								DiscoverDPAS_DoTransactionPostProcessing (pDataStruct);
							}
							__Cless_DiscoverDPAS_StopClessCard();

						}

						// Get the authorisation response code retrieved from the server
						nPosition = SHARED_EXCHANGE_POSITION_NULL;
						///if (GTL_SharedExchange_FindNext(pDataStruct, &nPosition, TAG_EMV_AUTHORISATION_RESPONSE_CODE, &ulReadLength, &pReadValue) == STATUS_SHARED_EXCHANGE_OK)
						if (GTL_SharedExchange_FindNext(pOnlineAuthorisationData, &nPosition, TAG_EMV_AUTHORISATION_RESPONSE_CODE, &ulReadLength, &pReadValue) == STATUS_SHARED_EXCHANGE_OK) {
							// Authorisation code is present, check it is 3030 (OK)
							if ((pReadValue[0] == 0x30) && (pReadValue[1] == 0x30)) {
								// Authorisation code is 3030 (OK), so transaction has been approved by the server
								// Check if the required CVM is signature
								if (ucCvm == DISCOVER_DPAS_CVM_SIGNATURE) {
									// Indicate signature is required on the receipt
									if (auCustomerDisplayAvailable) { // If a customer display is available
										Cless_Term_Read_Message(STD_MESS_SIGNATURE_CHECK_L1, merchLang, &tMsg);
										Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, LEDSOFF);
										Cless_Term_Read_Message(STD_MESS_SIGNATURE_CHECK_L2, merchLang, &tMsg);
										Helper_DisplayTextMerchant(NO_ERASE, HELPERS_MERCHANT_LINE_4, &tMsg, NOLEDSOFF);
										Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
									}
									Cless_Term_Read_Message(STD_MESS_SIGNATURE_CHECK_L1, nCardHolderLang, &tMsg);
									Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, LEDSOFF);
									Cless_Term_Read_Message(STD_MESS_SIGNATURE_CHECK_L2, nCardHolderLang, &tMsg);
									Helper_DisplayTextCustomer (NO_ERASE, HELPERS_CUSTOMER_LINE_3, &tMsg, Cless_ALIGN_CENTER, NOLEDSOFF);
									Helper_RefreshScreen(WAIT, HELPERS_CUSTOMER_SCREEN);
								} else {
									// Signature is not required, indicate the transaction has been approved online
									if (auCustomerDisplayAvailable) { // If a customer display is available
										Cless_Term_Read_Message(STD_MESS_ONLINE_APPROVED, merchLang, &tMsg);
										Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, LEDSOFF);
										Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
									}
									Cless_Term_Read_Message(STD_MESS_ONLINE_APPROVED, nCardHolderLang, &tMsg);
									Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, LEDSOFF);
									Helper_RefreshScreen(WAIT, HELPERS_CUSTOMER_SCREEN);
									bSaveInBatch = TRUE;

									// Transaction is approved, so it shall be added to the batch
									bSaveInBatch = TRUE;
								}

								// Get all the kernel data to print the receipt
								DiscoverDPAS_GetAllData(pDataStruct);

								// Read the "receipt" message
								Cless_Term_Read_Message(STD_MESS_PRINT_RECEIPT, merchLang, &tMsg);

								// If loop mode is not activated, print the receipt
								if (!bLoopMode) {
									//										// Print the receipt
									//										if ((Cless_DumpData_DumpGetOutputId() != CUSTOM_OUTPUT_NONE) && ((bReceiptRequired) || (ucCvm == DISCOVER_DPAS_CVM_SIGNATURE) || (HelperQuestionYesNo (&tMsg, 30, merchLang))))
									//											Cless_Receipt_PrintTransaction(pDataStruct, BATCH_TICKET_DISCOVER, APPROVED_TICKED, FALSE, merchLang, WITHOUT_AOSA);
								}

								// If signature has been required
								if (ucCvm == DISCOVER_DPAS_CVM_SIGNATURE) {
									Cless_Term_Read_Message(STD_MESS_SIGNATURE_OK, merchLang, &tMsg);
									if ((bLoopMode) || (HelperQuestionYesNo (&tMsg, 30, merchLang))) {

										if (auCustomerDisplayAvailable) { // If a customer display is available
											Cless_Term_Read_Message(STD_MESS_SIGNATURE_CORRECT, merchLang, &tMsg);
											Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, LEDSOFF);
											Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
										}
										Cless_Term_Read_Message(STD_MESS_ONLINE_APPROVED, nCardHolderLang, &tMsg);
										Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, LEDSOFF);
										Helper_RefreshScreen(WAIT, HELPERS_CUSTOMER_SCREEN);
										bSaveInBatch = TRUE;
									} else {
										if (auCustomerDisplayAvailable) { // If a customer display is available

											Cless_Term_Read_Message(STD_MESS_SIGNATURE_INCORRECT, merchLang, &tMsg);
											Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, LEDSOFF);
											Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
										}
										Cless_Term_Read_Message(STD_MESS_OFFLINE_DECLINED, nCardHolderLang, &tMsg);
										Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, LEDSOFF);
										Helper_RefreshScreen(WAIT, HELPERS_CUSTOMER_SCREEN);
										bSaveInBatch = FALSE;
									}
								}
							} else {
								// Indicate the transaction has been online declined
								if (auCustomerDisplayAvailable) { // If a customer display is available

									Cless_Term_Read_Message(STD_MESS_ONLINE_DECLINED, merchLang, &tMsg);
									Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, LEDSOFF);
									Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
								}
								Cless_Term_Read_Message(STD_MESS_ONLINE_DECLINED, nCardHolderLang, &tMsg);
								Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, LEDSOFF);
								Helper_RefreshScreen(WAIT, HELPERS_CUSTOMER_SCREEN);
							}
						} else {
							// Indicate the transaction has been online declined (no host answer)
							if (auCustomerDisplayAvailable) // If a customer display is available
							{
								Cless_Term_Read_Message(STD_MESS_ONLINE_DECLINED, merchLang, &tMsg);
								Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, LEDSOFF);
								Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
							}
							Cless_Term_Read_Message(STD_MESS_ONLINE_DECLINED, nCardHolderLang, &tMsg);
							Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, LEDSOFF);
							Helper_RefreshScreen(WAIT, HELPERS_CUSTOMER_SCREEN);

							bOnlineUnable = B_TRUE;
						}
					}
					else
					{
						// Indicate the transaction has been online declined (unable to go online)
						if (auCustomerDisplayAvailable) // If a customer display is available
						{
							Cless_Term_Read_Message(STD_MESS_UNABLE_ON_LINE, merchLang, &tMsg);
							Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, LEDSOFF);
							Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
						}
						Cless_Term_Read_Message(STD_MESS_ONLINE_DECLINED, nCardHolderLang, &tMsg);
						Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, LEDSOFF);
						Helper_RefreshScreen(WAIT, HELPERS_CUSTOMER_SCREEN);

						bOnlineUnable = B_TRUE;
					}
				}
				else
				{
					// Trace the error
					GTL_Traces_TraceDebug ("An error occurred when creating the online data for online authorisation");

					// Indicate an error occurred (transaction is terminated)
					Cless_Term_Read_Message(STD_MESS_ERROR, merchLang, &tMsg);
					Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
					Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);

					HelperErrorSequence(WITHBEEP);
					Helper_WaitUserDisplay();

					bOnlineUnable = B_TRUE;
				}
			}
			else
			{
				if (auCustomerDisplayAvailable) // If a customer display is available
				{
					if (bContinueWithOnlineAuthorisation == B_NON_INIT)
						Cless_Term_Read_Message(STD_MESS_PIN_CANCEL, merchLang, &tMsg);
					else
						Cless_Term_Read_Message(STD_MESS_ONLINE_PIN_ERROR, merchLang, &tMsg);
					Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, LEDSOFF);
					Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
				}
				Cless_Term_Read_Message(STD_MESS_OFFLINE_DECLINED, nCardHolderLang, &tMsg);
				Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, LEDSOFF);
				Helper_RefreshScreen(WAIT, HELPERS_CUSTOMER_SCREEN);
			}

			if (pOnlineAuthorisationData != NULL)
			{
				GTL_SharedExchange_DestroyLocal (pOnlineAuthorisationData);
				pOnlineAuthorisationData = NULL;
			}
		}
		else
		{
			// Trace the error
			GTL_Traces_TraceDebug ("Unable to create a buffer for online authorisation");

			// Indicate an error occurred (transaction is terminated)
			Cless_Term_Read_Message(STD_MESS_ERROR, merchLang, &tMsg);
			Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);
			Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);

			HelperErrorSequence(WITHBEEP);
			Helper_WaitUserDisplay();

			bOnlineUnable = B_TRUE;
		}

		// check whether the transaction should be able to be completed
		// over another interface

		if (bOnlineUnable && __Cless_DiscoverDPAS_SitchInterfaceOnOnlineFailure())
		{
			nResult = CLESS_CR_MANAGER_RESTART_WO_CLESS;
		}
		break;



		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		case (KERNEL_STATUS_COMMUNICATION_ERROR):
				//case (DISCOVER_DPAS_OPS_STATUS_TRY_AGAIN):
					// Transaction shall be restarted from the begining
					HelperErrorSequence(WITHBEEP);
		nResult = CLESS_CR_MANAGER_RESTART_NO_MESSAGE_BEFORE_RETRY;
		break;


		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//		case (DISCOVER_DPAS_OPS_STATUS_SELECT_NEXT):
		case (KERNEL_STATUS_REMOVE_AID):
					// The next AID shall be selected in the candidate list
					nResult = CLESS_CR_MANAGER_REMOVE_AID;
		break;

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		//		case (DISCOVER_DPAS_OPS_STATUS_TRY_ANOTHER_INTERFACE):
		case (KERNEL_STATUS_USE_CONTACT_INTERFACE):
					// Try another interface
					nResult = CLESS_CR_MANAGER_RESTART_WO_CLESS;
		break;

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		case (DISCOVER_DPAS_STATUS_USE_ANOTHER_CARD):
					// Transaction shall be restarted from the beginning
					HelperErrorSequence(WITHBEEP);

		// Indicate the transaction has been online declined
		if (auCustomerDisplayAvailable) // If a customer display is available
		{
			Cless_Term_Read_Message(STD_MESS_USE_OTHER_CARD_L1, merchLang, &tMsg);
			Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_2, &tMsg, LEDSOFF);
			Cless_Term_Read_Message(STD_MESS_USE_OTHER_CARD_L2, merchLang, &tMsg);
			Helper_DisplayTextMerchant(NO_ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, LEDSOFF);
			Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
		}
		Cless_Term_Read_Message(STD_MESS_USE_OTHER_CARD_L1, nCardHolderLang, &tMsg);
		Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, LEDSOFF);
		Cless_Term_Read_Message(STD_MESS_USE_OTHER_CARD_L2, merchLang, &tMsg);
		Helper_DisplayTextMerchant(NO_ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, LEDSOFF);
		Helper_RefreshScreen(WAIT, HELPERS_CUSTOMER_SCREEN);
		// If activated, dump all the kernel database
		if (Cless_Menu_IsTransactionDatabaseDumpingOn())
		{
			DiscoverDPAS_GetAllData(pDataStruct); // Get all the kernel data to print the receipt
			Cless_DumpData_DumpSharedBufferWithTitle (pDataStruct, "DATABASE DUMP");
		}


		nResult = CLESS_CR_MANAGER_RESTART_NO_MESSAGE_BEFORE_RETRY;
		break;

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		case (DISCOVER_DPAS_STATUS_CONSUMER_DEVICE_CVM_FAIL):

					// Transaction shall be restarted from the beginning
					HelperErrorSequence(WITHBEEP);

		// Indicate the transaction has been online declined
		if (auCustomerDisplayAvailable) // If a customer display is available
		{
			Cless_Term_Read_Message(STD_MESS_PHONE_INSTRUCTIONS_L1, merchLang, &tMsg);
			Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_2, &tMsg, LEDSOFF);
			Cless_Term_Read_Message(STD_MESS_PHONE_INSTRUCTIONS_L2, merchLang, &tMsg);
			Helper_DisplayTextMerchant(NO_ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, LEDSOFF);
			Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
		}
		Cless_Term_Read_Message(STD_MESS_PHONE_INSTRUCTIONS_L1, nCardHolderLang, &tMsg);
		Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, LEDSOFF);
		Cless_Term_Read_Message(STD_MESS_PHONE_INSTRUCTIONS_L2, merchLang, &tMsg);
		Helper_DisplayTextMerchant(NO_ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, LEDSOFF);
		Helper_RefreshScreen(WAIT, HELPERS_CUSTOMER_SCREEN);

		nResult = CLESS_CR_MANAGER_RESTART_NO_MESSAGE_BEFORE_RETRY;
		break;




		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		default: // Error case

			GTL_Traces_TraceDebug ("DiscoverDPAS_DoTransaction result = %02X", nTransactionOutcome);

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

			if (auCustomerDisplayAvailable) // If a customer display is available
			{
				Cless_Term_Read_Message(STD_MESS_OFFLINE_DECLINED, merchLang, &tMsg);
				Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, LEDSOFF);
				Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);
			}
			Cless_Term_Read_Message(STD_MESS_OFFLINE_DECLINED, nCardHolderLang, &tMsg);
			Helper_DisplayTextCustomer (ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, LEDSOFF);
			Helper_RefreshScreen(WAIT, HELPERS_CUSTOMER_SCREEN);

			// Deselect the card in the standard error case
			ClessEmv_DeselectCard  (0, TRUE, FALSE);
			break;
		}

		// If transaction shall be save in the batch, save it
		if (bSaveInBatch)
			__Cless_DiscoverDPAS_AddRecordToBatch (pDataStruct, usCardType);

		if ((nResult != CLESS_CR_MANAGER_RESTART) && (nResult != CLESS_CR_MANAGER_RESTART_NO_MESSAGE_BEFORE_RETRY) && (nResult != CLESS_CR_MANAGER_REMOVE_AID) && (nResult != CLESS_CR_MANAGER_RESTART_WO_CLESS))
		{
			// Deselect the card
			ClessEmv_DeselectCard  (0, TRUE, FALSE);
		}

		// If the transaction does not restart from the begining, set the LEDs into the idle state
		if ((nResult != CLESS_CR_MANAGER_RESTART) && (nResult != CLESS_CR_MANAGER_RESTART_NO_MESSAGE_BEFORE_RETRY) && (nResult != CLESS_CR_MANAGER_REMOVE_AID))
		{
			// Increment the transaction sequence counter
			Cless_Batch_IncrementTransactionSeqCounter();

			// If activated, dump all the kernel database
			if (Cless_Menu_IsTransactionDatabaseDumpingOn())
			{
				DiscoverDPAS_GetAllData(pDataStruct); // Get all the kernel data to print the receipt
				Cless_DumpData_DumpSharedBufferWithTitle (pDataStruct, "DATABASE DUMP");
			}
		}
	}

	// Transaction is completed, clear Discover kernel transaction data
	DiscoverDPAS_Clear ();

	// Return result
	return (nResult);
}



//! \brief Add the transaction data record to the batch.
//! \param[in] pSharedData Shared buffer to be used to retrieve the data record.
//! \param[in] usCardType Card type.

static void __Cless_DiscoverDPAS_AddRecordToBatch (T_SHARED_DATA_STRUCT * pSharedData, unsigned short usCardType)
{
	// Tags required for Offline and Online approved transactions
	T_TI_TAG tRequestedTags[] = {TAG_DISCOVER_DPAS_DATA_RECORD};

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

	if (pBatchData != NULL)
	{
		// Request Data Record and Discretionary Data to the kernel
		while (nIndex < NUMBER_OF_ITEMS(tRequestedTags))
		{
			GTL_SharedExchange_AddTag (pSharedData, tRequestedTags[nIndex], 0, NULL);
			nIndex ++;
		}

		// Get the tags from the kernel
		nResult = DiscoverDPAS_GetData (pSharedData);

		if (nResult == KERNEL_STATUS_OK)
		{
			// Get the Data Record
			nPosition = SHARED_EXCHANGE_POSITION_NULL;
			if (GTL_SharedExchange_FindNext (pSharedData, &nPosition, TAG_DISCOVER_DPAS_DATA_RECORD, &ulReadLength, (const unsigned char **)&pReadValue) == STATUS_SHARED_EXCHANGE_OK)
			{
				// Copy data record data in the local buffer
				if (GTL_SharedExchange_InitEx (&tTempStruct, ulReadLength, ulReadLength, pReadValue) == STATUS_SHARED_EXCHANGE_OK)
				{
					if (GTL_SharedExchange_AddSharedBufferContent (pBatchData, &tTempStruct) != STATUS_SHARED_EXCHANGE_OK)
					{
						GTL_Traces_TraceDebug ("__Cless_DiscoverDPAS_AddRecordToBatch : Unable to copy the local structure with data record");
						bErrorDetected = TRUE;
					}
				}
				else
				{
					GTL_Traces_TraceDebug ("__Cless_DiscoverDPAS_AddRecordToBatch : Unable to init the local structure with data record");
					bErrorDetected = TRUE;
				}
			}
			else
			{
				GTL_Traces_TraceDebug ("__Cless_DiscoverDPAS_AddRecordToBatch : Unable to retreive Data Record");
				bErrorDetected = TRUE;
			}

			if (!bErrorDetected)
			{
				if (!Cless_Batch_AddTransactionToBatch (pBatchData))
				{
					GTL_Traces_TraceDebug ("__Cless_DiscoverDPAS_AddRecordToBatch : Save transaction in batch failed");
					bErrorDetected = TRUE;
				}
			}
		}
		else
		{
			GTL_Traces_TraceDebug ("__Cless_DiscoverDPAS_AddRecordToBatch : Unable to get Discover data (nResult = %02x)\n", nResult);
			bErrorDetected = TRUE;
		}

		// Destroy allocated buffer
		GTL_SharedExchange_DestroyLocal (pBatchData);
	}
	else
	{
		// An error occurred when initialising the local data exchange
		GTL_Traces_TraceDebug ("__Cless_DiscoverDPAS_AddRecordToBatch : An error occurred when initialising the local data exchange");
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

//! \brief Create a shared buffer, and requests the selected language to the Discover DPAS kernel.
//! \param[out] pCardholderLanguage Selected language (equal to \a nMerchantLanguage if selected language is not found).
//! \param[in] nMerchantLanguage Merchant language as configured by the manager.

static void __Cless_DiscoverDPAS_GetSelectedPreferedLanguage (int * pCardholderLanguage, int nMerchantLanguage)
{
	int ret;
	T_SHARED_DATA_STRUCT * pStruct;
	unsigned char * pInfo;

	// By default, cardholder language is the same as the merchant language
	*pCardholderLanguage = nMerchantLanguage;

	// Init a shared buffer to get the prefered selected language
	pStruct = GTL_SharedExchange_InitShared (128);

	if (pStruct == NULL)
	{
		// An error occurred when creating the shared buffer
		GTL_Traces_TraceDebug ("__Cless_DiscoverDPAS_GetSelectedPreferedLanguage : An error occurred when creating the shared buffer");
		goto End;
	}

	// Add tag in the shared buffer to request it
	ret = GTL_SharedExchange_AddTag (pStruct, TAG_KERNEL_SELECTED_PREFERED_LANGUAGE, 0, NULL);

	if (ret != STATUS_SHARED_EXCHANGE_OK)
	{
		// An error occurred when adding the TAG_KERNEL_SELECTED_PREFERED_LANGUAGE tag in the structure
		GTL_Traces_TraceDebug ("__Cless_DiscoverDPAS_GetSelectedPreferedLanguage : An error occurred when adding the TAG_KERNEL_SELECTED_PREFERED_LANGUAGE tag in the structure (ret = %02x)", ret);
		goto End;
	}

	// Request data to PayPass kernel
	ret = DiscoverDPAS_GetData (pStruct);

	if (ret != KERNEL_STATUS_OK)
	{
		// An error occurred when getting data from the PayPass kernel
		GTL_Traces_TraceDebug ("__Cless_DiscoverDPAS_GetSelectedPreferedLanguage : An error occurred when getting data from the PayPass kernel (ret = %02x)", ret);
		goto End;
	}

	// Search the tag in the kernel response structure
	if (Cless_Common_RetrieveInfo (pStruct, TAG_KERNEL_SELECTED_PREFERED_LANGUAGE, &pInfo))
		*pCardholderLanguage = Cless_Term_GiveLangNumber(pInfo);

	End:
	// Destroy the shared buffer if created
	if (pStruct != NULL)
		GTL_SharedExchange_DestroyShare (pStruct);
}


//! \brief Show a present card screen.

static void __Cless_DiscoverDPAS_DisplayPresentCard (void)
{
	int merchLang, nCardHolderLang;
	MSGinfos tMsg;

	nCardHolderLang=0x00;		// default to English

	// Init languages
	merchLang = PSQ_Give_Language();

	// Get prefered card language (a request shall be done to the Discover DPAS kernel as the TAG_KERNEL_SELECTED_PREFERED_LANGUAGE tag is not returned by the DPAS kernel).
	__Cless_DiscoverDPAS_GetSelectedPreferedLanguage (&nCardHolderLang, merchLang);


	// show the screen
	Cless_Term_Read_Message(STD_MESS_REPRESENT_CARD, merchLang, &tMsg);
	Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_1, &tMsg, LEDSOFF);
	DisplayTargetCless(PERIPH_DISPLAY);
	Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);

}

//! \brief Add Issuer Scripts received from the authorization in the transaction data sent to the DiscoverDpas kernel.
//! \param[in/out] pTransactionData Transaction data.

static int __Cless_DiscoverDPAS_AddIssuerScripts (T_SHARED_DATA_STRUCT *pTransactionData, T_SHARED_DATA_STRUCT * pDataStruct)
{
	T_SHARED_DATA_STRUCT * pTmpScripts71, * pTmpScripts72;
	T_BER_TLV_DECODE_STRUCT BerTlvStruct;
	int nResult, cr;
	int BytesRead;
	BER_TLV_TAG ReadTag;
	T_TI_LENGTH ReadLength;
	T_TI_VALUE  ReadValue;
	int nScitpsPresent = FALSE;


	pTmpScripts71 = GTL_SharedExchange_InitLocal(512);
	pTmpScripts72 = GTL_SharedExchange_InitLocal(512);

	if ((pTmpScripts71 != NULL) && (pTmpScripts72 != NULL))
	{
		GTL_BerTlvDecode_Init (&BerTlvStruct, pTransactionData->nPtrData, pTransactionData->ulDataLength);

		// Parse Script  T1:71 or 72 L1 V1 ... Tn:71 or 72 Ln Vn
		for (;;)
		{
			//! \brief Parse the next tag in the BER-TLV structure.
			cr = GTL_BerTlvDecode_ParseTlv (&BerTlvStruct, &ReadTag, &ReadLength, (BER_TLV_VALUE*)&ReadValue, (unsigned char)FALSE, &BytesRead);

			if (cr == STATUS_BER_TLV_END)
				break ;

			if (cr == STATUS_BER_TLV_OK)
			{
				if (ReadTag == TAG_EMV_ISSUER_SCRIPT_TEMPLATE_1)
				{
					nResult = GTL_SharedExchange_AddTag(pTmpScripts71, ReadTag, ReadLength, ReadValue);
					if (nResult != STATUS_SHARED_EXCHANGE_OK)
						GTL_Traces_TraceDebug("__Cless_DiscoverDPAS_AddIssuerScripts : Unable to add TAG_EMV_ISSUER_SCRIPT_TEMPLATE_1 (nResult = %02x)", nResult);
				}
				if(ReadTag == TAG_EMV_ISSUER_SCRIPT_TEMPLATE_2)
				{
					nResult = GTL_SharedExchange_AddTag(pTmpScripts72, ReadTag, ReadLength, ReadValue);
					if (nResult != STATUS_SHARED_EXCHANGE_OK)
						GTL_Traces_TraceDebug("__Cless_DiscoverDPAS_AddIssuerScripts : Unable to add TAG_EMV_ISSUER_SCRIPT_TEMPLATE_2 (nResult = %02x)", nResult);
				}
			}
			else
				break; // An error occurs
		} // end of loop about Script parsing


		// Add TAG_DISCOVER_DPAS_ISSUER_SCRIPT_71_LIST and TAG_DISCOVER_DPAS_ISSUER_SCRIPT_72_LIST tags in the data sent to the kernel
		if (pTmpScripts71->ulDataLength)
		{
			nScitpsPresent = TRUE;

			nResult = GTL_SharedExchange_AddTag(pDataStruct, TAG_DISCOVER_DPAS_ISSUER_SCRIPT_71_LIST, pTmpScripts71->ulDataLength, pTmpScripts71->nPtrData);
			if (nResult != STATUS_SHARED_EXCHANGE_OK)
				GTL_Traces_TraceDebug("__Cless_DiscoverDPAS_AddIssuerScripts : Unable to add TAG_DISCOVER_DPAS_ISSUER_SCRIPT_71_LIST (nResult = %02x)", nResult);
		}

		if (pTmpScripts72->ulDataLength)
		{
			nScitpsPresent = TRUE;

			nResult = GTL_SharedExchange_AddTag(pDataStruct, TAG_DISCOVER_DPAS_ISSUER_SCRIPT_72_LIST, pTmpScripts72->ulDataLength, pTmpScripts72->nPtrData);
			if (nResult != STATUS_SHARED_EXCHANGE_OK)
				GTL_Traces_TraceDebug("__Cless_DiscoverDPAS_AddIssuerScripts : Unable to add TAG_DISCOVER_DPAS_ISSUER_SCRIPT_72_LIST (nResult = %02x)", nResult);
		}
	}
	else
		GTL_Traces_TraceDebug("__Cless_DiscoverDPAS_AddIssuerScripts : GTL_SharedExchange_InitLocal error");

	// Destroy the local buffers
	if (pTmpScripts71)
		GTL_SharedExchange_DestroyLocal(pTmpScripts71);
	if (pTmpScripts72)
		GTL_SharedExchange_DestroyLocal(pTmpScripts72);

	return (nScitpsPresent);
}

static int __Cless_DiscoverDPAS_SitchInterfaceOnOnlineFailure (void)
{
	int ret;
	T_SHARED_DATA_STRUCT * pStruct;
	unsigned char * pInfo;


	// Init a shared buffer to get the card processing requirements
	pStruct = GTL_SharedExchange_InitShared (128);

	if (pStruct == NULL)
	{
		// An error occurred when creating the shared buffer
		GTL_Traces_TraceDebug ("__Cless_DiscoverDPAS_SitchInterfaceOnOnlineFailure : An error occurred when creating the shared buffer");
		goto End;
	}

	// Add tag in the shared buffer to request it
	ret = GTL_SharedExchange_AddTag (pStruct, TAG_DISCOVER_DPAS_CARD_PROCESSING_REQUIREMENTS, 0, NULL);

	if (ret != STATUS_SHARED_EXCHANGE_OK)
	{
		// An error occurred when adding the TAG_KERNEL_SELECTED_PREFERED_LANGUAGE tag in the structure
		GTL_Traces_TraceDebug ("__Cless_DiscoverDPAS_SitchInterfaceOnOnlineFailure : An error occurred when adding the TAG_DISCOVER_DPAS_CARD_PROCESSING_REQUIREMENTS tag in the structure (ret = %02x)", ret);
		goto End;
	}

	// Request data to PayPass kernel
	ret = DiscoverDPAS_GetData (pStruct);

	if (ret != KERNEL_STATUS_OK)
	{
		// An error occurred when getting data from the DPAS kernel
		GTL_Traces_TraceDebug ("__Cless_DiscoverDPAS_SitchInterfaceOnOnlineFailure : An error occurred when getting data from the DPAS kernel (ret = %02x)", ret);
		goto End;
	}

	// Search the tag in the kernel response structure
	if (Cless_Common_RetrieveInfo (pStruct, TAG_DISCOVER_DPAS_CARD_PROCESSING_REQUIREMENTS, &pInfo))
	{
		if ((pInfo[1] & CPR_SWITCH_INTERFACE_ONLINE_UNABLE) == CPR_SWITCH_INTERFACE_ONLINE_UNABLE)
		{
			GTL_SharedExchange_DestroyShare (pStruct);
			return (TRUE);
		}
	}


	End:
	// Destroy the shared buffer if created
	if (pStruct != NULL)
		GTL_SharedExchange_DestroyShare (pStruct);
	return (FALSE);
}

#endif // DISABLE_OTHERS_KERNELS

