/**
 * \file	Cless_Fill.h
 * \brief	This module is used to fill the structure with data necessary for transaction.
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

#ifndef __CLESS_SAMPLE_FILL_H__INCLUDED__
#define __CLESS_SAMPLE_FILL_H__INCLUDED__

/////////////////////////////////////////////////////////////////
//// Macros & preprocessor definitions //////////////////////////

#define C_SHARED_KERNEL_BUFFER_SIZE				16384		/*!< Maximum size of the shared buffer exchanged between this application and the kernels. */


/////////////////////////////////////////////////////////////////
//// Types //////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////
//// Global variables ///////////////////////////////////////////

extern T_SHARED_DATA_STRUCT * g_pKernelSharedBuffer;		/*!< Share exchange buffer used with the payment kernels. */


/////////////////////////////////////////////////////////////////
//// Functions //////////////////////////////////////////////////


//! \brief Gives transaction amount.
//! \return
//!		- \a transaction amount.
//! \note This function shall be called after having loaded the parameters.
unsigned long Cless_Fill_GiveAmount(void);

//! \brief Gives the money label.
//! \return
//!		- \a money label.
//! \note This function shall be called after having loaded the parameters.
unsigned char *Cless_Fill_GiveMoneyLabel(void);

//! \brief Gives the transaction type.
//! \return
//!		- \a transaction type.
//! \note This function shall be called after the transaction type has been populated.
unsigned char Cless_Fill_GiveTransactionType(void);

//! \brief Indicates if there is at least one Visa AID in the parametres.
//! \return
//!		- \a TRUE if at least one Visa AID is present.
//!		- \a FALSE if not.
//! \note This function shall be called after having loaded the parameters.
int Cless_Fill_IsThereVisaAid (void);

//! \brief Indicates if there is at least one PURE AID in the parametres.
//! \return
//!		- \a TRUE if at least one PURE AID is present.
//!		- \a FALSE if not.
//! \note This function shall be called after having loaded the parameters.

int Cless_Fill_IsTherePureAid (void);

//! \brief Indicates if there is only Amex AIDs in the parameters.
//! \return
//!		- \a TRUE if only Amex AIDs are present.
//!		- \a FALSE if not.
//! \note This function shall be called after having loaded the parameters.

int Cless_Fill_IsThereAmexAidOnly (void);

//! \brief Indicates if there is only Interac AIDs in the parameters.
//! \return
//!		- \a TRUE if only Interac AIDs are present.
//!		- \a FALSE if not.
//! \note This function shall be called after having loaded the parameters.

int Cless_Fill_IsThereInteracAidOnly (void);


//! \brief Save the some transaction only provided on CLESS_GIVE_INFO service.
//! \param[in] bAmountPresent Indicates if the transaction amount is present or not.
//! \param[in] ucTransactionType Indicates the transaction type to save.
//! \param[in] pCurrencyCode Indicates the transaction currency code to save (shall be coded on 2 bytes).
//! \param[in] ucCurrencyExponent Indicates the transaction type to save.
//! \param[in] pCurrencyLabel Indicates the currency label to save.
//! \param[in] ulAmountBin Indicates the transaction amount (binary format) to save.
//! \param[in] ulAmountOtherBin Indicates the transaction amount other (binary format) to save.

void Cless_Fill_PrepareAndSaveClessGiveInfoTransactionData (const unsigned char bAmountPresent, const unsigned char ucTransactionType, const unsigned char ucTransactionCashBack, const unsigned char * pCurrencyCode, const unsigned char ucCurrencyExponent, const unsigned char * pCurrencyLabel, const unsigned long ulAmountBin, const unsigned long ulAmountOtherBin);



//! \brief Init the shared buffre exchanged with the contactless kernels.
//! \param[in] pKernelSharedBuffer Shared exchange to be initialised.
//! \return
//!		- \ref TRUE if correctly performed.
//!		- \ref FALSE if an error occurred.

int Cless_Fill_InitSharedBufferWithKernels (T_SHARED_DATA_STRUCT ** pSharedBuffer);



//! \brief Fill output buffer with data for _give_info use.
//! \param[out] Output TLV Tree filled with all the necessary data for Application Selection and card detection.
//! \param[in] pParamTlvTree TLV Tree containing all the parameters (supported AIDs, etc).
//! \param[in] bExplicitSelection \a TRUE if Explicit Selection is used. \a FALSE if Implicit Selection is used.
//! \return
//!		- \ref TRUE if correctly performed.
//!		- \ref FALSE if an error occurred.

int Cless_Fill_GetApplicationSelectionParameters (TLV_TREE_NODE * pOuputTlvTree, TLV_TREE_NODE * pParamTlvTree, const unsigned int bExplicitSelection);



//! \brief Fill the output CLESS_GIVE_INFO DEL with the serialisez TLV Tree containing all the necessary data for application selection.
//! \param[out] param_out Output structure provided on the CLESS_GIVE_INFO service.
//! \param[in] pParamTlvTree TLV Tree containing all the parameters (supported AIDs, etc).
//! \return
//!		- \ref TRUE if correctly performed.
//!		- \ref FALSE if an error occurred.

int Cless_Fill_GiveInfo (S_CLESS_GIVEINFO * param_out, TLV_TREE_NODE * pParamTlvTree);



//! \brief Initialise the shared exchange buffer to be used with the kernel.
//! \param[out] pDataStruct Shared exchange structure filled with the generic transaction data.
//! \return
//!		- \ref TRUE if correctly performed.
//!		- \ref FALSE if an error occurred.

int Cless_Fill_TransactionGenericData (T_SHARED_DATA_STRUCT * pDataStruct);



//! \brief Get the correct AID parameters (to perform the transaction) according to the application selection result.
//! \param[out] pKernelDataStruct Shared buffer to be filled with the correct AID parameters (depending on the Application Selection results).
//! \param[in] pSharedStruct Shared buffer containing the application selection results.
//! \param[out] pKernelToUse Indicates the kernel to be used for the transaction.
//! \return
//!		- \ref TRUE if correctly performed.
//!		- \ref FALSE if an error occurred.

int Cless_Fill_AidRelatedData (T_SHARED_DATA_STRUCT * pKernelDataStruct, T_SHARED_DATA_STRUCT * pSharedStruct, int * pKernelToUse);


//T_SHARED_DATA_STRUCT * Cless_Fill_CommonTransaction (T_SHARED_DATA_STRUCT * pDataStruct, int * pKernelToUse);




#endif // __CLESS_SAMPLE_FILL_H__INCLUDED__
