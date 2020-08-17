/**
 * \file	DllTesting_Implementation.h
 *
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
 **/

#ifndef __CLESS_SAMPLE_IMPLEMENTATION_H__INCLUDED__
#define __CLESS_SAMPLE_IMPLEMENTATION_H__INCLUDED__


// TDS includes
#include "sdk.h" // IngeDev automatic refactoring - 2014/06/19 20:42:43 - Replace '#include "SDK30.h"' to '#include "sdk.h"'
#include "TlvTree.h"
#include "del_lib.h"
#include "bitmap.h"
#include "MSGlib.h"
#include "hterm.h"
#include "XMLs.h"
#include "GL_GraphicLib.h"

#include "perf_log.h"

// Cless includes
#include "oem_cless.h"
#include "oem_clmf1.h"

// Generic Tool Library includes
#include "GTL_Assert.h"
#include "GTL_StdTimer.h"
#include "GTL_Traces.h"
#include "GTL_TagsInfo.h"
#include "GTL_BerTlv.h"
#include "GTL_BerTlvDecode.h"
#include "GTL_Convert.h"
#include "GTL_SharedExchange.h"
#include "GTL_SHA.h"

#include "TPass.h"

// Security includes
#include "SEC_interface.h"

// EMV tags definition
#include "EmvLib_Tags.h"

// Entry Point includes
#include "EntryPoint_Tags.h"

// Common cless kernel includes
#include "Common_Kernels_Tags.h"
#include "Common_Kernels_API.h"

// ExpressPay kernel includes
#include "ExpressPay_Tags.h"
#include "ExpressPay3_API.h"

// PayPass kernel includes
#include "PayPass3_API.h"
#include "PayPass_Tags.h"

#ifndef DISABLE_PAYWAVE_22
// payWave kernel includes
#include "payWave_API.h"
#include "payWave_Tags.h"
#endif // DISABLE_PAYWAVE_22

// QuickPass kernel includes
#include "QuickPass_API.h"
#include "QuickPass_Tags.h"

// ExpressPay kernel includes
#include "ExpressPay3_API.h"
#include "ExpressPay_Tags.h"

// Discover kernel includes
#include "Discover_API.h"
#include "Discover_Tags.h"

// DiscoverDPAS kernel includes
#include "DiscoverDPAS_API_Common.h"
#include "DiscoverDPAS_API.h"
#include "DiscoverDPAS_Tags.h"

// Interac kernel includes
#ifndef DISABLE_INTERAC
#include "Interac_API.h"
#include "Interac_Tags.h"
#endif

// JCB kernel includes
#include "JCB_Tags.h"
#include "JCB_API.h"

#ifndef DISABLE_OTHERS_KERNELS
#ifndef DISABLE_PURE
// PURE kernel includes
#include "PURE_API_Common.h"
#include "PURE_Tags.h"
#include "PURE_API.h"
#endif

//// PagoBancomat kernel includes
//#ifndef DISABLE_PAGOBANCOMAT
//#include "PagoBancomat_API.h"
//#include "PagoBancomat_Tags.h"
//#endif
//
//// MultiBanco kernel includes
//#ifndef DISABLE_MULTIBANCO_KERNEL
//#include "MultiBanco_Defines.h"
//#include "MultiBanco_Tags.h"
//#include "MultiBanco_API.h"
//#endif
#endif // DISABLE_OTHERS_KERNELS


#ifndef DEFAULT_EP_KERNEL_JCB_C5
#define DEFAULT_EP_KERNEL_JCB_C5                    0x0005			/*!< PPSE Application Selection - Defines a JCB-C5 Kernel as indicated in the <i>EMVCo Entry Point specification</i>. */
#endif // DEFAULT_EP_KERNEL_JCB_C5

#ifndef DEFAULT_EP_KERNEL_DISCOVER_DPAS
#define DEFAULT_EP_KERNEL_DISCOVER_DPAS				0x0006	        /*!< PPSE Application Selection - Defines a Discover DPAS kernel (outside of the EMVCo range as this has not been yet defined by EMVCo). */
#endif // DEFAULT_EP_KERNEL_DISCOVER_DPAS

#ifndef DEFAULT_EP_KERNEL_QUICKPASS
#define DEFAULT_EP_KERNEL_QUICKPASS					0x0007			/*!< PPSE Application Selection - Defines a QuickPass kernel (outside of the EMVCo range as this has not been yet defined by EMVCo). */
#endif // DEFAULT_EP_KERNEL_QUICKPASS

#ifndef DISABLE_OTHERS_KERNELS
///----------------------------------------------------------
#ifndef DEFAULT_EP_KERNEL_PURE
#define DEFAULT_EP_KERNEL_PURE						0x0105			/*!< PPSE Application Selection - Defines a PURE kernel (outside of the EMVCo range as this has not been yet defined by EMVCo). */
#endif // DEFAULT_EP_KERNEL_PURE

#ifndef DEFAULT_EP_KERNEL_PAGOBANCOMAT
#define DEFAULT_EP_KERNEL_PAGOBANCOMAT				0x0106			/*!< PPSE Application Selection - Defines a Pago Bancomat kernel (outside of the EMVCo range as this has not been yet defined by EMVCo). */
#endif // DEFAULT_EP_KERNEL_PAGOBANCOMAT

#ifndef DEFAULT_EP_KERNEL_MULTIBANCO
#define DEFAULT_EP_KERNEL_MULTIBANCO				0x4D42			/*!< PPSE Application Selection - Defines a MultiBanco kernel (outside of the EMVCo range as this has not been yet defined by EMVCo). */
#endif // DEFAULT_EP_KERNEL_MULTIBANCO
///----------------------------------------------------------
#endif // DISABLE_OTHERS_KERNELS

