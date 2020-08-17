#ifndef __EMV_SUPPORT_H__
#define __EMV_SUPPORT_H__

#include <globals.h>

word mapKeyTag(card tag);



#ifdef __EMV__

#define __OWNTAGPRE__ 0xDF

enum eTag {                     //EMV tag values
	tagBeg,
	tagAid = 0x4F,              //Application Identifier
	tagAppLbl = 0x50,           //Application Label
	tagCmdPfm = 0x52,           //Command to Perform
	tagTrk1 = 0x56,             //Track 1 Equivalent Data
	tagTrk2 = 0x57,             //Track 2 Equivalent Data
	tagPAN = 0x5A,              //Application Primary Account Number (PAN)
	tagChdNam = 0x5F20,         //Cardholder Name
	tagExpDat = 0x5F24,         //Application Expiration Date
	tagEffDat = 0x5F25,         //Application Effective Date
	tagIssCntCod = 0x5F28,      //Issuer Country Code
	tagTrnCurCod = 0x5F2A,      //Transaction Currency Code
	tagLanPrf = 0x5F2D,         //Language Preference
	tagSrvCod = 0x5F30,         //Service Code
	tagPANSeq = 0x5F34,         //Application Primary Account Number (PAN) Sequence Number
	tagTrnCurExp = 0x5F36,      //Transaction Currency Exponent
	tagActTyp = 0x5F57,         //Account Type. Indicates the type of account selected on the terminal, coded as specified in Annex G. Length = 1 byte.
	tagAplTpl = 0x61,           //Application Template
	tagFCITpl = 0x6F,           //File Control Information (FCI) Template
	tagAEFDtaTpl = 0x70,        //Application Elementary File (AEF) Data Template
	tagIssSc1 = 0x71,           //Issuer Script Template 1
	tagIssSc2 = 0x72,           //Issuer Script Template 2
	tagDirDscTpl = 0x73,        //Directory Discretionary Template
	tagRspMsgTpl2 = 0x77,       //Response Message Template Format 2
	tagRspMsgTpl1 = 0x80,       //Response Message Template Format 1
	tagAmtBin = 0x81,           //Amount, Authorised (Binary)
	tagAIP = 0x82,              //Application Interchange Profile
	tagCmdTpl = 0x83,           //Command Template
	tagDFNam = 0x84,            //Dedicated File (DF) Name
	tagIssScrCmd = 0x86,        //Issuer Script Command
	tagAplPrtInd = 0x87,        //Application Priority Indicator
	tagSFI = 0x88,              //Short File Identifier (SFI)
	tagAutCod = 0x89,           //Authorisation Code
	tagRspCod = 0x8A,           //Authorisation Response Code
	tagCDOL1 = 0x8C,            //Card Risk Management Data Object List 1 (CDOL1)
	tagCDOL2 = 0x8D,            //Card Risk Management Data Object List 2 (CDOL2)
	tagCVM = 0x8E,              //Cardholder Verification Method (CVM) List
	tagCapkIdx = 0x8F,          //Certificate Authority Public Key Index ICC
	tagPubKeyCrt = 0x90,        //Issuer Public Key Certificate
	tagIssAutDta = 0x91,        //Issuer Authentication Data
	tagIssPubRem = 0x92,        //Issuer Public Key Remainder
	tagSgnStaApl = 0x93,        //Signed Static Application Data
	tagAFL = 0x94,              //Application File Locator (AFL)
	tagTVR = 0x95,              //Terminal Verification Results
	tagTDOL = 0x97,             //Transaction Certificate Data Object List (TDOL)
	tagTCHshVal = 0x98,         //Transaction Certificate (TC) Hash Value
	tagTrnPINDta = 0x99,        //Transaction Personal Identification Number (PIN) Data
	tagTrnDat = 0x9A,           //Transaction Date
	tagTSI = 0x9B,              //Transaction Status Information
	tagTrnTyp = 0x9C,           //Transaction Type
	tagDDFNam = 0x9D,           //Directory Definition File (DDF) Name
	tagAcqId = 0x9F01,          //Acquirer Identifier (BIN)
	tagAmtNum = 0x9F02,         //Amount, Authorised (Numeric)
	tagAmtOthNum = 0x9F03,      //Amount, Other (Numeric)
	tagAmtOthBin = 0x9F04,      //Amount, Other (Binary)
	tagAplDscDta = 0x9F05,      //Application Discretionary Data
	tagAIDT = 0x9F06,           //Application Identifier (AID) Terminal
	tagAplUsgCtl = 0x9F07,      //Application Usage Control
	tagCrdAvn = 0x9F08,         //Application Version Number CCard
	tagTrmAvn = 0x9F09,         //Application Version Number Terminal
	tagCrdNamExt = 0x9F0B,      //Cardholder Name Extended
	tagIACDft = 0x9F0D,         //Issuer Action Code Default
	tagIACDnl = 0x9F0E,         //Issuer Action Code Denial
	tagIACOnl = 0x9F0F,         //Issuer Action Code Online
	tagIAD = 0x9F10,            //Issuer Application Data
	tagIssCodIdx = 0x9F11,      //Issuer Code Table Index
	tagAppPrfNam = 0x9F12,      //Application Preferred Name
	tagLstOnlATC = 0x9F13,      //Last Online Application Transaction Counter (ATC Register)
	tagLowConOfl = 0x9F14,      //Lower Consecutive Offline Limit
	tagMrcCatCod = 0x9F15,      //Merchant Category Code
	tagMrcID = 0x9F16,          //Merchant Identifier
	tagPINTryCnt = 0x9F17,      //Personal Identification Number (PIN) Try Counter
	tagIssScrID = 0x9F18,       //Issuer Script Identifier
	tagAccCntCod = 0x9F1A,      //Card Acceptor Country Code
	tagTrmFlrLim = 0x9F1B,      //Terminal Floor Limit
	tagTrmId = 0x9F1C,          //Terminal Identification
	tagTrmRskMng = 0x9F1D,      //Terminal Risk Management Data
	tagIFDSerNum = 0x9F1E,      //Interface Device (IFD) Serial Number
	tagTr1DscDta = 0x9F1F,      //Track 1 Discretionary Data
	tagTr2DscDta = 0x9F20,      //Track 2 Discretionary Data
	tagTrnTim = 0x9F21,         //Transaction Time
	tagCrtAutPubT = 0x9F22,     //Certification Authority Public Key Index Terminal
	tagUppConOfl = 0x9F23,      //Upper Consecutive Offline Limit
	tagIssTrnCrt = 0x9F26,      //Issuer Transaction Certificate
	tagCID = 0x9F27,            //Cryptogram Information Data (CID)
	tagClsAppCap = 0x9F28,      //Contactless Application Capabilities Type
	tagICCPINCrt = 0x9F2D,      //ICC PIN Encipherment Public Key Certificate
	tagICCPINExp = 0x9F2E,      //ICC PIN Encipherment Public Key Exponent
	tagICCPINRmd = 0x9F2F,      //ICC PIN Encipherment Public Key Remainder
	tagIssPubExp = 0x9F32,      //Issuer Public Key Exponent
	tagTrmCap = 0x9F33,         //Terminal Capabilities
	tagCVMRes = 0x9F34,         //Cardholder Verification Method (CVM) Results
	tagTrmTyp = 0x9F35,         //Terminal Type
	tagATC = 0x9F36,            //Application Transaction Counter (ATC)
	tagUnpNum = 0x9F37,         //Unpredictable Number
	tagPDOL = 0x9F38,           //Processing Options Data Object List (PDOL)
	tagPOSEntMod = 0x9F39,      //PointOfService (POS) Entry Mode
	tagAmtRefCur = 0x9F3A,      //Amount, Reference Currency
	tagAplRefCur = 0x9F3B,      //Application Reference Currency
	tagTrnRefCur = 0x9F3C,      //Transaction Reference Currency
	tagTrnRefExp = 0x9F3D,      //Transaction Reference Currency Exponent
	tagAddTrmCap = 0x9F40,      //Additional Terminal Capabilities
	tagTrnSeqCnt = 0x9F41,      //Transaction Sequence Counter
	tagAplCurCod = 0x9F42,      //Application Currency Code
	tagAplRefExp = 0x9F43,      //Application Reference Currency Exponent
	tagAplCurExp = 0x9F44,      //Application Currency Exponent
	tagDtaAutCod = 0x9F45,      //Data Authentication Code
	tagICCPubCrt = 0x9F46,      //ICC Public Key Certificate
	tagICCPubExp = 0x9F47,      //ICC Public Key Exponent
	tagICCPubRmd = 0x9F48,      //ICC Public Key Remainder
	tagDDOL = 0x9F49,           //Dynamic Data Object List (DDOL)
	tagSDALst = 0x9F4A,         //Static Data Authentication Tag List
	tagSgnDynApl = 0x9F4B,      //Signed Dynamic Application Data
	tagICCDynNbr = 0x9F4C,      //ICC Dynamic Number
	tagAplCurCodV = 0x9F51,     //VISA Application Currency Code
	tagADA = 0x9F52,            //Application Default Action (ADA)
	// tagCnsTrnLimV = 0x9F53,      //VISA Consecutive Transaction Limit (international)
	tagTCC = 0x9F53,            //MC Transaction Category Code
	tagCumTotLimV = 0x9F54,     //VISA Cumulative Total Transaction Amount Limit
	tagGeoInfV = 0x9F55,        //VISA Geographic Indicator
	tagIssAutInd = 0x9F56,      //Issuer Authentication Indicator
	tagIssCntCodV = 0x9F57,     //VISA Issuer CountryCode
	tagLowConOflV = 0x9F58,     //VISA Lower Consecutive Offline Limit
	tagUppConOflV = 0x9F59,     //VISA Upper Consecutive Offline Limit
	tagCrdAccNamM = 0x9F5A,     //Card Acceptor Name (MCard)
	tagCrdAccNamT = 0x9F5B,     //Card Acceptor Name (Terminal)
	tagCrdTrcNum = 0x9F5C,      //Ccard Trace Number
	tagMrcTrcNum = 0x9F5D,      //Mcard Trace Number
	tagSpnPwr = 0x9F5E,         //Spending Power
	tagTrnCod = 0x9F5F,         //Transaction Code
	tagMrcPrcCry = 0x9F63,      //Mcard Purchase Cryptogram or PUNATC(TRACK1) indicates to the Kernel the positions in the discretionary data field of Track 1 Data where the Unpredictable Number (Numeric) digits and Application Transaction Counter digits have to be copied.
	tagTrmTraCpb = 0x9F66,      //Terminal Transaction Capabilities (Visa kernel only)
	tagPaypassTrack2 = 0x9F6B,	//TAG_PAYPASS_TRACK2_DATA /*!< PAYPASS - Track 2 Data contains the data objects of the track 2 according to [ISO/IEC 7813], excluding start sentinel, end sentinel and LRC.<br><br>	- Format : b.<br>	- Length : var. up to 19 bytes.<br>	- Source : Card. */

