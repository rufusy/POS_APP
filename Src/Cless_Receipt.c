/**
 * \author	Ingenico
 * \author	Copyright (c) 2008 Ingenico, rue claude Chappe,\n
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
#include "Cless_Logos.h"


/////////////////////////////////////////////////////////////////
//// Macros & preprocessor definitions //////////////////////////


/////////////////////////////////////////////////////////////////
//// Define the transaction card provider type //////////////////


/////////////////////////////////////////////////////////////////
//// Static functions definition ////////////////////////////////

static void __Cless_Receipt_FormatAmount(unsigned long ulAmount, int bNegative, int nCurrencyExponent, int nWidth, char* pString);


/////////////////////////////////////////////////////////////////
//// Functions //////////////////////////////////////////////////

//! \brief Format the amount for printing purpose.
//! \param[in] ulAmount Amount to be formatted.
//! \param[in] bNegative Indicates if a minus character shall be displayed or not. 
//! \param[in] nCurrencyExponent Currency exponent (for decimal separator location).
//! \param[in] nWidth 
//! \param[out] pString Formatted string for receipt printing.

static void __Cless_Receipt_FormatAmount(unsigned long ulAmount, int bNegative, int nCurrencyExponent, int nWidth, char* pString)
{
	char Amount[10 + 1];
	char String[20];
	int nSrcIndex;
	int nDestIndex;
	int bDigitDetected;
	char* pPtr;

	Telium_Sprintf(Amount, "%010lu", ulAmount);

	bDigitDetected = FALSE;
	nDestIndex = 0;
	memset(String, 0, sizeof(String));

	for(nSrcIndex = 0; nSrcIndex < 10; nSrcIndex++)
	{
		if (!bDigitDetected)
		{
			if (Amount[nSrcIndex] != '0')
				bDigitDetected = TRUE;
			else if (nSrcIndex == 10 - 1 - nCurrencyExponent)
				bDigitDetected = TRUE;
		}

		if (bDigitDetected)
		{
			String[nDestIndex++] = Amount[nSrcIndex];

			if (nSrcIndex < 10 - 1 - nCurrencyExponent)
			{
				if (((10 - 1 - nCurrencyExponent - nSrcIndex) % 3) == 0)
					String[nDestIndex++] = ' ';
			}
			else if (nSrcIndex == 10 - 1 - nCurrencyExponent)
			{
				// If exponent = 0, do not print a decimal separator
				if (nCurrencyExponent != 0)
					String[nDestIndex++] = '.';
			}
		}
	}

	String[nDestIndex] = '\0';

	nDestIndex = strlen(String);
	if (bNegative)
		nDestIndex++;

	pPtr = pString;
	while(nDestIndex < nWidth)
	{
		*(pPtr++) = ' ';
		nDestIndex++;
	}

	if (bNegative)
		*(pPtr++) = '-';

	strcpy(pPtr, String);
}

void ClessSampleFormatSpendingAmount(const unsigned char * pAvailableOfflineSpendingAmount, unsigned int nLengthSpendingAmount, char * szSpendingAmount)
{
	char acStr[13];
	char * ptr;
	int index;
	int nLgStr;
	
	if (szSpendingAmount == NULL)
		return;
	
	if ((pAvailableOfflineSpendingAmount == NULL) || (nLengthSpendingAmount == 0))
	{
		*szSpendingAmount = 0; // String end...
		return;
	}
	
	ptr = acStr; 
	GTL_Convert_DcbNumberToAscii((const unsigned char*)pAvailableOfflineSpendingAmount, ptr, nLengthSpendingAmount, 12);

	index = 0;
	while ((ptr[index] == '0') && (index < 10))
		index++;
	ptr+=index;

	nLgStr = strlen(ptr);
	strcpy (szSpendingAmount, (const char *)Cless_Fill_GiveMoneyLabel());
	if (nLgStr > 2)
	{
		strcat (szSpendingAmount, " ");
		strncat (szSpendingAmount, ptr, nLgStr - 2);
		szSpendingAmount[strlen((const char *)Cless_Fill_GiveMoneyLabel()) + nLgStr-1] = 0;
		strcat (szSpendingAmount, ",");
		strncat (szSpendingAmount, &ptr[nLgStr-2], 2);
	}
	else
	{
		strcat (szSpendingAmount, " 0,");
		strcat (szSpendingAmount, ptr);
	}
}

static void _Cless_Receipt_PrintFooter(void)
{
	Cless_DumpData_DumpNewLine();
	Cless_DumpData_DumpNewLine();
	if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER) // Bitmaps only works with a printer
		Cless_DumpData ("\xF1\xF2");
	else
		Cless_DumpData ("-------------------------");
	Cless_DumpData_DumpNewLine();
	Cless_DumpData_DumpNewLine();

	Cless_DumpData("  Let's keep in touch !");

	Cless_DumpData_DumpNewLine();
	Cless_DumpData_DumpNewLine();

	if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER) // Different presentation if on a printer or in text mode
	{
		Cless_DumpData("       INGENICO");
		Cless_DumpData_DumpNewLine();
		Cless_DumpData("\x0F");
		Cless_DumpData("      28-32, boulevard de Grenelle");
		Cless_DumpData_DumpNewLine();
		Cless_DumpData("\x0F");
		Cless_DumpData("          75015 Paris - FRANCE");
		Cless_DumpData_DumpNewLine();
		Cless_DumpData("\x0F");
		Cless_DumpData("Tel:+33 158 01 80 00        Fax:+33 158 01 91 35");
		Cless_DumpData("\x1b" "@");
		Cless_DumpData_DumpNewLine();
		Cless_DumpData ("\xF1\xF2");
	}
	else
	{
		Cless_DumpData("       INGENICO");Cless_DumpData_DumpNewLine();
		Cless_DumpData("28-32, boulevard de Grenelle");Cless_DumpData_DumpNewLine();
		Cless_DumpData("     75015 Paris");Cless_DumpData_DumpNewLine();
		Cless_DumpData("   Tel:+33 158 01 80 00");Cless_DumpData_DumpNewLine();
		Cless_DumpData("   Fax:+33 158 01 91 35");Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();
		Cless_DumpData("---------------------------");
		Telium_Ttestall (0,5);
	}
}

//! \brief Print a transaction ticket (for cardholder or merchant) at the end of a transaction or print transaction batch data, according to input parameter (nTicketType).
//! \param[in] pTransaction Shared exchange buffer containing all the transaction data.
//! \param[in] nTicketType Ticket type.
//! \param[in] nTransactionResult APPROVED_TICKED or DECLINED_TICKED
//! \param[in] bSignatureRequested Indicates if signature is requested or not (\a WITH_SIGNATURE if requested, \a WITHOUT_SIGNATURE else).
//! \param[in] nCardHolderLang Indicates Cardholder lang to use.
//! \param[in] nAOSA : with Available Offline Spending Amount : WITH_AOSA or WITHOUT_AOSA
//! \note Make sure the output driver is already opened before calling this function.

void Cless_Receipt_PrintTransaction (T_SHARED_DATA_STRUCT * pTransaction, int nTicketType, int nTransactionResult, const int bSignatureRequested, int nCardHolderLang, int nAOSA)
{
	int nResult, nPosition;
	unsigned long ulTag;
	unsigned long ulReadLength;
	const unsigned char * pValue;
	const unsigned char* pValueStatusCode;
	const unsigned char* pValueTsc;
	unsigned int LengthTsc;
	const unsigned char* pValueAid;
	unsigned int LengthAid;
	//const unsigned char* pValueCid;
	const unsigned char* pValueCvrResult;
	const unsigned char* pValueTvr;
	const unsigned char* pValueAmountBin;
	const unsigned char* pValueAmountNum;
	const unsigned char* pValueAmountOtherBin;
	const unsigned char* pValueAmountOtherNum;
	const unsigned char* pValueExpirationDate;
	const unsigned char* pValuePan;
	unsigned int LengthPan;
	const unsigned char* pValuePanSeq;
	const unsigned char* pValueAuthorCode;
	const unsigned char* pValueAuthorRespCode;
	const unsigned char* pValueCurrencyCode;
	const unsigned char* pValueCurrencyExp;
	const unsigned char* pValueDate;
	const unsigned char* pValueTime;
	const unsigned char* pValueType;
	//const unsigned char* pValueForcedOnline;
	const unsigned char* pAvailableOfflineSpendingAmount;
	unsigned int length_spending_amount;
	const unsigned char* pVisaWaveAvailableOfflineSpendingAmount;
	unsigned int length_VisaWave_spending_amount;
	//const unsigned char* pValueCardType;
	unsigned char temp;
	const unsigned char* pValueApplicationLabel;
	unsigned int length_application_label;
	const unsigned char* pValueAppliPreferedName;
	unsigned int length_appli_prefered_name;
	unsigned long ulStatusCode;
	unsigned long ulTsc;
	unsigned char Aid[20];
	//unsigned char Cid;
	unsigned char CvrResult[3];
	unsigned char Tvr[5];
	unsigned long ulAmount;
	unsigned long ulAmountOther;
	char ExpirationDate[6 + 1];
	char Pan[19 + 1];
	unsigned long ulPanSeq;
	char AuthorCode[6 + 1];
	char AuthorRespCode[2 + 1];
	unsigned long ulCurrencyCode;
	unsigned long ulCurrencyExp;
	char Date[6 + 1];
	char Time[6 + 1];
	unsigned long ulType;
	//int bForcedOnline;
	char szAmount[20];
	char szSpendingAmount[20];
	char szVisaWaveSpendingAmount[20];
	//char transactionKind;
	//unsigned long ulAvailableOfflineSpendingAmount;
	//unsigned short usCardType;
	unsigned char ApplicationLabel [16 + 1]; // Application label + '\0'
	unsigned char AppliPreferedName [16 + 1]; // Application prefered name + '\0'
	MSGinfos tDisplayMsg;

	
	if (!Cless_DumpData_DumpOpenOutputDriver())
		return;

	pValueStatusCode = NULL;
	pValueTsc = NULL;
	LengthTsc = 0;
	pValueAid = NULL;
	LengthAid = 0;
	//pValueCid = NULL;
	pValueCvrResult = NULL;
	pValueTvr = NULL;
	pValueAmountBin = NULL;
	pValueAmountNum = NULL;
	pValueAmountOtherBin = NULL;
	pValueAmountOtherNum = NULL;
	pValueExpirationDate = NULL;
	pValuePan = NULL;
	LengthPan = 0;
	pValuePanSeq = NULL;
	pValueAuthorCode = NULL;
	pValueAuthorRespCode = NULL;
	pValueCurrencyCode = NULL;
	pValueCurrencyExp = NULL;
	pValueDate = NULL;
	pValueTime = NULL;
	pValueType = NULL;
	//pValueForcedOnline = NULL;
	pAvailableOfflineSpendingAmount = NULL;
	length_spending_amount = 0;
	pVisaWaveAvailableOfflineSpendingAmount = NULL;
	length_VisaWave_spending_amount = 0;
	//transactionKind =0;
	//pValueCardType = NULL;
	//usCardType = 0;
	pValueApplicationLabel = NULL;
	length_application_label = 0;
	pValueAppliPreferedName = NULL;
	length_appli_prefered_name = 0;
	Pan[0] = 0;


	Telium_DefprinterpatternHR (0, 0, 0xF1, (char *) Line_part_1);
	Telium_DefprinterpatternHR (0, 0, 0xF2, (char *) Line_part_2);
	Telium_DefprinterpatternHR (0, 0, 0xF3, (char *) Visa_payWave_part_1);
	Telium_DefprinterpatternHR (0, 0, 0xF4, (char *) Visa_payWave_part_2);
	Telium_DefprinterpatternHR (0, 0, 0xF7, (char *) Ingenico_part_1);
	Telium_DefprinterpatternHR (0, 0, 0xF8, (char *) Ingenico_part_2);
	Telium_DefprinterpatternHR (0, 0, 0xF9, (char *) Signature_part_1);
	Telium_DefprinterpatternHR (0, 0, 0xFA, (char *) Signature_part_2);
	Telium_DefprinterpatternHR (0, 0, 0xFD, (char *) Paypass_only_part_1);
	Telium_DefprinterpatternHR (0, 0, 0xFE, (char *) Paypass_only_part_2);
	Telium_DefprinterpatternHR (0, 0, 0xEE, (char *) AmericanExpress_part_1);
	Telium_DefprinterpatternHR (0, 0, 0xEF, (char *) AmericanExpress_part_2);
	Telium_DefprinterpatternHR (0, 0, 0xEC, (char *) qPBOC);
	Telium_DefprinterpatternHR (0, 0, 0xEC, (char *) Discover_part_1);
	Telium_DefprinterpatternHR (0, 0, 0xED, (char *) Discover_part_2);
	Telium_DefprinterpatternHR (0, 0, 0xEA, (char *) Interac_only_part_1);
	Telium_DefprinterpatternHR (0, 0, 0xEB, (char *) Interac_only_part_2);

	nPosition = SHARED_EXCHANGE_POSITION_NULL;
	
	do{
    	// Try to following tag.
    	nResult = GTL_SharedExchange_GetNext (pTransaction, &nPosition, &ulTag, &ulReadLength, &pValue);

        if (nResult == STATUS_SHARED_EXCHANGE_OK)  // If tag found
        {
			switch(ulTag)
			{
			case TAG_KERNEL_STATUS_CODE:					pValueStatusCode = pValue; break;
    		case TAG_EMV_TRANSACTION_SEQUENCE_COUNTER:		pValueTsc = pValue;	LengthTsc = ulReadLength; break;
    		case TAG_EMV_AID_CARD:							pValueAid = pValue;	LengthAid = ulReadLength; break;
    		case TAG_EMV_DF_NAME:							pValueAid = pValue;	LengthAid = ulReadLength; break;
    		//case TAG_EMV_CRYPTOGRAM_INFO_DATA:				pValueCid = pValue; break;
    		case TAG_EMV_CVM_RESULTS:						pValueCvrResult = pValue; break;
    		case TAG_EMV_TVR:								pValueTvr = pValue; break;
    		case TAG_EMV_AMOUNT_AUTH_BIN:					pValueAmountBin = pValue; break;
    		case TAG_EMV_AMOUNT_AUTH_NUM:					pValueAmountNum = pValue; break;
    		case TAG_EMV_AMOUNT_OTHER_BIN:					pValueAmountOtherBin = pValue; break;
    		case TAG_EMV_AMOUNT_OTHER_NUM:					pValueAmountOtherNum = pValue; break;
    		case TAG_EMV_APPLI_EXPIRATION_DATE:				pValueExpirationDate = pValue; break;
    		case TAG_EMV_APPLI_PAN:							pValuePan = pValue;	LengthPan = ulReadLength; break;
    		case TAG_EMV_APPLI_PAN_SEQUENCE_NUMBER:			pValuePanSeq = pValue; break;
    		case TAG_EMV_AUTHORISATION_CODE:				pValueAuthorCode = pValue; break;
    		case TAG_EMV_AUTHORISATION_RESPONSE_CODE:		pValueAuthorRespCode = pValue; break;
    		case TAG_EMV_TRANSACTION_CURRENCY_CODE:			pValueCurrencyCode = pValue; break;
    		case TAG_EMV_TRANSACTION_CURRENCY_EXPONENT:		pValueCurrencyExp = pValue; break;
    		case TAG_EMV_TRANSACTION_DATE:					pValueDate = pValue; break;
    		case TAG_EMV_TRANSACTION_TIME:					pValueTime = pValue; break;
    		case TAG_EMV_TRANSACTION_TYPE:					pValueType = pValue; break;
    		//case TAG_KERNEL_CARD_TYPE:						pValueCardType=pValue; break;
    		case TAG_EMV_APPLICATION_LABEL:					pValueApplicationLabel=pValue; length_application_label = ulReadLength; break;
			case TAG_EMV_APPLI_PREFERED_NAME:				pValueAppliPreferedName=pValue; length_appli_prefered_name = ulReadLength; break;
    		case TAG_PAYWAVE_AVAILABLE_OFFLINE_SPENDING_AMOUNT: pAvailableOfflineSpendingAmount=pValue; length_spending_amount = ulReadLength; break;
#ifndef DISABLE_OTHERS_KERNELS
			case TAG_VISAWAVE_VLP_AVAILABLE_FUNDS:			pVisaWaveAvailableOfflineSpendingAmount=pValue; length_VisaWave_spending_amount = ulReadLength; break;
#endif
			default:
				break;
			}
        }
        else if (nResult == STATUS_SHARED_EXCHANGE_END)  // Buffer end reached
        {
        }
        else // error or end or ...
        {
        	GTL_Traces_TraceDebug("Display tags error:%x\n", nResult);
        }
    }while (nResult == STATUS_SHARED_EXCHANGE_OK);



	ulStatusCode = 0;
	ulTsc = 0;
	memset(Aid, 0, sizeof(Aid));
	//Cid = 0;
	memset(CvrResult, 0, sizeof(CvrResult));
	memset(Tvr, 0, sizeof(Tvr));
	ulAmount = 0;
	ulAmountOther = 0;
	memset(ExpirationDate, '?', sizeof(ExpirationDate));
	memset(Pan, '?', sizeof(Pan));
	Pan[sizeof(Pan) - 1] = 0;
	ulPanSeq = 0;
	memset(AuthorCode, 0, sizeof(AuthorCode));
	memset(AuthorRespCode, 0, sizeof(AuthorRespCode));
	ulCurrencyCode = 0;
	ulCurrencyExp = 0;
	memset(Date, '?', sizeof(Date));
	memset(Time, '?', sizeof(Time));
	ulType = 0;
	//bForcedOnline = FALSE;
	//ulAvailableOfflineSpendingAmount = 0;
	memset (szSpendingAmount, '\0', sizeof(szSpendingAmount));
	memset (szVisaWaveSpendingAmount, '\0', sizeof(szVisaWaveSpendingAmount));
	memset (ApplicationLabel, '\0', sizeof(ApplicationLabel));
	memset (AppliPreferedName, '\0', sizeof(AppliPreferedName));

	if (pValueStatusCode != NULL)
		GTL_Convert_BinNumberToUl(pValueStatusCode, &ulStatusCode, 2);
	if (pValueTsc != NULL)
		GTL_Convert_DcbNumberToUl(pValueTsc, &ulTsc, LengthTsc);
	if (pValueAid != NULL)
		memcpy(Aid, pValueAid, LengthAid);
	/*if (pValueCid != NULL)
		Cid = *pValueCid;*/
	if (pValueCvrResult != NULL)
		memcpy(CvrResult, pValueCvrResult, sizeof(CvrResult));
	if (pValueTvr != NULL)
		memcpy(Tvr, pValueTvr, sizeof(Tvr));
	if (pValueAmountBin != NULL)
		GTL_Convert_BinNumberToUl(pValueAmountBin, &ulAmount, 4);
	if (pValueAmountOtherBin != NULL)
		GTL_Convert_BinNumberToUl(pValueAmountOtherBin, &ulAmountOther, 4);
	if (pValueAmountNum != NULL)
		GTL_Convert_DcbNumberToUl(pValueAmountNum, &ulAmount, 6);
	if (pValueAmountOtherNum != NULL)
		GTL_Convert_DcbNumberToUl(pValueAmountOtherNum, &ulAmountOther, 6);
	if (pValueExpirationDate != NULL)
		GTL_Convert_DcbNumberToAscii(pValueExpirationDate, ExpirationDate, 3, 6);
	if (pValuePan != NULL)
		GTL_Convert_DcbPaddedToAscii(pValuePan, Pan, LengthPan);
	if (pValuePanSeq != NULL)
		GTL_Convert_DcbNumberToUl(pValuePanSeq, &ulPanSeq, 1);
	if (pValueAuthorCode != NULL)
		memcpy(AuthorCode, pValueAuthorCode, 6);
	if (pValueAuthorRespCode != NULL)
		memcpy(AuthorRespCode, pValueAuthorRespCode, 2);
	if (pValueCurrencyCode != NULL)
		GTL_Convert_DcbNumberToUl(pValueCurrencyCode, &ulCurrencyCode, 2);
	if (pValueCurrencyExp != NULL)
		GTL_Convert_DcbNumberToUl(pValueCurrencyExp, &ulCurrencyExp, 1);
	if (pValueDate != NULL)
		GTL_Convert_DcbNumberToAscii(pValueDate, Date, 3, 6);
	if (pValueTime != NULL)
		GTL_Convert_DcbNumberToAscii(pValueTime, Time, 3, 6);
	if (pValueType != NULL)
		GTL_Convert_DcbNumberToUl(pValueType, &ulType, 1);
