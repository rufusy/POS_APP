//****************************************************************************
//       INGENICO                                INGEDEV 7
//============================================================================
//       FILE  MAPAPP.C                          (Copyright INGENICO 2012)
//============================================================================
//  Created :       09-July-2012     Kassovic
//  Last modified : 09-July-2012     Kassovic
//  Module : TRAINING
//
//  Purpose :
//  Supply access by integer key number to data by using the functions
//  saved inside none-volatile memory (DFS).
//
//  List of routines in file :
//      traReset : Reset none-volatile tralication parameters.
//      traPut : Store tralication parameter.
//      traGet : Retrieve tralication parameter.
//
//  File history :
//  070912-BK : File created
//
//****************************************************************************

//****************************************************************************
//      INCLUDES
//****************************************************************************
#include <globals.h>
#include "VGE_FMG.H"


//****************************************************************************
//      EXTERN
//****************************************************************************
//const char *getTraCmpDat(void){ return __DATE__;} // Return the date of compilation of the file Maptra.c
//const char *getTraCmpTim(void){ return __TIME__;} // Return the time of compilation of the file Maptra.c

//****************************************************************************
//      PRIVATE CONSTANTS
//****************************************************************************
/* */

//****************************************************************************
//      PRIVATE TYPES
//****************************************************************************
// Parameter row
// =============
typedef struct stTransRow
{
	word usKey;                // Parameter key
	word usLen;                // Parameter length
	void *pvDefault;           // Parameter default
} ST_TRANS_ROW;

//****************************************************************************
//      PRIVATE DATA
//****************************************************************************

