#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include "Goal.h"
#include "OSL_Layer.h"
#include "GTL_SharedExchange.h"
#include "Cless_Implementation.h"
#include <math.h>

typedef unsigned long card;     ///< 0x00000000..0xFFFFFFFF

#define CHECK(CND,LBL) {if(!(CND)){goto LBL;}}
#define roundFloat(x,d) ((floor(((x)*pow(10,d))+.5))/pow(10,d)) //x = float to round , d = decimal places

#ifndef NUMBER_OF_ITEMS
#define NUMBER_OF_ITEMS(a) (sizeof(a)/sizeof((a)[0]))
#endif

#define NUMBER_OF_LINES(a) (sizeof(a)/sizeof((a)[0]))

#define WAIT_RESPONSE  101

#define PARAM_DISK   "PARAMDISK"

#define PIXEL_BOTTOM   40


#ifndef __DSPH__
#define __DSPH__ 4
#endif

#ifndef __DSPW__
#define __DSPW__ 16
#endif

#ifndef __FNTH__
#define __FNTH__ 16
#endif

#define __EMV__

// Font sizes
#define PRT_NORMAL_SIZE   24
#define PRT_DOUBLE_SIZE   12
#define PRT_QDOUBLE_SIZE   6
#define PRT_HALF_SIZE     48
#define PRT_42COLUMN_SIZE 42


#define __PRTW__ PRT_NORMAL_SIZE

#define prtW __PRTW__
#define dspW __DSPW__
#define dspH __DSPH__
#define fntH __FNTH__


/** \weakgroup SYSBWC byte-word-card manipulation macros
 * @{
 */
///Combine two bytes into a word
#define WORDHL(H,L) ((word)((((word)H)<<8)|((word)L)))

///Extract highest byte from a word
#define HBYTE(W) (byte)(((word)W&0xFF00)>>8)

///Extract lowest byte from a word
#define LBYTE(W) (byte)((word)W&0x00FF)

///Combine two words into a card
#define CARDHL(H,L) ((card)((((card)H)<<16)|((card)L)))

///Extract highest word from a card
#define HWORD(C) (word)(((card)C&0xFFFF0000UL)>>16)

///Extract lowest word from a card
#define LWORD(C) (word)((card)C&0x0000FFFFUL)
/** @} */


///buffer descriptor
typedef struct sBuffer {
	byte *ptr;                  ///< buffer containing the data
	word dim;                   ///< number of bytes in the buffer
	word pos;                   ///< current position
} tBuffer;

/** @} */
/** @} */
void message_RemoveCard(void);

/** @} */
/** @} */

int PAR_PADDED_KSN_Get(unsigned char * buf);
int PAR_INITIAL_KEY_Get(unsigned char * buf);
/** @} */
/** @} */

int ComputeTotAmt(void);
int getDateTime(char *YYMMDDhhmmss);
int strFormatDatTim(word key);
int PrintReceipt(void);
void getResponse(char * ResponseName, card rspCode);
void getResponse_NonNumeric(char * ResponseName, char *rspCode);
card getCurrentMenu(void);
void strtouppercase(char s[]);
int Application_Request_Data(char * Narration, word KeySaveLocation, int lenField, char * mask);
void onlSession(void);
void Magnetic_Card_Transaction(void);
/** @} */
void fncBillerInquiry(void);
int fncBillerBeforePayment(void);
void fncBillerAdvice(void);
void fncBillerConfig(void);
void fncTerminalModeConfig(void);
void fncProcessTemrinalModes(void);
/** @} */
int FUN_EncryptPin(void) ;
int GenerateKeyAndCSR( void );
void TaskSimSlot(void);
void RefreshDB(void);
void confirmGraphicLibHandle(void);
int fncWriteStatusOfConnection(char Status_1_or_0);
int fncTMSConnectionSession(void);

int __Cless_Menu_TestTransactionData(void);
int __Cless_Menu_TestTransactionDumpDatabase(void);
int __Cless_Menu_ParametersDump (void);
int __Cless_Menu_TestChooseOutput (void);
int __Cless_Menu_TestTraces(void);

int __Cless_Menu_DefaultParameters(void);
int __Cless_Menu_EraseParameters (void);
int CLESS_Explicit_DoTransaction(void);

void ManageSTAN(void);
void ShortCutEnhanced(void);
void ShortCutAdminEnhanced(void);
char kbdKey(void);
int AutoSwitchSimSlots(void);
void ManualSwitchSimSlots(int SelectedSlot);
void IdleImageDisplay(void);

void bufInit(tBuffer * buf, byte * ptr, word dim);  ///<Initialize a buffer
word bufDim(const tBuffer * buf);   ///<Get buffer dimension
const byte *bufPtr(const tBuffer * buf);    ///<Get buffer pointer
word bufLen(const tBuffer * buf);   ///<Get buffer length (position)
void bufReset(tBuffer * buf);   ///<Fill the buffer by zeroes and reset the current position
int bufSet(tBuffer * buf, byte val, word num);  ///<Set num bytes of buffer to the value val
int bufIns(tBuffer * buf, word ofs, const byte * dat, int len); ///<Insert data at a given offset
int bufOwr(tBuffer * buf, word ofs, const byte * dat, int len); ///<Overwrite data at a given offset
int bufCat(tBuffer * dst, tBuffer * src);   //Append the content of src buffer to dst buffer
int bufDel(tBuffer * buf, word ofs, int len);   ///<Delete data at a given offset
int bufApp(tBuffer * buf, const byte * dat, int len);   ///<Append data to the end of buffer
int bufCpy(tBuffer * buf, const byte * dat, int len);   ///<Reset buffer and copy new data into it
int bufGet(tBuffer * buf, byte * dat);

/** A shortcut for string appending:
 * the third argument is zero (calculated as strlen)
 */
#define bufAppStr(BUF,STR) bufApp(BUF,(byte *)STR,0) /** Shortcut to append a zero-ended string */

///queue descriptor
typedef struct sQueue {
	tBuffer *buf;               ///< buffer containing the queue elements
	word cur;                   ///< current read position
	word put;                   ///< number of elements put (written)
	word get;                   ///< number of elements to get (written and not read yet)
} tQueue;


// Data base keys
// ==============
enum
{
	keyBeg=0,      // Application parameter keys (Record)
};

enum
{
	appBeg = (keyBeg + 1),      // Application parameter keys (Record)
};

// Application Parameters keys
// ===========================
enum
{
	appCmpDat=appBeg,    // Compile date Mapapp.c     Oct 24 2011
	appCmpTim,           // Compile time Mapapp.c     01:12:49
	appAppName,          // Application Name

