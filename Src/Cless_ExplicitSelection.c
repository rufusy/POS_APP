
/////////////////////////////////////////////////////////////////
//// Includes ///////////////////////////////////////////////////

#include "Cless_Implementation.h"
#include "Globals.h"


/////////////////////////////////////////////////////////////////
//// Macros & preprocessor definitions //////////////////////////


/////////////////////////////////////////////////////////////////
//// Static function definition /////////////////////////////////

static void __Cless_ExplicitSelection_FormatAmount(unsigned char ucFormat,unsigned char *ucCurrency,unsigned char ucPosition,unsigned long ulAmount,unsigned char *pFormattedAmountMessage);


/////////////////////////////////////////////////////////////////
//// Global data definition /////////////////////////////////////


/////////////////////////////////////////////////////////////////
//// Functions //////////////////////////////////////////////////

//===========================================================================
//! \brief // Format the message that contains the amount, the currency according to ucFormat, ucPosition...  
//! \param[in] ucFormat : the display format
//! \param[in] ucCurrency : transaction currency
//! \param[in] ucPosition : the position of the currency
//! \param[in] ulAmount : the transaction amount
//! \param[out] pFormattedAmountMessage : the formatted message to display
//! \return
//! 	lang code.
//===========================================================================

static void __Cless_ExplicitSelection_FormatAmount(unsigned char ucFormat,unsigned char *ucCurrency,unsigned char ucPosition,unsigned long ulAmount,unsigned char *pFormattedAmountMessage) {
	char            currency[4];
	char            separator1[2];
	char            separator2[2];
	char            position;
	int             exponent;
	unsigned long   divider;
	unsigned long   i;
	char			  amount_format[51];


	position=ucPosition;
	strcpy((char *)currency,(char *)ucCurrency);
	switch (ucFormat)
	{
	case 1 :
		strcpy(separator1,".");
		strcpy(separator2,",");
		exponent=2;
		break;
	case 2 :
		strcpy(separator1,",");
		strcpy(separator2," ");
		exponent=2;
		break;
	case 3 :
		strcpy(separator1," ");
		strcpy(separator2,".");
		exponent=0;
		break;
	case 4 :
		strcpy(separator1," ");
		strcpy(separator2,",");
		exponent=0;
		break;
	case 5 :
		strcpy(separator1," ");
		strcpy(separator2," ");
		exponent=0;
		break;
	case 6 :
		strcpy(separator1,",");
		strcpy(separator2,".");
		exponent=3;
		break;
	case 7 :
		strcpy(separator1,".");
		strcpy(separator2,",");
		exponent=3;
		break;
	case 8 :
		strcpy(separator1,",");
		strcpy(separator2," ");
		exponent=3;
		break;
	case 0 :
	default :
		strcpy(separator1,",");
		strcpy(separator2,".");
		exponent=2;
		break;
	}
	switch(exponent)
	{
	case 1 :
		for (divider=1L,i=0L;i<(unsigned)exponent;divider*=10L,i++);
		if ((ulAmount/divider)<1000L) {
			Telium_Sprintf(amount_format,"%lu%s%0*lu",
					(ulAmount/divider),
					separator2,
					exponent,
					(ulAmount%divider));
		} else {
			if ((ulAmount/divider)<1000000L) {
				Telium_Sprintf(amount_format,"%lu%s%03lu%s%0*lu",
						((ulAmount/divider)/1000L),
						separator1,
						((ulAmount/divider)%1000L),
						separator2,
						exponent,
						(ulAmount%divider));
			} else {
				Telium_Sprintf(amount_format,"%lu%s%03lu%s%03lu%s%0*lu",
						((ulAmount/divider)/1000000L),
						separator1,
						(((ulAmount/divider)/1000L)%1000L),
						separator1,
						((ulAmount/divider)%1000L),
						separator2,
						exponent,
						(ulAmount%divider));
			}
		}
		break;
	case 2 :
	case 3 :
	case 4 :
	case 5 :
	case 6 :
	case 7 :
		for (divider=1L,i=0L;i<(unsigned)exponent;divider*=10L,i++);
		if ((ulAmount/divider)<1000L) {
			Telium_Sprintf(amount_format,"%lu%s%0*lu",
					(ulAmount/divider),
					separator2,
					exponent,
					(ulAmount%divider));
		} else {
			Telium_Sprintf(amount_format,"%lu%s%03lu%s%0*lu",
					((ulAmount/divider)/1000L),
					separator1,
					((ulAmount/divider)%1000L),
					separator2,
					exponent,
					(ulAmount%divider));
		}
		break;
	case 0 :
	default :
		if ((ulAmount)<1000L) {
			Telium_Sprintf(amount_format,"%lu",(ulAmount));
		} else {
			if ((ulAmount)<1000000L) {
				Telium_Sprintf(amount_format,"%lu%s%03lu",((ulAmount)/1000L),separator1,((ulAmount)%1000L));
			} else {
				Telium_Sprintf(amount_format,"%lu%s%03lu%s%03lu",((ulAmount)/1000000L),separator1,(((ulAmount)/1000L)%1000L),separator1,((ulAmount)%1000L));
			}
		}
		break;
	}
	if (position==0) {
		Telium_Sprintf((char*)pFormattedAmountMessage,"%s %s",
				amount_format,
				currency);
	} else {
		Telium_Sprintf((char*)pFormattedAmountMessage,"%s %s",
				currency,
				amount_format);
	}
}

