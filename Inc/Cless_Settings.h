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

#ifndef __CLESS_SAMPLE_SETTINGS_H__INCLUDED__
#define __CLESS_SAMPLE_SETTINGS_H__INCLUDED__

/////////////////////////////////////////////////////////////////
//// Macros & preprocessor definitions //////////////////////////

/////////////////////////////////////////////////////////////////
//// Types //////////////////////////////////////////////////////

typedef enum
{
	SettingsDecoderTeliumXml	= 0,
	SettingsDecoderDekDet		= 1,
} T_SETTINGS_DECODER;

/////////////////////////////////////////////////////////////////
//// Global variables ///////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//// Functions //////////////////////////////////////////////////

/**
 * Gets the list of term settings names.
 * @param archiveName Name of the archive from which you want to get the list of files.
 * @param list List of names to be filled.
 * @return
 * - \a TRUE if list successfully retrieved.
 * - \a FALSE if an error occurred.
 */
int CS_Settings_getListOfFiles (const char * archiveName, T_NAMES_LIST * list);


/**
 * Select a file by its name and loads the parameter file into memory for future use.
 * @param[in] archiveName Name of the archive, without the extension (e.g. "EP_SETTINGS").
 * @param[in] fileName Name of the file to be selected (with the extension: ".PAR" or ".PAR.gz").
 * @param[in] nDecoderType Indicates the decoder type.
 * @param[in] fileExtension Indicates the file extension. "" if no extension.
 * @return
 * - true if the selected file is valid and successfully loaded.
 * - false otherwise.
 * @note The archive shall be loaded in the terminal HOST directory.
 */
int CS_Settings_loadFromFile (const char * archiveName, const char * fileName, TLV_TREE_NODE * loadedSettingTree, T_SETTINGS_DECODER decoderType, const char * fileExtension);


/**
 * Save the name of the file name to used for a given archiveName into a dedicated file named "archiveName.SAV".
 * @param archiveName Archive name.
 * @param fileName File name to be saved.
 * @return
 * - \a TRUE if list successfully saved.
 * - \a FALSE if an error occurred.
 */
int CS_Settings_SaveSelectedFileName (const char * archiveName, const char * fileName);


/**
 * Erase the file containing the saved file name.
 * @param archiveName Archive name.
 * @param fileName File name to be saved.
 */
void CS_Settings_DeleteSavedSelectedFileName (const char * archiveName);


/**
 * Check if a term setting file is present or not in the archive.
 * @param archiveName Archive name.
 * @param fileName fileName Name of the file to be selected (with the extension: ".PAR" or ".PAR.gz").
 * @return
 * - true if the selected file is valid and successfully loaded.
 * - false otherwise.
 * @note The archive shall be loaded in the terminal HOST directory.
 */
int CS_Settings_IsFileExisting (const char * archiveName, const char * fileName);


/**
 * Restore the parameter file saved in the archiveName.SAV file.
 * @param archiveName Archive name.
 * @param loadedSettingTree TLV Tree filled with the extracted parameters, if the restoration succeeded.
 * @param decoderType Indicates the decoder type.
 * @param fileExtension Indicates the file extension. "" if no extension.
 * @return
 * - \a TRUE if list successfully saved.
 * - \a FALSE if an error occurred.
 */
int CS_Settings_Restore (const char * archiveName, TLV_TREE_NODE * loadedSettingTree, T_SETTINGS_DECODER decoderType, const char * fileExtension);


/**
 * Returns the index of the saved parameter file in the list.
 * @param archiveName Archive name.
 * @return The index of the item, 0 if not in the list...
 */
int CS_Settings_GetIndexOfSavedParameterFile (const char * archiveName);


/**
 * Get the name of the setting stored in the .SAV file.
 * @param archiveName Archive name.
 * @param outputName Output buffer filled with the param file name.
 * @param outputNameMaxLength Size of \a outputName.
 * @return
 * - \a TRUE if list successfully saved.
 * - \a FALSE if an error occurred.
 */
int CS_Settings_GetCurrentName (const char * archiveName, char * outputName, const unsigned int outputNameMaxLength);


#endif // __CLESS_SAMPLE_SETTINGS_H__INCLUDED__
