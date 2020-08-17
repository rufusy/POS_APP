/**
* \file
* \brief This module manages the user interface.
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

#ifndef EMV_USER_INTERFACE_DISPLAY_H_INCLUDED
#define EMV_USER_INTERFACE_DISPLAY_H_INCLUDED

#include "EPSTOOL_PinEntry.h"

/////////////////////////////////////////////////////////////////
//// Macros & preprocessor definitions //////////////////////////

//// Types //////////////////////////////////////////////////////

//! brief Structure that contains all necessary information of an AID. Used to display a menu to select an AID.
typedef struct
{
	int aidLength;							//!< The length of the AID.
	unsigned char aid[32];					//!< The AID.
	char applicationLabel[32 + 1];			//!< The Application Label.
	unsigned char issuerCodeTableIndex;		//!< The Issuer Code Table Index.
	char preferredName[32 + 1];				//!< The Application Preferred Name.
} EMV_UI_SelectAidItem_t;

//// Static function definitions ////////////////////////////////

//// Global variables ///////////////////////////////////////////

//// Functions //////////////////////////////////////////////////

int EMV_UI_TransactionBegin(void);
void EMV_UI_TransactionEnd(int previous);

void EMV_UI_MessageInsertCard(void);
void EMV_UI_MessageRemoveCard(void);
void EMV_UI_MessagePleaseWait(void);
void EMV_UI_MessageNoRemainingPin(void);
void EMV_UI_MessageWrongPin(void);
void EMV_UI_MessagePinLastAttempt(void);
void EMV_UI_MessageCorrectPin(void);
void EMV_UI_MessageApproved(void);
void EMV_UI_MessageDeclined(void);
void EMV_UI_MessageServiceNotAllowed(void);
void EMV_UI_MessageTransactionCancelled(void);
void EMV_UI_MessageCardBlocked(void);
void EMV_UI_MessageCardRemoved(void);
void EMV_UI_MessageCardError(void);
void EMV_UI_MessageTransactionError(void);

void EMV_UI_MessageDisplayError(char * DisplayData);

int EMV_UI_MessageAuthorisation(void);
void EMV_UI_RequestSignature(void);

int EMV_UI_MenuSelectAid(int numOfAids, const EMV_UI_SelectAidItem_t *aids);
int EMV_UI_MenuSelectAccountType(unsigned char *accountType);
int EMV_UI_MenuForceAcceptance(void);
int EMV_UI_AmountEntry(unsigned long *amount);

EPSTOOL_PinEntry_Status_e EMV_UI_PinEntry(int pinType, int pinTryCounter, int *pinLength);

#endif // EMV_USER_INTERFACE_DISPLAY_H_INCLUDED
