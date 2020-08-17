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
#include "globals.h"
#include "EMV_Support.h"


/////////////////////////////////////////////////////////////////
//// Macros & preprocessor definitions //////////////////////////


/////////////////////////////////////////////////////////////////
//// Global data definition /////////////////////////////////////

static int gs_nTransactionMode = CLESS_SAMPLE_TRANSACTION_MODE_UNKNOWN;				/*!< Indicates the transaction mode. */
static int gs_nDeviceType = CLESS_SAMPLE_DEVICE_UNDETERMINED;						/*!< Indicates the type of device. */

/////////////////////////////////////////////////////////////////
//// Static functions definition ////////////////////////////////

static int __Cless_Common_DayOfYear (Telium_Date_t *date);


/////////////////////////////////////////////////////////////////
//// Functions //////////////////////////////////////////////////

//! \brief Extract the status code (TAG_KERNEL_STATUS_CODE) from a shared exchange buffer.
//! \param[in] pSharedStruct Shared exchange buffer from which the status code shall be extracted.
//! \return The exctracted status code.

int Cless_Common_GetStatusCode (T_SHARED_DATA_STRUCT * pSharedStruct) {
	int nResult;
	int nStatusCode = -1;
	unsigned long ulTag, ulReadLength;
	const unsigned char * pReadValue;
	int nPosition;

	nPosition = SHARED_EXCHANGE_POSITION_NULL;
	ulTag = TAG_KERNEL_STATUS_CODE;
	nResult = GTL_SharedExchange_FindNext (pSharedStruct, &nPosition, ulTag, &ulReadLength, &pReadValue);

	if (nResult == STATUS_SHARED_EXCHANGE_OK) {  // If tag found
		memcpy(&nStatusCode, pReadValue, ulReadLength);
	}

	return (nStatusCode);
}

//! \brief Get a specific information in a shared buffer.
//! \param[out] pSpendingAmount pointer
//! \return
//!		- \ref TRUE if correctly retrieved.
//!		- \ref FALSE if an error occurred.

int Cless_Common_RetrieveInfo (T_SHARED_DATA_STRUCT * pResultDataStruct, unsigned long ulTag, unsigned char ** pInfo) {
	int nResult = TRUE;
	int nPosition, cr;
	unsigned long ulReadLength;
	const unsigned char * pReadValue;

	if ((pResultDataStruct == NULL) || (ulTag == 0) || (pInfo == NULL)) {
		GTL_Traces_TraceDebug("Cless_Common_RetreiveInfo : parameter error");
		nResult = FALSE;
		goto End;
	}
	nPosition = SHARED_EXCHANGE_POSITION_NULL;
	*pInfo = NULL; // Default result : no information

	cr = GTL_SharedExchange_FindNext(pResultDataStruct, &nPosition, ulTag, &ulReadLength, &pReadValue);

	if (cr != STATUS_SHARED_EXCHANGE_OK) {
		GTL_Traces_TraceDebug("Cless_Common_RetrieveInfo : Unable to getinfo from the kernel response (tag=%02lx, cr=%02x)", ulTag, cr);
		nResult = FALSE;
		goto End;
	}

	// Get the transaction outcome
	*pInfo = (unsigned char *)pReadValue;

	End:
	return (nResult);
}


//! \brief Get a specific information in a TlvTree.
//! \param[in] pTlvTree Tree node.
//! \param[in] ulTag Tag to be found.
//! \param[out] pReadLength Read length in relation with tag asked.
//! \param[out] pReadValue Read value in relation with tag asked.
//! \return
//!		- \ref TRUE if correctly retrieved.
//!		- \ref FALSE if an error occurred.

int Cless_Common_GetTagInTlvTree (TLV_TREE_NODE pTlvTree, unsigned long ulTag, unsigned int *pReadLength, unsigned char ** pReadValue) {
	TLV_TREE_NODE pTlvTmp; 		// Temporary Node

	if(pTlvTree == NULL)
		return (FALSE);

	pTlvTmp = TlvTree_Find(pTlvTree, ulTag, 0);
	if (pTlvTmp != NULL) {
		*pReadLength = TlvTree_GetLength(pTlvTmp);
		*pReadValue = TlvTree_GetData(pTlvTmp);

		return (TRUE);
	}

	return (FALSE);
}