//! \brief Requests amount entry, prepare frame for Entry Point (fill shared memory)
//! \param[in] pCurrencyLabel : the currency label
//! \param[out] pAmountBin : the entered transaction amount
//! \param[out] pFormattedAmountMessage : the formatted message to display
//! \return 
//!		- TRUE if amount has been entered.
//!		- FALSE if amount entry has been cancelled.

int Cless_ExplicitSelection_AmountEntry (const unsigned char *pCurrencyLabel, unsigned long * pAmountBin, unsigned char *pFormattedAmountMessage) {
	//	TAB_ENTRY_FCT TabEntry;
	unsigned long ulAmount;
	int nResult = 0;
	unsigned char ucFormat, ucPosition;
	unsigned char CurrencyLabel[4];
	char Amount[lenAmt + 1];

	// Amount entry
	//	TabEntry.mask = 0;
	//	TabEntry.time_out = 60;
	*pAmountBin = 0;

	memset (Amount, 0, sizeof(Amount));
	memset (CurrencyLabel, 0, sizeof(CurrencyLabel));

	///////////////////////////////////////////////////
	//unsigned char  ucFormat: format of the displayed amount                         
	//                         0 ==> 1.000,00
	//                         1 ==> 1,000.00
	//                         2 ==> 1 000,00
	//                         3 ==> 1.000
	//                         4 ==> 1,000
	//                         5 ==> 1 000
	//                         6 ==> 1.000,000
	//                         7 ==> 1,000.000
	//                         8 ==> 1 000,000
	//unsigned char ucPosition: position of the currency
	//                         0 ==> after the amount
	//                         1 ==> before the amount
	////////////////////////////////////////////////////

	ucFormat = 1;
	ucPosition = 0;
	memcpy (CurrencyLabel, pCurrencyLabel, 3);

	mapGet(traAmt, Amount, lenAmt);

	if (strlen(Amount) < 1) {

		if (Cless_Goal_IsAvailable()) {
			unsigned long result;

			//			Cless_Goal_Open();
			//			result = GL_Dialog_Amount(Cless_Goal_Handle(), NULL, "Amount", "/d,/d/d/d,/d/d/d,/d/d/D./D/D", Amount, sizeof(Amount), (char*)CurrencyLabel, GL_ALIGN_RIGHT, TabEntry.time_out * 1000);
			result = GL_KEY_VALID;
			if (result == GL_KEY_VALID) {
				nResult = T_VAL;
				ulAmount = strtoul(Amount, NULL, 10);
			}
			//			Cless_Goal_Close();
		} else {
			ulAmount = strtoul(Amount, NULL, 10);
			//			nResult = GetAmount(ucFormat, CurrencyLabel, ucPosition, &ulAmount, (unsigned char*)"Enter amount", &TabEntry);

			memset (Amount, 0, sizeof(Amount));
			Telium_Sprintf(Amount, "%lld", ulAmount);
		}
	}else{
		nResult = T_VAL;
		ulAmount = strtoul(Amount, NULL, 10);
	}
	if (nResult == T_VAL) {
		// Format the message that contains the amount, the currency according to ucFormat, ucPosition...  
		__Cless_ExplicitSelection_FormatAmount(ucFormat,CurrencyLabel,ucPosition,ulAmount,pFormattedAmountMessage);

		*pAmountBin = ulAmount;
	}

	return (nResult == T_VAL);
}