// Transaction table
// ===============
static const ST_TRANS_ROW tzTra[traEnd-traBeg] = {
		{ traDatTim,                        lenDatTim,        "" },// current date and time CCYYMMDDhhmmss
		{ traMnuItm,                        lenMnu,           "" },// current menu item selected
		{ traCtx,                           lenprtW,          "" },// context message
		{ traSta,                           1,                "" },// communication session state
		{ traEntMod,                        1,                "" },// card entry mode
		{ traConCode,                       1,                "" },// condition code
		{ traTrk2,                          lenTrk2,          "" },// track 2 of the card
		{ traPan,                           lenPan,           "" },// pan of the card
		{ traExpDat,                        lenExpDat,        "" },// expiry date
		{ traMaskPan,                       lenPan,           "" },// pan of the card
		{ traSrvCd,                         lenSrvCd,         "" },
		{ traAmt,                           lenAmt,           "" },// Amount of the transaction
		{ traTransType,                     lenCardTransType, "" },// Transaction type
		{ traCardType,                      lenCardTransType, "" },// Card type
		{ traTipAmt,                        lenAmt,           "" },// Tip Amount
		{ traTotAmt,                        lenAmt,           "" },// Total Amount
		{ traAutCod,                        lenAutCod,        "" },// Authorization (approval) code
		{ traRspCod,                        lenRspCod,        "" },// Response code
		{ traRrn,                           lenRrn,           "" },// Retrieval Reference Number
		{ traTxnType,                       2,                "" },// Current Txn Type - used in rqs
		{ traIssName,                       lenprtW,          "" },// Issuer Name to be displayed
		{ traTrk21,                         lenprtW,          "" },// first half of track 2 of the card to be displayed
		{ traTrk22,                         lenprtW,          "" },// tail track 2 of the card to be displayed
		{ traCrdSeq,                        1,                "" },// Card Seq Number
		{ traBinIdx,                        1,                "" },// index in Bin multirecord
		{ traAcqIdx,                        1,                "" },// index in Acq multirecord
		{ traIssIdx,                        1,                "" },// index in Iss multirecord
		{ traSrv,                           lenSrv,           "" },// temporary varaible for IP Address or Port in dialog
		{ traTCPIPprm,                      1,                "" },// display when viewing IP add or Port config
		{ traPriSec,                        1,                "" },// display when viewing IP add and Port config if Primary or Secondary
		{ traIPConfig,                      1,                "" },// display when viewing IP add and Port config
		{ traPinBlk,                        lenPinBlk,        "" },// Pin block
		{ traTrk1,                          lenTrk2*2,        "" },// track 1 of the card
		{ traMnuItmBeforeOffline,           1,                "" },// current menu item selected
		{ traTxnTypeBeforeOffline,          2,                "" },// Current Txn Type - used in rqs
		{ traIdx, 		                    1,                "" },// transaction counter index number
		{ traLogTotalCount,                 1,                "" },// buffer placement for total transaction counts
		{ traTypeName, 	                    lenName,          "" },// Transaction Type Name
		{ traTypeCurName, 	                4,                "" },// Currency Name
		{ traLogTxnTtlCnt,                  3,                "" },// buffer placement for total transaction counts with length 3
		{ traLogTotalAmount,                lenAmt,           "" },// buffer placement for total transaction amt
		{ traAcqName, 		                lenName,          "" },// Acquirer Name
		{ traFindRoc, 		                lenInvNum,        "" },// Trace Number to Search
		{ traVoidFlag, 		                1,                "" },// Void Flag Indicator, >0 if void or 0 if not
		{ traFmtAmt, 		                lenAmt,           "" },// Display formatted Amount
		{ traVoidFlagContext, 		        1,                "" },// Void Flag Indicator Current Value Buffer
		{ traMnuItmContext,                 lenMnu,           "" },// Menu Item Current Value Buffer
		{ traDatTimContext,                 lenDatTim,        "" },// Current Value Buffer of date and time CCYYMMDDhhmmss
		{ traBinIdxContext,                 1,                "" },// Current Value Buffer for index in Bin multirecord
		{ traAcqIdxContext,                 1,                "" },// Current Value Buffer for index in Acq multirecord
		{ traIssIdxContext,                 1,                "" },// Current Value Buffer for index in Iss multirecord
		{ traPanContext,                    lenPan,           "" },// Current Value Buffer for Pan
		{ traExpDatContext,                 lenExpDat,        "" },// Current Value Buffer for expiry date
		{ traTrk2Context,                   lenTrk2,          "" },// Current Value Buffer for track 2 of the card
		{ traAutCodContext,                 lenAutCod,        "" },// Current Value Buffer for Authorization (traroval) code
		{ traAmtContext,                    lenAmt,           "" },// Current Value Buffer for amount
		{ traTipAmtContext,                 lenAmt,           "" },// Current Value Buffer for tip amount
		{ traRrnContext,                    lenRrn,           "" },// Current Value Buffer for Retrieval Reference Number
		{ traEntModContext,                 1,                "" },// Current Value Buffer for card entry mode
		{ traConCodeContext,                1,                "" },// Current Value Buffer for condition code
		{ traCardTypeContext,               lenCardTransType, "" },// Current Value Buffer for Card Type
		{ traTmsMsg,                        1,                "" },// TMS Msgs
		{ traTmsMreRec,                     1,                "" },// TMS Msgs
		{ traTmsEmvFleNme,                  lenFleNme,        "" },// TMS Emv File Name
		{ traTmsEmvFleHdl,                  lenFleHdl,        "" },// TMS Emv File Handle
		{ traTmsEmvFleOff,                  1,                "" },// TMS Emv File Offset(size of the received file)
		{ traTmsEmvFleSze,                  1,                "" },// TMS Emv File Size
		{ traTmsEmvFlechnlen,               1,                "" },// Length of the chunk to be requested
		{ traTmsDnlWidEmv,                  1,                "" },// TMS Download with or without emv params
		{ traPriIpBuf,                      lenSrv,           "" },// Primary TMS Server Phone Number or IP address/port
		{ traPriPortBuf,                    lenSrv,           "" },// Primary TMS Server Phone Number or IP address/port
		{ traSecIpBuf,                      lenSrv,           "" },// Primary TMS Server Phone Number or IP address/port
		{ traSecPortBuf,                    lenSrv,           "" },// Primary TMS Server Phone Number or IP address/port
		{ traFmtDate,                       lenDatTim,        "" },// Formatted Date for Trx printing DD MMM CCYY
		{ traFmtTime,                       lenDatTim,        "" },// Formatted Time for Trx printing HH:MM
		{ traInvNum,                        lenInvNum,        "" },// temporary invoice number (R.O.C.)
		{ traRspDspLin1,                    lenprtW,          "" },// response display line 1
		{ traRspDspLin2,                    lenprtW,          "" },// response display line 2
		{ traCVV2,                          (lenExpDat * 2) +2,        "" },// Card CVV2 from back of card MANULLY entered mode only
		{ emvAppPrfNam,                     (lenAppPrfNam * 2) +2,     "" },// 9F12 Application Preferred Name
		{ emvAmtBin,                        (lenAmtBin * 2) +2,        "" },// 81   Transaction Amount Binary
		{ emvAmtOthNum,                     (lenAmtNum * 2) +2,        "" },// 9F03 Amount other numeric
		{ emvAmtOthBin,                     (lenAmtBin * 2) +2,        "" },// 9F04 Amount other binary
		{ emvTrnTyp,                        (lenTrnTyp * 2) +2,        "" },// 9C Transaction Type
		{ emvAmtNum,                        (lenAmtNum * 2) +2,        "" },// 9F02 Transaction Amount Numeric
		{ emvTrnDat,                        (lenTrnDat * 2) +2,        "" },// 9A Transaction Date YYMMDD
		{ emvTrnTim,                        (lenTrnTim * 2) +2,        "" },// 9F21 Transaction Time hhmmss
		{ emvTrnSeqCnt,                     (lenTrnSeqCnt * 2) +2,     "" },// 9F41 Transaction Sequence Counter
		{ emvTSI,                           (lenTSI * 2) +2,           "" },// 9B   Transaction Status Information
		{ emvPAN,                           (lenEmvPAN * 2) +2,        "" },// 5A   Application Primary Account Number
		{ emvPANSeq,                        (lenPANSeq * 2) +2,        "" },// 5F34 PAN sequence number
		{ emvAIP,                           (lenAIP * 2) +2,           "" },// 82   Application interchange profile
		{ emvCapkIdx,                       (lenCapkIdx * 2) +2,       "" },// 8F   CAPK index
		{ emvTrk1,                          (lenEmvTrk2 * 2) +2,       "" },// 56   Track1 equivalent data
		{ emvTrk2,                          (lenEmvTrk2 * 2) +2,       "" },// 57   Track2 equivalent data
		{ emvExpDat,                        (lenEmvExpDat * 2) +2,     "" },// 5F24 Application Expiration date
		{ emvCVM,                           (lenCVM * 2) +2,           "" },// 8E   Cardholder verification method
		{ emvCrdAvn,                        (lenCrdAvn * 2) +2,        "" },// 9F08 card application version number
		{ emvChdNam,                        (lenChdNam * 2) +2,        "" },// 5F20 Cardholder Name
		{ emvLanPrf,                        (lenLanPrf * 2) +2,        "" },// 5F2D Language Preference
		{ emvRSAKeyExp,                     (lenRSAKeyExp * 2) +2,     "" },// DF7F RSA key exponent
		{ emvRSAKey,                        (lenRSAKey * 2) +2,        "" },// DF1F RSA key
		{ emvCVMOutRes,                     (lenCVMOutRes * 2) +2,     "" },// DF38 CVMOUT Result
		{ emvCVMRes,                        (lenCVMRes * 2) +2,        "" },// 9F34 Cardholder Verification Method Results
		{ emvTVR,                           (lenTVR * 2) +2,           "" },// 95   Terminal verification results
		{ emvFrcOnl,                        (lenFrcOnl * 2) +2,        "" },// DF1C Flag transaction forced online
		{ emvHotLst,                        (lenHotLst * 2) +2,        "" },// DF26 Flag set to TRUE if the card is in Opposition List
		{ emvAmtPrv,                        (lenAmtBin * 2) +2,        "" },// DF25 Amount found in the transaction file, in a previous transaction performed with the same card
		{ emvRspCod,                        (lenRspCod * 2) +2,        "" },// 8A   Authorization response code
		{ emvIAD,                           (lenIAD * 2) +2,           "" },// 9F10 Issuer Application Data
		{ emvIssTrnCrt,                     (lenIssTrnCrt * 2) +2,     "" },// 9F26 Issuer Transaction Certificate
		{ emvCID,                           (lenCID * 2) +2,           "" },// 9F27 Cryptogram Information Data (CID)
		{ emvUnpNum,                        (lenUnpNum * 2) +2,        "" },// 9F37 Unpredictable Number
		{ emvTrmDc1,                        (lenTrmDcs * 2) +2,        "" },// DF29 terminal decision for Generate AC (TC,AAC,ARQC)
		{ emvTrmDc2,                        (lenTrmDcs * 2) +2,        "" },// DF31 Terminal decision after Generate AC 0=Declined, 1= Approved
		{ emvATC,                           (lenATC * 2) +2,           "" },// 9F36 Application Transaction Counter (ATC)
		{ emvDFNam,                         (lenDFNam * 2) +2,         "" },// 84   Dedicated File (DF) Name
		{ emvOnlSta,                        (lenOnlSta * 2) +2,        "" },// DF39 Online process status
		{ emvAuthRes,                       (lenAuthRes * 2) +2,       "" },// Authorization result (telium)
		{ emvCommOk,                        (lenCommOk * 2) +2,        "" },// Is communication ok (telium)
		{ emvIssAutDta,                     (lenIssAutDta * 2) +2,     "" },// 91   Issuer Authentication Data
		{ emvAutCod,                        (lenAutCod * 2) +2,        "" },// 89   Authorization code
		{ emvIssSc1,                        (lenIssSc1 * 2) +2,        "" },// 71   issuer script template 1
		{ emvIssSc2,                        (lenIssSc2 * 2) +2,        "" }, // 72   issuer script template 2
		{ emvMrcPrcCry,                     6,                         "" }, // 9F63 PUNATC(Track1) indicates to the Kernel the positions in the discretionary data field of Track 1 Data where the Unpredictable Number (Numeric) digits and Application Transaction Counter digits have to be copied.
		{ traReversalFlag,                  1,                         "" }, // 72   issuer script template 2
		{ traRqsProcessingCode,             lenPrcCod*2,               "000000" }, // 72   issuer script template 2
		{ traRqsMTI,                        lenMti + 2,                "020000" }, // 72   issuer script template 2
		{ traRqsBitMap,                     (lenBitmap*4) + 2,         "00000000000000000000000000000000" }, // 72   issuer script template 2
		{ traAID,                           lenAID+2,                  "0000000000000000" }, // 72   issuer script template 2
		{ traApplicationLabel,              (lenAID)*2,                ""}, // Application Label
		{ traCardHolderName,                lenTrk1,                   ""}, // Application Label
		{ traIssTrnCrt,                     (lenIssTrnCrt)*2,          ""}, // Application Label
		{ traTVR,                           (lenTVR)*2,                ""}, // Application Label
		{ traTSI,                           (lenTSI)*2,                ""}, // Application Label
		{ traSTAN,                          lenSTAN,                   ""}, // System Trace Audit Number
		{ traPosEntMod,                     lenPOSE,                   ""},
		{ traEMVDATA,                       lenEMVDATA,                ""},
		{ traDrCr,                          2,                         " " },
		{ traCurrencyLabel,                 4,                         "TZS"},
		{ traCurrencyNum,                   4,                         "834"},
		{ traOtherAmt,                      lenAmt+1,                  "000000000000"},
		{ traReferenceNo,                   32,                        "000000000000"},
		{ traAlternateRsp,                  1000,                      ""},
		{ traCVMused,                       3,                         ""},
		{ traClessMagMode,                  2,                         ""},
		{ traRevVoidData,                   100,                       ""},
		{ traVoid63Data,                    128,                       ""},
		{ traCashbackAmt,                   lenAmt,                    ""},// Amount of the transaction
		{ traIssScript1,                    2,                         ""},
		{ traIssScript2,                    2,                         ""},
		{ traPaypassTrk2_Mstripe,           lenTrk2 + 1,               ""},
		{ traVisaTrk2_Mstripe,              lenTrk2 + 1,               ""},
		{ traCVMR,                          (lenCVMRes)*2,             ""}, // CVM RESULTS
		{ emvTrmRskMng,                     (lenLanPrf)*2,             ""}, // Terminal Risk Management
		{ emvFormFactor,                    256,                       ""}, //
		{ traClessTransType,                3,                         ""}, //
		{ traClessAmtType,                  3,                         ""}, //
		{ traCardKernel,                    5,                         ""}, // Terminal Selected kernel
		{ traManualPan,                     2,                         ""}, // Terminal Selected kernel
		{ traIdleTransaction,               20,                        ""}, // Terminal transaction done at idle
		{ traField063,                      999,                       ""}, // Field 63
		{ traBillerOriginalAmt,             50,                        ""}, // Original amount sent by the biller
		{ traOnlinePinLen,                  10,                        ""}, // Original amount sent by the biller
		{ traUPI_Credit_QuasiCredit,        3,                         "0"}, // Original amount sent by the biller
		{ traPreselectedAid,                lenAID+2,                  "0000000000000000" }, // Original amount sent by the biller
		{ traTLS_PositionMarker,            3,                          "0" },

		/// BILLER PAYMENTS GOVT
		{ traBillerCode,                    20,                        ""}, // Biller account or code without the prefix
		{ traBillerRef,                     50,                        ""}, // Reference or name of person making the payment
		{ traSurchargeAmt,                  50,                        ""}, // Reference or name of person making the payment
		{ traBillerPaymentDetails,          2048,                      ""}, // Reference or name of person making the payment
};

