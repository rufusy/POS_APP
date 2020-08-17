/**
 * \author	Copyright (c) 2009 Ingenico, rue claude Chappe,\n
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
#include "Globals.h"


/////////////////////////////////////////////////////////////////
//// Macros & preprocessor definitions //////////////////////////

#ifndef DEFAULT_EP_KERNEL_DISCOVER_DPAS
#define DEFAULT_EP_KERNEL_DISCOVER_DPAS		0x0006	/*!< PPSE Application Selection - Defines a Discover DPAS kernel (outside of the EMVCo range as this has not been yet defined by EMVCo). */
#endif

#ifndef DEFAULT_EP_KERNEL_PURE
#define DEFAULT_EP_KERNEL_PURE				0x0105	/*!< PPSE Application Selection - Defines a PURE kernel (outside of the EMVCo range as this has not been yet defined by EMVCo). */
#endif


#define MAX_FILE_LG						1024		/*!< max length buf for file. */
#define DISK_NAME_SIZE					33			/*!< Size of disk name. */


//! \brief TagAccess Tag reading access type
#define TAG_READING						1			/*!<  */
#define VALUE_READING					3			/*!<  */


//! \brief Action when a tag is allready found in destination TLVTree
#define C_PARAMS_NO_ACTION				0			/*!<  */
#define C_PARAMS_DELETE					1			/*!<  */


/////////////////////////////////////////////////////////////////
//// Global data definition /////////////////////////////////////

//! \brief 
const Struct_TagList CrcTagList[] = {
		{0x9F928210,			C_TAG_TYPE_BUF_BYTE,	NULL},
		{0x9F928212,			C_TAG_TYPE_BUF_BYTE,	NULL},
		{0x9F928214,			C_TAG_TYPE_BUF_BYTE,	NULL},
		{0x9F1A,				C_TAG_TYPE_BUF_BYTE,	NULL},
		{0x9F40,				C_TAG_TYPE_BUF_BYTE,	NULL},
		{0x9F35,				C_TAG_TYPE_BUF_BYTE,	NULL},
		{0x9F01,				C_TAG_TYPE_BUF_BYTE,	NULL},
		{0x9F15,				C_TAG_TYPE_BUF_BYTE,	NULL},
		{0x9F16,				C_TAG_TYPE_BUF_BYTE,	NULL},
		{0x9F1C,				C_TAG_TYPE_BUF_BYTE,	NULL},
};


