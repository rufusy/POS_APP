//! \file
//! Module that manages the terminal settings archive.
//
/////////////////////////////////////////////////////////////////
//// Includes ///////////////////////////////////////////////////

#include "Cless_Implementation.h"
#include "GL_File.h"
#include "GL_Dir.h"

/////////////////////////////////////////////////////////////////
//// Macros & preprocessor definitions //////////////////////////

static const char m_pFileType[] = "file://";
static const char m_pDirType[] = "dir://";
static const char m_pHostPath[] = "flash/HOST/";
static const char m_pRamPath[] = "ram/MYTMP/";
static const char m_pRamFullPathForCreation[] = "file://ram/MYTMP/MYFILE.TXT?access=full&maxfiles=2&size=65536";
static const char m_pRamFullPathCheckDisk[] = "file://ram/MYTMP/MYFILE.TXT";
//static const char m_pTempGzFileFullName[] = "file://flash/HOST/TEMP.GZ";
static const char m_pTempGzFileFullName[] = "file://ram/MYTMP/TEMP.GZ";
///static const char m_pParGzExtension[] = ".PAR.gz";
static const char m_pGzExtension[] = ".gz";
static const char m_pTarExtension[] = ".TAR";
///static const char m_pParExtension[] = ".PAR";
static const char m_pSavExtension[] = ".SAV";
static const char m_pTempExtension[] = ".TMP";


/////////////////////////////////////////////////////////////////
//// Types //////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////
//// Global variables ///////////////////////////////////////////


