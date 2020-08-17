/**
 * \file	Cless_GlobalTransactionData.h
 * \brief 	Manages the application Disk access.
 *
 * \author	Ingenico
 * \author	Copyright (c) 2009 Ingenico, rue claude Chappe,\n
 *			07503 Guilherand-Granges, France, All Rights Reserved.
 *
 * \author	Ingenico has intellectual property rights relating to the technology embodied \n
 *			in this software. In particular, and without limitation, these intellectual property rights may\n
 *			include one or more patents.\n
 *			This software is distributed under licenses restricting its use, copying, distribution, and\n
 *			and decompilation. No part of this software may be reproduced in any form by any means\n
 *			without prior written authorization of Ingenico.
 **/

#ifndef __CLESS_SAMPLE_GLOBAL_TXN_DATA_H__INCLUDED__
#define __CLESS_SAMPLE_GLOBAL_TXN_DATA_H__INCLUDED__

/////////////////////////////////////////////////////////////////
//// Macros & preprocessor definitions //////////////////////////

/////////////////////////////////////////////////////////////////
//// Types //////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//// Global variables ///////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//// Functions //////////////////////////////////////////////////

/**
 * Load all the global transaction data into a shared exchange structure.
 * @param bUseTestPlan Indicates if the test plan shall be used or not.
 * @param bUseDefaultAmount Indicates if the default amount shall be used or not.
 * @return
 * - \a TRUE if correctly completed.
 * - \a FALSE if an error occurred.
 */
int CS_GlobalTransactionData_Prepare (int bUseTestPlan, int bUseDefaultAmount);


/**
 * Retrieve the transaction amount, authorized
 * @param amount Filled with the amount. 0 if an error occurred.
 * @return
 * - \a TRUE if correctly completed.
 * - \a FALSE if an error occurred.
 */
int CS_GlobalTransactionData_GetAmountAuthorized (unsigned long * amount);



/**
 * Retrieve the transaction type.
 * @param amount Filled with the transaction type. CLESS_SAMPLE_TRANSACTION_TYPE_DEFAULT if an error occurred or if the tag is not found.
 * @return
 * - \a TRUE if correctly completed.
 * - \a FALSE if an error occurred.
 */
int CS_GlobalTransactionData_GetTransactionType (unsigned char * transactionType);



/**
 * Copy the global transaction data into the provided shared exchange structure.
 * @param pDestination Structure to be filled. Note it is clear if any error occurred.
 * @return
 * - \a TRUE if correctly performed.
 * - \a FALSE if an error occurred.
 */
int CS_GlobalTransactionData_CopyToSharedExchange (T_SHARED_DATA_STRUCT * pDestination);



/**
 * Retrieve the transaction type.
 * @param currencyLabel Filled with the pointer of the currency label.
 * @return
 * - \a TRUE if data correctly initialized.
 * - \a FALSE if not (an in this case, currency is probably not correct or empty).
 */
int CS_GlobalTransactionData_GetCurrencyLabel (char ** currencyLabel);



/**
 * Checks if the tag is present and not empty in the global transaction data.
 * @param ulTag Tag to be checked.
 * @return
 * - \a TRUE if the tag is present.
 * - \a FALSE else.
 */
int CS_GlobalTransactionData_IsTagPresentAndNotEmpty (unsigned long ulTag);



/**
 * Load the transaction global data from an external structure (typically used when data are extracted from a test plan file).
 * @param pStruct Data to be loaded in the global transaction data structure.
 * @return
 * - \a TRUE if correctly completed.
 * - \a FALSE if an error occurred.
 */
//int CS_GlobalTransactionData_LoadFromExternalData (T_SHARED_DATA_STRUCT * pStruct);



#endif // __CLESS_SAMPLE_GLOBAL_TXN_DATA_H__INCLUDED__