//! \brief Default parameters (XML format)
const char gs_DefaultParam[] = {
		""
		"<?xml version=\"1.0\" standalone=\"yes\" ?>"
		"<!-- Fichier de parametres base kenels contacless -->"
		""
		"<tlvtree ver=\"1.0\">"
		""
		"	<node tag=\"0x0\">"
		"		<node tag=\"0x9F918801\"> 44 65 66 61 75 6C 74 20 50 61 72 61 6d 65 74 65 72 73 </node>	<!-- TAG_SAMPLE_PARAMETERS_LABEL : Parameters identifier label -->"
		""
		"		<node tag=\"0xBF918800\">	<!-- TAG_SAMPLE_AID_PARAMETERS -->"
		""
		"			<node tag=\"0x1000\">"
		"				<!-- AID : VISA: Visa Credit or Debit or Easy Entry -->"
		"				<node tag=\"0x9F06\"> A0 00 00 00 03 10 10 </node>					<!-- TAG_AID_TERMINAL -->"
		"				<!-- Application Program Identifier not specified : default parameters set -->"
		"				<node tag=\"0x9F928101\"> 00 03 </node>								<!-- TAG_EP_KERNEL_TO_USE : VISA -->"
		"				<node tag=\"0x9F928100\"> 05 01 00 00 </node>						<!-- TAG_EP_AID_OPTIONS : TAG_EP_AID_OPTIONS : Partial AID & Zero amount + EP allowed -->"
		"				<node tag=\"0x9F66\"> 36 00 C0 00 </node>							<!-- TAG_EP_TERMINAL_TRANSACTION_QUALIFIERS clss MSD + qVSDC & onlinePIN + sign supported + CVN17 + ISP-->"
		"				<node tag=\"0x9F92810D\"> 99 99 10 00 00 00 </node>				<!-- TAG_EP_CLESS_TRANSACTION_LIMIT : 100 euros -->"
		"				<node tag=\"0x9F92810F\"> 00 00 00 00 15 00 </node>				<!-- TAG_EP_CLESS_FLOOR_LIMIT : 15 euros -->"
		"				<node tag=\"0x9F92810E\"> 00 00 00 00 10 00 </node>				<!-- TAG_EP_CLESS_CVM_REQUIRED_LIMIT: 10 euros -->"
		"				<node tag=\"0x9F1B\"> 00 00 05 AA </node>							<!-- TAG_EMV_TERMINAL_FLOOR_LIMIT : 14,50 euros -->"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<!-- AID : VISA: Visa Electron -->"
		"				<node tag=\"0x9F06\"> A0 00 00 00 03 20 10 </node>				<!-- TAG_AID_TERMINAL -->"
		"				<!-- Application Program Identifier not specified : default parameters set -->"
		"				<node tag=\"0x9F928101\"> 00 03 </node>							<!-- TAG_EP_KERNEL_TO_USE : VISA -->"
		"				<node tag=\"0x9F928100\"> 05 01 00 00 </node>						<!-- TAG_EP_AID_OPTIONS : TAG_EP_AID_OPTIONS : Partial AID & Zero amount + EP allowed -->"
		"				<node tag=\"0x9F66\"> 36 00 C0 00 </node>							<!-- TAG_EP_TERMINAL_TRANSACTION_QUALIFIERS clss MSD + qVSDC & onlinePIN + sign supported + CVN17 + ISP -->"
		"				<node tag=\"0x9F92810D\"> 99 99 10 00 00 00 </node>				<!-- TAG_EP_CLESS_TRANSACTION_LIMIT : 100 euros -->"
		"				<node tag=\"0x9F92810F\"> 99 99 00 00 15 00 </node>				<!-- TAG_EP_CLESS_FLOOR_LIMIT : 15 euros -->"
		"				<node tag=\"0x9F92810E\"> 00 00 00 00 10 00 </node>				<!-- TAG_EP_CLESS_CVM_REQUIRED_LIMIT: 10 euros -->"
		"				<node tag=\"0x9F1B\"> 00 00 05 AA </node>							<!-- TAG_EMV_TERMINAL_FLOOR_LIMIT : 14,50 euros -->"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<!-- AID : UPI: UnionPay Debit -->"
		"				<node tag=\"0x9F06\"> A0 00 00 03 33 01 01 01  </node>				<!-- TAG_AID_TERMINAL -->"
		"				<!-- Application Program Identifier not specified : default parameters set -->"
		"				<node tag=\"0x9F928101\"> 00 07 </node>							<!-- TAG_EP_KERNEL_TO_USE : VISA -->"
		"				<node tag=\"0x9F928100\"> 45 03 00 00 </node>						<!-- TAG_EP_AID_OPTIONS : TAG_EP_AID_OPTIONS : Partial AID & Zero amount + EP allowed -->"
		"				<node tag=\"0x9F66\"> 36 80 40 80  </node>							<!-- TAG_EP_TERMINAL_TRANSACTION_QUALIFIERS clss MSD + qVSDC & onlinePIN + sign supported + CVN17 + ISP -->"
		"				<node tag=\"0x9F92810D\"> 99 99 99 99 99 99 </node>				<!-- TAG_EP_CLESS_TRANSACTION_LIMIT : 100 euros -->"
		"				<node tag=\"0x9F92810F\"> 00 00 00 00 00 00 </node>				<!-- TAG_EP_CLESS_FLOOR_LIMIT : 15 euros -->"
		"				<node tag=\"0x9F92810E\"> 00 00 00 03 00 01 </node>				<!-- TAG_EP_CLESS_CVM_REQUIRED_LIMIT: 10 euros -->"
		"				<node tag=\"0x9F1B\"> 00 00 00 00 </node>							<!-- TAG_EMV_TERMINAL_FLOOR_LIMIT : 14,50 euros -->"
		"				<node tag=\"0x9F918709\"> D8 40 00 A8 00 </node>						<!-- TAG_EMV_INT_TAC_DEFAULT : 00 00 00 00 00 (no matching bit) -->"
		"				<node tag=\"0x9F91870A\"> 00 00 00 00 00 </node>						<!-- TAG_EMV_INT_TAC_DENIAL : 00 00 00 00 00 (no matching bit) -->"
		"				<node tag=\"0x9F91870B\"> DC 40 04 F8 00 </node>						<!-- TAG_EMV_INT_TAC_ONLINE : 00 00 00 00 00 (no matching bit) -->"
		"				<node tag=\"0x9F09\"> 00 30  </node>									<!-- TAG_EMV_APPLI_VERSION_NUMBER_TERM -->"
		"				<node tag=\"0x9F91870C\"> 00 00 00 00 </node>		<!-- TAG_EMV_INT_THRESHOLD_VALUE_BIASED_RAND_SEL  -->"
		"				<node tag=\"0x9F918707\"> 99 </node>		        <!-- TAG_EMV_INT_MAX_TARGET_PERC_BIASED_RAND_SEL  -->"
		"				<node tag=\"0x9F918708\"> 99 </node>		        <!-- TAG_EMV_INT_TARGET_PERC_RAND_SEL  -->"
		"				<node tag=\"0x9F918708\"> 99 </node>		        <!-- TAG_EMV_INT_TARGET_PERC_RAND_SEL  -->"
		"				<node tag=\"0x9F918705\"> 9F 37 04 </node>		    <!-- TAG_EMV_INT_DEFAULT_DDOL  -->"
		"				<node tag=\"0x9F33\"> E0 68 40 </node>			    <!-- TAG_EMV_TERMINAL_CAPABILITIES -->"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<!-- AID : UPI: UnionPay Credit -->"
		"				<node tag=\"0x9F06\"> A0 00 00 03 33 01 01 02  </node>				<!-- TAG_AID_TERMINAL -->"
		"				<!-- Application Program Identifier not specified : default parameters set -->"
		"				<node tag=\"0x9F928101\"> 00 07 </node>							<!-- TAG_EP_KERNEL_TO_USE : VISA -->"
		"				<node tag=\"0x9F928100\"> 45 03 00 00 </node>					<!-- TAG_EP_AID_OPTIONS : TAG_EP_AID_OPTIONS : Partial AID & Zero amount + EP allowed -->"
		"				<node tag=\"0x9F66\"> 36 80 40 80  </node>						<!-- TAG_EP_TERMINAL_TRANSACTION_QUALIFIERS clss MSD + qVSDC & onlinePIN + sign supported + CVN17 + ISP -->"
		"				<node tag=\"0x9F92810D\"> 99 99 99 99 99 99 </node>				<!-- TAG_EP_CLESS_TRANSACTION_LIMIT : 100 euros -->"
		"				<node tag=\"0x9F92810F\"> 00 00 00 00 00 00 </node>				<!-- TAG_EP_CLESS_FLOOR_LIMIT : 15 euros -->"
		"				<node tag=\"0x9F92810E\"> 00 00 00 03 00 00 </node>				<!-- TAG_EP_CLESS_CVM_REQUIRED_LIMIT: 10 euros -->"
		"				<node tag=\"0x9F1B\"> 00 00 00 00 </node>						<!-- TAG_EMV_TERMINAL_FLOOR_LIMIT : 14,50 euros -->"
		"				<node tag=\"0x9F918709\"> D8 40 00 A8 00 </node>				<!-- TAG_EMV_INT_TAC_DEFAULT : 00 00 00 00 00 (no matching bit) -->"
		"				<node tag=\"0x9F91870A\"> 00 00 00 00 00 </node>				<!-- TAG_EMV_INT_TAC_DENIAL : 00 00 00 00 00 (no matching bit) -->"
		"				<node tag=\"0x9F91870B\"> DC 40 04 F8 00 </node>				<!-- TAG_EMV_INT_TAC_ONLINE : 00 00 00 00 00 (no matching bit) -->"
		"				<node tag=\"0x9F09\"> 00 30  </node>							<!-- TAG_EMV_APPLI_VERSION_NUMBER_TERM -->"
		"				<node tag=\"0x9F91870C\"> 00 00 00 00 </node>		<!-- TAG_EMV_INT_THRESHOLD_VALUE_BIASED_RAND_SEL  -->"
		"				<node tag=\"0x9F918707\"> 99 </node>		        <!-- TAG_EMV_INT_MAX_TARGET_PERC_BIASED_RAND_SEL  -->"
		"				<node tag=\"0x9F918708\"> 99 </node>		        <!-- TAG_EMV_INT_TARGET_PERC_RAND_SEL  -->"
		"				<node tag=\"0x9F918708\"> 99 </node>		        <!-- TAG_EMV_INT_TARGET_PERC_RAND_SEL  -->"
		"				<node tag=\"0x9F918705\"> 9F 37 04 </node>		    <!-- TAG_EMV_INT_DEFAULT_DDOL  -->"
		"				<node tag=\"0x9F928510\"> 00 </node>		        <!-- TAG_QUICKPASS_TERMINAL_ENTRY_CAPABILITY  -->"
		"				<node tag=\"0x9F33\"> E0 68 C8 </node>			    <!-- TAG_EMV_TERMINAL_CAPABILITIES -->"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<!-- AID : UPI: UnionPay Quasi Credit -->"
		"				<node tag=\"0x9F06\"> A0 00 00 03 33 01 01 03  </node>				<!-- TAG_AID_TERMINAL -->"
		"				<!-- Application Program Identifier not specified : default parameters set -->"
		"				<node tag=\"0x9F928101\"> 00 07 </node>							<!-- TAG_EP_KERNEL_TO_USE : VISA -->"
		"				<node tag=\"0x9F928100\"> 45 03 00 00 </node>						<!-- TAG_EP_AID_OPTIONS : TAG_EP_AID_OPTIONS : Partial AID & Zero amount + EP allowed -->"
		"				<node tag=\"0x9F66\"> 36 80 40 80  </node>							<!-- TAG_EP_TERMINAL_TRANSACTION_QUALIFIERS clss MSD + qVSDC & onlinePIN + sign supported + CVN17 + ISP -->"
		"				<node tag=\"0x9F92810D\"> 99 99 99 99 99 99 </node>				<!-- TAG_EP_CLESS_TRANSACTION_LIMIT : 100 euros -->"
		"				<node tag=\"0x9F92810F\"> 00 00 00 00 00 00 </node>				<!-- TAG_EP_CLESS_FLOOR_LIMIT : 15 euros -->"
		"				<node tag=\"0x9F92810E\"> 00 00 00 03 00 00 </node>				<!-- TAG_EP_CLESS_CVM_REQUIRED_LIMIT: 10 euros -->"
		"				<node tag=\"0x9F1B\"> 00 00 00 00 </node>							<!-- TAG_EMV_TERMINAL_FLOOR_LIMIT : 14,50 euros -->"
		"				<node tag=\"0x9F918709\"> D8 40 00 A8 00 </node>						<!-- TAG_EMV_INT_TAC_DEFAULT : 00 00 00 00 00 (no matching bit) -->"
		"				<node tag=\"0x9F91870A\"> 00 00 00 00 00 </node>						<!-- TAG_EMV_INT_TAC_DENIAL : 00 00 00 00 00 (no matching bit) -->"
		"				<node tag=\"0x9F91870B\"> DC 40 04 F8 00 </node>						<!-- TAG_EMV_INT_TAC_ONLINE : 00 00 00 00 00 (no matching bit) -->"
		"				<node tag=\"0x9F09\"> 00 30  </node>									<!-- TAG_EMV_APPLI_VERSION_NUMBER_TERM -->"
		"				<node tag=\"0x9F91870C\"> 00 00 00 00 </node>		<!-- TAG_EMV_INT_THRESHOLD_VALUE_BIASED_RAND_SEL  -->"
		"				<node tag=\"0x9F918707\"> 99 </node>		        <!-- TAG_EMV_INT_MAX_TARGET_PERC_BIASED_RAND_SEL  -->"
		"				<node tag=\"0x9F918708\"> 99 </node>		        <!-- TAG_EMV_INT_TARGET_PERC_RAND_SEL  -->"
		"				<node tag=\"0x9F918708\"> 99 </node>		        <!-- TAG_EMV_INT_TARGET_PERC_RAND_SEL  -->"
		"				<node tag=\"0x9F918705\"> 9F 37 04 </node>		    <!-- TAG_EMV_INT_DEFAULT_DDOL  -->"
		"				<node tag=\"0x9F928510\"> 00 </node>		        <!-- TAG_QUICKPASS_TERMINAL_ENTRY_CAPABILITY  -->"
		"				<node tag=\"0x9F33\"> E0 68 C8 </node>			    <!-- TAG_EMV_TERMINAL_CAPABILITIES -->"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<!-- AID : UPI: UnionPay Electronic Cash -->"
		"				<node tag=\"0x9F06\"> A0 00 00 03 33 01 01 06  </node>				<!-- TAG_AID_TERMINAL -->"
		"				<!-- Application Program Identifier not specified : default parameters set -->"
		"				<node tag=\"0x9F928101\"> 00 07 </node>							<!-- TAG_EP_KERNEL_TO_USE : VISA -->"
		"				<node tag=\"0x9F928100\"> 45 03 00 00 </node>						<!-- TAG_EP_AID_OPTIONS : TAG_EP_AID_OPTIONS : Partial AID & Zero amount + EP allowed -->"
		"				<node tag=\"0x9F66\"> 36 80 40 80  </node>							<!-- TAG_EP_TERMINAL_TRANSACTION_QUALIFIERS clss MSD + qVSDC & onlinePIN + sign supported + CVN17 + ISP -->"
		"				<node tag=\"0x9F92810D\"> 99 99 99 99 99 99 </node>				<!-- TAG_EP_CLESS_TRANSACTION_LIMIT : 100 euros -->"
		"				<node tag=\"0x9F92810F\"> 00 00 00 00 00 00 </node>				<!-- TAG_EP_CLESS_FLOOR_LIMIT : 15 euros -->"
		"				<node tag=\"0x9F92810E\"> 00 00 00 03 00 00 </node>				<!-- TAG_EP_CLESS_CVM_REQUIRED_LIMIT: 10 euros -->"
		"				<node tag=\"0x9F1B\"> 00 00 00 00 </node>							<!-- TAG_EMV_TERMINAL_FLOOR_LIMIT : 14,50 euros -->"
		"				<node tag=\"0x9F918709\"> D8 40 00 A8 00 </node>						<!-- TAG_EMV_INT_TAC_DEFAULT : 00 00 00 00 00 (no matching bit) -->"
		"				<node tag=\"0x9F91870A\"> 00 00 00 00 00 </node>						<!-- TAG_EMV_INT_TAC_DENIAL : 00 00 00 00 00 (no matching bit) -->"
		"				<node tag=\"0x9F91870B\"> DC 40 04 F8 00 </node>						<!-- TAG_EMV_INT_TAC_ONLINE : 00 00 00 00 00 (no matching bit) -->"
		"				<node tag=\"0x9F09\"> 00 30  </node>									<!-- TAG_EMV_APPLI_VERSION_NUMBER_TERM -->"
		"				<node tag=\"0x9F91870C\"> 00 00 00 00 </node>		<!-- TAG_EMV_INT_THRESHOLD_VALUE_BIASED_RAND_SEL  -->"
		"				<node tag=\"0x9F918707\"> 99 </node>		        <!-- TAG_EMV_INT_MAX_TARGET_PERC_BIASED_RAND_SEL  -->"
		"				<node tag=\"0x9F918708\"> 99 </node>		        <!-- TAG_EMV_INT_TARGET_PERC_RAND_SEL  -->"
		"				<node tag=\"0x9F918708\"> 99 </node>		        <!-- TAG_EMV_INT_TARGET_PERC_RAND_SEL  -->"
		"				<node tag=\"0x9F918705\"> 9F 37 04 </node>		    <!-- TAG_EMV_INT_DEFAULT_DDOL  -->"
		"				<node tag=\"0x9F33\"> E0 68 C8 </node>			    <!-- TAG_EMV_TERMINAL_CAPABILITIES -->"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<!-- AID : UPI: U.S. UnionPay Common Debit AID	  -->"
		"				<node tag=\"0x9F06\"> A0 00 00 03 33 01 01 08  </node>				<!-- TAG_AID_TERMINAL -->"
		"				<!-- Application Program Identifier not specified : default parameters set -->"
		"				<node tag=\"0x9F928101\"> 00 07 </node>							<!-- TAG_EP_KERNEL_TO_USE : VISA -->"
		"				<node tag=\"0x9F928100\"> 45 01 00 00 </node>						<!-- TAG_EP_AID_OPTIONS : TAG_EP_AID_OPTIONS : Partial AID & Zero amount + EP allowed -->"
		"				<node tag=\"0x9F66\"> 36 80 40 80  </node>							<!-- TAG_EP_TERMINAL_TRANSACTION_QUALIFIERS clss MSD + qVSDC & onlinePIN + sign supported + CVN17 + ISP -->"
		"				<node tag=\"0x9F92810D\"> 99 99 99 99 99 99 </node>				<!-- TAG_EP_CLESS_TRANSACTION_LIMIT : 100 euros -->"
		"				<node tag=\"0x9F92810F\"> 00 00 00 00 00 01 </node>				<!-- TAG_EP_CLESS_FLOOR_LIMIT : 15 euros -->"
		"				<node tag=\"0x9F92810E\"> 00 00 00 03 00 00 </node>				<!-- TAG_EP_CLESS_CVM_REQUIRED_LIMIT: 10 euros -->"
		"				<node tag=\"0x9F1B\"> 00 00 00 00 </node>							<!-- TAG_EMV_TERMINAL_FLOOR_LIMIT : 14,50 euros -->"
		"				<node tag=\"0x9F918709\"> D8 40 00 A8 00 </node>						<!-- TAG_EMV_INT_TAC_DEFAULT : 00 00 00 00 00 (no matching bit) -->"
		"				<node tag=\"0x9F91870A\"> 00 00 00 00 00 </node>						<!-- TAG_EMV_INT_TAC_DENIAL : 00 00 00 00 00 (no matching bit) -->"
		"				<node tag=\"0x9F91870B\"> DC 40 04 F8 00 </node>						<!-- TAG_EMV_INT_TAC_ONLINE : 00 00 00 00 00 (no matching bit) -->"
		"				<node tag=\"0x9F09\"> 00 30  </node>									<!-- TAG_EMV_APPLI_VERSION_NUMBER_TERM -->"
		"				<node tag=\"0x9F91870C\"> 00 00 00 00 </node>		<!-- TAG_EMV_INT_THRESHOLD_VALUE_BIASED_RAND_SEL  -->"
		"				<node tag=\"0x9F918707\"> 99 </node>		        <!-- TAG_EMV_INT_MAX_TARGET_PERC_BIASED_RAND_SEL  -->"
		"				<node tag=\"0x9F918708\"> 99 </node>		        <!-- TAG_EMV_INT_TARGET_PERC_RAND_SEL  -->"
		"				<node tag=\"0x9F918708\"> 99 </node>		        <!-- TAG_EMV_INT_TARGET_PERC_RAND_SEL  -->"
		"				<node tag=\"0x9F918705\"> 9F 37 04 </node>		    <!-- TAG_EMV_INT_DEFAULT_DDOL  -->"
		"				<node tag=\"0x9F33\"> E0 68 40 </node>			    <!-- TAG_EMV_TERMINAL_CAPABILITIES -->"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<!-- AID : VISA: V PAY -->"
		"				<node tag=\"0x9F06\"> A0 00 00 00 03 20 20 </node>					<!-- TAG_AID_TERMINAL -->"
		"				<!-- Application Program Identifier not specified : default parameters set -->"
		"				<node tag=\"0x9F928101\"> 00 03 </node>							<!-- TAG_EP_KERNEL_TO_USE : VISA -->"
		"				<node tag=\"0x9F928100\"> 05 03 00 00 </node>						<!-- TAG_EP_AID_OPTIONS : Partial AID & Zero amount -->"
		"				<node tag=\"0x9F92810D\"> 99 99 10 00 00 00 </node>				<!-- TAG_EP_CLESS_TRANSACTION_LIMIT : 100 euros -->"
		"				<node tag=\"0x9F92810F\"> 99 99 00 00 15 00 </node>				<!-- TAG_EP_CLESS_FLOOR_LIMIT -->"
		"				<node tag=\"0x9F92810E\"> 00 00 00 00 10 00 </node>				<!-- TAG_EP_CLESS_CVM_REQUIRED_LIMIT -->"
		"				<node tag=\"0x9F66\"> 36 00 00 00 </node>							<!-- TAG_EP_TERMINAL_TRANSACTION_QUALIFIERS -->"
		"				<node tag=\"0x9F1B\"> 00 00 05 AA </node>							<!-- TAG_EMV_TERMINAL_FLOOR_LIMIT -->"
		""
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<!-- AID : MasterCard PayPass AID for debit -->"
		"				<node tag=\"0x9F06\"> A0 00 00 00 04 10 10 </node>					<!-- TAG_AID_TERMINAL : MasterCard PayPass AID -->"
		"				<node tag=\"0x9F918701\"> 00 </node>									<!-- TAG_EMV_INT_TRANSACTION_TYPE -->"
		"				<node tag=\"0x9F928100\"> 45 01 00 00 </node>							<!-- TAG_EP_AID_OPTIONS -->"
		"				<node tag=\"0x9F928101\"> 00 02 </node>								<!-- TAG_EP_KERNEL_TO_USE -->"
		"				<node tag=\"0x9F09\"> 00 02  </node>									<!-- TAG_EMV_APPLI_VERSION_NUMBER_TERM -->"
		"				<node tag=\"0x9F918523\"> 00 </node>									<!-- TAG_PAYPASS_CARD_DATA_INPUT_CAPABILITY -->"
		"				<node tag=\"0x9F918525\"> 60 </node>									<!-- TAG_PAYPASS_MCHIP_CVM_CAPABILITY_CVM_REQUIRED -->"
		"				<node tag=\"0x9F918526\"> 28 </node>									<!-- TAG_PAYPASS_MCHIP_CVM_CAPABILITY_CVM_NOT_REQUIRED -->"
		"				<node tag=\"0x9F918502\"> 9F 6A 04 </node>							<!-- TAG_PAYPASS_DEFAULT_UDOL -->"
		"				<node tag=\"0x9F918522\"> 20 </node>									<!-- TAG_PAYPASS_KERNEL_CONFIGURATION -->"
		"				<node tag=\"0x9F6D\"> 00 01 </node>									<!-- TAG_PAYPASS_MSTRIPE_APPLI_VERSION_NUMBER_TERM -->"
		"				<node tag=\"0x9F918527\"> 10 </node>									<!-- TAG_PAYPASS_MSTRIPE_CVM_CAPABILITY_CVM_REQUIRED -->"
		"				<node tag=\"0x9F918528\"> 00 </node>									<!-- TAG_PAYPASS_MSTRIPE_CVM_CAPABILITY_CVM_NOT_REQUIRED -->"
		"				<node tag=\"0x9F7C\"> 30 31 32 33 34 35 36 37 38 39 41 42 43 44 45 46 47 48 49 4A </node>							<!-- TAG_PAYPASS_MERCHANT_CUSTOM_DATA -->"
		"				<node tag=\"0x9F92810F\"> 99 99 00 10 00 00 </node>					<!-- TAG_EP_CLESS_FLOOR_LIMIT -->"
		"				<node tag=\"0x9F91851C\"> 99 99 00 10 00 00 </node>					<!-- TAG_PAYPASS_CLESS_TRANSACTION_LIMIT_NO_DCV -->"
		"				<node tag=\"0x9F91851D\"> 99 99 00 30 00 00 </node>					<!-- TAG_PAYPASS_CLESS_TRANSACTION_LIMIT_DCV -->"
		"				<node tag=\"0x9F92810E\"> 00 00 00 00 00 10 </node>					<!-- TAG_EP_CLESS_CVM_REQUIRED_LIMIT -->"
		"				<node tag=\"0x9F918524\"> 08 </node>									<!-- TAG_PAYPASS_SECURITY_CAPABILITY -->"
		"				<node tag=\"0x9F918709\"> FC 50 FC A0 00 </node>						<!-- TAG_EMV_INT_TAC_DEFAULT : 00 00 00 00 00 (no matching bit) -->"
		"				<node tag=\"0x9F91870A\"> 00 00 00 00 00 </node>						<!-- TAG_EMV_INT_TAC_DENIAL : 00 00 00 00 00 (no matching bit) -->"
		"				<node tag=\"0x9F91870B\"> F8 50 FC F8 00 </node>						<!-- TAG_EMV_INT_TAC_ONLINE : 00 00 00 00 00 (no matching bit) -->"
		"				<node tag=\"0x9F91841D\"> 65 6E 66 72 </node>							<!-- TAG_KERNEL_TERMINAL_SUPPORTED_LANGUAGES -->"
		""
		"				<node tag=\"0x9F1D\"> 6C F8 C0 00 00 00 00 00 </node>			     	<!-- TAG_EMV_TERMINAL_RISK_MANAGEMENT_DATA -->"
		"				<node tag=\"0x9F6A\"> 12 04 92 29 </node>					        	<!-- TAG_PAYPASS_MSTRIPE_UNPREDICTABLE_NUMBER  -->"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<!-- AID : MasterCard PayPass AID for Refund -->"
		"				<node tag=\"0x9F06\"> A0 00 00 00 04 10 10 </node>					<!-- TAG_AID_TERMINAL : MasterCard PayPass AID -->"
		"				<node tag=\"0x9F918701\"> 20 </node>									<!-- TAG_EMV_INT_TRANSACTION_TYPE -->"
		"				<node tag=\"0x9F928100\"> 45 01 00 00 </node>							<!-- TAG_EP_AID_OPTIONS -->"
		"				<node tag=\"0x9F928101\"> 00 02 </node>								<!-- TAG_EP_KERNEL_TO_USE -->"
		"				<node tag=\"0x9F09\"> 00 02  </node>									<!-- TAG_EMV_APPLI_VERSION_NUMBER_TERM -->"
		"				<node tag=\"0x9F918523\"> 00 </node>									<!-- TAG_PAYPASS_CARD_DATA_INPUT_CAPABILITY -->"
		"				<node tag=\"0x9F918525\"> 60 </node>									<!-- TAG_PAYPASS_MCHIP_CVM_CAPABILITY_CVM_REQUIRED -->"
		"				<node tag=\"0x9F918526\"> 28 </node>									<!-- TAG_PAYPASS_MCHIP_CVM_CAPABILITY_CVM_NOT_REQUIRED -->"
		"				<node tag=\"0x9F918502\"> 9F 6A 04 </node>							<!-- TAG_PAYPASS_DEFAULT_UDOL -->"
		"				<node tag=\"0x9F918522\"> 20 </node>									<!-- TAG_PAYPASS_KERNEL_CONFIGURATION -->"
		"				<node tag=\"0x9F6D\"> 00 01 </node>									<!-- TAG_PAYPASS_MSTRIPE_APPLI_VERSION_NUMBER_TERM -->"
		"				<node tag=\"0x9F918527\"> 10 </node>									<!-- TAG_PAYPASS_MSTRIPE_CVM_CAPABILITY_CVM_REQUIRED -->"
		"				<node tag=\"0x9F918528\"> 00 </node>									<!-- TAG_PAYPASS_MSTRIPE_CVM_CAPABILITY_CVM_NOT_REQUIRED -->"
		"				<node tag=\"0x9F7C\"> 30 31 32 33 34 35 36 37 38 39 41 42 43 44 45 46 47 48 49 4A </node>							<!-- TAG_PAYPASS_MERCHANT_CUSTOM_DATA -->"
		"				<node tag=\"0x9F92810F\"> 99 99 00 00 00 00 </node>					<!-- TAG_EP_CLESS_FLOOR_LIMIT -->"
		"				<node tag=\"0x9F91851C\"> 99 99 00 03 00 00 </node>					<!-- TAG_PAYPASS_CLESS_TRANSACTION_LIMIT_NO_DCV -->"
		"				<node tag=\"0x9F91851D\"> 99 99 00 05 00 00 </node>					<!-- TAG_PAYPASS_CLESS_TRANSACTION_LIMIT_DCV -->"
		"				<node tag=\"0x9F92810E\"> 00 00 00 00 00 00 </node>					<!-- TAG_EP_CLESS_CVM_REQUIRED_LIMIT -->"
		"				<node tag=\"0x9F918524\"> 08 </node>									<!-- TAG_PAYPASS_SECURITY_CAPABILITY -->"
		"				<node tag=\"0x9F918709\"> FC 50 FC A0 00 </node>						<!-- TAG_EMV_INT_TAC_DEFAULT -->"
		"				<node tag=\"0x9F91870A\"> F8 51 FC F8 00 </node>						<!-- TAG_EMV_INT_TAC_DENIAL -->"
		"				<node tag=\"0x9F91870B\"> 00 00 00 00 00 </node>						<!-- TAG_EMV_INT_TAC_ONLINE -->"
		"				<node tag=\"0x9F91841D\"> 65 6E 66 72 </node>							<!-- TAG_KERNEL_TERMINAL_SUPPORTED_LANGUAGES -->"
		""
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<!-- AID : MasterCard PayPass AID for Purchase with Cashback -->"
		"				<node tag=\"0x9F06\"> A0 00 00 00 04 10 10 </node>					<!-- TAG_AID_TERMINAL : MasterCard PayPass AID -->"
		"				<node tag=\"0x9F918701\"> 09 </node>									<!-- TAG_EMV_INT_TRANSACTION_TYPE -->"
		"				<node tag=\"0x9F928100\"> 45 01 00 00 </node>							<!-- TAG_EP_AID_OPTIONS -->"
		"				<node tag=\"0x9F928101\"> 00 02 </node>								<!-- TAG_EP_KERNEL_TO_USE -->"
		"				<node tag=\"0x9F09\"> 00 02  </node>									<!-- TAG_EMV_APPLI_VERSION_NUMBER_TERM -->"
		"				<node tag=\"0x9F918523\"> 00 </node>									<!-- TAG_PAYPASS_CARD_DATA_INPUT_CAPABILITY -->"
		"				<node tag=\"0x9F918525\"> 60 </node>									<!-- TAG_PAYPASS_MCHIP_CVM_CAPABILITY_CVM_REQUIRED -->"
		"				<node tag=\"0x9F918526\"> 28 </node>									<!-- TAG_PAYPASS_MCHIP_CVM_CAPABILITY_CVM_NOT_REQUIRED -->"
		"				<node tag=\"0x9F918502\"> 9F 6A 04 </node>							<!-- TAG_PAYPASS_DEFAULT_UDOL -->"
		"				<node tag=\"0x9F918522\"> 20 </node>									<!-- TAG_PAYPASS_KERNEL_CONFIGURATION -->"
		"				<node tag=\"0x9F6D\"> 00 01 </node>									<!-- TAG_PAYPASS_MSTRIPE_APPLI_VERSION_NUMBER_TERM -->"
		"				<node tag=\"0x9F918527\"> 10 </node>									<!-- TAG_PAYPASS_MSTRIPE_CVM_CAPABILITY_CVM_REQUIRED -->"
		"				<node tag=\"0x9F918528\"> 00 </node>									<!-- TAG_PAYPASS_MSTRIPE_CVM_CAPABILITY_CVM_NOT_REQUIRED -->"
		"				<node tag=\"0x9F7C\"> 30 31 32 33 34 35 36 37 38 39 41 42 43 44 45 46 47 48 49 4A </node>							<!-- TAG_PAYPASS_MERCHANT_CUSTOM_DATA -->"
		"				<node tag=\"0x9F92810F\"> 99 99 00 01 00 00 </node>					<!-- TAG_EP_CLESS_FLOOR_LIMIT -->"
		"				<node tag=\"0x9F91851C\"> 99 99 00 10 00 00 </node>					<!-- TAG_PAYPASS_CLESS_TRANSACTION_LIMIT_NO_DCV -->"
		"				<node tag=\"0x9F91851D\"> 99 99 00 30 00 00 </node>					<!-- TAG_PAYPASS_CLESS_TRANSACTION_LIMIT_DCV -->"
		"				<node tag=\"0x9F92810E\"> 00 00 00 00 00 00 </node>					<!-- TAG_EP_CLESS_CVM_REQUIRED_LIMIT -->"
		"				<node tag=\"0x9F918524\"> 08 </node>									<!-- TAG_PAYPASS_SECURITY_CAPABILITY -->"
		"				<node tag=\"0x9F918709\"> FC 50 FC A0 00 </node>						<!-- TAG_EMV_INT_TAC_DEFAULT : 00 00 00 00 00 (no matching bit) -->"
		"				<node tag=\"0x9F91870A\"> 00 00 00 00 00 </node>						<!-- TAG_EMV_INT_TAC_DENIAL : 00 00 00 00 00 (no matching bit) -->"
		"				<node tag=\"0x9F91870B\"> F8 50 FC F8 00 </node>						<!-- TAG_EMV_INT_TAC_ONLINE : 00 00 00 00 00 (no matching bit) -->"
		"				<node tag=\"0x9F91841D\"> 65 6E 66 72 </node>							<!-- TAG_KERNEL_TERMINAL_SUPPORTED_LANGUAGES -->"
		""
		"				<node tag=\"0x9F1D\"> 6C F8 C0 00 00 00 00 00 </node>				<!-- TAG_EMV_TERMINAL_RISK_MANAGEMENT_DATA -->"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<!-- AID : MasterCard PayPass AID for Cash -->"
		"				<node tag=\"0x9F06\"> A0 00 00 00 04 10 10 </node>					<!-- TAG_AID_TERMINAL : MasterCard PayPass AID -->"
		"				<node tag=\"0x9F918701\"> 01 </node>									<!-- TAG_EMV_INT_TRANSACTION_TYPE -->"
		"				<node tag=\"0x9F928100\"> 45 01 00 00 </node>							<!-- TAG_EP_AID_OPTIONS -->"
		"				<node tag=\"0x9F928101\"> 00 02 </node>								<!-- TAG_EP_KERNEL_TO_USE -->"
		"				<node tag=\"0x9F09\"> 00 02  </node>									<!-- TAG_EMV_APPLI_VERSION_NUMBER_TERM -->"
		"				<node tag=\"0x9F918523\"> 00 </node>									<!-- TAG_PAYPASS_CARD_DATA_INPUT_CAPABILITY -->"
		"				<node tag=\"0x9F918525\"> 60 </node>									<!-- TAG_PAYPASS_MCHIP_CVM_CAPABILITY_CVM_REQUIRED -->"
		"				<node tag=\"0x9F918526\"> 28 </node>									<!-- TAG_PAYPASS_MCHIP_CVM_CAPABILITY_CVM_NOT_REQUIRED -->"
		"				<node tag=\"0x9F918502\"> 9F 6A 04 </node>							<!-- TAG_PAYPASS_DEFAULT_UDOL -->"
		"				<node tag=\"0x9F918522\"> 20 </node>									<!-- TAG_PAYPASS_KERNEL_CONFIGURATION -->"
		"				<node tag=\"0x9F6D\"> 00 01 </node>									<!-- TAG_PAYPASS_MSTRIPE_APPLI_VERSION_NUMBER_TERM -->"
		"				<node tag=\"0x9F918527\"> 10 </node>									<!-- TAG_PAYPASS_MSTRIPE_CVM_CAPABILITY_CVM_REQUIRED -->"
		"				<node tag=\"0x9F918528\"> 00 </node>									<!-- TAG_PAYPASS_MSTRIPE_CVM_CAPABILITY_CVM_NOT_REQUIRED -->"
		"				<node tag=\"0x9F7C\"> 30 31 32 33 34 35 36 37 38 39 41 42 43 44 45 46 47 48 49 4A </node>							<!-- TAG_PAYPASS_MERCHANT_CUSTOM_DATA -->"
		"				<node tag=\"0x9F92810F\"> 99 99 00 01 00 00 </node>					<!-- TAG_EP_CLESS_FLOOR_LIMIT -->"
		"				<node tag=\"0x9F91851C\"> 99 99 00 10 00 00 </node>					<!-- TAG_PAYPASS_CLESS_TRANSACTION_LIMIT_NO_DCV -->"
		"				<node tag=\"0x9F91851D\"> 99 99 00 30 00 00 </node>					<!-- TAG_PAYPASS_CLESS_TRANSACTION_LIMIT_DCV -->"
		"				<node tag=\"0x9F92810E\"> 00 00 00 00 00 00 </node>					<!-- TAG_EP_CLESS_CVM_REQUIRED_LIMIT -->"
		"				<node tag=\"0x9F918524\"> 08 </node>									<!-- TAG_PAYPASS_SECURITY_CAPABILITY -->"
		"				<node tag=\"0x9F918709\"> FC 50 FC A0 00 </node>						<!-- TAG_EMV_INT_TAC_DEFAULT : 00 00 00 00 00 (no matching bit) -->"
		"				<node tag=\"0x9F91870A\"> 00 00 00 00 00 </node>						<!-- TAG_EMV_INT_TAC_DENIAL : 00 00 00 00 00 (no matching bit) -->"
		"				<node tag=\"0x9F91870B\"> F8 50 FC F8 00 </node>						<!-- TAG_EMV_INT_TAC_ONLINE : 00 00 00 00 00 (no matching bit) -->"
		"				<node tag=\"0x9F91841D\"> 65 6E 66 72 </node>							<!-- TAG_KERNEL_TERMINAL_SUPPORTED_LANGUAGES -->"
		""
		"				<node tag=\"0x9F1D\"> 6C F8 C0 00 00 00 00 00 </node>				<!-- TAG_EMV_TERMINAL_RISK_MANAGEMENT_DATA -->"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<!-- AID : Maestro PayPass AID for debit -->"
		"				<node tag=\"0x9F06\"> A0 00 00 00 04 30 60 </node>					<!-- TAG_AID_TERMINAL : Maestro PayPass AID -->"
		"				<node tag=\"0x9F918701\"> 00 </node>									<!-- TAG_EMV_INT_TRANSACTION_TYPE -->"
		"				<node tag=\"0x9F928100\"> 45 01 00 00 </node>							<!-- TAG_EP_AID_OPTIONS -->"
		"				<node tag=\"0x9F928101\"> 00 02 </node>								<!-- TAG_EP_KERNEL_TO_USE -->"
		"				<node tag=\"0x9F09\"> 00 02  </node>									<!-- TAG_EMV_APPLI_VERSION_NUMBER_TERM -->"
		"				<node tag=\"0x9F918523\"> 00 </node>									<!-- TAG_PAYPASS_CARD_DATA_INPUT_CAPABILITY -->"
		"				<node tag=\"0x9F918525\"> 60 </node>									<!-- TAG_PAYPASS_MCHIP_CVM_CAPABILITY_CVM_REQUIRED -->"
		"				<node tag=\"0x9F918526\"> 28 </node>									<!-- TAG_PAYPASS_MCHIP_CVM_CAPABILITY_CVM_NOT_REQUIRED -->"
		"				<node tag=\"0x9F918502\"> 9F 6A 04 </node>							<!-- TAG_PAYPASS_DEFAULT_UDOL -->"
		"				<node tag=\"0x9F918522\"> A0 </node>									<!-- TAG_PAYPASS_KERNEL_CONFIGURATION -->"
		"				<node tag=\"0x9F6D\"> 00 01 </node>									<!-- TAG_PAYPASS_MSTRIPE_APPLI_VERSION_NUMBER_TERM -->"
		"				<node tag=\"0x9F918527\"> 10 </node>									<!-- TAG_PAYPASS_MSTRIPE_CVM_CAPABILITY_CVM_REQUIRED -->"
		"				<node tag=\"0x9F918528\"> 00 </node>									<!-- TAG_PAYPASS_MSTRIPE_CVM_CAPABILITY_CVM_NOT_REQUIRED -->"
		"				<node tag=\"0x9F7C\"> 30 31 32 33 34 35 36 37 38 39 41 42 43 44 45 46 47 48 49 4A </node>							<!-- TAG_PAYPASS_MERCHANT_CUSTOM_DATA -->"
		"				<node tag=\"0x9F92810F\"> 99 99 00 00 15 00 </node>					<!-- TAG_EP_CLESS_FLOOR_LIMIT -->"
		"				<node tag=\"0x9F91851C\"> 99 99 00 10 00 00 </node>					<!-- TAG_PAYPASS_CLESS_TRANSACTION_LIMIT_NO_DCV -->"
		"				<node tag=\"0x9F91851D\"> 99 99 00 30 00 00 </node>					<!-- TAG_PAYPASS_CLESS_TRANSACTION_LIMIT_DCV -->"
		"				<node tag=\"0x9F92810E\"> 00 00 00 00 00 00 </node>					<!-- TAG_EP_CLESS_CVM_REQUIRED_LIMIT -->"
		"				<node tag=\"0x9F918524\"> 08 </node>									<!-- TAG_PAYPASS_SECURITY_CAPABILITY -->"
		"				<node tag=\"0x9F918709\"> FC 50 AC A0 00 </node>						<!-- TAG_EMV_INT_TAC_DEFAULT : 00 00 00 00 00 (no matching bit) -->"
		"				<node tag=\"0x9F91870A\"> 00 01 80 00 00 </node>						<!-- TAG_EMV_INT_TAC_DENIAL : 00 00 00 00 00 (no matching bit) -->"
		"				<node tag=\"0x9F91870B\"> 78 50 2C F8 00 </node>						<!-- TAG_EMV_INT_TAC_ONLINE : 00 00 00 00 00 (no matching bit) -->"
		"				<node tag=\"0x9F91841D\"> 65 6E 66 72 </node>							<!-- TAG_KERNEL_TERMINAL_SUPPORTED_LANGUAGES -->"
		""
		"				<node tag=\"0x9F1D\"> 6C F8 80 00 00 00 00 00 </node>				<!-- TAG_EMV_TERMINAL_RISK_MANAGEMENT_DATA -->"
		"			</node>"
		""
		""
		"			<node tag=\"0x1000\">"
		"				<!-- AID : Maestro PayPass AID for Refund -->"
		"				<node tag=\"0x9F06\"> A0 00 00 00 04 30 60 </node>					<!-- TAG_AID_TERMINAL : MasterCard PayPass AID -->"
		"				<node tag=\"0x9F918701\"> 20 </node>									<!-- TAG_EMV_INT_TRANSACTION_TYPE -->"
		"				<node tag=\"0x9F928100\"> 45 01 00 00 </node>							<!-- TAG_EP_AID_OPTIONS -->"
		"				<node tag=\"0x9F928101\"> 00 02 </node>								<!-- TAG_EP_KERNEL_TO_USE : MasterCard -->"
		"				<node tag=\"0x9F09\"> 00 02  </node>									<!-- TAG_EMV_APPLI_VERSION_NUMBER_TERM -->"
		"				<node tag=\"0x9F918523\"> 00 </node>									<!-- TAG_PAYPASS_CARD_DATA_INPUT_CAPABILITY -->"
		"				<node tag=\"0x9F918525\"> 60 </node>									<!-- TAG_PAYPASS_MCHIP_CVM_CAPABILITY_CVM_REQUIRED -->"
		"				<node tag=\"0x9F918526\"> 28 </node>									<!-- TAG_PAYPASS_MCHIP_CVM_CAPABILITY_CVM_NOT_REQUIRED -->"
		"				<node tag=\"0x9F918502\"> 9F 6A 04 </node>							<!-- TAG_PAYPASS_DEFAULT_UDOL -->"
		"				<node tag=\"0x9F918522\"> A0 </node>									<!-- TAG_PAYPASS_KERNEL_CONFIGURATION -->"
		"				<node tag=\"0x9F6D\"> 00 01 </node>									<!-- TAG_PAYPASS_MSTRIPE_APPLI_VERSION_NUMBER_TERM -->"
		"				<node tag=\"0x9F918527\"> 10 </node>									<!-- TAG_PAYPASS_MSTRIPE_CVM_CAPABILITY_CVM_REQUIRED -->"
		"				<node tag=\"0x9F918528\"> 00 </node>									<!-- TAG_PAYPASS_MSTRIPE_CVM_CAPABILITY_CVM_NOT_REQUIRED -->"
		"				<node tag=\"0x9F7C\"> 30 31 32 33 34 35 36 37 38 39 41 42 43 44 45 46 47 48 49 4A </node>							<!-- TAG_PAYPASS_MERCHANT_CUSTOM_DATA -->"
		"				<node tag=\"0x9F92810F\"> 99 99 00 00 00 01 </node>					<!-- TAG_EP_CLESS_FLOOR_LIMIT -->"
		"				<node tag=\"0x9F91851C\"> 99 99 00 03 00 00 </node>					<!-- TAG_PAYPASS_CLESS_TRANSACTION_LIMIT_NO_DCV -->"
		"				<node tag=\"0x9F91851D\"> 99 99 00 05 00 00 </node>					<!-- TAG_PAYPASS_CLESS_TRANSACTION_LIMIT_DCV -->"
		"				<node tag=\"0x9F92810E\"> 00 00 00 00 00 10 </node>					<!-- TAG_EP_CLESS_CVM_REQUIRED_LIMIT -->"
		"				<node tag=\"0x9F918524\"> 08 </node>									<!-- TAG_PAYPASS_SECURITY_CAPABILITY -->"
		"				<node tag=\"0x9F918709\"> FC 50 AC A0 00 </node>						<!-- TAG_EMV_INT_TAC_DEFAULT -->"
		"				<node tag=\"0x9F91870A\"> F8 51 FC F8 00 </node>						<!-- TAG_EMV_INT_TAC_DENIAL -->"
		"				<node tag=\"0x9F91870B\"> 00 00 00 00 00 </node>						<!-- TAG_EMV_INT_TAC_ONLINE -->"
		"				<node tag=\"0x9F91841D\"> 65 6E 66 72 </node>							<!-- TAG_KERNEL_TERMINAL_SUPPORTED_LANGUAGES -->"
		""
		"			</node>"
		""
		""
		"			<node tag=\"0x1000\">"
		"				<!-- AID : Maestro PayPass AID for Purchase with cashback -->"
		"				<node tag=\"0x9F06\"> A0 00 00 00 04 30 60 </node>					<!-- TAG_AID_TERMINAL : Maestro PayPass AID -->"
		"				<node tag=\"0x9F918701\"> 09 </node>									<!-- TAG_EMV_INT_TRANSACTION_TYPE -->"
		"				<node tag=\"0x9F928100\"> 45 01 00 00 </node>							<!-- TAG_EP_AID_OPTIONS -->"
		"				<node tag=\"0x9F928101\"> 00 02 </node>								<!-- TAG_EP_KERNEL_TO_USE -->"
		"				<node tag=\"0x9F09\"> 00 02  </node>									<!-- TAG_EMV_APPLI_VERSION_NUMBER_TERM -->"
		"				<node tag=\"0x9F918523\"> 00 </node>									<!-- TAG_PAYPASS_CARD_DATA_INPUT_CAPABILITY -->"
		"				<node tag=\"0x9F918525\"> 60 </node>									<!-- TAG_PAYPASS_MCHIP_CVM_CAPABILITY_CVM_REQUIRED -->"
		"				<node tag=\"0x9F918526\"> 28 </node>									<!-- TAG_PAYPASS_MCHIP_CVM_CAPABILITY_CVM_NOT_REQUIRED -->"
		"				<node tag=\"0x9F918502\"> 9F 6A 04 </node>							<!-- TAG_PAYPASS_DEFAULT_UDOL -->"
		"				<node tag=\"0x9F918522\"> A0 </node>									<!-- TAG_PAYPASS_KERNEL_CONFIGURATION -->"
		"				<node tag=\"0x9F6D\"> 00 01 </node>									<!-- TAG_PAYPASS_MSTRIPE_APPLI_VERSION_NUMBER_TERM -->"
		"				<node tag=\"0x9F918527\"> 10 </node>									<!-- TAG_PAYPASS_MSTRIPE_CVM_CAPABILITY_CVM_REQUIRED -->"
		"				<node tag=\"0x9F918528\"> 00 </node>									<!-- TAG_PAYPASS_MSTRIPE_CVM_CAPABILITY_CVM_NOT_REQUIRED -->"
		"				<node tag=\"0x9F7C\"> 30 31 32 33 34 35 36 37 38 39 41 42 43 44 45 46 47 48 49 4A </node>							<!-- TAG_PAYPASS_MERCHANT_CUSTOM_DATA -->"
		"				<node tag=\"0x9F92810F\"> 99 99 00 00 15 00 </node>					<!-- TAG_EP_CLESS_FLOOR_LIMIT -->"
		"				<node tag=\"0x9F91851C\"> 99 99 00 10 00 00 </node>					<!-- TAG_PAYPASS_CLESS_TRANSACTION_LIMIT_NO_DCV -->"
		"				<node tag=\"0x9F91851D\"> 99 99 00 30 00 00 </node>					<!-- TAG_PAYPASS_CLESS_TRANSACTION_LIMIT_DCV -->"
		"				<node tag=\"0x9F92810E\"> 00 00 00 00 00 10 </node>					<!-- TAG_EP_CLESS_CVM_REQUIRED_LIMIT -->"
		"				<node tag=\"0x9F918524\"> 08 </node>									<!-- TAG_PAYPASS_SECURITY_CAPABILITY -->"
		"				<node tag=\"0x9F918709\"> FC 50 FC A0 00 </node>						<!-- TAG_EMV_INT_TAC_DEFAULT : 00 00 00 00 00 (no matching bit) -->"
		"				<node tag=\"0x9F91870A\"> 00 00 00 00 00 </node>						<!-- TAG_EMV_INT_TAC_DENIAL : 00 00 00 00 00 (no matching bit) -->"
		"				<node tag=\"0x9F91870B\"> 78 50 2C F8 00 </node>						<!-- TAG_EMV_INT_TAC_ONLINE : 00 00 00 00 00 (no matching bit) -->"
		"				<node tag=\"0x9F91841D\"> 65 6E 66 72 </node>							<!-- TAG_KERNEL_TERMINAL_SUPPORTED_LANGUAGES -->"
		""
		"				<node tag=\"0x9F1D\"> 6C F8 80 00 00 00 00 00 </node>				<!-- TAG_EMV_TERMINAL_RISK_MANAGEMENT_DATA -->"
		"			</node>"
		""
		""
		"			<node tag=\"0x1000\">"
		"				<!-- AID : Maestro PayPass AID for cash -->"
		"				<node tag=\"0x9F06\"> A0 00 00 00 04 30 60 </node>					<!-- TAG_AID_TERMINAL : Maestro PayPass AID -->"
		"				<node tag=\"0x9F918701\"> 01 </node>									<!-- TAG_EMV_INT_TRANSACTION_TYPE -->"
		"				<node tag=\"0x9F928100\"> 45 01 00 00 </node>							<!-- TAG_EP_AID_OPTIONS -->"
		"				<node tag=\"0x9F928101\"> 00 02 </node>								<!-- TAG_EP_KERNEL_TO_USE -->"
		"				<node tag=\"0x9F09\"> 00 02  </node>									<!-- TAG_EMV_APPLI_VERSION_NUMBER_TERM -->"
		"				<node tag=\"0x9F918523\"> 00 </node>									<!-- TAG_PAYPASS_CARD_DATA_INPUT_CAPABILITY -->"
		"				<node tag=\"0x9F918525\"> 60 </node>									<!-- TAG_PAYPASS_MCHIP_CVM_CAPABILITY_CVM_REQUIRED -->"
		"				<node tag=\"0x9F918526\"> 28 </node>									<!-- TAG_PAYPASS_MCHIP_CVM_CAPABILITY_CVM_NOT_REQUIRED -->"
		"				<node tag=\"0x9F918502\"> 9F 6A 04 </node>							<!-- TAG_PAYPASS_DEFAULT_UDOL -->"
		"				<node tag=\"0x9F918522\"> A0 </node>									<!-- TAG_PAYPASS_KERNEL_CONFIGURATION -->"
		"				<node tag=\"0x9F6D\"> 00 01 </node>									<!-- TAG_PAYPASS_MSTRIPE_APPLI_VERSION_NUMBER_TERM -->"
		"				<node tag=\"0x9F918527\"> 10 </node>									<!-- TAG_PAYPASS_MSTRIPE_CVM_CAPABILITY_CVM_REQUIRED -->"
		"				<node tag=\"0x9F918528\"> 00 </node>									<!-- TAG_PAYPASS_MSTRIPE_CVM_CAPABILITY_CVM_NOT_REQUIRED -->"
		"				<node tag=\"0x9F7C\"> 30 31 32 33 34 35 36 37 38 39 41 42 43 44 45 46 47 48 49 4A </node>							<!-- TAG_PAYPASS_MERCHANT_CUSTOM_DATA -->"
		"				<node tag=\"0x9F92810F\"> 99 99 00 00 15 00 </node>					<!-- TAG_EP_CLESS_FLOOR_LIMIT -->"
		"				<node tag=\"0x9F91851C\"> 99 99 00 10 00 00 </node>					<!-- TAG_PAYPASS_CLESS_TRANSACTION_LIMIT_NO_DCV -->"
		"				<node tag=\"0x9F91851D\"> 99 99 00 30 00 00 </node>					<!-- TAG_PAYPASS_CLESS_TRANSACTION_LIMIT_DCV -->"
		"				<node tag=\"0x9F92810E\"> 00 00 00 00 00 10 </node>					<!-- TAG_EP_CLESS_CVM_REQUIRED_LIMIT -->"
		"				<node tag=\"0x9F918524\"> 08 </node>									<!-- TAG_PAYPASS_SECURITY_CAPABILITY -->"
		"				<node tag=\"0x9F918709\"> FC 50 FC A0 00 </node>						<!-- TAG_EMV_INT_TAC_DEFAULT : 00 00 00 00 00 (no matching bit) -->"
		"				<node tag=\"0x9F91870A\"> 00 00 00 00 00 </node>						<!-- TAG_EMV_INT_TAC_DENIAL : 00 00 00 00 00 (no matching bit) -->"
		"				<node tag=\"0x9F91870B\"> 78 50 2C F8 00 </node>						<!-- TAG_EMV_INT_TAC_ONLINE : 00 00 00 00 00 (no matching bit) -->"
		"				<node tag=\"0x9F91841D\"> 65 6E 66 72 </node>							<!-- TAG_KERNEL_TERMINAL_SUPPORTED_LANGUAGES -->"
		""
		"				<node tag=\"0x9F1D\"> 6C F8 80 00 00 00 00 00 </node>				<!-- TAG_EMV_TERMINAL_RISK_MANAGEMENT_DATA -->"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<!-- AID : Discover DPAS AID for debit -->"
		"				<node tag=\"0x9F06\"> A0 00 00 01 52 30 10 </node>				<!-- TAG_AID_TERMINAL : Discover DPAS AID -->"
		"				<node tag=\"0x9F09\"> 01 00 </node>                  				<!-- TAG_EMV_APPLI_VERSION_NUMBER_TERM : 0100 -->"
		"				<node tag=\"0x9F1B\"> 00 00 3A 98 </node>							<!-- TAG_EMV_TERMINAL_FLOOR_LIMIT : 150 euros -->"
		"				<node tag=\"0x9F33\"> E0 E8 08 </node>				          	<!-- TAG_EMV_TERMINAL_CAPABILITIES -->"
		"				<node tag=\"0x9F66\"> B6 00 C0 00 </node>							<!-- TAG_EP_TERMINAL_TRANSACTION_QUALIFIERS -->"
		"				<node tag=\"0x9F918709\"> 00 00 00 00 00 </node>					<!-- TAG_EMV_INT_TAC_DEFAULT -->"
		"				<node tag=\"0x9F91870A\"> 00 00 00 00 00 </node>					<!-- TAG_EMV_INT_TAC_DENIAL -->"
		"				<node tag=\"0x9F91870B\"> 00 00 00 00 00 </node>					<!-- TAG_EMV_INT_TAC_ONLINE -->"
		"				<node tag=\"0x9F928100\"> 07 01 00 00 </node>						<!-- TAG_EP_AID_OPTIONS : Status check Zero amount Partial Match PPSE -->"
		"				<node tag=\"0x9F928101\"> 00 06 </node>								<!-- TAG_EP_KERNEL_TO_USE : Discover DPAS -->"
		"				<node tag=\"0x9F92810D\"> 99 99 10 00 00 00 </node>					<!-- TAG_EP_CLESS_TRANSACTION_LIMIT : 300 euros -->"
		"				<node tag=\"0x9F92810E\"> 00 00 00 00 00 10 </node>					<!-- TAG_EP_CLESS_CVM_REQUIRED_LIMIT: 50 euros -->"
		"				<node tag=\"0x9F92810F\"> 99 99 00 01 50 00 </node>					<!-- TAG_EP_CLESS_FLOOR_LIMIT : 150 euros -->"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<!-- AID : Discover AID for debit -->"
		"				<node tag=\"0x9F06\"> A0 00 00 03 24 10 10 </node>					<!-- TAG_AID_TERMINAL : Discover ZIP AID -->"
		"				<node tag=\"0x9F09\"> 01 00 </node>                     			<!-- TAG_EMV_APPLI_VERSION_NUMBER_TERM : 0100 -->"
		"				<node tag=\"0x9F1B\"> 00 00 3A 98 </node>							<!-- TAG_EMV_TERMINAL_FLOOR_LIMIT : 150 euros -->"
		"				<node tag=\"0x9F33\"> E0 E8 08 </node>								<!-- TAG_EMV_TERMINAL_CAPABILITIES -->"
		"				<node tag=\"0x9F918709\"> 00 00 00 00 00 </node>					<!-- TAG_EMV_INT_TAC_DEFAULT -->"
		"				<node tag=\"0x9F91870A\"> 00 00 00 00 00 </node>					<!-- TAG_EMV_INT_TAC_DENIAL -->"
		"				<node tag=\"0x9F91870B\"> 00 00 00 00 00 </node>					<!-- TAG_EMV_INT_TAC_ONLINE -->"
		"				<node tag=\"0x9F928101\"> 01 02 </node>								<!-- TAG_EP_KERNEL_TO_USE : Discover -->"
		"				<node tag=\"0x9F928100\"> 07 01 00 00 </node>						<!-- TAG_EP_AID_OPTIONS : Zero amount Partial Match PPSE LOA -->"
		"				<node tag=\"0x9F92810D\"> 99 99 10 00 00 00 </node>					<!-- TAG_EP_CLESS_TRANSACTION_LIMIT : 300 euros -->"
		"				<node tag=\"0x9F92810E\"> 00 00 00 00 00 10 </node>					<!-- TAG_EP_CLESS_CVM_REQUIRED_LIMIT: 20 euros -->"
		"				<node tag=\"0x9F92810F\"> 99 99 00 01 50 00 </node>					<!-- TAG_EP_CLESS_FLOOR_LIMIT : 150 euros -->"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<!-- AID : Interac AID -->"
		"				<node tag=\"0x9F06\"> A0 00 00 02 77 10 10 </node>				<!-- TAG_AID_TERMINAL : Interac AID -->"
		"				<node tag=\"0x9F928101\"> 01 03 </node>							<!-- TAG_EP_KERNEL_TO_USE : Interac -->"
		"				<node tag=\"0x9F928100\"> 05 03 00 00 </node>						<!-- TAG_EP_AID_OPTIONS : Partial AID & Zero amount & PPSE -->"
		"				<node tag=\"0x9F92810D\"> 99 99 10 00 00 00 </node>				<!-- TAG_EP_CLESS_TRANSACTION_LIMIT : 100.00 -->"
		"				<node tag=\"0x9F92810E\"> 00 00 00 00 00 10 </node>				<!-- TAG_EP_CLESS_CVM_REQUIRED_LIMIT : 10.00 -->"
		"				<node tag=\"0x9F92810F\"> 99 99 00 00 15 00 </node>				<!-- TAG_EP_CLESS_FLOOR_LIMIT : 15.00 -->"
		"				<node tag=\"0x9F1B\"> 00 00 27 10 </node>							<!-- TAG_EMV_TERMINAL_FLOOR_LIMIT : 100.00 -->"
		"				<node tag=\"0x9F918A11\"> 00 02 </node>				            <!-- TAG_INTERAC_INT_TERMINAL_AVN_LIST : 0002 -->"
		"				<node tag=\"0x9F918A01\"> 03 </node>								<!-- TAG_INTERAC_RETRY_LIMIT : 3 tries -->"
		"				<node tag=\"0x9F918A04\"> E0 08 00 </node>						<!-- TAG_INTERAC_TERMINAL_CAPABILITIES_CVM_REQ : E0 08 00 (No CVM and no CDA) -->"
		"				<node tag=\"0x9F918A05\"> E0 08 00 </node>						<!-- TAG_INTERAC_TERMINAL_CAPABILITIES_NO_CVM_REQ : E0 08 00 (No CVM and no CDA) -->"
		"				<node tag=\"0x9F918709\"> 00 00 00 00 00 </node>					<!-- TAG_EMV_INT_TAC_DEFAULT : 00 00 00 00 00 (no matching bit) -->"
		"				<node tag=\"0x9F91870A\"> 00 00 00 00 00 </node>					<!-- TAG_EMV_INT_TAC_DENIAL : 00 00 00 00 00 (no matching bit) -->"
		"				<node tag=\"0x9F91870B\"> 00 00 00 00 00 </node>					<!-- TAG_EMV_INT_TAC_ONLINE : 00 00 00 00 00 (no matching bit) -->"
		"				<node tag=\"0x9F918706\"> 9F 08 02 </node>						<!-- TAG_EMV_INT_DEFAULT_TDOL : 9F 08 02 -->"
		"				<node tag=\"0x9F91841D\"> 65 6E 66 72 </node>						<!-- TAG_KERNEL_TERMINAL_SUPPORTED_LANGUAGES : en, fr -->"
		"				<node tag=\"0x9F58\"> 03 </node>				            		<!-- TAG_INTERAC_MERCHANT_TYPE_INDICATOR : 03 (in range 01 to 05) -->"
		"				<node tag=\"0x9F59\"> C0 80 00 </node>			            	<!-- TAG_INTERAC_TERMINAL_TRANSACTION_INFORMATION : C0 80 00 (Display, contactless, contact and mag: other bits set in kernel) -->"
		"				<node tag=\"0x9F5A\"> 00 </node>			                		<!-- TAG_INTERAC_TERMINAL_TRANSACTION_TYPE : 00 (01 is refund) -->"
		"				<node tag=\"0x9F5D\"> 00 00 00 00 00 00 </node>	             	<!-- TAG_INTERAC_RECEIPT_LIMIT : 00 00 00 00 00 00 -->"
		"				<node tag=\"0x9F5E\"> E0 00 </node>				            	<!-- TAG_INTERAC_TERMINAL_OPTION_STATUS : D0 00 -->"
		"				<node tag=\"0x9F1A\"> 08 34 </node>								<!-- TAG_EMV_TERMINAL_COUNTRY_CODE -->"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<!-- AID : PURE AID -->"
		"				<node tag=\"0x9F06\"> A0 00 00 04 94 10 10 </node>					<!-- TAG_AID_TERMINAL : PURE AID -->"
		"				<node tag=\"0x9F918701\"> 00 </node>								<!-- TAG_EMV_INT_TRANSACTION_TYPE (Purchase) -->"
		"				<node tag=\"0x9F928101\"> 01 05 </node>								<!-- TAG_EP_KERNEL_TO_USE : PURE -->"
		"				<node tag=\"0x9F928100\"> 05 01 00 00 </node>						<!-- TAG_EP_AID_OPTIONS : Partial AID & Zero amount & PPSE -->"
		"				<node tag=\"0x9F92810D\"> 00 00 00 01 00 00 </node>					<!-- TAG_EP_CLESS_TRANSACTION_LIMIT : 100.00 -->"
		"				<node tag=\"0x9F92810E\"> 00 00 00 00 25 00 </node>					<!-- TAG_EP_CLESS_CVM_REQUIRED_LIMIT : 25.00 -->"
		"				<node tag=\"0x9F92810F\"> 00 00 00 00 50 00 </node>					<!-- TAG_EP_CLESS_FLOOR_LIMIT : 50.00 -->"
		"				<node tag=\"0x9F1B\"> 00 00 13 88 </node>							<!-- TAG_EMV_TERMINAL_FLOOR_LIMIT : 50.00 -->"
		"				<node tag=\"0x9F918B11\"> 00 01 </node>				            	<!-- TAG_PURE_INT_TERMINAL_AVN_LIST : 0100 -->"
		"				<node tag=\"0x9F918D04\"> 3E 00 80 47 F9 </node>					<!-- TAG_PURE_KERNEL_CAPABILITIES : E0 48 08 (No CVM, Online PIN and CDA) -->"
		"				<node tag=\"0x9F918709\"> 00 00 00 00 00 </node>					<!-- TAG_EMV_INT_TAC_DEFAULT : 00 00 00 00 00 (no matching bit) -->"
		"				<node tag=\"0x9F91870A\"> 00 00 00 00 00 </node>					<!-- TAG_EMV_INT_TAC_DENIAL : 00 00 00 00 00 (no matching bit) -->"
		"				<node tag=\"0x9F91870B\"> 00 00 00 80 00 </node>					<!-- TAG_EMV_INT_TAC_ONLINE : 00 00 00 80 00 (txn exceeds floor limit) -->"
		"				<node tag=\"0x9F91841D\"> 65 6E 66 72 </node>						<!-- TAG_KERNEL_TERMINAL_SUPPORTED_LANGUAGES : en, fr -->"
		"				<node tag=\"0x9F918D05\"> F0 </node>								<!-- TAG_PURE_IMPLEMENTATION_OPTIONS : PURE_IO_OPTION1, 2, 3 and 4 supported -->"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<!-- AID : PURE AID -->"
		"				<node tag=\"0x9F06\"> A0 00 00 04 94 20 10 </node>					<!-- TAG_AID_TERMINAL : PURE AID -->"
		"				<node tag=\"0x9F918701\"> 00 </node>								<!-- TAG_EMV_INT_TRANSACTION_TYPE (Purchase) -->"
		"				<node tag=\"0x9F928101\"> 01 05 </node>								<!-- TAG_EP_KERNEL_TO_USE : PURE -->"
		"				<node tag=\"0x9F928100\"> 05 01 00 00 </node>						<!-- TAG_EP_AID_OPTIONS : Partial AID & Zero amount & PPSE -->"
		"				<node tag=\"0x9F92810D\"> 00 00 00 01 00 00 </node>					<!-- TAG_EP_CLESS_TRANSACTION_LIMIT : 100.00 -->"
		"				<node tag=\"0x9F92810E\"> 00 00 00 00 25 00 </node>					<!-- TAG_EP_CLESS_CVM_REQUIRED_LIMIT : 25.00 -->"
		"				<node tag=\"0x9F92810F\"> 00 00 00 00 50 00 </node>					<!-- TAG_EP_CLESS_FLOOR_LIMIT : 50.00 -->"
		"				<node tag=\"0x9F1B\"> 00 00 13 88 </node>							<!-- TAG_EMV_TERMINAL_FLOOR_LIMIT : 50.00 -->"
		"				<node tag=\"0x9F918B11\"> 00 01 </node>				            	<!-- TAG_PURE_INT_TERMINAL_AVN_LIST : 0100 -->"
		"				<node tag=\"0x9F918D04\"> 3E 00 80 47 F9 </node>					<!-- TAG_PURE_KERNEL_CAPABILITIES : E0 48 08 (No CVM, Online PIN and CDA) -->"
		"				<node tag=\"0x9F918709\"> 00 00 00 00 00 </node>					<!-- TAG_EMV_INT_TAC_DEFAULT : 00 00 00 00 00 (no matching bit) -->"
		"				<node tag=\"0x9F91870A\"> 00 00 00 00 00 </node>					<!-- TAG_EMV_INT_TAC_DENIAL : 00 00 00 00 00 (no matching bit) -->"
		"				<node tag=\"0x9F91870B\"> 00 00 00 80 00 </node>					<!-- TAG_EMV_INT_TAC_ONLINE : 00 00 00 80 00 (txn exceeds floor limit) -->"
		"				<node tag=\"0x9F91841D\"> 65 6E 66 72 </node>						<!-- TAG_KERNEL_TERMINAL_SUPPORTED_LANGUAGES : en, fr -->"
		"				<node tag=\"0x9F918D05\"> F0 </node>								<!-- TAG_PURE_IMPLEMENTATION_OPTIONS : PURE_IO_OPTION1, 2, 3 and 4 supported -->"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<!-- AID : PURE AID (test) -->"
		"				<node tag=\"0x9F06\"> D9 99 99 99 99 10 10 </node>					<!-- TAG_AID_TERMINAL : PURE AID -->"
		"				<node tag=\"0x9F918701\"> 00 </node>								<!-- TAG_EMV_INT_TRANSACTION_TYPE (Purchase) -->"
		"				<node tag=\"0x9F928101\"> 01 05 </node>								<!-- TAG_EP_KERNEL_TO_USE : PURE -->"
		"				<node tag=\"0x9F928100\"> 45 01 00 00 </node>						<!-- TAG_EP_AID_OPTIONS : Partial AID & Zero amount & PPSE -->"
		"				<node tag=\"0x9F92810D\"> 00 00 00 01 00 00 </node>					<!-- TAG_EP_CLESS_TRANSACTION_LIMIT : 100.00 -->"
		"				<node tag=\"0x9F92810E\"> 00 00 00 00 25 00 </node>					<!-- TAG_EP_CLESS_CVM_REQUIRED_LIMIT : 25.00 -->"
		"				<node tag=\"0x9F92810F\"> 00 00 00 00 50 00 </node>					<!-- TAG_EP_CLESS_FLOOR_LIMIT : 50.00 -->"
		"				<node tag=\"0x9F1B\"> 00 00 13 88 </node>							<!-- TAG_EMV_TERMINAL_FLOOR_LIMIT : 50.00 -->"
		"				<node tag=\"0x9F918B11\"> 00 01 </node>				            	<!-- TAG_PURE_INT_TERMINAL_AVN_LIST : 0100 -->"
		"				<node tag=\"0x9F918D04\"> 36 00 E0 4B F9 </node>					<!-- TAG_PURE_KERNEL_CAPABILITIES -->"
		"				<node tag=\"0x9F918709\"> 00 00 00 00 00 </node>					<!-- TAG_EMV_INT_TAC_DEFAULT : 00 00 00 00 00 (no matching bit) -->"
		"				<node tag=\"0x9F91870A\"> 00 00 00 00 00 </node>					<!-- TAG_EMV_INT_TAC_DENIAL : 00 00 00 00 00 (no matching bit) -->"
		"				<node tag=\"0x9F91870B\"> 00 00 00 80 00 </node>					<!-- TAG_EMV_INT_TAC_ONLINE : 00 00 00 80 00 (txn exceeds floor limit) -->"
		"				<node tag=\"0x9F91841D\"> 65 6E 66 72 </node>						<!-- TAG_KERNEL_TERMINAL_SUPPORTED_LANGUAGES : en, fr -->"
		"				<node tag=\"0x9F918D05\"> F0 </node>								<!-- TAG_PURE_IMPLEMENTATION_OPTIONS : PURE_IO_OPTION1, 2, 3 and 4 supported -->"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<!-- AID : PURE AID (test) -->"
		"				<node tag=\"0x9F06\"> D9 99 99 99 99 20 20 </node>					<!-- TAG_AID_TERMINAL : PURE AID -->"
		"				<node tag=\"0x9F918701\"> 00 </node>								<!-- TAG_EMV_INT_TRANSACTION_TYPE (Purchase) -->"
		"				<node tag=\"0x9F928101\"> 01 05 </node>								<!-- TAG_EP_KERNEL_TO_USE : PURE -->"
		"				<node tag=\"0x9F928100\"> 45 01 00 00 </node>						<!-- TAG_EP_AID_OPTIONS : Partial AID & Zero amount & PPSE -->"
		"				<node tag=\"0x9F92810D\"> 00 00 00 01 00 00 </node>					<!-- TAG_EP_CLESS_TRANSACTION_LIMIT : 100.00 -->"
		"				<node tag=\"0x9F92810E\"> 00 00 00 00 25 00 </node>					<!-- TAG_EP_CLESS_CVM_REQUIRED_LIMIT : 25.00 -->"
		"				<node tag=\"0x9F92810F\"> 00 00 00 00 50 00 </node>					<!-- TAG_EP_CLESS_FLOOR_LIMIT : 50.00 -->"
		"				<node tag=\"0x9F1B\"> 00 00 13 88 </node>							<!-- TAG_EMV_TERMINAL_FLOOR_LIMIT : 50.00 -->"
		"				<node tag=\"0x9F918B11\"> 00 01 </node>				            	<!-- TAG_PURE_INT_TERMINAL_AVN_LIST : 0100 -->"
		"				<node tag=\"0x9F918D04\"> 36 00 E0 4B F9 </node>					<!-- TAG_PURE_KERNEL_CAPABILITIES -->"
		"				<node tag=\"0x9F918709\"> 00 00 00 00 00 </node>					<!-- TAG_EMV_INT_TAC_DEFAULT : 00 00 00 00 00 (no matching bit) -->"
		"				<node tag=\"0x9F91870A\"> 00 00 00 00 00 </node>					<!-- TAG_EMV_INT_TAC_DENIAL : 00 00 00 00 00 (no matching bit) -->"
		"				<node tag=\"0x9F91870B\"> 00 00 00 80 00 </node>					<!-- TAG_EMV_INT_TAC_ONLINE : 00 00 00 80 00 (txn exceeds floor limit) -->"
		"				<node tag=\"0x9F91841D\"> 65 6E 66 72 </node>						<!-- TAG_KERNEL_TERMINAL_SUPPORTED_LANGUAGES : en, fr -->"
		"				<node tag=\"0x9F918D05\"> F0 </node>								<!-- TAG_PURE_IMPLEMENTATION_OPTIONS : PURE_IO_OPTION1, 2, 3 and 4 supported -->"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<!-- AID : PagoBANCOMAT AID for debit -->"
		"				<node tag=\"0x9F06\"> 	A0 00 00 01 41 00 01 </node>				<!-- TAG_AID_TERMINAL : 		Pagobancomat AID A0000001410001 -->"
		"				<node tag=\"0x9F928100\"> 04 01 00 00 </node>						<!-- TAG_EP_AID_OPTIONS :		PPSE method and Zero Amount -->"
		"				<node tag=\"0x9F928101\"> 01 06 </node>								<!-- TAG_EP_KERNEL_TO_USE -->"
		"				<node tag=\"0x9F09\"> 	00 0B  </node>								<!-- TAG_EMV_APPLI_VERSION_NUMBER_TERM 	00 0B -->"
		"				<node tag=\"0x9F66\"> 	34 20 40 00 </node>							<!-- TAG_PAGO_BANCOMAT_TERMINAL_TRANSACTION_QUALIFIERS -->"
		"				<!-- EMV Mode Supported, EMV chip supported, Pin Online supported -->"
		"				<!-- EMV chip offline pin supported, Consumer device cvm supported -->"
		"				<node tag=\"0x9F92810D\"> 00 10 00 00 00 00  </node>				<!-- TAG_EP_CLESS_TRANSACTION_LIMIT :   10.000.000,00 Euro -->"
		"				<node tag=\"0x9F92810E\"> 00 00 00 00 25 00 </node>					<!-- TAG_EP_CLESS_CVM_REQUIRED_LIMIT            25,00 Euro -->"
		"				<node tag=\"0x9F92810F\"> 00 00 00 00 25 00 </node>					<!-- TAG_EP_CLESS_FLOOR_LIMIT 		            25,00 Euro -->"
		"				<node tag=\"0x9F918709\"> 10 60 04 80 00 </node>					<!-- TAG_EMV_INT_TAC_DEFAULT -->"
		"				<node tag=\"0x9F91870A\"> A4 00 18 00 00 </node>					<!-- TAG_EMV_INT_TAC_DENIAL -->"
		"				<node tag=\"0x9F91870B\"> 10 60 04 18 00 </node>					<!-- TAG_EMV_INT_TAC_ONLINE -->"
		"				<node tag=\"0x9F928305\">	00 10 40 00 00 </node>					<!-- TAG_PAGO_BANCOMAT_TAC_SWITCH_INTERFACE     25,00 Euro -->"
		"				<node tag=\"0x9F918707\"> 00 </node>								<!-- TAG_EMV_INT_MAX_TARGET_PERC_BIASED_RAND_SEL   00 -->"
		"				<node tag=\"0x9F918708\"> 00 </node>								<!-- TAG_EMV_INT_TARGET_PERC_RAND_SEL              00 -->"
		"				<node tag=\"0x9F91870C\"> 00 00 00 00 </node>						<!-- TAG_EMV_INT_THRESHOLD_VALUE_BIASED_RAND_SEL   00000000	-->"
		"				<node tag=\"0x9F91841D\"> 49 54 65 6E </node>						<!-- TAG_KERNEL_TERMINAL_SUPPORTED_LANGUAGES 		IT, en -->"
		"				<node tag=\"0x9F928310\"> 00 </node>								<!-- TAG_PAGO_BANCOMAT_INT_ACCEPT_MISSING_IAC_SWITCH_INTERFACE  PAGO_BANCOMAT_USE_DEFAULT_IAC_SWITCH_INTERFACE_VALUE -->"
		"			</node>"
		""
		"		</node> <!-- End node 0xBF918800 : TAG_SAMPLE_AID_PARAMETERS -->"
		""
		"		<node tag=\"0xBF918801\">	<!-- TAG_SAMPLE_CAKEYS_PARAMETERS -->"
		""
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F06\"> A0 00 00 00 03 </node>"
		"				<node tag=\"0x9F22\"> 05 </node>"
		"				<node tag=\"0x9F918704\">"
		"													D0 13 5C E8 A4 43 6C 7F 9D 5C C6 65 47 E3 0E A4"
		"													02 F9 81 05 B7 17 22 E2 4B C0 8D CC 80 AB 7E 71"
		"													EC 23 B8 CE 6A 1D C6 AC 2A 8C F5 55 43 D7 4A 8A"
		"													E7 B3 88 F9 B1 74 B7 F0 D7 56 C2 2C BB 59 74 F9"
		"													01 6A 56 B6 01 CC A6 4C 71 F0 4B 78 E8 6C 50 1B"
		"													19 3A 55 56 D5 38 9E CE 4D EA 25 8A B9 7F 52 A3</node>"
		"				<node tag=\"0x9F918703\"> 03 </node>"
		"				<node tag=\"0x9F918702\"> 86 DF 04 1E 79 95 02 35 52 A7 9E 26 23 E4 91 80 C0 CD 95 7A </node>"
		"			</node>"
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F06\"> A0 00 00 00 03 </node>"
		"				<node tag=\"0x9F22\"> 07 </node>"
		"				<node tag=\"0x9F918704\"> A8 9F 25 A5 6F A6 DA 25 8C 8C A8 B4 04 27 D9 27 B4 A1 EB 4D"
		"													7E A3 26 BB B1 2F 97 DE D7 0A E5 E4 48 0F C9 C5 E8 A9 72 17"
		"													71 10 A1 CC 31 8D 06 D2 F8 F5 C4 84 4A C5 FA 79 A4 DC 47 0B"
		"													B1 1E D6 35 69 9C 17 08 1B 90 F1 B9 84 F1 2E 92 C1 C5 29 27"
		"													6D 8A F8 EC 7F 28 49 20 97 D8 CD 5B EC EA 16 FE 40 88 F6 CF"
		"													AB 4A 1B 42 32 8A 1B 99 6F 92 78 B0 B7 E3 31 1C A5 EF 85 6C"
		"													2F 88 84 74 B8 36 12 A8 2E 4E 00 D0 CD 40 69 A6 78 31 40 43"
		"													3D 50 72 5F </node>"
		"				<node tag=\"0x9F918703\"> 03 </node>"
		"				<node tag=\"0x9F918702\"> B4 BC 56 CC 4E 88 32 49 32 CB C6 43 D6 89 8F 6F E5 93 B1 72 </node>"
		"			</node>"
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F06\"> A0 00 00 00 03 </node>"
		"				<node tag=\"0x9F22\"> 08 </node>"
		"				<node tag=\"0x9F918704\">"
		"													D9 FD 6E D7 5D 51 D0 E3 06 64 BD 15 70 23 EA A1"
		"													FF A8 71 E4 DA 65 67 2B 86 3D 25 5E 81 E1 37 A5"
		"													1D E4 F7 2B CC 9E 44 AC E1 21 27 F8 7E 26 3D 3A"
		"													F9 DD 9C F3 5C A4 A7 B0 1E 90 70 00 BA 85 D2 49"
		"													54 C2 FC A3 07 48 25 DD D4 C0 C8 F1 86 CB 02 0F"
		"													68 3E 02 F2 DE AD 39 69 13 3F 06 F7 84 51 66 AC"
		"													EB 57 CA 0F C2 60 34 45 46 98 11 D2 93 BF EF BA"
		"													FA B5 76 31 B3 DD 91 E7 96 BF 85 0A 25 01 2F 1A"
		"													E3 8F 05 AA 5C 4D 6D 03 B1 DC 2E 56 86 12 78 59"
		"													38 BB C9 B3 CD 3A 91 0C 1D A5 5A 5A 92 18 AC E0"
		"													F7 A2 12 87 75 26 82 F1 58 32 A6 78 D6 E1 ED 0B </node>"
		"				<node tag=\"0x9F918703\"> 03 </node>"
		"				<node tag=\"0x9F918702\"> 20 D2 13 12 69 55 DE 20 5A DC 2F D2 82 2B D2 2D E2 1C F9 A8 </node>"
		"			</node>"
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F06\"> A0 00 00 00 03 </node>"
		"				<node tag=\"0x9F22\"> 09 </node>"
		"				<node tag=\"0x9F918704\">"
		"													 9D 91 22 48 DE 0A 4E 39 C1 A7 DD E3 F6 D2 58 89"
		"													 92 C1 A4 09 5A FB D1 82 4D 1B A7 48 47 F2 BC 49"
		"													 26 D2 EF D9 04 B4 B5 49 54 CD 18 9A 54 C5 D1 17"
		"													 96 54 F8 F9 B0 D2 AB 5F 03 57 EB 64 2F ED A9 5D"
		"													 39 12 C6 57 69 45 FA B8 97 E7 06 2C AA 44 A4 AA"
		"													 06 B8 FE 6E 3D BA 18 AF 6A E3 73 8E 30 42 9E E9"
		"													 BE 03 42 7C 9D 64 F6 95 FA 8C AB 4B FE 37 68 53"
		"													 EA 34 AD 1D 76 BF CA D1 59 08 C0 77 FF E6 DC 55"
		"													 21 EC EF 5D 27 8A 96 E2 6F 57 35 9F FA ED A1 94"
		"													 34 B9 37 F1 AD 99 9D C5 C4 1E B1 19 35 B4 4C 18"
		"													 10 0E 85 7F 43 1A 4A 5A 6B B6 51 14 F1 74 C2 D7"
		"													 B5 9F DF 23 7D 6B B1 DD 09 16 E6 44 D7 09 DE D5"
		"													 64 81 47 7C 75 D9 5C DD 68 25 46 15 F7 74 0E C0"
		"													 7F 33 0A C5 D6 7B CD 75 BF 23 D2 8A 14 08 26 C0"
		"													 26 DB DE 97 1A 37 CD 3E F9 B8 DF 64 4A C3 85 01"
		"													 05 01 EF C6 50 9D 7A 41 </node>"
		"				<node tag=\"0x9F918703\"> 03 </node>"
		"				<node tag=\"0x9F918702\"> 1F F8 0A 40 17 3F 52 D7 D2 7E 0F 26 A1 46 A1 C8 CC B2 90 46 </node>"
		"			</node>"
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F06\"> A0 00 00 00 03 </node>"
		"				<node tag=\"0x9F22\"> 50 </node>"
		"				<node tag=\"0x9F918704\"> D1 11 97 59 00 57 B8 41 96 C2 F4 D1 1A 8F 3C 05"
		"													40 8F 42 2A 35 D7 02 F9 01 06 EA 5B 01 9B B2 8A"
		"													E6 07 AA 9C DE BC D0 D8 1A 38 D4 8C 7E BB 00 62"
		"													D2 87 36 9E C0 C4 21 24 24 6A C3 0D 80 CD 60 2A"
		"													B7 23 8D 51 08 4D ED 46 98 16 2C 59 D2 5E AC 1E"
		"													66 25 5B 4D B2 35 25 26 EF 09 82 C3 B8 AD 3D 1C"
		"													CE 85 B0 1D B5 78 8E 75 E0 9F 44 BE 73 61 36 6D"
		"													EF 9D 1E 13 17 B0 5E 5D 0F F5 29 0F 88 A0 DB 47 </node>"
		"				<node tag=\"0x9F918703\"> 01 00 01 </node>"
		"				<node tag=\"0x9F918702\"> B7 69 77 56 68 CA CB 5D 22 A6 47 D1 D9 93 14 1E DA B7 23 7B </node>"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F06\"> A0 00 00 00 03 </node>"
		"				<node tag=\"0x9F22\"> 51 </node>"
		"				<node tag=\"0x9F918704\"> DB 5F A2 9D 1F DA 8C 16 34 B0 4D CC FF 14 8A BE"
		"													E6 3C 77 20 35 C7 98 51 D3 51 21 07 58 6E 02 A9"
		"													17 F7 C7 E8 85 E7 C4 A7 D5 29 71 0A 14 53 34 CE"
		"													67 DC 41 2C B1 59 7B 77 AA 25 43 B9 8D 19 CF 2C"
		"													B8 0C 52 2B DB EA 0F 1B 11 3F A2 C8 62 16 C8 C6"
		"													10 A2 D5 8F 29 CF 33 55 CE B1 BD 3E F4 10 D1 ED"
		"													D1 F7 AE 0F 16 89 79 79 DE 28 C6 EF 29 3E 0A 19"
		"													28 2B D1 D7 93 F1 33 15 23 FC 71 A2 28 80 04 68"
		"													C0 1A 36 53 D1 4C 6B 48 51 A5 C0 29 47 8E 75 7F </node>"
		"				<node tag=\"0x9F918703\"> 03 </node>"
		"				<node tag=\"0x9F918702\"> B9 D2 48 07 5A 3F 23 B5 22 FE 45 57 3E 04 37 4D C4 99 5D 71 </node>"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F06\"> A0 00 00 00 03 </node>"
		"				<node tag=\"0x9F22\"> 53 </node>"
		"				<node tag=\"0x9F918704\"> BC D8 37 21 BE 52 CC CC 4B 64 57 32 1F 22 A7 DC"
		"													76 9F 54 EB 80 25 91 3B E8 04 D9 EA BB FA 19 B3"
		"													D7 C5 D3 CA 65 8D 76 8C AF 57 06 7E EC 83 C7 E6"
		"													E9 F8 1D 05 86 70 3E D9 DD DA DD 20 67 5D 63 42"
		"													49 80 B1 0E B3 64 E8 1E B3 7D B4 0E D1 00 34 4C"
		"													92 88 86 FF 4C CC 37 20 3E E6 10 6D 5B 59 D1 AC"
		"													10 2E 2C D2 D7 AC 17 F4 D9 6C 39 8E 5F D9 93 EC"
		"													B4 FF DF 79 B1 75 47 FF 9F A2 AA 8E EF D6 CB DA"
		"													12 4C BB 17 A0 F8 52 81 46 38 71 35 E2 26 B0 05"
		"													A4 74 B9 06 2F F2 64 D2 FF 8E FA 36 81 4A A2 95"
		"													00 65 B1 B0 4C 0A 1A E9 B2 F6 9D 4A 4A A9 79 D6"
		"													CE 95 FE E9 48 5E D0 A0 3A EE 9B D9 53 E8 1C FD"
		"													1E F6 E8 14 DF D3 C2 CE 37 AE FA 38 C1 F9 87 73"
		"													71 E9 1D 6A 5E B5 9F DE DF 75 D3 32 5F A3 CA 66"
		"													CD FB A0 E5 71 46 CC 78 98 18 FF 06 BE 5F CC 50"
		"													AB D3 62 AE 4B 80 99 6D </node>"
		"				<node tag=\"0x9F918703\"> 03 </node>"
		"				<node tag=\"0x9F918702\"> AC 21 3A 2E 0D 2C 0C A3 5A D0 20 13 23 53 6D 58 09 7E 4E 57 </node>"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F06\"> A0 00 00 00 03 </node>"
		"				<node tag=\"0x9F22\"> 89 </node>"
		"				<node tag=\"0x9F918704\"> E5 E1 95 70 5C E6 1A 06 72 B8 36 7E 7A 51 71 39"
		"													27 A0 42 89 EA 30 83 28 FA D2 80 71 EC EA E8 89"
		"													B3 C4 F2 9A C3 BD E4 67 72 B0 0D 42 FD 05 F2 72"
		"													28 82 0F 26 93 99 0F 81 B0 F6 92 8E 24 0D 95 7E"
		"													C4 48 43 54 CD 5E 5C A9 09 2B 44 47 41 A0 39 4D"
		"													34 76 65 12 32 47 4A 9B 87 A9 61 DA 8D D9 6D 90"
		"													F0 36 E9 B3 C5 2F B0 97 66 BD A4 D6 BC 3B DA DB"
		"													C8 91 22 B7 40 68 F8 FA 04 02 6C 5F A8 EF 39 8B"
		"													C3 AB 39 92 A8 7F 6A 78 5C C7 79 BA 99 F1 70 95"
		"													66 23 D6 7A 18 EB 83 24 26 3D 62 6B E8 5B FF 77"
		"													B8 B9 81 C0 A3 F7 84 9C 4F 3D 8E 20 54 29 55 D1"
		"													91 28 19 85 47 B4 7A E3 4D F6 7F 28 BE 43 3F 33 </node>"
		"				<node tag=\"0x9F918703\"> 03 </node>"
		"				<node tag=\"0x9F918702\"> 71 70 85 0B 97 F8 39 52 04 5C F9 CA 8B 76 12 DF EB 69 E9 EF </node>"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F06\"> A0 00 00 00 03 </node>"
		"				<node tag=\"0x9F22\"> 92 </node>"
		"				<node tag=\"0x9F918704\"> 99 6A F5 6F 56 91 87 D0 92 93 C1 48 10 45 0E D8"
		"													EE 33 57 39 7B 18 A2 45 8E FA A9 2D A3 B6 DF 65"
		"													14 EC 06 01 95 31 8F D4 3B E9 B8 F0 CC 66 9E 3F"
		"													84 40 57 CB DD F8 BD A1 91 BB 64 47 3B C8 DC 9A"
		"													73 0D B8 F6 B4 ED E3 92 41 86 FF D9 B8 C7 73 57"
		"													89 C2 3A 36 BA 0B 8A F6 53 72 EB 57 EA 5D 89 E7"
		"													D1 4E 9C 7B 6B 55 74 60 F1 08 85 DA 16 AC 92 3F"
		"													15 AF 37 58 F0 F0 3E BD 3C 5C 2C 94 9C BA 30 6D"
		"													B4 4E 6A 2C 07 6C 5F 67 E2 81 D7 EF 56 78 5D C4"
		"													D7 59 45 E4 91 F0 19 18 80 0A 9E 2D C6 6F 60 08"
		"													05 66 CE 0D AF 8D 17 EA D4 6A D8 E3 0A 24 7C 9F </node>"
		"				<node tag=\"0x9F918703\"> 03 </node>"
		"				<node tag=\"0x9F918702\"> 42 9C 95 4A 38 59 CE F9 12 95 F6 63 C9 63 E5 82 ED 6E B2 53 </node>"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F06\"> A0 00 00 00 03 </node>"
		"				<node tag=\"0x9F22\"> 94 </node>"
		"				<node tag=\"0x9F918704\"> AC D2 B1 23 02 EE 64 4F 3F 83 5A BD 1F C7 A6 F6"
		"													2C CE 48 FF EC 62 2A A8 EF 06 2B EF 6F B8 BA 8B"
		"													C6 8B BF 6A B5 87 0E ED 57 9B C3 97 3E 12 13 03"
		"													D3 48 41 A7 96 D6 DC BC 41 DB F9 E5 2C 46 09 79"
		"													5C 0C CF 7E E8 6F A1 D5 CB 04 10 71 ED 2C 51 D2"
		"													20 2F 63 F1 15 6C 58 A9 2D 38 BC 60 BD F4 24 E1"
		"													77 6E 2B C9 64 80 78 A0 3B 36 FB 55 43 75 FC 53"
		"													D5 7C 73 F5 16 0E A5 9F 3A FC 53 98 EC 7B 67 75"
		"													8D 65 C9 BF F7 82 8B 6B 82 D4 BE 12 4A 41 6A B7"
		"													30 19 14 31 1E A4 62 C1 9F 77 1F 31 B3 B5 73 36"
		"													00 0D FF 73 2D 3B 83 DE 07 05 2D 73 03 54 D2 97"
		"													BE C7 28 71 DC CF 0E 19 3F 17 1A BA 27 EE 46 4C"
		"													6A 97 69 09 43 D5 9B DA BB 2A 27 EB 71 CE EB DA"
		"													FA 11 76 04 64 78 FD 62 FE C4 52 D5 CA 39 32 96"
		"													53 0A A3 F4 19 27 AD FE 43 4A 2D F2 AE 30 54 F8"
		"													84 06 57 A2 6E 0F C6 17 </node>"
		"				<node tag=\"0x9F918703\"> 03 </node>"
		"				<node tag=\"0x9F918702\"> C4 A3 C4 3C CF 87 32 7D 13 6B 80 41 60 E4 7D 43 B6 0E 6E 0F </node>"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F06\"> A0 00 00 00 03 </node>"
		"				<node tag=\"0x9F22\"> 95 </node>"
		"				<node tag=\"0x9F918704\"> BE 9E 1F A5 E9 A8 03 85 29 99 C4 AB 43 2D B2 86"
		"													00 DC D9 DA B7 6D FA AA 47 35 5A 0F E3 7B 15 08"
		"													AC 6B F3 88 60 D3 C6 C2 E5 B1 2A 3C AA F2 A7 00"
		"													5A 72 41 EB AA 77 71 11 2C 74 CF 9A 06 34 65 2F"
		"													BC A0 E5 98 0C 54 A6 47 61 EA 10 1A 11 4E 0F 0B"
		"													55 72 AD D5 7D 01 0B 7C 9C 88 7E 10 4C A4 EE 12"
		"													72 DA 66 D9 97 B9 A9 0B 5A 6D 62 4A B6 C5 7E 73"
		"													C8 F9 19 00 0E B5 F6 84 89 8E F8 C3 DB EF B3 30"
		"													C6 26 60 BE D8 8E A7 8E 90 9A FF 05 F6 DA 62 7B </node>"
		"				<node tag=\"0x9F918703\"> 03 </node>"
		"				<node tag=\"0x9F918702\"> EE 15 11 CE C7 10 20 A9 B9 04 43 B3 7B 1D 5F 6E 70 30 30 F6 </node>"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F06\"> A0 00 00 00 03 </node>"
		"				<node tag=\"0x9F22\"> 96 </node>"
		"				<node tag=\"0x9F918704\"> B7 45 86 D1 9A 20 7B E6 62 7C 5B 0A AF BC 44 A2"
		"													EC F5 A2 94 2D 3A 26 CE 19 C4 FF AE EE 92 05 21"
		"													86 89 22 E8 93 E7 83 82 25 A3 94 7A 26 14 79 6F"
		"													B2 C0 62 8C E8 C1 1E 38 25 A5 6D 3B 1B BA EF 78"
		"													3A 5C 6A 81 F3 6F 86 25 39 51 26 FA 98 3C 52 16"
		"													D3 16 6D 48 AC DE 8A 43 12 12 FF 76 3A 7F 79 D9"
		"													ED B7 FE D7 6B 48 5D E4 5B EB 82 9A 3D 47 30 84"
		"													8A 36 6D 33 24 C3 02 70 32 FF 8D 16 A1 E4 4D 8D </node>"
		"				<node tag=\"0x9F918703\"> 03 </node>"
		"				<node tag=\"0x9F918702\"> 76 16 E9 AC 8B E0 14 AF 88 CA 11 A8 FB 17 96 7B 73 94 03 0E </node>"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F06\"> A0 00 00 00 03 </node>"
		"				<node tag=\"0x9F22\"> 97 </node>"
		"				<node tag=\"0x9F918704\"> AF 07 54 EA ED 97 70 43 AB 6F 41 D6 31 2A B1 E2"
		"													2A 68 09 17 5B EB 28 E7 0D 5F 99 B2 DF 18 CA E7"
		"													35 19 34 1B BB D3 27 D0 B8 BE 9D 4D 0E 15 F0 7D"
		"													36 EA 3E 3A 05 C8 92 F5 B1 9A 3E 9D 34 13 B0 D9"
		"													7E 7A D1 0A 5F 5D E8 E3 88 60 C0 AD 00 4B 1E 06"
		"													F4 04 0C 29 5A CB 45 7A 78 85 51 B6 12 7C 0B 29 </node>"
		"				<node tag=\"0x9F918703\"> 03 </node>"
		"				<node tag=\"0x9F918702\"> 80 01 CA 76 C1 20 39 55 E2 C6 28 41 CD 6F 20 10 87 E5 64 BF </node>"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F06\"> A0 00 00 00 03 </node>"
		"				<node tag=\"0x9F22\"> 98 </node>"
		"				<node tag=\"0x9F918704\"> CA 02 6E 52 A6 95 E7 2B D3 0A F9 28 19 6E ED C9"
		"													FA F4 A6 19 F2 49 2E 3F B3 11 69 78 9C 27 6F FB"
		"													B7 D4 31 16 64 7B A9 E0 D1 06 A3 54 2E 39 65 29"
		"													2C F7 78 23 DD 34 CA 8E EC 7D E3 67 E0 80 70 89"
		"													50 77 C7 EF AD 93 99 24 CB 18 70 67 DB F9 2C B1"
		"													E7 85 91 7B D3 8B AC E0 C1 94 CA 12 DF 0C E5 B7"
		"													A5 02 75 AC 61 BE 7C 3B 43 68 87 CA 98 C9 FD 39 </node>"
		"				<node tag=\"0x9F918703\"> 03 </node>"
		"				<node tag=\"0x9F918702\"> E7 AC 9A A8 EE D1 B5 FF 1B D5 32 CF 14 89 A3 E5 55 75 72 C1 </node>"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F06\"> A0 00 00 00 03 </node>"
		"				<node tag=\"0x9F22\"> 99 </node>"
		"				<node tag=\"0x9F918704\"> AB 79 FC C9 52 08 96 96 7E 77 6E 64 44 4E 5D CD"
		"													D6 E1 36 11 87 4F 39 85 72 25 20 42 52 95 EE A4"
		"													BD 0C 27 81 DE 7F 31 CD 3D 04 1F 56 5F 74 73 06"
		"													EE D6 29 54 B1 7E DA BA 3A 6C 5B 85 A1 DE 1B EB"
		"													9A 34 14 1A F3 8F CF 82 79 C9 DE A0 D5 A6 71 0D"
		"													08 DB 41 24 F0 41 94 55 87 E2 03 59 BA B4 7B 75"
		"													75 AD 94 26 2D 4B 25 F2 64 AF 33 DE DC F2 8E 09"
		"													61 5E 93 7D E3 2E DC 03 C5 44 45 FE 7E 38 27 77 </node>"
		"				<node tag=\"0x9F918703\"> 03 </node>"
		"				<node tag=\"0x9F918702\"> 4A BF FD 6B 1C 51 21 2D 05 55 2E 43 1C 5B 17 00 7D 2F 5E 6D </node>"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F06\"> A0 00 00 00 04 </node>"
		"				<node tag=\"0x9F22\"> 00 </node>"
		"				<node tag=\"0x9F918704\"> 9E 15 21 42 12 F6 30 8A CA 78 B8 0B D9 86 AC 28"
		"													75 16 84 6C 8D 54 8A 9E D0 A4 2E 7D 99 7C 90 2C"
		"													3E 12 2D 1B 9D C3 09 95 F4 E2 5C 75 DD 7E E0 A0"
		"													CE 29 3B 8C C0 2B 97 72 78 EF 25 6D 76 11 94 92"
		"													47 64 94 2F E7 14 FA 02 E4 D5 7F 28 2B A3 B2 B6"
		"													2C 9E 38 EF 65 17 82 3F 2C A8 31 BD DF 6D 36 3D </node>"
		"				<node tag=\"0x9F918703\"> 03 </node>"
		"				<node tag=\"0x9F918702\"> 8B B9 9A DD F7 B5 60 11 09 55 01 45 05 FB 6B 5F 83 08 CE 27 </node>"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F06\"> A0 00 00 00 04 </node>"
		"				<node tag=\"0x9F22\"> 01 </node>"
		"				<node tag=\"0x9F918704\"> D2 01 07 16 C9 FB 52 64 D8 C9 1A 14 F4 F3 2F 89"
		"													81 EE 95 4F 20 08 7E D7 7C DC 58 68 43 17 28 D3"
		"													63 7C 63 2C CF 27 18 A4 F5 D9 2E A8 AB 16 6A B9"
		"													92 D2 DE 24 E9 FB DC 7C AB 97 29 40 1E 91 C5 02"
		"													D7 2B 39 F6 86 6F 5C 09 8B 12 43 B1 32 AF EE 65"
		"													F5 03 6E 16 83 23 11 63 38 F8 04 08 34 B9 87 25 </node>"
		"				<node tag=\"0x9F918703\"> 03 </node>"
		"				<node tag=\"0x9F918702\"> EA 95 0D D4 23 4F EB 7C 90 0C 0B E8 17 F6 4D E6 6E EE F7 C4 </node>"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F06\"> A0 00 00 00 04 </node>"
		"				<node tag=\"0x9F22\"> 02 </node>"
		"				<node tag=\"0x9F918704\"> CF 42 64 E1 70 2D 34 CA 89 7D 1F 9B 66 C5 D6 36"
		"													91 EA CC 61 2C 8F 14 71 16 BB 22 D0 C4 63 49 5B"
		"													D5 BA 70 FB 15 38 48 89 52 20 B8 AD EE C3 E7 BA"
		"													B3 1E A2 2C 1D C9 97 2F A0 27 D5 42 65 BE BF 0A"
		"													E3 A2 3A 8A 09 18 7F 21 C8 56 60 7B 98 BD A6 FC"
		"													90 81 16 81 6C 50 2B 3E 58 A1 45 25 4E EF EE 2A"
		"													33 35 11 02 24 02 8B 67 80 9D CB 80 58 E2 48 95 </node>"
		"				<node tag=\"0x9F918703\"> 03 </node>"
		"				<node tag=\"0x9F918702\"> AF 1C C1 FD 1C 1B C9 BC A0 7E 78 DA 6C BA 21 63 F1 69 CB B7 </node>"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F06\"> A0 00 00 00 04 </node>"
		"				<node tag=\"0x9F22\"> 03 </node>"
		"				<node tag=\"0x9F918704\"> C2 49 07 47 FE 17 EB 05 84 C8 8D 47 B1 60 27 04"
		"													15 0A DC 88 C5 B9 98 BD 59 CE 04 3E DE BF 0F FE"
		"													E3 09 3A C7 95 6A D3 B6 AD 45 54 C6 DE 19 A1 78"
		"													D6 DA 29 5B E1 5D 52 20 64 5E 3C 81 31 66 6F A4"
		"													BE 5B 84 FE 13 1E A4 4B 03 93 07 63 8B 9E 74 A8"
		"													C4 25 64 F8 92 A6 4D F1 CB 15 71 2B 73 6E 33 74"
		"													F1 BB B6 81 93 71 60 2D 89 70 E9 7B 90 07 93 C7"
		"													C2 A8 9A 4A 16 49 A5 9B E6 80 57 4D D0 B6 01 45 </node>"
		"				<node tag=\"0x9F918703\"> 03 </node>"
		"				<node tag=\"0x9F918702\"> 5A DD F2 1D 09 27 86 61 14 11 79 CB EF F2 72 EA 38 4B 13 BB </node>"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F06\"> A0 00 00 00 04 </node>"
		"				<node tag=\"0x9F22\"> 04 </node>"
		"				<node tag=\"0x9F918704\">"
		"													A6 DA 42 83 87 A5 02 D7 DD FB 7A 74 D3 F4 12 BE"
		"													76 26 27 19 7B 25 43 5B 7A 81 71 6A 70 01 57 DD"
		"													D0 6F 7C C9 9D 6C A2 8C 24 70 52 7E 2C 03 61 6B"
		"													9C 59 21 73 57 C2 67 4F 58 3B 3B A5 C7 DC F2 83"
		"													86 92 D0 23 E3 56 24 20 B4 61 5C 43 9C A9 7C 44"
		"													DC 9A 24 9C FC E7 B3 BF B2 2F 68 22 8C 3A F1 33"
		"													29 AA 4A 61 3C F8 DD 85 35 02 37 3D 62 E4 9A B2"
		"													56 D2 BC 17 12 0E 54 AE DC ED 6D 96 A4 28 7A CC"
		"													5C 04 67 7D 4A 5A 32 0D B8 BE E2 F7 75 E5 FE C5 </node>"
		"				<node tag=\"0x9F918703\"> 03 </node>"
		"				<node tag=\"0x9F918702\"> 38 1A 03 5D A5 8B 48 2E E2 AF 75 F4 C3 F2 CA 46 9B A4 AA 6C </node>"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F06\"> A0 00 00 00 04 </node>"
		"				<node tag=\"0x9F22\"> 05 </node>"
		"				<node tag=\"0x9F918704\"> B8 04 8A BC 30 C9 0D 97 63 36 54 3E 3F D7 09 1C"
		"													8F E4 80 0D F8 20 ED 55 E7 E9 48 13 ED 00 55 5B"
		"													57 3F EC A3 D8 4A F6 13 1A 65 1D 66 CF F4 28 4F"
		"													B1 3B 63 5E DD 0E E4 01 76 D8 BF 04 B7 FD 1C 7B"
		"													AC F9 AC 73 27 DF AA 8A A7 2D 10 DB 3B 8E 70 B2"
		"													DD D8 11 CB 41 96 52 5E A3 86 AC C3 3C 0D 9D 45"
		"													75 91 64 69 C4 E4 F5 3E 8E 1C 91 2C C6 18 CB 22"
		"													DD E7 C3 56 8E 90 02 2E 6B BA 77 02 02 E4 52 2A"
		"													2D D6 23 D1 80 E2 15 BD 1D 15 07 FE 3D C9 0C A3"
		"													10 D2 7B 3E FC CD 8F 83 DE 30 52 CA D1 E4 89 38"
		"													C6 8D 09 5A AC 91 B5 F3 7E 28 BB 49 EC 7E D5 97 </node>"
		"				<node tag=\"0x9F918703\"> 03 </node>"
		"				<node tag=\"0x9F918702\"> EB FA 0D 5D 06 D8 CE 70 2D A3 EA E8 90 70 1D 45 E2 74 C8 45 </node>"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F06\"> A0 00 00 00 04 </node>"
		"				<node tag=\"0x9F22\"> 06 </node>"
		"				<node tag=\"0x9F918704\">"
		"													CB 26 FC 83 0B 43 78 5B 2B CE 37 C8 1E D3 34 62"
		"													2F 96 22 F4 C8 9A AE 64 10 46 B2 35 34 33 88 3F"
		"													30 7F B7 C9 74 16 2D A7 2F 7A 4E C7 5D 9D 65 73"
		"													36 86 5B 8D 30 23 D3 D6 45 66 76 25 C9 A0 7A 6B"
		"													7A 13 7C F0 C6 41 98 AE 38 FC 23 80 06 FB 26 03"
		"													F4 1F 4F 3B B9 DA 13 47 27 0F 2F 5D 8C 60 6E 42"
		"													09 58 C5 F7 D5 0A 71 DE 30 14 2F 70 DE 46 88 89"
		"													B5 E3 A0 86 95 B9 38 A5 0F C9 80 39 3A 9C BC E4"
		"													4A D2 D6 4F 63 0B B3 3A D3 F5 F5 FD 49 5D 31 F3"
		"													78 18 C1 D9 40 71 34 2E 07 F1 BE C2 19 4F 60 35"
		"													BA 5D ED 39 36 50 0E B8 2D FD A6 E8 AF B6 55 B1"
		"													EF 3D 0D 7E BF 86 B6 6D D9 F2 9F 6B 1D 32 4F E8"
		"													B2 6C E3 8A B2 01 3D D1 3F 61 1E 7A 59 4D 67 5C"
		"													44 32 35 0E A2 44 CC 34 F3 87 3C BA 06 59 29 87"
		"													A1 D7 E8 52 AD C2 2E F5 A2 EE 28 13 20 31 E4 8F"
		"													74 03 7E 3B 34 AB 74 7F </node>"
		"				<node tag=\"0x9F918703\"> 03 </node>"
		"				<node tag=\"0x9F918702\"> F9 10 A1 50 4D 5F FB 79 3D 94 F3 B5 00 76 5E 1A BC AD 72 D9 </node>"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F06\"> A0 00 00 00 04 </node>"
		"				<node tag=\"0x9F22\"> 09 </node>"
		"				<node tag=\"0x9F918704\"> 96 7B 62 64 43 6C 96 AA 93 05 77 6A 59 19 C7 0D"
		"													A7 96 34 0F 99 97 A6 C6 EF 7B EF 1D 4D BF 9C B4"
		"													28 9F B7 99 0A BF F1 F3 AE 69 2F 12 84 4B 24 52"
		"													A5 0A E0 75 FB 32 79 76 A4 0E 80 28 F2 79 B1 E3"
		"													CC B6 23 95 7D 69 6F C1 22 5C A2 EC 95 0E 2D 41"
		"													5E 9A A9 31 FF 18 B1 31 68 D6 61 FB D0 6F 0A BB </node>"
		"				<node tag=\"0x9F918703\"> 03 </node>"
		"				<node tag=\"0x9F918702\"> 1D 90 59 5C 2E F9 FC 6E 71 B0 C7 21 11 83 33 DF 8A 71 FE 21 </node>"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F06\"> A0 00 00 00 04 </node>"
		"				<node tag=\"0x9F22\"> 22 </node>"
		"				<node tag=\"0x9F918704\"> BB E4 38 77 CC 28 C0 CE 1E 14 BC 14 E8 47 73 17"
		"													E2 18 36 45 31 D1 55 BB 8A C5 B6 3C 0D 6E 28 4D"
		"													D2 42 59 19 38 99 F9 C0 4C 30 BA F1 67 D5 79 29"
		"													45 1F 67 AE BD 3B BD 0D 41 44 45 01 84 7D 8F 02"
		"													F2 C2 A2 D1 48 17 D9 7A E2 62 5D C1 63 BF 8B 48"
		"													4C 40 FF B5 17 49 CE DD E9 43 4F B2 A0 A4 10 99 </node>"
		"				<node tag=\"0x9F918703\"> 03 </node>"
		"				<node tag=\"0x9F918702\"> 00 8C 39 B1 D1 19 49 82 68 B0 78 43 34 94 27 AC 6E 98 F8 07 </node>"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F06\"> A0 00 00 00 04 </node>"
		"				<node tag=\"0x9F22\"> 52 </node>"
		"				<node tag=\"0x9F918704\"> B8 31 41 4E 0B 46 13 92 2B D3 5B 4B 36 80 2B C1"
		"													E1 E8 1C 95 A2 7C 95 8F 53 82 00 3D F6 46 15 4C"
		"													A9 2F C1 CE 02 C3 BE 04 7A 45 E9 B0 2A 90 89 B4"
		"													B9 02 78 23 7C 96 51 92 A0 FC C8 6B B4 9B C8 2A"
		"													E6 FD C2 DE 70 90 06 B8 6C 76 76 EF DF 59 76 26"
		"													FA D6 33 A4 F7 DC 48 C4 45 D3 7E B5 5F CB 3B 1A"
		"													BB 95 BA AA 82 6D 53 90 E1 5F D1 4E D4 03 FA 2D"
		"													0C B8 41 C6 50 60 95 24 EC 55 5E 3B C5 6C A9 57 </node>"
		"				<node tag=\"0x9F918703\"> 01 00 01 </node>"
		"				<node tag=\"0x9F918702\"> DE B8 1E DB 26 26 A4 BB 6A E2 3B 77 D1 9A 77 53 9D 0E 67 16 </node>"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F06\"> A0 00 00 00 04 </node>"
		"				<node tag=\"0x9F22\"> EF </node>"
		"				<node tag=\"0x9F918704\"> A1 91 CB 87 47 3F 29 34 9B 5D 60 A8 8B 3E AE E0"
		"													97 3A A6 F1 A0 82 F3 58 D8 49 FD DF F9 C0 91 F8"
		"													99 ED A9 79 2C AF 09 EF 28 F5 D2 24 04 B8 8A 22"
		"													93 EE BB C1 94 9C 43 BE A4 D6 0C FD 87 9A 15 39"
		"													54 4E 09 E0 F0 9F 60 F0 65 B2 BF 2A 13 EC C7 05"
		"													F3 D4 68 B9 D3 3A E7 7A D9 D3 F1 9C A4 0F 23 DC"
		"													F5 EB 7C 04 DC 8F 69 EB A5 65 B1 EB CB 46 86 CD"
		"													27 47 85 53 0F F6 F6 E9 EE 43 AA 43 FD B0 2C E0"
		"													0D AE C1 5C 7B 8F D6 A9 B3 94 BA BA 41 9D 3F 6D"
		"													C8 5E 16 56 9B E8 E7 69 89 68 8E FE A2 DF 22 FF"
		"													7D 35 C0 43 33 8D EA A9 82 A0 2B 86 6D E5 32 85"
		"													19 EB BC D6 F0 3C DD 68 66 73 84 7F 84 DB 65 1A"
		"													B8 6C 28 CF 14 62 56 2C 57 7B 85 35 64 A2 90 C8"
		"													55 6D 81 85 31 26 8D 25 CC 98 A4 CC 6A 0B DF FF"
		"													DA 2D CC A3 A9 4C 99 85 59 E3 07 FD DF 91 50 06"
		"													D9 A9 87 B0 7D DA EB 3B </node>"
		"				<node tag=\"0x9F918703\"> 03 </node>"
		"				<node tag=\"0x9F918702\"> 21 76 6E BB 0E E1 22 AF B6 5D 78 45 B7 3D B4 6B AB 65 42 7A </node>"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F06\"> A0 00 00 00 04 </node>"
		"				<node tag=\"0x9F22\"> F0 </node>"
		"				<node tag=\"0x9F918704\"> 75 63 C5 1B 52 76 AA 63 70 AB 84 05 52 24 14 64"
		"													58 32 B6 BE F2 A9 89 C7 71 47 5B 2E 8D C6 54 DC"
		"													8A 5B FF 9E 28 E3 1F F1 A3 70 A4 0D C3 FF EB 06"
		"													BC 85 48 7D 5F 1C B6 1C 24 41 FD 71 CB CD 05 D8"
		"													83 F8 DE 41 3B 24 3A FC 9D CA 76 8B 06 1E 35 B8"
		"													84 B5 D2 1B 6B 01 6A A3 6B A1 2D AB CF E4 9F 8E"
		"													52 8C 89 3C 34 C7 D4 79 39 77 E4 CC 99 AB 09 64"
		"													0D 9C 7A AB 7E C5 FF 3F 40 E3 D4 D1 8D F7 E3 A7 </node>"
		"				<node tag=\"0x9F918703\"> 03 </node>"
		"				<node tag=\"0x9F918702\"> AE 66 74 45 F8 DE 6F 82 C3 88 00 E5 EB AB A3 22 F0 3F 58 F2 </node>"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F06\"> A0 00 00 00 04 </node>"
		"				<node tag=\"0x9F22\"> F1 </node>"
		"				<node tag=\"0x9F918704\"> A0 DC F4 BD E1 9C 35 46 B4 B6 F0 41 4D 17 4D DE"
		"													29 4A AB BB 82 8C 5A 83 4D 73 AA E2 7C 99 B0 B0"
		"													53 A9 02 78 00 72 39 B6 45 9F F0 BB CD 7B 4B 9C"
		"													6C 50 AC 02 CE 91 36 8D A1 BD 21 AA EA DB C6 53"
		"													47 33 7D 89 B6 8F 5C 99 A0 9D 05 BE 02 DD 1F 8C"
		"													5B A2 0E 2F 13 FB 2A 27 C4 1D 3F 85 CA D5 CF 66"
		"													68 E7 58 51 EC 66 ED BF 98 85 1F D4 E4 2C 44 C1"
		"													D5 9F 59 84 70 3B 27 D5 B9 F2 1B 8F A0 D9 32 79"
		"													FB BF 69 E0 90 64 29 09 C9 EA 27 F8 98 95 95 41"
		"													AA 67 57 F5 F6 24 10 4F 6E 1D 3A 95 32 F2 A6 E5"
		"													15 15 AE AD 1B 43 B3 D7 83 50 88 A2 FA FA 7B E7 </node>"
		"				<node tag=\"0x9F918703\"> 03 </node>"
		"				<node tag=\"0x9F918702\"> D8 E6 8D A1 67 AB 5A 85 D8 C3 D5 5E CB 9B 05 17 A1 A5 B4 BB </node>"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F06\"> A0 00 00 00 04 </node>"
		"				<node tag=\"0x9F22\"> F3 </node>"
		"				<node tag=\"0x9F918704\"> 98 F0 C7 70 F2 38 64 C2 E7 66 DF 02 D1 E8 33 DF"
		"													F4 FF E9 2D 69 6E 16 42 F0 A8 8C 56 94 C6 47 9D"
		"													16 DB 15 37 BF E2 9E 4F DC 6E 6E 8A FD 1B 0E B7"
		"													EA 01 24 72 3C 33 31 79 BF 19 E9 3F 10 65 8B 2F"
		"													77 6E 82 9E 87 DA ED A9 C9 4A 8B 33 82 19 9A 35"
		"													0C 07 79 77 C9 7A FF 08 FD 11 31 0A C9 50 A7 2C"
		"													3C A5 00 2E F5 13 FC CC 28 6E 64 6E 3C 53 87 53"
		"													5D 50 95 14 B3 B3 26 E1 23 4F 9C B4 8C 36 DD D4"
		"													4B 41 6D 23 65 40 34 A6 6F 40 3B A5 11 C5 EF A3 </node>"
		"				<node tag=\"0x9F918703\"> 03 </node>"
		"				<node tag=\"0x9F918702\"> A6 9A C7 60 3D AF 56 6E 97 2D ED C2 CB 43 3E 07 E8 B0 1A 9A </node>"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F06\"> A0 00 00 00 04 </node>"
		"				<node tag=\"0x9F22\"> F5 </node>"
		"				<node tag=\"0x9F918704\"> A6 E6 FB 72 17 95 06 F8 60 CC CA 8C 27 F9 9C EC"
		"													D9 4C 7D 4F 31 91 D3 03 BB EE 37 48 1C 7A A1 5F"
		"													23 3B A7 55 E9 E4 37 63 45 A9 A6 7E 79 94 BD C1"
		"													C6 80 BB 35 22 D8 C9 3E B0 CC C9 1A D3 1A D4 50"
		"													DA 30 D3 37 66 2D 19 AC 03 E2 B4 EF 5F 6E C1 82"
		"													82 D4 91 E1 97 67 D7 B2 45 42 DF DE FF 6F 62 18"
		"													55 03 53 20 69 BB B3 69 E3 BB 9F B1 9A C6 F1 C3"
		"													0B 97 D2 49 EE E7 64 E0 BA C9 7F 25 C8 73 D9 73"
		"													95 3E 51 53 A4 20 64 BB FA BF D0 6A 4B B4 86 86"
		"													0B F6 63 74 06 C9 FC 36 81 3A 4A 75 F7 5C 31 CC"
		"													A9 F6 9F 8D E5 9A DE CE F6 BD E7 E0 78 00 FC BE"
		"													03 5D 31 76 AF 84 73 E2 3E 9A A3 DF EE 22 11 96"
		"													D1 14 83 02 67 7C 72 0C FE 25 44 A0 3D B5 53 E7"
		"													F1 B8 42 7B A1 CC 72 B0 F2 9B 12 DF EF 4C 08 1D"
		"													07 6D 35 3E 71 88 0A AD FF 38 63 52 AF 0A B7 B2"
		"													8E D4 9E 1E 67 2D 11 F9 </node>"
		"				<node tag=\"0x9F918703\"> 01 00 01 </node>"
		"				<node tag=\"0x9F918702\"> C2 23 98 04 C8 09 81 70 BE 52 D6 D5 D4 15 9E 81 CE 84 66 BF </node>"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F06\"> A0 00 00 00 04 </node>"
		"				<node tag=\"0x9F22\"> F6 </node>"
		"				<node tag=\"0x9F918704\"> A2 5A 6B D7 83 A5 EF 6B 8F B6 F8 30 55 C2 60 F5"
		"													F9 9E A1 66 78 F3 B9 05 3E 0F 64 98 E8 2C 3F 5D"
		"													1E 8C 38 F1 35 88 01 7E 2B 12 B3 D8 FF 6F 50 16"
		"													7F 46 44 29 10 72 9E 9E 4D 1B 37 39 E5 06 7C 0A"
		"													C7 A1 F4 48 7E 35 F6 75 BC 16 E2 33 31 51 65 CB"
		"													14 2B FD B2 5E 30 1A 63 2A 54 A3 37 1E BA B6 57"
		"													2D EE BA F3 70 F3 37 F0 57 EE 73 B4 AE 46 D1 A8"
		"													BC 4D A8 53 EC 3C C1 2C 8C BC 2D A1 83 22 D6 85"
		"													30 C7 0B 22 BD AC 35 1D D3 60 68 AE 32 1E 11 AB"
		"													F2 64 F4 D3 56 9B B7 12 14 54 50 05 55 8D E2 60"
		"													83 C7 35 DB 77 63 68 17 2F E8 C2 F5 C8 5E 8B 5B"
		"													89 0C C6 82 91 1D 2D E7 1F A6 26 B8 81 7F CC C0"
		"													89 22 B7 03 86 9F 3B AE AC 14 59 D7 7C D8 53 76"
		"													BC 36 18 2F 42 38 31 4D 6C 42 12 FB DD 7F 23 D3 </node>"
		"				<node tag=\"0x9F918703\"> 03 </node>"
		"				<node tag=\"0x9F918702\"> 50 29 09 ED 54 5E 3C 8D BD 00 EA 58 2D 06 17 FE E9 F6 F6 84 </node>"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F06\"> A0 00 00 00 04 </node>"
		"				<node tag=\"0x9F22\"> F7 </node>"
		"				<node tag=\"0x9F918704\"> 94 EA 62 F6 D5 83 20 E3 54 C0 22 AD DC F0 55 9D"
		"													8C F2 06 CD 92 E8 69 56 49 05 CE 21 D7 20 F9 71"
		"													B7 AE A3 74 83 0E BE 17 57 11 5A 85 E0 88 D4 1C"
		"													6B 77 CF 5E C8 21 F3 0B 1D 89 04 17 BF 2F A3 1E"
		"													59 08 DE D5 FA 67 7F 8C 7B 18 4A D0 90 28 FD DE"
		"													96 B6 A6 10 98 50 AA 80 01 75 EA BC DB BB 68 4A"
		"													96 C2 EB 63 79 DF EA 08 D3 2F E2 33 1F E1 03 23"
		"													3A D5 8D CD B1 E6 E0 77 CB 9F 24 EA EC 5C 25 AF </node>"
		"				<node tag=\"0x9F918703\"> 01 00 01 </node>"
		"				<node tag=\"0x9F918702\"> EE B0 DD 9B 24 77 BE E3 20 9A 91 4C DB A9 4C 1C 4A 9B DE D9 </node>"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F06\"> A0 00 00 00 04 </node>"
		"				<node tag=\"0x9F22\"> F8 </node>"
		"				<node tag=\"0x9F918704\"> A1 F5 E1 C9 BD 86 50 BD 43 AB 6E E5 6B 89 1E F7"
		"													45 9C 0A 24 FA 84 F9 12 7D 1A 6C 79 D4 93 0F 6D"
		"													B1 85 2E 25 10 F1 8B 61 CD 35 4D B8 3A 35 6B D1"
		"													90 B8 8A B8 DF 04 28 4D 02 A4 20 4A 7B 6C B7 C5"
		"													55 19 77 A9 B3 63 79 CA 3D E1 A0 8E 69 F3 01 C9"
		"													5C C1 C2 05 06 95 92 75 F4 17 23 DD 5D 29 25 29"
		"													05 79 E5 A9 5B 0D F6 32 3F C8 E9 27 3D 6F 84 91"
		"													98 C4 99 62 09 16 6D 9B FC 97 3C 36 1C C8 26 E1 </node>"
		"				<node tag=\"0x9F918703\"> 03 </node>"
		"				<node tag=\"0x9F918702\"> F0 6E CC 6D 2A AE BF 25 9B 7E 75 5A 38 D9 A9 B2 4E 2F F3 DD </node>"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F06\"> A0 00 00 00 04 </node>"
		"				<node tag=\"0x9F22\"> F9 </node>"
		"				<node tag=\"0x9F918704\"> A9 9A 6D 3E 07 18 89 ED 9E 3A 0C 39 1C 69 B0 B8"
		"													04 FC 16 0B 2B 4B DD 57 0C 92 DD 5A 0F 45 F5 3E"
		"													86 21 F7 C9 6C 40 22 42 66 73 5E 1E E1 B3 C0 62"
		"													38 AE 35 04 63 20 FD 8E 81 F8 CE B3 F8 B4 C9 7B"
		"													94 09 30 A3 AC 5E 79 00 86 DA D4 1A 6A 4F 51 17"
		"													BA 1C E2 43 8A 51 AC 05 3E B0 02 AE D8 66 D2 C4"
		"													58 FD 73 35 90 21 A1 20 29 A0 C0 43 04 5C 11 66"
		"													4F E0 21 9E C6 3C 10 BF 21 55 BB 27 84 60 9A 10"
		"													64 21 D4 51 63 79 97 38 C1 C3 09 09 BB 6C 6F E5"
		"													2B BB 76 39 7B 97 40 CE 06 4A 61 3F F8 41 11 85"
		"													F0 88 42 A4 23 EA D2 0E DF FB FF 1C D6 C3 FE 0C"
		"													98 21 47 91 99 C2 6D 85 72 CC 8A FF F0 87 A9 C3 </node>"
		"				<node tag=\"0x9F918703\"> 03 </node>"
		"				<node tag=\"0x9F918702\"> 33 67 12 DC C2 85 54 80 9C 6A A9 B0 23 58 DE 6F 75 51 64 DB </node>"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F06\"> A0 00 00 00 04 </node>"
		"				<node tag=\"0x9F22\"> FA </node>"
		"				<node tag=\"0x9F918704\"> 9C 6B E5 AD B1 0B 4B E3 DC E2 09 9B 4B 21 06 72"
		"													B8 96 56 EB A0 91 20 4F 61 3E CC 62 3B ED C9 C6"
		"													D7 7B 66 0E 8B AE EA 7F 7C E3 0F 1B 15 38 79 A4"
		"													E3 64 59 34 3D 1F E4 7A CD BD 41 FC D7 10 03 0C"
		"													2B A1 D9 46 15 97 98 2C 6E 1B DD 08 55 4B 72 6F"
		"													5E FF 79 13 CE 59 E7 9E 35 72 95 C3 21 E2 6D 0B"
		"													8B E2 70 A9 44 23 45 C7 53 E2 AA 2A CF C9 D3 08"
		"													50 60 2F E6 CA C0 0C 6D DF 6B 8D 9D 9B 48 79 B2"
		"													82 6B 04 2A 07 F0 E5 AE 52 6A 3D 3C 4D 22 C7 2B"
		"													9E AA 52 EE D8 89 38 66 F8 66 38 7A C0 5A 13 99 </node>"
		"				<node tag=\"0x9F918703\"> 03 </node>"
		"				<node tag=\"0x9F918702\"> 0A BC AD AD 2C 75 58 CA 9C 70 81 AE 55 DD DC 71 4F 8D 45 F8 </node>"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F06\"> A0 00 00 00 04 </node>"
		"				<node tag=\"0x9F22\"> FB </node>"
		"				<node tag=\"0x9F918704\"> A9 54 8D FB 39 8B 48 12 3F AF 41 E6 CF A4 AE 1E"
		"													23 52 B5 18 AB 4B CE FE CD B0 B3 ED EC 09 02 87"
		"													D8 8B 12 25 9F 36 1C 1C C0 88 E5 F0 66 49 44 17"
		"													E8 EE 8B BF 89 91 E2 B3 2F F1 6F 99 46 97 84 2B"
		"													3D 6C B3 7A 2B B5 74 2A 44 0B 63 56 C6 2A A3 3D"
		"													B3 C4 55 E5 9E DD F7 86 47 01 D0 3A 5B 83 EE 9E"
		"													9B D8 3A B9 33 02 AC 2D FE 63 E6 61 20 B0 51 CF"
		"													08 1F 56 32 6A 71 30 3D 95 2B B3 36 FF 12 61 0D  </node>"
		"				<node tag=\"0x9F918703\"> 03 </node>"
		"				<node tag=\"0x9F918702\"> 6C 72 89 63 29 19 AB EE 6E 11 63 D7 E6 BF 69 3F D8 8E BD 35 </node>"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F06\"> A0 00 00 00 04 </node>"
		"				<node tag=\"0x9F22\"> FE </node>"
		"				<node tag=\"0x9F918704\"> A6 53 EA C1 C0 F7 86 C8 72 4F 73 7F 17 29 97 D6"
		"													3D 1C 32 51 C4 44 02 04 9B 86 5B AE 87 7D 0F 39"
		"													8C BF BE 8A 60 35 E2 4A FA 08 6B EF DE 93 51 E5"
		"													4B 95 70 8E E6 72 F0 96 8B CD 50 DC E4 0F 78 33"
		"													22 B2 AB A0 4E F1 37 EF 18 AB F0 3C 7D BC 58 13"
		"													AE AE F3 AA 77 97 BA 15 DF 7D 5B A1 CB AF 7F D5"
		"													20 B5 A4 82 D8 D3 FE E1 05 07 78 71 11 3E 23 A4"
		"													9A F3 92 65 54 A7 0F E1 0E D7 28 CF 79 3B 62 A1 </node>"
		"				<node tag=\"0x9F918703\"> 03 </node>"
		"				<node tag=\"0x9F918702\"> 9A 29 5B 05 FB 39 0E F7 92 3F 57 61 8A 9F DA 29 41 FC 34 E0 </node>"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F06\"> A0 00 00 00 04 </node>"
		"				<node tag=\"0x9F22\"> FF </node>"
		"				<node tag=\"0x9F918704\"> B8 55 CC 64 31 3A F9 9C 45 3D 18 16 42 EE 7D D2"
		"													1A 67 D0 FF 50 C6 1F E2 13 BC DC 18 AF BC D0 77"
		"													22 EF DD 25 94 EF DC 22 7D A3 DA 23 AD CC 90 E3"
		"													FA 90 74 53 AC C9 54 C4 73 23 BE DC F8 D4 86 2C"
		"													45 7D 25 F4 7B 16 D7 C3 50 2B E0 81 91 3E 5B 04"
		"													82 D8 38 48 40 65 DA 5F 66 59 E0 0A 9E 5D 57 0A"
		"													DA 1E C6 AF 8C 57 96 00 75 11 95 81 FC 81 46 8D  </node>"
		"				<node tag=\"0x9F918703\"> 03 </node>"
		"				<node tag=\"0x9F918702\"> B4 E7 69 CE CF 7A AC 47 83 F3 05 E0 B1 10 60 2A 07 A6 35 5B </node>"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F06\"> A0 00 00 01 52 </node>"
		"				<node tag=\"0x9F22\"> 5B </node>"
		"				<node tag=\"0x9F918704\"> D3 F4 5D 06 5D 4D 90 0F 68 B2 12 9A FA 38 F5 49"
		"													AB 9A E4 61 9E 55 45 81 4E 46 8F 38 20 49 A0 B9"
		"													77 66 20 DA 60 D6 25 37 F0 70 5A 2C 92 6D BE AD"
		"													4C A7 CB 43 F0 F0 DD 80 95 84 E9 F7 EF BD A3 77"
		"													87 47 BC 9E 25 C5 60 65 26 FA B5 E4 91 64 6D 4D"
		"													D2 82 78 69 1C 25 95 6C 8F ED 5E 45 2F 24 42 E2"
		"													5E DC 6B 0C 1A A4 B2 E9 EC 4A D9 B2 5A 1B 83 62"
		"													95 B8 23 ED DC 5E B6 E1 E0 A3 F4 1B 28 DB 8C 3B"
		"													7E 3E 9B 59 79 CD 7E 07 9E F0 24 09 5A 1D 19 DD </node>"
		"				<node tag=\"0x9F918703\"> 03 </node>"
		"				<node tag=\"0x9F918702\"> 4D C5 C6 CA B6 AE 96 97 4D 9D C8 B2 43 5E 21 F5 26 BC 7A 60 </node>"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F06\"> A0 00 00 01 52 </node>"
		"				<node tag=\"0x9F22\"> 5C </node>"
		"				<node tag=\"0x9F918704\"> 83 3F 27 5F CF 5C A4 CB 6F 1B F8 80 E5 4D CF EB"
		"				                        72 1A 31 66 92 CA FE B2 8B 69 8C AE CA FA 2B 2D"
		"				                        2A D8 51 7B 1E FB 59 DD EF C3 9F 9C 3B 33 DD EE"
		"				                        40 E7 A6 3C 03 E9 0A 4D D2 61 BC 0F 28 B4 2E A6"
		"				                        E7 A1 F3 07 17 8E 2D 63 FA 16 49 15 5C 3A 5F 92"
		"				                        6B 4C 7D 7C 25 8B CA 98 EF 90 C7 F4 11 7C 20 5E"
		"				                        8E 32 C4 5D 10 E3 D4 94 05 9D 2F 29 33 89 1B 97"
		"				                        9C E4 A8 31 B3 01 B0 55 0C DA E9 B6 70 64 B3 1D"
		"				                        8B 48 1B 85 A5 B0 46 BE 8F FA 7B DB 58 DC 0D 70"
		"				                        32 52 52 97 F2 6F F6 19 AF 7F 15 BC EC 0C 92 BC"
		"				                        DC BC 4F B2 07 D1 15 AA 65 CD 04 C1 CF 98 21 91 </node>"
		"				<node tag=\"0x9F918703\"> 03 </node>"
		"				<node tag=\"0x9F918702\"> 60 15 40 98 CB BA 35 0F 5F 48 6C A3 10 83 D1 FC 47 4E 31 F8 </node>"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F06\"> A0 00 00 01 52 </node>"
		"				<node tag=\"0x9F22\"> 5D </node>"
		"				<node tag=\"0x9F918704\"> AD 93 8E A9 88 8E 51 55 F8 CD 27 27 49 17 2B 3A"
		"				                        8C 50 4C 17 46 0E FA 0B ED 7C BC 5F D3 2C 4A 80"
		"				                        FD 81 03 12 28 1B 5A 35 56 28 00 CD C3 25 35 8A"
		"				                        96 39 C5 01 A5 37 B7 AE 43 DF 26 3E 6D 23 2B 81"
		"				                        1A CD B6 DD E9 79 D5 5D 6C 91 11 73 48 39 93 A4"
		"				                        23 A0 A5 B1 E1 A7 02 37 88 5A 24 1B 8E EB B5 57"
		"				                        1E 2D 32 B4 1F 9C C5 51 4D F8 3F 0D 69 27 0E 10"
		"				                        9A F1 42 2F 98 5A 52 CC E0 4F 3D F2 69 B7 95 15"
		"				                        5A 68 AD 2D 6B 66 0D DC D7 59 F0 A5 DA 7B 64 10"
		"				                        4D 22 C2 77 1E CE 7A 5F FD 40 C7 74 E4 41 37 9D"
		"				                        11 32 FA F0 4C DF 55 B9 50 4C 6D CE 9F 61 77 6D"
		"				                        81 C7 C4 5F 19 B9 EF B3 74 9A C7 D4 86 A5 AD 2E"
		"				                        78 1F A9 D0 82 FB 26 77 66 5B 99 FA 5F 15 53 13"
		"				                        5A 1F D2 A2 A9 FB F6 25 CA 84 A7 D7 36 52 14 31"
		"				                        17 8F 13 10 0A 25 16 F9 A4 3C E0 95 B0 32 B8 86"
		"				                        C7 A6 AB 12 6E 20 3B E7 </node>"
		"				<node tag=\"0x9F918703\"> 03 </node>"
		"				<node tag=\"0x9F918702\"> B5 1E C5 F7 DE 9B B6 D8 BC E8 FB 5F 69 BA 57 A0 42 21 F3 9B </node>"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F06\"> D9 99 99 99 99 </node>"
		"				<node tag=\"0x9F22\"> 01 </node>"
		"				<node tag=\"0x9F918704\"> C4 BC 12 80 1C 97 5A 0F 13 F8 12 90 D9 53 31 3B"
		"			                          B0 AC B3 41 45 8E 8A 07 C7 7D DE 0D A5 58 C2 78"
		"			                          86 FE 29 C2 89 C6 E3 48 0F 6C 6B 39 56 3F 1E 63"
		"			                          E1 7C 2E 80 C7 A4 C1 6C C9 E9 BD BC 5A 9B B5 80"
		"			                          6E E9 83 05 96 43 C5 35 1A 99 0A 69 38 F6 16 D8"
		"			                          28 E7 CB 86 50 16 B1 6C F6 80 90 87 D5 68 C7 87"
		"			                          4D BA 67 78 56 00 B6 4F 1E 14 81 87 DC 75 BC 3F"
		"			                          D3 0F DA 63 C5 FC 9E 91 C6 95 8C E2 98 6C D0 FA"
		"			                          33 54 D6 2A E0 0E 5C DD 14 4C 22 0A 22 8D A7 C9"
		"			                          48 91 8D EE 92 AD 3D B6 93 26 60 08 65 A4 22 99"
		"			                          63 E3 FB 94 D9 E7 9A C4 35 0A C9 ED 44 27 4E BD  </node>"
		"				<node tag=\"0x9F918703\"> 03 </node>"
		"				<node tag=\"0x9F918702\"> 86 47 3A B7 71 60 D0 B2 DA B6 BB 17 23 6B 8B 37 E2 59 D3 50 </node>"
		"			</node>"
		""
		"			<!-- UPI_CAKEYS_PARAMETERS-->"
		""
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F06\"> A0 00 00 03 33 </node>"
		"				<node tag=\"0x9F22\"> 01 </node> <!-- CAKEYS_INDEX -->"
		"				<node tag=\"0x9F918704\"> BB E9 06 6D 25 17 51 1D 23 9C 7B FA 77 88 41 44"
		"										AE 20 C7 37 2F 51 51 47 E8 CE 65 37 C5 4C 0A 6A"
		"										4D 45 F8 CA 4D 29 08 70 CD A5 9F 13 44 EF 71 D1"
		"										7D 3F 35 D9 2F 3F 06 77 8D 0D 51 1E C2 A7 DC 4F"
		"										FE AD F4 FB 12 53 CE 37 A7 B2 B5 A3 74 12 27 BE"
		"										F7 25 24 DA 7A 2B 7B 1C B4 26 BE E2 7B C5 13 B0"
		"										CB 11 AB 99 BC 1B C6 1D F5 AC 6C C4 D8 31 D0 84"
		"										87 88 CD 74 F6 D5 43 AD 37 C5 A2 B4 C5 D5 A9 3B  </node>"
		"				<node tag=\"0x9F918703\"> 03 </node> <!-- CAKEYS_EXPONENT -->"
		"				<node tag=\"0x9F918702\"> E8 81 E3 90 67 5D 44 C2 DD 81 23 4D CE 29 C3 F5 AB 22 97 A0 </node>"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F06\"> A0 00 00 03 33 </node>"
		"				<node tag=\"0x9F22\"> 02 </node> <!-- CAKEYS_INDEX -->"
		"				<node tag=\"0x9F918704\"> A3 76 7A BD 1B 6A A6 9D 7F 3F BF 28 C0 92 DE 9E"
		"										D1 E6 58 BA 5F 09 09 AF 7A 1C CD 90 73 73 B7 21"
		"										0F DE B1 62 87 BA 8E 78 E1 52 9F 44 39 76 FD 27"
		"										F9 91 EC 67 D9 5E 5F 4E 96 B1 27 CA B2 39 6A 94"
		"										D6 E4 5C DA 44 CA 4C 48 67 57 0D 6B 07 54 2F 8D"
		"										4B F9 FF 97 97 5D B9 89 15 15 E6 6F 52 5D 2B 3C"
		"										BE B6 D6 62 BF B6 C3 F3 38 E9 3B 02 14 2B FC 44"
		"										17 3A 37 64 C5 6A AD D2 02 07 5B 26 DC 2F 9F 7D"
		"										7A E7 4B D7 D0 0F D0 5E E4 30 03 26 63 D2 7A 57   </node>"
		"				<node tag=\"0x9F918703\"> 03 </node> <!-- CAKEYS_EXPONENT -->"
		"				<node tag=\"0x9F918702\"> 03 BB 33 5A 85 49 A0 3B 87 AB 08 9D 00 6F 60 85 2E 4B 80 60 </node>"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F06\"> A0 00 00 03 33 </node>"
		"				<node tag=\"0x9F22\"> 03 </node> <!-- CAKEYS_INDEX -->"
		"				<node tag=\"0x9F918704\"> B0 62 7D EE 87 86 4F 9C 18 C1 3B 9A 1F 02 54 48"
		"										BF 13 C5 83 80 C9 1F 4C EB A9 F9 BC B2 14 FF 84"
		"										14 E9 B5 9D 6A BA 10 F9 41 C7 33 17 68 F4 7B 21"
		"										27 90 7D 85 7F A3 9A AF 8C E0 20 45 DD 01 61 9D"
		"										68 9E E7 31 C5 51 15 9B E7 EB 2D 51 A3 72 FF 56"
		"										B5 56 E5 CB 2F DE 36 E2 30 73 A4 4C A2 15 D6 C2"
		"										6C A6 88 47 B3 88 E3 95 20 E0 02 6E 62 29 4B 55"
		"										7D 64 70 44 0C A0 AE FC 94 38 C9 23 AE C9 B2 09"
		"										8D 6D 3A 1A F5 E8 B1 DE 36 F4 B5 30 40 10 9D 89"
		"										B7 7C AF AF 70 C2 6C 60 1A BD F5 9E EC 0F DC 8A"
		"										99 08 91 40 CD 2E 81 7E 33 51 75 B0 3B 7A A3 3D    </node>"
		"				<node tag=\"0x9F918703\"> 03 </node> <!-- CAKEYS_EXPONENT -->"
		"				<node tag=\"0x9F918702\"> 87 F0 CD 7C 0E 86 F3 8F 89 A6 6F 8C 47 07 1A 8B 88 58 6F 26  </node>"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F06\"> A0 00 00 03 33 </node>"
		"				<node tag=\"0x9F22\"> 04 </node> <!-- CAKEYS_INDEX -->"
		"				<node tag=\"0x9F918704\"> BC 85 3E 6B 53 65 E8 9E 7E E9 31 7C 94 B0 2D 0A"
		"										BB 0D BD 91 C0 5A 22 4A 25 54 AA 29 ED 9F CB 9D"
		"										86 EB 9C CB B3 22 A5 78 11 F8 61 88 AA C7 35 1C"
		"										72 BD 9E F1 96 C5 A0 1A CE F7 A4 EB 0D 2A D6 3D"
		"										9E 6A C2 E7 83 65 47 CB 15 95 C6 8B CB AF D0 F6"
		"										72 87 60 F3 A7 CA 7B 97 30 1B 7E 02 20 18 4E FC"
		"										4F 65 30 08 D9 3C E0 98 C0 D9 3B 45 20 10 96 D1"
		"										AD FF 4C F1 F9 FC 02 AF 75 9D A2 7C D6 DF D6 D7"
		"										89 B0 99 F1 6F 37 8B 61 00 33 4E 63 F3 D3 5F 32"
		"										51 A5 EC 78 69 37 31 F5 23 35 19 CD B3 80 F5 AB"
		"										8C 0F 02 72 8E 91 D4 69 AB D0 EA E0 D9 3B 1C C6"
		"										6C E1 27 B2 9C 7D 77 44 1A 49 D0 9F CA 5D 6D 97"
		"										62 FC 74 C3 1B B5 06 C8 BA E3 C7 9A D6 C2 57 87"
		"										75 B9 59 56 B5 37 0D 1D 05 19 E3 79 06 B3 84 73"
		"										62 33 25 1E 8F 09 AD 79 DF BE 2C 6A BF AD AC 8E"
		"										4D 86 24 31 8C 27 DA F1    </node>"
		"				<node tag=\"0x9F918703\"> 03 </node> <!-- CAKEYS_EXPONENT -->"
		"				<node tag=\"0x9F918702\"> F5 27 08 1C F3 71 DD 7E 1F D4 FA 41 4A 66 50 36 E0 F5 E6 E5 </node>"
		"			</node>"
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F06\"> A0 00 00 03 33 </node>"
		"				<node tag=\"0x9F22\"> 61 </node>"
		"				<node tag=\"0x9F918704\"> A3 B0 36 FC 94 60 A0 7B 31 FC B0 B3 14 37 FE D7"
		"					                        6A 8E F0 73 30 EE 0E BE 86 B9 C8 F7 2F F4 19 A1"
		"											D4 62 F3 25 F6 2E 77 95 AF D3 F1 17 4A 3D 49 65"
		"											81 D9 AE 81 2D CD 11 60 21 57 6C CE B6 09 2E 1A"
		"											8D 24 B4 C5 AD 80 98 5D EB 63 F7 48 C0 C0 AA 93"
		"											B7 D5 17 B0 BA C6 4D 05 7D 48 E8 E0 42 AC 0B A9"
		"											50 60 27 D4 59 F8 DF 2B 42 74 CD 14 CA 12 64 A5"
		"											C4 40 F8 68 BB 2B C3 26 49 73 16 AC 24 F7 08 43 </node>"
		"				<node tag=\"0x9F918703\"> 03 </node>"
		"				<node tag=\"0x9F918702\"> 61 BD CB 08 78 51 5D D9 89 9E 64 47 F1 AC 4C B8 E1 94 1B 68 </node>"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F06\"> A0 00 00 03 33 </node>"
		"				<node tag=\"0x9F22\"> 62 </node>"
		"				<node tag=\"0x9F918704\"> 9C 2D AB 17 1C A2 59 F7 35 F8 BF 0D EE 24 6C 14"
		"					                        69 13 FE 7F 78 15 2B 81 EB 61 08 51 0E 17 9C 09"
		"											12 E1 16 93 D2 BD AF 26 B4 73 70 4D 63 E1 B7 D2"
		"											70 42 BE 15 E6 06 27 69 1C 58 F3 67 11 57 66 BC"
		"											DC DE 2F BA D2 22 1E 97 9C E0 4B 02 79 B6 9E B6"
		"											B9 9B A0 9E BB 4B CF 62 32 C9 2D C8 1A C5 47 EE"
		"											BE 88 02 09 D7 B5 2B 14 25 37 4B 56 10 DB CD B7"
		"											E5 F8 21 27 12 E1 C8 94 20 3F DE 1A 6E 29 BE 35 </node>"
		"				<node tag=\"0x9F918703\"> 03 </node>"
		"				<node tag=\"0x9F918702\"> 1D F4 AE 4F A6 96 1D D3 15 07 2D D3 03 4C CC 81 5C E4 E1 07 </node>"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F06\"> A0 00 00 03 33 </node>"
		"				<node tag=\"0x9F22\"> 63 </node>"
		"				<node tag=\"0x9F918704\"> 98 A2 24 31 3A CB 0C 8E 6F 79 F3 79 78 E2 F1 53"
		"					                        94 3D 7E B7 1B 08 79 CE 82 0D 53 46 92 D4 12 AA"
		"											A8 1E 98 62 18 C0 FE 48 50 0A AF 75 53 F2 2E 52"
		"											5A 00 B7 9A 58 1A CB 0B B6 AA 7A 1C 54 31 3E F7"
		"											61 76 25 3E 76 8F A0 F8 D0 8F 57 57 3D 54 A9 E7"
		"											E0 B1 A0 5F 74 1E 36 55 CA 2E D0 7F 54 FC C9 77"
		"											78 1B 9A DF 63 95 89 E2 2F 67 F4 51 F1 1D 87 1F"
		"											73 49 F0 AC BE FE 8B 55 14 10 47 64 55 7D CD 43 </node>"
		"				<node tag=\"0x9F918703\"> 03 </node>"
		"				<node tag=\"0x9F918702\"> A3 EF 67 21 65 D9 92 ED A5 1F FB 1B B5 CC 61 58 14 B1 D0 66 </node>"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F06\"> A0 00 00 03 33 </node>"
		"				<node tag=\"0x9F22\"> 64 </node>"
		"				<node tag=\"0x9F918704\"> A2 FF F1 CC BD A1 87 04 37 00 DE 72 57 6A CC 77"
		"					                        72 4B C4 FF 9D BC F0 71 23 13 BB 34 D3 67 34 47"
		"											E5 57 F1 3E 49 C3 C0 00 46 32 AC 31 E0 C7 07 4B"
		"											DC 08 21 DE 98 D1 FD 17 DE 77 F3 57 5C 2A D1 5A"
		"											B7 7B 2E 83 CB F4 22 F1 D1 E0 1C FF 1B 5A DC 25"
		"											16 71 BA C4 BD F6 E7 74 8F 6F D5 79 FF B0 7E AE"
		"											3D 31 B8 E4 21 79 18 A2 49 33 27 67 05 CF 8B AF"
		"											CE 60 94 7C CD C1 AF AB 0D 43 DB 4E 1A B9 4F E3 </node>"
		"				<node tag=\"0x9F918703\"> 03 </node>"
		"				<node tag=\"0x9F918702\"> B1 33 44 B0 DF 3F EF F9 CA 0E C8 E8 9D CD 06 EF F3 57 A9 90 </node>"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F06\"> A0 00 00 03 33 </node>"
		"				<node tag=\"0x9F22\"> 65 </node>"
		"				<node tag=\"0x9F918704\"> A1 82 C7 82 D2 77 CF 33 61 62 49 12 08 3E 27 86"
		"					                        31 EF 3F CD BF 31 D7 92 C2 9C 20 AA 31 8D C2 CA"
		"											C5 BF 4A 96 E2 68 73 9C CC 01 25 B6 09 76 AD 7F"
		"											64 02 D6 70 82 15 CA 91 B2 AA B3 2B B4 DD 0C 3B"
		"											D6 32 65 18 01 C2 6D A7 81 7D A7 87 10 C4 39 74"
		"											D1 D8 07 C3 AD 05 EB 19 D0 DB B9 9A 86 EB 1C 7D"
		"											67 7E 02 75 8E 48 00 3D 7F 54 09 8E 81 DC 8E 0A"
		"											FB 68 A7 20 DC 4D 32 78 38 9E 04 B5 9E 54 80 4B  </node>"
		"				<node tag=\"0x9F918703\"> 03 </node>"
		"				<node tag=\"0x9F918702\"> 56 F0 56 E0 DC 66 67 49 B3 DA 71 EE 4F FC 45 11 DA CF E1 C3 </node>"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F06\"> A0 00 00 03 33 </node>"
		"				<node tag=\"0x9F22\"> 66 </node>"
		"				<node tag=\"0x9F918704\"> A0 6B B0 6D 08 25 DE C1 F6 F3 07 87 91 E7 61 BC"
		"											A4 57 6B 26 E6 3B CB 67 38 6A 1A D2 6C D7 F4 7B"
		"											2E 48 AD 08 E1 2B FB F3 93 D4 C4 30 0B 28 D5 DF"
		"											6A 2B CF D3 B1 AF B7 78 BF 6E 3A CA A5 BA C7 18"
		"											5E 49 FF E0 93 5E 14 A2 20 EE FD 27 95 87 09 3A"
		"											23 41 BD 9C 46 81 D3 CE 32 37 0C D3 D7 AE 95 2A"
		"											66 2E 56 56 26 A2 97 30 63 A0 C0 A2 29 9B F1 DE"
		"											C3 99 49 AB 26 09 CD E3 CB 97 C9 8C C9 ED 6F E5 </node>"
		"				<node tag=\"0x9F918703\"> 03 </node>"
		"				<node tag=\"0x9F918702\"> D8 F3 05 F5 1E 48 2D 4E 5B 44 D1 D9 37 AB A7 FC D9 C9 77 85 </node>"
		"			</node>"
		""
		"			<!-- UPI_CAKEYS_PRODUCTION -->"
		""
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F06\"> A0 00 00 03 33 </node>"
		"				<node tag=\"0x9F22\"> 08 </node> <!-- CAKEYS_INDEX -->"
		"				<node tag=\"0x9F918704\"> B6 16 45 ED FD 54 98 FB 24 64 44 03 7A 0F A1 8C"
		"										0F 10 1E BD 8E FA 54 57 3C E6 E6 A7 FB F6 3E D2"
		"										1D 66 34 08 52 B0 21 1C F5 EE F6 A1 CD 98 9F 66"
		"										AF 21 A8 EB 19 DB D8 DB C3 70 6D 13 53 63 A0 D6"
		"										83 D0 46 30 4F 5A 83 6B C1 BC 63 28 21 AF E7 A2"
		"										F7 5D A3 C5 0A C7 4C 54 5A 75 45 62 20 41 37 16"
		"										96 63 CF CC 0B 06 E6 7E 21 09 EB A4 1B C6 7F F2"
		"										0C C8 AC 80 D7 B6 EE 1A 95 46 5B 3B 26 57 53 3E"
		"										A5 6D 92 D5 39 E5 06 43 60 EA 48 50 FE D2 D1 BF     </node>"
		"				<node tag=\"0x9F918703\"> 03 </node> <!-- CAKEYS_EXPONENT -->"
		"				<node tag=\"0x9F918702\"> EE 23 B6 16 C9 5C 02 65 2A D1 88 60 E4 87 87 C0 79 E8 E8 5A  </node>"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F06\"> A0 00 00 03 33 </node>"
		"				<node tag=\"0x9F22\"> 09 </node> <!-- CAKEYS_INDEX -->"
		"				<node tag=\"0x9F918704\"> EB 37 4D FC 5A 96 B7 1D 28 63 87 5E DA 2E AF B9"
		"										6B 1B 43 9D 3E CE 0B 18 26 A2 67 2E EE FA 79 90"
		"										28 67 76 F8 BD 98 9A 15 14 1A 75 C3 84 DF C1 4F"
		"										EF 92 43 AA B3 27 07 65 9B E9 E4 79 7A 24 7C 2F"
		"										0B 6D 99 37 2F 38 4A F6 2F E2 3B C5 4B CD C5 7A"
		"										9A CD 1D 55 85 C3 03 F2 01 EF 4E 8B 80 6A FB 80"
		"										9D B1 A3 DB 1C D1 12 AC 88 4F 16 4A 67 B9 9C 7D"
		"										6E 5A 8A 6D F1 D3 CA E6 D7 ED 3D 5B E7 25 B2 DE"
		"										4A DE 23 FA 67 9B F4 EB 15 A9 3D 8A 6E 29 C7 FF"
		"										A1 A7 0D E2 E5 4F 59 3D 90 8A 3B F9 EB BD 76 0B"
		"										BF DC 8D B8 B5 44 97 E6 C5 BE 0E 4A 4D AC 29 E5    </node>"
		"				<node tag=\"0x9F918703\"> 03 </node> <!-- CAKEYS_EXPONENT -->"
		"				<node tag=\"0x9F918702\"> A0 75 30 6E AB 00 45 BA F7 2C DD 33 B3 B6 78 77 9D E1 F5 27 </node>"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F06\"> A0 00 00 03 33 </node>"
		"				<node tag=\"0x9F22\"> 0A </node> <!-- CAKEYS_INDEX -->"
		"				<node tag=\"0x9F918704\"> B2 AB 1B 6E 9A C5 5A 75 AD FD 5B BC 34 49 0E 53"
		"										C4 C3 38 1F 34 E6 0E 7F AC 21 CC 2B 26 DD 34 46"
		"										2B 64 A6 FA E2 49 5E D1 DD 38 3B 81 38 BE A1 00"
		"										FF 9B 7A 11 18 17 E7 B9 86 9A 97 42 B1 9E 5C 9D"
		"										AC 56 F8 B8 82 7F 11 B0 5A 08 EC CF 9E 8D 5E 85"
		"										B0 F7 CF A6 44 EF F3 E9 B7 96 68 8F 38 E0 06 DE"
		"										B2 1E 10 1C 01 02 89 03 A0 60 23 AC 5A AB 86 35"
		"										F8 E3 07 A5 3A C7 42 BD CE 6A 28 3F 58 5F 48 EF     </node>"
		"				<node tag=\"0x9F918703\"> 03 </node> <!-- CAKEYS_EXPONENT -->"
		"				<node tag=\"0x9F918702\"> C8 8B E6 B2 41 7C 4F 94 1C 93 71 EA 35 A3 77 15 87 67 E4 E3 </node>"
		"			</node>"
		""
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F06\"> A0 00 00 03 33 </node>"
		"				<node tag=\"0x9F22\"> 0B </node> <!-- CAKEYS_INDEX -->"
		"				<node tag=\"0x9F918704\"> CF 9F DF 46 B3 56 37 8E 9A F3 11 B0 F9 81 B2 1A"
		"										1F 22 F2 50 FB 11 F5 5C 95 87 09 E3 C7 24 19 18"
		"										29 34 83 28 9E AE 68 8A 09 4C 02 C3 44 E2 99 9F"
		"										31 5A 72 84 1F 48 9E 24 B1 BA 00 56 CF AB 3B 47"
		"										9D 0E 82 64 52 37 5D CD BB 67 E9 7E C2 AA 66 F4"
		"										60 1D 77 4F EA EF 77 5A CC C6 21 BF EB 65 FB 00"
		"										53 FC 5F 39 2A A5 E1 D4 C4 1A 4D E9 FF DF DF 13"
		"										27 C4 BB 87 4F 1F 63 A5 99 EE 39 02 FE 95 E7 29"
		"										FD 78 D4 23 4D C7 E6 CF 1A BA BA A3 F6 DB 29 B7"
		"										F0 5D 1D 90 1D 2E 76 A6 06 A8 CB FF FF EC BD 91"
		"										8F A2 D2 78 BD B4 3B 04 34 F5 D4 51 34 BE 1C 27"
		"										81 D1 57 D5 01 FF 43 E5 F1 C4 70 96 7C D5 7C E5"
		"										3B 64 D8 29 74 C8 27 59 37 C5 D8 50 2A 12 52 A8"
		"										A5 D6 08 8A 25 9B 69 4F 98 64 8D 9A F2 CB 0E FD"
		"										9D 94 3C 69 F8 96 D4 9F A3 97 02 16 2A CB 5A F2"
		"										9B 90 BA DE 00 5B C1 57    </node>"
		"				<node tag=\"0x9F918703\"> 03 </node> <!-- CAKEYS_EXPONENT -->"
		"				<node tag=\"0x9F918702\"> BD 33 1F 99 96 A4 90 B3 3C 13 44 10 66 A0 9A D3 FE B5 F6 6C </node>"
		"			</node>"
		""
		"		</node> <!-- End node 0xBF918801 : TAG_SAMPLE_CAKEYS_PARAMETERS-->"
		""
		""
		"		<node tag=\"0xBF918802\">	<!-- TAG_SAMPLE_CAREVOK_PARAMETERS -->"
		"       	<node tag=\"0x9F91841F\"> A0 00 00 03 33 01 00 10 00 </node>	<!-- TAG_KERNEL_REVOKED_CERTIFICATE_LIST -->"
		"		</node> <!-- End node 0xBF918802 : TAG_SAMPLE_CAREVOK_PARAMETERS -->"
		""
		""
		"		<node tag=\"0xBF918803\">	<!-- TAG_SAMPLE_ICS_PARAMETERS -->"
		""
		"			<node tag=\"0x1000\">"
		"				<!-- ICS0 values -->"
		"				<node tag=\"0x9F928210\"> 03 00 00 00 </node>				<!-- TAG_GENERIC_DETECTION_TYPE -->"
		"				<node tag=\"0x9F928212\"> 00 00 17 70 </node>				<!-- TAG_GENERIC_DETECTION_GLOBAL_TIMEOUT : 6000 -->"
		"				<node tag=\"0x9F928214\"> 01 </node>						<!-- TAG_GENERIC_DETECTION_NB_CARDS_TO_DETECT -->"
		"				<node tag=\"0x9F918804\"> 03 A9 80 </node>					<!-- TAG_SAMPLE_NO_CARD_TIMEOUT : 4 minutes (240000ms = 3A980) (Interac GUI mode) -->"
		""
		"				<!-- Application Independent Data -->"
		"				<node tag=\"0x9F1A\"> 08 34 </node>						<!-- TAG_EMV_TERMINAL_COUNTRY_CODE -->"
		"				<node tag=\"0x9F40\"> FF 00 F0 A1 01 </node>				<!-- TAG_EMV_ADD_TERMINAL_CAPABILITIES -->"
		"				<node tag=\"0x9F35\"> 22 </node>							<!-- TAG_EMV_TERMINAL_TYPE -->"
		""
		"				<!-- Application Dependent Data -->"
		"				<node tag=\"0x9F01\"> 01 23 45 67 89 01 </node>				<!-- TAG_EMV_ACQUIRER_IDENTIFIER -->"
		"				<node tag=\"0x9F15\"> 93 11 </node>							<!-- TAG_EMV_MERCHANT_CATEGORY_CODE -->"
		"				<node tag=\"0x9F16\"> 31 31 32 32 33 33 34 34 35 35 36 36 37 37 38 </node>	<!-- TAG_EMV_MERCHANT_IDENTIFIER -->"
		"				<node tag=\"0x9F1C\"> 31 32 33 34 35 36 37 38 </node> 		<!-- TAG_EMV_TERMINAL_IDENTIFICATION -->"
		"				<node tag=\"0x9F918565\"> 00 00 </node>						<!-- TAG_PAYPASS_MAX_LIFETIME_OF_TORN_TXN_LOG_RECORD -->"
		"				<node tag=\"0x9F918561\"> 00 </node>						<!-- TAG_PAYPASS_MAX_NUMBER_OF_TORN_TXN_LOG_RECORDS -->"
		"				<node tag=\"0x9F4E\" format=\"ascii\"> \"BSEAPP_Merchants\" </node>   		<!-- TAG_EMV_MERCHANT_NAME_AND_LOCATION -->"
		""
		"			</node> <!-- End node 1000 -->"
		"		</node> <!-- End node 0xBF918803 : TAG_SAMPLE_ICS_PARAMETERS-->"
		""
		""
		""
		"		<node tag=\"0xBF918804\">	<!-- TAG_SAMPLE_SPECIFIC_PARAMETERS -->"
		""
		"			<!-- JCB Specific parameters -->"
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F928101\"> 00 01 </node>				<!-- TAG_EP_KERNEL_TO_USE : JCB -->"
		"			</node> <!-- End node 1000 -->"
		""
		"			<!-- PAYPASS Specific parameters -->"
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F928101\"> 00 02 </node>				<!-- TAG_EP_KERNEL_TO_USE : PAYPASS -->"
		"			</node> <!-- End node 1000 -->"
		""
		"			<!-- VISA Specific parameters -->"
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F928101\"> 00 03 </node>				<!-- TAG_EP_KERNEL_TO_USE : VISA -->"
		""
		"				<!-- Supported fDDA versions = 01 -->"
		"				<node tag=\"0x9f918307\"> 00 01 </node>				<!-- TAG_PAYWAVE_TERM_SUPPORTED_FDDA_VERSIONS -->"
		"				<node tag=\"0x9F33\"> E0 28 C8 </node>				<!-- TAG_EMV_TERMINAL_CAPABILITIES -->"
		"			</node> <!-- End node 1000 -->"
		""
		"			<!-- UPI Specific parameters -->"
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F928101\"> 00 07 </node>				<!-- TAG_EP_KERNEL_TO_USE : UPI -->"
		""
		"				<!-- Supported fDDA versions = 00 01 -->"
		"				<node tag=\"0x9F928506\"> 01 </node>				<!-- TAG_QUICKPASS_TERM_SUPPORTED_FDDA_VERSIONS -->"
		"			</node> <!-- End node 1000 -->"
		""
		"			<!-- EMVCO Specific parameters -->"
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F928101\"> 01 01 </node>				<!-- TAG_EP_KERNEL_TO_USE : EMVCO -->"
		"			</node> <!-- End node 1000 -->"
		""
		"			<!-- Discover Specific parameters -->"
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F928101\"> 01 02 </node>				<!-- TAG_EP_KERNEL_TO_USE : DISCOVER -->"
		"			</node> <!-- End node 1000 -->"
		""
		"			<!-- Interac Specific parameters -->"
		"			<node tag=\"0x1000\">"
		"				<node tag=\"0x9F928101\"> 01 03 </node>				<!-- TAG_EP_KERNEL_TO_USE : INTERAC -->"
		"			</node> <!-- End node 1000 -->"
		""
		"			<node tag=\"0x9F918802\" format=\"ascii\"> \"TZS8342.,1\" </node>		<!-- TAG_SAMPLE_GENERIC_MONEY_EXTENDED EUR9782,.1 -->"
		"			<node tag=\"0x9F918802\" format=\"ascii\"> \"USD8402.,1\" </node>		<!-- TAG_SAMPLE_GENERIC_MONEY_EXTENDED MYR4582,.1 -->"
		"			<node tag=\"0x9F918802\" format=\"ascii\"> \"RMB1562.,1\" </node>		<!-- TAG_SAMPLE_GENERIC_MONEY_EXTENDED MYR4582,.1 -->"
		"		</node> <!-- End node 0xBF918804 : TAG_SAMPLE_SPECIFIC_PARAMETERS-->"
		""
		"	</node>	 <!-- End node 0 -->"
		"</tlvtree>"
		""
};