/////////////////////////////////////////////////////////////////
//// Static function definitions ////////////////////////////////


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
int CS_Settings_getListOfFiles (const char * archiveName, T_NAMES_LIST * list)
{
	char archiveFileFullName[256];
	int fileCounter;
	int bContinue;
	T_GL_HDIR archiveDirectory = NULL;

	// Check the parameters are correct
	if (list == NULL)
	{
        GTL_Traces_TraceDebug ("CS_Settings_getListOfFiles: Invalid list");
        return (FALSE);
	}

	// Check that the archive file name is not empty
	if (archiveName == NULL)
	{
		GTL_Traces_TraceDebug ("CS_Settings_getListOfFiles: The archive name of the file that we are looking for is empty.");
		return (FALSE);
	}

	// Initialize the strings
	memset (archiveFileFullName, '\0', sizeof(archiveFileFullName));
	Telium_Sprintf (archiveFileFullName, "%s%s%s%s", m_pFileType, m_pHostPath, archiveName, m_pTarExtension);

    // Check that the file exists
	if (GL_File_Exists(archiveFileFullName) != GL_SUCCESS)
    {
    	GTL_Traces_TraceDebug("CS_Settings_getListOfFiles: The file <%s> that we are looking for doesn't exist.", archiveFileFullName);
        return (FALSE);
    }

    // Archive exists, open it
    archiveDirectory = GL_Dir_Open(archiveFileFullName);
    if (archiveDirectory == NULL)
    {
    	GTL_Traces_TraceDebug("CS_Settings_getListOfFiles: The directory <%s> that we are looking failed to be opened.", archiveFileFullName);
        return (FALSE);
    }

    // Count the number of files in the archive
    fileCounter = 0;
   	while (GL_Dir_Next(archiveDirectory) == GL_SUCCESS)
   		fileCounter ++;

    GL_Dir_Close(archiveDirectory);

    if (fileCounter == 0)
    {
    	GTL_Traces_TraceDebug("CS_Settings_getListOfFiles: The directory <%s> contains no parameter file.", archiveFileFullName);
        return (FALSE);
    }

    // Initialize the list
    CS_NameList_Init (list, fileCounter+1);

    // Archive exists, open it again to get now the list of file names
    archiveDirectory = GL_Dir_Open(archiveFileFullName);
    if (archiveDirectory == NULL)
    {
    	GTL_Traces_TraceDebug("CS_Settings_getListOfFiles: The directory <%s> that we are looking failed to be opened.", archiveFileFullName);
        return (FALSE);
    }

    // Count the number of files in the archive
    bContinue = TRUE;
    do
    {
    	bContinue = (GL_Dir_Next(archiveDirectory) == GL_SUCCESS);
    	if (bContinue)
    		bContinue = CS_NameList_AddItem (list, GL_Dir_GetName(archiveDirectory));
    } while (bContinue == TRUE);

    GL_Dir_Close(archiveDirectory);

    // Theorically, the list should be full
    if (list->maxNumberOfEntries != (list->numberOfEntries + 1))
    {
    	CS_NameList_Init(list, 0);
    	GTL_Traces_TraceDebug("CS_Settings_getListOfFiles: The list should be full at this step... (max %d, real + 1  %d)", list->maxNumberOfEntries, list->numberOfEntries + 1);
        return (FALSE);
    }

    return (TRUE);
}



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
int CS_Settings_loadFromFile (const char * archiveName, const char * fileName, TLV_TREE_NODE * loadedSettingTree, T_SETTINGS_DECODER decoderType, const char * fileExtension)
{
	char tarFullName[256];
	char archiveFullName[256];
	char tarFullNameGz[256];
	char fileNameNoGz[128];
	char fullFileExtension[16];
	char * pBuffer = NULL;
	T_GL_HFILE pFileToLoad = NULL;
	T_GL_HFILE pTempFileGz = NULL;
	T_GL_HFILE pTempRamDiskFile = NULL;
	int result = FALSE;
	long fileLength = 0;
	long readLength = 0;
	long wroteLength = 0;
	int cr;
	int fileExtensionLength;


	// Initialize buffers
	memset (tarFullName, '\0', sizeof(tarFullName));
	memset (archiveFullName, '\0', sizeof(archiveFullName));
	memset (tarFullNameGz, '\0', sizeof(tarFullNameGz));
	memset (fullFileExtension, '\0', sizeof(fullFileExtension));

	// Check that the output TLV Tree structure is provided
	if (loadedSettingTree == NULL)
	{
		GTL_Traces_TraceDebug ("CS_TermSettings_loadFromFile: The output TLV Tree is not provided.");
		return (FALSE);
	}

	// Initialize the output TLV Tree
	if (*loadedSettingTree != NULL)
	{
		TlvTree_Release(*loadedSettingTree);
		*loadedSettingTree = NULL;
	}

	// Check that the file name is not empty
	if (fileName == NULL)
	{
		GTL_Traces_TraceDebug ("CS_TermSettings_loadFromFile: The name of the file that we are looking for is empty.");
		return (FALSE);
	}

	// Check that the archive file name is not empty
	if (archiveName == NULL)
	{
		GTL_Traces_TraceDebug ("CS_TermSettings_loadFromFile: The archive name of the file that we are looking for is empty.");
		return (FALSE);
	}

	// build the full archive name
	Telium_Sprintf (archiveFullName, "%s%s%s%s", m_pFileType, m_pHostPath, archiveName, m_pTarExtension);

    // test if the archive exists
    if (GL_File_Exists(archiveFullName) != GL_SUCCESS)
    {
        GTL_Traces_TraceDebug ("CS_TermSettings_loadFromFile: The archive file <%s> doesn't exist.", archiveFullName);
        return(FALSE);
    }

    // Create the full name of the PAR or GZ file that we are looking for
    // example: "file://flash/HOST/EP_SETTINGS.TAR/TermSetting1.par" or "file://flash/HOST/EP_SETTINGS.TAR/TermSetting1.par.gz"
    Telium_Sprintf (tarFullName, "%s%s%s%s/%s", m_pFileType, m_pHostPath, archiveName, m_pTarExtension, fileName);

    // Check that the file exists
    if (GL_File_Exists(tarFullName) != GL_SUCCESS)
    {
        GTL_Traces_TraceDebug ("CS_TermSettings_loadFromFile: The file <%s> that we are looking for doesn't exist.", tarFullName);
        return(FALSE);
    }

    // Open the file
    pFileToLoad = GL_File_Open(tarFullName, GL_FILE_OPEN_EXISTING, GL_FILE_ACCESS_READ);
    if (pFileToLoad == NULL)
    {
    	GTL_Traces_TraceDebug ("CS_TermSettings_loadFromFile: The file <%s> can't be opened.", tarFullName);
        goto end;
    }

    // Get the file length
    fileLength = GL_File_GetSize(pFileToLoad);
    if (fileLength==0)
    {
    	GTL_Traces_TraceDebug ("CS_TermSettings_loadFromFile: The file <%s> is empty.", tarFullName);
        goto end;
    }

    // Allocate the buffer to read the file
    pBuffer = (char*)umalloc(fileLength);
    if (pBuffer == NULL)
    {
    	GTL_Traces_TraceDebug ("CS_TermSettings_loadFromFile: Unable to allocate <%ld> bytes.", fileLength);
        goto end;
    }

    // Read the file
    readLength = GL_File_Read(pFileToLoad, pBuffer, fileLength);
    if (readLength != fileLength)
    {
    	GTL_Traces_TraceDebug ("CS_TermSettings_loadFromFile: The file <%s> can't be read correctly. Only %ld read instead of %ld", tarFullName, readLength, fileLength);
        goto end;
    }

    // Check if the file is a GZIP file
    fileExtensionLength = strlen (fileExtension);
    Telium_Sprintf (fullFileExtension, "%s%s", fileExtension, m_pGzExtension);

    if (((fileExtensionLength > 0) && (strcmp (&tarFullName[strlen(tarFullName) - fileExtensionLength - strlen (m_pGzExtension)], fullFileExtension) == 0)) ||
    	(fileExtensionLength == 0))
    {
    	// Create the RAM disk if not already created
    	if (GL_File_Exists(m_pRamFullPathCheckDisk) != GL_SUCCESS)
    	{
    		pTempRamDiskFile = GL_File_Open (m_pRamFullPathForCreation, GL_FILE_OPEN_ALWAYS, GL_FILE_ACCESS_READ_WRITE);
    		if (pTempRamDiskFile == NULL)
    		{
            	GTL_Traces_TraceDebug ("CS_TermSettings_loadFromFile: The RAM disk <%s> cannot be created.", m_pRamFullPathCheckDisk);
                goto end;
    		}
    		else
    		{
    			GL_File_Close(pTempRamDiskFile);
    			pTempRamDiskFile = NULL;
    		}
    	}

    	if (GL_File_Exists(m_pTempGzFileFullName) == GL_SUCCESS)
    		GL_File_Delete(m_pTempGzFileFullName);

        // File is GZIPPED, so complete the file path to include the GZIP archive
        // example: "file://flash/HOST/EP_SETTINGS.TAR/TermSetting1.par.gz"
        pTempFileGz = GL_File_Open (m_pTempGzFileFullName, GL_FILE_OPEN_ALWAYS, GL_FILE_ACCESS_READ_WRITE);
        if (pTempFileGz == NULL)
        {
        	GTL_Traces_TraceDebug ("CS_TermSettings_loadFromFile: The file <%s> can't be opened.", m_pTempGzFileFullName);
            goto end;
        }

        // Write the file content
        wroteLength = GL_File_Write (pTempFileGz, pBuffer, readLength);
        if (wroteLength != readLength)
        {
        	GTL_Traces_TraceDebug ("CS_TermSettings_loadFromFile: The file <%s> can't be correctly written. Only %ld written instead of %ld", m_pTempGzFileFullName, wroteLength, readLength);
            goto end;
        }

        // free the buffer
        ufree(pBuffer);
        pBuffer = NULL;

        // force writing and close file
        //GL_File_Flush(pTempFileGz);
        GL_File_Close (pTempFileGz);
        pTempFileGz = NULL;

        // Build the name of the PAR located in the temporary GZIP file
        // example: "file://flash/HOST/TEMP.GZ/TermSetting1.par.gz" becomes "file://flash/HOST/TEMP.GZ/TermSetting1.par"
        // Remove extension
        Telium_Sprintf (fileNameNoGz, "%s", fileName);
        fileNameNoGz[strlen(fileNameNoGz)-3] = '\0';

        Telium_Sprintf (tarFullNameGz, "%s/%s", m_pTempGzFileFullName, fileNameNoGz);

        // Open the file located in the renamed GZ file supposed to have the same name
        pTempFileGz = GL_File_Open(tarFullNameGz, GL_FILE_OPEN_EXISTING, GL_FILE_ACCESS_READ);
        if (pTempFileGz == NULL)
        {
        	GTL_Traces_TraceDebug ("CS_TermSettings_loadFromFile: The file <%s> can't be opened.", tarFullNameGz);
            goto end;
        }

        // Get the PAR file size
        fileLength = GL_File_GetSize(pTempFileGz);
        if (fileLength==0)
        {
        	GTL_Traces_TraceDebug ("CS_TermSettings_loadFromFile: The file <%s> is empty.", tarFullNameGz);
            goto end;
        }

        // Allocate the buffer to read the file
        pBuffer = (char*)umalloc(fileLength);
        if (pBuffer == NULL)
        {
        	GTL_Traces_TraceDebug ("CS_TermSettings_loadFromFile: Unable to allocate <%ld> bytes.", fileLength);
            goto end;
        }

        // Read the file
        readLength = GL_File_Read (pTempFileGz, pBuffer, fileLength);
        if (readLength != fileLength)
        {
        	GTL_Traces_TraceDebug ("CS_TermSettings_loadFromFile: The file <%s> can't be read correctly. Only %ld read instead of %ld", tarFullNameGz, readLength, fileLength);
            goto end;
        }

        GL_File_Close (pTempFileGz);
        pTempFileGz = NULL;
    }

    switch (decoderType)
    {
    	case (SettingsDecoderTeliumXml):
			// At this step, all the PAR file is located in "pBuffer", so serialize it in a TLV Tree
			cr = TlvTree_Unserialize(loadedSettingTree, TLV_TREE_SERIALIZER_XML, (const unsigned char*)pBuffer, fileLength);
			if (cr != TLV_TREE_OK)
			{
				GTL_Traces_TraceDebug ("CS_Settings_loadFromFile: Error (%d) when unserializing the terminal settings.", cr);
				// release tree node
				if (*loadedSettingTree != NULL)
				{
					TlvTree_Release (*loadedSettingTree);
					*loadedSettingTree = NULL;
				}
				goto end;
			}
			break;

    	case (SettingsDecoderDekDet):
			Cless_DataExchange_DecodeXMLBuffer(loadedSettingTree, pBuffer, fileLength);
			break;

    	default:
        	GTL_Traces_TraceDebug ("CS_TermSettings_loadFromFile: The provided decoder is unknown <%d>", decoderType);
            goto end;
    		break;
    }


    result = TRUE;


    // Check CA Keys CRC, create supported list of CA per AIDs and build AID nodes
	Cless_Parameters_CheckCaKeysCrc(pTreeCurrentParam);
	Cless_Parameters_CreateSupportedCertificateListForKernel (pTreeCurrentParam, DEFAULT_EP_KERNEL_PAYPASS);
	Cless_Parameters_BuildAIDNodes(pTreeCurrentParam);

end:

	if (pTempFileGz)
	{
		GL_File_Close(pTempFileGz);
		pTempFileGz = NULL;
	}

	if (pFileToLoad)
	{
		GL_File_Close(pFileToLoad);
		pFileToLoad = NULL;
	}

    // Delete the temporary files, if present
	if (GL_File_Exists(m_pTempGzFileFullName) == GL_SUCCESS)
		GL_File_Delete(m_pTempGzFileFullName);

    // Deallocate the buffers
	if (pBuffer != NULL)
	{
		ufree (pBuffer);
		pBuffer = NULL;
	}

    return (result);
}