	appSerialItem,       // Com Port, Data Bits, Parity, Stop Bits, Baud Rate
	appSerialInit,       //              8         N        1         115200

	appModemItem,        //   ,Data Bits, Parity,   ,Baud Rate
	appModemInit,        // T      8         N    1    115200
	appModemPabx,        // Pabx
	appModemPrefix,      // Prefix
	appModemCountry,     // Country code
	appModemPhone,       // Phone number

	appEthIpLocal,       // Local Ip Address
	appEthPort,          // Port number

	appGprsRequirePin,   // Pin required?
	appGprsApn,          // Apn
	appGprsUser,         // User name
	appGprsPass,         // Password
	appGprsIpRemote,     // Remote Ip Address
	appGprsPort,         // Port number

	appPppPabx,          // Pabx
	appPppPrefix,        // Prefix
	appPppCountry,       // Country code
	appPppPhone,         // Phone number
	appPppUser,          // User name
	appPppPass,          // Password
	appPppIpLocal,       // Local Ip Address
	appPppPort,          // Port number

	appCommRoute,
	appCommSSL,
	appNII,
	apptpduHead,
	apptpduTCPIP,
	apptpduModem,
	apptpduRS232,
	appSTAN,
	appCurBat,
	appCurrCodeAlpha,
	appExp,
	appTID,
	appMID,
	appMerchantName,
	appHeader1,
	appHeader2,
	appHeader3,
	appAppLoggedName,
	appAppVersionNumber,
	appAppVersionName,
	appAppVersion,
	appFooterText,

	////-----  emv data  -----

	emvTrnCurCod,
	emvTrnCurExp,
	emvMrcCatCod,
	emvTrmCntCod,
	emvTrmId,
	emvTrmCap,
	emvTrmTyp,
	emvAddTrmCap,
	emvVlpSupInd,
	emvVlpTrmLim,
	emvLstRspCod,
	emvIFDSerNum,

	appBatchNumber,
	appSessionKey_SecureIso,
	appMkey,
	appTkey,
	appTerminalSerial,

	//appDUKPT_KSI,
	appDUKPT_KSN,
	appDUKPT_BDK,
	appDUKPT_IPEK,
	appFallback,
	appInvNum,
	appTID_1,
	appMID_1,
	appTID_2,
	appMID_2,
	appKeyPart,
	appReversalFlag,
	appAutoReversal,
	appClessMagMode,
	appCVM_ForcePIN,
	appClessModeOff,
	appAdminPass,
	appSupervPass,
	appMerchPass,
	appCurrCodeAlpha1,
	appCurrCodeNumerc1,
	appCurrExp1,
	appCurrCodeAlpha2,
	appCurrCodeNumerc2,
	appCurrExp2,
	appShowControlPanel,
	appLastSettlementDate,
	appCardName,
	appSwitchLoops,

	///BILLER GOVT PAYMENTS
	appBillerPrefix,
	appBillerServiceProductCodes,
	appBillerServiceProduct,
	appIsRef_ON,
	appBillerSurcharge,
	appTerminalMode,

	appEnd
};

enum {
	mnuBeg = (appEnd + 1),      // Application parameter keys (Record)
};

enum {
	mnuMainMenu = mnuBeg,    // INGETRAIN
	mnuCustomer,             // CUSTOMER>
	mnuSale,                 // SALE
	mnuSaleCB,               // SALE + CASHBACK
	mnuDeposit,              // DEPOSIT
	mnuWithdrawal,           // WITHDRAWAL
	mnuPreaut,               // PREAUTHORIZATION
	mnuCompletion,           // COMPLETION
	mnuBalanceEnquiry,       // BALANCE ENQUIRY
	mnuMiniStatement,        // MINISTATEMENT
	mnuPayment,              // PAYMENT
	mnuRefund,               // REFUND
	mnuOffline,              // OFFLINE
	mnuBiller,               // BILLER PAYMENT
	mnuVoid,                 // VOID
	mnuAdjust,               // ADJUST
	mnuReversal,             // REVERSAL
	mnuLogOn,                // LOG ON
	mnuEchoTest,             // ECHO TEST
	mnuCustSettlement,       // CUSTOMER SETTLEMENT

	mnuAgent,                // AGENT
	mnuAgencyDeposit,        // AGENCY DEPOSIT

	mnuMerchant,             // MERCHANT>
	mnuSettlement,           // SETTLEMENT
	mnuDetailedReport,       // DETAILED REPORT
	mnuSummaryReport,        // SUMMARY REPORT
	mnuDuplicateReceipt,     // DUPLICATE RECEIPT
	mnuReprintReceipt,       // REPRINT RECEIPT
	mnuAdmChgPwd,            // ADMIN PASSWORD
	mnuMrcChgPwd,            // MERCH PASSWORD
	mnuMrcReset,             // DELETE BATCH
	mnuMrcResetRev,          // DELETE REVERSAL
	mnuMngUsers,             // MANAGE USERS
	mnuBillerResend,         // BILLER RESEND

	mnuSupervisor,

	mnuAdmin,                // ADMIN>
	mnuTerminalPar,          // Temrinal Parameters
	mnuBillerConfig,         // Biller configuration
	mnuTMKey,    	         // Master key load
	mnuGenerateTLSkey,    	 // Generate TLS key
	mnuCmmIS,                // IP Setup
	mnuClessModeOff,         // CLESS Mstripe or Mchip
	mnuConnMode,             // Terminal Connection Mode
	mnuControlPanel,         // Control Panel enabled mode ON or OFF
	mnuSwapSimSlot,          // Manual SIM Slot swapping
	mnuCvmMode,              // Terminal CVM mode Force PIN or card CVM
	mnuUsbTraces,

	/////--------- PLACE HOLDERS ------------

	mnuBillerInquiry,
	mnuBillerPayment,
	mnuBillerAdvice,
	mnuTerminalMode,

	mnuEnd
};

// Data base keys
// ==============
enum {
	traBeg = (mnuEnd + 1),      // Application parameter keys (Record)
};