//! \brief Global TLV Tree used for parameters management.
TLV_TREE_NODE pTreeCurrentParam = NULL;							/*!< Node from param file parameters. */
TLV_TREE_NODE pTreeDefaultParam = NULL;							/*!< Node from default parameters. */

static char gs_Label_Disk_Name[DISK_NAME_SIZE];				/*!< To store the disk name. */

// Global variable declaration
static T_PARAMETER_AID gs_Parameter_AID[C_NUMBER_AID_MAX];	/*!< Static structure containing all the AID data prepared before card presentation. */
static int gs_nNbParameterSet; 								/*!< Static data containing the number of configured AID. */

static int nNbMoneyExtended = 0;
static S_MONEY_EXTENDED * ptr_money_extended = NULL;

/////////////////////////////////////////////////////////////////
//// Static functions definition ////////////////////////////////

static void __Cless_Parameters_BuildListValue(TLV_TREE_NODE * pTree, char * pc_x_buffer, int * pi_x_Index, Struct_TagList * ps_x_ListPointer, int i_x_NbTag);
static void __Cless_Parameters_Get_Disk_Name(char* ptDiskName);
static void __Cless_Parameters_SetKRevokParametersInFinalStruct(TLV_TREE_NODE pnode, int nNbAID);
static void __Cless_Parameters_SetKernelParametersInFinalStruct(TLV_TREE_NODE pnode, int nNbAID);
static void __Cless_Parameters_SetGlobalParametersInFinalStruct(TLV_TREE_NODE pnode, int nNbAID);
static int __Cless_Parameters_HexToAscii(char * ac_x_hexbuf, int i_x_hexbuflg, unsigned char * ac_x_asciibuf);

