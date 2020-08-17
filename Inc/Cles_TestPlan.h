/**
 * \file	Cless_Disk.h
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

#ifndef __CLESS_SAMPLE_TEST_PLAN_H__INCLUDED__
#define __CLESS_SAMPLE_TEST_PLAN_H__INCLUDED__

/////////////////////////////////////////////////////////////////
//// Macros & preprocessor definitions //////////////////////////

#define TEST_PLAN_TAG_NAME					0x9900
#define TEST_PLAN_TAG_TERMINAL_SETTING		0x9901
#define TEST_PLAN_TAG_KERNEL_SETTINGS		0x9902
#define TEST_PLAN_TAG_AUTORUN				0x9903


/////////////////////////////////////////////////////////////////
//// Types //////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//// Global variables ///////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//// Functions //////////////////////////////////////////////////

/**
 * Erase TLV Tree containing the loaded test plan.
 */
void CS_TestPlan_Erase (void);



/**
 * Display a menu in order the operator can choose the test case to be used to perform the transaction. It also load all the test case related data
 * into the shared exchange that can be used later to get data to perform the transaction.
 * @return
 * - \a TRUE if successfully performed.
 * - \a FALSE if an error occurred.
 */
int CS_TestPlan_ChooseAndLoad (void);



/**
 * Copy data into provided shared exchange structure.
 * @param pStruct Destination structure.
 * @return
 * - \a TRUE if correctly completed.
 * - \a FALSE if an error occurred.
 */
int CS_TestPlan_CopyToSharedStruct (T_SHARED_DATA_STRUCT * pStruct);



#endif // __CLESS_SAMPLE_TEST_PLAN_H__INCLUDED__
