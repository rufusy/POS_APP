//! \file
//! Module that manages the global transaction data.
//
/////////////////////////////////////////////////////////////////
//// Includes ///////////////////////////////////////////////////

#include "Cless_Implementation.h"
#include "Globals.h"


/////////////////////////////////////////////////////////////////
//// Macros & preprocessor definitions //////////////////////////

#define GLOBAL_TRANSACTION_DATA_MAX_LENGTH				16384


/////////////////////////////////////////////////////////////////
//// Types //////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////
//// Global variables ///////////////////////////////////////////

static T_SHARED_DATA_STRUCT * gs_pGlobalTransactionData = NULL;

static char gs_ucCurrencyLabel[4];

/////////////////////////////////////////////////////////////////
//// Static function definitions ////////////////////////////////


/////////////////////////////////////////////////////////////////
//// Functions //////////////////////////////////////////////////

void CS_GlobalTransactionData_Clear (void)
{
	// Clear the currency label
	memset (gs_ucCurrencyLabel, 0, sizeof(gs_ucCurrencyLabel));

	if (gs_pGlobalTransactionData != NULL)
	{
		GTL_SharedExchange_DestroyShare(gs_pGlobalTransactionData);
		gs_pGlobalTransactionData = NULL;
	}
}


int CS_GlobalTransactionData_Init (void)
{
	int nResult = FALSE;

	// Clear and destroy any existing structure
	CS_GlobalTransactionData_Clear();

	// Allocate memory
	gs_pGlobalTransactionData = GTL_SharedExchange_InitShared (GLOBAL_TRANSACTION_DATA_MAX_LENGTH);
	if (gs_pGlobalTransactionData)
	{
		GTL_SharedExchange_Clear (gs_pGlobalTransactionData);
	}
	else
	{
		GTL_Traces_TraceDebug("CS_GlobalTransactionData_Init: Unable to allocate memory to create the global transaction data structure");
		goto end;
	}

	nResult = TRUE;

	end:
	return (nResult);
}



/**
 * Load the transaction global data from an external structure (typically used when data are extracted from a test plan file).
 * @param pStruct Data to be loaded in the global transaction data structure.
 * @return
 * - \a TRUE if correctly completed.
 * - \a FALSE if an error occurred.
 */
/*int CS_GlobalTransactionData_LoadFromExternalData (T_SHARED_DATA_STRUCT * pStruct)
{
	int nResult = FALSE;
	int cr;

	if (pStruct != NULL)
	{
		// Initialization
		if (CS_GlobalTransactionData_Init())
		{
			cr = GTL_SharedExchange_AddSharedBufferContent(gs_pGlobalTransactionData, pStruct);
			if (cr != STATUS_SHARED_EXCHANGE_OK)
			{
				GTL_Traces_TraceDebug("CS_GlobalTransactionData_LoadFromExternalData: Fail to copy pStruct to gs_pGlobalTransactionData (cr = %d)", cr);
				goto end;
			}
		}
		else
		{
			GTL_Traces_TraceDebug("CS_GlobalTransactionData_LoadFromExternalData: Unable initialize the global transaction data structure");
			goto end;
		}
	}
	else
	{
		GTL_Traces_TraceDebug("CS_GlobalTransactionData_LoadFromExternalData: pStruct = NULL");
		goto end;
	}

	nResult = TRUE;

end:
	return (nResult);
}*/



/**
 * Copy the global transaction data into the provided shared exchange structure.
 * @param pDestination Structure to be filled. Note it is clear if any error occurred.
 * @return
 * - \a TRUE if correctly performed.
 * - \a FALSE if an error occurred.
 */
