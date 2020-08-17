/**
* \file
* \brief This module implements main the EMV API functionalities.
*
* \author Ingenico
* \author Copyright (c) 2012 Ingenico, 28-32, boulevard de Grenelle,\n
* 75015 Paris, France, All Rights Reserved.
*
* \author Ingenico has intellectual property rights relating to the technology embodied\n
* in this software. In particular, and without limitation, these intellectual property rights may\n
* include one or more patents.\n
* This software is distributed under licenses restricting its use, copying, distribution, and\n
* and decompilation. No part of this software may be reproduced in any form by any means\n
* without prior written authorisation of Ingenico.
**/

#ifndef EMV_SERVICES_EMV_H_INCLUDED
#define EMV_SERVICES_EMV_H_INCLUDED

#include "EMV_Status.h"

/////////////////////////////////////////////////////////////////
//// Macros & preprocessor definitions //////////////////////////

//// Types //////////////////////////////////////////////////////

//! \brief Status of the transaction.
typedef enum
{
	EMV_TR_STATUS_UNKNOWN,				//!< Unknown status.

	EMV_TR_STATUS_APPROVED,				//!< The transaction is approved.
	EMV_TR_STATUS_DECLINED,				//!< The transaction is declined.

	EMV_TR_STATUS_SERVICE_NOT_ALLOWED,	//!< The service is not allowed.
	EMV_TR_STATUS_CANCELLED,				//!< The transaction is cancelled.

	EMV_TR_STATUS_CARD_BLOCKED,			//!< The card is blocked.
	EMV_TR_STATUS_CARD_REMOVED,			//!< The transaction is removed.

	EMV_TR_STATUS_TERMINAL_ERROR,			//!< There is an error that comes from the terminal.
	EMV_TR_STATUS_CARD_ERROR,				//!< There is an error that comes from the card.

	EMV_TR_STATUS_END
} EMV_ServicesEmv_TransactionStatus_e;

//// Static function definitions ////////////////////////////////

//// Global variables ///////////////////////////////////////////

//// Functions //////////////////////////////////////////////////

EMV_Status_t EMV_ServicesEmv_DoTransaction(const char *cardReader, unsigned int cardReaderBufferSize, int doEmvAppliSelection, TLV_TREE_NODE inputTlvTree);
int EMV_ServicesEmv_GetTransactionData(TLV_TREE_NODE outputTlvTree);

int EMV_ServicesEmv_AmountIsSet(void);
int EMV_ServicesEmv_AmountSet(unsigned long long amount, TLV_TREE_NODE outputTlvTree);

EMV_ServicesEmv_TransactionStatus_e EMV_ServicesEmv_TransacStatusGet(void);
void EMV_ServicesEmv_TransacStatusChange(EMV_ServicesEmv_TransactionStatus_e status);
void EMV_ServicesEmv_TransacStatusSet(EMV_Status_t emvStatus, TLV_TREE_NODE transactionData);

void EMV_ServicesEmv_GetGlobalParam(TLV_TREE_NODE outputTlvTree);

void EMV_ServicesEmv_WaitCard(void);

void EMV_ServicesEmv_RemoveCard(void);

#endif // EMV_SERVICES_EMV_H_INCLUDED