/**
 * Check if a term setting file is present or not in the archive.
 * @param archiveName Archive name.
 * @param fileName fileName Name of the file to be selected (with the extension: ".PAR" or ".PAR.gz").
 * @return
 * - true if the selected file is valid and successfully loaded.
 * - false otherwise.
 * @note The archive shall be loaded in the terminal HOST directory.
 */
int CS_Settings_IsFileExisting (const char * archiveName, const char * fileName)
{
	char tarFullName[256];
	int result = FALSE;


	// Initialize buffers
	memset (tarFullName, '\0', sizeof(tarFullName));

	// Check that the file name is not empty
	if (fileName == NULL)
	{
		GTL_Traces_TraceDebug ("CS_Settings_IsFileExisting: The name of the file that we are looking for is empty.");
		goto end;
	}

	// Check that the archive file name is not empty
	if (archiveName == NULL)
	{
		GTL_Traces_TraceDebug ("CS_Settings_IsFileExisting: The archive name of the file that we are looking for is empty.");
		goto end;
	}

    // Create the full name of the PAR or GZ file that we are looking for
    // example: "file://flash/HOST/EP_SETTINGS.TAR/TermSetting1.par" or "file://flash/HOST/EP_SETTINGS.TAR/TermSetting1.par.gz"
    Telium_Sprintf (tarFullName, "%s%s%s%s/%s", m_pFileType, m_pHostPath, archiveName, m_pTarExtension, fileName);

    // Check that the file exists
    if (GL_File_Exists(tarFullName) != GL_SUCCESS)
    {
        GTL_Traces_TraceDebug ("CS_Settings_IsFileExisting: The file <%s> that we are looking for doesn't exist.", tarFullName);
        goto end;
    }

    result = TRUE;

end:

    return (result);
}



