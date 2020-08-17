/**
* \file
* \brief This module implements the EMV API functionalities related to online processing
* (Authorisation, Referral, Online Processing, Issuer Scripts and the 2nd Generate AC).
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

#ifndef EMV_SERVICES_EMV_ONLINE_PROCESSING_H_INCLUDED
#define EMV_SERVICES_EMV_ONLINE_PROCESSING_H_INCLUDED

/////////////////////////////////////////////////////////////////
//// Macros & preprocessor definitions //////////////////////////

//// Types //////////////////////////////////////////////////////

//// Static function definitions ////////////////////////////////

//// Global variables ///////////////////////////////////////////

//// Functions //////////////////////////////////////////////////

void EMV_ServicesEmv_Authorisation( TLV_TREE_NODE outputTlvTree);
void EMV_ServicesEmv_VoiceReferral(TLV_TREE_NODE inputTlvTree, TLV_TREE_NODE outputTlvTree);
EMV_Status_t EMV_ServicesEmv_OnlineProcessing(EMV_Object_t EMV_object, TLV_TREE_NODE inputTlvTree,
		TLV_TREE_NODE *tagsKernelToAppli, TLV_TREE_NODE *tagsRequestedByKernel);
EMV_Status_t EMV_ServicesEmv_ISP1(EMV_Object_t EMV_object, TLV_TREE_NODE inputTlvTree,
		TLV_TREE_NODE *tagsKernelToAppli, TLV_TREE_NODE *tagsRequestedByKernel);
EMV_Status_t EMV_ServicesEmv_Completion(EMV_Object_t EMV_object, TLV_TREE_NODE inputTlvTree,
		TLV_TREE_NODE *tagsKernelToAppli, TLV_TREE_NODE *tagsRequestedByKernel);
EMV_Status_t EMV_ServicesEmv_ISP2(EMV_Object_t EMV_object, TLV_TREE_NODE inputTlvTree,
		TLV_TREE_NODE *tagsKernelToAppli, TLV_TREE_NODE *tagsRequestedByKernel);

#endif // EMV_SERVICES_EMV_ONLINE_PROCESSING_H_INCLUDED