	tagFormFactor = 0x9F6E,     //The form factor of the consumer payment device and the type of contactless interface over which the transaction was conducted
	tagExclusivData = 0x9F7C,   // Customer Exclusive Data

	tagVisaTrack2 =  0x9F918303,//TAG_PAYWAVE_MAGSTRIPE_TRACK_2 /*!< Original Track 2 converted from the track 2 equivalent data.<br> - Format : B.<br> - Length : 1 to 70 bytes.<br> - Source : Terminal. */
	tagVlpSupInd = 0x9F7A,      //VLP Terminal Support Indicator
	tagVlpTrmLim = 0x9F7B,      //VLP Terminal Transaction Limit
	tagOnlSta = WORDHL(__OWNTAGPRE__, 0x39),    //Online process status

	tagKernelTraceError = 0x9F918406,
	tagCTQ = 0x9F6C,
	tagPayWaveDeclinedByCard = 0x9F91831D,  //Reason By Card for decline
	tagEMV_AfterODA_Error = 0x9F918C70,  //Contains an error code if ODA failed : : TAG_EMV_INT_ODA_ERROR_values for possible values


#ifdef __TELIUM__
	tagLastAPDUCmdRsp = 0x9F8102,   //Last APDU Command Response
	tagAIDLstTrm = 0x9F8110,
	tagEMVDCOpt = 0x9F8111,     //Special options to be applied to the transaction.
	tagAmtPrv = 0x9F8112,       //Indicates the cumulative transaction amount for the current card
	tagDlyPinCod = 0x9f811C,    //!< Obsolete value : Do not use.
	tagDlyInterChar = 0x9f811D, //!< Obsolete value : Do not use.
	tagRSAKeyExp = 0x9F8122,    //RSA key exponent
	tagRSAKey = 0x9F8123,       //RSA key
	tagDftValDDOL = 0x9F8124,   //Default value for DDOL
	tagDftValTDOL = 0x9F8125,   //Default value for TDOL
	tagMaxTarPer = 0x9F8126,    //Maximum target percentage
	tagTarPer = 0x9F8127,       //Target percentage
	tagTACDft = 0x9F8128,       //Terminal Action Code Default
	tagTACDen = 0x9F8129,       //Terminal Action Code Denial
	tagTACOnl = 0x9F812A,       //Terminal Action Code Online
	tagThrVal = 0x9F812B,       //Threshold value
	tagScrRes = 0x9F812D,       //result of script execution
	tagScrResLst = 0x9F812F,    //Issuer Script Result List
	tagScrResLstLen = 0x9F8137, //Issuer Script Result List Length
	tagAutoMate = 0x9F8132,     //Result of processing of CVM on the application side (i.e : result of pin entry)
	tagStatusCVP = 0x9F8133,    //Current Cardholder Verication Method to be processed
	tagPinCode = 0x9F8134,      //Pin value (Fill in with dummy bytes, as Pin value is no more accessible
	tagRemainPin = 0x9F8135,    //Number of remaining PIN tries
	tagSignature = 0x9F8136,    //Signature
	tagBypassPin = 0x9F8139,    //Pin Bypass True (1) if bypass required - 1 byte
	tagSchemasKSU = 0x9F813A,   //Identify the type of schemes to be managed. Only one scheme is defined up to now : Scheme for PIN presentation
	tagPinPadOK = 0x9F813B,     //Boolean on one Byte. True if Pinpad is functioning
	tagAIDProp = 0x9F8140,
	tagCompatibility2000 = 0x9F8141,    // Boolean TRUE for a compatibility with EMV 2000 requirements.
	tagUsePSE = 0x9F8142,       // Boolean indicating whether the PSE algorithm shall be used or not. (By default, only the list of AID method is used).
	tagResPropMeth = 0x9F8147,  //Indicate if CVM proprietary method is successfull
	tagWaitAfterATR = 0x9F818D, //Special need for EMV probe tools : Add a delay after ATR.
	tagPSEAlgo = 0x9F8195,      // Enables to require an alternative option to the PSE algorithm.
	tagTRMOverpassAIP = 0x9F81A3,   //Indicates if function TRM has to be done even if AIP value does not indicate TRM to do.
	tagCustStaCod = 0x9F8401,   //EMV Custom service status code. Coded on 2 bytes.
	tagCustAppName = 0x9F8402,  //Name of EMV application, displayed by M2OS. Name on 8 alphanumeric bytes.
	tagCustIdleMsg = 0x9F8403,  //Idle message to be displayed, if no other application is present. Name on 16 alphanumeric bytes.
	tagNumAIDTrm = 0x9F8407,    //Number of AID managed by the application. Length = 1 byte.
	tagCustKSU = 0x9F8408,      //Boolean, TRUE if the pin code should be managed by KSU (if present). Length = 2 bytes.
	tagCustBlacklist = 0x9F8409,    //Boolean, TRUE if the custom component manages the black list. Length = 2 bytes.
	tagCustAdviceOnline = 0x9F840B, //Boolean, TRUE if advices are supported and generate an online message. Length = 2 bytes.
	tagCustFrcOnl = 0x9F840E,   //Boolean, TRUE if the merchant is allowed to force the transaction to go online. Length = 2 bytes.
	tagCustFrcAprv = 0x9F840F,  // Boolean, TRUE if the merchant is allowed to force acceptance of the transaction. Length = 2 bytes.
	tagCustCommAppTyp = 0x9F8411,   //Application type associated to COMM component. Length = 2 bytes.
	tagFrcOnl = 0x9F8414,       //Flag transaction forced online TAG_CUST_FORCED_ONLINE_REQUEST
	tagLstTraAmtBin = 0x9F8416, //!< Amount of the last transaction. Binary value on 4 bytes. (most significant byte is byte 0).
	tagLstTraCurCod = 0x9F8417, //!< Currency code of the last transaction. Length = 2 bytes.
	tagHotLst = 0x9F841B,       //Flag set to TRUE if the card is in Black List
	tagCustAppSelID = 0x9F841D,
	tagCustTRMOverpassAIP = 0x9F841E,   //Indicates if function TRM has to be done even if AIP value does not indicate TRM to do.
	tagCustSrvAuthNum = 0x9F8420,   //Number of the service to be called for online authorization request.
	tagCustCommsOk = 0x9F8422,  //Boolean, in case of on-line authorisation, indicates if the communication succeeded. Length = 2 bytes.
	tagCusrCARevok1 = 0x9F8425, //First tag to use to fill in the list.
	tagCusrCARevok2 = 0x9F8426, //Second tag use only if previous one it full.
	tagCusrCARevok3 = 0x9F8427, //Third tag use only if previous ones are full.
	tagCusrCARevok4 = 0x9F8428, //Fourth tag use only if previous ones are full.
	tagCusrCARevok5 = 0x9F8429, //Fifth tag use only if previous ones are full.
	tagFrcAcceptance = 0x9F842A,    //Boolean, TRUE if the merchant wants to force the transaction acceptance. Length = 1 byte.
	tagCustPosISO8583_93 = 0x9F842B,
	tagCommManagedByCust = 0x9F8430,
	tagCustSupportedLang = 0x9F8431,
	tagCustMultilangSupport = 0x9F8441,
	tagCustOnlineCapture = 0x9F8448,
	tagCustCardholderConf = 0x9F844B,
	tagCustBatchCapture = 0x9F8447,
	tagCustFlrLimitChecking = 0x9F8453,
	tagCustRandTraSelect = 0x9F8454,
	tagCustVelocityChecking = 0x9F8455,
	tagCustODAProcessing = 0x9F8458,
	tagCustActType = 0x9F8459,
	tagCustSkipDefautl = 0x9F845A,
	tagCustSkipOnline = 0x9F845B,
	tagCustDetectFailPriorTAA = 0x9F845C,
	tagCustCDANeverReqARQC1GenAC = 0x9F845D,
	tagCustCDANeverReqOnl2GenAC = 0x9F845E,
	tagCustSubsequentBypassPin = 0x9F8460,
	tagCustFinalSelCall = 0x9F8462, //Tag indicating if CUSTOM manages the step EMVDC_FINAL_SELECTION or not : FASLE do not manager EMVDC_FINAL_SELECTION, TRUE manages EMVDC_FINAL_SELECTION. Length = 1 byte.
	tagMsgCodNum = 0x9F8703,    //Message code number. This is the ID of a standard message, to display. Length = 2 bytes.
	tagCrdholderLang = 0x9F8704,    //Language to use. On 2 bytes represented by 2 alphabetical characters according to ISO 639.
	tagAmtRequired = 0x9F8706,  //TRUE  : Engine asks Custom for an amount entry.  FALSE: Amount is already available. No amount entry needed.
	tagDspDestination = 0x9F8722,   //Display Destination
#else
	tagTACDft = WORDHL(__OWNTAGPRE__, 0x03),    //Terminal Action Code Default
	tagTACDen = WORDHL(__OWNTAGPRE__, 0x04),    //Terminal Action Code Denial
	tagTACOnl = WORDHL(__OWNTAGPRE__, 0x05),    //Terminal Action Code Online
	tagThrVal = WORDHL(__OWNTAGPRE__, 0x07),    //Threshold value
	tagTarPer = WORDHL(__OWNTAGPRE__, 0x08),    //Target percentage
	tagMaxTarPer = WORDHL(__OWNTAGPRE__, 0x09), //Maximum target percentage
	tagScrRes = WORDHL(__OWNTAGPRE__, 0x11),    //result of script execution
	tagDftValDDOL = WORDHL(__OWNTAGPRE__, 0x15),    //Default value for DDOL
	tagDftValTDOL = WORDHL(__OWNTAGPRE__, 0x18),    //Default value for TDOL
	tagFrcOnl = WORDHL(__OWNTAGPRE__, 0x1C),    //Flag transaction forced online
	tagRSAKey = WORDHL(__OWNTAGPRE__, 0x1F),    //RSA key
	tagAmtPrv = WORDHL(__OWNTAGPRE__, 0x25),    //Amount found in the transaction file, in a previous transaction performed with the same card
	tagHotLst = WORDHL(__OWNTAGPRE__, 0x26),    //Flag set to TRUE if the card is in Opposition List
	tagRSAKeyExp = WORDHL(__OWNTAGPRE__, 0x7F), //RSA key exponent
	tagDftValONL = WORDHL(__OWNTAGPRE__, 0x19), //Online DOL
	tagDftValAMT = WORDHL(__OWNTAGPRE__, 0x1A), //Amount DOL
	tagDftValRSP = WORDHL(__OWNTAGPRE__, 0x1B), //Responce DOL
	tagTraLst = WORDHL(__OWNTAGPRE__, 0x3A),    //Last Tra
	tagDebugInfo = WORDHL(__OWNTAGPRE__, 0x42), //Debug Information
	tagTRMOverpassAIP = WORDHL(__OWNTAGPRE__, 0x0B),    //Debug Information
#endif
	tagTrmDc1 = WORDHL(__OWNTAGPRE__, 0x29),    //Terminal decision for Generate AC (TC,AAC,ARQC)
	tagTrmDc2 = WORDHL(__OWNTAGPRE__, 0x31),    //Terminal decision after Generate AC 0=Declined, 1= Approved
	tagLstRspCod = WORDHL(__OWNTAGPRE__, 0x16), //List of possible response codes
	tagCVMOutRes = WORDHL(__OWNTAGPRE__, 0x38), //CVMOUT Result