static int __Cless_Parameters_ReadParameter(TLV_TREE_NODE * pTree, unsigned int nIndex, unsigned long tag, ParameterElement *data);
static int __Cless_Parameters_SetBooleanValue (unsigned char *value, unsigned char *pBool);
static int __Cless_Parameters_FileRead(TLV_TREE_NODE * pTree, char *pc_x_DirFile, char *pc_x_fileName);
static int __Cless_Parameters_WriteBinFile(TLV_TREE_NODE pTree, char * ptr_OutBuf);
static int __Cless_Parameters_ReadParamFile(TLV_TREE_NODE * p_node, char *pc_x_DirFile, unsigned int i_x_ParamFileType);
static int __Cless_Parameters_CreateDisk (void);
static int __Cless_Parameters_IsTagAlreadyInTLVTree(TLV_TREE_NODE pTree, unsigned long ulTag, unsigned int nAction);;
static int __Cless_Parameters_CopyParameters( const TLV_TREE_NODE hDestTree, const TLV_TREE_NODE hSrcTree, int nParametersType);
static int __Cless_Parameters_SetAIDParametersInFinalStruct(TLV_TREE_NODE pnode);
static int __Cless_Parameters_EncodeTLVTreeInShareBuf(int nNbAID);
static int __Cless_Parameters_AddTLVTreeInShareBuf (int nNumAID);