/**
 * Save the name of the file name to used for a given archiveName into a dedicated file named "archiveName.SAV".
 * @param archiveName Archive name.
 * @param fileName File name to be saved.
 * @return
 * - \a TRUE if list successfully saved.
 * - \a FALSE if an error occurred.
 */
int CS_Settings_SaveSelectedFileName (const char * archiveName, const char * fileName)
{
	int bErrorOccured = FALSE;
	char savedArchiveFileName[256];
	char savedArchiveFileNameTemp[256];
	char * pBuffer = NULL;
	int fileLength;
	int readLength;
	T_GL_HFILE mySavedFile = NULL;
	int bUpdateSaveFile = FALSE;

	// Check that the file name is not empty
	if (fileName == NULL)
	{
		GTL_Traces_TraceDebug ("CS_Settings_SaveSelectedFileName: The name of the file that we are looking for is NULL.");
		return (FALSE);
	}

	// Check that the file name is not empty
	if (strlen (fileName) == 0)
	{
		GTL_Traces_TraceDebug ("CS_Settings_SaveSelectedFileName: The name of the file that we are looking for is empty.");
		return (FALSE);
	}

	// Check that the archive file name is not empty
	if (archiveName == NULL)
	{
		GTL_Traces_TraceDebug ("CS_Settings_SaveSelectedFileName: The archive name of the file that we are looking for is empty.");
		return (FALSE);
	}

	// Initialize the archive name saved file
	memset (savedArchiveFileName, '\0', sizeof(savedArchiveFileName));
	memset (savedArchiveFileNameTemp, '\0', sizeof(savedArchiveFileNameTemp));
	Telium_Sprintf (savedArchiveFileName, "%s%s%s%s", m_pFileType, m_pHostPath, archiveName, m_pSavExtension);
	Telium_Sprintf (savedArchiveFileNameTemp, "%s%s%s%s", m_pFileType, m_pHostPath, archiveName, m_pTempExtension);

	// If the temporary file already exists, erase it
	if (GL_File_Exists(savedArchiveFileNameTemp) == GL_SUCCESS)
	{
		// File already exist, erase it
		GL_File_Delete(savedArchiveFileNameTemp);
	}

	// Check if the save file already exist, and remove it if it is the case
	if (GL_File_Exists(savedArchiveFileName) == GL_SUCCESS)
	{
		// Check if the file name we want to save is already in the file
		mySavedFile = GL_File_Open(savedArchiveFileName, GL_FILE_OPEN_EXISTING, GL_FILE_ACCESS_READ);
		if (mySavedFile == NULL)
		{
        	GTL_Traces_TraceDebug ("CS_Settings_SaveSelectedFileName: The file <%s> can't be opened.", savedArchiveFileName);
        	bErrorOccured = TRUE;
            goto End;
		}

		fileLength = GL_File_GetSize(mySavedFile);
	    if (fileLength==0)
	    {
	    	GTL_Traces_TraceDebug ("CS_Settings_SaveSelectedFileName: The file <%s> is empty.", savedArchiveFileName);
	    	bErrorOccured = TRUE;
		    GL_File_Close(mySavedFile);
		    mySavedFile = NULL;
	        goto End;
	    }

	    // Allocate the buffer to read the file
	    pBuffer = (char*)umalloc(fileLength);
	    if (pBuffer == NULL)
	    {
	    	GTL_Traces_TraceDebug ("CS_Settings_SaveSelectedFileName: Unable to allocate <%d> bytes.", fileLength);
	    	bErrorOccured = TRUE;
		    GL_File_Close(mySavedFile);
		    mySavedFile = NULL;
	        goto End;
	    }

	    // Read the file
	    readLength = GL_File_Read(mySavedFile, pBuffer, fileLength);
	    if (readLength != fileLength)
	    {
	    	GTL_Traces_TraceDebug ("CS_Settings_SaveSelectedFileName: The file <%s> can't be read correctly. Only %d read instead of %d", savedArchiveFileName, readLength, fileLength);
	    	bErrorOccured = TRUE;
		    GL_File_Close(mySavedFile);
		    mySavedFile = NULL;
	        goto End;
	    }

	    if (readLength == strlen(fileName))
	    {
	    	if (memcmp (pBuffer, fileName, readLength) != 0)
	    	{
	    		bUpdateSaveFile = TRUE;
	    	}
	    }
	    else
	    {
	    	bUpdateSaveFile = TRUE;
	    }

	    GL_File_Close(mySavedFile);
	    mySavedFile = NULL;
	}
	else
	{
		bUpdateSaveFile = TRUE;
	}

	if (bUpdateSaveFile)
	{
		// Create the new file
		mySavedFile = GL_File_Open(savedArchiveFileNameTemp, GL_FILE_OPEN_ALWAYS, GL_FILE_ACCESS_READ_WRITE);
		if (mySavedFile == NULL)
		{
			GTL_Traces_TraceDebug ("CS_Settings_SaveSelectedFileName: The file <%s> cannot be created.", savedArchiveFileNameTemp);
			bErrorOccured = TRUE;
			goto End;
		}

		// Write the fileName in the save file
		if (GL_File_Write (mySavedFile, fileName, strlen(fileName)) != (strlen(fileName)))
		{
			GTL_Traces_TraceDebug ("CS_Settings_SaveSelectedFileName: Unable to write the file name in the save file <%s>.", savedArchiveFileNameTemp);
			GL_File_Close(mySavedFile);
			mySavedFile = NULL;
			bErrorOccured = TRUE;
			goto End;
		}

		GL_File_Flush(mySavedFile);
		GL_File_Close(mySavedFile);
		mySavedFile = NULL;

		// If a previous file exists, erase it
		if (GL_File_Exists(savedArchiveFileName) == GL_SUCCESS)
		{
			GL_File_Delete(savedArchiveFileName);
		}

		if (GL_File_Copy (savedArchiveFileNameTemp, savedArchiveFileName) != GL_SUCCESS)
		{
			GTL_Traces_TraceDebug ("CS_Settings_SaveSelectedFileName: Cannot rename the temporary file <%s>...", savedArchiveFileNameTemp);
			bErrorOccured = TRUE;
			goto End;
		}
	}

End:
	// If the file already exists, erase it
	if (GL_File_Exists(savedArchiveFileNameTemp) == GL_SUCCESS)
	{
		// File already exist, erase it
		GL_File_Delete(savedArchiveFileNameTemp);
	}

	if (pBuffer != NULL) {
		ufree (pBuffer);
		pBuffer = NULL;
	}

	return (!bErrorOccured);
}



