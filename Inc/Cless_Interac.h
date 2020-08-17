/**
* \file	Cless_Interac.h
* \brief	Manages the interface with the Interac contactless kernel.
*
* \author ------------------------------------------------------------------------------\n
* \author INGENICO Technical Software Department\n
* \author ------------------------------------------------------------------------------\n
* \author Copyright (c) 2015 - 2015 INGENICO.\n
* \author 28-32 boulevard de Grenelle 75015 Paris, France.\n
* \author All rights reserved.\n
* \author This source program is the property of the INGENICO Company mentioned above\n
* \author and may not be copied in any form or by any means, whether in part or in whole,\n
* \author except under license expressly granted by such INGENICO company.\n
* \author All copies of this program, whether in part or in whole, and\n
* \author whether modified or not, must display this and all other\n
* \author embedded copyright and ownership notices in full.\n
**/

#ifndef __CLESS_SAMPLE_INTERAC_H__INCLUDED__
#define __CLESS_SAMPLE_INTERAC_H__INCLUDED__

#ifndef DISABLE_INTERAC

/////////////////////////////////////////////////////////////////
//// Macros & preprocessor definitions //////////////////////////


// Interac not ready yet
#define Interac_Clear()
#define Interac_DebugManagement(p1)         KERNEL_STATUS_OK
#define Interac_DoTransaction(p1)			KERNEL_STATUS_OFFLINE_DECLINED
#define Interac_GetData(p1)                 KERNEL_STATUS_OK
#define Interac_GetAllData(p1)              KERNEL_STATUS_OK


/////////////////////////////////////////////////////////////////
//// Types //////////////////////////////////////////////////////
#define CLESS_SAMPLE_MODE_INTERAC			1		/*!< Interac Sample GUI mode. */

#define LANGUAGE_SELECTED_NO		0
#define LANGUAGE_SELECTED_YES		1
#define LANGUAGE_SELCTED_IDENTICAL	2
/////////////////////////////////////////////////////////////////
//// Global variables ///////////////////////////////////////////


/////////////////////////////////////////////////////////////////
//// Functions //////////////////////////////////////////////////

//! \brief Manage the debug mode for Interac kernel
//! \param[in] bActivate \a TRUE to activate the debug features. \a FALSE to deactivate features.

void Cless_Interac_DebugActivation (int bActivate);



//! \brief Perform the Interac kernel customisation.
//! \param[in,out] pSharedData Shared buffer used for customisation.
//! \param[in] ucCustomisationStep Step to be customised.
//! \return
//!		- \a KERNEL_STATUS_CONTINUE always.

int Cless_Interac_CustomiseStep (T_SHARED_DATA_STRUCT * pSharedData, const unsigned char ucCustomisationStep);



//! \brief Calls the Interac kernel to perform the transaction.
//! \param[in] pDataStruct Data buffer to be filled and used for Interac transaction.
//!	\return
//!		- Interac kernel result.

int Cless_Interac_PerformTransaction (T_SHARED_DATA_STRUCT * pDataStruct);

//! \brief Check in the parameter file if the GUI mode to use is Interac.
void Cless_Interac_SetInteracGuiMode (void);

//! \brief Check if the Interac GUI mode is activated.
//!	\return
//!		- TRUE if Interac GUI mode is used.
//!		- FALSE else.
int Cless_Interac_IsInteracGuiMode (void);

//! \brief Get the timeout value for card detection process from the parameter file.
//! \param[out] pNoCardTimeOut the timeout value (in milliseconds).
//! \return
//!		- TRUE if tag is present.
//!		- FALSE else.
int Cless_Interac_GetNoCardTimeOut (unsigned long *pNoCardTimeOut);


//! \brief Initialize the timer when card detection starts.
void Interac_StartDetectionTimer (void);


//! \brief Get the timer value.
//!	\return The tick value.
unsigned long Interac_GetDetectionTimer (void);


//! \brief Set the variable to display or not the "NO CARD" message when timeout elapsed.
//! \param[in] bValue : TRUE -> message will be displayed, FALSE -> message will not be displayed
void Interac_SetDisplayNoCard (int bValue);


//! \brief Get the variable to display or not the "NO CARD" message when timeout elapsed.
//!	\return
//!		- TRUE message will be displayed.
//!		- FALSE message will not be displayed.
int Interac_GetDisplayNoCard (void);


//! \brief Check in the parameter file if the GUI mode to use is Interac.
void Cless_Interac_SetInteracGuiMode (void);


//! \brief Check if the Interac GUI mode is activated.
//!	\return
//!		- TRUE if Interac GUI mode is used.
//!		- FALSE else.
int Cless_Interac_IsInteracGuiMode (void);

#endif

#endif // __CLESS_SAMPLE_INTERAC_H__INCLUDED__