	tagAuthRes = 0x9F811A,      //Issuer decision on one byte : 0 declined, 1 accepted
	tagODA = 0x9F81A2,          //Offline Data Authentication
	tagDetFaitPriTAA = 0x9F81A6,    //TAG_DETECT_FAIL_PRIOR_TAA - no comment
	tagCDANevReqARQ1 = 0x9F81A7,    //TAG_CDA_NEVER_REQUEST_ARQC_1GENAC - no comment
	tagCDANevReqONL2 = 0x9F81A8,    //TAG_CDA_NEVER_REQUEST_ONLINE_2GENAC - no comment
	tagCommOk = 0x9F8422,       //Boolean, In case of on-line authorisation, indicates if the communication succeeded. Length = 2 bytes

	//emv engine tags
	tagEngWaitCrdTimeout = 0x9f8730,
	tagEngWaitCrdCancelEvent = 0x9f8731,
	tagEngRmvCrdTimeout = 0x9f8732,
	tagEngRmvCardCancelEvent = 0x9f8733,
	tagEngCustAppType = 0x9f8734,
	tagEngAppSelPerformed = 0x9f8735,
	tagEngWaitChipRdr = 0x9f8736,
	tagEngRemoveChipRdr = 0x9f8737,
	tagFlrLmtChecking = 0x9F819F,   //Inidcates if floor limit checking
	tagRndTrnSel = 0x9F81A0,    //Indicates if random transaction selection will be performed
	tagVelChecking = 0x9F81A1,  //Indicates if velocity checking will be performed

	tagEnd
};
enum eTsi {                     //Transaction Status Information
	tsiBeg,
	tsiOflDtaAut,               //Offline data authentication was performed
	tsiChdVrf,                  //Cardholder verification was performed
	tsiCrdRskMng,               //Card risk management was performed
	tsiIssAut,                  //Issuer authentication was performed
	tsiTrmRskMng,               //Terminal risk management was performed
	tsiScrPrc,                  //Script processing was performed
	tsiEnd
};

enum eTvr {                     //terminal verification results bits
	tvrBeg,

	tvrOflDtaAut,               //Offline data authentication was not performed
	tvrOflSdtAut,               //Offline static data authentication failed
	tvrICCDtaMis,               //ICC data missing
	tvrCrdExcFil,               //Card appears on terminal exception file
	tvrOflDdtAut,               //Offline dynamic data authentication failed
	tvrRFU13,
	tvrRFU12,
	tvrRFU11,

	tvrICCTrmApl,               //ICC and terminal have different application versions
	tvrExpApl,                  //Expired application
	tvrAplNotEff,               //Application not yet effective
	tvrSrvNotAlw,               //Requested service not allowed for card product
	tvrNewCrd,                  //New card
	tvrMrcNotOpr,               //Mcard not operative
	tvrRFU22,
	tvrRFU21,

	tvrCrdVrf,                  //Cardholder verification was not successful
	tvrUnrCVM,                  //Unrecognised CVM
	tvrPINTryLim,               //PIN Try Limit exceeded
	tvrPPNotPrs,                //PIN entry required and PIN pad not present or not working
	tvrPINNotEnt,               //PIN entry required, PIN pad present, but PIN was not entered
	tvrOnlPINEnt,               //Online PIN entered
	tvrRFU32,
	tvrRFU31,

	tvrFlrLimExc,               //Transaction exceeds floor limit
	tvrLwrOflLim,               //Lower consecutive offline limit exceeded
	tvrUprOflLim,               //Upper consecutive offline limit exceeded
	tvrSelRndOnl,               //Transaction selected randomly for online processing
	tvrTrnFrcOnl,               //Merchant forced transaction online
	tvrRFU43,
	tvrRFU42,
	tvrRFU41,

	tvrDftTDOL,                 //Default TDOL used
	tvrIssAut,                  //Issuer authentication was unsuccessful
	tvrScrBfr,                  //Script processing failed before final GENERATE AC
	tvrScrAft,                  //Script processing failed after final GENERATE AC
	tvrRFU54,
	tvrRFU53,
	tvrRFU52,
	tvrRFU51,

	tvrEnd
};

enum eAuc {                     //application usage control bits
	aucBeg,

	aucDomCsh,                  //Valid for domestic cash transactions
	aucIntCsh,                  //Valid for international cash transactions
	aucDomGds,                  //Valid for domestic goods
	aucIntGds,                  //Valid for international goods
	aucDomSrv,                  //Valid for domestic services
	aucIntSrv,                  //Valid for international services
	aucATM,                     //Valid at ATMs
	aucNonATM,                  //Valid at terminals other than ATMs

	aucDomCbk,                  //Domestic cashback allowed
	aucIntCbk,                  //International cashback allowed
	aucRFU26,
	aucRFU25,
	aucRFU24,
	aucRFU23,
	aucRFU22,
	aucRFU21,

	aucEnd
};

enum eAip {                     //application interchange profile bits
	aipBeg,

	aipIni,                     //Initiate
	aipOflStaAut,               //Offline static data authentication is supported
	aipOflDynAut,               //Offline dynamic data authentication is supported
	aipChdVrf,                  //Cardholder verification is supported
	aipTrmRskMng,               //Terminal risk management is to be performed
	aipIssAut,                  //Issuer authentication is supported
	aipRFU12,
	aipRFU11,

	aipRFU28,
	aipRFU27,
	aipRFU26,
	aipRFU25,
	aipRFU24,
	aipRFU23,
	aipRFU22,
	aipRFU21,

	aipEnd
};

enum eTcp {                     //terminal capabilities bits
	tcpBeg,

	tcpManKeyEnt,               //Manual Key Entry
	tcpMagStr,                  //Magnetic Stripe
	tcpICC,                     //IC with contacts
	tcpRFU15,
	tcpRFU14,
	tcpRFU13,
	tcpRFU12,
	tcpRFU11,

	tcpPlnPinICC,               //Plaintext PIN for ICC verification
	tcpEncPinOnl,               //Enciphered PIN for online verification
	tcpSigPpr,                  //Signature (paper)
	tcpEncPinOff,               //Enciphered PIN for offline verification
	tcpRFU24,
	tcpRFU23,
	tcpRFU22,
	tcpRFU21,

	tcpSDA,                     //Static data authentication
	tcpDDA,                     //Dynamic data authentication
	tcpCrdCap,                  //Card capture
	tcpRFU35,
	tcpRFU34,
	tcpRFU33,
	tcpRFU32,
	tcpRFU31,

	tcpEnd
};

enum eAcp {                     //additional terminal capabilities bits
	acpBeg,

	acpCsh,                     //Cash
	acpGds,                     //Goods
	acpSrv,                     //Services
	acpCbk,                     //Cashback
	acpInq,                     //Inquiry
	acpTrf,                     //Transfer
	acpPmt,                     //Payment
	acpAdm,                     //Administrative

	acpRFU28,
	acpRFU27,
	acpRFU26,
	acpRFU25,
	acpRFU24,
	acpRFU23,
	acpRFU22,
	acpRFU21,

	acpNumKey,                  //Numeric keys
	acpChrKey,                  //Alphabetic and special characters keys
	acpCmdKey,                  //Command keys
	acpFunKey,                  //Function keys
	acpRFU34,
	acpRFU33,
	acpRFU32,
	acpRFU31,

