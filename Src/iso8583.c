#include <globals.h>
#include "iso8583.h"
#include "GTL_Assert.h"

#define LLBCD (-2)
#define LLLBCD (-3)
#define LLASC (-4)
#define LLLASC (-5)

static int fmt[isoBitEnd - isoBitBeg - 1] = {
		1,                          //001 Bit Map, Secondary
		LLBCD,                      //002 Primary Account Number (PAN)
		6,                          //003 Processing Code
		12,                         //004 Amount, Transaction
		12,                         //-005 not used
		12,                         //-006 not used
		10,                         //-007 Transmission Date and Time
		8,                          //-008 TMS max buffer rcv size
		8,                          //-009 not used
		8,                          //-010 not used
		6,                          //011 System Trace Audit Number
		6,                          //012 Time, Local Transaction
		4,                          //013 Date, Local Transaction
		4,                          //014 Date, Expiration
		6,                          //015 Date, Settlement
		4,                          //-016 not used
		4,                          //-017 not used
		4,                          //-018 not used
		3,                          //-019 not used
		3,                          //-020 not used
		3,                          //-021 not used
		3,                          //022 Point-of-Service Entry Mode
		3,                          //-023 not used
		3,                          //024 Network International Identifier
		2,                          //025 POS Condition Code
		4,                          //-026 Card Acceptor Business Code
		1,                          //-027 not used
		6,                          //-028 Reconciliation Date
		3,                          //-029 not used
		24,                         //-030 Original Amount
		LLBCD,                      //-031 not used
		LLBCD,                      //-032 Acquirer Institution Identification Code
		LLBCD,                      //-033 not used
		LLBCD,                      //-034 not used
		LLBCD,                      //035 Track 2 Data
		LLLBCD,                     //-036 not used
		12,                         //037 Retrieval Reference Number
		6,                          //038 Authorization Identification Response
		2,                          //039 Response Code
		3,                          //-040 not used
		8,                          //041 Card Acceptor Terminal Identification
		15,                         //042 Card Acceptor Identification Code
		40,                         //-043 not used in SAMA
		LLBCD,                      //-044 not used in SAMA
		LLBCD,                      //-045 not used in SAMA
		LLLBCD,                     //-046 not used in SAMA
		LLLBCD,                     //-047 Private Card Scheme Sponser ID
		LLLBCD,                     //048 Pin/MAC session keys
		3,                          //049 Currency Code, Transaction
		3,                          //-050 Currency Code, Settlement
		3,                          //-051 not used in SAMA
		8,                          //052 Personal Identification Number (PIN) Data
		48,                         //-053 Security Related Control Information
		LLLBCD,                     //054 Additional Amounts
		LLLBCD,                     //-055 ICC System related data
		LLLBCD,                     //-056 Original Data Elements
		3,                          //-057 not used
		LLBCD,                      //-058 not used
		LLLBCD,                     //-059 not used
		LLLBCD,                     //060 Private use, Batch Number
		LLLBCD,                     //061 Amount original transaction
		LLLBCD,                     //062 Reconciliation Totals
		LLLBCD,                     //063 Additional Data
		8,                          //064 Message Authentication Code (MAC)
		8,                     		//-065 Reserved for ISO use
		LLLBCD, 					//-066 Amounts, original fees
		2, 							//-067 Extended payment data
		3, 							//-068 Country code, receiving institution
		3, 							//-069 Country code, settlement institution
		3, 							//-070 Country code, authorizing agent Inst.
		8, 						    //-071 Message number
		LLLBCD, 					//-072 Data record
		6, 							//-073 Date, action
		10, 						//-074 Credits, number
		10, 						//-075 Credits, reversal number
		10, 						//-076 Debits, number
		10, 						//-077 Debits, reversal number
		10, 						//-078 Transfer, number
		10, 						//-079 Transfer, reversal number
		10, 						//-080 Inquiries, number
		10, 						//-081 Authorizations, number
		10, 						//-082 Inquiries, reversal number
		10, 						//-083 Payments, number
		10, 						//-084 Payments, reversal number
		10, 						//-085 Fee collections, number
		16, 						//-086 Credits, amount
		16, 						//-087 Credits, reversal amount
		16, 						//-088 Debits, amount
		16, 						//-089 Debits, reversal amount
		10, 						//-090 Authorizations, reversal number
		3, 							//-091 Country code, transaction Dest. Inst
		3,                          //-092
		LLBCD,                      //-093 not used
		LLBCD,                      //-094
		LLBCD, 						//-095 Card issuer reference data
		LLLBCD,                     //-096
		17, 						//-097 Amount, Net reconciliation
		25, 						//-098 Payee
		11,                         //099
		LLBCD, 						//-100 Receiving institution Id code
		LLBCD, 						//-101 File name
		LLBCD, 						//-102 Account identification 1
		LLBCD, 					//-103 Account identification 2
		LLLBCD,                     //-104 not used
		16,                         // 105
		16,                         // 106
		10,                         //-107 not used in SAMA
		10,                         //-108 not used in SAMA
		LLBCD,                      //-109 not used in SAMA
		LLBCD,                      //-110 not used in SAMA
		LLLBCD,                     //-111
		LLLBCD,                     // 112
		LLLBCD,                     // 113
		LLLBCD,                     //-114
		LLLBCD,                     //-115 not used in SAMA
		LLLBCD,                     // 116
		LLLBCD,                     //-117
		LLLBCD, 					//-118 Reserved for national use
		LLLBCD, 					//-119 Reserved for national use
		LLLBCD,                     //-120
		LLLBCD,                     //-121 not used
		LLLBCD,                     //-122 not used
		LLLBCD, 					//-123 Reserved for private use
		LLLBCD,                     // 124
		LLLBCD,                     // 125
		LLLBCD,                     // 126
		LLLBCD,                     // 127
		8                           // 128 Message Authentication Code (MAC)
};

int isoFmt(byte bit) {
	VERIFY(isoBitBeg < bit);
	VERIFY(bit < isoBitEnd);
	return fmt[bit - 1];
}
