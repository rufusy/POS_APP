/**
* \file
* \brief This module implements the EMV API functionalities related to the Application Selection.
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

#ifndef EMV_SERVICES_EMV_APPSEL_H_INCLUDED
#define EMV_SERVICES_EMV_APPSEL_H_INCLUDED

/////////////////////////////////////////////////////////////////
//// Macros & preprocessor definitions //////////////////////////

//// Types //////////////////////////////////////////////////////

//// Static function definitions ////////////////////////////////

//// Global variables ///////////////////////////////////////////

//// Functions //////////////////////////////////////////////////

void EMV_ServicesEmv_MenuSelect(EMVAS_Object_t AS_object, TLV_TREE_NODE outputTlvTree);
EMV_Status_t EMV_ServicesEmv_ApplicationSelection(EMVAS_Object_t AS_object);

#endif // EMV_SERVICES_EMV_APPSEL_H_INCLUDED