	acpPrtAtt,                  //Print, attendant
	acpPrtChd,                  //Print, cardholder
	acpDspAtt,                  //Display, attendant
	acpDspChd,                  //Display, cardholder
	acpRFU44,
	acpRFU43,
	acpCT0,                     //Code table 10
	acpCT9,                     //Code table 9

	acpCT8,                     //Code table 8
	acpCT7,                     //Code table 7
	acpCT6,                     //Code table 6
	acpCT5,                     //Code table 5
	acpCT4,                     //Code table 4
	acpCT3,                     //Code table 3
	acpCT2,                     //Code table 2
	acpCT1,                     //Code table 1

	acpEnd
};

enum eCvm {                     //CVMOUT result bits
	cvmBeg,

	cvmPinRevocationPossible,
	cvmOfflinePinOK,
	cvmLastAttempt,
	cvmWrongPin,
	cvmOfflinePin,
	cvmOnlinePin,
	cvmSignature,
	cvmEnding,

	cvmRFU28,
	cvmRFU27,
	cvmRFU26,
	cvmRFU25,
	cvmRFU24,
	cvmRFU23,
	cvmRFU22,
	cvmProprietaryCVM,

	cvmTelium,                  ///<Telium specific follows

	cvmNO_REMAINING_PIN = 52,   ///< PIN Try limit Exceeded
	cvmINPUT_PIN_OFF = 53,      ///< Perform PIN Entry for offline verification
	cvmINPUT_PIN_ON = 54,       ///< Perform PIN Entry for online verification
	cvmINPUT_PIN_OK = 55,       ///< PIN entry performed
	cvmNO_INPUT = 56,           ///< CVM End
	cvmPP_HS = 60,              ///< Pinpad Out of Services
	cvmCANCEL_INPUT = 61,       ///< Pin entry cancelled
	cvmTO_INPUT = 62,           ///< Time Out during PIN Entry
	cvmREMOVED_CARD = 63,       ///< Card Removed
	cvmERROR_CARD = 64,         ///< Card Error
	cvmMORE = 65,               ///< Internal use only
	cvmERROR_ENC_DATA = 66,     ///< Error during PIN Encpherment

	cvmEnd
};

#ifdef __CMV__
enum cTag {                     //CMV tag values
	tagGuiMod = (int) 0x9F928230,   //TAG_GENERIC_GUI_MODE: GUI mode which can be either CLESS_GUI_MODE_NONE/CLESS_GUI_MODE_NONE/CLESS_GUI_MODE_VISA_EUROPE/CLESS_GUI_MODE_VISA_ASIA/CLESS_GUI_MODE_MASTERCARD_COLORED_LEDS
	tagTrmSupFDDAVer = (int) 0x9f918307,    //TAG_PAYWAVE_TERM_SUPPORTED_FDDA_VERSIONS
	tagPmtFlwStp = (int) 0x9F918400,    //TAG_KERNEL_PAYMENT_FLOW_STOP: Indicates the steps on which the kernel shall stop the processing and give hand back to the custom application
	tagPmtFlwCus = (int) 0x9F918401,    //TAG_KERNEL_PAYMENT_FLOW_CUSTOM: Indicates the steps on which the kernel shall call the curstom application for step customisation
	tagSigReq = (int) 0x9F918413,   //TAG_KERNEL_SIGNATURE_REQUESTED: Indicates whether a signature is required or not (1 if signature is required, 0 or not present if not required)
	tagBlkLst = (int) 0x9F918415,   //TAG_KERNEL_PAN_IN_BLACK_LIST: Indicates if the Card PAN is in the black list or not
	tagOnlPinReq = (int) 0x9F918417,    //TAG_KERNEL_ONLINE_PIN_REQUESTED   Indicates if online PIN is requested by the kernel or not (1 if Online PIN is requested, 0 or not present if not).
	tagTrmSupLan = (int) 0x9F91841D,    //TAG_KERNEL_TERMINAL_SUPPORTED_LANGUAGES: List of languages supported by the terminal (concatenation of languages identifiers : for example "enfrde"). From this tag would be determined the common prefered language (if kernel supports multi-language), stored in the TAG_KERNEL_SELECTED_PREFERED_LANGUAGE tag
	tagCmvPpUdol = (int) 0x9F918502,    //TAG_PAYPASS_DEFAULT_UDOL: The Default UDOL is the UDOL to be used for constructing the value field of the COMPUTE CRYPTOGRAPHIC CHECKSUM command if the UDOL in the card is not present. The Default UDOL must always be present and must contain as its only entry the tag and length of the UN (Numeric). The value of the Default UDOL must be: '9F6A04'
	tagCmvPpMstI = (int) 0x9F918503,    //TAG_PAYPASS_MSTRIPE_INDICATOR: Indicates for each AID whether the PayPass - Mag Stripe profile is supported or not by the PayPass reader. Its value is implementation specific (0 if not supported, 1 if supported)
	tagCmvPpCvmY = (int) 0x9F918504,    //TAG_PAYPASS_TERMINAL_CAPABILITIES_CVM_REQ: Indicates the card data input, CVM, and security capabilities of the terminal and PayPass reader when the transaction amount is greater than or equal to the Terminal CVM Required Limit
	tagCmvPpCvmN = (int) 0x9F918505,    //TAG_PAYPASS_TERMINAL_CAPABILITIES_NO_CVM_REQ: Indicates the card data input, CVM, and security capabilities of the terminal and PayPass reader when the transaction amount is below the Terminal CVM Required Limit
	tagPayPassCvm = (int) 0x9F918506,   //TAG_PAYPASS_TRANSACTION_CVM: Data object used to indicate to the terminal the outcome of the CVM Selection function
	tagPayPassRefund = (int) 0x9F918519,    //TAG_PAYPASS_INT_USE_REFUND_FLOW: indicates if the refund transaction flow as descibed in the PayPass 2.1 specification shall be used
	tagCmvPpAvnL = (int) 0x9F91850D,    //TAG_PAYPASS_INT_MSTRIPE_TERMINAL_AVN_LIST: Internal tag indicating the list of MStripe application version number supported by the terminal. This consist of a concatenation of AVN (each of them is coded on 2 bytes). ()
	tagCmvMcAvnL = (int) 0x9F918511,    //TAG_PAYPASS_INT_MCHIP_TERMINAL_AVN_LIST: Internal tag indicating the list of MChip application version number supported by the terminal. This consists of a concatenation of AVN (each of them is coded on 2 bytes)
	tagCmvPpCapkLstIdx = (int) 0x9F918517,  //TAG_PAYPASS_INT_SUPPORTED_CAPK_INDEX_LIST: PAYPASS - Internal tag containing the list of CA Public Key index list (concatenation of indexes (each one coded on one byte)). This tag can contain up to 64 supported key indexes. - Format : b.- Length : up to 64 bytes.
	tagCmvInteracCapkLstIdx = (int) 0x9F918A17, //TAG_INTERAC_INT_SUPPORTED_CAPK_INDEX_LIST: INTERAC - Internal tag containing the list of CA Public Key index list (concatenation of indexes (each one coded on one byte)). This tag can contain up to 64 supported key indexes.    - Format : b.   - Length : up to 64 bytes.
	tagCmvTrnTyp = (int) 0x9F918701,    //TAG_EMV_INT_TRANSACTION_TYPE: Internal transaction type. It can be different than 0x9C (value shall be 0x00, 0x01, 0x02, 0x09 or 0x20)
	tagCmvTdol = (int) 0x9F918706,  //TAG_EMV_INT_DEFAULT_TDOL: Default TDOL. DOL to be used for generating the TC Hash Value if the TDOL in the card is not present.
	tagCmvTACDft = (int) 0x9F918709,    //TAG_EMV_INT_TAC_DEFAULT: Terminal Action Code Default. Specifies the acquirer's conditions that cause a transaction to be rejected if it might have been approved online, but the terminal is unable to process the transaction online
	tagCmvTACDnl = (int) 0x9F91870A,    //TAG_EMV_INT_TAC_DENIAL: Terminal Action Code Denial. Specifies the acquirer's conditions that cause the denial of a transaction without attempt to go online
	tagCmvTACOnl = (int) 0x9F91870B,    //TAG_EMV_INT_TAC_ONLINE: Terminal Action Code Online. Specifies the acquirer's conditions that cause a transaction to be transmitted online
	tagCmvAidOpt = (int) 0x9F928100,    //TAG_EP_AID_OPTIONS: AID Options
	tagCmvKerUse = (int) 0x9F928101,    //TAG_EP_KERNEL_TO_USE: Indicates the default kernel to use for a given AID
	tagCmvAddRes = (int) 0x9F928104,    //TAG_EP_AID_ADDITIONAL_RESULTS: Contains the pre processing additional results for an AID (Exceeded floor limit, etc)
	tagFinSelCmd = (int) 0x9F928107,    //TAG_EP_FINAL_SELECT_COMMAND_SENT    : Final SELECT command sent to the card
	tagFinSelRsp = (int) 0x9F928108,    //TAG_EP_FINAL_SELECT_RESPONSE: Final SELECT card response
	tagFinSelSta = (int) 0x9F928109,    //TAG_EP_FINAL_SELECT_STATUS_WORD: Final SELECT card response Status Word
	tagCmvLim = (int) 0x9F92810D,   //TAG_EP_CLESS_TRANSACTION_LIMIT: Indicates the limit for which contactless transaction can be conducted in the terminal relating to the AID
	tagCmvCvm = (int) 0x9F92810E,   //TAG_EP_CLESS_CVM_REQUIRED_LIMIT: Indicates the contactless floor limit from which a CVM is required relating to an AID
	tagCmvFlr = (int) 0x9F92810F,   //TAG_EP_CLESS_FLOOR_LIMIT: Indicates the maximum transaction amount relating to the AID
	tagNumCndLst = (int) 0x9F928110,    //TAG_EP_NUMBER_OF_CANDIDATE_AID_IN_LIST: Indicates the number of AID available in the candidate list after the application selection process
	tagAidPropId = (int) 0x9F928202,    //TAG_GENERIC_AID_PROPRIETARY_IDENTIFIER: Indicates the proprietary identifier of a given AID (to differentiate an AID configuration according to specific parameters)
	tagDetTyp = (int) 0x9F928210,   //TAG_GENERIC_DETECTION_TYPE: Card detection type
	tagDetDly = (int) 0x9F928212,   //TAG_GENERIC_DETECTION_GLOBAL_TIMEOUT: Card detection global timeout
	tagMaxDetNum = (int) 0x9F928214,    //TAG_GENERIC_DETECTION_NB_CARDS_TO_DETECT: Maximum number of cards that can be detected
	tagDbgAct = (int) 0x9F918402,   //TAG_KERNEL_DEBUG_ACTIVATION: Indicates the kernel debug mode (tag sent on the kernel debug API function, if available).
	tagCapkChk = (int) 0x9F918702,  //TAG_EMV_INT_CAPK_CHECKSUM
	tagCapkExp = (int) 0x9F918703,  //TAG_EMV_INT_CAPK_EXPONENT: Certification Authority Public Key Exponent.
	tagCapkMod = (int) 0x9F918704,  //TAG_EMV_INT_CAPK_MODULUS:Certification Authority Public Key Modulus
	tagVisaWaveCvm = (int) 0x9F918903,  //TAG_VISAWAVE_TRANSACTION_CVM: Data object used to indicate to the terminal the outcome of the CVM Selection function. Possible values are "No CVM", "Signature", "Online PIN". The coding of the value is implementation specific
	tagAidInfo = (int) 0xBF928102,  //TAG_EP_AID_INFORMATION: Constructed tag containing all the AID information.
	tagCanLst = (int) 0xBF928103,    //TAG_EP_CANDIDATE_LIST_ELEMENT