/////////////////////////////////////////////////////////////////
//// Functions //////////////////////////////////////////////////

//! \brief This function converts an hexa buffer given as input parameter 
//! to its ascii values.
//! \param[in] str buffer to convert (hexa format).
//! \param[out] dest buffer in ascii format.
//! \return
//! - 0 if successfull.
//! - -1 otherwise.

static int __Cless_Parameters_HexToAscii (char * ac_x_hexbuf, int i_x_hexbuflg, unsigned char * ac_x_asciibuf)
{
	unsigned int i_l_Index;
	unsigned char value;

	for (i_l_Index = 0; i_l_Index<(unsigned int)i_x_hexbuflg; i_l_Index++)
	{
		value = ac_x_hexbuf[i_l_Index] >> 4;
		value &= 0x0f;
		if (value <= 9)
			ac_x_asciibuf[2*i_l_Index] = value + 0x30;
		else
			ac_x_asciibuf[2*i_l_Index] = value + 0x37;

		value = ac_x_hexbuf[i_l_Index] & 0x0f;
		if (value <= 9)
			ac_x_asciibuf[2*i_l_Index+1] = value + 0x30;
		else
			ac_x_asciibuf[2*i_l_Index+1] = value + 0x37;
	}
	// String end
	ac_x_asciibuf[2*i_l_Index] = 0;
	return 1;
}



