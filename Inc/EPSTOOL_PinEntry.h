/**
* \file
* \brief This module implements the PIN entry common treatments.
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

#ifndef EPSTOOL_PINENTRY_H_INCLUDED
#define EPSTOOL_PINENTRY_H_INCLUDED

/////////////////////////////////////////////////////////////////
//// Macros & preprocessor definitions //////////////////////////

//// Types //////////////////////////////////////////////////////

//! \brief Possible results of a PIN entry.
typedef enum
{
	EPSTOOL_PINENTRY_SUCCESS,	//< The PIN has been correctly entered.
	EPSTOOL_PINENTRY_BYPASS,	//< The cardholder requested a PIN Bypass.
	EPSTOOL_PINENTRY_CANCEL,	//< The cardholder cancelled the PIN entry.
	EPSTOOL_PINENTRY_EVENT,		//< An external user event occurred.
	EPSTOOL_PINENTRY_TIMEOUT,	//< The PIN entry exited due to a timeout.
	EPSTOOL_PINENTRY_INVALID,	//< The entered PIN is invalid (too short).
	EPSTOOL_PINENTRY_ERROR,		//< An error occurred.

	EPSTOOL_PINENTRY_END
} EPSTOOL_PinEntry_Status_e;

//! \brief PIN entry parameters.
typedef struct EPSTOOL_PinEntry_Infos_t
{
	T_SEC_ENTRYCONF pinEntryConfig;		//!< Configuration of the scheme.
	T_SEC_PARTTYPE secureType;			//!< Indication on where the PIN must be stored.
	int pinBypassAllowed;				//!< \a TRUE to allow PIN Bypass, \a FALSE to disable this feature.
	unsigned long eventsToWait;			//!< Some external user events that can cancel the PIN entry (contact card removal...)

	//! \brief Called when one or several external user event occur. Can be \a NULL.
	//! \param[in] interface PIN entry parameters.
	//! \param[in] label Handle of the GOAL widget that is used to display the PIN entry.
	//! \param[in] events Events that occurred.
	//! \param[in] pinLength Current length of the entered PIN.
	//! \return \a TRUE to continue the PIN entry or \a FALSE to abort it. If no function is given (\a NULL pointer), any user event stops the PIN entry.
	int (*userEvents)(struct EPSTOOL_PinEntry_Infos_t *interface, T_GL_HWIDGET label, unsigned long events, int pinLength);

	//! \brief Called when the display must be updated.
	//! \param[in] interface PIN entry parameters.
	//! \param[in] label Handle of the GOAL widget that is used to display the PIN entry.
	//! \param[in] pinLength Current length of the entered PIN.
	void (*refresh)(struct EPSTOOL_PinEntry_Infos_t *interface, T_GL_HWIDGET label, int pinLength);

	void *privateData;					//!< Private data that can be used in \a userEvents and \a refresh.
} EPSTOOL_PinEntry_Infos_t;

//// Global variables ///////////////////////////////////////////

//// Functions //////////////////////////////////////////////////

EPSTOOL_PinEntry_Status_e EPSTOOL_PinEntry(T_GL_HGRAPHIC_LIB goalHandle, const char *title, const char *text, const char *help, EPSTOOL_PinEntry_Infos_t *pinEntryInfo, int *pinLength);

#endif // EPSTOOL_PINENTRY_H_INCLUDED