	/* New tags for paypass 3.0
	 */
	tagPaypassMerchantCustData = (int) 0x9f7c, //TAG_PAYPASS_MERCHANT_CUSTOM_DATA: Proprietary merchant data that may be requested by the card.
	tagPaypassMStripeAppVerNumTerm = (int) 0x9f6d, //TAG_PAYPASS_MSTRIPE_APPLI_VERSION_NUMBER_TERM: Version number assigned by the payment system for the specific PayPass – Mag Stripe functionality of the application (terminal value).
	tagPaypassClessLimitNoDcv = (int) 0x9f91851c, //TAG_PAYPASS_CLESS_TRANSACTION_LIMIT_NO_DCV: Indicates the transaction amount limit above which the transaction is not allowed, when on-device cardholder verification is not supported.
	tagPaypassClessLimitWithDcv = (int) 0x9f91851d, //TAG_PAYPASS_CLESS_TRANSACTION_LIMIT_DCV: Indicates the transaction amount limit above which the transaction is not allowed, when on-device cardholder verification is supported.
	tagPaypassOPS = (int) 0x9F91851F, //TAG_PAYPASS_OUTCOME_PARAMETER_SET: Indicates to the Terminal the outcome of the transaction processing by the Kernel.
	tagPaypassKernelCfg = (int) 0x9f918522, //TAG_PAYPASS_KERNEL_CONFIGURATION: Indicates the Kernel configuration options. Set this to 0 if you support both MChip and MStripe transaction flows.
	tagPaypassCardDataInputCap = (int) 0x9F918523, //TAG_PAYPASS_CARD_DATA_INPUT_CAPABILITY: Indicates the card data input capability of the Terminal and Reader.
	tagPaypassSecCap = (int) 0x9f918524,//TAG_PAYPASS_SECURITY_CAPABILITY: Indicates the security capability of the Kernel
	tagPaypassMChipCVMCapCVMReq = (int) 0x9f918525, //TAG_PAYPASS_MCHIP_CVM_CAPABILITY_CVM_REQUIRED: Indicates the MChip CVM Capability of the Terminal and Reader when transaction amount is greater than the Reader CVM Required Limit.
	tagPaypassMChipCVMCapCVMNotReq = (int) 0x9f918526, //TAG_PAYPASS_MCHIP_CVM_CAPABILITY_CVM_NOT_REQUIRED: Indicates the MChip CVM Capability of the Terminal and Reader when transaction amount is lesser than or equal to the Reader CVM Required Limit.
	tagPaypassMStripeCVMCapCVMReq = (int) 0x9f918527, //TAG_PAYPASS_MSTRIPE_CVM_CAPABILITY_CVM_REQUIRED: Indicates the MStripe CVM Capability of the Terminal and Reader when transaction amount is greater than the Reader CVM Required Limit.
	tagPaypassMStripeCVMCapCVMNotReq = (int) 0x9f918528, //TAG_PAYPASS_MSTRIPE_CVM_CAPABILITY_CVM_NOT_REQUIRED: Indicates the MStripe CVM Capability of the Terminal and Reader when transaction amount is lesser than or equal to the Reader CVM Required Limit.
	tagPaypassIntId = (int)0x9F91852C, //TAG_PAYPASS_INT_RID: Indicates the RID to be used to retreive the CA public key
	tagPaypassMaxNumTornTxnLog = (int) 0x9f918561, //TAG_PAYPASS_MAX_NUMBER_OF_TORN_TXN_LOG_RECORDS: Maximum number of records that can be contained in the torn transaction log.
	tagPaypassTornRecordTagList = (int)0x9F918564, //TAG_PAYPASS_TORN_RECORD_TAG_LIST: List of transaction data that is stored in a torn record.
	tagPaypassIntDoNotRemoveTornRecord = (int)0x9F918567, //TAG_PAYPASS_INT_DO_NOT_REMOVE_TORN_RECORD: Internal tag indicating if the Torn Record shall not be removed from the Torn Log (1 if shall not be removed, 0 or absent indicates the record shall be removed).<br>Note that this tag is unknow from the kernel and is provided just as indication for Torn Management.
	tagPaypassMaxLifeTimeTornTxnLog = (int) 0x9f918565, //TAG_PAYPASS_MAX_LIFETIME_OF_TORN_TXN_LOG_RECORD: Maximum time, in seconds, that a record can remain in the Torn Transaction Log.
	tagPaypassCustDataExchange = (int) 0x9F91854D, //TAG_PAYPASS_DATA_EXCHANGE_CUSTOM: Indicates the how the kernel shall call the custom application for data exchange
};