int CS_GlobalTransactionData_CopyToSharedExchange (T_SHARED_DATA_STRUCT * pDestination)
{
	int nResult = FALSE;
	int cr;

	if (pDestination)
	{
		if (gs_pGlobalTransactionData)
		{
			cr = GTL_SharedExchange_AddSharedBufferContent(pDestination, gs_pGlobalTransactionData);
			if (cr != STATUS_SHARED_EXCHANGE_OK)
			{
				GTL_Traces_TraceDebug("CS_GlobalTransactionData_CopyToSharedExchange: Unable to copy gs_pGlobalTransactionData into pDestination (cr = %d)", cr);
				goto end;
			}
		}
		else
		{
			GTL_Traces_TraceDebug("CS_GlobalTransactionData_CopyToSharedExchange: gs_pGlobalTransactionData is NULL");
			goto end;
		}
	}
	else
	{
		GTL_Traces_TraceDebug("CS_GlobalTransactionData_CopyToSharedExchange: pDestination is NULL");
		goto end;
	}

	nResult = TRUE;

	end:
	return (nResult);
}



/**
 * Load all the global transaction data into a shared exchange structure.
 * @param bUseTestPlan Indicates if the test plan shall be used or not.
 * @param bUseDefaultAmount Indicates if the default amount shall be used or not.
 * @return
 * - \a TRUE if correctly completed.
 * - \a FALSE if an error occurred.
 */