/**
 * Erase the file containing the saved file name.
 * @param archiveName Archive name.
 * @param fileName File name to be saved.
 */
void CS_Settings_DeleteSavedSelectedFileName (const char * archiveName)
{
	char savedArchiveFileName[256];

	// Initialize the archive name saved file
	memset (savedArchiveFileName, '\0', sizeof(savedArchiveFileName));
	Telium_Sprintf (savedArchiveFileName, "%s%s%s%s", m_pFileType, m_pHostPath, archiveName, m_pSavExtension);

	// If the  file already exists, erase it
	if (GL_File_Exists(savedArchiveFileName) == GL_SUCCESS)
	{
		// File already exist, erase it
		GL_File_Delete(savedArchiveFileName);
	}
}



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
int CS_Settings_Restore (const char * archiveName, TLV_TREE_NODE * loadedSettingTree, T_SETTINGS_DECODER decoderType, const char * fileExtension)
{
	int bErrorOccured = FALSE;
	char savedArchiveFileName[256];
	char * pBuffer = NULL;
	int fileLength;
	int readLength;
	T_GL_HFILE mySavedFile = NULL;

	// Check that the archive file name is not empty
	if (archiveName == NULL)
	{
		GTL_Traces_TraceDebug ("CS_Settings_Restore: The archive name of the file that we are looking for is empty.");
		return (FALSE);
	}

	// Check that the output TLV Tree structure is provided
	if (loadedSettingTree == NULL)
	{
		GTL_Traces_TraceDebug ("CS_Settings_Restore: The output TLV Tree is not provided.");
		return (FALSE);
	}

	// Initialize the archive name saved file
	memset (savedArchiveFileName, '\0', sizeof(savedArchiveFileName));
	Telium_Sprintf (savedArchiveFileName, "%s%s%s%s", m_pFileType, m_pHostPath, archiveName, m_pSavExtension);

	// Check if the save file already exist, and remove it if it is the case
	if (GL_File_Exists(savedArchiveFileName) == GL_SUCCESS)
	{
		// Check if the file name we want to save is already in the file
		mySavedFile = GL_File_Open(savedArchiveFileName, GL_FILE_OPEN_EXISTING, GL_FILE_ACCESS_READ);
		if (mySavedFile == NULL)
		{
        	GTL_Traces_TraceDebug ("CS_Settings_Restore: The file <%s> can't be opened.", savedArchiveFileName);
        	bErrorOccured = TRUE;
            goto End;
		}

		fileLength = GL_File_GetSize(mySavedFile);
	    if (fileLength==0)
	    {
	    	GTL_Traces_TraceDebug ("CS_Settings_Restore: The file <%s> is empty.", savedArchiveFileName);
	    	bErrorOccured = TRUE;
		    GL_File_Close(mySavedFile);
		    mySavedFile = NULL;
	        goto End;
	    }

	    // Allocate the buffer to read the file
	    pBuffer = (char*)umalloc(fileLength + 1);
	    if (pBuffer == NULL)
	    {
	    	GTL_Traces_TraceDebug ("CS_Settings_Restore: Unable to allocate <%d> bytes.", fileLength);
	    	bErrorOccured = TRUE;
		    GL_File_Close(mySavedFile);
		    mySavedFile = NULL;
	        goto End;
	    }

	    // Read the file
	    readLength = GL_File_Read(mySavedFile, pBuffer, fileLength);
	    if (readLength != fileLength)
	    {
	    	GTL_Traces_TraceDebug ("CS_Settings_Restore: The file <%s> can't be read correctly. Only %d read instead of %d", savedArchiveFileName, readLength, fileLength);
	    	bErrorOccured = TRUE;
		    GL_File_Close(mySavedFile);
		    mySavedFile = NULL;
	        goto End;
	    }

	    GL_File_Close(mySavedFile);
	    mySavedFile = NULL;

	    // Add the '\0' at the end of the buffer
	    pBuffer[fileLength] = '\0';

	    if (!CS_Settings_loadFromFile(archiveName, (const char *)pBuffer, loadedSettingTree, decoderType, fileExtension))
	    {
	    	GTL_Traces_TraceDebug ("CS_Settings_Restore: An error occurred when loading file <%s> from <%s>", pBuffer, savedArchiveFileName);
	    	bErrorOccured = TRUE;

	    	// The name extracted from the archive is not valid anymore, delete the saved file and used the default parameters for example...
	    	CS_Settings_DeleteSavedSelectedFileName(archiveName);

	        goto End;
	    }
	}
	else
	{
		bErrorOccured = TRUE;
	}

End:
	if (mySavedFile)
	{
		GL_File_Close(mySavedFile);
		mySavedFile = NULL;
	}

	if (pBuffer != NULL) {
		ufree (pBuffer);
		pBuffer = NULL;
	}

	return (!bErrorOccured);
}


