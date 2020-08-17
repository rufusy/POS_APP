/**
 * \file	Cless_ExplicitSelection.h
 * \brief 	Perform a transaction in explicit selection (without interaction with the Manager).
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


/////////////////////////////////////////////////////////////////////////////////////
//// Macros & preprocessor definitions //////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////
//// Types //////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////
//// Global data definitions ////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////
//// Functions //////////////////////////////////////////////////////////////////////

//! \brief Function called for to perform explicit transaction (card detection and application selection).
//! \param[in] ucTransactionType Indicates the transaction type (debit or refund).
//! \param[in] ucAmountType Indicates the amount to manage : CLESS_SAMPLE_AMOUNT_TYPE_BIN or CLESS_SAMPLE_AMOUNT_TYPE_OTHER_BIN.
int Cless_ExplicitSelection_Process (const unsigned char ucTransactionType, const char ucAmountType);

//! \brief Requests amount entry, prepare frame for Entry Point (fill shared memory)
//! \param[in] pCurrencyLabel : the currency label
//! \param[out] pAmountBin : the entered transaction amount
//! \param[out] pFormattedAmountMessage : the formatted message to display
//! \return 
//!		- TRUE if amount has been entered.
//!		- FALSE if amount entry has been cancelled.

int Cless_ExplicitSelection_AmountEntry (const unsigned char *pCurrencyLabel, unsigned long * pAmountBin, unsigned char *pFormattedAmountMessage);