enum cCmvStep {                 //Custom steps
	cmvStepBeg,
	cmvStepPpBeg,
	cmvStepPpInitial,           //STEP_PAYPASS_INITIAL 0x00 ///< PayPass Step : Initial process management. It consists of the starting PayPass transaction step.
	cmvStepPpFinalSelection,    //STEP_PAYPASS_FINAL_SELECTION 0x01 ///< PayPass Step : Performs the final card application selection process. The PayPass reader performs certain checks on the data received in reply to the SELECT AID command as described in <i>PayPass 2.0 specification Section 4.3.1</i>.<br>The Final SELECT card response and status word are provided by the custom application (application selection result performed by Entry Point).
	cmvStepPpInitiateApplicationProcessing, //STEP_PAYPASS_INITIATE_APPLI_PROCESSING 0x02 ///< PayPass Step : Performs the "Initiate Application Processing" step. The PayPass reader initiates the transaction by issuing the <i>GET PROCESSING OPTIONS</i> command as described in <i>PayPass 2.0 specification Section 4.3.2</i>.<br>The PayPass card returns the AIP and the AFL.<br>Then, the PayPass reader verifies if the "M/Chip profile is supported" bit in the AIP is set. If the bit is set, the PayPass reader continues by selecting the method of offline data authentication to be used. If the bit is not set, then it continues by reading from the PayPass card the PayPass M/Stripe application data.
	cmvStepPpMethodSelection,   //STEP_PAYPASS_MCHIP_ODA_METHOD_SELECTION 0x10 ///< PayPass Step (MCHIP) : The PayPass reader selects the offline data authentication method to be used in the transaction. As described in Section 4.3.6, it compares the functionality available on the card, as indicated in the AIP, with its own capabilities. The result of this process is a decision to perform CDA, SDA or not to perform any offline data authentication.
	cmvStepPpMcReadApplicationData, //STEP_PAYPASS_MCHIP_READ_APPLI_DATA 0x11 ///< PayPass Step (MCHIP) : The PayPass reader reads the necessary data using <i>READ RECORD</i> commands as specified in <i>PayPass 2.0 specification, Section 4.3.7</i>.
	cmvStepPpMcProcessingRestrictions,  //STEP_PAYPASS_MCHIP_PROCESSING_RESTRICTIONS 0x12 ///< PayPass Step (MCHIP) : The PayPass reader performs the Processing Restrictions function as specified in <i>PayPass 2.0 specification, Section 4.3.8</i>. This includes application version number checking, application usage control checking and application effective/expiry dates checking.
	cmvStepPpMcTerminalRiskManagement,  //STEP_PAYPASS_MCHIP_TERMINAL_RISK_MANAGEMENT 0x13 ///< PayPass Step (MCHIP) : The PayPass reader performs Terminal Risk Management as specified in <i>PayPass 2.0 specification, Section 4.3.9</i>.
	cmvStepPpMcCvmSelection,    //STEP_PAYPASS_MCHIP_CVM_SELECTION 0x14 ///< PayPass Step (MCHIP) : The PayPass reader selects a cardholder verification method as specified in <i>PayPass 2.0 specification, Section 4.3.10</i>. The result of this function is stored as the Transaction CVM (\ref TAG_PAYPASS_TRANSACTION_CVM).
	cmvStepPpMcTerminalActionAnalysis,  //STEP_PAYPASS_MCHIP_TERMINAL_ACTION_ANALYSIS 0x15 ///< PayPass Step (MCHIP) : The PayPass reader performs Terminal Action Analysis in order to decide whether the transaction should be approved offline, declined offline, or transmitted online. The PayPass reader makes this decision based on the content of the TVR, the Issuer Action Codes and Terminal Action Codes as specified in <i>PayPass 2.0 specification, Section 4.3.11</i>.
	cmvStepPpMcGenerateAc,      //STEP_PAYPASS_MCHIP_GENERATE_AC 0x16 ///< PayPass Step (MCHIP) : The PayPass reader issues a <i>GENERATE AC</i> command, as described in <i>PayPass 2.0 specification, Section 4.3.12</i>, requesting a TC, ARQC or an AAC based on the results of Terminal Action Analysis. The PayPass card performs its card risk management when it receives the <i>GENERATE AC</i> command, and may decide to complete the transaction online (ARQC), offline (TC) or decline the transaction (AAC).
	cmvStepPpMcRemoveCard,      //STEP_PAYPASS_MCHIP_REMOVE_CARD 0x17 ///< PayPass Step (MCHIP) : This step is just there to indicate card read is complete and cardholder can remove the card from the field. It would be used for GUI customisation only.
	cmvStepPpMcGetCertificate,  //STEP_PAYPASS_MCHIP_GET_CERTIFICATE 0x18 ///< PayPass Step (MCHIP) : This is a step where the kernel is looking for. Usually, the kernel calls the application to request the transaction certificate.<br><br>The application shall provide the following data to the kernel to perform ODA :<br>    - \ref TAG_EMV_INT_CAPK_MODULUS containing the CA public key modulus.<br>   - \ref TAG_EMV_INT_CAPK_EXPONENT containing the CA public key exponent.
	cmvStepPpMcOdaRouter,       //STEP_PAYPASS_MCHIP_ODA_ROUTER 0x19 ///< PayPass Step (MCHIP) : This choose the correct ODA step to execute (just reads a value and return the appropriate status code).
	cmvStepPpMcCdaProcessing,   //STEP_PAYPASS_MCHIP_CDA_PROCESSING 0x1A ///< PayPass Step (MCHIP) : The PayPass reader retrieves the ICC Public Key and verifies the SDAD generated by the PayPass card as specified in <i>PayPass 2.0 specification, Section 4.3.13</i>.
	cmvStepPpMcSdaProcessing,   //STEP_PAYPASS_MCHIP_SDA_PROCESSING 0x1B ///< PayPass Step (MCHIP) : The PayPass reader performs SDA as specified in <i>PayPass 2.0 specification, Section 4.3.14</i>. The PayPass reader sets the Transaction Outcome to "Approved".
	cmvStepPpMcExceptionFileGetData,    //STEP_PAYPASS_MCHIP_EXCEPTION_FILE_GET_DATA 0x1C ///< PayPass Step (MCHIP) : This step is part of \ref STEP_PAYPASS_MCHIP_TERMINAL_RISK_MANAGEMENT step. It prepares the necessary data to be check for exception file. The kernel does not manage the blacklist itself. This is the role of the custom application to manage it.<br><br>This step is only necessary to provide the following data to custom to check if PAN is present in the black list :<br>    - \ref TAG_EMV_APPLI_PAN to indicate the card PAN.<br>  - \ref TAG_EMV_APPLI_PAN_SEQUENCE_NUMBER to indicate the PAN Sequence number (if present).<br><br>As output, custom provide the \ref TAG_KERNEL_PAN_IN_BLACK_LIST tag to indicate if PAN has been found in the exception file or not.
	cmvStepPpMcExceptionFileChecking,   //STEP_PAYPASS_MCHIP_EXCEPTION_FILE_CHECKING 0x1D ///< PayPass Step (MCHIP) : Based on the data returned by the custom on the \ref STEP_PAYPASS_MCHIP_EXCEPTION_FILE_GET_DATA step, it sets the TVR if the PAN has been found in the exception file.
	cmvStepPpMcBalReadingPreGenAC,   //STEP_PAYPASS_MCHIP_BALANCE_READING_PRE_GENAC 0x1E ///<PayPass Step (MCHIP) : Read the card balance from the card (if supported by the card and required by the reader) before the Generate AC command.
	cmvStepPpMcBalReadingPostGenAC,   //STEP_PAYPASS_MCHIP_BALANCE_READING_POST_GENAC 0x1F ///<PayPass Step (MCHIP) : Read the card balance from the card (if supported by the card and required by the reader) after the Generate AC command.
	cmvStepPpMcGenACRecACAnalysis,   //STEP_PAYPASS_MCHIP_GENAC_OR_RECAC_ANALYSIS 0x20 ///<PayPass Step (MCHIP) : This step parses and analyses the card response to a Generate AC or Recover AC command.
	cmvStepPpMcIsTornRecord,   //STEP_PAYPASS_MCHIP_IS_TORN_RECORD 0x21 ///<PayPass Step (MCHIP) : This step parses and analyses the card response to a Generate AC or Recover AC command.
	cmvStepPPMcIsTornRecordChecking,   //STEP_PAYPASS_MCHIP_IS_TORN_RECORD_CHECKING 0x22 ///<PayPass Step (MCHIP) : In the case of a torn transaction, the reader issues a <i>RECOVER AC</i> command to the card. Otherwise, the kernel simply continues with normal transaction processing (<i>GENERATE AC</i> command).
	cmvStepPpMcAddTornRecord,   //STEP_PAYPASS_MCHIP_ADD_TORN_RECORD 0x23 ///<PayPass Step (MCHIP) : This step prepares the necessary data in a record to be added in the torn transaction log. The kernel does not manage the torn transaction log itself. This is the role of the custom application to add the new torn record.<br><br>If adding this new record to the log means that an old record is removed (maximum number of torn transaction log records exceeded), then the old record is provided by the custom as output (in the output structure).
	cmvStepPpMcRemoveTornRecord,   //STEP_PAYPASS_MCHIP_REMOVE_TORN_RECORD 0x24 ///<PayPass Step (MCHIP) : This step is called to indicate that the transaction is completed and the torn record must be removed from the torn transaction log. It would be used for process customisation only: this is the role of the custom application to remove the used torn record.
	cmvStepPpMcSendPhoneMsg,   //STEP_PAYPASS_MCHIP_SEND_PHONE_MSG 0x25 ///<PayPass Step (MCHIP) : This step is called to indicate to the custom application that a phone specific message must be displayed.
	cmvStepPpMcSendPhoneMsgPostProc,   //STEP_PAYPASS_MCHIP_SEND_PHONE_MSG_POST_PROCESSING 0x26 ///<PayPass Step (MCHIP) : This step is called to update UIRD before returning the <i>PayPass_DoTransaction()</i> function.
	cmvStepPpMcPreReadAppData,   //STEP_PAYPASS_MCHIP_PRE_READ_APPLI_DATA 0x27 ///<PayPass Step (MCHIP) : Process the processing before the read application data (GET DATA to be sent before the RAD sequence).
	cmvStepPpMcPutDataBeforeGenAC,   //STEP_PAYPASS_MCHIP_PUT_DATA_BEFORE_GENAC 0x28 ///<PayPass Step (MCHIP) : Send the PUT DATA prior to GenAC.
	cmvStepPpMcPutDataAfterGenAC,   //STEP_PAYPASS_MCHIP_PUT_DATA_AFTER_GENAC 0x29 ///<PayPass Step (MCHIP) : Send the PUT DATA after GenAC.
	cmvStepPpMcCancelDEProc,   //STEP_PAYPASS_CANCEL_DE_PROCESSING 0x2A ///<PayPass Step : The PayPass reader indicates to the payment application that the Data Exchange task needs to be cancelled.
	cmvStepPpMcCancelDECheck,   //STEP_PAYPASS_CANCEL_DE_CHECK 0x2B ///<PayPass Step : The PayPass reader needs to check that the payment application has returned to the Data Exchange task. If the payment application does not respond to the DE task then the terminal will need to perform a soft reset after a timeout. The kernel cannot kill the DE task as the payment application is running in the context of the DE task and could have pending operations.
	cmvStepPpMsReadApplicationData, //STEP_PAYPASS_MSTRIPE_READ_APPLI_DATA 0x30 ///< PayPass Step (MSTRIPE) : Based on the AFL previously received from the card, the PayPass reader reads the necessary data using the READ RECORD command as specified in PayPass 2.0 specification, Section 4.3.3.
	cmvStepPpMsAvnChecking,     //STEP_PAYPASS_MSTRIPE_APPLI_VERSION_NB_CHECKING 0x31 ///< PayPass Step (MSTRIPE) : The PayPass reader verifies the compatibility of its application with the PayPass M/Stripe application in the PayPass card as specified in PayPass 2.0 specification, Section 4.3.4.
	cmvStepPpMsComputeCryptoChecksum,   //STEP_PAYPASS_MSTRIPE_COMPUTE_CRYPTO_CHECKSUM 0x32 ///< PayPass Step (MSTRIPE) : The PayPass reader continues with the COMPUTE CRYPTOGRAPHIC CHECKSUM command as specified in PayPass 2.0 specification, Section 4.3.5. The PayPass reader then sets the Transaction Outcome to "Online Request".
	cmvStepPpMsRemoveCard,      //STEP_PAYPASS_MSTRIPE_REMOVE_CARD 0x33 ///< PayPass Step (MSTRIPE) : This step is just there to indicate card read is complete and cardholder can remove the card from the field. It would be used for GUI customisation for example.
	cmvStepPpMsCvmSelection,   //STEP_PAYPASS_MSTRIPE_CVM_SELECTION 0x34 ///<PayPass Step (MSTRIPE) : The PayPass reader checks if the CVM Required Limit has been exceeded. If it is, it sets the "Offline PIN Required" bit in the \ref TAG_PAYPASS_MOBILE_SUPPORT_INDICATOR.
	cmvStepPpMsCccPostProc,   //STEP_PAYPASS_MSTRIPE_CCC_POST_PROCESSING 0x35 ///<PayPass Step (MSTRIPE) : The PayPass reader process additional MStripe checkings, to be done after the card removal indication.
	cmvStepPpMsSendPhoneMsg,   //STEP_PAYPASS_MSTRIPE_SEND_PHONE_MSG 0x36 ///<PayPass Step (MSTRIPE) : This step is called to indicate to the custom application that a phone specific message must be displayed.
	cmvStepPpMsSendPhoneMsgPostProc,   //STEP_PAYPASS_MSTRIPE_SEND_PHONE_MSG_POST_PROCESSING 0x37 ///<PayPass Step (MSTRIPE) : This step is called to update UIRD before returning the PayPass_DoTransaction() function.
	cmvStepPpMsPreReadAppliData,   //STEP_PAYPASS_MSTRIPE_PRE_READ_APPLI_DATA 0x38 ///<PayPass Step (MSTRIPE) : Process the processing before the read application data (specific DS processing).
	cmvStepPpCompletion,        //STEP_PAYPASS_COMPLETION 0x40 ///< PayPass Step : The PayPass reader executes the Completion function as specified in <i>PayPass 2.0 specification, Section 4.3.15</i>. The exchange structure is filled with the transaction tags, as specified by the <i>PayPass 2.0 specification, section 4.3.15</i>. Please refer to \ref PayPass_DoTransaction() function for more information about the returned data.
	cmvStepPpStop,              //STEP_PAYPASS_STOP 0x50 ///< PayPass Step : Last step of the transaction flow. It indicates transaction results shall be provided to the custom application.
	cmvStepPpEnd,
	cmvStepVwBeg,
	cmvStepVwInitial,           //STEP_VISAWAVE_INITIAL 0x00 ///< Initial process management. It consists of the starting VisaWave transaction step
	cmvStepVwFinalSelection,    //STEP_VISAWAVE_FINAL_SELECTION   0x01 /// Performs the final card application selection process
	cmvStepVwInitiateApplicationProcessing, //STEP_VISAWAVE_INITIATE_APPLI_PROCESSING 0x02 /// Performs the "Initiate Application Processing" step. The VisaWave reader initiates the transaction by issuing the GET PROCESSING OPTIONS
	cmvStepVwReadApplicationData,   //STEP_VISAWAVE_READ_APPLI_DATA 0x03 /// The VisaWave reader reads the necessary data using READ RECORD commands
	cmvStepVwCvmSelection,      //STEP_VISAWAVE_CVM_SELECTION 0x04 /// The VisaWave reader selects a cardholder verification method. The result of this function is stored as the Transaction CVM
	cmvStepVwFlowRouter,        //STEP_VISAWAVE_FLOW_ROUTER 0x05 /// This is a step where the kernel determines if it is an offline or online (according to the presence of the SFI 11 in the AFL).
	cmvStepVwGenerateAc,        //STEP_VISAWAVE_GENERATE_AC 0x06 /// The VisaWave reader issues a GENERATE AC command requesting an ARQC
	cmvStepVwGetCertificate,    //STEP_VISAWAVE_GET_CERTIFICATE 0x07 /// This is a step where the kernel is looking for the certificate for offline data authentication. Usually, the kernel calls the application to request the transaction certificate
	cmvStepVwDDAProcessing,     //STEP_VISAWAVE_DDA_PROCESSING 0x08 /// The VisaWave reader retrieves the ICC Public Key and verifies the SDAD generated by the VisaWave card.
	cmvStepVwRemoveCard,        //STEP_VISAWAVE_REMOVE_CARD 0x09 /// This step is just there to indicate card read is complete and cardholder can remove the card from the field. It would be used for GUI customisation only.
	cmvStepVwCompletion,        //STEP_VISAWAVE_COMPLETION 0x10 /// The VisaWave reader executes the Completion function. The exchange structure is filled with the transaction tags
	cmvStepVwStop,              //STEP_VISAWAVE_STOP 0x20 /// Last step of the transaction flow. It indicates transaction results shall be provided to the custom application.
	cmvStepVwEnd,
	cmvStepVpBeg,
	cmvStepVpInitial,           //STEP_PAYWAVE_INITIAL 0x00 ///initial process management.
	cmvStepVpFinalApplicationSelection, //STEP_PAYWAVE_FINAL_APPLICATION_SELECTION 0x01 ///Final card selection.
	cmvStepVpApplicationBlocked,    //STEP_PAYWAVE_APPLICATION_BLOCKED 0x02 /// Step if application is blocked
	cmvStepVpCardBlocked,       //STEP_PAYWAVE_CARD_BLOCKED 0x03 /// Step if card is blocked.
	cmvStepVpSelectAnotherApplication,  //STEP_PAYWAVE_SELECT_ANOTHER_APPLICATION 0x04 /// Step if an another application must be selected.
	cmvStepVpInitiateApplicationProcessing, //STEP_PAYWAVE_INITIATE_APPLICATION_PROCESSING 0x05 ///Initiate Application processing.
	cmvStepVpReadApplicationData,   //STEP_PAYWAVE_MSD_READ_APPLICATION_DATA 0x06 /// Read card data.
	cmvStepVpMandatoryDataChecking, //STEP_PAYWAVE_MSD_MANDATORY_DATA_CHECKING 0x07 /// Card MSD Mandatory Data Checking.
	cmvStepVpMsdCompletion,     //STEP_PAYWAVE_MSD_COMPLETION 0x08 /// Card MSD Completion.
	cmvStepVpMsdRemoveCard,     //STEP_PAYWAVE_MSD_REMOVE_CARD 0x09 /// Card MSD Remove Card.
	cmvStepVpVsdcReadApplicationData,   //STEP_PAYWAVE_QVSDC_READ_APPLICATION_DATA 0x0A /// Card QVSDC Read Application data.
	cmvStepVpVsdcRemoveCard,    //STEP_PAYWAVE_QVSDC_REMOVE_CARD 0x0B /// Card QVSDC Remove Card.
	cmvStepVpVsdcDataChecking,  //STEP_PAYWAVE_QVSDC_MANDATORY_DATA_CHECKING 0x0C /// Card QVSDC Check Mandatory Data.
	cmvStepVpVsdcBlackListControl,  //STEP_PAYWAVE_QVSDC_BLACK_LIST_CONTROL 0x0D /// Card QVSDC Black List control.
	cmvStepVpVsdcPostProcessing,    //STEP_PAYWAVE_QVSDC_POST_PROCESSING 0x0E /// Card QVSDC Post Processing.
	cmvStepVpVsdcGetCertificate,    //STEP_PAYWAVE_QVSDC_GET_CERTIFICATE 0x0F /// Card QVSDC Get Certificate.
	cmvStepVpVsdcOdaProcessing, //STEP_PAYWAVE_QVSDC_ODA_PROCESSING 0x10 /// Card QVSDC ODA Processing.
	cmvStepVpVsdcFddaProcessing,    //STEP_PAYWAVE_QVSDC_FDDA_PROCESSING 0x11 /// Card QVSDC DDA Processing.
	cmvStepVpVsdcSdaProcessing, //STEP_PAYWAVE_QVSDC_SDA_PROCESSING 0x12 /// Card QVSDC SDA Processing.
	cmvStepVpVsdcCompletion,    //STEP_PAYWAVE_QVSDC_COMPLETION 0x13 /// Card QVSDC Completion.
	cmvStepVpVsdcCompletionWithError,   //STEP_PAYWAVE_QVSDC_COMPLETION_WITH_ERROR 0x14 /// Card Completion when an error occured.
	cmvStepVpVsdcPaywaveStop,   //STEP_PAYWAVE_STOP 0x1F /// Last transaction step used for release memory, POFF, traces ...
	cmvStepVpEnd,
	cmvStepEnd
};