/*	if (pValueForcedOnline != NULL)
	{
		if (*pValueForcedOnline)
			bForcedOnline = TRUE;
	}*/
	
	if (pAvailableOfflineSpendingAmount != NULL) {
		ClessSampleFormatSpendingAmount(pAvailableOfflineSpendingAmount, length_spending_amount, szSpendingAmount);
	}

	if (pVisaWaveAvailableOfflineSpendingAmount != NULL) {
		ClessSampleFormatSpendingAmount(pVisaWaveAvailableOfflineSpendingAmount, length_VisaWave_spending_amount, szVisaWaveSpendingAmount);
	}
	
	//transactionKind = UNKNOWN_TRANSACTION;
	/*if (pValueCardType != NULL)
		usCardType = (pValueCardType[0] << 8) + pValueCardType[1];*/

	if (pValueApplicationLabel != NULL)
		memcpy (ApplicationLabel, pValueApplicationLabel, length_application_label);

	if (pValueAppliPreferedName != NULL)
		memcpy (AppliPreferedName, pValueAppliPreferedName, length_appli_prefered_name);

	// Print a VISA or qPBOC ticket
	if ((nTicketType == BATCH_TICKET_VISA) || (nTicketType == BATCH_TICKET_PBOC))
	{
		int nLen;

		__Cless_Receipt_FormatAmount(ulAmount, FALSE, ulCurrencyExp, 0, szAmount);

		if (pValuePan != NULL)
		{
			nLen = strlen(Pan);
			if (nLen)
			{
				Pan[nLen - 1] = '-';
				if (nLen > 6)
					memset(Pan, '-', 6);
				else
					memset(Pan, '-', nLen-1);
			}
		}

		if (nTicketType == BATCH_TICKET_VISA)
		{
#ifndef DISABLE_OTHERS_KERNELS
			if(VisaWave_isVisaWaveProcessing())
			{
				if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER) // Bitmaps only works with a printer
				{
					Cless_DumpData("\xF7\xF8");
				}
				else
				{
					Cless_DumpData("#       #           #         #                    )");Cless_DumpData_DumpNewLine();
					Cless_DumpData(" #     #   ###       #   #   #                   )  )");Cless_DumpData_DumpNewLine();
					Cless_DumpData("  #   #  # #     ###  # # # # ### #   # ##     )  )  )");Cless_DumpData_DumpNewLine();
					Cless_DumpData("   # #   #   #  #  #   #   # #  #  # # ## #  )  )  ) )");Cless_DumpData_DumpNewLine();
					Cless_DumpData("    #    # ###   ####  #   #  ####  #   ##     )  )  )");Cless_DumpData_DumpNewLine();
					Cless_DumpData("                                                 )  )");Cless_DumpData_DumpNewLine();
					Cless_DumpData("                                                   ) ");Cless_DumpData_DumpNewLine();
				}
			}
			else
#endif // DISABLE_OTHERS_KERNELS
			{
				if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER) // Bitmaps only works with a printer
				{
					Cless_DumpData("\xF3\xF4");
				}
				else
				{
					Cless_DumpData("               #         #                    )");Cless_DumpData_DumpNewLine();
					Cless_DumpData("                #   #   #                   )  )");Cless_DumpData_DumpNewLine();
					Cless_DumpData(" ###   ### #   # # # # # ### #   # ##     )  )  )");Cless_DumpData_DumpNewLine();
					Cless_DumpData(" #  # #  #  # #   #   # #  #  # # ## #  )  )  ) )");Cless_DumpData_DumpNewLine();
					Cless_DumpData(" ###   ####  #    #   #  ####  #   ##     )  )  )");Cless_DumpData_DumpNewLine();
					Cless_DumpData(" #          #                               )  )");Cless_DumpData_DumpNewLine();
					Cless_DumpData(" #         #                                  ) ");Cless_DumpData_DumpNewLine();
				}
			}
		}
		else // if (nTicketType == BATCH_TICKET_PBOC))
		{
			if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER) // Bitmaps only works with a printer
			{
				Cless_DumpData("\xEC");
			}
			else
			{
				Cless_DumpData("           ####  ####    ###   ###            )");Cless_DumpData_DumpNewLine();
				Cless_DumpData("           #   # #   #  #   # #             )  )");Cless_DumpData_DumpNewLine();
				Cless_DumpData("       ### ####  #### # #   # #           )  )  )");Cless_DumpData_DumpNewLine();
				Cless_DumpData("      #  # #     #   #  #   # #         )  )  ) )");Cless_DumpData_DumpNewLine();
				Cless_DumpData("       ### #     ####    ###   ###        )  )  )");Cless_DumpData_DumpNewLine();
				Cless_DumpData("         #                                  )  )");Cless_DumpData_DumpNewLine();
				Cless_DumpData("         #                                    ) ");Cless_DumpData_DumpNewLine();
			}
		}
		
		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();

		if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER) // Bitmaps only works with a printer
			Cless_DumpData ("\xF1\xF2");
		else
			Cless_DumpData ("---------------------------");

		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();

		// Display the application label if available
		if (pValueApplicationLabel != NULL)
		{
			for (temp=0;temp<(unsigned char)length_application_label;temp++)
			{
				if (!(((ApplicationLabel[temp] >= 0x30) && (ApplicationLabel[temp] <= 0x39))
					|| ((ApplicationLabel[temp] >= 0x41) && (ApplicationLabel[temp] <= 0x5A))
					|| ((ApplicationLabel[temp] >= 0x61) && (ApplicationLabel[temp] <= 0x7A))))
				{
					ApplicationLabel[temp] = 0x20;
				}
			}

			Cless_DumpData("\x1b" "@");
			Cless_DumpData("%s", ApplicationLabel);
			Cless_DumpData_DumpNewLine();
		}

		for (temp=0;temp<(unsigned char)LengthAid;temp++)
			Cless_DumpData("%02x", Aid[temp]);

		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();

		Cless_DumpData("\x1b" "@\x1b" "E");

		
		if (ulType == CLESS_SAMPLE_TRANSACTION_TYPE_DEBIT)       // 0x00
		{
			Cless_Term_Read_Message(STD_MESS_DEBIT, nCardHolderLang, &tDisplayMsg);
			Cless_DumpData(tDisplayMsg.message);
		}
		else if (ulType == CLESS_SAMPLE_TRANSACTION_TYPE_CASH)   // 0x01
		{
			Cless_Term_Read_Message(STD_MESS_CASH, nCardHolderLang, &tDisplayMsg);
			Cless_DumpData(tDisplayMsg.message);
		}
		else if (ulType == CLESS_SAMPLE_TRANSACTION_TYPE_REFUND) // 0x20
		{
			Cless_Term_Read_Message(STD_MESS_REFUND, nCardHolderLang, &tDisplayMsg);
			Cless_DumpData(tDisplayMsg.message);
		}
		else
		{
			Cless_Term_Read_Message(STD_MESS_TRANSAC_TYPE, nCardHolderLang, &tDisplayMsg);
			Cless_DumpData("%s %lu", tDisplayMsg.message, ulType);
		}

		Cless_DumpData("\x1b" "@");
		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();
		Cless_DumpData ("%c%c/%c%c/%c%c %c%c:%c%c:%c%c", Date[4], Date[5], Date[2], Date[3], Date[0], Date[1], Time[0], Time[1], Time[2], Time[3], Time[4], Time[5]);
		Cless_DumpData_DumpNewLine();

		if (pValuePan != NULL)
		{
			Cless_DumpData ("\x1b" "@");
			Cless_DumpData ("%s", Pan);
			Cless_DumpData_DumpNewLine();
		}
		else
		{
			Cless_DumpData_DumpNewLine();
		}

		if (pValueExpirationDate != NULL)
		{
			Cless_DumpData ("\x1b" "@");
			Cless_DumpData ("%c%c/%c%c", ExpirationDate[2], ExpirationDate[3], ExpirationDate[0], ExpirationDate[1]);
			Cless_DumpData_DumpNewLine();
			Cless_DumpData_DumpNewLine();
		}
		else
		{
			Cless_DumpData_DumpNewLine();
		}

		if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER)
			Cless_DumpData ("\x1b" "@" "\x1b" "E");

        if (nTransactionResult == APPROVED_TICKED)
		    Cless_Term_Read_Message(STD_MESS_APPROVED_TICKET, nCardHolderLang, &tDisplayMsg);
		else // (nTransactionResult == DECLINED_TICKED)
		    Cless_Term_Read_Message(STD_MESS_DECLINED_TICKET, nCardHolderLang, &tDisplayMsg);
		Cless_DumpData (tDisplayMsg.message);

		Cless_DumpData ("\x1b" "@");
		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();
		Cless_Term_Read_Message(STD_MESS_AMOUNT, nCardHolderLang, &tDisplayMsg);
		Cless_DumpData (tDisplayMsg.message);
		
		if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER)
			Cless_DumpData ("\x1b" "E");

		Cless_DumpData ("%s %s", Cless_Fill_GiveMoneyLabel(), szAmount);
		
		
		if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER)
			Cless_DumpData ("\x1b" "@");
		Cless_DumpData_DumpNewLine();

		if (nAOSA == WITH_AOSA)
		{
#ifndef DISABLE_OTHERS_KERNELS
			if(VisaWave_isVisaWaveProcessing())
			{
				if (pVisaWaveAvailableOfflineSpendingAmount != NULL)
				{
					if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER)
						Cless_DumpData ("\x0F");

					Cless_Term_Read_Message(STD_MESS_AVAILABLE, nCardHolderLang, &tDisplayMsg);
					Cless_DumpData (tDisplayMsg.message);
					if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER)
						Cless_DumpData ("\x1b" "@");

					Cless_DumpData ("%s", szVisaWaveSpendingAmount);

					if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER)
						Cless_DumpData ("\x1b" "@");
					Cless_DumpData_DumpNewLine();
				}
			}
			else
