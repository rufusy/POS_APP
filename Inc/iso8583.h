//Subversion reference
//$HeadURL: https://svn.ingenico.com/UTAPROJECTS/sea-partner/5W/trunk/ISO5WPH/log/iso.h $
//$Id: iso.h 1490 2009-02-05 09:39:05Z jelemia $

#ifndef __ISO8583_H__

/** \addtogroup loggroup
 * @{
 */

/** \addtogroup isogroup ISO ISO8583
 * ISO component describes the format of each field in the ISO8583 protocol used.
 * @{
 */
enum eIso {                     //ISO8583 fields
    isoBitBeg,
    isoBitMapSec,               //-001 Bit Map, Secondary
    isoPan,                     //-002 Primary Account Number (PAN)
    isoPrcCod,                  //-003 Processing Code
    isoAmt,                     //-004 Amount, Transaction
    iso005,                     //-005 not used
    iso006,                     //-006 not used
    isoDatTim,                  //-007 Transmission Date and Time
    isoMaxBuf,                  //-008 maximum buffer to be received by application from TMS
    iso009,                     //-009 not used
    iso010,                     //-010 not used
    isoSTAN,                    //-011 System Trace Audit Number
    isoTim,                     //-012 Transaction Time
    isoDat,                     //-013 Transaction Date
    isoDatExp,                  //-014 Date, Expiration
    iso015,                     //-015 not used
    iso016,                     //-016 not used
    iso017,                     //-017 not used
    iso018,                     //-018 not used
    iso019,                     //-019 not used
    iso020,                     //-020 not used
    iso021,                     //-021 not used
    isoPosEntMod,               //-022 Point-of-Service Entry Mode
    isoCrdSeq,                  //-023 Card Seq Number
    isoNII,                     //-024 Network International Identifier
    isoPosCndCod,               //-025 POS Condition Code
    isoBusCod,                  //-026 Card Acceptor Business Code
    iso027,                     //-027 not used
    isoRcnDat,                  //-028 Reconciliation Date
    iso029,                     //-029 not used
    isoOrgAmt,                  //-030 Original Amount
    iso031,                     //-031 not used
    isoAcqIIC,                  //-032 Acquirer Institution Identification Code
    iso033,                     //-033 not used
    iso034,                     //-034 not used
    isoTrk2,                    //-035 Track 2 Data
    iso036,                     //-036 not used
    isoRrn,                     //-037 Retrieval Reference Number
    isoAutCod,                  //-038 Authorization Identification Response
    isoRspCod,                  //-039 Response Code
    iso040,                     //-040 not used
    isoTid,                     //-041 Card Acceptor Terminal Identification
    isoMid,                     //-042 Card Acceptor Identification Code
    iso043,                     //-043 not used in SAMA
    iso044,                     //-044 not used in SAMA
    iso045,                     //-045 not used in SAMA
    iso046,                     //-046 not used in SAMA
    isoSpnsrId,                 //-047 Private Card Scheme Sponser ID
    isoPinMacKey,               //-048 Pin/MAC session keys
    isoCur,                     //-049 Currency Code, Transaction
    isoCurStl,                  //-050 Currency Code, Settlement
    iso051,                     //-051 not used in SAMA
    isoPinDat,                  //-052 Personal Identification Number (PIN) Data
    isoSecCtl,                  //-053 Security Related Control Information
    isoAddAmt,                  //-054 Additional Amounts
    isoEmvPds,                  //-055 ICC System related data
    isoOrgnlDatElmnt,           //-056 Original Data Elements
    iso057,                     //-057 not used
    iso058,                     //-058 not used
    iso059,                     //-059 not used
    isoBatNum,                  //-060 Batch Number
    isoRsvPvt = isoBatNum,      //-060 Reserved Private (Can be used depending on the need)
    isoAmtOrg,                  //-061 Amount original transaction
    isoTot,                     //-062 Reconciliation Totals
    isoRoc = isoTot,            //-062 ROC-Trace No
    isoAddDat,                  //-063 Additional Data
    isoMac,                     //-064 Message Authentication Code (MAC)
	iso065,                     //-065 Reserved for ISO use
	iso066, 					//-066 Amounts, original fees
	iso067, 					//-067 Extended payment data
	iso068, 					//-068 Country code, receiving institution
	iso069, 					//-069 Country code, settlement institution
	iso070, 					//-070 Country code, authorizing agent Inst.
	iso071, 					//-071 Message number
	iso072, 					//-072 Data record
	iso073, 					//-073 Date, action
	iso074, 					//-074 Credits, number
	iso075, 					//-075 Credits, reversal number
	iso076, 					//-076 Debits, number
	iso077, 					//-077 Debits, reversal number
	iso078, 					//-078 Transfer, number
	iso079, 					//-079 Transfer, reversal number
	iso080, 					//-080 Inquiries, number
	iso081, 					//-081 Authorizations, number
	iso082, 					//-082 Inquiries, reversal number
	iso083, 					//-083 Payments, number
	iso084, 					//-084 Payments, reversal number
	iso085, 					//-085 Fee collections, number
	iso086, 					//-086 Credits, amount
	iso087, 					//-087 Credits, reversal amount
	iso088, 					//-088 Debits, amount
	iso089, 					//-089 Debits, reversal amount
	iso090, 					//-090 Authorizations, reversal number
	iso091, 					//-091 Country code, transaction Dest. Inst
	iso092, 					//-092 Country code, transaction Orig. Inst
	iso093, 					//-093 Transaction Dest. Inst. Id code
	iso094, 					//-094 Transaction Orig. Inst. Id code
	iso095, 					//-095 Card issuer reference data
	iso096, 					//-096 Key management data
	iso097, 					//-097 Amount, Net reconciliation
	iso098, 					//-098 Payee
	iso099, 					//-099 Settlement institution Id code
	iso100, 					//-100 Receiving institution Id code
	iso101, 					//-101 File name
	iso102, 					//-102 Account identification 1
	iso103, 					//-103 Account identification 2
	iso104, 					//-104 Transaction description
	iso105, 					//-105 Credits, Chargeback amount
	iso106, 					//-106 Debits, Chargeback amount
	iso107, 					//-107 Credits, Chargeback number
	iso108, 					//-108 Debits, Chargeback number
	iso109, 					//-109 Credits, Fee amounts
	iso110, 					//-110 Debits, Fee amounts
	iso111, 					//-111 Reserved for ISO use
	iso112, 					//-112 Reserved for ISO use
	iso113, 					//-113 Reserved for ISO use
	iso114, 					//-114 Reserved for ISO use
	iso115, 					//-115 Reserved for ISO use
	iso116, 					//-116 Reserved for national use
	iso117, 					//-117 Reserved for national use
	iso118, 					//-118 Reserved for national use
	iso119, 					//-119 Reserved for national use
	iso120, 					//-120 Reserved for national use
	iso121, 					//-121 Reserved for national use
	iso122, 					//-122 Reserved for national use
	iso123, 					//-123 Reserved for private use
	iso124, 					//-124 Reserved for private use
	iso125, 					//-125 Reserved for private use
	iso126, 					//-126 Reserved for private use
	iso127, 					//-127 Reserved for private use
	iso128, 					//-128 Message authentication code field
    isoBitEnd
};

int isoFmt(byte bit);

/** @} */
/** @} */

#define __ISO8583_H__

#endif