static const char zTraTab[] = "traTSLTab.par";

//****************************************************************************
//                          int traReset(void)
// This function creates "tra" table (\PARAMDISK\traTab.par).
//  If the file already exist, delete it then re-create it.
//  Finally "tra" table is built with parameters initialized to default value.
// This function has no parameters.
// This function has return value.
//	 >=0 : Initialization done (size of bytes reseted).
//   <0  : Initialization failed (FMG failed).
//****************************************************************************

int traReset (void) {
	// Local variables
	// ***************
	FMG_t_file_info xFileInfo;
	word usIdx;
	int iByteNbr=0, iRet;
	char datetime[100 + 1];
	byte temp = 0;


	memset(datetime, 0, sizeof(datetime));

	// Create "tra" table
	// ******************
	iRet = FMG_CreateFile(PARAM_DISK, (char*)zTraTab, FMG_VARIABLE_LENGTH, FMG_WITH_CKECKSUM);
	CHECK((iRet==FMG_SUCCESS)||(iRet==FMG_FILE_ALREADY_EXIST), lblKO);

	if (iRet==FMG_FILE_ALREADY_EXIST) {                  // File already exist?
		iRet = FMG_DeleteFile(PARAM_DISK, (char*)zTraTab);
		CHECK(iRet==FMG_SUCCESS, lblKO);                 // Delete it
		iRet = FMG_CreateFile(PARAM_DISK, (char*)zTraTab, FMG_VARIABLE_LENGTH, FMG_WITH_CKECKSUM);
		CHECK(iRet==FMG_SUCCESS, lblKO);                 // Re-create it
	}

	// Reset "tra" table
	// *****************
	xFileInfo.eCreationType = FMGPathAndName;            // File type with Path and Name
	memset((char*)xFileInfo.ucFilePath, 0, (MAX_FMG_FILE_PATH+1));
	strcpy((char*)xFileInfo.ucFilePath, PARAM_DISK);     // \PARAMDISK

	memset((char*)xFileInfo.ucFileName, 0, (MAX_FMG_FILE_NAME+1));
	strcpy((char*)xFileInfo.ucFileName, zTraTab);        // \traTab.par
	for (usIdx=0; usIdx<traEnd-traBeg; usIdx++) {        // Build "tra" table with parameters filled with default value
		CHECK(tzTra[usIdx].usKey==usIdx+traBeg, lblKO);  // Check if it is the right key

		iByteNbr += (int)tzTra[usIdx].usLen;
		iRet = FMG_AddRecord(&xFileInfo, tzTra[usIdx].pvDefault, (long)tzTra[usIdx].usLen, FMGMiddle, usIdx);
		CHECK(iRet==FMG_SUCCESS, lblKO);
	}

	strcpy(datetime, "20");     //CC
	iRet = getDateTime(datetime + 2);    //CC+YYMMDDhhmmss
	CHECK(iRet > 0, lblKO);

	iRet = mapPut(traDatTim, datetime, lenDatTim);
	CHECK(iRet > 0, lblKO);

	temp = 0;
	mapPutByte(traIssScript1, temp);
	mapPutByte(traIssScript2, temp);

	iRet = iByteNbr;                                     // Size of bytes reseted
	goto lblEnd;

	// Errors treatment
	// ****************
	lblKO:                                                   // Initialization failed
	iRet=-1;
	goto lblEnd;
	lblEnd:
	return iRet;
}