enum cmvRes {                   //Transaction Results
	cmvCardBlocked = 0x010D,    //KERNEL_STATUS_CARD_BLOCKED
	cmvAppBlocked = 0x010E,     //KERNEL_STATUS_APPLICATION_BLOCKED
	cmvOfflineApproved = 0x0112,    //KERNEL_STATUS_OFFLINE_APPROVED
	cmvOfflineDeclined = 0x0113,    //KERNEL_STATUS_OFFLINE_DECLINED
	cmvOnlineAuth = 0x0114,     //KERNEL_STATUS_ONLINE_AUTHORISATION
	cmvCancelled = 0x0115,      //KERNEL_STATUS_CANCELLED
	cmvUseContactInterface = 0x0116 //KERNEL_STATUS_USE_CONTACT_INTERFACE
};

enum cmvSta { //CMV Status
	cmvStaOK = 0x0A00, //If card detection is correctly performed.
	cmvStaKO = 0x0A01, //if an error occurred when filling \a pSharedData with the detection results.
	cmvStaMissingParam = 0x0A02, //mandatory parameter is missing
	cmvStaNoMatchingAid=0x0A03, //no AID in the candidate list
	cmvStaCardBlocked=0x0A04, //processing is terminated because the card is blocked
	cmvStaAppBlocked=0x0A05, //processing is terminated because the card is blocked
	cmvStaLackOfMemory = 0x0A06, //not enough memory to complete the processing
	cmvStaInvalidParam = 0x0A07, //one of the provided parameters is incorrect
	cmvStaComsError = 0x0A09, //communication error with the card (card removed)
	cmvStaMissingIccData = 0x0A0B, //a mandatory card data is missing
	cmvStaIccFormatError = 0x0A0D, //a data returned by the card is not correctly formatted
	cmvStaIccRedundantData = 0x0A0E, //a data returned by the card is redundant
	cmvStaInternalError = 0x0A11, //internal error occured
	cmvStaBadPpseSW = 0x0A12, //if the status word returned by the card on PPSE is not expected
	cmvStaEpNotLoaded = 0x0A17, //Entry Point component is not loaded
	cmvStaEpSelNotAllowed = 0x0A26, //Entry point method is not allowed as no AID is available
	cmvStaNotAllowed = 0x0AFD, //No detection has been launched (mode = NONE)
	cmvStaUnknown = 0x0AFF, // If card detection is not performed.
	cmvStaCancelled = 0x0A18, //the card detection has been cancelled.
	cmvStaUseAnotherInterface = 0x0A20, //The custom application required to use another interface (customisation)
	cmvStaStop = 0x0A21, //if customisation returned an unknown status or decided to stop the detection.
	cmvStaBadCompInit = 0x0AFE, //Entry Point cannot be used because of a bad initialisation
};

