/**
 * \file	Cless_NameList.h
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

#ifndef __CLESS_SAMPLE_NAME_LIST_H__INCLUDED__
#define __CLESS_SAMPLE_NAME_LIST_H__INCLUDED__

/////////////////////////////////////////////////////////////////
//// Macros & preprocessor definitions //////////////////////////

/////////////////////////////////////////////////////////////////
//// Types //////////////////////////////////////////////////////

typedef struct T_NAMES_LIST
{
	int isInitialized;			//!< Indicates if the list is initialized
	int numberOfEntries;        //!< Indicates the number of entries in the list.
	int maxNumberOfEntries;     //!< Indicates the maximum number of entries in the list.
	char ** pListOfNames ;      //!< Contains the entries.
} T_NAMES_LIST;


/////////////////////////////////////////////////////////////////
//// Global variables ///////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//// Functions //////////////////////////////////////////////////

/**
 * Initialize the list of names.
 * @param list List of names to be initialized.
 * @param maxNumberOfEntries Max number of entries to be set for this list.
 */
void CS_NameList_Init (T_NAMES_LIST * list, int maxNumberOfEntries);



/**
 * Destroy the list of names.
 * @param list List of names to be destroyed. It is set to NULL when destroyed.
 */
void CS_NameList_Destroy (T_NAMES_LIST ** list);



/**
 * Add a new name into the list.
 * @param list List to be filled.
 * @param name Name to be added.
 * @return
 * - \a TRUE if list successfully added.
 * - \a FALSE if an error occurred.
 */
int CS_NameList_AddItem (T_NAMES_LIST * list, const char * name);


/**
 * Print the list of names on the output.
 * @param list List to be printed.
 */
void CS_NameList_Print (T_NAMES_LIST * list);


#endif // __CLESS_SAMPLE_NAME_LIST_H__INCLUDED__