//! \brief This function builds a list of value with tags given as parameter.
//! \param[in] pTree TLVTree to go throw to find tag values.
//! \param[out] pc_x_buffer : buffer to update with tag value.
//! \param[in,out] pi_x_Index : index on pc_x_buffer.
//! \param[in] ps_x_ListPointer : tag list (Struct_TagList format).
//! \param[in] i_x_NbTag : number of tags in list.

static void __Cless_Parameters_BuildListValue(TLV_TREE_NODE * pTree, char * pc_x_buffer, int * pi_x_Index, Struct_TagList * ps_x_ListPointer, int i_x_NbTag)
{
	unsigned int    ListIndex;
	unsigned int    CarIndex;
	ParameterElement data;

	for (ListIndex=0; (unsigned int)ListIndex<(unsigned int)i_x_NbTag; ListIndex++)
	{
		if (__Cless_Parameters_ReadParameter(pTree, 0, ps_x_ListPointer[ListIndex].tag, &data))
			switch (ps_x_ListPointer[ListIndex].tagtype)
			{
			case C_TAG_TYPE_HEXA_BYTE:
				pc_x_buffer[(*pi_x_Index)++] = *(data.ptValue);
				break;

			case C_TAG_TYPE_BUF_BYTE:
				memcpy(&pc_x_buffer[(*pi_x_Index)], data.ptValue, data.length);
				(*pi_x_Index) += data.length;
				break;

			case C_TAG_TYPE_BUF_BYTE_WITH_MASQ:
				for (CarIndex = 0; CarIndex<data.length; CarIndex++)
				{
					pc_x_buffer[(*pi_x_Index) + CarIndex] = data.ptValue[CarIndex] & ps_x_ListPointer[ListIndex].Masq[CarIndex];
				}
				(*pi_x_Index) += data.length;
				break;

			case C_TAG_TYPE_BOOLEAN:
				// __Cless_Parameters_SetBooleanValue (data.ptValue, (unsigned char *)&pc_x_buffer[(*pi_x_Index)++]);
				pc_x_buffer[(*pi_x_Index)++] = *(data.ptValue);
				break;

			case C_TAG_TYPE_TXT:
			default:
				// Not managed
				break;
			}
	}
}



//! \brief This function reads tag informations in a TLVTree.
//! \param[in] pTree to go throw to found tag informations.
//! \param[in] nIndex for multi-value of a same tag.
//! \param[in] tag to look for.
//! \param[out] data informations on tag.
//! \return
//! - 0 if successfull.
//! - -1 otherwise.

static int __Cless_Parameters_ReadParameter(TLV_TREE_NODE * pTree, unsigned int nIndex, unsigned long tag, ParameterElement *data)
{
	int           Result = 0;
	TLV_TREE_NODE pNode;
	static unsigned char BoolValue;

	// Initialization
	data->tag=0;
	data->length=0;
	data->ptValue = NULL;

	// Look for the tag in the pTree TLVTree
	pNode = TlvTree_Find(*pTree, tag, nIndex);

	// If a tag was found
	if (pNode != NULL)
	{
		data->tag     = tag;
		data->length  = TlvTree_GetLength(pNode);
		data->ptValue = (unsigned char*)TlvTree_GetData(pNode);

		// If the parameter has the "YES" or "NO" value
		if ((data->length > 0) && (data->ptValue != NULL))
		{
			if (__Cless_Parameters_SetBooleanValue(data->ptValue, &BoolValue) == TRUE)
			{
				TlvTree_SetData(pNode, &BoolValue, 1);
			}
		}
		Result = 1;
	}

	return (Result);
}



//! \brief This function sets the boolean given as output parameter according 
//! to the value of input parameter 'value': 
//! - if value equals NO pBool is set to 0.
//! - if value equals YES pBool is set to 1.
//! - for other values of value pBool is set to 0.
//! \param[in] value : string to test.
//! \param[out] pBool : boolean to set.
//! \return
//! - \ref TRUE if value equals YES or NO.
//! - \ref FALSE otherwise.

static int __Cless_Parameters_SetBooleanValue (unsigned char *value, unsigned char *pBool)
{
	int iRet;

	iRet = TRUE;
	if (strcmp((char*)value, NO) == 0)
	{
		*pBool = 0;
	}
	else
	{
		// YES
		if (strcmp((char*)value, YES) == 0)
		{
			*pBool = 1;
		}
		// ERROR
		else
		{
			*pBool = 0;
			iRet = FALSE;
		}
	}
	return (iRet);
}



//! \brief This function reads an array (XML format) and extract tags and value to build a TLVTree. 
//! \param[in] ptr_ac_x_ParamTable array with parameters with XML format.
//! \param[in] i_x_ArraySize array number of elements.
//! \param[out] pBool : boolean to set.
//! \return TLVTree updated with parameters.

static TLV_TREE_NODE __Cless_Parameters_ReadArray(char *ptr_ac_x_ParamTable, int i_x_ArraySize)
{
	TLV_TREE_NODE  pTree = 0;
	int s;

	s = TlvTree_Unserialize(&pTree, TLV_TREE_SERIALIZER_XML, (const unsigned char *)ptr_ac_x_ParamTable, i_x_ArraySize );

	if (s != 0)
	{
		upload_diagnostic_txt((unsigned char *)"PB on xml parameters !!!\n");
		GTL_Traces_TraceDebug ("__Cless_Parameters_ReadArray : PB on xml parameters !!!");
		// Reset of this node, a problem occur
		pTree = 0;
	}

	return (pTree);
}



//! \brief This function reads a file of parameters (XML format) and extract tags and value to build a TLVTree. 
//! \param[in] pTree to go throw to found tag informations.
//! \param[in] pc_x_DirFile file path of xml text file to read.
//! \param[in] pc_x_fileName file name of xml text file to read.
//! \return
//! - 0 if file read ok, 1 else.

static int __Cless_Parameters_FileRead(TLV_TREE_NODE * pTree, char *pc_x_DirFile, char *pc_x_fileName)
{
	char ac_l_OutBuf[80];
	char LabelDiskName[DISK_NAME_SIZE];
	int nResult = 0;

	memclr(LabelDiskName, sizeof(LabelDiskName));
	__Cless_Parameters_Get_Disk_Name(LabelDiskName);

	Telium_Sprintf(ac_l_OutBuf, "/%s/%s", LabelDiskName, C_FILE_PARAM_SAV);

	if ((strncmp(pc_x_fileName, C_FILE_NAME_XML, sizeof(C_FILE_NAME_XML)-1) == 0) || (strncmp(pc_x_fileName, C_FILE_PARAM_SAV, sizeof(C_FILE_PARAM_SAV)-1) == 0) )
	{
		unsigned int FileTypeAccess = TLV_TREE_SERIALIZER_DEFAULT;

		if (strncmp(pc_x_fileName, C_FILE_NAME_XML, sizeof(C_FILE_NAME_XML)-1) == 0) {
			// XML format used to load parameter
			FileTypeAccess = TLV_TREE_SERIALIZER_XML;
		}

		if (__Cless_Parameters_ReadParamFile(pTree, pc_x_DirFile, FileTypeAccess) != 0) {
			GTL_Traces_TraceDebug ("__Cless_Parameters_ReadParamFile() error");
			nResult = 1;
		}
		//Cless_Parameters_TlvTree_Display(pTree, "Read param.par");
	}
	return (nResult);
}



//! \brief This function update a file with parameters given in a TLVTree.
//! File is updated in TLVTree serial binary format.
//! \param[in] pTree to go save in file.
//! \param[in] ptr_OutBuf file path and name to update.
//! \return
//! - 0 if ok, 1 else.

static int __Cless_Parameters_WriteBinFile(TLV_TREE_NODE pTree, char * ptr_OutBuf)
{
	unsigned char * ac_l_pucBuffer = NULL;
	unsigned int  result = 0;
	int           i_l_Size;
	S_FS_FILE     *ptr_file;

	// Input parameters checking
	if ((strlen(ptr_OutBuf) == 0) || (pTree == 0)) {
		return 1;
	}

	// Check size
	i_l_Size = TlvTree_GetSerializationSize(pTree, TLV_TREE_SERIALIZER_DEFAULT);

	if (i_l_Size>C_PUC_MAX_BUFFER_SIZE) {// If file size greater than max size
		GTL_Traces_TraceDebug ("__Cless_Parameters_WriteBinFile : Parameter node size too important");
		result = 1;
	} else {
		ac_l_pucBuffer = (unsigned char *) umalloc(i_l_Size + 10);
		i_l_Size = TlvTree_Serialize( pTree, TLV_TREE_SERIALIZER_DEFAULT, ac_l_pucBuffer, i_l_Size + 10);

		// The buffer is saved in a file
		if ((i_l_Size < 0) || (i_l_Size > C_PUC_MAX_BUFFER_SIZE)) {// If serialization pb
			GTL_Traces_TraceDebug ("__Cless_Parameters_WriteBinFile : Parameter node serialisation pb");
			result = 1;
		}
	}

	if (result == 0)	// If ok
	{
		//unsigned int Mode,rc;
		int ret;

		ret = __Cless_Parameters_CreateDisk();
		if (ret == FS_OK) {
			ret = FS_exist (ptr_OutBuf);

			if (ret == FS_OK) {
				GTL_Traces_TraceDebug ("__Cless_Parameters_WriteBinFile : FILE EXIST");
				// The former file is suppressed. 
				ret = FS_unlink (ptr_OutBuf);
			}
		}

		/*		Mode = FS_WRTMOD;
		rc = FS_mount(ptr_OutBuf, &Mode);
		if (rc != FS_OK)
		{
			upload_diagnostic_txt((unsigned char *)"PB Mount COM File param\n");
			result = 1;
		} */

		ptr_file = FS_open (ptr_OutBuf, "a");
		if (ptr_file == NULL) {
			result = 1;
			upload_diagnostic_txt ((unsigned char *)"Error create file\n");
		}
	}

	if (result == 0) {// If ok
		FS_write (ac_l_pucBuffer, 1, i_l_Size, ptr_file);
		FS_close(ptr_file);	// Close file
	}

	if (ac_l_pucBuffer != 0)
		ufree(ac_l_pucBuffer);
	return (result);
}



//! \brief This function reads a file of parameters (XML format) and extract tags and value to build a TLVTree. 
//! \param[in] pnode to store tags informations.
//! \param[in] pc_x_DirFile file path of xml text file to read.
//! \param[in] pc_x_fileName file name of xml text file to read.
//! \return
//! 0 if ok 1 else.

static int __Cless_Parameters_ReadParamFile(TLV_TREE_NODE * p_node, char *pc_x_DirFile, unsigned int i_x_ParamFileType)
{
	int  result = 0;
	unsigned char * ac_l_pucBuffer = NULL;
	unsigned int Mode,rc;
	S_FS_FILE * ptr_file;

	// Read data from parameters file
	Mode = FS_READMOD;
	rc= FS_mount (pc_x_DirFile,&Mode);
	if (rc != FS_OK)
	{
		upload_diagnostic_txt((unsigned char *)"PB Mount COM Filen\n");
	}
	ptr_file = FS_open(pc_x_DirFile, "r");

	if (ptr_file != NULL) {
		int s;
		unsigned long ulFileLength;

		ulFileLength = FS_length (ptr_file);

		if (ulFileLength>C_PUC_MAX_BUFFER_SIZE) // If file size greater than max size
		{
			GTL_Traces_TraceDebug ("__Cless_Parameters_ReadParamFile : Parameter file size too important !!!");
			result = 1;
		} else {
			ac_l_pucBuffer = (unsigned char *) umalloc(ulFileLength+10);
			FS_read (ac_l_pucBuffer, 1, ulFileLength+10, ptr_file);
			s = TlvTree_Unserialize(p_node, i_x_ParamFileType, ac_l_pucBuffer, ulFileLength+10);
			ufree(ac_l_pucBuffer);

			if (s != 0) {
				// Reset of this node, a problem occurred
				p_node = 0;
				result = 1;
			}
		}
		FS_close(ptr_file);
	}
	else // Pb on FS_open()
	{
		result = 1;
	}

	return (result);
}



//! \brief This function give disk label used to store parameters. 
//! \param[in/out] Disk name to update.

static void __Cless_Parameters_Get_Disk_Name(char* ptDiskName) {
	Telium_Sprintf(gs_Label_Disk_Name,"%s%02x", LABEL_DISK, 0);
	memcpy(ptDiskName, gs_Label_Disk_Name, sizeof(gs_Label_Disk_Name));
}



//! \brief This function create give disk to store parameters. 
//! \return
//! FS_OK if disk created, other else.

static int __Cless_Parameters_CreateDisk (void) {
	int               Retour;
	unsigned long     DiskSize;
	S_FS_PARAM_CREATE ParamCreat;
	char              FilePath_L[33];

	ParamCreat.Mode         = FS_WRITEONCE;
	ParamCreat.IdentZone    = FS_WO_ZONE_DATA;

	memclr(gs_Label_Disk_Name,sizeof(gs_Label_Disk_Name));
	Telium_Sprintf(gs_Label_Disk_Name,"%s%02x", LABEL_DISK, 0);

	strcpy(ParamCreat.Label, gs_Label_Disk_Name);
	ParamCreat.AccessMode   = FS_WRTMOD;
	ParamCreat.NbFichierMax = 20;
	DiskSize              = DISK_SIZE;

	Telium_Sprintf(FilePath_L,"/%s",gs_Label_Disk_Name);
	Retour = FS_mount (FilePath_L,&ParamCreat.Mode);
	if (Retour != FS_OK) {
		ParamCreat.Mode = FS_WRITEONCE;
		Retour=FS_dskcreate(&ParamCreat,&DiskSize);
	}
	return (Retour);
}



//! \brief This function check if tag if allready in destination TLVTree.
//! If tag is allready present => no action or delete it 
//! \param[in] pTree TLVTree with parameters set 
//! \param[in] ulTag tag to check in structure
//! \param[in] nAction, C_PARAMS_NO_ACTION (no ation) or C_PARAMS_DELETE (suppress it)
//! \return 1 if Tag is already present, 0 else

static int __Cless_Parameters_IsTagAlreadyInTLVTree(TLV_TREE_NODE pTree, unsigned long ulTag, unsigned int nAction)
{
	int	nIndex, nFound;
	TLV_TREE_NODE pNode;

	if ((pTree == NULL) || (ulTag == 0))
		return (0); // A problem occurs, input parameters are empty ! Nothing could be made

	nIndex = 0;
	nFound = 0;

	pNode = TlvTree_Find(pTree, (unsigned int) ulTag, nIndex++);
	do{
		if (pNode) // If something found
		{
			nFound = 1; // Super we found it !!!
			if (nAction == C_PARAMS_NO_ACTION)	// If no action is asked on each tag found
				break;	// We can stop treatment

			if (nAction == C_PARAMS_DELETE)		// If a delete action has been asked
			{
				TlvTree_Release(pNode);			// Disapeared !
			}

			pNode = TlvTree_Find(pTree, (unsigned int) ulTag, nIndex++);
		}
	}while (pNode != 0);

	return (nFound);
}



//! \brief This function copies all parameters under hSrcTree TLVTree in the hDestTree TLVTree 
//! \param[in] hSrcTree TLVTree node with parameters to copies.
//! \param[in] hDescTree TLVTree node with parameters to update.
//! \param[in] nParametersType allows to specify if parameters to add are prioritary, exclusive or not. 
//! return 0 if copied has been correctly executed, 1 else.

static int __Cless_Parameters_CopyParameters (const TLV_TREE_NODE hDestTree, const TLV_TREE_NODE hSrcTree, int nParametersType)
{
	unsigned int nResult = 0;
	int nTLVResult;
	TLV_TREE_NODE pResearch;
	int nCopyCanBeMade;
	unsigned int nTag;
	TLV_TREE_NODE pTmpNode;

	if ((hDestTree == NULL) || (hSrcTree == NULL)) {
		nResult = 1;
		return (nResult);
	}

	// Init local data
	pTmpNode = NULL;

	// For each tag found in Src TLVTree
	pResearch = TlvTree_GetFirstChild(hSrcTree);

	do {
		if (pResearch != 0) {
			// Node has been found
			nTag = TlvTree_GetTag(pResearch); // What tag is managed

			switch (nParametersType)
			{
			case C_PRIORITY_PARAMS:
				__Cless_Parameters_IsTagAlreadyInTLVTree(hDestTree, nTag, C_PARAMS_DELETE); // If tag is already present => delete it
				nCopyCanBeMade = 1; // Copy can be made ...
				break;

			case C_UNPRIORITY_PARAMS:
				if (__Cless_Parameters_IsTagAlreadyInTLVTree(hDestTree, nTag, C_PARAMS_NO_ACTION) != 0) // If tag is already present => we keep it
					nCopyCanBeMade = 0; // Copy cannot be made ...
				else
					nCopyCanBeMade = 1; // Tag isn't already present, we can copy it 
				break;

			case C_MULTI_PARAMS: // No action, just a copy is necessary
			default:
				nCopyCanBeMade = 1; // Copy can be made ...
				break;
			}

			if (nCopyCanBeMade) // If copy can be made (no problem of multi same tags)
			{
				// Copy it in destination pNode
				pTmpNode = TlvTree_Copy(pResearch);
				nTLVResult = TlvTree_Graft(hDestTree, pTmpNode);
			} else {
				nTLVResult = TLV_TREE_OK; // To allow to continue
			}

			if (nTLVResult == TLV_TREE_OK)
				pResearch = TlvTree_GetNext(pResearch);
			else {
				nResult = 1;// A problem occurs
				pResearch = NULL; // To stop process
				GTL_Traces_TraceDebug ("__Cless_Parameters_CopyParameters : Unable to add tag in dest TLVTree");
			}
		} // End (if pResearch != 0)
	}while (pResearch != 0);

	return (nResult);
}



//! \brief This function add AID parameters in global gs_Parameter_AID[] structure.
//! \param[in] pnode parameters node for the kernel.
//! \return number of AID found in input parameters.