enum cmvCvm {                   //CardHolder Verification Methods
	cmvNoCvm = 0x01,            ///<No CVM To be Perfomed
	cmvSignature = 0x02,        ///<Signature is required
	cmvOnlinePin = 0x04         ///<Online Pin Entry should be performed
};

enum cmvAidOpt {                //Bits in the tag tagCmvAidOpt=9F928100
	cmvAidOptBeg,

	cmvAidOptRFU18,
	cmvAidOptRFU17,
	cmvAidOptRFU16,
	cmvAidOptRFU15,
	cmvAidOptRFU14,
	cmvAidOptZeroAmountAllowed,
	cmvAidOptStatusCheckSupported,
	cmvAidOptPartialAidSupported,

	cmvAidOptRFU28,
	cmvAidOptRFU27,
	cmvAidOptRFU26,
	cmvAidOptRFU25,
	cmvAidOptRFU24,
	cmvAidOptRFU23,
	cmvAidOptListOfAidAllowed,
	cmvAidOptEntryPointAllowed,

	//... Bytes 3 and 4 are RFU

	cmvAidOptAnd
};

enum cmvTrmTraQua {             //Bits in the tag tagTrmTraCpb =9F66
	cmvTrmTraQuaBeg,

	cmvClessMSDSupported,
	cmvClessVSDCSupported,
	cmvClessqVSDCSupported,
	cmvContactVSDCSupported,
	cmvReaderIsOfflineOnly,
	cmvOnlinePinSupported,
	cmvSignatureSupported,
	cmvTrmTraQuaRFU1,

	cmvOnlineCryptRequired,
	cmvCvmRequired,
	cmvTrmTraQuaRFU14,
	cmvTrmTraQuaRFU13,
	cmvTrmTraQuaRFU12,
	cmvTrmTraQuaRFU11,
	cmvTrmTraQuaRFU10,
	cmvTrmTraQuaRFU9,

	cmvTwoTapSupported,
	cmvPreTapSupported,
	cmvConsumerDeviceCVMSupported,
	cmvTrmTraQuaRFU21,
	cmvTrmTraQuaRFU20,
	cmvTrmTraQuaRFU19,
	cmvTrmTraQuaRFU18,
	cmvTrmTraQuaRFU17,

	//... Byte 4 is RFU

	cmvTrmTraQuaEnd
};

enum cmvAIDT {                  //Bits in the tag tagAIDT = 0x9F06
	cmvAIDTBeg,

	cmvClessFloorLimitExceeded,
	cmvClessTransactionLimitExceeded,
	cmvClessCvmLimitExceeded,
	cmvStatusCheckRequested,
	cmvZeroAmount,
	cmvContactlessApplicationNotAllowed,
	cmvAIDTRFU2,
	cmvAIDTRFU1,

	//... Bytes 2, 3 and 4 are RFU

	cmvAIDTEnd
};

enum eSta {
	staComplete = 0x0000,
	staAccepted = 0x0001,
	staDeclined = 0x0002,
	staAuthRequest = 0x0003,
	staReferralReq = 0x0004,
	staVisaEasyEntryCard = 0x0005,
	staCdaFailed = 0x0006,
	staSuspendEngine = 0x0007,
	staEnd
};

#endif                          //end __cmv__

#ifdef __ENG__

enum engCustSteps {
	engCustStepsBeg,
	engGetGlobalParam,
	engGetAIDList,
	engGetAIDData,
	engGetAIDParam,
	engProcessStep,
	engDCStart,                 //EMVDC_START
	engAppSelect,               //EMVDC_APPLICATION_SELECTION - Build Candidate List.
	engInitAppProcessing,       //EMVDC_INITIATE_APPLICATION_PROCESSING - Initiate Transaction (Get Processing Option command).
	engReadAppliData,           //EMVDC_READ_APPLICATION_DATA - Read Application Data.
	engOffDataAuth,             //EMVDC_OFFLINE_DATA_AUTHENTICATION - Offline Data Authentication (SDA or DDA).
	engProcRestrictions,        //EMVDC_PROCESSING_RESTRICTIONS - Processing Restrictions.
	engCardholderVerification,  //EMVDC_CARDHOLDER_VERIFICATION - Obsolete !! DO NOT USE.
	engTrmRiskManagement,       //EMVDC_TERMINAL_RISK_MANAGEMENT - Terminal Risk Management.
	engTrmActAnalysis,          //EMVDC_TERMINAL_ACTION_ANALYSIS - Terminal Action Analysis.
	engCrdActAnalysis,          //EMVDC_CARD_ACTION_ANALYSIS - Card Action Analysis (First Generate_AC).
	engOnlProcessing,           //EMVDC_ON_LINE_PROCESSING, //!< Online Processing.
	engIssScriptProcessing1,    //EMVDC_ISSUER_TO_CARD_SCRIPT_PROCESSING1 - Issuer Script Processing phase 1 (scripts 71).
	engCompletion,              //EMVDC_COMPLETION - Transaction Completion.
	engIssScriptProcessing2,    //EMVDC_ISSUER_TO_CARD_SCRIPT_PROCESSING2 - Issuer Script Processing phase 2 (scripts 72).
	engDCStop,                  //EMVDC_STOP - Power off the ICC.
	engCVMFirst,                //EMVDC_CARDHOLDER_VERIFICATION_FIRST - Beginning of CVM step.
	engCVMOther,                //EMVDC_CARDHOLDER_VERIFICATION_OTHER -CVM Step Continuation.
	engExtODABeg,               //EMVDC_ODA_EXTERN_INIT - Beginning of External ODA. Do not use if not explicitly needed.
	engExtDDA,                  //EMVDC_ODA_EXTERN_INTERNAL_AUTHENTICATE - Extern DDA. Do not use if not explicitly needed.
	engExtODAEnd,               //EMVDC_ODA_EXTERN_RESULT - End of  of External ODA. Do not use if not explicitly needed.
	engFinalSelect,             //EMVDC_FINAL_SELECTION - Final Application Selection Step.
	engInitTraLogProcessing,    //EMVDC_INITIATE_TRLOG_PROCESSING - Read Log Entry for Transaction Log.
	engReadTraLogData,          //EMVDC_READ_TRLOG_DATA - Read Transaction Log.
	engDisplayMessage,
	engGetLastTrans,
	engVoiceReferral,
	engAuthorization,
	engChooseLang,
	engMenuSelect,
	engChooseActTypeMenu,
	engGetAIDIcs,
	engCustStepsEnd
};

enum engErrors {
	engProcessCompleted = 0x0000,   //!< Service Completed Successfully - Nothing Special.
	engTansactionAccepted = 0x0001, //!< Service Completed Successfully - Transaction Accepted.
	engTransactionDeclined = 0x0002,    //!< Service Completed Successfully - Transaction Declined.
	engAuthorisationRequest = 0x0003,   //!< Service Completed Successfully - Online Authorisation Requested.
	engReferalRequest = 0x0004, //!< Service Completed Successfully - Referral Requested.
	engVisaEasyEntryCard = 0x0005,  //!< Service Completed Successfully - Special Case of Visa Easy Entry Cards.
	engCDAFailed = 0x0006,      //!< Error during CDA.
	engSuspendEngine = 0x0007,  //!< Suspend transaction.
	engTransactionCancelled = 0x0010,   //!< Transaction terminated - Transaction canceled.
	engProcessingError = 0x0011,    //!< Transaction terminated - ICC Protocol error.
	engCardError = 0x0012,      //!< Transaction terminated - Unexpected Response Code from ICC.
	engNotAccepted = 0x0013,    //!< Transaction terminated - Bad Tag given by the card.
	engICCDataFormatError = 0x0014, //!< Transaction terminated - Format error in a tag value.
	engMissingMandatoryICCData = 0x0015,    //!< Transaction terminated - Mandatory Data missing in ICC.
	engICCRedundantData = 0x0016,   //!< Transaction terminated - Redundant Data in ICC.
	engCardBlock = 0x0017,      //!< Transaction terminated - Card Blocked.
	engCondOfUseNotSatisfied = 0x0018,  //!< Transaction terminated - ICC replies Condition of Use not Satisfied to GPO command.
	engMissingMandatoryTermData = 0x0019,   //!< Transaction terminated - Terminal Parameters Missing.
	engInternalError = 0x001A,  //!< Transaction terminated - Internal Error during Step Processing.
	engRemoveCard = 0x001B,     //!< Transaction terminated - Card Removed.
	engServiceNotAllowed = 0x001C,  //!< Transaction terminated - Service Not Allowed.
	engNotEnoughMemory = 0x0101,    //!< Execution Error - Missing Memory.
	engWrongDELInputParameter = 0x0102, //!< Execution Error - Bad entry Data.
	engServiceNotAvailable = 0x0103,    //!< The AID parameter selection failed.
	engPanNotFound = 0x0104,    //!< The primary account number was not found in the database.
	engNewParameters = 0x0105,  //!< New manager parameters are present in the terminal.
	engReferrralDeclined = 0x0106,  //!< The referral is declined.
	engReferralOnline = 0x0107, //!< An online referral is requested.
	engReferralNotAccepted = 0x0108 //!< The referral is not accepted.
};

#endif                          //en  __eng__

#endif                          //end__emv__


#endif