#endif
			{
				if (pAvailableOfflineSpendingAmount != NULL)
				{
					if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER)
						Cless_DumpData ("\x0F");

					Cless_Term_Read_Message(STD_MESS_AVAILABLE, nCardHolderLang, &tDisplayMsg);
					Cless_DumpData (tDisplayMsg.message);
					if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER)
						Cless_DumpData ("\x1b" "@");

					Cless_DumpData ("%s", szSpendingAmount);

					if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER)
						Cless_DumpData ("\x1b" "@");
					Cless_DumpData_DumpNewLine();
				}
			}
		} // else (nAOSA == WITHOUT_AOSA) => no action

		if (bSignatureRequested == WITH_SIGNATURE)
		{
			Cless_DumpData_DumpNewLine();

			if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER) // Bitmaps only works with a printer
			{
				Cless_DumpData ("\xF9\xFA");
			}
			else
			{
				Cless_DumpData (" *** Signature ******************* ");Cless_DumpData_DumpNewLine();
				Cless_DumpData ("*                                 *");Cless_DumpData_DumpNewLine();
				Cless_DumpData ("*                                 *");Cless_DumpData_DumpNewLine();
				Cless_DumpData ("*                                 *");Cless_DumpData_DumpNewLine();
				Cless_DumpData (" ********************************* ");
			}
		}
		
		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();
		Cless_DumpData("Love Every Quick Payment");
		Cless_DumpData_DumpNewLine();

		if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER) // Bitmaps only works with a printer
			Cless_DumpData ("\xF1\xF2");
		else
			Cless_DumpData ("-------------------------");

		Cless_DumpData_DumpNewLine();
		Cless_DumpData("  And keep in touch !");
		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();

		if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER) // Different presentation if on a printer or in text mode
		{
			Cless_DumpData("      Ingenico");
			Cless_DumpData_DumpNewLine();
			Cless_DumpData("\x0F");
			Cless_DumpData("           1, rue Claude Chappe BP 346");
			Cless_DumpData_DumpNewLine();
			Cless_DumpData("\x0F");
			Cless_DumpData("      07 503  Guilherand-Granges - FRANCE");
			Cless_DumpData_DumpNewLine();
			Cless_DumpData("\x0F");
			Cless_DumpData("Tel:+33 475 81 40 40        Fax:+33 475 81 43 00");
			Cless_DumpData("\x1b" "@");
			Cless_DumpData_DumpNewLine();
			Cless_DumpData_DumpNewLine();
			Cless_DumpData ("\xF1\xF2");
		}
		else
		{
			Cless_DumpData("      Ingenico");Cless_DumpData_DumpNewLine();
			Cless_DumpData("1, rue Claude Chappe BP 346");Cless_DumpData_DumpNewLine();
			Cless_DumpData(" 07 503 Guilherand-Granges");Cless_DumpData_DumpNewLine();
			Cless_DumpData("    Tel:+33 475 81 40 40");Cless_DumpData_DumpNewLine();
			Cless_DumpData("    Fax:+33 475 81 43 00");
			Cless_DumpData_DumpNewLine();
			Cless_DumpData_DumpNewLine();
			Cless_DumpData ("-------------------------");
			Telium_Ttestall (0,5);
		}

		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();
	}

	// Print a Mastercard ticket
	else if (nTicketType == BATCH_TICKET_MASTERCARD)
	{
		int nLen;

		__Cless_Receipt_FormatAmount(ulAmount, FALSE, ulCurrencyExp, 0, szAmount);

		if (pValuePan != NULL)
		{
			nLen = strlen(Pan);
			if (nLen)
			{
				Pan[nLen - 1] = '-';
				if (nLen > 6)
					memset(Pan, '-', 6);
				else
					memset(Pan, '-', nLen-1);
			}
		}

		if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER) // Bitmaps only works with a printer
		{
			Cless_DumpData("\xFD\xFE");
		}
		else
		{
			Cless_DumpData(" ####            #### ");Cless_DumpData_DumpNewLine();
			Cless_DumpData(" #   #           #   #");Cless_DumpData_DumpNewLine();
			Cless_DumpData(" ####  ### #   # ####  ###   ###  ###");Cless_DumpData_DumpNewLine();
			Cless_DumpData(" #    #  #  # #  #    #  #  #### ####");Cless_DumpData_DumpNewLine();
			Cless_DumpData(" #     ####  #   #     #### ###  ###");Cless_DumpData_DumpNewLine();
			Cless_DumpData("            #");Cless_DumpData_DumpNewLine();
			Cless_DumpData("           #");Cless_DumpData_DumpNewLine();
		}
			
		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();
		
		if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER) // Bitmaps only works with a printer
			Cless_DumpData ("\xF1\xF2");
		else
			Cless_DumpData ("-------------------------");

		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();

		Cless_DumpData ("\x1b" "@\x1b" "E");

		// Display the application label if available
		if (pValueApplicationLabel != NULL)
		{
			for (temp=0;temp<(unsigned char)length_application_label;temp++)
			{
				if (!(((ApplicationLabel[temp] >= 0x30) && (ApplicationLabel[temp] <= 0x39))
					|| ((ApplicationLabel[temp] >= 0x41) && (ApplicationLabel[temp] <= 0x5A))
					|| ((ApplicationLabel[temp] >= 0x61) && (ApplicationLabel[temp] <= 0x7A))))
				{
					ApplicationLabel[temp] = 0x20;
				}
			}

			Cless_DumpData("\x1b" "@");
			Cless_DumpData("%s", ApplicationLabel);
			Cless_DumpData_DumpNewLine();
		}

		for (temp=0;temp<(unsigned char)LengthAid;temp++)
			Cless_DumpData("%02x", Aid[temp]);

		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();

		Cless_DumpData("\x1b" "@\x1b" "E");

		if (ulType == 0)
		{
			Cless_Term_Read_Message(STD_MESS_DEBIT, nCardHolderLang, &tDisplayMsg);
			Cless_DumpData(tDisplayMsg.message);
		}
		else
		{
			Cless_Term_Read_Message(STD_MESS_TRANSAC_TYPE, nCardHolderLang, &tDisplayMsg);
			Cless_DumpData("%s %lu", tDisplayMsg.message, ulType);
		}

		Cless_DumpData ("\x1b" "@");
		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();
		Cless_DumpData ("%c%c/%c%c/%c%c %c%c:%c%c:%c%c", Date[4], Date[5], Date[2], Date[3], Date[0], Date[1], Time[0], Time[1], Time[2], Time[3], Time[4], Time[5]);
		Cless_DumpData_DumpNewLine();
		Cless_DumpData ("%s", Pan);
		Cless_DumpData_DumpNewLine();
		Cless_DumpData ("%c%c/%c%c", ExpirationDate[2], ExpirationDate[3], ExpirationDate[0], ExpirationDate[1]);
		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();
		Cless_DumpData ("\x1b" "E");

		Cless_Term_Read_Message(STD_MESS_APPROVED_TICKET, nCardHolderLang, &tDisplayMsg);
		Cless_DumpData (tDisplayMsg.message);
		
		Cless_DumpData ("\x1b" "@");
		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();
		
		Cless_Term_Read_Message(STD_MESS_AMOUNT, nCardHolderLang, &tDisplayMsg);
		Cless_DumpData (tDisplayMsg.message);
		
		if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER)
			Cless_DumpData ("\x1b" "E");

		Cless_DumpData ("%s %s", Cless_Fill_GiveMoneyLabel(), szAmount);
		Cless_DumpData ("\x1b" "@");
		Cless_DumpData_DumpNewLine();


		if (bSignatureRequested == WITH_SIGNATURE)
		{
			Cless_DumpData_DumpNewLine();

			if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER) // Bitmaps only works with a printer
			{
				Cless_DumpData ("\xF9\xFA");
			}
			else
			{
				Cless_DumpData (" *** Signature ******************* ");Cless_DumpData_DumpNewLine();
				Cless_DumpData ("*                                 *");Cless_DumpData_DumpNewLine();
				Cless_DumpData ("*                                 *");Cless_DumpData_DumpNewLine();
				Cless_DumpData ("*                                 *");Cless_DumpData_DumpNewLine();
				Cless_DumpData (" ********************************* ");
			}
		}

		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();
		Cless_DumpData("  Let's keep in touch !");
		Cless_DumpData_DumpNewLine();

		if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER) // Bitmaps only works with a printer
			Cless_DumpData ("\xF1\xF2");
		else
			Cless_DumpData ("---------------------------");

		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();

		if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER) // Different presentation if on a printer or in text mode
		{
			Cless_DumpData("      Ingenico");
			Cless_DumpData_DumpNewLine();
			Cless_DumpData("\x0F");
			Cless_DumpData("           1, rue Claude Chappe BP 346");
			Cless_DumpData_DumpNewLine();
			Cless_DumpData("\x0F");
			Cless_DumpData("      07 503  Guilherand-Granges - FRANCE");
			Cless_DumpData_DumpNewLine();
			Cless_DumpData("\x0F");
			Cless_DumpData("Tel:+33 475 81 40 40        Fax:+33 475 81 43 00");
			Cless_DumpData("\x1b" "@");
			Cless_DumpData_DumpNewLine();
			Cless_DumpData_DumpNewLine();
			Cless_DumpData ("\xF1\xF2");
		}
		else
		{
			Cless_DumpData("       Ingenico");Cless_DumpData_DumpNewLine();
			Cless_DumpData("1, rue Claude Chappe BP 346");Cless_DumpData_DumpNewLine();
			Cless_DumpData(" 07 503 Guilherand-Granges");Cless_DumpData_DumpNewLine();
			Cless_DumpData("   Tel:+33 475 81 40 40");Cless_DumpData_DumpNewLine();
			Cless_DumpData("   Fax:+33 475 81 43 00");Cless_DumpData_DumpNewLine();
			Cless_DumpData_DumpNewLine();
			Cless_DumpData("---------------------------");
			Telium_Ttestall (0,5);
		}

		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();
	}
		// Print an ExpressPay ticket
	else if (nTicketType == BATCH_TICKET_AMEX)
	{
		int nLen;

		__Cless_Receipt_FormatAmount(ulAmount, FALSE, ulCurrencyExp, 0, szAmount);

		if (pValuePan != NULL)
		{
			nLen = strlen(Pan);
			if (nLen)
			{
				Pan[nLen - 1] = '-';
				if (nLen > 6)
					memset(Pan, '-', 6);
				else
					memset(Pan, '-', nLen-1);
			}
		}

		if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER) // Bitmaps only works with a printer
		{
			Cless_DumpData("\xEE\xEF");
		}
		else
		{
			Cless_DumpData("  ###             ");Cless_DumpData_DumpNewLine();
			Cless_DumpData(" #   #              ");Cless_DumpData_DumpNewLine();
			Cless_DumpData(" #####  ### ### ###  # # ");Cless_DumpData_DumpNewLine();
			Cless_DumpData(" #   # #  ##  # # #   # ");Cless_DumpData_DumpNewLine();
			Cless_DumpData(" #   # #  #   #  ### # #");Cless_DumpData_DumpNewLine();
		}
			
		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();
		
		if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER) // Bitmaps only works with a printer
			Cless_DumpData ("\xF1\xF2");
		else
			Cless_DumpData ("-------------------------");

		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();

		Cless_DumpData ("\x1b" "@\x1b" "E");

		// Display the application label if available
		if (pValueApplicationLabel != NULL)
		{
			for (temp=0;temp<(unsigned char)length_application_label;temp++)
			{
				if (!(((ApplicationLabel[temp] >= 0x30) && (ApplicationLabel[temp] <= 0x39))
					|| ((ApplicationLabel[temp] >= 0x41) && (ApplicationLabel[temp] <= 0x5A))
					|| ((ApplicationLabel[temp] >= 0x61) && (ApplicationLabel[temp] <= 0x7A))))
				{
					ApplicationLabel[temp] = 0x20;
				}
			}

			Cless_DumpData("\x1b" "@");
			Cless_DumpData("%s", ApplicationLabel);
			Cless_DumpData_DumpNewLine();
		}

		for (temp=0;temp<(unsigned char)LengthAid;temp++)
			Cless_DumpData("%02x", Aid[temp]);

		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();

		Cless_DumpData("\x1b" "@\x1b" "E");

		if (ulType == 0)
		{
			Cless_Term_Read_Message(STD_MESS_DEBIT, nCardHolderLang, &tDisplayMsg);
			Cless_DumpData(tDisplayMsg.message);
		}
		else
		{
			Cless_Term_Read_Message(STD_MESS_TRANSAC_TYPE, nCardHolderLang, &tDisplayMsg);
			Cless_DumpData("%s %lu", tDisplayMsg.message, ulType);
		}

		Cless_DumpData ("\x1b" "@");
		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();
		Cless_DumpData ("%c%c/%c%c/%c%c %c%c:%c%c:%c%c", Date[4], Date[5], Date[2], Date[3], Date[0], Date[1], Time[0], Time[1], Time[2], Time[3], Time[4], Time[5]);
		Cless_DumpData_DumpNewLine();
		Cless_DumpData ("%s", Pan);
		Cless_DumpData_DumpNewLine();
		Cless_DumpData ("%c%c/%c%c", ExpirationDate[2], ExpirationDate[3], ExpirationDate[0], ExpirationDate[1]);
		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();
		Cless_DumpData ("\x1b" "E");

		Cless_Term_Read_Message(STD_MESS_APPROVED_TICKET, nCardHolderLang, &tDisplayMsg);
		Cless_DumpData (tDisplayMsg.message);

		Cless_DumpData ("\x1b" "@");
		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();
		
		Cless_Term_Read_Message(STD_MESS_AMOUNT, nCardHolderLang, &tDisplayMsg);
		Cless_DumpData (tDisplayMsg.message);
		
		if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER)
			Cless_DumpData ("\x1b" "E");

		Cless_DumpData ("%s %s", Cless_Fill_GiveMoneyLabel(), szAmount);
		
		Cless_DumpData ("\x1b" "@");
		Cless_DumpData_DumpNewLine();


		if (bSignatureRequested == WITH_SIGNATURE)
		{
			Cless_DumpData_DumpNewLine();

			if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER) // Bitmaps only works with a printer
			{
				Cless_DumpData ("\xF9\xFA");
			}
			else
			{
				Cless_DumpData (" *** Signature ******************* ");Cless_DumpData_DumpNewLine();
				Cless_DumpData ("*                                 *");Cless_DumpData_DumpNewLine();
				Cless_DumpData ("*                                 *");Cless_DumpData_DumpNewLine();
				Cless_DumpData ("*                                 *");Cless_DumpData_DumpNewLine();
				Cless_DumpData (" ********************************* ");
			}
		}

		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();
		Cless_DumpData("  Let's keep in touch !");
		Cless_DumpData_DumpNewLine();

		if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER) // Bitmaps only works with a printer
			Cless_DumpData ("\xF1\xF2");
		else
			Cless_DumpData ("---------------------------");

		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();
		if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER) // Different presentation if on a printer or in text mode
		{
			Cless_DumpData("      Ingenico");
			Cless_DumpData_DumpNewLine();
			Cless_DumpData("\x0F");
			Cless_DumpData("           1, rue Claude Chappe BP 346");
			Cless_DumpData_DumpNewLine();
			Cless_DumpData("\x0F");
			Cless_DumpData("      07 503  Guilherand-Granges - FRANCE");
			Cless_DumpData_DumpNewLine();
			Cless_DumpData("\x0F");
			Cless_DumpData("Tel:+33 475 81 40 40        Fax:+33 475 81 43 00");
			Cless_DumpData("\x1b" "@");
			Cless_DumpData_DumpNewLine();
			Cless_DumpData_DumpNewLine();
			Cless_DumpData ("\xF1\xF2");
		}
		else
		{
			Cless_DumpData("       Ingenico");Cless_DumpData_DumpNewLine();
			Cless_DumpData("1, rue Claude Chappe BP 346");Cless_DumpData_DumpNewLine();
			Cless_DumpData(" 07 503 Guilherand-Granges");Cless_DumpData_DumpNewLine();
			Cless_DumpData("   Tel:+33 475 81 40 40");Cless_DumpData_DumpNewLine();
			Cless_DumpData("   Fax:+33 475 81 43 00");Cless_DumpData_DumpNewLine();
			Cless_DumpData_DumpNewLine();
			Cless_DumpData("---------------------------");
			Telium_Ttestall (0,5);
		}

		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();
	}
	
	// Print a Discover ticket
	else if (nTicketType == BATCH_TICKET_DISCOVER)
	{
		int nLen;

		__Cless_Receipt_FormatAmount(ulAmount, FALSE, ulCurrencyExp, 0, szAmount);

		if (pValuePan != NULL)
		{
			nLen = strlen(Pan);
			if (nLen)
			{
				Pan[nLen - 1] = '-';
				if (nLen > 6)
					memset(Pan, '-', 6);
				else
					memset(Pan, '-', nLen-1);
			}
		}

		if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER) // Bitmaps only works with a printer
		{
			Cless_DumpData("\xEC\xED");
		}
		else
		{
			Cless_DumpData(" #### ");Cless_DumpData_DumpNewLine();
			Cless_DumpData(" #   #  #");Cless_DumpData_DumpNewLine();
			Cless_DumpData(" #    #     ##  ##  ##  #  #  ##   ##");Cless_DumpData_DumpNewLine();
			Cless_DumpData(" #    # #  #   #   #  # #  # #  # #");Cless_DumpData_DumpNewLine();
			Cless_DumpData(" #    # #  ##  #   #  # #  # ###  #");Cless_DumpData_DumpNewLine();
			Cless_DumpData(" #   #  #    # #   #  #  ##  #    #");Cless_DumpData_DumpNewLine();
			Cless_DumpData(" ####   #  ##   ##  ##   ##   ##  #");Cless_DumpData_DumpNewLine();
		}

		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();

		if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER) // Bitmaps only works with a printer
			Cless_DumpData ("\xF1\xF2");
		else
			Cless_DumpData ("-------------------------");

		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();

		Cless_DumpData ("\x1b" "@\x1b" "E");

		// Display the application label if available
		if (pValueApplicationLabel != NULL)
		{
			for (temp=0;temp<(unsigned char)length_application_label;temp++)
			{
				if (!(((ApplicationLabel[temp] >= 0x30) && (ApplicationLabel[temp] <= 0x39))
					|| ((ApplicationLabel[temp] >= 0x41) && (ApplicationLabel[temp] <= 0x5A))
					|| ((ApplicationLabel[temp] >= 0x61) && (ApplicationLabel[temp] <= 0x7A))))
				{
					ApplicationLabel[temp] = 0x20;
				}
			}

			Cless_DumpData("\x1b" "@");
			Cless_DumpData("%s", ApplicationLabel);
			Cless_DumpData_DumpNewLine();
		}

		// Print the Application Prefered Name
		Cless_DumpData("\x1b" "@");
		if (length_appli_prefered_name)
		{
			Cless_DumpData("%s", AppliPreferedName);
			Cless_DumpData_DumpNewLine();
		}

		for (temp=0;temp<(unsigned char)LengthAid;temp++)
			Cless_DumpData("%02x", Aid[temp]);

		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();

		Cless_DumpData("\x1b" "@\x1b" "E");

		if (ulType == CLESS_SAMPLE_TRANSACTION_TYPE_DEBIT)
		{
			Cless_Term_Read_Message(STD_MESS_DEBIT, nCardHolderLang, &tDisplayMsg);
			Cless_DumpData(tDisplayMsg.message);
		}
		else
		{
			Cless_Term_Read_Message(STD_MESS_TRANSAC_TYPE, nCardHolderLang, &tDisplayMsg);
			Cless_DumpData("%s %lX", tDisplayMsg.message, ulType);
		}

		Cless_DumpData ("\x1b" "@");
		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();
		Cless_DumpData ("%c%c/%c%c/%c%c %c%c:%c%c:%c%c", Date[4], Date[5], Date[2], Date[3], Date[0], Date[1], Time[0], Time[1], Time[2], Time[3], Time[4], Time[5]);
		Cless_DumpData_DumpNewLine();
		Cless_DumpData ("%s", Pan);
		Cless_DumpData_DumpNewLine();
		Cless_DumpData ("%c%c/%c%c", ExpirationDate[2], ExpirationDate[3], ExpirationDate[0], ExpirationDate[1]);
		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();
		Cless_DumpData ("\x1b" "E");

		Cless_Term_Read_Message(STD_MESS_APPROVED_TICKET, nCardHolderLang, &tDisplayMsg);
		Cless_DumpData (tDisplayMsg.message);

		Cless_DumpData ("\x1b" "@");
		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();

		Cless_Term_Read_Message(STD_MESS_AMOUNT, nCardHolderLang, &tDisplayMsg);
		Cless_DumpData (tDisplayMsg.message);

		if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER)
			Cless_DumpData ("\x1b" "E");

		Cless_DumpData ("%s %s", Cless_Fill_GiveMoneyLabel(), szAmount);
		Cless_DumpData ("\x1b" "@");
		Cless_DumpData_DumpNewLine();


		if (bSignatureRequested)
		{
			Cless_DumpData_DumpNewLine();

			if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER) // Bitmaps only works with a printer
			{
				Cless_DumpData ("\xF9\xFA");
			}
			else
			{
				Cless_DumpData (" *** Signature ******************* ");Cless_DumpData_DumpNewLine();
				Cless_DumpData ("*                                 *");Cless_DumpData_DumpNewLine();
				Cless_DumpData ("*                                 *");Cless_DumpData_DumpNewLine();
				Cless_DumpData ("*                                 *");Cless_DumpData_DumpNewLine();
				Cless_DumpData (" ********************************* ");
			}
		}

		_Cless_Receipt_PrintFooter();

		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();
	}

	// Print a Interac ticket
	else if (nTicketType == BATCH_TICKET_INTERAC)
	{
		int nLen;

		__Cless_Receipt_FormatAmount(ulAmount, FALSE, ulCurrencyExp, 0, szAmount);

		if (pValuePan != NULL)
		{
			nLen = strlen(Pan);
			if (nLen)
			{
				Pan[nLen - 1] = '-';
				if (nLen > 6)
					memset(Pan, '-', 6);
				else
					memset(Pan, '-', nLen-1);
			}
		}

		if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER) // Bitmaps only works with a printer
		{
			Cless_DumpData("       \xEA\xEB");
		}
		else
		{
			Cless_DumpData(" #                            ");Cless_DumpData_DumpNewLine();
			Cless_DumpData(" #       #                    ");Cless_DumpData_DumpNewLine();
			Cless_DumpData(" #      ###                   ");Cless_DumpData_DumpNewLine();
			Cless_DumpData(" # ###   #    ##  ##  ##   ## ");Cless_DumpData_DumpNewLine();
			Cless_DumpData(" # #  #  #   # # #   #  # #   ");Cless_DumpData_DumpNewLine();
			Cless_DumpData(" # #  #  # # ##  #   #  # #   ");Cless_DumpData_DumpNewLine();
			Cless_DumpData(" # #  #  ##   ## #    ###  ## ");Cless_DumpData_DumpNewLine();
		}

		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();

		if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER) // Bitmaps only works with a printer
			Cless_DumpData ("\xF1\xF2");
		else
			Cless_DumpData ("-------------------------");

		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();

		Cless_DumpData ("\x1b" "@\x1b" "E");

		// Display the application label if available
		if (pValueApplicationLabel != NULL)
		{
			for (temp=0;temp<(unsigned char)length_application_label;temp++)
			{
				if (!(((ApplicationLabel[temp] >= 0x30) && (ApplicationLabel[temp] <= 0x39))
					|| ((ApplicationLabel[temp] >= 0x41) && (ApplicationLabel[temp] <= 0x5A))
					|| ((ApplicationLabel[temp] >= 0x61) && (ApplicationLabel[temp] <= 0x7A))))
				{
					ApplicationLabel[temp] = 0x20;
				}
			}

			Cless_DumpData("\x1b" "@");
			Cless_DumpData("%s", ApplicationLabel);
			Cless_DumpData_DumpNewLine();
		}

		// Print the Application Prefered Name
		Cless_DumpData("\x1b" "@");
		Cless_DumpData("%s", AppliPreferedName);
		Cless_DumpData_DumpNewLine();

		for (temp=0;temp<(unsigned char)LengthAid;temp++)
			Cless_DumpData("%02x", Aid[temp]);

		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();

		Cless_DumpData("\x1b" "@\x1b" "E");

		if (ulType == CLESS_SAMPLE_TRANSACTION_TYPE_DEBIT)
		{
			Cless_Term_Read_Message(STD_MESS_DEBIT, nCardHolderLang, &tDisplayMsg);
			Cless_DumpData(tDisplayMsg.message);
		}
		else
		{
			Cless_Term_Read_Message(STD_MESS_TRANSAC_TYPE, nCardHolderLang, &tDisplayMsg);
			Cless_DumpData("%s %lX", tDisplayMsg.message, ulType);
		}

		Cless_DumpData ("\x1b" "@");
		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();
		Cless_DumpData ("%c%c/%c%c/%c%c %c%c:%c%c:%c%c", Date[4], Date[5], Date[2], Date[3], Date[0], Date[1], Time[0], Time[1], Time[2], Time[3], Time[4], Time[5]);
		Cless_DumpData_DumpNewLine();
		Cless_DumpData ("%s", Pan);
		Cless_DumpData_DumpNewLine();
		Cless_DumpData ("%c%c/%c%c", ExpirationDate[2], ExpirationDate[3], ExpirationDate[0], ExpirationDate[1]);
		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();
		Cless_DumpData ("\x1b" "E");

		Cless_Term_Read_Message(STD_MESS_APPROVED_TICKET, nCardHolderLang, &tDisplayMsg);
		Cless_DumpData (tDisplayMsg.message);

		Cless_DumpData ("\x1b" "@");
		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();

		Cless_Term_Read_Message(STD_MESS_AMOUNT, nCardHolderLang, &tDisplayMsg);
		Cless_DumpData (tDisplayMsg.message);

		if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER)
			Cless_DumpData ("\x1b" "E");

		Cless_DumpData ("%s %s", Cless_Fill_GiveMoneyLabel(), szAmount);
		Cless_DumpData ("\x1b" "@");
		Cless_DumpData_DumpNewLine();


		if (bSignatureRequested)
		{
			Cless_DumpData_DumpNewLine();

			if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER) // Bitmaps only works with a printer
			{
				Cless_DumpData ("\xF9\xFA");
			}
			else
			{
				Cless_DumpData (" *** Signature ******************* ");Cless_DumpData_DumpNewLine();
				Cless_DumpData ("*                                 *");Cless_DumpData_DumpNewLine();
				Cless_DumpData ("*                                 *");Cless_DumpData_DumpNewLine();
				Cless_DumpData ("*                                 *");Cless_DumpData_DumpNewLine();
				Cless_DumpData (" ********************************* ");
			}
		}

		_Cless_Receipt_PrintFooter();

		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();

		// output the TVR
		Cless_DumpData("TVR: ");

		for (temp=0;temp<5;temp++)
			Cless_DumpData("%02x", Tvr[temp]);

		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();
	}

	Cless_DumpData_DumpCloseOutputDriver();
}