//****************************************************************************
//              int traPut(word usKey, void *pvDat, word usLen)
// This function stores the parameter related to the key into the traTab
//  file.
// This function has parameters.
//     (I-) usKey : Key from enum
//     (I-) pvDat : Parameter to be stored
//     (I-) usLen : Parameter length
// This function has return value.
//   >=0 : Storage done (size of bytes stored).
//   <0  : Storage failed (FMG failed).
//****************************************************************************

int traPut (word usKey,const void *pvDat, word usLen) {
	// Local variables
	// ***************
	FMG_t_file_info xFileInfo;
	long lLength;
	int iRet;

	// Store parameter
	// ***************
	CHECK(tzTra[usKey-traBeg].usKey==usKey, lblKO);     // Check if it is the right key

	lLength = (long)tzTra[usKey-traBeg].usLen;
	if (lLength > usLen)
		lLength = (long)usLen;
	xFileInfo.eCreationType = FMGPathAndName;           // File type with Path and Name
	memset((char*)xFileInfo.ucFilePath, 0, (MAX_FMG_FILE_PATH+1));
	strcpy((char*)xFileInfo.ucFilePath, PARAM_DISK);    // \PARAMDISK

	memset((char*)xFileInfo.ucFileName, 0, (MAX_FMG_FILE_NAME+1));
	strcpy((char*)xFileInfo.ucFileName, zTraTab);       // \traTab.par

	iRet = FMG_ModifyRecord(&xFileInfo, (void *)pvDat, lLength, FMGMiddle, usKey-traBeg);
	CHECK(iRet==FMG_SUCCESS, lblKO);                    // Store the parameter related to this key

	iRet = (int)lLength;                                // Size of bytes stored.
	goto lblEnd;

	// Errors treatment
	// ****************
	lblKO:                                                  // Storage parameter failed
	iRet=-1;
	goto lblEnd;
	lblEnd:
	return iRet;
}