int CS_GlobalTransactionData_Prepare (int bUseTestPlan, int bUseDefaultAmount) {
	int nResult = FALSE;
	unsigned long ulAmountAuth = 0;
	unsigned long ulAmountOther = 0;
	unsigned char ucTransactionCurrencyCode[2];
	unsigned char ucTransactionCurrencyExponent;
	//	int nTransactionType;
	unsigned char ucTransactionType;
	int cr;
	unsigned char ucTempBuffer[6];
	NO_SERIE serial;
	unsigned char ucCategoryCode;
	Telium_Date_t Date;
	unsigned char ucDate[3];
	unsigned char ucTime[3];
	int ret = 0 ;
	char MENU[lenMnu + 1];
	card mnuitem = 0;
	//	int nPosition;
	//	unsigned long ulReadLength;
	//	unsigned char * pReadValue = NULL;
	char temp[256];
	char currencyName[lenCurrSign + 1];

	if (CS_GlobalTransactionData_Init()) {
		//		// if test plan is used, choose the test case to be executed
		//		if (bUseTestPlan)
		//		{
		//			// Load the test plan data from the TEST PLAN file loaded in the terminal
		//			if (!CS_TestPlan_ChooseAndLoad())
		//			{
		//				// An error occurred when preparing the static transaction data
		//				GTL_Traces_TraceDebug ("CS_GlobalTransactionData_Prepare : An error occurred when loading data from the selected test case");
		//				goto end;
		//			}
		//			else
		//			{
		//				// Copy the test plan data into the Global Transaction Data
		//				if (!CS_TestPlan_CopyToSharedStruct(gs_pGlobalTransactionData))
		//				{
		//					GTL_Traces_TraceDebug ("CS_GlobalTransactionData_Prepare : An error occurred when loading test plan data into the global transaction data structure");
		//					goto end;
		//				}
		//				else
		//				{
		//					nPosition = SHARED_EXCHANGE_POSITION_NULL;
		//
		//					// Extract the currency label
		//					cr = GTL_SharedExchange_FindNext(gs_pGlobalTransactionData, &nPosition, TAG_EMV_TRANSACTION_CURRENCY_CODE, &ulReadLength, (const unsigned char **)&pReadValue);
		//					if (cr == STATUS_SHARED_EXCHANGE_OK)
		//					{
		//						if (ulReadLength == 2)
		//						{
		//							// Get the currency configured in the manager
		//							memset (&money, 0x00, sizeof(S_MONEY));
		//							PSQ_Est_money (&money);
		//
		//							if ((pReadValue[0] == (unsigned char)(money.code[0] - '0')) &&
		//									(pReadValue[1] == (unsigned char)(((money.code[1] - '0') << 4) | (money.code[2] - '0'))))
		//							{
		//								memcpy (gs_ucCurrencyLabel, (char*)money.nom, 3);
		//							}
		//						}
		//					}
		//
		//				}
		//			}
		//		}
		//		else
		{
			// ***********************************************************************
			// ******************** TRANSACTION TYPE
			// ***********************************************************************
			// Input the data directly from the different input, for standard mode
			// Check if unattended mode is enabled
#ifndef DISABLE_UNATTENDED
			if (Cless_Unattended_IsUnattendedMechanism())
			{
				// If unattended, the transaction amount is already available, and no entry is necessary
				nTransactionType = Cless_Unattended_GetTransactionType();
				ucTransactionType = (unsigned char)nTransactionType;
			}
			else
#endif
			{
				//				// Transaction is started using the normal way (amount entry, etc).
				//				if (Cless_Menu_IsEnableInputTransactionType()) // Is transactionType must be inputed
				//				{
				//					// Input nTransactionType
				//					Cless_Menu_TransactionTypeMenuDisplay (&nTransactionType);
				//					ucTransactionType = (unsigned char)nTransactionType;
				//				}
				//				else
				//				{

				//TODO: NEW-CLESS Load the transaction from DB and menu selection
				// If the operator does not choose anything, use DEBIT as default
				ucTransactionType = CLESS_SAMPLE_TRANSACTION_TYPE_DEBIT;

				MAPGET(traMnuItm, MENU, lblKO);
				dec2num(&mnuitem, MENU,0);

				switch (mnuitem) {
				case mnuDeposit:
					ucTransactionType = CLESS_SAMPLE_TRANSACTION_TYPE_CASH_DEPOSIT;
					break;
				case mnuWithdrawal:
					ucTransactionType = CLESS_SAMPLE_TRANSACTION_TYPE_CASH;
					break;
				case mnuVoid:
					ucTransactionType = CLESS_SAMPLE_TRANSACTION_TYPE_REFUND;
					break;
				case mnuSaleCB:
					ucTransactionType = CLESS_SAMPLE_TRANSACTION_TYPE_PURSH_WITH_CASHBACK;
					break;
				case mnuSale:
				default:
					ucTransactionType = CLESS_SAMPLE_TRANSACTION_TYPE_DEBIT;
					break;
				}
				lblKO:;

				//				}
			}

			// Check the provided transaction type is correct
			if ((ucTransactionType != CLESS_SAMPLE_TRANSACTION_TYPE_DEBIT) && (ucTransactionType != CLESS_SAMPLE_TRANSACTION_TYPE_REFUND) &&
					(ucTransactionType != CLESS_SAMPLE_TRANSACTION_TYPE_CASH) && (ucTransactionType != CLESS_SAMPLE_TRANSACTION_TYPE_PURSH_WITH_CASHBACK) &&
					(ucTransactionType != CLESS_SAMPLE_TRANSACTION_TYPE_MANUAL_CASH) && (ucTransactionType != CLESS_SAMPLE_TRANSACTION_TYPE_CASH_DEPOSIT) &&
					(ucTransactionType != CLESS_SAMPLE_TRANSACTION_TYPE_CASH_DISBURSEMENT))
			{
				GTL_Traces_TraceDebug("__Cless_ExplicitSelection_PrepareStaticTransactionData : Transaction type is unknown (value = %02X)", ucTransactionType);
				goto end;
			}

			// Add the transaction type to the data exchange
			cr = GTL_SharedExchange_AddTag(gs_pGlobalTransactionData, TAG_EMV_TRANSACTION_TYPE, 1, &ucTransactionType);
			if (cr != STATUS_SHARED_EXCHANGE_OK) {
				GTL_Traces_TraceDebug ("CS_GlobalTransactionData_Prepare : An error occurred when adding the transaction type to the shared exchange structure (cr = %d)", cr);
				goto end;
			}

			// Add the transaction type to the data exchange
			/*cr = GTL_SharedExchange_AddTag(gs_pGlobalTransactionData, TAG_EMV_INT_TRANSACTION_TYPE, 1, &ucTransactionType);
			if (cr != STATUS_SHARED_EXCHANGE_OK)
			{
				GTL_Traces_TraceDebug ("CS_GlobalTransactionData_Prepare : An error occurred when adding the internal transaction type to the shared exchange structure (cr = %d)", cr);
				goto end;
			}*/


			// ***********************************************************************
			// ******************** TRANSACTION CURRENCY CODE
			// ***********************************************************************
			memset(temp, 0, sizeof(temp));
			mapGet(traCurrencyNum, temp, lenCurrSign);
			fmtPad(temp, -(lenCurrSign+1), '0');
			hex2bin(ucTransactionCurrencyCode, temp, (lenCurrSign+1)/2);

			// Add the currency code into the structure
			cr = GTL_SharedExchange_AddTag(gs_pGlobalTransactionData, TAG_EMV_TRANSACTION_CURRENCY_CODE, 2, ucTransactionCurrencyCode);
			if (cr != STATUS_SHARED_EXCHANGE_OK) {
				GTL_Traces_TraceDebug ("CS_GlobalTransactionData_Prepare : An error occurred when adding the transaction currency code to the shared exchange structure (cr = %d)", cr);
				goto end;
			}



			// ***********************************************************************
			// ******************** TRANSACTION CURRENCY EXPONENT
			// ***********************************************************************
			// Get the transaction currency exponent
			ucTransactionCurrencyExponent = 2;

			// Add the currency code into the structure
			cr = GTL_SharedExchange_AddTag(gs_pGlobalTransactionData, TAG_EMV_TRANSACTION_CURRENCY_EXPONENT, 1, &ucTransactionCurrencyExponent);
			if (cr != STATUS_SHARED_EXCHANGE_OK) {
				GTL_Traces_TraceDebug ("CS_GlobalTransactionData_Prepare : An error occurred when adding the transaction currency exponent to the shared exchange structure (cr = %d)", cr);
				goto end;
			}



			// ***********************************************************************
			// ******************** TRANSACTION CURRENCY LABEL
			// ***********************************************************************
			// Get the currency label


			memset(temp, 0, sizeof(temp));
			mapGet(traCurrencyLabel, temp, lenCurrSign);
			fmtPad(temp, -(lenCurrSign), '0');
			hex2bin((byte *)currencyName, temp, (lenCurrSign+1)/2);
			memcpy (gs_ucCurrencyLabel, (char*)currencyName, 3);

			// ***********************************************************************
			// ******************** TRANSACTION AMOUNT AUTHORIZED (BIN & NUM)
			// ***********************************************************************


			// Whatever the transaction, amount is entered
			if (!Cless_Common_AmountEntry (FALSE, (unsigned char*)currencyName, &ulAmountAuth)) {
				GTL_Traces_TraceDebug("CS_GlobalTransactionData_Prepare : Transaction amount entry cancelled");
				goto end;
			}
		}

		// Add the amount into the shared exchange structure
		memset (ucTempBuffer, 0, sizeof(ucTempBuffer));
		GTL_Convert_UlToDcbNumber(ulAmountAuth, ucTempBuffer, 6);
		cr = GTL_SharedExchange_AddTag(gs_pGlobalTransactionData, TAG_EMV_AMOUNT_AUTH_NUM, 6, (const unsigned char *) ucTempBuffer);
		if (cr != STATUS_SHARED_EXCHANGE_OK)
		{
			GTL_Traces_TraceDebug ("CS_GlobalTransactionData_Prepare : An error occurred when adding the amount authorized numeric to the shared exchange structure (cr = %d)", cr);
			goto end;
		}

		memset (ucTempBuffer, 0, sizeof(ucTempBuffer));
		GTL_Convert_UlToBinNumber(ulAmountAuth, ucTempBuffer, 4);
		cr = GTL_SharedExchange_AddTag(gs_pGlobalTransactionData, TAG_EMV_AMOUNT_AUTH_BIN, 4, (const unsigned char *)ucTempBuffer);
		if (cr != STATUS_SHARED_EXCHANGE_OK)
		{
			GTL_Traces_TraceDebug ("CS_GlobalTransactionData_Prepare : An error occurred when adding the amount authorized binary to the shared exchange structure (cr = %d)", cr);
			goto end;
		}



		// ***********************************************************************
		// ******************** TRANSACTION AMOUNT OTHER (BIN & NUM)
		// ***********************************************************************
		// If transaction is cashback, enter amount other
		if (ucTransactionType == CLESS_SAMPLE_TRANSACTION_TYPE_PURSH_WITH_CASHBACK)
		{
			// Check if unattended mode is enabled
#ifndef DISABLE_UNATTENDED
			if (Cless_Unattended_IsUnattendedMechanism())
			{
				// If unattended, the transaction amount other is already available, and no entry is necessary
				ulAmountOther = Cless_Unattended_GetAmountOther();

				if (Cless_DumpData_DumpOpenOutputDriver())
				{
					Cless_DumpData ("Amount Auth = %lu", ulAmountAuth);
					Cless_DumpData_DumpNewLine();
					Cless_DumpData ("Amount Other = %lu", ulAmountOther);
					Cless_DumpData_DumpNewLine();
					Cless_DumpData_DumpCloseOutputDriver();
				}
			}
			else
#endif
			{
				if (!Cless_Common_AmountEntry (TRUE,(const unsigned char *) currencyName, &ulAmountOther)) {
					// Display an error message
					Cless_GuiState_DisplayScreen (CLESS_SAMPLE_SCREEN_ERROR, -1, -1);

					GTL_Traces_TraceDebug("CS_GlobalTransactionData_Prepare : Transaction amount other entry cancelled");
					goto end;
				}
			}

			// Anti-bug:
			if (ulAmountOther > ulAmountAuth) {
				// Display an error message
				Cless_GuiState_DisplayScreen (CLESS_SAMPLE_SCREEN_ERROR, -1, -1);

				// An error occurred when preparing the static transaction data
				GTL_Traces_TraceDebug ("CS_GlobalTransactionData_Prepare : An error occurred when preparing the static transaction data");
				goto end;
			}
		}
		else
		{
			// Indicate the amount other is present
			ulAmountOther = 0;
		}

		// Add the amount into the shared exchange structure
		memset (ucTempBuffer, 0, sizeof(ucTempBuffer));
		GTL_Convert_UlToDcbNumber(ulAmountOther, ucTempBuffer, 6);
		cr = GTL_SharedExchange_AddTag(gs_pGlobalTransactionData, TAG_EMV_AMOUNT_OTHER_NUM, 6, (const unsigned char *)ucTempBuffer);
		if (cr != STATUS_SHARED_EXCHANGE_OK)
		{
			GTL_Traces_TraceDebug ("CS_GlobalTransactionData_Prepare : An error occurred when adding the amount other numeric to the shared exchange structure (cr = %d)", cr);
			goto end;
		}

		memset (ucTempBuffer, 0, sizeof(ucTempBuffer));
		GTL_Convert_UlToBinNumber(ulAmountOther, ucTempBuffer, 4);
		cr = GTL_SharedExchange_AddTag(gs_pGlobalTransactionData, TAG_EMV_AMOUNT_OTHER_BIN, 4, (const unsigned char *)ucTempBuffer);
		if (cr != STATUS_SHARED_EXCHANGE_OK)
		{
			GTL_Traces_TraceDebug ("CS_GlobalTransactionData_Prepare : An error occurred when adding the amount other binary to the shared exchange structure (cr = %d)", cr);
			goto end;
		}



		// ***********************************************************************
		// ******************** IFD SERIAL NUMBER
		// ***********************************************************************
		memset (serial, 0, sizeof(NO_SERIE));

		// Extract SN and initialize the structure
		PSQ_Give_Serial_Number(serial);

		// Add the serial code into the structure
		cr = GTL_SharedExchange_AddTag(gs_pGlobalTransactionData, TAG_EMV_IFD_SERIAL_NUMBER, 8, (const unsigned char *) &serial);
		if (cr != STATUS_SHARED_EXCHANGE_OK) {
			GTL_Traces_TraceDebug ("CS_GlobalTransactionData_Prepare : An error occurred when adding the IFD serial number to the shared exchange structure (cr = %d)", cr);
			goto end;
		}



		// ***********************************************************************
		// ******************** TRANSACTION CATEGORY CODE
		// ***********************************************************************
		ucCategoryCode = 0x52; // "R"

		// Add the currency code into the structure
		cr = GTL_SharedExchange_AddTag(gs_pGlobalTransactionData, TAG_PAYPASS_TRANSACTION_CATEGORY_CODE, 1, &ucCategoryCode);
		if (cr != STATUS_SHARED_EXCHANGE_OK) {
			GTL_Traces_TraceDebug ("CS_GlobalTransactionData_Prepare : An error occurred when adding the Transaction Category Code to the shared exchange structure (cr = %d)", cr);
			goto end;
		}



		// ***********************************************************************
		// ******************** TRANSACTION DATE & TIME
		// ***********************************************************************
		if (Telium_Read_date(&Date) == OK)
		{
			ucDate[0] = (unsigned char)(((Date.year[0] - '0') << 4) | (Date.year[1] - '0'));
			ucDate[1] = (unsigned char)(((Date.month[0] - '0') << 4) | (Date.month[1] - '0'));
			ucDate[2] = (unsigned char)(((Date.day[0] - '0') << 4) | (Date.day[1] - '0'));
			ucTime[0] = (unsigned char)(((Date.hour[0] - '0') << 4) | (Date.hour[1] - '0'));
			ucTime[1] = (unsigned char)(((Date.minute[0] - '0') << 4) | (Date.minute[1] - '0'));
			ucTime[2] = (unsigned char)(((Date.second[0] - '0') << 4) | (Date.second[1] - '0'));
		}
		else
		{
			GTL_Traces_TraceDebug ("CS_GlobalTransactionData_Prepare: Cannot read the date and time.");
			goto end;
		}

		// Store the tag
		cr = GTL_SharedExchange_AddTag(gs_pGlobalTransactionData, TAG_EMV_TRANSACTION_DATE, 3, ucDate);
		if (cr != STATUS_SHARED_EXCHANGE_OK)
		{
			GTL_Traces_TraceDebug ("CS_GlobalTransactionData_Prepare : An error occurred when adding the Transaction Date to the shared exchange structure (cr = %d)", cr);
			goto end;
		}

		cr = GTL_SharedExchange_AddTag(gs_pGlobalTransactionData, TAG_EMV_TRANSACTION_TIME, 3, ucTime);
		if (cr != STATUS_SHARED_EXCHANGE_OK)
		{
			GTL_Traces_TraceDebug ("CS_GlobalTransactionData_Prepare : An error occurred when adding the Transaction Time to the shared exchange structure (cr = %d)", cr);
			goto end;
		}



		// ***********************************************************************
		// ******************** TRANSACTION SEQUENCE COUNTER
		// ***********************************************************************
		// Add the transaction sequence counter
		if (!Cless_Batch_AddTscToSharedBuffer(gs_pGlobalTransactionData)) {
			GTL_Traces_TraceDebug("CS_GlobalTransactionData_Prepare : Cless_Batch_AddTscToSharedBuffer failed");
			nResult = FALSE;
			goto end;
		}

	} else {
		GTL_Traces_TraceDebug ("CS_GlobalTransactionData_Prepare : An error occurred when initializing the global transaction data structure");
		goto end;
	}

	nResult = TRUE;

	end:

	if (!nResult) {
		CS_GlobalTransactionData_Clear();
	}
	return (nResult);
}