//! \brief Prints the receipt for PURE kernel transactions.
//! \param[in] pTransaction Structure containing the data returned by the PURE kernel.
//! \param[in] bMobileTransaction Indicates if the transaction has bee performed with a mobile or not.
//! \param[in] nRequiredCvm Indicates the required CVM.
//! \param[in] nCardHolderLang Indicates the cardholder language.

void Cless_Receipt_PrintPURE (T_SHARED_DATA_STRUCT * pTransaction, const int bMobileTransaction, const int nRequiredCvm, int nCardHolderLang)
{
	int nResult, nPosition;
	unsigned long ulTag;
	unsigned long ulReadLength;
	const unsigned char * pValue;
	const unsigned char* pValueAid;
	unsigned int LengthAid;
	const unsigned char* pValueTvr;
	const unsigned char* pValueAmountNum;
	const unsigned char* pValueAmountOtherNum;
	const unsigned char* pValueExpirationDate;
	const unsigned char* pValuePan;
	unsigned int LengthPan;
	const unsigned char* pValuePanSeq;
	const unsigned char* pValueCurrencyCode;
	const unsigned char* pValueCurrencyExp;
	const unsigned char* pValueDate;
	const unsigned char* pValueType;
	//const unsigned char* pValueCardType;
	const unsigned char* pValueApplicationLabel;
	unsigned int length_application_label;
	const unsigned char* pValueAppliPreferedName;
	unsigned int length_appli_prefered_name;

	unsigned char Aid[20];
	unsigned char Tvr[5];
	unsigned long ulAmount;
	unsigned long ulAmountOther;
	char ExpirationDate[6 + 1];
	char Pan[19 + 1];
	unsigned long ulPanSeq;
	unsigned long ulCurrencyCode;
	unsigned long ulCurrencyExp;
	char Date[6 + 1];
	unsigned long ulType;
	char szAmount[20];
//	unsigned short usCardType;
	unsigned char ApplicationLabel [16 + 1]; // Application label + '\0'
	unsigned char AppliPreferedName [16 + 1]; // Application prefered name + '\0'
	int temp;
	MSGinfos tDisplayMsg;


	// Open output device
	if (!Cless_DumpData_DumpOpenOutputDriver())
		return;

	pValueAid = NULL;
	LengthAid = 0;
	pValueTvr = NULL;
	pValueAmountNum = NULL;
	pValueAmountOtherNum = NULL;
	pValueExpirationDate = NULL;
	pValuePan = NULL;
	LengthPan = 0;
	pValuePanSeq = NULL;
	pValueCurrencyCode = NULL;
	pValueCurrencyExp = NULL;
	pValueDate = NULL;
	pValueType = NULL;
//	pValueCardType = NULL;
//	usCardType = 0;
	pValueApplicationLabel = NULL;
	length_application_label = 0;
	pValueAppliPreferedName = NULL;
	length_appli_prefered_name = 0;

	Telium_DefprinterpatternHR (0, 0, 0xF1, (char *) Line_part_1);
	Telium_DefprinterpatternHR (0, 0, 0xF2, (char *) Line_part_2);
	Telium_DefprinterpatternHR (0, 0, 0xF7, (char *) Ingenico_part_1);
	Telium_DefprinterpatternHR (0, 0, 0xF8, (char *) Ingenico_part_2);
	Telium_DefprinterpatternHR (0, 0, 0xF9, (char *) Signature_part_1);
	Telium_DefprinterpatternHR (0, 0, 0xFA, (char *) Signature_part_2);
	Telium_DefprinterpatternHR (0, 0, 0xE6, (char *) PURE_part_1);
	Telium_DefprinterpatternHR (0, 0, 0xE7, (char *) PURE_part_2);

	nPosition = SHARED_EXCHANGE_POSITION_NULL;

	do{
		// Try to following tag.
		nResult = GTL_SharedExchange_GetNext (pTransaction, &nPosition, &ulTag, &ulReadLength, &pValue);

		if (nResult == STATUS_SHARED_EXCHANGE_OK)  // If tag found
		{
			switch(ulTag)
			{
			case TAG_EMV_DF_NAME:							pValueAid = pValue;	LengthAid = ulReadLength; break;
			case TAG_EMV_TVR:								pValueTvr = pValue; break;
			case TAG_EMV_AMOUNT_AUTH_NUM:					pValueAmountNum = pValue; break;
			case TAG_EMV_AMOUNT_OTHER_NUM:					pValueAmountOtherNum = pValue; break;
			case TAG_EMV_APPLI_EXPIRATION_DATE:				pValueExpirationDate = pValue; break;
			case TAG_EMV_APPLI_PAN:							pValuePan = pValue;	LengthPan = ulReadLength; break;
			case TAG_EMV_APPLI_PAN_SEQUENCE_NUMBER:			pValuePanSeq = pValue; break;
			case TAG_EMV_TRANSACTION_CURRENCY_CODE:			pValueCurrencyCode = pValue; break;
			case TAG_EMV_TRANSACTION_CURRENCY_EXPONENT:		pValueCurrencyExp = pValue; break;
			case TAG_EMV_TRANSACTION_DATE:					pValueDate = pValue; break;
			case TAG_EMV_TRANSACTION_TYPE:					pValueType = pValue; break;
			//case TAG_KERNEL_CARD_TYPE:						pValueCardType=pValue; break;
			case TAG_EMV_APPLICATION_LABEL:					pValueApplicationLabel=pValue; length_application_label = ulReadLength; break;
			case TAG_EMV_APPLI_PREFERED_NAME:				pValueAppliPreferedName=pValue; length_appli_prefered_name = ulReadLength; break;
			default:
				break;
			}
		}
		else if (nResult != STATUS_SHARED_EXCHANGE_END)  // Error
		{
			GTL_Traces_TraceDebug("Display tags error:%x\n", nResult);
		}
	}while (nResult == STATUS_SHARED_EXCHANGE_OK);


	memset(Aid, 0, sizeof(Aid));
	memset(Tvr, 0, sizeof(Tvr));
	ulAmount = 0;
	ulAmountOther = 0;
	memset(ExpirationDate, '?', sizeof(ExpirationDate));
	memset(Pan, '?', sizeof(Pan));
	Pan[sizeof(Pan) - 1] = 0;
	ulPanSeq = 0;
	ulCurrencyCode = 0;
	ulCurrencyExp = 0;
	memset(Date, '?', sizeof(Date));
	ulType = 0;
	memset (ApplicationLabel, '\0', sizeof(ApplicationLabel));
	memset (AppliPreferedName, '\0', sizeof(AppliPreferedName));

	if (pValueAid != NULL)
		memcpy(Aid, pValueAid, LengthAid);
	if (pValueTvr != NULL)
		memcpy(Tvr, pValueTvr, sizeof(Tvr));
	if (pValueAmountNum != NULL)
		GTL_Convert_DcbNumberToUl(pValueAmountNum, &ulAmount, 6);
	if (pValueAmountOtherNum != NULL)
		GTL_Convert_DcbNumberToUl(pValueAmountOtherNum, &ulAmountOther, 6);
	if (pValueExpirationDate != NULL)
		GTL_Convert_DcbNumberToAscii(pValueExpirationDate, ExpirationDate, 3, 6);
	if (pValuePan != NULL)
		GTL_Convert_DcbPaddedToAscii(pValuePan, Pan, LengthPan);
	if (pValuePanSeq != NULL)
		GTL_Convert_DcbNumberToUl(pValuePanSeq, &ulPanSeq, 1);
	if (pValueCurrencyCode != NULL)
		GTL_Convert_DcbNumberToUl(pValueCurrencyCode, &ulCurrencyCode, 2);
	if (pValueCurrencyExp != NULL)
		GTL_Convert_DcbNumberToUl(pValueCurrencyExp, &ulCurrencyExp, 1);
	if (pValueDate != NULL)
		GTL_Convert_DcbNumberToAscii(pValueDate, Date, 3, 6);
	if (pValueType != NULL)
		ulType = pValueType[0];
//	if (pValueCardType != NULL)
//		usCardType = (pValueCardType[0] << 8) + pValueCardType[1];
	if (pValueApplicationLabel != NULL)
		memcpy (ApplicationLabel, pValueApplicationLabel, length_application_label);
	if (pValueAppliPreferedName != NULL)
		memcpy (AppliPreferedName, pValueAppliPreferedName, length_appli_prefered_name);

	// Print the receipt
	__Cless_Receipt_FormatAmount(ulAmount, FALSE, ulCurrencyExp, 0, szAmount);

	// Format the PAN
	{
		int nLen;

		if (pValuePan != NULL)
		{
			nLen = strlen(Pan);
			if (nLen)
			{
				Pan[nLen - 1] = '-';
				if (nLen > 6)
					memset(Pan, '-', 6);
				else
					memset(Pan, '-', nLen-1);
			}
		}
	}

	// Print the logo
	if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER) // Bitmaps only works with a printer
	{
//		Cless_DumpData("\xF7\xF8");
		Cless_DumpData (" ********************** ");Cless_DumpData_DumpNewLine();
		Cless_DumpData ("*    MERCHANT LOGO     *");Cless_DumpData_DumpNewLine();
		Cless_DumpData ("*   WILL APPEAR HERE   *");Cless_DumpData_DumpNewLine();
		Cless_DumpData (" ********************** ");Cless_DumpData_DumpNewLine();
	}
	else
	{
		Cless_DumpData("  ####   #    #   ###   #####");Cless_DumpData_DumpNewLine();
		Cless_DumpData(" #    #  #    #  #   #  #    ");Cless_DumpData_DumpNewLine();
		Cless_DumpData(" #    #  #    #  #   #  #    ");Cless_DumpData_DumpNewLine();
		Cless_DumpData(" ####    #    #  ####   #### ");Cless_DumpData_DumpNewLine();
		Cless_DumpData(" #       #    #  #   #  #    ");Cless_DumpData_DumpNewLine();
		Cless_DumpData(" #       #    #  #   #  #    ");Cless_DumpData_DumpNewLine();
		Cless_DumpData(" #        ####   #   #  #####");Cless_DumpData_DumpNewLine();
	}