// Transaction Parameters
// ===========================
enum {
	traDatTim = traBeg,                         // current date and time CCYYMMDDhhmmss
	traMnuItm,                                  // current menu item selected
	traCtx,                                     // context message
	traSta,                                     // communication session state
	traEntMod,                                  // card entry mode
	traConCode,                                 // condition code
	traTrk2,                                    // track 2 of the card
	traPan,                                     // pan of the card
	traExpDat,                                  // expiry date
	traMaskPan,                                 // Masked pan of the card
	traSrvCd,                                   // Service code of the customer card
	traAmt,                                     // Amount of the transaction
	traTransType,                               // Transaction type
	traCardType,                                // Card type
	traTipAmt,                                  // Tip Amount
	traTotAmt,                                  // Total Amount
	traAutCod,                                  // Authorization (approval) code
	traRspCod,                                  // Response code
	traRrn,                                     // Retrieval Reference Number
	traTxnType,                                 // Current Txn Type - used in rqs
	traIssName,                                 // Issuer Name to be displayed
	traTrk21,                                   // first half of track 2 of the card to be displayed
	traTrk22,                                   // tail track 2 of the card to be displayed
	traCrdSeq,                                  // Card Seq Number
	traBinIdx,                                  // index in Bin multirecord
	traAcqIdx,                                  // index in Acq multirecord
	traIssIdx,                                  // index in Iss multirecord
	traSrv,                                     // temporary varaible for IP Address or Port in dialog
	traTCPIPprm,                                // display when viewing IP add or Port config
	traPriSec,                                  // display when viewing IP add and Port config if Primary or Secondary
	traIPConfig,                                // display when viewing IP add and Port config
	traPinBlk,                                  // Pin block
	traTrk1,                                    // track 1 of the card
	traMnuItmBeforeOffline,                     // current menu item selected
	traTxnTypeBeforeOffline,                    // Current Txn Type - used in rqs
	traIdx, 		                            // transaction counter index number
	traLogTotalCount,                           // buffer placement for total transaction counts
	traTypeName, 	                            // Transaction Type Name
	traTypeCurName, 	                        // Currency Name
	traLogTxnTtlCnt,                            // buffer placement for total transaction counts with length 3
	traLogTotalAmount,                          // buffer placement for total transaction amt
	traAcqName, 		                        // Acquirer Name
	traFindRoc, 		                        // Trace Number to Search
	traVoidFlag, 		                        // Void Flag Indicator , >0 if void or 0 if not
	traFmtAmt, 		                            // Display formatted Amount
	traVoidFlagContext, 		                // Void Flag Indicator Current Value Buffer
	traMnuItmContext,                           // Menu Item Current Value Buffer
	traDatTimContext,                           // Current Value Buffer of date and time CCYYMMDDhhmmss
	traBinIdxContext,                           // Current Value Buffer for index in Bin multirecord
	traAcqIdxContext,                           // Current Value Buffer for index in Acq multirecord
	traIssIdxContext,                           // Current Value Buffer for index in Iss multirecord
	traPanContext,                              // Current Value Buffer for Pan
	traExpDatContext,                           // Current Value Buffer for expiry date
	traTrk2Context,                             // Current Value Buffer for track 2 of the card
	traAutCodContext,                           // Current Value Buffer for Authorization (approval) code
	traAmtContext,                              // Current Value Buffer for amount
	traTipAmtContext,                           // Current Value Buffer for tip amount
	traRrnContext,                              // Current Value Buffer for Retrieval Reference Number
	traEntModContext,                           // Current Value Buffer for card entry mode
	traConCodeContext,                          // Current Value Buffer for condition code
	traCardTypeContext,                         // Current Value Buffer for Card Type
	traTmsMsg,                                  // TMS Msgs
	traTmsMreRec,                               // TMS Msgs
	traTmsEmvFleNme,                            // TMS Emv File Name
	traTmsEmvFleHdl,                            // TMS Emv File Handle
	traTmsEmvFleOff,                            // TMS Emv File Offset(size of the received file)
	traTmsEmvFleSze,                            // TMS Emv File Size
	traTmsEmvFlechnlen,                         // Length of the chunk to be requested
	traTmsDnlWidEmv,                            // TMS Download with or without emv params
	traPriIpBuf,                                // Primary TMS Server Phone Number or IP address/port
	traPriPortBuf,                              // Primary TMS Server Phone Number or IP address/port
	traSecIpBuf,                                // Primary TMS Server Phone Number or IP address/port
	traSecPortBuf,                              // Primary TMS Server Phone Number or IP address/port
	traFmtDate,                                 // Formatted Date for Trx printing DD MMM CCYY
	traFmtTime,                                 // Formatted Time for Trx printing HH:MM
	traInvNum,                                  // temporary invoice number (R.O.C.)
	traRspDspLin1,                              // response display line 1
	traRspDspLin2,                              // response display line 2
	traCVV2,                                    // Card CVV2 from back of card MANULLY entered mode only
	emvAppPrfNam,                               // 9F12 Application Preferred Name
	emvAmtBin,                                  // 81   Transaction Amount Binary
	emvAmtOthNum,                               // 9F03 Amount other numeric
	emvAmtOthBin,                               // 9F04 Amount other binary
	emvTrnTyp,                                  // 9C Transaction Type
	emvAmtNum,                                  // 9F02 Transaction Amount Numeric
	emvTrnDat,                                  // 9A Transaction Date YYMMDD
	emvTrnTim,                                  // 9F21 Transaction Time hhmmss
	emvTrnSeqCnt,                               // 9F41 Transaction Sequence Counter
	emvTSI,                                     // 9B   Transaction Status Information
	emvPAN,                                     // 5A   Application Primary Account Number
	emvPANSeq,                                  // 5F34 PAN sequence number
	emvAIP,                                     // 82   Application interchange profile
	emvCapkIdx,                                 // 8F   CAPK index
	emvTrk1,                                    // 57   Track2 equivalent data
	emvTrk2,                                    // 57   Track2 equivalent data
	emvExpDat,                                  // 5F24 Application Expiration date
	emvCVM,                                     // 8E   Cardholder verification method
	emvCrdAvn,                                  // 9F08 card application version number
	emvChdNam,                                  // 5F20 Cardholder Name
	emvLanPrf,                                  // 5F2D Language Preference
	emvRSAKeyExp,                               // DF7F RSA key exponent
	emvRSAKey,                                  // DF1F RSA key
	emvCVMOutRes,                               // DF38 CVMOUT Result
	emvCVMRes,                                  // 9F34 Cardholder Verification Method Results
	emvTVR,                                     // 95   Terminal verification results
	emvFrcOnl,                                  // DF1C Flag transaction forced online
	emvHotLst,                                  // DF26 Flag set to TRUE if the card is in Opposition List
	emvAmtPrv,                                  // DF25 Amount found in the transaction file, in a previous transaction performed with the same card
	emvRspCod,                                  // 8A   Authorization response code
	emvIAD,                                     // 9F10 Issuer Application Data
	emvIssTrnCrt,                               // 9F26 Issuer Transaction Certificate
	emvCID,                                     // 9F27 Cryptogram Information Data (CID)
	emvUnpNum,                                  // 9F37 Unpredictable Number
	emvTrmDc1,                                  // DF29 terminal decision for Generate AC (TC,AAC,ARQC)
	emvTrmDc2,                                  // DF31 Terminal decision after Generate AC 0=Declined, 1= Approved
	emvATC,                                     // 9F36 Application Transaction Counter (ATC)
	emvDFNam,                                   // 84   Dedicated File (DF) Name
	emvOnlSta,                                  // DF39 Online process status
	emvAuthRes,                                 // Authorization result (telium)
	emvCommOk,                                  // Is communication ok (telium)
	emvIssAutDta,                               // 91   Issuer Authentication Data
	emvAutCod,                                  // 89   Authorization code
	emvIssSc1,                                  // 71   issuer script template 1
	emvIssSc2,                                  // 72   issuer script template 2
	emvMrcPrcCry,                               // 9F63 PUNATC(Track1) indicates to the Kernel the positions in the discretionary data field of Track 1 Data where the Unpredictable Number (Numeric) digits and Application Transaction Counter digits have to be copied.