#ifndef DISABLE_OTHERS_KERNELS

// VisaWave kernel includes
#include "VisaWave_API.h"
#include "VisaWave_Tags.h"

// ExpressPay kernel includes
#include "ExpressPay3_API.h"
#include "ExpressPay_Tags.h"

// Discover kernel includes
#include "Discover_API.h"
#include "Discover_Tags.h"

// DiscoverDPAS kernel includes
#include "DiscoverDPAS_API_Common.h"
#include "DiscoverDPAS_API.h"
#include "DiscoverDPAS_Tags.h"

// Interac kernel includes
#ifndef DISABLE_INTERAC
#include "Interac_API.h"
#include "Interac_Tags.h"
#endif

#ifndef DISABLE_PURE
// PURE kernel includes
//#include "PURE_API_Common.h"
//#include "PURE_Tags.h"
//#include "PURE_API.h"
#endif

#endif

// Cless Sample includes
#include "Cless_Tags.h"
#include "Cless_GuiState.h"
#include "Cless_DumpData.h"
#include "Cless_Receipt.h"
#include "Cless_Common.h"
#include "Cless_Scan.h"
#include "Cless_payWave.h"
#include "Cless_qPBOC.h"
#include "Cless_QuickPass.h"

#ifndef DISABLE_OTHERS_KERNELS
#include "Cless_VisaWave.h"
#include "Cless_PayPass.h"
#include "Cless_ExpressPay.h"
#include "Cless_Discover.h"
#include "Cless_DiscoverDPAS.h"
#ifndef DISABLE_INTERAC
#include "Cless_Interac.h"
#endif
#ifndef DISABLE_PURE
#include "Cless_PURE.h"
#endif
#endif

#include "Cless_Parameters.h"
#include "Cless_Fill.h"
#include "Cless_FinancialCommunication.h"
#include "Cless_Customisation.h"
#include "Cless_ExplicitSelection.h"
#include "Cless_Menu.h"
#include "Cless_Disk.h"
#include "Cless_BlackList.h"
#include "Cless_PinManagement.h"
#include "Cless_PinEnManagement.h"
#include "Cless_Term.h"
#include "Cless_Batch.h"
#include "Cless_Torn.h"
#include "Cless_TransactionFile.h"
#include "Cless_PinEntryGoal.h"
#include "Cless_Goal.h"
#include "UserInterfaceHelpers.h"
#include "MessagesDefinitions.h"
#include "Cless_XML.h"
#include "Cless_NameList.h"
#include "Cless_Settings.h"
//#include "Cless_TestPlan.h"
#include "Cless_GlobalTransactionData.h"
#include "servcomm.h"
#include "Cless_DataExchange.h"
#include "Cless_GuiState.h"

#ifndef __TELIUM3__
#include "tlvVar_def.h"
#include "schVar_def.h"
#endif


#ifndef DISABLE_UNATTENDED
//#include "UcmTelium.h"
//#include "UcmHostDLL.h"
//#include "UcmcLIB.h"
#endif
/////////////////////////////////////////////////////////////////
//// Macros & preprocessor definitions //////////////////////////

#define NUMBER_OF_ITEMS(Array)					(sizeof(Array)/sizeof((Array)[0]))

#define C_INDEX_TEST_BASE						0x0999 // See global range
#define SERVICE_CUSTOM_KERNEL					C_INDEX_TEST_BASE + 0x0000
#define SERVICE_DE_KERNEL						C_INDEX_TEST_BASE + 0x0001

#ifndef _EMVDCTAG_H

//#define  PROPRIETARY_METHOD						0x50			//!< Perform Proprietary method
//#define  NO_REMAINING_PIN							52				//!< PIN Try limit Exceeded.
//#define  INPUT_PIN_OFF							53				//!< Perform PIN Entry for offline verification.
#define  INPUT_PIN_ON								54				//!< Perform PIN Entry for online verification.
#define  INPUT_PIN_OK								55				//!< PIN entry performed.
#define  NO_INPUT									56				//!< CVM End.
#define  PP_HS										60				//!< Pinpad Out of Service.
#define  CANCEL_INPUT								61				//!< Pin entry cancelled.
#define  TO_INPUT  									62				//!< Time Out during PIN Entry.
//#define  REMOVED_CARD				 				63				//!< Card Removed.
//#define  ERROR_CARD								64				//!< Card Error.
//#define  MORE								 		65				//!< Internal use only.
//#define  ERROR_ENC_DATA							66				//!< Error during PIN Encipherment.
//#define  INPUT_INCORRECT_PIN						67              //!< Incorrect Pin Verification

#endif


//#ifndef DES_KEY_SIZE
//#define DES_KEY_SIZE	(8)
//#endif
#ifndef ERR_TIMEOUT
#define ERR_TIMEOUT						(-1032)		/*!< Scheme Error code */
#endif
#ifndef TLV_TYPE_KDES
#define TLV_TYPE_KDES					1
#endif
#ifndef CIPHERING_PIN
#define CIPHERING_PIN					2
#endif
#ifndef BK_SAGEM
#define BK_SAGEM						0x80000000
#endif

/////////////////////////////////////////////////////////////////
//// Types //////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////
//// Global variables ///////////////////////////////////////////


/////////////////////////////////////////////////////////////////
//// Functions //////////////////////////////////////////////////

extern long atol(const char *nptr);
#undef PSQ_Give_Language
#define PSQ_Give_Language	My_PSQ_Give_Language
int My_PSQ_Give_Language(void);


int __CLESS_CallAuthorisationHost(T_SHARED_DATA_STRUCT* dataStruct);
#endif // __CLESS_SAMPLE_IMPLEMENTATION_H__INCLUDED__