/**
 * Returns the index of the saved parameter file in the list.
 * @param archiveName Archive name.
 * @return The index of the item, 0 if not in the list...
 */
int CS_Settings_GetIndexOfSavedParameterFile (const char * archiveName)
{
	int bErrorOccured = FALSE;
	char savedArchiveFileName[256];
	char * pBuffer = NULL;
	int fileLength;
	int readLength;
	T_GL_HFILE mySavedFile = NULL;
	T_NAMES_LIST myList;
	int returnedIndex = 0;

	// Check that the archive file name is not empty
	if (archiveName == NULL)
	{
		GTL_Traces_TraceDebug ("CS_Settings_Restore: The archive name of the file that we are looking for is empty.");
		return (FALSE);
	}

	memset (&myList, 0, sizeof(T_NAMES_LIST));
	CS_NameList_Init(&myList, 0);

	// Initialize the archive name saved file
	memset (savedArchiveFileName, '\0', sizeof(savedArchiveFileName));
	Telium_Sprintf (savedArchiveFileName, "%s%s%s%s", m_pFileType, m_pHostPath, archiveName, m_pSavExtension);

	// Check if the save file already exist, and remove it if it is the case
	if (GL_File_Exists(savedArchiveFileName) == GL_SUCCESS)
	{
		// Check if the file name we want to save is already in the file
		mySavedFile = GL_File_Open(savedArchiveFileName, GL_FILE_OPEN_EXISTING, GL_FILE_ACCESS_READ);
		if (mySavedFile == NULL)
		{
        	GTL_Traces_TraceDebug ("CS_Settings_Restore: The file <%s> can't be opened.", savedArchiveFileName);
        	bErrorOccured = TRUE;
            goto End;
		}

		fileLength = GL_File_GetSize(mySavedFile);
	    if (fileLength==0)
	    {
	    	GTL_Traces_TraceDebug ("CS_Settings_Restore: The file <%s> is empty.", savedArchiveFileName);
	    	bErrorOccured = TRUE;
		    GL_File_Close(mySavedFile);
		    mySavedFile = NULL;
	        goto End;
	    }

	    // Allocate the buffer to read the file
	    pBuffer = (char*)umalloc(fileLength + 1);
	    if (pBuffer == NULL)
	    {
	    	GTL_Traces_TraceDebug ("CS_Settings_Restore: Unable to allocate <%d> bytes.", fileLength);
	    	bErrorOccured = TRUE;
		    GL_File_Close(mySavedFile);
		    mySavedFile = NULL;
	        goto End;
	    }

	    // Read the file
	    readLength = GL_File_Read(mySavedFile, pBuffer, fileLength);
	    if (readLength != fileLength)
	    {
	    	GTL_Traces_TraceDebug ("CS_Settings_Restore: The file <%s> can't be read correctly. Only %d read instead of %d", savedArchiveFileName, readLength, fileLength);
	    	bErrorOccured = TRUE;
		    GL_File_Close(mySavedFile);
		    mySavedFile = NULL;
	        goto End;
	    }

	    GL_File_Close(mySavedFile);
	    mySavedFile = NULL;

	    // Add the '\0' at the end of the buffer
	    pBuffer[fileLength] = '\0';

	    if (CS_Settings_getListOfFiles(archiveName, &myList))
	    {
	    	int i = 0;

	    	while (i<myList.numberOfEntries)
	    	{
	    		if (strcmp(myList.pListOfNames[i], pBuffer) == 0)
	    		{
	    			returnedIndex = i;
	    			i = myList.numberOfEntries;
	    		}
	    		else
	    		{
	    			i++;
	    		}
	    	}
	    }
	}

End:
	if (mySavedFile)
	{
		GL_File_Close(mySavedFile);
		mySavedFile = NULL;
	}

	if (pBuffer != NULL) {
		ufree (pBuffer);
		pBuffer = NULL;
	}

	CS_NameList_Init(&myList, 0);

	return (bErrorOccured ? 0 : returnedIndex);
}