/**
 * Retrieve the transaction amount, authorized
 * @param amount Filled with the amount. 0 if an error occurred.
 * @return
 * - \a TRUE if correctly completed.
 * - \a FALSE if an error occurred.
 */
int CS_GlobalTransactionData_GetAmountAuthorized (unsigned long * amount) {
	int nResult = FALSE;
	unsigned long ulAmount;
	int cr;
	unsigned char * pReadValue;
	unsigned long ulReadLength;
	int nPosition = SHARED_EXCHANGE_POSITION_NULL;

	if (amount) {
		*amount = 0;

		if (gs_pGlobalTransactionData) {
			cr = GTL_SharedExchange_FindNext(gs_pGlobalTransactionData, &nPosition, TAG_EMV_AMOUNT_AUTH_NUM, &ulReadLength, (const unsigned char **)&pReadValue);
			if (cr == STATUS_SHARED_EXCHANGE_OK) {
				if (ulReadLength > 0) {
					GTL_Convert_DcbNumberToUl(pReadValue, &ulAmount, 6);
					*amount = ulAmount;
				}
			} else {
				GTL_Traces_TraceDebug ("CS_GlobalTransactionData_GetAmountAuthorized : TAG_EMV_AMOUNT_AUTH_NUM not present in gs_pGlobalTransactionData");
				goto end;
			}
		} else {
			GTL_Traces_TraceDebug ("CS_GlobalTransactionData_GetAmountAuthorized : gs_pGlobalTransactionData is NULL");
			goto end;
		}
	} else {
		GTL_Traces_TraceDebug ("CS_GlobalTransactionData_GetAmountAuthorized : amount is NULL");
		goto end;
	}

	nResult = TRUE;

	end:
	return (nResult);
}