	traReversalFlag,

	traRqsProcessingCode,
	traRqsMTI,
	traRqsBitMap,

	traAID,
	traApplicationLabel,
	traCardHolderName,
	traIssTrnCrt,
	traTVR,
	traTSI,
	traSTAN,
	traPosEntMod,
	traEMVDATA,
	traDrCr,
	traCurrencyLabel,
	traCurrencyNum,
	traOtherAmt,
	traReferenceNo,
	traAlternateRsp,                            // Response from the host showing extra instructions on screen
	traCVMused,                                 // Type of CVM used by card
	traClessMagMode,                            // CLESS check if the transaction is a Mag mode or a Chip mode
	traRevVoidData,
	traVoid63Data,
	traCashbackAmt,
	traIssScript1,
	traIssScript2,
	traPaypassTrk2_Mstripe,
	traVisaTrk2_Mstripe,
	traCVMR,
	emvTrmRskMng,                               //EMV Data on Terminal Risk Management
	emvFormFactor,                              //CLESS card form factor
	traClessTransType,
	traClessAmtType,

	traCardKernel,
	traManualPan,
	traIdleTransaction,
	traField063,
	traBillerOriginalAmt,
	traOnlinePinLen,
	traUPI_Credit_QuasiCredit,
	traPreselectedAid,
	traTLS_PositionMarker,

	/// BILLER PAYMENTS GOVT
	traBillerCode,
	traBillerRef,
	traSurchargeAmt,
	traBillerPaymentDetails,

	traEnd
};

enum {
	trtSale = (traEnd + 1),
	trtPreAut,
	trtRefund,
	trtCash,
	trtBalance,
	trtVerify,
	trtVoid,
	trtCompletion,
	trtSettlement,
	trtSettlementReconcile,
	trtBatchUpload,
	trtTMSLOGON,
	trtTMSDnlTbl,
	trtTMSLOGOFF,
	trtTMSEmvReq,
	trtTMSEmvFleDnl,
	trtOffline,
	trtManual,
	trtAdjust,
	trtReversal,
	trtLogOn,
	trtEchoTest,

	trtEnd
};


enum {
	emvBeg = (trtEnd + 1),      // Application parameter keys (Record)
};


enum {
	emvAid = emvBeg,
	emvTACDft,
	emvTACDen,
	emvTACOnl,
	emvThrVal,
	emvTarPer,
	emvMaxTarPer,
	emvDftValDDOL,
	emvDftValTDOL,
	emvTrmAvn,
	emvAcqId,
	emvTrmFlrLim,
	emvTCC,
	emvAidTxnType,

	emvEnd
};
// Data base keys end
// ==============
enum {
	keyEnd = (emvEnd + 1),      // Application parameter keys (Record)
};

// Parameters length
// =================
enum {
	lenCmpDat = 11,
	lenCmpTim = 8,

	lenSerialItem = 5,
	lenSerialInit = 16,

	lenModemItem = 3,
	lenModemInit = 16,
	lenModemPabx = 4,
	lenModemPrefix = 4,
	lenModemCountry = 3,
	lenModemPhone = 16,

	lenEthIpLocal = 32,
	lenEthPort = 12,

	lenGprsRequirePin = 1,
	lenGprsPin = 8,
	lenGprsPuk = 8,
	lenGprsApn = 30,
	lenGprsUser = 30,
	lenGprsPass = 30,
	lenGprsIpRemote = 32,
	lenGprsPort = 12,