/**
 * Get the name of the setting stored in the .SAV file.
 * @param archiveName Archive name.
 * @param outputName Output buffer filled with the param file name.
 * @param outputNameMaxLength Size of \a outputName.
 * @return
 * - \a TRUE if list successfully saved.
 * - \a FALSE if an error occurred.
 */
int CS_Settings_GetCurrentName (const char * archiveName, char * outputName, const unsigned int outputNameMaxLength) {
	int bErrorOccured = FALSE;
	char savedArchiveFileName[256];
	char * pBuffer = NULL;
	int fileLength;
	int readLength;
	T_GL_HFILE mySavedFile = NULL;

	// Check that the archive file name is not empty
	if (archiveName == NULL) {
		GTL_Traces_TraceDebug ("CS_Settings_GetCurrentName: The archive name of the file that we are looking for is empty.");
		return (FALSE);
	}

	if ((outputNameMaxLength == 0) || (outputName == NULL)) {
		GTL_Traces_TraceDebug ("CS_Settings_GetCurrentName: OUtput parameters are not valid.");
		return (FALSE);
	}

	// Initialize output buffer
	memset (outputName, '\0', outputNameMaxLength);

	// Initialize the archive name saved file
	memset (savedArchiveFileName, '\0', sizeof(savedArchiveFileName));
	Telium_Sprintf (savedArchiveFileName, "%s%s%s%s", m_pFileType, m_pHostPath, archiveName, m_pSavExtension);

	// Check if the save file already exist, and remove it if it is the case
	if (GL_File_Exists(savedArchiveFileName) == GL_SUCCESS) {
		// Check if the file name we want to save is already in the file
		mySavedFile = GL_File_Open(savedArchiveFileName, GL_FILE_OPEN_EXISTING, GL_FILE_ACCESS_READ);
		if (mySavedFile == NULL) {
        	GTL_Traces_TraceDebug ("CS_Settings_GetCurrentName: The file <%s> can't be opened.", savedArchiveFileName);
        	bErrorOccured = TRUE;
            goto End;
		}

		fileLength = GL_File_GetSize(mySavedFile);
	    if (fileLength==0) {
	    	GTL_Traces_TraceDebug ("CS_Settings_GetCurrentName: The file <%s> is empty.", savedArchiveFileName);
	    	bErrorOccured = TRUE;
		    GL_File_Close(mySavedFile);
		    mySavedFile = NULL;
	        goto End;
	    }

	    // Allocate the buffer to read the file
	    pBuffer = (char*)umalloc(fileLength + 1);
	    if (pBuffer == NULL) {
	    	GTL_Traces_TraceDebug ("CS_Settings_GetCurrentName: Unable to allocate <%d> bytes.", fileLength);
	    	bErrorOccured = TRUE;
		    GL_File_Close(mySavedFile);
		    mySavedFile = NULL;
	        goto End;
	    }

	    // Read the file
	    readLength = GL_File_Read(mySavedFile, pBuffer, fileLength);
	    if (readLength != fileLength) {
	    	GTL_Traces_TraceDebug ("CS_Settings_GetCurrentName: The file <%s> can't be read correctly. Only %d read instead of %d", savedArchiveFileName, readLength, fileLength);
	    	bErrorOccured = TRUE;
		    GL_File_Close(mySavedFile);
		    mySavedFile = NULL;
	        goto End;
	    }

	    GL_File_Close(mySavedFile);
	    mySavedFile = NULL;

	    // Add the '\0' at the end of the buffer
	    pBuffer[fileLength] = '\0';

	    // Copy the name into the output buffer
	    Telium_Sprintf (outputName, "%s", pBuffer);
	} else {
		bErrorOccured = TRUE;
	}

End:
	if (mySavedFile) {
		GL_File_Close(mySavedFile);
		mySavedFile = NULL;
	}

	if (pBuffer != NULL) {
		ufree (pBuffer);
		pBuffer = NULL;
	}

	return (!bErrorOccured);
}