//! \brief Format the message that contains the amount, the currency according to ucFormat, ucPosition...  
//! \param[in] ucFormat : the display format
//! \param[in] ucCurrency : transaction currency
//! \param[in] ucPosition : the position of the currency
//! \param[in] ulAmount : the transaction amount
//! \param[out] pFormattedAmountMessage : the formatted message to display

void Cless_Common_FormatAmount (unsigned char ucFormat, char *ucCurrency, unsigned char ucPosition, unsigned long ulAmount, char *pFormattedAmountMessage)
{
	char            currency[4];
	char            separator1[2];
	char            separator2[2];
	char            position;
	int             exponent;
	unsigned long   divider;
	unsigned long   i;
	char   amount_format[51];



	// Determine the separators and the exponent
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

	// Format the amount
	switch(exponent)
	{
	case 1 :
		for (divider=1L,i=0L;i<(unsigned)exponent;divider*=10L,i++);

		if ((ulAmount/divider)<1000L)
		{
			Telium_Sprintf(amount_format,"%lu%s%0*lu", (ulAmount/divider), separator2, exponent, (ulAmount%divider));
		}
		else
		{
			if ((ulAmount/divider)<1000000L)
			{
				Telium_Sprintf(amount_format,"%lu%s%03lu%s%0*lu", ((ulAmount/divider)/1000L), separator1, ((ulAmount/divider)%1000L), separator2, exponent, (ulAmount%divider));
			}
			else
			{
				Telium_Sprintf(amount_format,"%lu%s%03lu%s%03lu%s%0*lu", ((ulAmount/divider)/1000000L), separator1, (((ulAmount/divider)/1000L)%1000L), separator1, ((ulAmount/divider)%1000L), separator2, exponent, (ulAmount%divider));
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

		if ((ulAmount/divider)<1000L)
		{
			Telium_Sprintf(amount_format,"%lu%s%0*lu", (ulAmount/divider), separator2, exponent, (ulAmount%divider));
		}
		else
		{
			Telium_Sprintf(amount_format,"%lu%s%03lu%s%0*lu", ((ulAmount/divider)/1000L), separator1, ((ulAmount/divider)%1000L), separator2, exponent, (ulAmount%divider));
		}
		break;  
	case 0 :
	default :
		if ((ulAmount)<1000L)
		{
			Telium_Sprintf(amount_format,"%lu",(ulAmount));
		}
		else
		{
			if ((ulAmount)<1000000L)
			{
				Telium_Sprintf(amount_format,"%lu%s%03lu",((ulAmount)/1000L),separator1,((ulAmount)%1000L));
			}
			else
			{
				Telium_Sprintf(amount_format,"%lu%s%03lu%s%03lu",((ulAmount)/1000000L),separator1,(((ulAmount)/1000L)%1000L),separator1,((ulAmount)%1000L));
			}
		}
		break;
	}

	if (position == 0)
	{
		Telium_Sprintf(pFormattedAmountMessage,"%s %s", amount_format, currency);
	}
	else
	{
		Telium_Sprintf(pFormattedAmountMessage,"%s %s", currency, amount_format);
	}
}



//! \brief Requests amount entry, prepare frame for Entry Point (fill shared memory)
//! \param[in] bEnterAmountOther \a TRUE if amount other shall be enetered (change the label).
//! \param[in] pCurrencyLabel : the currency label
//! \param[out] pAmountBin : the entered transaction amount
//! \return 
//!		- TRUE if amount has been entered.
//!		- FALSE if amount entry has been cancelled.

int Cless_Common_AmountEntry (const int bEnterAmountOther, const unsigned char *pCurrencyLabel, unsigned long * pAmountBin) {
	//	TAB_ENTRY_FCT TabEntry;
	unsigned long ulAmount = 0;
	int nResult;
	//	unsigned char ucFormat, ucPosition;
	unsigned char CurrencyLabel[4];
	char amount[64];


	nResult = T_ANN;

	// Init amount entry structure
	//	TabEntry.mask = 0;
	//	TabEntry.time_out = 60;
	*pAmountBin = 0;
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
	memset(amount, 0, sizeof(amount));
	if (!bEnterAmountOther) {
		mapGet(traAmt, amount, lenAmt);
	}else{
		mapGet(traOtherAmt, amount, lenAmt);
	}


	//	ucFormat = 1;
	//	ucPosition = 0;
	memcpy (CurrencyLabel, pCurrencyLabel, 3);

	if (Cless_Goal_IsAvailable()) {
		unsigned long result;

		if (strlen(amount) < 1) { //There was no Amount entered previously

			result = GL_KEY_VALID;

			//			Cless_Goal_Open();
			//			if (!bEnterAmountOther) {
			//				result = GL_Dialog_Amount(Cless_Goal_Handle(), NULL, "Amount", "/d,/d/d/d,/d/d/d,/d/d/D./D/D", amount, sizeof(amount), (char*)CurrencyLabel, GL_ALIGN_RIGHT, TabEntry.time_out * 1000);
			//			} else {
			//				result = GL_Dialog_Amount(Cless_Goal_Handle(), NULL, "Amount Other", "/d,/d/d/d,/d/d/d,/d/d/D./D/D", amount, sizeof(amount), (char*)CurrencyLabel, GL_ALIGN_RIGHT, TabEntry.time_out * 1000);
			//			}
			//			Cless_Goal_Close();

		}else{
			result = GL_KEY_VALID;
		}
		if (result == GL_KEY_VALID) {
			nResult = T_VAL;
			ulAmount = strtoul(amount, NULL, 10);
		}
	} else {

		if (strlen(amount) < 1) { //There was no Amount entered previously

			ulAmount = strtoul(amount, NULL, 10);
			nResult = T_VAL;

			// Get transaction amount
			//			if (!bEnterAmountOther)
			//				nResult = GetAmount(ucFormat, CurrencyLabel, ucPosition, &ulAmount, (unsigned char*)"Amount", &TabEntry);
			//			else
			//				nResult = GetAmount(ucFormat, CurrencyLabel, ucPosition, &ulAmount, (unsigned char*)"Amount Other", &TabEntry);
		}else{
			nResult = T_VAL;
		}
	}

	// If amount is validated
	if (nResult == T_VAL) {
		/// Format the message that contains the amount, the currency according to ucFormat, ucPosition...  
		///Cless_Common_FormatAmount(ucFormat,CurrencyLabel,ucPosition,ulAmount,pFormattedAmountMessage);

		// Return the transaction amount
		*pAmountBin = ulAmount;
	}

	// Return amount entry result
	return (nResult == T_VAL);
}


//! \brief Set the transaction mode.
//! \param[in] nMode Transaction mode (implicit or explicit).

void Cless_Common_SetTransactionMode (const int nMode)
{
	gs_nTransactionMode = nMode;
}



//! \brief Get the transaction mode.
//! \return The transaction mode.

int Cless_Common_GetTransactionMode (void)
{
	return (gs_nTransactionMode);
}



//! \brief Calculate the day number in the year, based on the date.
//! \param[in] date Date.
//! \return The day number in the year.

//static int __Cless_Common_DayOfYear (DATE *date)
static int __Cless_Common_DayOfYear (Telium_Date_t *date)
{
	unsigned long year;
	unsigned long month;
	unsigned long day;
	unsigned int i;
	int leap_year;

	const unsigned char day_per_month[2][13] = {
			{0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
			{0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
	};

	Ascbin (&year, date->year, 2);
	Ascbin (&month, date->month, 2);
	Ascbin (&day, date->day, 2);

	// Determine if it is a leap year or not
	leap_year = (year % 4) == 0;

	// Calculate the days
	for (i=1; i<month; i++)
		day = day + day_per_month[leap_year][i];

	return (day);
}



//! \brief This function returns the number of days bewteen two dates.
//! \param[in] date1 First date
//! \param[in] date2 Second date
//! \returns the number of days between date1 and date2.
//! \note No date check is done, so the provided dates shall be valid !

int Cless_Common_NumberOfDaysBetweenTwoDates (DATE *date1, DATE *date2)
{
	int  day1, day2;
	unsigned long  year1, year2, year;
	DATE temp;
	int result;

	// Prepare temporary data set to 31/12, allowing to calculate the number of days or a given year
	Binasc(temp.day, 31, 2);
	Binasc(temp.month, 12, 2);

	// Calculate the delta in days
	day1 = __Cless_Common_DayOfYear(date1);
	day2 = __Cless_Common_DayOfYear(date2);
	result = day2 - day1;

	// Add the delat in years, if necessary
	Ascbin (&year1, date1->year, 2);
	Ascbin (&year2, date2->year, 2);

	// Management of year 2000
	if (year1 <= 50)
		year1 = year1 + 2000;
	else
		year1 = year1 + 1900;
	if (year2 <= 50)
		year2 = year2 + 2000;
	else
		year2 = year2 + 1900;

	// Determine the most recent year to determine the loop
	if (year2  <= year1 )
	{
		for (year=year2; year<year1; year++)
		{
			Binasc(temp.year, year % 100, 2);
			result = result - __Cless_Common_DayOfYear(&temp); // Negative delta
		}
	}
	else
	{
		for (year=year1; year<year2; year++)
		{
			Binasc(temp.year, year % 100, 2);
			result = result + __Cless_Common_DayOfYear(&temp); // Positive delta
		}
	}

	return (result);
}



//! \brief Determine the type of terminal.
//! \return The type of terminal.

int Cless_Common_GetTerminalType (void)
{
	// Device type already determined
	if (gs_nDeviceType == CLESS_SAMPLE_DEVICE_UNDETERMINED)
	{
		// Device type not yet determined, so, do it
		if (IsICT220())
			gs_nDeviceType = TERM_ICT220;
		else if (IsICT250())
			gs_nDeviceType = TERM_ICT250;
		else if (IsISC350())
			gs_nDeviceType = TERM_ISC350;
		else if (IsISC250())
			gs_nDeviceType = TERM_ISC250;
		else if (IsIWL220())
			gs_nDeviceType = TERM_IWL220;
		else if (IsIWL250())
			gs_nDeviceType = TERM_IWL250;
		else if (IsIWL280())
			gs_nDeviceType = TERM_IWL280;
		else if (IsIPP320())
			gs_nDeviceType = TERM_IPP320;
		else if (IsIPP350())
			gs_nDeviceType = TERM_IPP350;
		else if (IsIMP350())
			gs_nDeviceType = TERM_IMP350;
		else if (IsIMP3xxCompanion())
			gs_nDeviceType = TERM_IMP320;
		else if (IsML30())
			gs_nDeviceType = TERM_P300;
		else if (IsEFT30())
			gs_nDeviceType = TERM_EFT930; //TERM_EFT930BT, TERM_EFT930G, TERM_EFT930SGEM, TERM_EFT930W, TERM_EFT930S
		else if (IsSPM())
			gs_nDeviceType = TERM_SPM;
		else if (IsIWL350())
			gs_nDeviceType = TERM_IWL350;
		else if (IsISC480())
			gs_nDeviceType = TERM_ISC480;
		else if (IsIPP480())
			gs_nDeviceType = TERM_IPP480;
		else if (IsIUC150())
			gs_nDeviceType = TERM_IUN150;
		else if (IsIUC180() || IsIUC180B())
			gs_nDeviceType = TERM_IUN180;
		else if (IsIST150())
			gs_nDeviceType = TERM_IST150;
		else if (IsIUP250())
			gs_nDeviceType = TERM_IUN230;
		else if (IsIWB220())
			gs_nDeviceType = TERM_IWB220;
		else if (IsIWB250())
			gs_nDeviceType = TERM_IWB250;
		else if (TM_GetTerminalType() == TYPE_TERMINAL_MR40)
			gs_nDeviceType = TERM_MR40;
		else
			gs_nDeviceType = CLESS_SAMPLE_DEVICE_UNKNOWN;
	}

	return (gs_nDeviceType);
}

#ifndef __TEST_VERSION__



//defines for trace debug
#define mask8 ((byte)0x80)
#define mask6 ((byte)0x20)
#define mask87 ((byte)0xC0)
#define mask85 ((byte)0x90)
#define mask54321 ((byte)0x1F)
#define mask7654321 ((byte)0x7F)
#define mask854321 ((byte)0x9F)




/** Convert GTL encoded data into a queue containing tags and values
 * \param que (O) Output queue containing intermittent T,V,T,V,... pairs
 *   Each tag is a card number.
 *   The values are arbitrary binary data, the first byte being the length of the value.
 * \param gtl (I) all tags and values in GTL-TLV format
 * \return non-negative value if OK; negative otherwise
 */
static int gtl2que(tQueue * que, T_SHARED_DATA_STRUCT * gtl) {
	int ret, pos;
	card tag, len;
	const byte *data;

	queReset(que);

	pos = SHARED_EXCHANGE_POSITION_NULL;
	while(42) {
		ret = GTL_SharedExchange_GetNext(gtl, &pos, &tag, &len, &data);
		if(ret != STATUS_SHARED_EXCHANGE_OK) {
			break;
		}

		quePutTlv(que, tag, (word) len, (byte *) data);
	}

	return queLen(que);
}


static int cmvGetT(byte * tag, const byte * src) {  //extract tag from BER-TLV encoded buffer
	byte ret;
	int flag;

	*tag = *src;
	ret = 1;
	if(((*tag) & mask54321) != mask54321)
		return 1;               //1-byte tag
	do {                        //process multiple-byte tags
		if(((*tag) & mask854321) == mask854321) {
			flag = 1;
		} else {
			flag = 0;
		}
		ret++;
		tag++;
		src++;
		*tag = *src;
		if(flag && (((*tag) & mask85) == mask85)) {
			ret++;
			tag++;
			src++;
			*tag = *src;
			ret++;
			tag++;
			src++;
			*tag = *src;
			break;
		}
	} while((*tag) & mask8);
	VERIFY(ret <= 4);
	return ret;
}

static int cmvGetL(card * len, const byte * src) {  //extract length from BER-TLV encoded buffer
	byte ret;

	if(((*src) & mask8) != mask8) { //the easiest case : 1-byte length
		*len = *src;
		ret = 1;
		goto lblOK;
	}
	ret = (*src) & mask7654321;
	src++;
	*len = *src++;
	ret--;

	if(ret == 0) {              //two-byte length
		ret = 2;
		goto lblOK;
	}
	*len *= 0x100;
	*len += *src++;
	ret--;

	if(ret == 0) {              //three-byte length
		ret = 3;
		goto lblOK;
	}
	*len *= 0x100;
	*len += *src++;
	ret--;

	if(ret == 0) {              //four-byte length
		ret = 4;
		goto lblOK;
	}
	*len *= 0x100;
	*len += *src++;
	ret--;
	if(ret == 0) {              //five-byte length
		ret = 5;
		goto lblOK;
	}

	return -1;                  //very long TLVs are not supported
	lblOK:
	return ret;
}

static int cmvGetTLV(byte * tag, card * len, byte * val, const byte * src) {    //retrieve TLV from BER-TLV encoded buffer
	word ret;

	ret = cmvGetT(tag, src);    //extract tag
	if(*tag == 0)               //It is the case for ETEC 6.0, Interoper.07 MC
		return -1;
	if(ret > 4)
		return -1;

	ret += cmvGetL(len, src + ret); //extract length
	src += ret;

	memcpy(val, src, (word) * len); //extract value
	ret += (word) * len;

	return ret;
}


/** Convert TLV from constructed data
 * \param que (O) Output queue containing intermittent T,V,T,V,... pairs
 *   Each tag is a card number.
 *   The values are arbitrary binary data, the first byte being the length of the value.
 * \param buf (I) all tags and values in GTL-TLV format
 * \return non-negative value if OK; negative otherwise
 */
static int getConstructedTLVData(tQueue * que, T_SHARED_DATA_STRUCT * buf) {
	int ret, idx,y=0;
	int res;
	int found;
	card curTag;
	card cmvTag;
	byte tag[4];                //EMV tag
	card len;                   //EMV tag length
	word cmvLen;
	byte val[300];              //EMV tag value
	byte cmvBuf[300];
	byte *dbg;

	idx = buf->ulDataLength;
	dbg = buf->nPtrData;

	while(idx > 0) {            //parse EMV TLVs from response using BER-TLV coding rules
		memset(tag, 0, sizeof(tag));
		memset(val, 0, sizeof(val));
		res = cmvGetTLV(tag, &len, val, dbg);   //retrieve (tag,length,value)
		if(res < 0)
			break;

		found = 0;
		if((tag[0] != 0) && (tag[1] != 0) && (tag[2] != 0) && (tag[3] != 0)) {
			curTag = CARDHL(WORDHL(tag[0], tag[1]), WORDHL(tag[2], tag[3]));
		} else if((tag[0] != 0) && (tag[1] != 0) && (tag[2] != 0)) {
			curTag = CARDHL(WORDHL(0, tag[0]), WORDHL(tag[1], tag[2]));
		} else if((tag[0] != 0) && (tag[1] != 0)) {
			curTag = CARDHL(0, WORDHL(tag[0], tag[1]));
		} else {
			curTag = CARDHL(0, WORDHL(0, tag[0]));
		}

		queRewind(que);
		while(queLen(que)) {
			memset(cmvBuf, 0, sizeof(cmvBuf));
			ret = queGetTlv(que, &cmvTag, &cmvLen, cmvBuf);
			CHECK(ret>0,lblBreak);
			VERIFY(ret == cmvLen);

			if(curTag == cmvTag) {
				found = 1;
			}
			goto lblC;
			lblBreak:
			break;
			lblC:
			y+=1;

		}

		if(found == 0) {
			quePutTlv(que, curTag, len, val);
		}

		idx = 0;
	}

	return 1;
}

static void CLESS_Save_TLV_To_Tra(word key,byte *val,word len){
	word localLen = 0;
	int ret = 0,plus=0;
	char dataString[1024];
	card saveKey = 0;

	switch (key) {
	case emvPAN:
		plus=1;
		saveKey = traPan; break; //PAN
	case emvTrk1:
		plus=1;
		saveKey = traTrk1; break; //track 1
	case emvTrk2:
		plus=1;
		saveKey = traTrk2; break; //track 2
	case emvExpDat:
		plus=1;
		saveKey = traExpDat; break; //Expiry
	case emvChdNam:
		plus=1;
		saveKey = traCardHolderName;  break; //Card holder name
	default:
		saveKey = 0;
		break;
	}
	memset(dataString,0,sizeof(dataString));
	bin2hex(dataString,val+plus,len-plus);

	len=(len-plus)*2;
	if (saveKey>0) { //// Make sure there is data to save in the database
		//Make sure the length does not exceed the database requirement
		localLen = mapDatLen(saveKey);
		if (len > localLen) {
			len = localLen;
		}

		//Then now save safely no worries juu ya lenght kuwa kubwa hehehhe
		ret = mapPut(saveKey, dataString, len);
		CHECK(ret >0, lblKO);
	}

	lblKO:;
}

static void CLESS_process_Trk2(word tagtoRead){
	char track2_Full[lenTrk2 * 2];
	char track2[lenTrk2+1],ExpDat[lenExpDat + 1];
	int ret = 0, len = 0,var = 0,Flag = 0,saveLoop = 0,startFrom = 0;

	memset(ExpDat, 0, sizeof(ExpDat));
	memset(track2, 0, sizeof(track2));
	memset(track2_Full, 0, sizeof(track2_Full));

	MAPGET(tagtoRead, track2_Full, lblKO);

	switch (tagtoRead) {
	case traPaypassTrk2_Mstripe:
		////// =================  PAYPASS KERNEL ==================
		len = strlen(track2_Full);

		CHECK(len > 15,lblKO);

		if ((track2_Full[0] == 'B') || (track2_Full[0] == 'A')) {
			startFrom = 1;
		}else if((track2_Full[1] == 'B') || (track2_Full[1] == 'A')){
			startFrom = 2;
		}else if((track2_Full[0] != 'B') || (track2_Full[0] != 'A')){
			startFrom = 0;
		}


		for (var = startFrom; var < len; var++) {
			if (track2_Full[var] == 'D') {
				Flag = 1;
				MAPPUTSTR(traPan,track2,lblKO);
				track2[saveLoop] = '=';
				saveLoop = saveLoop + 1;
				continue;
			}

			if ((Flag > 0)&& Flag < 5){
				ExpDat[Flag-1]=track2_Full[var];
				Flag=Flag+1;
			}

			track2[saveLoop] = track2_Full[var];

			saveLoop = saveLoop + 1;
		}

		//		//CLess PIN online forced
		//		FUN_PinEntry();

		break;
	case traVisaTrk2_Mstripe:
		////// =================  VISA KERNEL ==================
		len = strlen(track2_Full);

		CHECK(len > 15,lblKO);

		if ((track2_Full[0] == 'B') || (track2_Full[0] == 'A')|| (track2_Full[0] == ';')) {
			startFrom = 1;
		}else if((track2_Full[0] == 'B') || (track2_Full[0] == 'A')|| (track2_Full[0] == ';')) {
			startFrom = 2;
		}else if((track2_Full[0] != 'B') || (track2_Full[0] != 'A') || (track2_Full[0] != ';')){
			startFrom = 0;
		}

		for (var = startFrom; var < len; var++) {
			if (track2_Full[var] == '?') {
				break;
			}

			if (track2_Full[var] == '=') {
				Flag = 1;
				MAPPUTSTR(traPan,track2,lblKO);
				track2[saveLoop] = '=';
				saveLoop = saveLoop + 1;
				continue;
			}

			if ((Flag > 0)&& Flag < 5){
				ExpDat[Flag-1]=track2_Full[var];
				Flag=Flag+1;
			}

			track2[saveLoop] = track2_Full[var];

			saveLoop = saveLoop + 1;
		}

		//		//CLess PIN online forced
		//		FUN_PinEntry();
		break;
	default:
		break;
	}

	MAPPUTSTR(traExpDat, ExpDat, lblKO);
	MAPPUTSTR(traTrk2, track2, lblKO);

	lblKO:;
}

/** Used to Obtain All Tags/Data After Do Transaction
 * \param que (O) Contains the transaction data in TLV queue
 * \return 1
 */
int CLESS_Data_Save_To_DB(T_SHARED_DATA_STRUCT* traKernelBuf ) {
	tQueue dataQue;
	tBuffer dataBuff;
	byte dataContainer[4096];
	card tag;
	word len;
	word key,beg;
	byte buf[1 + 256];
	byte aid[lenAID + 1];
	byte MrcPrcCry[6 + 1];
	byte CardHolderName[100 + 1],ppTrack2[lenTrk2*2], TagLenVal[512];
	int ret = 0;
	char temp[256];

	bufInit(&dataBuff ,dataContainer , sizeof(dataContainer));
	queInit(&dataQue, &dataBuff);

	gtl2que(&dataQue, traKernelBuf);
	getConstructedTLVData(&dataQue, traKernelBuf);

	queRewind(&dataQue);

	while(queLen(&dataQue)) {
		ret = queGetTlv(&dataQue, &tag, &len, &buf[1]);
		CHECK(ret>0,lblBreak);
		VERIFY(ret >= 0);
		VERIFY(ret == len);
		CHECK(len <= sizeof(buf), lblDBA);
		buf[0] = len;
		goto lblC;
		lblBreak:
		break;


		lblC:
		//		key = mapCmvKeyTag(tag);// Convert tag retrieved into database key
		key = mapKeyTag(tag);
		if(!key) { // if not supported, skip it
			continue;
		}

		switch (tag) {
		case TAG_PAYPASS_MSTRIPE_UNPREDICTABLE_NUMBER:
			memset(TagLenVal, 0, sizeof(TagLenVal));
			bin2hex((char *)TagLenVal, &buf[1], buf[0]);
			bin2hex((char *)TagLenVal, &buf[1], buf[0]);
			break;
		case TAG_QUICKPASS_DECLINED_BY_CARD:            //Reason By Card for decline
		case TAG_PAYWAVE_DECLINED_BY_CARD:            //Reason By Card for decline
			memset(TagLenVal, 0, sizeof(TagLenVal));
			bin2hex((char *)TagLenVal, &buf[1], buf[0]);
			bin2hex((char *)TagLenVal, &buf[1], buf[0]);
			break;
		case TAG_PAYWAVE_CARD_TRANSACTION_QUALIFIERS:
			memset(TagLenVal, 0, sizeof(TagLenVal));
			bin2hex((char *)TagLenVal, &buf[1], buf[0]);
			bin2hex((char *)TagLenVal, &buf[1], buf[0]);
			break;
		case TAG_KERNEL_TRACE_ERROR: //TODO: KevCode checking TAG_KERNEL_TRACE_ERROR
			memset(TagLenVal, 0, sizeof(TagLenVal));
			bin2hex((char *)TagLenVal, &buf[1], buf[0]);
			bin2hex((char *)TagLenVal, &buf[1], buf[0]);
			//			ERR_QUICKPASS_000002
			//			error such as ERR_PAYWAVE_000128 = Mandatory tag missing
			break;
		case tagTrmRskMng:
			memset(ppTrack2, 0, sizeof(ppTrack2));
			bin2hex((char *)ppTrack2, &buf[1], buf[0]);
			break;
		case tagPaypassTrack2:
			memset(ppTrack2, 0, sizeof(ppTrack2));
			bin2hex((char *)ppTrack2, &buf[1], buf[0]);
			MAPPUTSTR(traPaypassTrk2_Mstripe, ppTrack2, lblDBA);
			CLESS_process_Trk2(traPaypassTrk2_Mstripe);
			break;
		case tagVisaTrack2:
			MAPPUTSTR(traVisaTrk2_Mstripe, &buf[1], lblDBA);
			CLESS_process_Trk2(traVisaTrk2_Mstripe);
			break;
		case tagChdNam:
			memset(CardHolderName, 0, sizeof(CardHolderName));
			bin2hex((char *)CardHolderName, &buf[1], buf[0]);
			MAPPUTSTR(emvChdNam, CardHolderName, lblDBA);
			break;
		case tagTrk1:
			memset(CardHolderName, 0, sizeof(CardHolderName));
			bin2hex((char *)CardHolderName, &buf[1], buf[0]);
			MAPPUTSTR(traTrk1, CardHolderName, lblDBA);
			break;
		case tagAIDT:
			bin2hex((char *)aid, &buf[1], buf[0]);
			MAPPUTSTR(traAID, aid, lblDBA);
			break;
		case tagMrcPrcCry:
			memset(MrcPrcCry, 0, sizeof(MrcPrcCry));
			bin2hex((char *)MrcPrcCry, &buf[1], buf[0]);
			MAPPUTSTR(emvMrcPrcCry, MrcPrcCry, lblDBA);
			break;
		case tagTrmTraCpb:
		case tagTrmFlrLim:
			break;
		case tagTCC:
		case tagICCPubCrt:
		case tagICCPubExp:
			break;
		case tagCVMRes:
			memset(temp, 0, sizeof(temp));
			bin2hex(temp, buf, len+1);
		default: {
			beg = begKey(key);
			switch (beg) {
			case traBeg:
			case appBeg: //Local to the application
				memset(temp, 0, sizeof(temp));
				bin2hex(temp, buf, len+1);
				ret = mapPut(key, temp, ((len + 1)*2));
				break;

			default:// All others come from Kernel
				ret = mapPut(key, buf, len+1);
				break;
			}

			CLESS_Save_TLV_To_Tra(key, buf, len+1);
		}
		break;
		}
	}

	lblDBA:
	return 1;
}



#endif