	lenPppPabx = 4,
	lenPppPrefix = 4,
	lenPppCountry = 3,
	lenPppPhone = 16,
	lenPppUser = 30,
	lenPppPass = 30,
	lenPppIpLocal = 15,
	lenPppPort = 5,
	//==================  ADDED LATER ====================
	wdtPrt = (2+(2*prtW)),
	lenDatTim = 14,
	lenEndTime = 6,
	lenDat = 4,
	lenDatBCD = (lenDat+1)/2,
	lenTim = 6,
	lenTimBCD = (lenTim+1)/2,
	lenMnu = (2+dspW),
	lenTrk2 = 40,
	lenTrk1 = 76,
	lenTrk2Bcd = (lenTrk2+1)/2,
	lenPan = 19,
	lenSrvCd = 3,
	lenExpDat = 4,
	lenAmt = 12,
	lenAmtBcd = (lenAmt+1)/2,
	lenTotAmt = 16,
	lenChn = 16,
	lenApn = 128,
	lenPabx = 4,
	lenPhone = 17,
	lenPrefix = 4,
	lenPwd = 4,
	lenTid = 8,
	lenMid = 15,
	lenSrv = 32,
	lenNII = 3,
	lenHostName = 15,
	lenCardTransType = 15,
	lenTrnRefNum = 12,
	lenTrnApprv = 6,
	lenBatNum = 7,
	lenInvNum = 7,
	lenBitmap = 8,
	lenMti = 4,
	lenPrcCod = 3,
	lenRspCod = 3,
	lenAutCod = 6,
	lenRrn = 12,
	lenCurrSign = 3,
	lenBinPrefix = 5,
	lenName = 10,
	lenIssRef = 24,
	lenkeyData = 254,
	lenTPDU = 5,
	lenSTAN = 6,
	lenPOSE = 4,
	lenEMVDATA = 2048,
	lenPOSEBCD = (lenPOSE+1)/2,
	lenNii = 4,
	lenROC = 6,
	lenAIDTxnType = 2,
	lenAIDTxnTags = 254,
	lenPIN = 8,
	lenBCDMsg = 2,
	lenICCData = 264,
	lenICCDataBCD = (lenICCData+1)/2,
	lenCrdSeq = 3,
	lenCrdSeqBCD = (lenCrdSeq+1)/2,
	lenCrdInpCtl = 3,
	lenPinBlk = 8,
	lenTmsRefNum = 8,
	lenTmsFleChkSze = 444,
	lenTmsIssDatSze = 26,
	lenTmsBinDatSze = 28,
	lenFleNme = 30,
	lenAID = 64,
	lenAppNam = 64,
	lenAppPrfNam = 64,
	lenTrnCurCod = 2,
	lenAmtBin = 4,
	lenTrnTyp = 1,
	lenAmtNum = 6,
	lenTrnCurExp = 1,
	lenTrnDat = 3,
	lenAcqId = 6,
	lenTrmAvn = 2,
	lenMrcCatCod = 2,
	lenTrmCntCod = 2,
	lenTrmFlrLim = 4,
	lenTID = 8,
	lenTrnTim = 3,
	lenTrmCap = 3,
	lenEmvTrmTyp = 1,
	lenAddTrmCap = 5,
	lenTrnSeqCnt = 4,
	lenThrVal = 4,
	lenTarPer = 2,
	lenMaxTarPer = 2,
	lenLstRspCod = 20,
	lenTCC = 1,
	lenIFDSerNum = 8,
	lenDftValDDOL = 254,
	lenDftValTDOL = 254,
	lenTVR = 5,
	lenVlpSupInd = 1,
	lenVlpTrmLim = 6,
	lenCardType = 2,
	lenFloorLimit = 9,
	lenTSI = 2,
	lenEmvPAN = 10,
	lenPANSeq = 1,
	lenAIP = 2,
	lenCapkIdx = 1,
	lenEmvTrk2 = 19,
	lenEmvExpDat = 3,
	lenCVM = 252,
	lenCrdAvn = 2,
	lenChdNam = 26,
	lenLanPrf = 8,
	lenRid = 5,
	lenMod = 254,
	lenRSAKeyExp = 3,
	lenRSAKey = 254,
	lenCVMOutRes = 4,
	lenCVMRes = 3,
	lenFrcOnl = 1,
	lenHotLst = 1,
	lenIAD = 32,
	lenIssTrnCrt = 8,
	lenCID = 1,
	lenUnpNum = 4,
	lenTrmDcs = 1,
	lenATC = 2,
	lenOnlSta = 1,
	lenCommOk = 1,
	lenAuthRes = 1,
	lenEmvRspDat = 64,
	lenIssAutDta = 16,
	lenIssSc1 = 254,
	lenIssSc2 = 254,
	lenDFNam = 9,
	lenLogTotalTxn = 3,
	lenFleHdl = 4,
	txnTypeTip = 500,
	lenMon = 3,
	lenTraName = 8,
	lenLogo = 6,
	lenCommRoute = 1,
	//=====================================================

	lenprtW = prtW,

	lenAppEnd
};



///================= DATABASE FIELDS ==================
//word isoMnuItm;
word orgMnuItm;

char isoDrCr[1 + 1];
char isoMnuItm[lenMnu + 1];
char invoiceNo[lenInvNum + 1];
char isoField000[lenMti + 3];
char isoField001[99 + 1];
char isoField002[99 + 1];
char isoField003[6 + 1];
char isoField004[12 + 1];
char isoField005[12 + 1];
char isoField006[3 + 1];
char isoField007[14 + 1];
char isoField008[8 + 1];
char isoField009[8 + 1];
char isoField010[8 + 1];
char isoField011[6 + 1];
char isoField012[6 + 1];
char isoField013[4 + 1];
char isoField014[4 + 1];
char isoField015[6 + 1];
char isoField016[4 + 1];
char isoField017[4 + 1];
char isoField018[4 + 1];
char isoField019[3 + 1];
char isoField020[3 + 1];
char isoField021[3 + 1];
char isoField022[4 + 1];
char isoField023[4 + 1];
char isoField024[4 + 1];
char isoField025[2 + 1];
char isoField026[2 + 1];
char isoField027[2 + 1];
char isoField028[9 + 1];
char isoField029[9 + 1];
char isoField030[9 + 1];
char isoField031[9 + 1];
char isoField032[99 + 1];
char isoField033[9 + 1];
char isoField034[9 + 1];
char isoField035[99 + 1];
char isoField036[99 + 1];
char isoField037[24 + 1];
char isoField038[12 + 1];
char isoField039[4 + 1];
char isoField040[6 + 1];
char isoField041[8 + 1];
char isoField042[30 + 1];
char isoField043[80 + 1];
char isoField044[9 + 1];
char isoField045[9 + 1];
char isoField046[99 + 1];
char isoField047[999 + 1];
char isoField048[999 + 1];
char isoField049[12 + 1];
char isoField050[4 + 1];
char isoField051[4 + 1];
char isoField052[16 + 1];
char isoField053[16 + 1];
char isoField054[999 + 1];
char isoField055[512 + 1];
char isoField056[99 + 1];
char isoField057[99 + 1];
char isoField058[99 + 1];
char isoField059[99 + 1];
char isoField060[99 + 1];
char isoField061[99 + 1];
char isoField062[2050 + 1];
char isoField063[999 + 1];
char isoField064[8 + 1];
char isoField065[999 + 1];
char isoField066[999 + 1];
char isoField067[999 + 1];
char isoField068[999 + 1];
char isoField069[999 + 1];
char isoField070[999 + 1];
char isoField071[999 + 1];
char isoField072[999 + 1];
char isoField073[999 + 1];
char isoField074[999 + 1];
char isoField075[999 + 1];
char isoField076[999 + 1];
char isoField077[999 + 1];
char isoField078[999 + 1];
char isoField079[999 + 1];
char isoField080[999 + 1];
char isoField081[999 + 1];
char isoField082[999 + 1];
char isoField083[999 + 1];
char isoField084[999 + 1];
char isoField085[999 + 1];
char isoField086[999 + 1];
char isoField087[999 + 1];
char isoField088[999 + 1];
char isoField089[999 + 1];
char isoField090[999 + 1];
char isoField091[999 + 1];
char isoField092[999 + 1];
char isoField093[999 + 1];
char isoField094[999 + 1];
char isoField095[999 + 1];
char isoField096[999 + 1];
char isoField097[999 + 1];
char isoField098[999 + 1];
char isoField099[999 + 1];
char isoField100[999 + 1];
char isoField101[999 + 1];
char isoField102[999 + 1];
char isoField103[999 + 1];
char isoField104[999 + 1];
char isoField105[999 + 1];
char isoField106[999 + 1];
char isoField107[999 + 1];
char isoField108[999 + 1];
char isoField109[999 + 1];
char isoField110[999 + 1];
char isoField111[999 + 1];
char isoField112[999 + 1];
char isoField113[999 + 1];
char isoField114[999 + 1];
char isoField115[999 + 1];
char isoField116[999 + 1];
char isoField117[999 + 1];
char isoField118[999 + 1];
char isoField119[999 + 1];
char isoField120[999 + 1];
char isoField121[999 + 1];
char isoField122[999 + 1];
char isoField123[999 + 1];
char isoField124[999 + 1];
char isoField125[999 + 1];
char isoField126[999 + 1];
char isoField127[999 + 1];
char isoField128[999 + 1];