static int __Cless_Parameters_SetAIDParametersInFinalStruct(TLV_TREE_NODE pnode)
{
	unsigned int nNbAID;
	int	nIndex;
	TLV_TREE_NODE pChild;
	TLV_TREE_NODE pAIDParams, pResearch;			// Node for AID parameters.

	//unsigned long ulTag;
	//unsigned long lFoundTag;
	//unsigned int nLengthFoundTag;
	//unsigned char * pFoundTagValue;


	// Found main AID nodes in TLVTree given in parameters
	nNbAID = 0;
	nIndex = 0;

	// Look for the main AID tag in the pTree TLVTree
	pAIDParams = TlvTree_Find(pnode, C_PARAM_AID_TAG, nIndex);

	if (pAIDParams == 0) // No AID information found
		return(nNbAID);

	// For each AID found in parameters
	pChild = TlvTree_GetFirstChild(pAIDParams);

	// Loop on each parameters set
	while ((pChild != 0) && (nNbAID < C_NUMBER_AID_MAX)) {// Loop on each child

		//nIndex = 0;

		// Create a tree containing the AID data
		gs_Parameter_AID[nNbAID].pTreeAID = TlvTree_New(C_TAG_MAIN);

		pResearch = TlvTree_Find (pChild, TAG_EMV_AID_TERMINAL, 0); // If AID informations have been found

		if (pResearch != 0) // If AID informations have been found
		{
			if (__Cless_Parameters_CopyParameters(gs_Parameter_AID[nNbAID].pTreeAID, pChild, C_MULTI_PARAMS) != 0) {
				GTL_Traces_TraceDebug ("__Cless_Parameters_SetAIDParametersInFinalStruct : Unable to add tag in AID parameters");
			} else {
				// Get informations from tag found
				//lFoundTag = TlvTree_GetTag(pResearch);
				//nLengthFoundTag = TlvTree_GetLength(pResearch);
				//pFoundTagValue = TlvTree_GetData(pResearch);

				// Put index reference in AID informations structure
				gs_Parameter_AID[nNbAID].nAIDLength = TlvTree_GetLength(pResearch);
				memcpy (gs_Parameter_AID[nNbAID].ucAID, TlvTree_GetData(pResearch), TlvTree_GetLength(pResearch));

				// Get the AID Proprietary identifier (if present)
				//pResearch = TlvTree_Find (pChild, TAG_GENERIC_AID_PROPRIETARY_IDENTIFIER, 0); // If AID informations have been found

				/*if (pResearch != NULL)
				{
					gs_Parameter_AID[nNbAID].bAidProprietaryIdPresent = TRUE;
					memcpy (gs_Parameter_AID[nNbAID].ucAidProprietaryId, TlvTree_GetData(pResearch), TlvTree_GetLength(pResearch));
				}
				else
				{
					gs_Parameter_AID[nNbAID].bAidProprietaryIdPresent = FALSE;
					memset (gs_Parameter_AID[nNbAID].ucAidProprietaryId, 0x00, sizeof(gs_Parameter_AID[nNbAID].ucAidProprietaryId));
				}*/

				// Store the index
				gs_Parameter_AID[nNbAID].ucAIDIndex = nNbAID;

				// Delete AID informations from destination TLVTree
				//nIndex = 0;
				pResearch = TlvTree_Find(gs_Parameter_AID[nNbAID].pTreeAID, TAG_EMV_AID_TERMINAL, 0);
				if (pResearch)
					TlvTree_Release(pResearch);
			}
		}

		nNbAID ++; // Increase the number of supported AID
		pChild = TlvTree_GetNext(pChild); // Next AID (next child)
	} // End loop on each AID parameters

	return (nNbAID);
}

//! \brief This function add Krevok parameters in global gs_Parameter_AID[] structure.
//! \param[in] nNbAID Number of available AID.
//! \param[in] pnode parameters node for the kernel.

static void __Cless_Parameters_SetKRevokParametersInFinalStruct(TLV_TREE_NODE pnode, int nNbAID)
{
	unsigned int nIndex, nIndexAID;
	TLV_TREE_NODE pKRevokParams;		// Node for KRevok parameters.

	nIndex = 0;
	nIndexAID = 0;

	// Found global parameters
	pKRevokParams = TlvTree_Find (pnode, C_PARAM_KREVOK_TAG, nIndex);

	// Set global parameters in AID TLVTree
	if (pKRevokParams != NULL) // If KRevok parameters found
	{
		// Loop on each AID
		for (nIndexAID = 0; nIndexAID<(unsigned int)nNbAID; nIndexAID++)
		{
			if (__Cless_Parameters_CopyParameters(gs_Parameter_AID[nIndex].pTreeAID, pKRevokParams, C_MULTI_PARAMS) != 0)
			{
				GTL_Traces_TraceDebug ("__Cless_Parameters_SetKRevokParametersInFinalStruct: Pb copy Krevok parameters");
			}
		}
	}
}


//! \brief This function add kernel parameters in global gs_Parameter_AID[] structure.
//! \param[in] nNbAID Number of available AID.
//! \param[in] pnode parameters node for the kernel.

static void __Cless_Parameters_SetKernelParametersInFinalStruct(TLV_TREE_NODE pnode, int nNbAID)
{
	unsigned char * pucTagPtr;
	int nTagLentgth;
	unsigned int nIndex, nIndexAID;
	TLV_TREE_NODE pKernelParams;		// Node for global parameters.
	TLV_TREE_NODE pResearch;			// Node for research.
	TLV_TREE_NODE pNode;

	nIndex = 0;
	nIndexAID = 0;

	// Found global parameters
	pKernelParams = TlvTree_Find (pnode, C_PARAM_KERNEL_TAG, nIndex);

	// Set global parameters in AID TLVTree
	if (pKernelParams != NULL) // If Kernel parameters found
	{
		// Loop on each AID
		for (nIndexAID = 0; nIndexAID<(unsigned int)nNbAID; nIndexAID++) {
			// Found kernel parameters to use
			//nIndex = 0;
			pResearch = TlvTree_Find(gs_Parameter_AID[nIndexAID].pTreeAID, TAG_EP_KERNEL_TO_USE, 0);

			if (pResearch) // If kernel to use has been found
			{
				//unsigned long lTag = TAG_EP_KERNEL_TO_USE;
				//unsigned int nLengthTag = TlvTree_GetLength(pResearch);
				//unsigned char * pTagValue = TlvTree_GetData(pResearch);

				// Store kernel parameters.
				if (Cless_Parameters_AddParametersInNode(gs_Parameter_AID[nIndexAID].pTreeAID, pnode, C_PARAM_KERNEL_TAG, TAG_EP_KERNEL_TO_USE, TlvTree_GetLength(pResearch), TlvTree_GetData(pResearch), C_UNPRIORITY_PARAMS) != 0)
				{
					GTL_Traces_TraceDebug ("__Cless_Parameters_SetKernelParametersInFinalStruct: Pb kernel parameters copy");
				}
			}
		}

		nIndex = 0;
		pNode = TlvTree_Find(pKernelParams, (unsigned int) TAG_SAMPLE_GENERIC_MONEY_EXTENDED, nIndex++);

		if (pNode) // If something found
		{
			nNbMoneyExtended = 0; // Reset for update it
			if (ptr_money_extended)
				ufree(ptr_money_extended);
			ptr_money_extended = NULL;
		}

		do{
			if (pNode) // If something found
			{
				// Add informations in ptr_money_extended array
				nNbMoneyExtended++;

				pNode = TlvTree_Find(pKernelParams, (unsigned int) TAG_SAMPLE_GENERIC_MONEY_EXTENDED, nIndex++);
			}
		}while (pNode != 0);

		nIndex = 0;
		if (nNbMoneyExtended) {
			ptr_money_extended = umalloc(nNbMoneyExtended * sizeof(S_MONEY_EXTENDED));
			pNode = TlvTree_Find(pKernelParams, (unsigned int) TAG_SAMPLE_GENERIC_MONEY_EXTENDED, nIndex++);

			do{
				if (pNode) // If something found
				{
					nTagLentgth = TlvTree_GetLength(pNode);
					pucTagPtr = TlvTree_GetData(pNode);
				}

				if (pNode) // If something found
				{
					memset(&ptr_money_extended[nIndex-1], 0, sizeof(S_MONEY_EXTENDED)); 		// Reset structure
					if (nTagLentgth >= 10)														// If something found
					{
						memcpy(ptr_money_extended[nIndex-1].currency.nom, pucTagPtr, 3); 		// Nom copy, 3 bytes
						memcpy(ptr_money_extended[nIndex-1].currency.code, pucTagPtr + 3, 3); 	// Code copy, 3 bytes
						ptr_money_extended[nIndex-1].currency.posdec = pucTagPtr[6] - 0x30;		// Decimal position in the amount  

						ptr_money_extended[nIndex-1].infos.cents_separator = pucTagPtr[7];
						ptr_money_extended[nIndex-1].infos.thousands_separator = pucTagPtr[8];
						ptr_money_extended[nIndex-1].infos.currency_position = pucTagPtr[9] - 0x30;
					}
					pNode = TlvTree_Find(pKernelParams, (unsigned int) TAG_SAMPLE_GENERIC_MONEY_EXTENDED, nIndex++);
				}
			}while (pNode != 0);
		}
	}
}


//! \brief This function give Extended money structure initialized with parameters.
//! \param[in/out] n_x_NbMoney Number of money managed.
//! \param[in/out] ps_x_Money pnode parameters node for the kernel.

void Cless_Parameters_GetMoneyExtended(int * n_x_NbMoney, S_MONEY_EXTENDED ** ps_x_Money)
{
	*n_x_NbMoney = nNbMoneyExtended;
	*ps_x_Money  = ptr_money_extended;
}


//! \brief This function add global parameters in global gs_Parameter_AID[] structure.
//! \param[in] nNbAID.
//! \param[in] pnode parameters node for the kernel.

static void __Cless_Parameters_SetGlobalParametersInFinalStruct(TLV_TREE_NODE pnode, int nNbAID)
{
	TLV_TREE_NODE pGlobalParams;		// Node for global parameters.
	unsigned int nIndex;

	nIndex = 0;

	// Found global parameters
	nIndex = 0;
	pGlobalParams = TlvTree_Find(pnode, C_PARAM_GLOBAL_TAG, nIndex);

	// Set global parameters in AID TLVTree
	if (pGlobalParams) // If global parameters found
	{
		// Loop on each AID
		for (nIndex = 0; nIndex<(unsigned int)nNbAID; nIndex++) {
			// Store global parameters.
			if (Cless_Parameters_AddParametersInNode(gs_Parameter_AID[nIndex].pTreeAID, pnode, C_PARAM_GLOBAL_TAG, 0, 0, NULL, C_UNPRIORITY_PARAMS) != 0) {
				GTL_Traces_TraceDebug ("__Cless_Parameters_SetGlobalParametersInFinalStruct : Cless_Parameters_BuildAIDNodes : No global parameters found");
			}
		}
	}
}



//! \brief This function add global parameters in global gs_Parameter_AID[] structure.
//! \param[in] nNbAID Number of parametered AID.
//! \param[in] pnode parameters node for the kernel.
//! \return KERNEL_STATUS_OK if no problem, other else.

static int __Cless_Parameters_EncodeTLVTreeInShareBuf(int nNbAID)
{
	int nResult = 0;
	int nIndex;
	T_SHARED_DATA_STRUCT* pAidInfo;

	// Loop on each AID set of parameter to manage
	for (nIndex=0; nIndex<nNbAID; nIndex++)
	{
		pAidInfo = GTL_SharedExchange_Init(C_SHARE_BUFFER_SIZE, C_BUFFER_SHARED);
		if (pAidInfo == NULL) {
			nResult = 1; // Error
			goto End;
		}

		gs_Parameter_AID[nIndex].pSharedAIDParam = pAidInfo; 

		if (__Cless_Parameters_AddTLVTreeInShareBuf(nIndex) != 0)
			nResult = 1; // Error
	} // End Loop on each AID set of parameter to manage

	End :
	return (nResult);
}



//! \brief This function add parameters in global g_Parameter_AID[] structure.
//! \param[in] nNumAID parameter set number.
//! \return
//!		- 0 if no problem
//!		- 1 else.

static int __Cless_Parameters_AddTLVTreeInShareBuf (int nNumAID)
{
	int nResult = 0;
	int nResultSharedExchange;
	TLV_TREE_NODE hRootNode, hNode; 
	T_SHARED_DATA_STRUCT* pAidInfo;

	hRootNode = gs_Parameter_AID[nNumAID].pTreeAID;
	pAidInfo = gs_Parameter_AID[nNumAID].pSharedAIDParam; 
	hNode = hRootNode;

	// Loop on each tag to add in Share exchange buffer
	do{
		hNode = TlvTree_Iterate(hNode, hRootNode );

		if (hNode != 0) // Tag may be added in Shared Buffer
		{
			// Get informations from tag found					

			nResultSharedExchange = GTL_SharedExchange_AddTag (pAidInfo, TlvTree_GetTag(hNode), TlvTree_GetLength(hNode), TlvTree_GetData(hNode));
			switch (nResultSharedExchange)
			{
			case STATUS_SHARED_EXCHANGE_OK:
				break;
			case STATUS_SHARED_EXCHANGE_NOT_ENOUGH_SPACE:
				GTL_Traces_TraceDebug ("__Cless_Parameters_EncodeTLVTreeInShareBuf() not enought space");
				nResult = 1; // Error
				break;
			case STATUS_SHARED_EXCHANGE_ERROR:
			default:
				GTL_Traces_TraceDebug ("__Cless_Parameters_EncodeTLVTreeInShareBuf() exchange error");
				nResult = 1; // Error
				break;
			}
		}
	}while((hNode != 0) && (nResultSharedExchange == STATUS_SHARED_EXCHANGE_OK)); // End loop on tag to add in Share exchange buffer

	return (nResult);
}






//! \brief This function clear TLVTree used to store parameters. 

void Cless_Parameters_InitParameters(TLV_TREE_NODE * pTree) {
	if (pTree) {
		if (*pTree != NULL) {
			TlvTree_Release(*pTree);
			*pTree = NULL;
		}
	}
}



//! \brief This function update TLVTree with defaut parameters (read in an array). 

void Cless_Parameters_ReadDefaultParameters(TLV_TREE_NODE * pTree) {
	Cless_Parameters_InitParameters (pTree);

	// Default parameters initialisation => set in TLV tree
	*pTree = __Cless_Parameters_ReadArray((char*)gs_DefaultParam, sizeof(gs_DefaultParam));
}



//! \brief This function update TLVTree with parameters read in a XML text file. 

int Cless_Parameters_ReadXMLParamFile (TLV_TREE_NODE * p_node, unsigned int i_x_tag, char *filePath, char *fileName) {
	int  result = 0;
	unsigned char * ac_l_pucBuffer;
	unsigned int Mode,rc;
	S_FS_FILE * ptr_file;
	unsigned int i_x_ParamFileType = TLV_TREE_SERIALIZER_XML;
	int s = 0;
	unsigned long ulFileLength = 0;

	(void)i_x_tag;
	ptr_file = NULL;

	// Open input file
	Mode = FS_READMOD;
	rc= FS_mount (filePath,&Mode);
	if (rc != FS_OK) {
		// Cannot mount the file
		GTL_Traces_DiagnosticText("Cless_Parameters_ReadXMLParamFile : Cannot mount the file\n");
		result = 1;
		goto End;
	}

	// Open the file
	ptr_file = FS_open (fileName, "r");

	if (ptr_file == NULL) {
		// Problem with file
		GTL_Traces_DiagnosticText("Cless_Parameters_ReadXMLParamFile : FS_open(%s) ko\n", fileName);
		result = 1;
		goto End;
	} else {
		// File correctly opened, allocate a buffer to store the file content
		ulFileLength = FS_length (ptr_file);

		if (ulFileLength>C_PUC_MAX_BUFFER_SIZE) { // If file size greater than max size

			result = 1;
			GTL_Traces_DiagnosticText("Cless_Parameters_ReadXMLParamFile : XML param file size too important\n");
			goto End;
		} else {
			ac_l_pucBuffer = (unsigned char *) umalloc(ulFileLength+10);
			// Read the file content, and store it in ac_l_pucBuffer
			FS_read (ac_l_pucBuffer, 1, ulFileLength+10, ptr_file);
			// Unserialise the file content (as it is a TLV Tree)
			s = TlvTree_Unserialize(p_node, i_x_ParamFileType, ac_l_pucBuffer, ulFileLength+10);
			// TLV Tree is unserialised, free the allocated buffer
			ufree(ac_l_pucBuffer);
		}

		if (s != 0) {
			// Reset of this node, a problem occurred
			p_node = 0;
			result = 1;
			GTL_Traces_DiagnosticText("Cless_Parameters_ReadXMLParamFile : PB unserialize param file\n");
			goto End;
		}
	}

	End:
	// If mount has been made, unmount the disk
	if (rc == FS_OK)
		FS_unmount(filePath);

	if (ptr_file != NULL)
		FS_close(ptr_file);

	return (result);
}



//! \brief This function set TLVTree parameters in a save file. 

int Cless_Parameters_SaveParam (TLV_TREE_NODE pTree) {
	// Local variables duaration
	// Function result
	int nResult;
	char ac_l_FilePath_L[80];
	char LabelDiskName[DISK_NAME_SIZE];

	memclr(LabelDiskName, sizeof(LabelDiskName));
	__Cless_Parameters_Get_Disk_Name(LabelDiskName);

	// Update PARAM.PAR serial binary TLVTree format file
	Telium_Sprintf(ac_l_FilePath_L, "/%s/%s", LabelDiskName, C_FILE_PARAM_SAV);
	nResult = __Cless_Parameters_WriteBinFile(pTree, ac_l_FilePath_L);

	return(nResult);
}



//! \brief This function update TLVTree with parameters read in a save file. 

int Cless_Parameters_RestoreParam (TLV_TREE_NODE * pTree) {
	char FilePath_L[80];
	int ret = FS_OK;
	char LabelDiskName[DISK_NAME_SIZE];
	int nResult;
	unsigned int Mode,rc;

	memclr(FilePath_L,sizeof(FilePath_L));

	memclr(LabelDiskName, sizeof(LabelDiskName));
	__Cless_Parameters_Get_Disk_Name(LabelDiskName);

	Mode = FS_WRTMOD;
	Telium_Sprintf(FilePath_L, "/%s", LabelDiskName);
	rc = FS_mount(FilePath_L, &Mode);
	if (rc != FS_OK) {
		upload_diagnostic_txt((unsigned char *)"PB Mount COM File param\n");
		nResult = 1;
	} else {

		Telium_Sprintf(FilePath_L, "/%s/%s", LabelDiskName, C_FILE_PARAM_SAV);
		// Set path in input buffer for BuildParameters() call
		ret = FS_exist (FilePath_L);

		if (ret == FS_OK) {
			GTL_Traces_TraceDebug("Cless_Parameters_RestoreParam : Load %s file", C_FILE_PARAM_SAV);
			// Load parameter file in the terminal
			nResult = __Cless_Parameters_FileRead (pTree, FilePath_L, C_FILE_PARAM_SAV);
		} else {
			GTL_Traces_TraceDebug("Cless_Parameters_RestoreParam : %s file not existing", C_FILE_PARAM_SAV);
			nResult = 1;
		}
	}

	return (nResult);
}



//! \brief This function compute a CRC allowing to check parameters loaded.
//! The CRC is computed on a list of a fix list of tags.

int Cless_Parameters_Compute_Config_CRC(TLV_TREE_NODE * pTree, int * nCRCLength, unsigned char ** pSha) {
	unsigned char   buffer[370];
	unsigned int    i;

	i = 0;
	memset (buffer, 0, sizeof (buffer));

	// Buffer build
	__Cless_Parameters_BuildListValue(pTree, (char*)buffer, (int*)&i, (Struct_TagList *)CrcTagList, sizeof(CrcTagList) / sizeof(Struct_TagList));

	*pSha = (unsigned char*)GTL_SHA_Sha1Compute ((unsigned char*)buffer, i);

	*nCRCLength = 20;

	return TRUE;
}



//! \brief This function allows to trace TLV tree given as parameter on trace tool

void Cless_Parameters_TlvTree_Display(TLV_TREE_NODE pTree , char * txt) {
	// #define DETAIL

	// trace when simulating on target
	TLV_TREE_NODE pNode;
	char TmpBuf[100];
#ifdef DETAIL
	unsigned char * ptr_car;
	unsigned int index;
#endif // DETAIL


	pNode = pTree;

	GTL_Traces_TraceDebug("\nCless_Parameters_TlvTree_Display:\n %s", txt);

	while( pNode != NULL ) {
		Telium_Sprintf(TmpBuf, "- %08x [- %08x]: (%d/%d)   ", TlvTree_GetTag(pNode), (int)(TlvTree_GetParent(pNode) ? TlvTree_GetParent(pNode) : 0 ),
				(int)TlvTree_GetData(pNode), (int)TlvTree_GetLength(pNode));
		GTL_Traces_TraceDebug(TmpBuf);

#ifdef DETAIL
		ptr_car = (unsigned char*)TlvTree_GetData(pNode);
		for (index=0; ((index<(TlvTree_GetLength(pNode))) && (index<30)); index++) {
			Telium_Sprintf(&TmpBuf[index*3], "%2x ", *ptr_car++);
		}
		GTL_Traces_TraceDebug(TmpBuf);
#endif // DETAIL

		pNode = TlvTree_Iterate( pNode,  pTree );
	} 
}



//! \brief This function trace parameters store in a TLVTree.
//! \brief Trace may be made on printer, USB, ... 

void Cless_Parameters_Trace_Param_Supported(TLV_TREE_NODE pnode, unsigned char ucOutputType, char *ptr_x_DataName, unsigned char c_x_tracetype, unsigned long ul_x_tag) {
	unsigned long lLocalTag;
	unsigned int nLocalLen;
	unsigned char text[1024];
	char partialtext[130];
	char * PtrText;
	char * ptr_data;
	TLV_TREE_NODE hnode;
	TLV_TREE_NODE pNode;
	unsigned char ucSavedOutputDevice;

	// Save default output device
	ucSavedOutputDevice = Cless_DumpData_DumpGetOutputId ();

	Cless_DumpData_DumpSetOutputId(ucOutputType); 			// Output on printer/USB...

	if (Cless_DumpData_DumpOpenOutputDriver() == FALSE) {	// Open Driver

		// Restore default output device
		Cless_DumpData_DumpSetOutputId (ucSavedOutputDevice);
		return; // A problem occurs
	}

	hnode = pnode;

	Cless_DumpData("%s", ptr_x_DataName);

	// If main Mark has been found
	if (hnode == NULL)
		goto End; // A problem occurs, dump data driver shall be closed

	// Display all the nodes and children under the mark given
	pNode = hnode;

	pNode = TlvTree_Iterate(pNode, hnode);

	while( pNode != NULL )
	{   
		lLocalTag = TlvTree_GetTag(pNode);
		nLocalLen = TlvTree_GetLength(pNode);
		ptr_data = (char *)TlvTree_GetData(pNode);

		switch (c_x_tracetype) {
		case C_TRACE_ONE_TAG :
			if (TlvTree_GetTag(pNode) != ul_x_tag)
				break;

		case C_TRACE_ALL_CHILD:
		{
			Cless_DumpData("T:%lx L:%x V:", lLocalTag, nLocalLen);

			memset (text, '\0', sizeof(text));
			__Cless_Parameters_HexToAscii (ptr_data, nLocalLen, text);

			if (strlen ((char*)text) > 120) {
				PtrText = (char*)text;
				do {
					strncpy(partialtext, PtrText, 120); // warning, strncpy dosn't copy string end if maximun length is reached
					partialtext[120] = 0; // Protection against overflow
					PtrText += 120;
					Cless_DumpData("%s\n", partialtext);
				}while(strlen(partialtext) == 120);
			} else
				Cless_DumpData("%s", text);

			break;
		}

		default: // No treatment
			break;

		}
		pNode = TlvTree_Iterate(pNode, hnode);
	} // End while on Node

	Cless_DumpData_DumpNewLine();							// CR

	End:
	Cless_DumpData_DumpCloseOutputDriver();				// close Output Driver

	// Restore default output device
	Cless_DumpData_DumpSetOutputId (ucSavedOutputDevice);
}



//! \brief This function trace CRC of a predefine list of tag of a TLVTree.

void Cless_Parameters_Trace_CRC(TLV_TREE_NODE pnode, unsigned char ucOutputType, char *ptr_x_DataName)
{
	int nCRCLength, nIndex;
	unsigned char * pSha;
	char          acStr[370];
	char          acTmpStr[10];
	unsigned char ucSavedOutputDevice;

	// Save default output device
	ucSavedOutputDevice = Cless_DumpData_DumpGetOutputId();

	Cless_DumpData_DumpSetOutputId(ucOutputType); 			// Output on printer/USB...
	if (Cless_DumpData_DumpOpenOutputDriver() == FALSE)	// Open Driver
	{
		// Restore default output device
		Cless_DumpData_DumpSetOutputId(ucSavedOutputDevice);
		return; // A problem occurs
	}

	Cless_DumpData("%s", ptr_x_DataName);

	Cless_Parameters_Compute_Config_CRC(&pnode, &nCRCLength, &pSha);
	Telium_Sprintf(acStr, "CRC loaded file : ");
	for (nIndex=0; nIndex<nCRCLength; nIndex++)
	{
		Telium_Sprintf(acTmpStr, "%02x", pSha[nIndex]);
		strcat(acStr, acTmpStr);
	}
	Cless_DumpData("%s", acStr);

	Cless_DumpData_DumpNewLine();							// CR
	Cless_DumpData_DumpCloseOutputDriver();				// close Output Driver

	// Restore default output device
	Cless_DumpData_DumpSetOutputId(ucSavedOutputDevice);
}



//! \brief This function select TLVTree of parameters in relationchip with Index AID.

int Cless_Parameters_GiveAIDNumber(void)
{
	return (gs_nNbParameterSet);
}



//! \brief This function Send AID of parameters store.

unsigned char * Cless_Parameters_GiveAID(unsigned int n_x_AIDIndex, int * pAIDLength) {
	unsigned char * pAID = NULL;

	*pAIDLength = 0;

	if (n_x_AIDIndex >= (unsigned int)gs_nNbParameterSet) { // If index greater than number of parameter set

		GTL_Traces_TraceDebug("Cless_Parameters_GiveAID : Bad index");
		goto End;
	}

	pAID = gs_Parameter_AID[n_x_AIDIndex].ucAID;
	*pAIDLength	= gs_Parameter_AID[n_x_AIDIndex].nAIDLength;

	End:
	return (pAID);
}



//! \brief This function select TLVTree of parameters in relationchip with Index AID.

TLV_TREE_NODE Cless_Parameters_GiveAIDTLVTtree(unsigned int n_x_AIDIndex) {
	if (n_x_AIDIndex >= (unsigned int)gs_nNbParameterSet) { // If index greater than number of parameter set

		GTL_Traces_TraceDebug("Cless_Parameters_GiveAIDTLVTtree : Bad index");
		return (NULL);
	}

	return (gs_Parameter_AID[n_x_AIDIndex].pTreeAID);
}



//! \brief This function select shared buffer of parameters to use with Index AID.

T_SHARED_DATA_STRUCT * Cless_Parameters_GiveAIDShareBuffer(unsigned int n_x_AIDIndex) {
	if (n_x_AIDIndex >= (unsigned int)gs_nNbParameterSet) {// If index greater than number of parameter set
		GTL_Traces_TraceDebug("Cless_Parameters_GiveAIDShareBuffer : Bad index");
		return (NULL);
	}

	return (gs_Parameter_AID[n_x_AIDIndex].pSharedAIDParam);
}



//! \brief This function found subset parameters and select parameters set to store in MainNode.

int Cless_Parameters_AddParametersInNode(TLV_TREE_NODE pMainNode, TLV_TREE_NODE pParameterNode, unsigned long ulSubsetTag, unsigned long ulTag, int nTagLength, unsigned char * ucTagValue, int nParametersType)
{
	int nResult = 0; // No problem = Default result
	//  Find main tag in input parameters node
	int	nIndex = 0;
	int nFound = 0;
	TLV_TREE_NODE pNode, pChild, pResearch;

	if ((pMainNode == NULL) || (pParameterNode == NULL) || (ulSubsetTag == 0))
	{
		GTL_Traces_TraceDebug ("Cless_Parameters_AddParametersInNode : Invalid input parameter");
		nResult = 1; // A problem occurs, input parameters are empty !
		goto End;
	}

	// Look for the tag in the pTree TLVTree
	pNode = TlvTree_Find(pParameterNode, ulSubsetTag, 0);

	// If a tag was found
	if (pNode != NULL) // If subset has been found
	{
		pChild = TlvTree_GetFirstChild(pNode);

		// Loop on each parameters set
		while((pChild != 0) && (!nFound))
		{
			nIndex = 0;

			if (ulTag == 0) // If no tag is specified, first set of parameters will be taken.
			{
				nFound = TRUE;
			}
			else // A tag is specified : we have to look for the tag with appropriate value.
			{
				do{
					pResearch = TlvTree_Find(pChild, ulTag, nIndex++);

					if (pResearch != 0)
					{
						// Check if tag found is same as we want 					
						if (((unsigned long)TlvTree_GetTag(pResearch) == ulTag) && (TlvTree_GetLength(pResearch) == (unsigned long)nTagLength) && (memcmp(TlvTree_GetData(pResearch), ucTagValue, nTagLength) == 0))
							nFound = TRUE;					 
					}
				}while ((pResearch != NULL) && (!nFound));
			}

			if (!nFound)
			{
				pChild = TlvTree_GetNext(pChild); // Next Child
			}
		}

		if (nFound) // If specific tag has been found, all parameters in pChild set may be copied in pMainNode main Node parameter set
		{
			if (__Cless_Parameters_CopyParameters(pMainNode, pChild, nParametersType) != 0)
			{
				GTL_Traces_TraceDebug ("Cless_Parameters_AddParametersInNode : __Cless_Parameters_CopyParameters error");
				nResult = 1;// A problem occurs
				goto End;
			}
		}
	}
	End:
	return (nResult);
}



//! \brief This function organises TLVTree nodes with parameters used for AID selection.

void Cless_Parameters_BuildAIDNodes(TLV_TREE_NODE pnode)
{
	int nIndex;

	// Init structures
	for (nIndex = 0; nIndex < gs_nNbParameterSet; nIndex++) {
		if (gs_Parameter_AID[nIndex].pTreeAID != NULL) {
			TlvTree_Release(gs_Parameter_AID[nIndex].pTreeAID);
			gs_Parameter_AID[nIndex].pTreeAID = NULL;
		}

		if (gs_Parameter_AID[nIndex].pSharedAIDParam != NULL)
			GTL_SharedExchange_DestroyShare (gs_Parameter_AID[nIndex].pSharedAIDParam);
	}

	// Init structures
	memset(gs_Parameter_AID[0].ucAID, 0, sizeof(T_PARAMETER_AID) * C_NUMBER_AID_MAX);
	gs_nNbParameterSet = 0;

	// Add AID parameters in final global Structure
	gs_nNbParameterSet = __Cless_Parameters_SetAIDParametersInFinalStruct(pnode);

	if (gs_nNbParameterSet == 0) {
		GTL_Traces_TraceDebug("Cless_Parameters_BuildAIDNodes : No avaibale AID");
		return; // No AID found ...
	}

	// Here, gs_Parameter_AID[] is initialised with AID parameters, AID value (and length) and AID index    

	// Add kernel parameters in final global Structure
	__Cless_Parameters_SetKernelParametersInFinalStruct(pnode, gs_nNbParameterSet);

	// Add global parameters in final global Structure
	__Cless_Parameters_SetGlobalParametersInFinalStruct(pnode, gs_nNbParameterSet);

	// Add KRevok parameters in final global Structure
	__Cless_Parameters_SetKRevokParametersInFinalStruct(pnode, gs_nNbParameterSet);


	// Put set of parameters in ShareTLV (BERTLV) format (for kernel use when AID will be used).
	if (__Cless_Parameters_EncodeTLVTreeInShareBuf(gs_nNbParameterSet) != 0)
		GTL_Traces_TraceDebug ("Cless_Parameters_BuildAIDNodes : __Cless_Parameters_EncodeTLVTreeInShareBuf failed");
}



//! \brief Dump parameters set on trace link.

void Cless_Parameters_Trace_Parameter_AID_Structure(void)
{
	unsigned int nIndexAID;

	// Loop on each AID parameter set
	for (nIndexAID = 0; nIndexAID<(unsigned int)gs_nNbParameterSet; nIndexAID++)
	{
		GTL_Traces_TraceDebug("AID parameters set index : %d\n", gs_Parameter_AID[nIndexAID].ucAIDIndex);
		GTL_Traces_TraceDebugBuffer (gs_Parameter_AID[nIndexAID].nAIDLength, gs_Parameter_AID[nIndexAID].ucAID,	"AID :");
		Cless_Parameters_Trace_Param_Supported(gs_Parameter_AID[nIndexAID].pTreeAID, CUSTOM_OUTPUT_TRACE, "TLVTree:", C_TRACE_ALL_CHILD, 0);
	}
}



//! \brief Read the XML file and store it in a TLV Tree.

