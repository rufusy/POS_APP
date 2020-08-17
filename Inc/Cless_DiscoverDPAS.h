/**
* \file   Cless_DiscoverDPAS.h
* \brief  Discover DPAS functionality
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

#ifndef Cless_DISCOVERDPAS_H_
#define Cless_DISCOVERDPAS_H_

#ifdef __cplusplus
extern "C" {
#endif



/////////////////////////////////////////////////////////////////
//// Macros & preprocessor definitions //////////////////////////


/////////////////////////////////////////////////////////////////
//// Types //////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////
//// Global variables ///////////////////////////////////////////


/////////////////////////////////////////////////////////////////
//// Functions //////////////////////////////////////////////////

//! \brief Manage the debug mode for Discover kernel
//! \param[in] bActivate \a TRUE to activate the debug features. \a FALSE to deactivate features.

void Cless_DiscoverDPAS_DebugActivation (int bActivate);



//! \brief Perform the Discover kernel customisation.
//! \param[in,out] pSharedData Shared buffer used for customisation.
//! \param[in] ucCustomisationStep Step to be customised.
//! \return
//!		- \a KERNEL_STATUS_CONTINUE always.

int Cless_DiscoverDPAS_CustomiseStep (T_SHARED_DATA_STRUCT * pSharedData, const unsigned char usCustomisationStep);



//! \brief Calls the Discover kernel to perform the transaction.
//! \param[in] pDataStruct Data buffer to be filled and used for Discover transaction.
//!	\return
//!		- Discover kernel result.

int Cless_DiscoverDPAS_PerformTransaction (T_SHARED_DATA_STRUCT * pDataStruct);

#ifdef __cplusplus
}
#endif



#endif /* Cless_DISCOVERDPAS_H_ */