//! \brief Function called for to perform explicit transaction (card detection and application selection).
//! \param[in] ucTransactionType Indicates the transaction type (debit or refund).

int Cless_ExplicitSelection_Process (const unsigned char ucTransactionType, const char ucAmountType) {
	TLV_TREE_NODE hOutputTlvTree = NULL;
	int nResult, nEvent;
	unsigned char ucBuffer[4];
	unsigned char bEnd, bCancel = FALSE;
	unsigned char bLoopMode = FALSE;
	unsigned char ucCurrencyCode[2];
	unsigned long ulAmount = 0;
	unsigned long ulAmountOther = 0;
	unsigned char ucFormattedAmountMessage[32];
	//	MSGinfos tDisplayMsg;
	//	MSGinfos tAmountMsg;
	//	int merchLang;
	//	int nCardholderLanguage;
	unsigned char ucTransactionCashBack = 0;
	char Temp[256];
	char CurrencyName[4];

	//	Telium_File_t * hKeyboard;
	T_SHARED_DATA_STRUCT * pSharedData = NULL;
	int nKernelToUse;

	memset(Temp, 0, sizeof(Temp));
	memset(CurrencyName, 0, sizeof(CurrencyName));

	// Get the currency
	mapGet(traCurrencyLabel, CurrencyName, lenCurrSign);

	memset(Temp, 0, sizeof(Temp));
	mapGet(traCurrencyNum, Temp, lenCurrSign);
	fmtPad(Temp, -(lenCurrSign+1), '0');
	hex2bin(ucCurrencyCode, Temp, (lenCurrSign+1)/2);

	Cless_Customisation_SetUsedPaymentScheme (CLESS_SAMPLE_CUST_UNKNOWN);

#ifndef DISABLE_OTHERS_KERNELS
	VisaWave_UnsetpayWaveFallBack();
#endif

	//	merchLang = PSQ_Give_Language();
	//	nCardholderLanguage = merchLang; // By default, cardholder language is set to default language
	//	// Enter transaction amount

	if (ucAmountType == CLESS_SAMPLE_AMOUNT_TYPE_BIN) {
		if (!Cless_ExplicitSelection_AmountEntry((const unsigned char *)CurrencyName, &ulAmount, ucFormattedAmountMessage)) {
			goto End;
		}
	} else if (ucAmountType == CLESS_SAMPLE_AMOUNT_TYPE_OTHER_BIN) {
		if (!Cless_ExplicitSelection_AmountEntry((const unsigned char *)CurrencyName, &ulAmountOther, ucFormattedAmountMessage)) {
			goto End;
		}
	}

	// Loop mode information
	bLoopMode = Cless_Menu_IsTransactionLoopOn();

	do { // bLoopMode

#ifndef DISABLE_OTHERS_KERNELS
		Restart:
#endif

		memset(ucBuffer, 0, sizeof(ucBuffer));

		//// Prepare data and structure for kernel processing after application selection
		// Input parameters
		if (!Cless_Fill_InitSharedBufferWithKernels (&g_pKernelSharedBuffer)) {
			GTL_Traces_TraceDebug("Cless_ExplicitSelection_Process : Cless_Fill_InitSharedBufferWithKernels failed");
			goto End;
		}

		// Save the transaction related data for future use
		Cless_Fill_PrepareAndSaveClessGiveInfoTransactionData (TRUE, ucTransactionType, ucTransactionCashBack, ucCurrencyCode, 2, (const unsigned char *)CurrencyName, ulAmount, ulAmountOther);

		//// Prepare data for card detection and application selection
		// Create the shared buffer (for transaction result)
		pSharedData = GTL_SharedExchange_InitShared(10240);

		if (pSharedData == NULL) {
			GTL_Traces_TraceDebug ("Cless_ExplicitSelection_Process : pSharedData is NULL");
			goto End;
		}

		// Create the TLV Tree to provide all the application selection parameters (including transaction amount)
		hOutputTlvTree = TlvTree_New(0);

		if (hOutputTlvTree != NULL) {
			// Fill the hOutputTlvTree with the correct parameters
			if (!Cless_Fill_GetApplicationSelectionParameters (&hOutputTlvTree, &pTreeCurrentParam, TRUE)) {
				GTL_Traces_TraceDebug ("Cless_ExplicitSelection_Process : Cless_Fill_GetApplicationSelectionParameters failed");
				goto End;
			}


			// Warning : the Cless_ExplicitSelection_Selection_ApplicationSelectionProcess() function knows only the TAG_EMV_AMOUNT_AUTH_BIN
			// Add the transaction amount
			GTL_Convert_UlToBinNumber(ulAmount, ucBuffer, 4);
			TlvTree_AddChild(hOutputTlvTree, TAG_EMV_AMOUNT_AUTH_BIN, ucBuffer, 4);

			// Call DLL to load data
			nResult = Cless_ExplicitSelection_LoadData (hOutputTlvTree);
			if (nResult != CLESS_STATUS_OK){
				GTL_Traces_TraceDebug("Cless_ExplicitSelection_Process : Cless_ExplicitSelection_LoadData failed (nResult=%02x)", nResult);
				goto End;
			}

			// Perform the transaction pre processing
			nResult = Cless_ExplicitSelection_EntryPoint_TransactionPreProcessing ();
			if (nResult != CLESS_STATUS_OK) {
				GTL_Traces_TraceDebug("Cless_ExplicitSelection_Process : Cless_ExplicitSelection_EntryPoint_TransactionPreProcessing failed (nResult=%02x)", nResult);
				goto End;
			}

			RestartDoubleTap:

			// Launch the card detection
			nResult = Cless_ExplicitSelection_GlobalCardDetection();
			if (nResult != CLESS_STATUS_OK) {
				GTL_Traces_TraceDebug("Cless_ExplicitSelection_Process : Cless_ExplicitSelection_GlobalCardDetection failed (nResult=%02x)", nResult);
				goto End;
			}


			// Wait contactless event (or cancel if red key is pressed
			bEnd = FALSE;
			bCancel = FALSE;

			//Do card detection
			(void)bEnd;
			nEvent = Telium_Ttestall (CLESS, 0);

			// Check if card detection has been cancelled
			if (bCancel) {
				GTL_Traces_TraceDebug ("Cless_ExplicitSelection_Process : Card detection has been cancelled");
				goto End;
			}

			if (nEvent != CLESS) {
				GTL_Traces_TraceDebug ("Cless_ExplicitSelection_Process : Unknown event (nEvent=%02x)", nEvent);
				goto End;
			}


			// Get the card detection result
			nResult = Cless_Generic_CardDetectionGetResults (pSharedData, TRUE);
			if (nResult != CLESS_STATUS_OK) {
				GTL_Traces_TraceDebug("Cless_ExplicitSelection_Process : Cless_Generic_CardDetectionGetResults failed (nResult=%02x)", nResult);
				goto End;
			}


			// A card has been detected, perform the application selection
			nResult = Cless_ExplicitSelection_Selection_ApplicationSelectionProcess (pSharedData);
			////Cless_DumpData_DumpSharedBuffer(pSharedData, 0);
			if (nResult != CLESS_STATUS_OK) {
				GTL_Traces_TraceDebug("Cless_ExplicitSelection_Process : Cless_ExplicitSelection_Selection_ApplicationSelectionProcess failed (nResult=%02x)", nResult);
				goto End;
			}

			// First clear the buffer to be used with the contactless kernel
			GTL_SharedExchange_ClearEx (g_pKernelSharedBuffer, FALSE);

			// Add the generic transaction data (previously saved) in the shared buffer (date, time, amount, etc).
			if (!Cless_Fill_TransactionGenericData(g_pKernelSharedBuffer)) {
				GTL_Traces_TraceDebug("Cless_ExplicitSelection_Process : Cless_Fill_TransactionGenericData failed");
				goto End;
			}

			//// Add AID related data to the shared buffer used with kernels
			// Application selection succeeded, dump the application selection results
			if (!Cless_Fill_AidRelatedData (g_pKernelSharedBuffer, pSharedData, &nKernelToUse)) {
				GTL_Traces_TraceDebug("Cless_ExplicitSelection_Process : Cless_Fill_AidRelatedData failed");
				goto End;
			}

			mapPutCard(traCardKernel, nKernelToUse);

			// Call kernel in relationship with AID
			switch (nKernelToUse)
			{
#ifndef DISABLE_OTHERS_KERNELS
			case DEFAULT_EP_KERNEL_QUICKPASS:
				nResult = Cless_QuickPass_PerformTransaction(g_pKernelSharedBuffer);

				if(nResult == CLESS_CR_MANAGER_RESTART_DOUBLE_TAP) {

					Cless_ExplicitSelection_DetectionPrepareForRestart(TRUE);

					// Reset CLESS transaction parameters
					Cless_Customisation_SetUsedPaymentScheme (CLESS_SAMPLE_CUST_UNKNOWN);

					// Clear the selection Shared buffer (for selection result)
					GTL_SharedExchange_ClearEx(pSharedData, FALSE);

					goto RestartDoubleTap;
				}
				break;
			case DEFAULT_EP_KERNEL_PAYPASS :
				nResult = Cless_PayPass_PerformTransaction(g_pKernelSharedBuffer);
				break;
#endif

			case DEFAULT_EP_KERNEL_VISA :
#ifndef DISABLE_OTHERS_KERNELS
				if(VisaWave_isVisaWaveProcessing()) {
					nResult = Cless_VisaWave_PerformTransaction(g_pKernelSharedBuffer);
					if(nResult == VISAWAVE_STATUS_PAYWAVE_CARD) {
						VisaWave_SetpayWaveFallBack();

						// Release the TLV Tree if allocated
						if (hOutputTlvTree != NULL)
							TlvTree_Release(hOutputTlvTree);

						// Release shared buffer if allocated
						if (pSharedData != NULL)
							GTL_SharedExchange_DestroyShare(pSharedData);

						goto Restart;
					}
				} else
#endif
					nResult = Cless_payWave_PerformTransaction(g_pKernelSharedBuffer);
				break;

#ifndef DISABLE_OTHERS_KERNELS
			case DEFAULT_EP_KERNEL_AMEX :
				nResult = Cless_ExpressPay_PerformTransaction(g_pKernelSharedBuffer);
				break;
#endif
			default:
				GTL_Traces_TraceDebug("Cless_ExplicitSelection_Process : Unknown Kernel to use in cless_debit_aid() : %x", nKernelToUse);
				break;
			}
		} else {
			GTL_Traces_TraceDebug ("Cless_ExplicitSelection_Process : hOutputTlvTree is NULL");
			goto End;
		}

		End:
		// Release the TLV Tree if allocated
		if (hOutputTlvTree != NULL){
			TlvTree_Release(hOutputTlvTree);
			hOutputTlvTree = NULL;
		}

		// Release shared buffer if allocated
		if (pSharedData != NULL)
			GTL_SharedExchange_DestroyShare(pSharedData);

		// Clear the detection/selection resources
		Cless_ExplicitSelection_ClearGlobalData();

		// Reset CLESS transaction parameters
		Cless_Customisation_SetUsedPaymentScheme (CLESS_SAMPLE_CUST_UNKNOWN);

	}while (bLoopMode && !bCancel);

	if(ClessEmv_IsDriverOpened())
		ClessEmv_CloseDriver();

	//Finish On the transaction
	All_AfterTransaction();

	return nResult;
}