//****************************************************************************
//              int traGet(word usKey, void *pvDat, word usLen)
// This function retrieves the parameter related to the key from the traTab
//  file.
// This function has parameters.
//     (I-) usKey : Key from enum
//     (-O) pvDat : Parameter to be retrieved
//     (I-) usLen : Parameter length
// This function has return value.
//   >=0 : Retrieve done (size of bytes retrieved)
//   <0  : Retrieve failed (FMG failed)
//****************************************************************************

int traGet (word usKey, void *pvDat, word usLen) {
	// Local variables
	// ***************
	FMG_t_file_info xFileInfo;
	long lLength;
	int iRet;

	// Retrieve parameter
	// ******************
	CHECK(tzTra[usKey-traBeg].usKey==usKey, lblKO);

	memset(pvDat, 0, usLen);
	lLength = (long) tzTra[usKey-traBeg].usLen;
	if (lLength > usLen)
		lLength = (long)usLen;
	xFileInfo.eCreationType = FMGPathAndName;            // File type with Path and Name
	memset((char*)xFileInfo.ucFilePath, 0, (MAX_FMG_FILE_PATH+1));
	strcpy((char*)xFileInfo.ucFilePath, PARAM_DISK);     // \PARAMDISK

	memset((char*)xFileInfo.ucFileName, 0, (MAX_FMG_FILE_NAME+1));
	strcpy((char*)xFileInfo.ucFileName, zTraTab);        // \traTab.par

	iRet = FMG_ReadRecord(&xFileInfo, pvDat, &lLength, FMGMiddle, usKey-traBeg);
	CHECK(iRet==FMG_SUCCESS, lblKO);	                 // Retrieve the parameter related to this key

	iRet = (int)lLength;                                 // Size of bytes retrieved.
	goto lblEnd;

	// Errors treatment
	// ****************
	lblKO:                                                   // Retrieve parameter failed
	iRet=-1;
	goto lblEnd;
	lblEnd:
	return iRet;
}

word mapDatLen(word key){
	int beg;
	VERIFY(isSorted(keyBeg,key,keyEnd));  //TODO: Kevcode Assertion fails

	beg= begKey(key);
	VERIFY(beg>0);

	switch(beg){
	case appBeg: return appLen(key);
	case traBeg: return traLen(key);
	default: break;
	}
	return -1;
}


word traLen(word key){

	// Retrieve parameter
	// ******************
	CHECK(tzTra[key-traBeg].usKey == key, lblKO);

	return tzTra[key-traBeg].usLen;

	// Errors treatment
	// ****************
	lblKO:                                                   // Retrieve parameter failed
	return -1;
}
