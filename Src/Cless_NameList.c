//! \file
//! Module that manages the terminal settings archive.
//
/////////////////////////////////////////////////////////////////
//// Includes ///////////////////////////////////////////////////

#include "Cless_Implementation.h"


/////////////////////////////////////////////////////////////////
//// Macros & preprocessor definitions //////////////////////////


/////////////////////////////////////////////////////////////////
//// Types //////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////
//// Global variables ///////////////////////////////////////////


/////////////////////////////////////////////////////////////////
//// Static function definitions ////////////////////////////////


/////////////////////////////////////////////////////////////////
//// Functions //////////////////////////////////////////////////


/**
 * Initialize the list of names.
 * @param list List of names to be initialized.
 * @param maxNumberOfEntries Max number of entries to be set for this list.
 */
void CS_NameList_Init (T_NAMES_LIST * list, int maxNumberOfEntries) {
	int index;

	if (list != NULL) {
		// If already initialized, erase existing list
		if (list->isInitialized == TRUE) {
			for (index = 0; index < list->numberOfEntries; index++) {
				ufree (list->pListOfNames[index]);
				list->pListOfNames[index] = NULL;
			}

			ufree (list->pListOfNames);
		}

		// Set the new maximum number of entries
		list->maxNumberOfEntries = maxNumberOfEntries;

		// Allocate the necessary size in the list
		list->numberOfEntries = 0;
		list->pListOfNames = NULL;
		list->isInitialized = FALSE;

		if (maxNumberOfEntries > 0) {
			list->pListOfNames = (char **) umalloc (maxNumberOfEntries * sizeof(char*));
			if (list->pListOfNames != NULL) {
				for (index = 0; index < list->maxNumberOfEntries; index++) {
					// Initialize each entry to NULL in the list
					list->pListOfNames[index] = NULL;
				}

				// The list is now initialized
				list->isInitialized = TRUE;
			}
		}
	}
}



/**
 * Destroy the list of names.
 * @param list List of names to be destroyed. It is set to NULL when destroyed.
 */
void CS_NameList_Destroy (T_NAMES_LIST ** list) {
	if (list != NULL) {
		if (*list != NULL) {
			CS_NameList_Init (*list, 0);
			*list = NULL;
		}
	}
}



/**
 * Add a new name into the list.
 * @param list List to be filled.
 * @param name Name to be added.
 * @return
 * - \a TRUE if list successfully added.
 * - \a FALSE if an error occurred.
 */
int CS_NameList_AddItem (T_NAMES_LIST * list, const char * name) {
	int result = FALSE;
	int nameLength;

	if (list)
	{
		if (name != NULL)
		{
			if (list->numberOfEntries < list->maxNumberOfEntries)
			{
				nameLength = strlen(name);
				list->pListOfNames[list->numberOfEntries] =  umalloc (nameLength + 1);
				if (list->pListOfNames[list->numberOfEntries] != NULL)
				{
					memcpy(list->pListOfNames[list->numberOfEntries], name, nameLength);
					list->pListOfNames[list->numberOfEntries][nameLength] = '\0';
					list->numberOfEntries++;
					result = TRUE;
				}
				else
				{
					GTL_Traces_TraceDebug("CS_NameList_AddItem: Cannot allocate memory to store the name in the list");
				}
			}
			else
			{
				GTL_Traces_TraceDebug("CS_NameList_AddItem: List is full");
			}
		}
		else
		{
			GTL_Traces_TraceDebug("CS_NameList_AddItem: Cannot add a NULL name in the list");
		}
	}
	else
	{
		GTL_Traces_TraceDebug("CS_NameList_AddItem: List is NULL");
	}

	return (result);
}



/**
 * Print the list of names on the output.
 * @param list List to be printed.
 */
void CS_NameList_Print (T_NAMES_LIST * list) {
	int index;

	if (list) {
		Cless_DumpData_DumpTitle((unsigned char *)"List:");

		if (Cless_DumpData_DumpOpenOutputDriver()) {
			//if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER)
			//	Cless_DumpData("\x0F");


			for (index = 0; index<list->numberOfEntries; index++) {
				Cless_DumpData ("%02d: %s", index+1, list->pListOfNames[index]);
				Cless_DumpData_DumpNewLine();
			}

			//if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER)
			//	Cless_DumpData ("\x1b" "@");

			Cless_DumpData_DumpCloseOutputDriver();
		}
	} else {
		Cless_DumpData_DumpTitle((unsigned char *)"!!! list = NULL !!!");
		GTL_Traces_TraceDebug("CS_NameList_Print: List is NULL");
	}
}
