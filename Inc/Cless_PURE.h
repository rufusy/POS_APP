/**
 * \file	Cless_PURE.h
 * \brief	Manages the interface with the PURE contactless kernel.
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
 */

#ifndef __CLESS_SAMPLE_PURE_H__INCLUDED__
#define __CLESS_SAMPLE_PURE_H__INCLUDED__

#ifndef DISABLE_PURE

/////////////////////////////////////////////////////////////////
//// Macros & preprocessor definitions //////////////////////////


/////////////////////////////////////////////////////////////////
//// Types //////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//// Global variables ///////////////////////////////////////////


/////////////////////////////////////////////////////////////////
//// Functions //////////////////////////////////////////////////

//! \brief Manage the debug mode for PURE kernel
//! \param[in] bActivate \a TRUE to activate the debug features. \a FALSE to deactivate features.

void Cless_PURE_DebugActivation (int bActivate);



//! \brief Perform the PURE kernel customisation.
//! \param[in,out] pSharedData Shared buffer used for customisation.
//! \param[in] ucCustomisationStep Step to be customised.
//! \return
//!		- \a KERNEL_STATUS_CONTINUE always.

int Cless_PURE_CustomiseStep (T_SHARED_DATA_STRUCT * pSharedData, const unsigned char ucCustomisationStep);



//! \brief Calls the PURE kernel to perform the transaction.
//! \param[in] pDataStruct Data buffer to be filled and used for PURE transaction.
//!	\return
//!		- PURE kernel result.

int Cless_PURE_PerformTransaction (T_SHARED_DATA_STRUCT * pDataStruct);

//! \brief Add Specific PUREParameters in relationship to AID before to use it.
//! \param[in] pShareStruct Share buffer given and modified by EntryPoint.
//! \param[in/out] pAidParameters parameters found to manage AID.
//! \return
//!		- \ref TRUE if correctly performed.
//!		- \ref FALSE if an error occurred.
int Cless_PURE_AidRelatedData(T_SHARED_DATA_STRUCT *pKernelDataStruct, T_SHARED_DATA_STRUCT * pAidParameters);


//! \brief Dump the PURE transaction data (for debug purpose).

void Cless_PURE_DumpTransationDataLog (void);



//! \brief Dump a specific PURE tag.
//! \param[in] ulTag Specific tag to be dumped.
//! \param[in] bRequestToKernelIfNotPresent Boolean that indicates if the tag must requested or not to the kernel if it is not present in pExchangeStruct.
//!	- \ref TAG_PURE_OUTCOME_PARAMETER_SET
//!	- \ref TAG_PURE_ERROR_INDICATION
//!	- \ref TAG_PURE_USER_INTERFACE_REQUEST_DATA
//!	- \ref TAG_PURE_DATA_RECORD
//!	- \ref TAG_PURE_DISCRETIONARY_DATA

void Cless_PURE_DumpSpecificStructure (T_SHARED_DATA_STRUCT * pExchangeStruct, unsigned long ulTag, int bRequestToKernelIfNotPresent);

#endif

#endif // __CLESS_SAMPLE_PURE_H__INCLUDED__