///====================================================

// Mapapp.c
// ========
int appReset(void);
int appPut(word usKey,const void *pvDat, word usLen);
int appGet(word usKey, void *pvDat, word usLen);
word appLen (word usKey);

// Maptra.c
// ========
int traReset(void);
int traPut(word usKey,const void *pvDat, word usLen);
int traGet(word usKey, void *pvDat, word usLen);
word traLen(word key);
int mapGet_AID_Data(word emvkey ,unsigned char * BinData);//extract from sqlite

int mapGet(word key,void *ptr,word len); ///<retrieve data element
int mapPut(word key,const void *ptr,word len); ///<save data element
word mapDatLen(word key);

int begKey(word key);

#define DIM(a)			(sizeof(a)/sizeof((a)[0]))

#define mapGetByte(KEY,DST) mapGet(KEY,&DST,sizeof(byte))
#define mapPutByte(KEY,SRC) mapPut(KEY,bPtr(SRC),sizeof(byte))
#define mapGetWord(KEY,DST) mapGet(KEY,&DST,sizeof(word))
#define mapPutWord(KEY,SRC) mapPut(KEY,wPtr(SRC),sizeof(word))
#define mapGetCard(KEY,DST) mapGet(KEY,&DST,sizeof(card))
#define mapPutCard(KEY,SRC) mapPut(KEY,cPtr(SRC),sizeof(card))
#define mapPutStr(KEY,SRC) mapPut(KEY,SRC,0)

#define MAPGET(KEY,BUF,LBL) { ret= mapGet(KEY,BUF,sizeof(BUF)); CHECK(ret>=0,LBL);}
#define MAPGETBYTE(KEY,VAR,LBL) { ret= mapGetByte(KEY,VAR); CHECK(ret>=0,LBL);}
#define MAPGETWORD(KEY,VAR,LBL) { ret= mapGetWord(KEY,VAR); CHECK(ret>=0,LBL);}
#define MAPGETCARD(KEY,VAR,LBL) { ret= mapGetCard(KEY,VAR); CHECK(ret>=0,LBL);}

#define MAPPUTSTR(KEY,VAR,LBL) { ret= mapPutStr(KEY,VAR); CHECK(ret>=0,LBL);}
#define MAPPUTBYTE(KEY,VAR,LBL) { ret= mapPutByte(KEY,VAR); CHECK(ret>=0,LBL);}
#define MAPPUTWORD(KEY,VAR,LBL) { ret= mapPutWord(KEY,VAR); CHECK(ret>=0,LBL);}
#define MAPPUTCARD(KEY,VAR,LBL) { ret= mapPutCard(KEY,VAR); CHECK(ret>=0,LBL);}

// LL GPRS Status 
// ==============
#define LL_STATUS_GPRS_NO_SIM                 LL_ERRORS+10
#define LL_STATUS_GPRS_ERR_SIM_LOCK           LL_ERRORS+9
#define LL_STATUS_GPRS_ERR_PPP                LL_ERRORS+8
#define LL_STATUS_GPRS_ERR_UNKNOWN            LL_ERRORS+7

// LL ETHERNET Status
// ==================
#define LL_STATUS_ETH_AVAILABLE               LL_ERRORS+20
#define LL_STATUS_ETH_NO_DEFAULT_ROUTE        LL_ERRORS+19
#define LL_STATUS_ETH_NOT_PLUGGED             LL_ERRORS+18
#define LL_STATUS_ETH_BASE_NOT_READY          LL_ERRORS+17
#define LL_STATUS_ETH_OUT_OF_BASE             LL_ERRORS+16

// LL PHYSICAL Status
// ==================
#define LL_STATUS_PHY_PERIPHERAL_BUSY         LL_ERRORS+30
#define LL_STATUS_PHY_PERIPHERAL_OUT_OF_BASE  LL_ERRORS+31
#define LL_STATUS_PHY_CONVERTER_NOT_PLUGGED   LL_ERRORS+32

// LL BLUETOOTH Status
// ===================
#define LL_STATUS_BT_NO_BASE_ASSOCIATED		  LL_ERRORS+40
#define LL_STATUS_BT_PERIPHERAL_NOT_AVAILABLE LL_ERRORS+41
#define LL_STATUS_BT_PERIPHERAL_BUSY		  LL_ERRORS+42
#define LL_STATUS_BT_BASE_NOT_AVAILABLE		  LL_ERRORS+43

// Application
// ===========

byte num2dec(char *dec, card num, byte len);    ///< numeric value num --> decimal string dec of length len
byte dec2num(card * num, const char *dec, byte len);    ///< decimal string dec of length len --> numeric value num
byte num2hex(char *hex, card num, byte len);    ///< numeric value num --> hexadecimal string hex of length len
byte hex2num(card * num, const char *hex, byte len);    ///< hexadecimal string hex of length len --> numeric value num
byte bin2num(card * num, const byte * bin, byte len);   ///< binary buffer bin of length len --> numeric value num using direct byte order
byte num2bin(byte * bin, card num, byte len);   ///< numeric value num --> binary buffer bin of length len using direct byte order
int bin2hex(char *hex, const byte * bin, int len);  ///< binary buffer bin of length len --> hexadecimal string hex
int hex2bin(byte * bin, const char *hex, int len);  ///< hexadecimal string hex --> binary buffer bin of length len
int Hex2Text(char * Text, const char *hex, int len);

/** @} */
void stdDES(byte * dst, byte * src, byte * key);    ///<Encrypt the binary array src using the key and put the result into the buffer dst
void stdSED(byte * dst, byte * src, byte * key);    ///<Decrypt the binary array src using the key key and put the result into the buffer dst
void stdEDE(byte * dst, byte * src, byte * key);    ///<Triple DES encryption of  binary array src by double length key
void stdDED(byte * dst, byte * src, byte * key);    ///<Triple DES decryption of  binary array src by double length key
int stdOWF(byte * dst, const byte * dat, const byte * key); ///<One Way Function of binary array dat by simple length key

/** @} */