int Cless_Parameters_ReadXMLFile (char * VolumeName, char * FileName, TLV_TREE_NODE * pTree) {
	int nResult;
	char FilePath_L[DISK_NAME_SIZE];
	char FileName_L[DISK_NAME_SIZE];

	if (strcmp(FileName, C_FILE_PARAM_XML) == 0) { // Does the file name is CLESSCUST.PAR ?
		// Reset TLVTree
		//Cless_Parameters_InitParameters(&pTreeCurrentParam);
		Cless_Parameters_InitParameters(pTree);

		// Read XML file
		Telium_Sprintf(FilePath_L, "/%s", VolumeName);
		Telium_Sprintf(FileName_L, "/%s/%s", VolumeName, FileName);

		// Read the parameter file and stor ethe content in pTreeCurrentParam
		//nResult = Cless_Parameters_ReadXMLParamFile (&pTreeCurrentParam, 0, FilePath_L, FileName_L);
		nResult = Cless_Parameters_ReadXMLParamFile (pTree, 0, FilePath_L, FileName_L);

		//Trace parameters on printer
		if (nResult == 0) { // If no problem
			// Trace the supported parameters in the Trace tool
			//Cless_Parameters_Trace_Param_Supported(pTreeCurrentParam, CUSTOM_OUTPUT_TRACE, "Read parameters", C_TRACE_ALL_CHILD, 0);
			Cless_Parameters_Trace_Param_Supported(*pTree, CUSTOM_OUTPUT_TRACE, "Read parameters", C_TRACE_ALL_CHILD, 0);

			// Save the parameters into a file (CLESSPARAM.PAR)
			//nResult = Cless_Parameters_SaveParam (pTreeCurrentParam);
			nResult = Cless_Parameters_SaveParam (*pTree);

			if (nResult != 0) {// If a problem occurs
				GTL_Traces_TraceDebug("Cless_Parameters_ReadXMLFile : Cless_Parameters_SaveParam() error");				
			} else {
				//Cless_Parameters_Trace_CRC(pTreeCurrentParam, CUSTOM_OUTPUT_TRACE, "CRC loaded file :");
				Cless_Parameters_Trace_CRC(*pTree, CUSTOM_OUTPUT_TRACE, "CRC loaded file :");
			}
		} else {
			GTL_Traces_DiagnosticText ("Cless_Parameters_ReadXMLFile : CLESSCUST.PAR incorrect file received\n");

			//Use default Parameters
			__Cless_Menu_DefaultParameters();
		}
		return (STOP); // File is ok for this application
	}
	return (FCT_OK);
}



//! \brief Get the CA Key data according to the RID and the CA Key index.
//! \param[in] ParameterTlvTree TLV Tree containing all the parameters.
//! \param[in] ucCaKeyIndex CA Public Key index to find.
//! \param[in] pRid RID to be used to find the CA Public Key.
//! \param[out] pOutputDataStruct Shared buffer filled with the CA Public Key data.
//! \return
//!		- \ref TRUE if correctly performed (data found and \a pOutputDataStruct filled).
//!		- \ref FALSE if an error occurred.

int Cless_Parameters_GetCaKeyData (TLV_TREE_NODE ParameterTlvTree, const unsigned char ucCaKeyIndex, const unsigned char * pRid, T_SHARED_DATA_STRUCT * pOutputDataStruct)
{
	TLV_TREE_NODE pCaKeysParams;		// Node for CAKeys.
	TLV_TREE_NODE pChild;				// CA Key content
	TLV_TREE_NODE pResearch;			// Working Node
	unsigned char * pValue;
	int nResult = FALSE;
	int bFound = FALSE;
	int cr;

	pCaKeysParams = TlvTree_Find(ParameterTlvTree, C_PARAM_CAKEY_TAG, 0);

	if (pCaKeysParams != NULL)
	{
		// For each CA Key found in parameters
		pChild = TlvTree_GetFirstChild(pCaKeysParams);

		while (pChild != NULL)
		{
			pResearch = TlvTree_Find(pChild, TAG_EMV_AID_TERMINAL, 0); // If AID informations have been found

			if (pResearch != NULL)
			{
				if (memcmp(pRid, TlvTree_GetData(pResearch), 5) == 0)
				{
					// Same RID, check if CA Key index is identical
					pResearch = TlvTree_Find(pChild, TAG_EMV_CA_PUBLIC_KEY_INDEX_TERM, 0); // If AID informations have been found

					if (pResearch != NULL)
					{
						pValue = TlvTree_GetData(pResearch);
						if (ucCaKeyIndex == pValue[0])
						{
							// This is the correct CA Key, copy the necessary data in pOutputDataStruct (modulus and exponent)
							bFound = TRUE;							

							pResearch = TlvTree_Find(pChild, TAG_EMV_INT_CAPK_MODULUS, 0); // If AID informations have been found
							if (pResearch != NULL)
							{
								cr = GTL_SharedExchange_AddTag(pOutputDataStruct, TAG_EMV_INT_CAPK_MODULUS, TlvTree_GetLength(pResearch), TlvTree_GetData(pResearch));
								if (cr != STATUS_SHARED_EXCHANGE_OK)
								{
									GTL_Traces_TraceDebug("Cless_Parameters_GetCaKeyData An error occurred when adding TAG_EMV_INT_CAPK_MODULUS in the shared structure (cr = %02x)", cr);
									GTL_SharedExchange_ClearEx(pOutputDataStruct, FALSE);
									goto End;
								}
							}

							pResearch = TlvTree_Find(pChild, TAG_EMV_INT_CAPK_EXPONENT, 0); // If AID informations have been found
							if (pResearch != NULL)
							{
								cr = GTL_SharedExchange_AddTag(pOutputDataStruct, TAG_EMV_INT_CAPK_EXPONENT, TlvTree_GetLength(pResearch), TlvTree_GetData(pResearch));
								if (cr != STATUS_SHARED_EXCHANGE_OK)
								{
									GTL_Traces_TraceDebug("Cless_Parameters_GetCaKeyData An error occurred when adding TAG_EMV_INT_CAPK_EXPONENT in the shared structure (cr = %02x)", cr);
									GTL_SharedExchange_ClearEx(pOutputDataStruct, FALSE);
									goto End;
								}
							}

							nResult = TRUE;
							goto End;
						}
					}
				}
			}

			pChild = TlvTree_GetNext(pChild);
		}
	}

	if (!bFound)
	{
		GTL_Traces_TraceDebug("Cless_Parameters_GetCaKeyData : CA Public Key is not found");
	}
	End:
	return (nResult);
}



//! \brief Create the certificate supported list for each AID.
//! \param[in] ParameterTlvTree TLV Tree containing all the parameters.
//! \param[in] usKernelId Kernel identifier to determine which AID structure shall be updated with the supported certificate list.
//! \return
//!		- \ref TRUE if correctly performed.
//!		- \ref FALSE if an error occurred.

int Cless_Parameters_CreateSupportedCertificateListForKernel (TLV_TREE_NODE ParameterTlvTree, unsigned short usKernelId) {
	unsigned char ucSupportedCaList[64];
	unsigned long ulSupportedListLength;
	unsigned char * pAid;
	int nResult = TRUE;
	TLV_TREE_NODE pAidParams; // AID structure content
	TLV_TREE_NODE pChild; // CA Key content
	TLV_TREE_NODE pResearch; // Working Node
	unsigned short usCurrentKernel;
	unsigned char * pKernelToUse;

	pAidParams = TlvTree_Find(ParameterTlvTree, C_PARAM_AID_TAG, 0);

	if (pAidParams != NULL) {
		pChild = TlvTree_GetFirstChild(pAidParams);

		while (pChild != NULL) {
			// Init kernel identifier
			usCurrentKernel = DEFAULT_EP_KERNEL_UNKNOWN;

			// Search the kernel to use for this AID
			pResearch = TlvTree_Find(pChild, TAG_EP_KERNEL_TO_USE, 0); // Get the kernel to use for this AID

			if (pResearch != NULL) {
				// Build the kernel to use for future use
				pKernelToUse = TlvTree_GetData(pResearch);
				usCurrentKernel = pKernelToUse[1] + (pKernelToUse[0] << 8);

				// If the AID kernel is the same as the requested one
				if (usCurrentKernel == usKernelId) {
					// Get the AID
					pResearch = TlvTree_Find(pChild, TAG_EMV_AID_TERMINAL, 0); // If AID informations have been found

					if (pResearch != NULL) {
						// Save the AID pointer
						pAid = TlvTree_GetData(pResearch);

						// Init output data (supported certificate list)
						ulSupportedListLength = sizeof (ucSupportedCaList);
						memset (ucSupportedCaList, 0, ulSupportedListLength);

						// Get the certificate list for this AID
						Cless_Parameters_GetSupportedCertificateList (ParameterTlvTree, ucSupportedCaList, &ulSupportedListLength, pAid);

#ifndef DISABLE_OTHERS_KERNELS
						if (ulSupportedListLength > 0) {
							if (usKernelId == DEFAULT_EP_KERNEL_PAYPASS) {
								// Add the tag in the TLV Tree (for the current AID)
								if (TlvTree_AddChild (pChild, TAG_PAYPASS_INT_SUPPORTED_CAPK_INDEX_LIST, ucSupportedCaList, ulSupportedListLength) == NULL) {
									GTL_Traces_TraceDebug ("Cless_Parameters_CreateSupportedCertificateListForKernel : An error occurred when adding the supported certificate list to the AID structure");
									nResult = FALSE;
								}
							}
#ifndef DISABLE_INTERAC
							else if (usKernelId == DEFAULT_EP_KERNEL_INTERAC) {
								if (TlvTree_AddChild (pChild, TAG_INTERAC_INT_SUPPORTED_CAPK_INDEX_LIST, ucSupportedCaList, ulSupportedListLength) == NULL) {
									GTL_Traces_TraceDebug ("Cless_Parameters_CreateSupportedCertificateListForKernel : An error occurred when adding the supported certificate list to the Interac AID structure");
									nResult = FALSE;
								}
							}
#endif
							else if (usKernelId == DEFAULT_EP_KERNEL_DISCOVER_DPAS) {
								if (TlvTree_AddChild (pChild, TAG_DISCOVER_DPAS_INT_SUPPORTED_CAPK_INDEX_LIST, ucSupportedCaList, ulSupportedListLength) == NULL) {
									GTL_Traces_TraceDebug ("Cless_Parameters_CreateSupportedCertificateListForKernel : An error occurred when adding the supported certificate list to the Discover DPAS AID structure");
									nResult = FALSE;
								}
							}
#ifndef DISABLE_PURE
							else if (usKernelId == DEFAULT_EP_KERNEL_PURE) {
								if (TlvTree_AddChild (pChild, TAG_PURE_INT_SUPPORTED_CAPK_INDEX_LIST, ucSupportedCaList, ulSupportedListLength) == NULL) {
									GTL_Traces_TraceDebug ("Cless_Parameters_CreateSupportedCertificateListForKernel : An error occurred when adding the supported certificate list to the PURE AID structure");
									nResult = FALSE;
								}
							}
#endif
						}
#endif
					}
				}
			}

			// Get the next AID structure
			pChild = TlvTree_GetNext(pChild);
		}
	}

	return (nResult);
}



//! \brief Get the CA Key data according to the RID and the CA Key index.
//! \param[in] ParameterTlvTree TLV Tree containing all the parameters.
//! \param[out] pCertificateList List of supported certificate indexes.
//! \param[in,out] Input : The length of \a pCertificate. Output : length of the certificate list.
//! \param[in] pRid RID to be used to find the CA Public Key.
//! \param[out] pOutputDataStruct Shared buffer filled with the CA Public Key data.

void Cless_Parameters_GetSupportedCertificateList (TLV_TREE_NODE ParameterTlvTree, unsigned char * pCertificateList, unsigned long * pCertificateListLength, const unsigned char * pRid)
{
	TLV_TREE_NODE pCaKeysParams;		// Node for CAKeys.
	TLV_TREE_NODE pChild;				// CA Key content
	TLV_TREE_NODE pResearch;			// Working Node
	unsigned char * pValue;
	unsigned long ulTempLength = 0;

	pCaKeysParams = TlvTree_Find(ParameterTlvTree, C_PARAM_CAKEY_TAG, 0);

	if (pCaKeysParams != NULL) {
		// For each CA Key found in parameters
		pChild = TlvTree_GetFirstChild(pCaKeysParams);

		while (pChild != NULL) {
			pResearch = TlvTree_Find(pChild, TAG_EMV_AID_TERMINAL, 0); // If AID informations have been found

			if (pResearch != NULL) {
				if (memcmp(pRid, TlvTree_GetData(pResearch), 5) == 0) {
					// Same RID, check if CA Key index is identical
					pResearch = TlvTree_Find(pChild, TAG_EMV_CA_PUBLIC_KEY_INDEX_TERM, 0); // If AID informations have been found

					if (pResearch != NULL) {
						pValue = TlvTree_GetData(pResearch);

						if (*pCertificateListLength > ulTempLength) {
							pCertificateList[ulTempLength] = pValue[0];
							ulTempLength ++; // One more supported CA
						}
					}
				}
			}

			pChild = TlvTree_GetNext(pChild);
		}
	}

	if (pCertificateListLength != NULL)
		*pCertificateListLength = ulTempLength;
}



//! \brief Check the CA keys coherence and verify the checksum.
//! \param[in] ParameterTlvTree TLV Tree containing all the parameters.
//! \note It dumps all the invalid data using the configured output device (cf "Cless_DumpData.h" functions).

void Cless_Parameters_CheckCaKeysCrc (TLV_TREE_NODE ParameterTlvTree)
{
	TLV_TREE_NODE pCaKeysParams;		// Node for CAKeys.
	TLV_TREE_NODE pChild, pChildTmp;	// CA Key content
	TLV_TREE_NODE pResearch;			// Working Node
	unsigned char ucTempBuffer[288];
	unsigned int nDataLength;
	int bContinue;
	unsigned long * pShaResult;
	unsigned char * pRid;
	unsigned char * pCapkIndex;
	unsigned char * pCapkModulus;
	unsigned char * pCapkExponent;
	unsigned char * pCapkCrc;
	unsigned int nRidLength;
	unsigned int nCapkIndexLength;
	unsigned int nCapkModulusLength;
	unsigned int nCapkExponentLength;
	unsigned int nCapkCrcLength;
	unsigned int nIndex;
	unsigned int bCertificateErrorDetected = FALSE;
	MSGinfos tDisplayMsg;

	pCaKeysParams = TlvTree_Find(ParameterTlvTree, C_PARAM_CAKEY_TAG, 0);

	if (pCaKeysParams != NULL) {
		// For each CA Key found in parameters
		pChild = TlvTree_GetFirstChild(pCaKeysParams);

		while (pChild != NULL) {
			// Clear the temporary buffer
			memset (ucTempBuffer, 0x00, sizeof(ucTempBuffer));
			nDataLength = 0;
			bContinue = TRUE;
			pShaResult = NULL;
			pRid = NULL;
			pCapkIndex = NULL;
			pCapkModulus = NULL;
			pCapkExponent = NULL;
			pCapkCrc = NULL;
			nRidLength = 0;
			nCapkIndexLength = 0;
			nCapkModulusLength = 0;
			nCapkExponentLength = 0;
			nCapkCrcLength = 0;


			// First, check the RID is present with length equal to 5 bytes
			pResearch = TlvTree_Find(pChild, TAG_EMV_AID_TERMINAL, 0); // If AID informations have been found

			if (pResearch != NULL)
			{
				pRid = TlvTree_GetData(pResearch);
				nRidLength = TlvTree_GetLength(pResearch);

				if (nRidLength == 5)
				{
					memcpy (&(ucTempBuffer[nDataLength]), pRid, nRidLength);
					nDataLength += nRidLength;
				}
				else
				{
					// RID has a bad length
					bContinue = FALSE;
				}
			}
			else
			{
				// RID is missing, check next 
				bContinue = FALSE;
			}


			// Check CA PK index is present with length set to 1
			pResearch = TlvTree_Find(pChild, TAG_EMV_CA_PUBLIC_KEY_INDEX_TERM, 0); // If AID informations have been found

			if (pResearch != NULL)
			{
				pCapkIndex = TlvTree_GetData(pResearch);
				nCapkIndexLength = TlvTree_GetLength(pResearch);

				if (nCapkIndexLength == 1) {
					memcpy (&(ucTempBuffer[nDataLength]), pCapkIndex, nCapkIndexLength);
					nDataLength += nCapkIndexLength;
				} else {
					// CA Public Key Index has a bad length, check next
					bContinue = FALSE;
				}
			} else {
				// CA Public Key Index is missing, check next
				bContinue = FALSE;
			}


			// Check CA PK Modulus is present with length in [1;248]
			pResearch = TlvTree_Find(pChild, TAG_EMV_INT_CAPK_MODULUS, 0);

			if (pResearch != NULL) {
				pCapkModulus = TlvTree_GetData(pResearch);
				nCapkModulusLength = TlvTree_GetLength(pResearch);

				if ((nCapkModulusLength > 0) && (nCapkModulusLength <= 248)) {
					memcpy (&(ucTempBuffer[nDataLength]), pCapkModulus, nCapkModulusLength);
					nDataLength += nCapkModulusLength;
				} else {
					// CA Public Key Modulus has a bad length, check next
					bContinue = FALSE;
				}
			} else {
				// CA Public Key Modulus is missing, check next
				bContinue = FALSE;
			}


			// Check CA PK Exponent is present and length is either 1 or 3.
			pResearch = TlvTree_Find(pChild, TAG_EMV_INT_CAPK_EXPONENT, 0); // If AID informations have been found

			if (pResearch != NULL) {
				pCapkExponent = TlvTree_GetData(pResearch);
				nCapkExponentLength = TlvTree_GetLength(pResearch);

				if ((nCapkExponentLength == 1) || (nCapkExponentLength == 3)) {
					memcpy (&(ucTempBuffer[nDataLength]), pCapkExponent, nCapkExponentLength);
					nDataLength += nCapkExponentLength;
				} else {
					// CA Public Key Exponent has a bad length, check next
					bContinue = FALSE;
				}
			} else {
				// CA Public Key Exponent is missing, check next
				bContinue = FALSE;
			}


			// Calculate the CRC
			if (bContinue) {
				pShaResult = GTL_SHA_Sha1Compute (ucTempBuffer, nDataLength);
			}



			// Check the CA PK Checksum is present with length 20, and check it matches the computed checksum
			pResearch = TlvTree_Find(pChild, TAG_EMV_INT_CAPK_CHECKSUM, 0);

			if (pResearch != NULL) {
				pCapkCrc = TlvTree_GetData(pResearch);
				nCapkCrcLength = TlvTree_GetLength(pResearch);

				if ((nCapkCrcLength == 20) && (pShaResult != NULL)) {
					if (memcmp ((unsigned char *)pShaResult, pCapkCrc, 20) != 0)
						bContinue = FALSE;
				} else {
					// CA Public Key CRC has a bad length, check next
					bContinue = FALSE;
				}
			} else {
				// CA Public Key CRC is missing, check next
				bContinue = FALSE;
			}


			// If a checking error occurred, dump the incorrect CA structure information
			if (!bContinue)
			{
				if (Cless_DumpData_DumpOpenOutputDriver())
				{
					if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER)
						Cless_DumpData ("\x1b" "E");

					Cless_DumpData ("INVALID CERTIFICATE");

					if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER)
						Cless_DumpData ("\x1b" "@" "\x0F");

					Cless_DumpData_DumpNewLine();
					Cless_DumpData_DumpNewLine();
					Cless_DumpData ("RID = ");
					if (nRidLength != 0)
						if (nRidLength != 5)
							Cless_DumpData("Bad Length");
						else
							Cless_DumpData("%02x%02x%02x%02x%02x", pRid[0], pRid[1], pRid[2], pRid[3], pRid[4]);
					else
						Cless_DumpData("Missing");
					Cless_DumpData_DumpNewLine();

					Cless_DumpData ("Index = ");
					if (nCapkIndexLength != 0)
						if (nCapkIndexLength != 1)
							Cless_DumpData("Bad Length");
						else
							Cless_DumpData ("%02x", pCapkIndex[0]);
					else
						Cless_DumpData("Missing");
					Cless_DumpData_DumpNewLine();

					Cless_DumpData ("Modulus = ");
					if (nCapkModulusLength != 0)
						if (nCapkModulusLength > 248)
							Cless_DumpData("Bad Length");
						else
							Cless_DumpData ("%02x%02x...%02x%02x", pCapkModulus[0], pCapkModulus[1], pCapkModulus[nCapkModulusLength-2], pCapkModulus[nCapkModulusLength-1]);
					else
						Cless_DumpData("Missing");
					Cless_DumpData_DumpNewLine();

					Cless_DumpData ("Exponent = ");
					if (nCapkExponentLength != 0)
						if ((nCapkExponentLength != 1) && (nCapkExponentLength != 3))
							Cless_DumpData("Bad Length");
						else
							if (nCapkExponentLength == 1)
								Cless_DumpData ("%02x", pCapkExponent[0]);
							else
								Cless_DumpData ("%02x%02x%02x", pCapkExponent[0], pCapkExponent[1], pCapkExponent[2]);
					else
						Cless_DumpData("Missing");
					Cless_DumpData_DumpNewLine();

					Cless_DumpData ("CRC = ");
					if (nCapkCrcLength != 0)
						if (nCapkCrcLength != 20)
							Cless_DumpData("Bad Length");
						else {
							for (nIndex=0;nIndex<20;nIndex++)
								Cless_DumpData ("%02x", pCapkCrc[nIndex]);
						} else
							Cless_DumpData("Missing");
					Cless_DumpData_DumpNewLine();
					Cless_DumpData_DumpNewLine();

					if (pShaResult != NULL) {
						Cless_DumpData ("Expected Cks = ");
						for (nIndex=0;nIndex<20;nIndex++)
							Cless_DumpData ("%02x", ((unsigned char *)pShaResult)[nIndex]);
					}		
					Cless_DumpData_DumpNewLine();
					Cless_DumpData_DumpNewLine();
					Cless_DumpData_DumpNewLine();

					if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER)
						Cless_DumpData ("\x1b" "@");

					Cless_DumpData_DumpCloseOutputDriver();
				}
			}

			// Get the next CA structure
			pChildTmp = pChild;
			pChild = TlvTree_GetNext(pChild);

			if (!bContinue) {
				bCertificateErrorDetected = TRUE;
				TlvTree_Release(pChildTmp);
			}
		}
	}

	// If a certificate error has been detected, display a message on th emerchant screen
	if (bCertificateErrorDetected) {
		Cless_Term_Read_Message(STD_MESS_INVALID_CERTIFICATE, PSQ_Give_Language(), &tDisplayMsg);
		Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_2, &tDisplayMsg, LEDSOFF);
		Helper_RefreshScreen(WAIT, HELPERS_MERCHANT_SCREEN);
	}
}



//! \brief Dump the CA keys configured in the parameter file.
//! \param[in] ParameterTlvTree TLV Tree containing all the parameters.
//! \note It dumps all the invalid data using the configured output device (cf "Cless_DumpData.h" functions).

void Cless_Parameters_DumpCaKeys (TLV_TREE_NODE ParameterTlvTree) {
	TLV_TREE_NODE pCaKeysParams;		// Node for CAKeys.
	TLV_TREE_NODE pChild;				// CA Key content

	pCaKeysParams = TlvTree_Find(ParameterTlvTree, C_PARAM_CAKEY_TAG, 0);

	if (pCaKeysParams != NULL) {
		// For each CA Key found in parameters
		pChild = TlvTree_GetFirstChild(pCaKeysParams);

		while (pChild != NULL) {
			// Dump the structure
			Cless_DumpData_DumpTlvTreeNodeWithTitle ((unsigned char *)"CERTIFICATE", pChild);

			// Get the next CA structure
			pChild = TlvTree_GetNext(pChild);
		}
	}
}



//! \brief Dumps the parameters identifier label.
//! \param[in] ParameterTlvTree TLV Tree containing all the parameters.
//! \note It dumps all the invalid data using the configured output device (cf "Cless_DumpData.h" functions).

void Cless_Parameters_DumpIdentificationLabel (TLV_TREE_NODE ParameterTlvTree) {
	TLV_TREE_NODE pParamIdLabel;		// Node for Parameter Identification Label.
	unsigned char * pValue;
	unsigned int nDataLength;
	unsigned char szString[32];

	pValue = NULL;
	nDataLength = 0;
	memset (szString, '\0', sizeof(szString));

	pParamIdLabel = TlvTree_Find(ParameterTlvTree, TAG_SAMPLE_PARAMETERS_LABEL, 0);

	if (pParamIdLabel != NULL) {
		pValue = TlvTree_GetData(pParamIdLabel);
		nDataLength = TlvTree_GetLength(pParamIdLabel);
	}

	// Dump the title
	if (Cless_DumpData_DumpOpenOutputDriver()) {
		if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER)
			Cless_DumpData ("\x1b" "E");

		if ((pValue != NULL) && (nDataLength != 0)) {
			memcpy (szString, pValue, nDataLength);
			Cless_DumpData ("%s", szString);
		} else {
			Cless_DumpData ("UNKNOWN PARAM ID");
		}

		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();

		if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER)
			Cless_DumpData ("\x1b" "@");

		// Close the output driver
		Cless_DumpData_DumpCloseOutputDriver();
	}
}



//! \brief Dump the AIDs configured in the parameter file.
//! \param[in] ParameterTlvTree TLV Tree containing all the parameters.
//! \note It dumps all the invalid data using the configured output device (cf "Cless_DumpData.h" functions).

void Cless_Parameters_DumpAids (TLV_TREE_NODE ParameterTlvTree) {
	TLV_TREE_NODE pAidsParams;		// Node for CAKeys.
	TLV_TREE_NODE pChild;				// CA Key content

	pAidsParams = TlvTree_Find(ParameterTlvTree, C_PARAM_AID_TAG, 0);

	if (pAidsParams != NULL) {
		// For each CA Key found in parameters
		pChild = TlvTree_GetFirstChild(pAidsParams);

		while (pChild != NULL) {
			// Dump the title
			Cless_DumpData_DumpTlvTreeNodeWithTitle ((unsigned char *)"AID STRUCTURE", pChild);

			// Get the next CA structure
			pChild = TlvTree_GetNext(pChild);
		}
	}
}



//! \brief Dump the global parameters configured in the parameter file.
//! \param[in] ParameterTlvTree TLV Tree containing all the parameters.
//! \note It dumps all the invalid data using the configured output device (cf "Cless_DumpData.h" functions).

void Cless_Parameters_DumpGlobals (TLV_TREE_NODE ParameterTlvTree) {
	TLV_TREE_NODE pGlobalParams;		// Node for Global data.
	TLV_TREE_NODE pChild;				// CA Key content

	pGlobalParams = TlvTree_Find(ParameterTlvTree, C_PARAM_GLOBAL_TAG, 0);

	if (pGlobalParams != NULL) {
		// For each CA Key found in parameters
		pChild = TlvTree_GetFirstChild(pGlobalParams);

		while (pChild != NULL) {
			// Dump the title
			Cless_DumpData_DumpTlvTreeNodeWithTitle ((unsigned char *)"GLOBAL DATA", pChild);

			// Get the next CA structure
			pChild = TlvTree_GetNext(pChild);
		}
	}
}



//! \brief Dump the kernel specific parameters configured in the parameter file.
//! \param[in] ParameterTlvTree TLV Tree containing all the parameters.
//! \note It dumps all the invalid data using the configured output device (cf "Cless_DumpData.h" functions).

void Cless_Parameters_DumpKernelSpecific (TLV_TREE_NODE ParameterTlvTree) {
	TLV_TREE_NODE pKernelParams;		// Node for kernel specific data.
	TLV_TREE_NODE pChild;				// CA Key content

	pKernelParams = TlvTree_Find(ParameterTlvTree, C_PARAM_KERNEL_TAG, 0);

	if (pKernelParams != NULL) {
		// For each CA Key found in parameters
		pChild = TlvTree_GetFirstChild(pKernelParams);

		while (pChild != NULL) {
			// Dump the title
			Cless_DumpData_DumpTlvTreeNodeWithTitle ((unsigned char *)"KERNEL SPECIFIC", pChild);

			// Get the next CA structure
			pChild = TlvTree_GetNext(pChild);
		}
	}
}



//! \brief This function get the currency label and exponent from the parameters.
//! \param[in] pCurrencyCode Currency code to find in the parameter file.
//! \param[out] pCurrencyLabel Currency label to get.
//!	\return
//!	- \a TRUE if currency is found.
//!	- \a FALSE indicate an invalid parameter as the currency code provided is unknown from the application.

int Cless_Parameters_GetCurrencyFromParameters(unsigned char *pCurrencyCode, char **pCurrencyLabel) {
	int n_NbMoney;
	S_MONEY_EXTENDED * ps_x_Money;
	int i;
	static char *pEurLabel = "TZS";
	char ucCurrencyCode[3];

	GTL_Convert_DcbToAscii(pCurrencyCode, ucCurrencyCode, 1, 4);

	// Get the list of supported money
	Cless_Parameters_GetMoneyExtended(&n_NbMoney, &ps_x_Money);

	// Try to find the requested money
	for(i=0; i<n_NbMoney; i++)
	{
		if(memcmp(ps_x_Money[i].currency.code, ucCurrencyCode, 3) == 0) {
			// Currency code found in the parameter file, get the currency label and exponent
			*pCurrencyLabel = (char*)ps_x_Money[i].currency.nom;
			return (TRUE);
		}
	}

	// Not found, check if it is the default currency TZS
	if(memcmp(ucCurrencyCode,"834", 3) == 0) {
		*pCurrencyLabel = pEurLabel;
		return (TRUE);
	}

	return (FALSE);
}



//! \brief This function get the format and the currency position from the parameters to display the amount.
//! \param[in] pCurrencyLabel Currency label to find in the parameter file.
//! \param[out] pFormat Format of the displayed amount.
//! \param[out] pPosition Position of the currency (before or after the amount).
//!	\return
//!	- \a TRUE if format to display is found.
//!	- \a FALSE indicate an invalid parameter as the currency label provided is unknown from the application.

int Cless_Parameters_GetCurrencyFormatFromParameters(char *pCurrencyLabel, unsigned char *pFormat,unsigned char *pPosition) {
	int n_NbMoney;
	S_MONEY_EXTENDED * ps_x_Money;
	int i;
	unsigned char ucExponent;
	unsigned char ucCents;			// character used to separate the cents
	unsigned char ucThousands;		// character used to separate the thousands

	// Set the default format
	*pFormat = 0;	// 0 ==> 1.000,00
	*pPosition = CURRENCY_AFTER_AMOUNT;	// 0 ==> currency displayed after the amount

	// Get the list of supported money
	Cless_Parameters_GetMoneyExtended(&n_NbMoney, &ps_x_Money);

	// Try to find the requested money
	for(i=0; i<n_NbMoney; i++) {
		if(memcmp(ps_x_Money[i].currency.nom, pCurrencyLabel, 3) == 0) {
			// Currency label found in the parameter file, get the format and the position

			if(ps_x_Money[i].infos.currency_position == CURRENCY_BEFORE_AMOUNT)
				*pPosition = CURRENCY_BEFORE_AMOUNT;
			else
				*pPosition = CURRENCY_AFTER_AMOUNT;

			ucCents = ps_x_Money[i].infos.cents_separator;
			ucThousands = ps_x_Money[i].infos.thousands_separator;
			ucExponent = ps_x_Money[i].currency.posdec;

			if ((ucCents == ',') && (ucThousands == '.') && (ucExponent == 2))
				*pFormat = 0;
			else if ((ucCents == '.') && (ucThousands == ',') && (ucExponent == 2))
				*pFormat = 1;
			else if ((ucCents == ',') && (ucThousands == ' ') && (ucExponent == 2))
				*pFormat = 2;
			else if ((ucCents == ' ') && (ucThousands == '.') && (ucExponent == 0))
				*pFormat = 3;
			else if ((ucCents == ' ') && (ucThousands == ',') && (ucExponent == 0))
				*pFormat = 4;
			else if ((ucCents == ' ') && (ucThousands == ' ') && (ucExponent == 0))
				*pFormat = 5;
			else if ((ucCents == ',') && (ucThousands == '.') && (ucExponent == 3))
				*pFormat = 6;
			else if ((ucCents == '.') && (ucThousands == ',') && (ucExponent == 3))
				*pFormat = 7;
			else if ((ucCents == ',') && (ucThousands == ' ') && (ucExponent == 3))
				*pFormat = 8;
			else
				*pFormat = 0;

			return (TRUE);
		}
	}

	return (FALSE);
}