//	Cless_DumpData_DumpNewLine();
//	Cless_DumpData_DumpNewLine();

	// Print separator line
	Cless_DumpData_DumpNewLine();
	if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER) // Bitmaps only works with a printer
		Cless_DumpData ("\xF1\xF2");
	else
		Cless_DumpData ("-------------------------");
	Cless_DumpData_DumpNewLine();
	Cless_DumpData_DumpNewLine();

	// Print receipt depending on the action
	Cless_DumpData ("\x1b" "@\x1b" "E");

	// Display the application label if available
	if (pValueApplicationLabel != NULL)
	{
		for (temp=0;temp<(unsigned char)length_application_label;temp++)
		{
			if (!(((ApplicationLabel[temp] >= 0x30) && (ApplicationLabel[temp] <= 0x39))
				|| ((ApplicationLabel[temp] >= 0x41) && (ApplicationLabel[temp] <= 0x5A))
				|| ((ApplicationLabel[temp] >= 0x61) && (ApplicationLabel[temp] <= 0x7A))))
			{
				ApplicationLabel[temp] = 0x20;
			}
		}

		Cless_DumpData("\x1b" "@");
		Cless_DumpData("%s", ApplicationLabel);
		Cless_DumpData_DumpNewLine();
	}

	// Display the application preferred name if available
	if (pValueAppliPreferedName != NULL)
	{
		for (temp=0;temp<(unsigned char)length_appli_prefered_name;temp++)
		{
			if (!(((AppliPreferedName[temp] >= 0x30) && (AppliPreferedName[temp] <= 0x39))
				|| ((AppliPreferedName[temp] >= 0x41) && (AppliPreferedName[temp] <= 0x5A))
				|| ((AppliPreferedName[temp] >= 0x61) && (AppliPreferedName[temp] <= 0x7A))))
			{
				AppliPreferedName[temp] = 0x20;
			}
		}

		Cless_DumpData("\x1b" "@");
		Cless_DumpData("%s", AppliPreferedName);
		Cless_DumpData_DumpNewLine();
	}

	Cless_DumpData_DumpNewLine();

	Cless_DumpData("\x1b" "@\x1b" "E");

	if (ulType == CLESS_SAMPLE_TRANSACTION_TYPE_DEBIT)
	{
		Cless_Term_Read_Message(STD_MESS_DEBIT, nCardHolderLang, &tDisplayMsg);
		Cless_DumpData(tDisplayMsg.message);
	}
	else if (ulType == CLESS_SAMPLE_TRANSACTION_TYPE_REFUND)
	{
		Cless_Term_Read_Message(STD_MESS_REFUND, nCardHolderLang, &tDisplayMsg);
		Cless_DumpData(tDisplayMsg.message);
	}
	else
	{
		Cless_Term_Read_Message(STD_MESS_TRANSAC_TYPE, nCardHolderLang, &tDisplayMsg);
		Cless_DumpData("%s %lX", tDisplayMsg.message, ulType);
	}

	Cless_DumpData_DumpNewLine();
	Cless_Term_Read_Message(STD_MESS_APPROVED_TICKET, nCardHolderLang, &tDisplayMsg);
	Cless_DumpData (tDisplayMsg.message);
	Cless_DumpData ("\x1b" "@");

	Cless_DumpData_DumpNewLine();
	Cless_DumpData_DumpNewLine();

	Cless_DumpData ("%c%c/%c%c/%c%c", Date[4], Date[5], Date[2], Date[3], Date[0], Date[1]);
	Cless_DumpData_DumpNewLine();

	// If the PAN is present, print it (with hidden parts)
	if (pValuePan != NULL)
	{
		Cless_DumpData ("%s", Pan);
		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();
	}

	Cless_Term_Read_Message(STD_MESS_AMOUNT, nCardHolderLang, &tDisplayMsg);
	Cless_DumpData (tDisplayMsg.message);

	if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER)
		Cless_DumpData ("\x1b" "E");

	Cless_DumpData ("%s %s", Cless_Fill_GiveMoneyLabel(), szAmount);
	Cless_DumpData ("\x1b" "@");
	Cless_DumpData_DumpNewLine();

	// Check if signature is required
	if (nRequiredCvm == PAYPASS_OPS_CVM_SIGNATURE)
	{
		Cless_DumpData_DumpNewLine();

		if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER) // Bitmaps only works with a printer
		{
			Cless_DumpData ("\xF9\xFA");
			Cless_DumpData_DumpNewLine();
		}
		else
		{
			Cless_DumpData (" *** Signature ******************* ");Cless_DumpData_DumpNewLine();
			Cless_DumpData ("*                                 *");Cless_DumpData_DumpNewLine();
			Cless_DumpData ("*                                 *");Cless_DumpData_DumpNewLine();
			Cless_DumpData ("*                                 *");Cless_DumpData_DumpNewLine();
			Cless_DumpData (" ********************************* ");Cless_DumpData_DumpNewLine();
		}
	}

	Cless_DumpData_DumpNewLine();

	if (bMobileTransaction)
	{
		Cless_Term_Read_Message(STD_MESS_RECEIPT_TXN_WITH_MOBILE, nCardHolderLang, &tDisplayMsg);
		Cless_DumpData (tDisplayMsg.message);

		Cless_DumpData("\x0F");

		if (nRequiredCvm == PAYPASS_OPS_CVM_CONFIRMATION_CODE_VERIFIED)
			Cless_Term_Read_Message(STD_MESS_RECEIPT_PIN_VERIFIED, nCardHolderLang, &tDisplayMsg);
		else
			Cless_Term_Read_Message(STD_MESS_RECEIPT_NO_CARDHOLDER_VERIFICATION, nCardHolderLang, &tDisplayMsg);

		Cless_DumpData (tDisplayMsg.message);

		Cless_DumpData("\x1b" "@");
	}
	else
	{
		Cless_Term_Read_Message(STD_MESS_RECEIPT_TXN_WITH_CARD, nCardHolderLang, &tDisplayMsg);
		Cless_DumpData (tDisplayMsg.message);
	}


	_Cless_Receipt_PrintFooter();

	Cless_DumpData_DumpNewLine();
	Cless_DumpData_DumpNewLine();
	Cless_DumpData_DumpNewLine();
	Cless_DumpData_DumpNewLine();
	Cless_DumpData_DumpNewLine();

	Cless_DumpData_DumpCloseOutputDriver();
}