int fmtSbs(char *dst, const char *src, byte ofs, byte len); ///<copy into dst substring of src starting from ofs of length
int fmtTok(char *dst, const char *src, const char *sep);    ///<Extract a token dst from the string src that can contain several tokens separated by characters from sep
int fmtPad(char *str, int len, char chr);   ///<pad the string str to length abs(len) by the character chr
int fmtCnt(char *str, int len, char chr);   ///<center the string str to length abs(len) by the character chr
int fmtAmt(char *dst, const char *src, byte exp, const char *sep);  ///<format an amount string with decimals
int fmtMut(char *dst, const char *fgd, const char *bgd, const char *ctl);   ///<Permutation of fgd and bgd according to the control string ctl
int fmtIns(char *dst, const char *bgd, const char *ins, byte ofs, int len); ///<insert a string to an another sting
int fmtDel(char *dst, const char *src, byte ofs, int len);  ///<erase the len long substring from ofs
int fmtTcp(char *dst, char *src, int len);  ///<format TCP/IP address
int fmtDate(char *dst, char *src, int len); //firas
int fmtTime(char *dst, char *src, int len); //firas

/** @} */
/** @} */
int ComSSL(tBuffer * req,tBuffer * rsp);
void ComSSL_Prepare(void);

int comCheckSslProfile(void);
void ComGPRS_Prepare(void);
int comGPRS_SetDefaultsValues(void);

void comWifi_Prepare(void);
int comWifiConnect(tBuffer * req,tBuffer * rsp, word SSL);

void All_AfterTransaction(void);
void TaskInitiateGPRS(void);
int isReversibleSend(void);
byte isSorted(word a, word b, word c);
void revAutoReversal(void);
int incCard(word key);
void CommsGetChannel(byte CommsChannelNow);
int isApproved(void);

/** @} */
/** @} */

int MaskPan(void);
int ApplicationVoid(char * STAN_Rsp);
void ApplicationBuildReversalData(void);
void dateTimeRefresh(void);

/** @} */
/** @} */

int tmrStart(byte tmr, int dly);    ///<start the timer tmr for dly centiseconds
int tmrGet(byte tmr);           ///<get the number of centiseconds waitng for timer tmr
void tmrStop(byte tmr);         ///<stop the timer tmr
void tmrPause(byte sec);        ///<Halts and pauses the processing for a given number of seconds
int getTS(char unit);           ///<returns the number of time units since first call or since 01/01/1970
void tmrSleep(card dly);        ///<sleep for dly seconds

/** \weakgroup SYSBIT Bit operations
 * The leftmost bit of buf has number 1
 *
 * The rightmost bit of the first byte of buf has number 8
 *
 * The leftmost bit of the second byte of buf has number 9
 *
 * etc
 * @{
 */
void bitOn(byte * buf, byte idx);   ///< turn on bit idx in buf
void bitOff(byte * buf, byte idx);  ///< turn off bit idx in buf
byte bitTest(const byte * buf, byte idx);   ///< test the state of bit idx in buf
void tbxParity(byte * buf, word dim, byte par, byte typ);   ///<set parity bits
byte tbxCheckParity(byte * buf, word dim, byte par, byte typ);  ///<check parity bits

/** \weakgroup PRIARI Arithmetic operations with strings
 * @{
 */
byte addStr(char *dst, const char *arg1, const char *arg2); ///<dst:= arg1+arg2 in decimal; the maximum length is 16
byte subStr(char *dst, const char *arg1, const char *arg2); ///<dst:= arg1-arg2 in decimal; the maximum length is 16


/** @} */

int count_chars(const char* string, char ch);

/** @} */

void xor(const byte * b1, const byte * b2, byte * dst, word len);


/** \weakgroup SYSPTR Pointer shortcuts
 * The following functions return pointers to constant values of given type.
 *
 * For example, bPtr(2) returns a pointer to a byte that contains the value 2.
 * @{
 */
const byte *bPtr(byte val);     ///< return the pointer to a byte containing the value val
const word *wPtr(word val);     ///< return the pointer to a word containing the value val
const card *cPtr(card val);     ///< return the pointer to a card containing the value val
const byte *dPtr(const char *s);    ///< return the pointer to a binary data


/** @} */

int OpenPeripherals(void);
void ClosePeripherals(void);
void ResetPeripherals(unsigned int uiEvents);
void GlDialogMessage(void);
void GlDialogInput(void);
void GlDialogList(void);
void GlDialogFile(void);
void Layout(void);
void DisplayLabel(void);
void Picture(void);
void AnimatedPicture(void);
void Drawing(void);
void PrintDocument(void);
void Keyboard(void);
void IsoError(int iSta, char *pcTrk);
void Magnetic(void);
void Smart(void);
void PromptSerial(void);
int ComSerial(tBuffer * req,tBuffer * rsp, word SSL);
void PromptModem(void);
int ComModem(tBuffer * req,tBuffer * rsp, word SSL);
int ComUSB(tBuffer * req,tBuffer * rsp, word SSL);
void PromptEthernet(void);
int ComEthernet(tBuffer * req,tBuffer * rsp, word SSL);
int ComEthernetCheck(int SSL);
void PromptGPRS(void);
int ComGPRS(tBuffer * req,tBuffer * rsp, word SSL);
int ComGPRSCheck(int SSL);
void PromptPPP(void);
int ComPPP(tBuffer * req,tBuffer * rsp, word SSL);
void VFSWrite(int VFSType);
void VFSRead(int VFSType);
void VFSDelete(int VFSType);
void Buzzer(void); 
void Backlight(void);
void TaskAnimation(void);
void TaskMutex(void);
void TaskSemaphore(void);
void Delay(void);
long TimerStart(byte ucTimerNbr, long lDelay);
long TimerGet(byte ucTimerNbr);
int TimerStop(byte ucTimerNbr);
void Timer(void);
void DiskCreate(void);
void FileWrite(void);
void FileRead(void);
void DiskUse(void);
void DiskKill(void);
void FMGCreateBatch(void);
void FMGDeleteBatch(void);
void FMGAddDelTransactions(void);
void FMGReadTransactions(void);
T_OSL_HDLL BeepDll_Open(void);
void BeepDll(void);
void BeepLib(void);
int GetPpdDisplay(void);
void SetConfig(void);
void GetConfig(void);
void CreateSecret(void);
void FreeSecret(void);
void LoadIso9564Key(void);
void Iso9564PinEntry(void);
void LoadMacKey(void);
void MacCalculation(void);
int IacCallUser2(void);
void TrainingToUser2(void);
void TrainingToAll(void);
void TrainingToItself(void);
void TrainingToTask(void);
void Candara(void);
void German(void);
void Polish(void);
void Turkish(void);
void Russian(void);
void Arabic(void);
void Greek(void);
void FontUtf8(void);
void GetMsg(void);
void SelectLang(void);
void Testing(void);
void emvStop(void);
void ConfFlash(doubleword uiCom);
void ConfSoftware(doubleword uiCom);
void ConfHardware(doubleword uiCom);
void CrashScenario(void);
void SoftwareReset(void);
char* FMG_ErrorMsg (int iValue);
char* LL_ErrorMsg (int iValue); 
char* SEC_ErrorMsg (int iValue);
int isApp_Already_in_Session(void);
Telium_File_t *OpenConverter(char *pcName, byte ucSize);
void SetParity(char *pcBuf, word usDim, byte ucPar, byte ucTyp);
byte CheckParity(const char *pcBuf, word usDim, byte ucPar, byte ucTyp);
int ResetParam(void);
int SetParam (word usKey, char *pcParam);
int GetParam (word usKey, char *pcParam, word usLen);
const char *parseStr(char ucToken, char *pcDst, const char *pcSrc, int iDim);
char* UintToIp(doubleword uiVal);
doubleword IpToUint(const char *pcStr);
int IngenicoFontExist(unsigned int usAppliType, char *pcFamilyName);
int ParamExist(word usAppliType, char *pcFamilyName);
int AppliExist(word usAppliType);
int Application_Get_Destination_Card(word KeySaveLocation);
word ApplicationSelectCurrency(void);
word ApplicationCurrencyFill(char * Currency);
word ApplicationCurrencyFillAuto(char * Currency);
int Application_Completion(void);
void applicationClessMagMode(void);
void applicationCVM_Mode(void);
void applicationTraces(void);
char* FMG_ErrorMsg (int iValue);