/**
 * Retrieve the transaction type.
 * @param amount Filled with the transaction type. CLESS_SAMPLE_TRANSACTION_TYPE_DEFAULT if an error occurred or if the tag is not found.
 * @return
 * - \a TRUE if correctly completed.
 * - \a FALSE if an error occurred.
 */
int CS_GlobalTransactionData_GetTransactionType (unsigned char * transactionType) {
	int nResult = FALSE;
	int cr;
	unsigned char * pReadValue;
	unsigned long ulReadLength;
	int nPosition = SHARED_EXCHANGE_POSITION_NULL;

	if (transactionType) {
		*transactionType = CLESS_SAMPLE_TRANSACTION_TYPE_DEFAULT;

		if (gs_pGlobalTransactionData) {
			cr = GTL_SharedExchange_FindNext(gs_pGlobalTransactionData, &nPosition, TAG_EMV_TRANSACTION_TYPE, &ulReadLength, (const unsigned char **)&pReadValue);
			if (cr == STATUS_SHARED_EXCHANGE_OK) {
				if (ulReadLength > 0) {
					*transactionType = pReadValue[0];
				}
			} else {
				// Transaction type is not present, use the default value
				GTL_Traces_TraceDebug ("CS_GlobalTransactionData_GetTransactionType : Transaction type is not present in gs_pGlobalTransactionData, use the default value");
				goto end;
			}
		} else {
			GTL_Traces_TraceDebug ("CS_GlobalTransactionData_GetTransactionType : gs_pGlobalTransactionData is NULL");
			goto end;
		}
	} else {
		GTL_Traces_TraceDebug ("CS_GlobalTransactionData_GetTransactionType : transactionType is NULL");
		goto end;
	}

	nResult = TRUE;

	end:
	return (nResult);
}