/** @} */

int reqBuild(tBuffer * req);
int rspParse(const byte * rsp, word len) ;

/** @} */

int EnterPin(void);
int FUN_PinEntry(void);

/** @} */
/** @} */

int performOlineTransaction(void);
int checkOlineServer(void);
int TransactionFlow(void);
/** @} */
/** @} */

int PrintDetailedLog(void);
int PrintSummaryLog(void);
int PrintDuplicateReceipt(void);
int PrintReprintReceipt(void);

/** @} */
int logSave(void);
int logReset(void);
void logSettlement(void);
void logIncrementBatch(void);
int logPrintSummaryLog(void);
void logGetUniqueTransactionRecordMenus(char * MenuRecords);
int logGetLastStan(char *STAN);
int logGetLastInv(char *Inv);
void logCalcTot(char *Curr, char *Debits, char *Credits,char *DebitReversal, char *CreditReversal, char *DebitCount, char *CreditCount, char *DebitReversalCount, char *CreditReversalCount, char * Totals);
/** @} */
/** @} */

void MemoryManagerClearGarbage(void);
void fncPromptTerminalParameters(void);
void fncUserManager(void);
void fncAutoSettlementChecker(void);
int fncIsNumeric(char * isNum);
void fncSwitchSimSlot(void);
void fncDigestConfigFile(char * tcRsp);
int fncReadConfigFile(void);
void fncKeyManager(void);
void fncShowControlPanel(void);
int fncSecurityPassword(char * SecurityLevel);
int fncOverwriteConfigFile(char * FileName, char * tcRsp,int iLen);
int fncDisplayData_Goal(const char *header, const char *line1, const char *line2,int duration,int beeping) ;
int fncConfirmTotalAmount(void);
void fncResetTerminalData(void);
void fncPanRemove_F(char * PAN_Number);

/** @} */
/** @} */
void AdviseTransactionManager(void);
/** @} */
/** @} */

int CLESS_Data_Save_To_DB(T_SHARED_DATA_STRUCT* traKernelBuf );
/** @} */
/** @} */

int getOnlinePin(void);
/** @} */
/** @} */
void CLESS_Prepare_DoTransaction(void);
void CLESS_End_DoTransaction(void);

/** @} */
/** @} */
unsigned char *LoadBMP(char *BmpFileName);

/** @} */
/** @} */

int valRspCod(void);

/** @} */
/** @} */

void queInit(tQueue * que, tBuffer * buf);  ///<Initialize a queue
int quePut(tQueue * que, const byte * dat, int len);    ///<Put a data element into a queue
int queGet(tQueue * que, byte * dat);   ///<Retrieve a data element from the queue
word queNxt(const tQueue * que);    ///<Get the size of the next data element in the queue
word queLen(const tQueue * que);    ///<Get queue length (number of elements not rtetrieved yet)
void queReset(tQueue * que);    ///<Empty the queue
void queRewind(tQueue * que);   ///<Put the current position to the beginning of the queue
int quePutTag(tQueue * que, card tag);  ///<Put a tag into queue
int queGetTag(tQueue * que, card * tag);    ///<Retrieve a tag from the queue
int quePutTlv(tQueue * que, card tag, word len, byte * val);    ///<Put (Tag-Length-Value) triple into the queue
int queGetTlv(tQueue * que, card * tag, word * len, byte * val);    ///<Retrieve (Tag-Length-Value) from the queue
word queFindTlv(tQueue * que, card tag, byte * val);    ///<Find Tag and obtain its Length and Value from the queue
int queMergeTlv(tQueue * que1, tQueue * que2);  ///<Merge TLV Data from 2 Queue's

/** @} */


enum eKbd {                     ///<The key codes
	kbdBeg,                     ///< start sentinel

	kbdVAL = 0x16,              ///< Enter
	kbdANN = 0x17,              ///< Esc
	kbd00 = 0x3C,               ///< not available
	kbdCOR = 0x18,              ///< BkSp
	kbdINI = 0x28,              ///< F button
	kbdFWD = 0x07,              ///< Paper Feed, not available at application level
	kbdF1 = 0x19,               ///< F1
	kbdF2 = 0x20,               ///< F2
	kbdF3 = 0x21,               ///< F3
	kbdF4 = 0x22,               ///< F4
	kbdF5 = 0x44,               ///< not available
	kbdF6 = 0x45,               ///< not available
	kbdF7 = 0x46,               ///< not available
	kbdF8 = 0x47,               ///< not available
	kbdDN = 0x24,               ///< Down arrow
	kbdUP = 0x23,               ///< Up arrow
	kbdMNU = kbdINI,            ///< MENU button
	kbdR = '.',                 ///< T_POINT
	kbdJST = 0x25,              ///< NAVI_OK
	kbdJUP = kbdF4,             ///< joystick up
	kbdJDN = kbdF3,             ///< joystick down
	kbdJLF = 'D',               ///< not available
	kbdJRG = 'F',               ///< not available

	kbdEnd
};

#endif