/**
 * Retrieve the transaction type.
 * @param currencyLabel Filled with the pointer of the currency label.
 * @return
 * - \a TRUE if data correctly initialized.
 * - \a FALSE if not (an in this case, currency is probably not correct or empty).
 */
int CS_GlobalTransactionData_GetCurrencyLabel (char ** currencyLabel) {
	if (currencyLabel) {
		*currencyLabel = gs_ucCurrencyLabel;
	}

	return (gs_pGlobalTransactionData != NULL);
}



/**
 * Checks if the tag is present and not empty in the global transaction data.
 * @param ulTag Tag to be checked.
 * @return
 * - \a TRUE if the tag is present.
 * - \a FALSE else.
 */
int CS_GlobalTransactionData_IsTagPresentAndNotEmpty (unsigned long ulTag) {
	int nResult = FALSE;
	int cr;
	unsigned char * pReadValue;
	unsigned long ulReadLength;
	int nPosition = SHARED_EXCHANGE_POSITION_NULL;

	if (gs_pGlobalTransactionData) {
		cr = GTL_SharedExchange_FindNext(gs_pGlobalTransactionData, &nPosition, ulTag, &ulReadLength, (const unsigned char **)&pReadValue);
		if (cr == STATUS_SHARED_EXCHANGE_OK) {
			nResult = (ulReadLength > 0);
		} else {
			// Transaction type is not present, use the default value
			GTL_Traces_TraceDebug ("CS_GlobalTransactionData_GetTransactionType : Transaction type is not present in gs_pGlobalTransactionData, use the default value");
		}
	} else {
		GTL_Traces_TraceDebug ("CS_GlobalTransactionData_GetTransactionType : gs_pGlobalTransactionData is NULL");
	}

	return (nResult);
}
