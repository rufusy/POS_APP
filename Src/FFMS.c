//****************************************************************************
//       INGENICO                                INGEDEV 7                  
//============================================================================
//       FILE  FFMS.C                             (Copyright INGENICO 2012)
//============================================================================
//  Created :       13-July-2008         Kassovic
//  Last modified : 29-November-2013     Kassovic
//  Module : TRAINING                                                          
//                                                                          
//  Purpose :                                                               
//                  *** Flash File Management System (FFMS) ***
//      The FFMS library provides a low level of services allowing the applications
//      to manage and save files in the flash. It is recommended to use:
//      - FFMS API(s) to manage disk.
//      - GOAL API(s) to manage File and Directory (Url: protocol://drive/path).
//        [instead of FFMS library].
//      To show how to create a disk and open a file for writing and reading
//      inside the flash.
//                                                                            
//  List of routines in file :  
//      DiskCreate : Create, initialize and format a disk.
//      DiskKill : Suppress a disk.
//      DiskUse : Space used by the disk.
//      FileRead : Read N bytes from a file.
//      FileWrite : Write N bytes into a file.                                        
//                            
//  File history :
//  071312-BK : File created
//  112913-BK : FFMS using GOAL
//                                                                           
//****************************************************************************

//****************************************************************************
//      INCLUDES                                                            
//****************************************************************************
#include <globals.h>

//****************************************************************************
//      EXTERN                                                              
//****************************************************************************
extern T_GL_HGRAPHIC_LIB hGoal; // Handle of the graphics object library

//****************************************************************************
//      PRIVATE CONSTANTS                                                   
//****************************************************************************
    /* */

//****************************************************************************
//      PRIVATE TYPES                                                       
//****************************************************************************
    /* */

//****************************************************************************
//      PRIVATE DATA                                                        
//****************************************************************************
// Properties of the FFMS screen (Goal)
// =====================================
static const ST_DSP_LINE txFFMS[] =
{
	{ {GL_ALIGN_LEFT, GL_ALIGN_CENTER, GL_COLOR_WHITE, GL_COLOR_BLACK, 100, FALSE, {1, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_WHITE}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}}, // Line0
	  {GL_ALIGN_LEFT, GL_ALIGN_CENTER, FALSE, 100, FALSE, {2, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}} },
	{ {GL_ALIGN_LEFT, GL_ALIGN_CENTER, GL_COLOR_WHITE, GL_COLOR_BLACK, 100, FALSE, {1, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_WHITE}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}}, // Line1
	  {GL_ALIGN_LEFT, GL_ALIGN_CENTER, FALSE, 100, FALSE, {2, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}} },
	{ {GL_ALIGN_LEFT, GL_ALIGN_CENTER, GL_COLOR_WHITE, GL_COLOR_BLACK, 100, FALSE, {1, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_WHITE}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}}, // Line2
	  {GL_ALIGN_LEFT, GL_ALIGN_CENTER, FALSE, 100, FALSE, {2, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}} },
	{ {GL_ALIGN_LEFT, GL_ALIGN_CENTER, GL_COLOR_WHITE, GL_COLOR_BLACK, 100, FALSE, {1, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_WHITE}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}}, // Line3
	  {GL_ALIGN_LEFT, GL_ALIGN_CENTER, FALSE, 100, FALSE, {2, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}} },
	{ {GL_ALIGN_LEFT, GL_ALIGN_CENTER, GL_COLOR_WHITE, GL_COLOR_BLACK, 100, FALSE, {1, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_WHITE}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}}, // Line4
	  {GL_ALIGN_LEFT, GL_ALIGN_CENTER, FALSE, 100, FALSE, {2, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}} }
};

static long lMsgNbr;      // Message number

static const char *tzMsg[4] = { "One",      // Messages to write in Flash
                                "Two",
						        "Three",
						        "Seven" };

static char tcBufR[50+1]; // Reading buffer from file
static char tcBufW[50+1]; // Writing buffer into file
static int iLen;          // Message length

//****************************************************************************
//                          void DiskCreate (void)                            
//  This function creates, initializes (mount) and formats a disk.
//   - FS_dskcreate() : Create a volume or a disk.
//  This function has no parameters.    
//  This function has no return value.                                      
//****************************************************************************

void DiskCreate(void) {
	// Local variables 
    // ***************
	T_GL_HWIDGET hScreen=NULL;
	S_FS_PARAM_CREATE xCfg;
    unsigned long ulSize; // Not a doubleword but unsigned long inside prototype
    char *pcMsg=NULL;
    char tcDisplay[50+1];
	int iRet;
	
	// Create a screen
	// ===============
	hScreen = GoalCreateScreen(hGoal, txFFMS, NUMBER_OF_LINES(txFFMS), GL_ENCODING_UTF8);
	CHECK(hScreen!=NULL, lblKO);         // Create screen and clear it
	iRet = GoalClrScreen(hScreen, GL_COLOR_BLACK, KEY_CANCEL | KEY_VALID, false);
	CHECK(iRet>=0, lblKO);

	// Disk creation in progress
	// *************************
	strcpy(xCfg.Label, "TRAINING" );     // Disk name
    xCfg.Mode = FS_WRITEONCE;            // Disk on Flash
	xCfg.AccessMode	= FS_WRTMOD;         // r/w access
	xCfg.NbFichierMax = 16;              // Max files number
	xCfg.IdentZone	= FS_WO_ZONE_DATA;   // Zone id
	ulSize= xCfg.NbFichierMax*32768;     // Disk size in bytes

    iRet = FS_dskcreate(&xCfg, &ulSize); // Create and format the disk
    CHECK(iRet==FS_OK, lblKODisk);

	iRet = GoalDspLine(hScreen, 0, "Disk name: TRAINING", &txFFMS[0], 0, false);
	CHECK(iRet>=0, lblKO);
	iRet = GoalDspLine(hScreen, 1, "Disk created in Flash", &txFFMS[1], 0, false);
	CHECK(iRet>=0, lblKO);
	iRet = GoalDspLine(hScreen, 2, "Disk r/w access", &txFFMS[2], 0, false);
	CHECK(iRet>=0, lblKO);
	iRet = GoalDspLine(hScreen, 3, "Max files: 16", &txFFMS[3], 0, false);
	CHECK(iRet>=0, lblKO);
	Telium_Sprintf (tcDisplay, "Size: %ld bytes", (long) ulSize);
	iRet = GoalDspLine(hScreen, 4, tcDisplay, &txFFMS[4], 30*1000, true); // Show disk size
	CHECK(iRet>=0, lblKO);

	lMsgNbr=0;

    goto lblEnd;

	// Errors treatment 
    // ****************
lblKO:                                   // None-classified low level error
    GL_Dialog_Message(hGoal, NULL, "Processing Error", GL_ICON_ERROR, GL_BUTTON_VALID, 5*1000);
    goto lblEnd;
lblKODisk:                               // Disk error
    switch (iRet)
	{
	case FS_NAMEUSED: pcMsg = "Disk already created";    break;
	case FS_NOMEM:    pcMsg = "No space in Flash";       break;
	case FS_NOACCESS: pcMsg = "No access from Appli";    break;
	case FS_NOFORMAT: pcMsg = "Volume not formatted";    break;
	case FS_KO:
	default:          pcMsg = "Disk Flash KO";           break;
	}
    GL_Dialog_Message(hGoal, NULL, pcMsg, GL_ICON_ERROR, GL_BUTTON_VALID, 5*1000);
    goto lblEnd;
lblEnd:  
	if (hScreen)
		GoalDestroyScreen(&hScreen);     // Destroy screen
}

//****************************************************************************
//                          void DiskKill (void)                            
//  This function suppresses a disk.
//   - FS_mount()   : initialize the context of the volume disk.
//   - FS_unmount() : Release resources on disk.
//   - FS_dskkill() : Suppress a disk.
//  This function has no parameters.    
//  This function has no return value.                                      
//****************************************************************************

void DiskKill(void) 
{
	// Local variables 
    // ***************
	T_GL_HWIDGET hScreen=NULL;
	doubleword uiMode;  // Bug in prototype => 2nd parameter (output) on FS_mount returns Mode (FS_WRITEONCE or FS_WRITEMANY)
                        // and NOT AccessMode (FS_NOFLAGS or FS_RONLYMOD or FS_WRTMOD)
	char *pcMsg=NULL;
	int iRet;

	// Create a screen
	// ===============
	hScreen = GoalCreateScreen(hGoal, txFFMS, NUMBER_OF_LINES(txFFMS), GL_ENCODING_UTF8);
	CHECK(hScreen!=NULL, lblKO);            // Create screen and clear it
	iRet = GoalClrScreen(hScreen, GL_COLOR_BLACK, KEY_CANCEL | KEY_VALID, false);
	CHECK(iRet>=0, lblKO);

    // Mount the disk first
    // ********************
	iRet = FS_mount ("/TRAINING", &uiMode); // Activate a disk (!!! second parameter returns (FS_WRITEONCE or FS_WRITEMANY))
	CHECK(iRet==FS_OK, lblKODisk);

	// Disk suppression in progress
	// ****************************
    iRet = FS_unmount("/TRAINING");         // Release resources on disk
    CHECK(iRet==FS_OK, lblKODisk);
    iRet = FS_dskkill("/TRAINING");         // Disk suppression
    CHECK(iRet==FS_OK, lblKODisk);
    
	iRet = GoalDspLine(hScreen, 0, "Disk name: TRAINING", &txFFMS[0], 0, false);
	CHECK(iRet>=0, lblKO);
	iRet = GoalDspLine(hScreen, 1, "Disk killed in Flash", &txFFMS[1], 30*1000, true);
	CHECK(iRet>=0, lblKO);

    goto lblEnd;

	// Errors treatment 
    // ****************
lblKO:                                      // None-classified low level error
	GL_Dialog_Message(hGoal, NULL, "Processing Error", GL_ICON_ERROR, GL_BUTTON_VALID, 5*1000);
	goto lblEnd;
lblKODisk:                                  // Disk error
    switch (iRet)
	{
	case FS_NOACCESS: pcMsg = "No access from Appli";  break;
	case FS_DSKUSED:  pcMsg = "Disk is still mounted"; break;
	case FS_FILEUSED: pcMsg = "Files are still open";  break;
	case FS_KO:	
	default:          pcMsg = "Disk not found";        break;
	}
    GL_Dialog_Message(hGoal, NULL, pcMsg, GL_ICON_ERROR, GL_BUTTON_VALID, 5*1000);
    goto lblEnd;
lblEnd:
	if (hScreen)
		GoalDestroyScreen(&hScreen);        // Destroy screen
}

//****************************************************************************
//                          void DiskUse (void)                            
//  This function gives the space used by the disk in bytes (size-free).
//   - FS_mount()   : initialize the context of the volume disk.
//   - FS_dsksize() : Disk size in bytes.
//   - FS_dskfree() : Free space in bytes.
//  This function has no parameters.    
//  This function has no return value.                                      
//****************************************************************************

void DiskUse(void) 
{
	// Local variables 
    // ***************
	T_GL_HWIDGET hScreen=NULL;
	doubleword uiMode; // Bug in prototype => 2nd parameter (output) on FS_mount returns Mode (FS_WRITEONCE or FS_WRITEMANY)
                       // and NOT AccessMode (FS_NOFLAGS or FS_RONLYMOD or FS_WRTMOD)
	char *pcMsg;
	char tcDisplay[50+1];
    long lSize, lFree;          
    int iRet;

    // Create a screen
    // ===============
	hScreen = GoalCreateScreen(hGoal, txFFMS, NUMBER_OF_LINES(txFFMS), GL_ENCODING_UTF8);
	CHECK(hScreen!=NULL, lblKO);            // Create screen and clear it
	iRet = GoalClrScreen(hScreen, GL_COLOR_BLACK, KEY_CANCEL | KEY_VALID, false);
	CHECK(iRet>=0, lblKO);

    // Mount the disk first
    // ********************
    pcMsg = "Disk not found";
	iRet = FS_mount ("/TRAINING", &uiMode); // Activate a disk (!!! second parameter returns (FS_WRITEONCE or FS_WRITEMANY))
	CHECK(iRet==FS_OK, lblKODisk);

	// Disk space used info
	// ********************
    lSize = FS_dsksize("/TRAINING");        // Disk size in bytes
    CHECK(lSize!=FS_ERROR, lblKODisk);
    lFree = FS_dskfree("/TRAINING");        // Free space in bytes
    CHECK(lFree!=FS_ERROR, lblKODisk);

	iRet = GoalDspLine(hScreen, 0, "Disk name: TRAINING", &txFFMS[0], 0, false);
	CHECK(iRet>=0, lblKO);
	Telium_Sprintf(tcDisplay, "Used: %ld bytes", lSize-lFree);
	iRet = GoalDspLine(hScreen, 1, tcDisplay, &txFFMS[1], 30*1000, true); // Show disk size
	CHECK(iRet>=0, lblKO);

    goto lblEnd;

	// Errors treatment 
    // ****************
lblKO:                                      // None-classified low level error
	GL_Dialog_Message(hGoal, NULL, "Processing Error", GL_ICON_ERROR, GL_BUTTON_VALID, 5*1000);
	goto lblEnd;
lblKODisk:                                  // Disk error
	GL_Dialog_Message(hGoal, NULL, pcMsg, GL_ICON_ERROR, GL_BUTTON_VALID, 5*1000);
	goto lblEnd;
lblEnd:
	if (hScreen)
		GoalDestroyScreen(&hScreen);        // Destroy screen
}

//****************************************************************************
//                          void FileRead (void)                            
//  This function reads N bytes from the file.      
//   - GL_File_Open() : Open file in reading mode (Disk automatically mount).
//   - GL_File_GetSize() : Retrieve file length in bytes.
//   - GL_File_Seek() : Set the offset into the file.
//   - GL_File_Read() : Read N bytes from the file.
//   - GL_File_Close() : Close file.
//  This function has no parameters.    
//  This function has no return value.                                      
//****************************************************************************

void FileRead(void) {
	// Local variables 
    // ***************
	T_GL_HWIDGET hScreen=NULL;
	char *pcMsg;
	long lOfs=0;
	char tcDisplay[21+1];
	T_GL_HFILE hFile;                             // File handle
	int iRet;

    // Create a screen
    // ===============
	hScreen = GoalCreateScreen(hGoal, txFFMS, NUMBER_OF_LINES(txFFMS), GL_ENCODING_UTF8);
	CHECK(hScreen!=NULL, lblKO);                  // Create screen and clear it
	iRet = GoalClrScreen(hScreen, GL_COLOR_BLACK, KEY_CANCEL | KEY_VALID, false);
	CHECK(iRet>=0, lblKO);

    // File open in progress
	// *********************
	pcMsg = "File missing";
	hFile = GL_File_Open("file://flash/TRAINING/MyFile.txt", GL_FILE_OPEN_ALWAYS, GL_FILE_ACCESS_READ); // Open file in reading mode
    pcMsg = "File open failed";
    CHECK(hFile!=NULL, lblKOFile);

	// File length
    // ===========
    iLen = GL_File_GetSize(hFile);                // File length in bytes
    pcMsg = "File length failed";
	CHECK(iLen>=0, lblKOFile);

	// Offset start to file beginning
	// ==============================
	lOfs=0;
	iRet = GL_File_Seek(hFile, lOfs, GL_FILE_SEEK_BEGIN); // Seek given offset
    pcMsg = "File seek failed";
    CHECK(iRet==GL_SUCCESS, lblKOFile);

	// File read in progress
	// *********************
	memset (tcBufR, 0, sizeof(tcBufR));
	if (iLen != 0)
	{
		iRet = GL_File_Read(hFile, tcBufR, iLen); // Read from file
		pcMsg = "File read failed";
		CHECK(iRet==iLen, lblKOFile);
	}

	// File close in progress
	// **********************
	iRet = GL_File_Close(hFile);                  // Close file
    pcMsg = "File close failed";
    CHECK(iRet==GL_SUCCESS, lblKOFile);

	// Display file contain
	// ====================
	if (tcBufR[0] == 0)
		strcpy(tcBufR, "Empty");

	iRet = GoalDspLine(hScreen, 0, "File name: MyFile.txt", &txFFMS[0], 0, false);
	CHECK(iRet>=0, lblKO);                        // Show file contain in bytes
    Telium_Sprintf(tcDisplay, "Read: %d bytes", iLen);
	iRet = GoalDspLine(hScreen, 1, tcDisplay, &txFFMS[1], 0, false);
	CHECK(iRet>=0, lblKO);                        // Show number of byte
	iRet = GoalDspLine(hScreen, 2, tcBufR, &txFFMS[2], 30*1000, true);
	CHECK(iRet>=0, lblKO);                        // Show bytes
    
    goto lblEnd;

	// Errors treatment 
    // ****************
lblKO:                                            // None-classified low level error
	GL_Dialog_Message(hGoal, NULL, "Processing Error", GL_ICON_ERROR, GL_BUTTON_VALID, 5*1000);
    goto lblEnd;
lblKOFile:                                        // File error
	GL_Dialog_Message(hGoal, NULL, pcMsg, GL_ICON_ERROR, GL_BUTTON_VALID, 5*1000);
	goto lblEnd;
lblEnd:
	if (hScreen)
		GoalDestroyScreen(&hScreen);              // Destroy screen
}

//****************************************************************************
//                          void FileWrite (void)                            
//  This function writes N bytes into the file.      
//  NAND flash is composed of:
//    - 512 bytes(8MB->16MB) or 2048 bytes(32MB->128MB) per page.
//  Page is the unit for writing data in flash.
//  Block is the unit for erasing, pages are grouped in blocs.
//  Mechanism of buffered data is implemented so that writing is done per page of
//  512 bytes (8MB->16MB) or 2048 bytes (32MB->128MB).
//  Application can flush at any time and the flushing is automatically done when
//  closing the file.
//   - GL_File_Open() : Open file in writing mode (Disk automatically mount).
//   - GL_File_GetSize() : Retrieve file length in bytes.
//   - GL_File_Seek() : Set the offset into file.
//   - GL_File_Write() : Write N bytes into file.
//   - GL_File_Flush() : Flush buffer 512 bytes or 2048 bytes into file.
//   - GL_File_Close() : Close file.
//   - GL_File_Delete() : Delete file (Disk automatically mount).
//  This function has no parameters.    
//  This function has no return value.                                      
//****************************************************************************

void FileWrite(void) 
{
	// Local variables 
    // ***************
	T_GL_HWIDGET hScreen=NULL;
	long lOfs=0;
	char *pcMsg;
	char tcDisplay[21+1];
	T_GL_HFILE hFile;                       // File handle
	int iRet, iLen;

    // Create a screen
    // ===============
	hScreen = GoalCreateScreen(hGoal, txFFMS, NUMBER_OF_LINES(txFFMS), GL_ENCODING_UTF8);
	CHECK(hScreen!=NULL, lblKO);
	iRet = GoalClrScreen(hScreen, GL_COLOR_BLACK, KEY_CANCEL | KEY_VALID, false);
	CHECK(iRet>=0, lblKO);                  // Create screen and clear it

	// Delete file after 5 writes
	// **************************
	pcMsg = "File missing";
    if (lMsgNbr == 4){                      // Delete file
    	iRet= GL_File_Delete("file://flash/TRAINING/MyFile.txt");
    	CHECK(iRet==GL_SUCCESS, lblKOFile);
		lMsgNbr=0;
	}

    // File open in progress
	// *********************
	hFile = GL_File_Open("file://flash/TRAINING/MyFile.txt", GL_FILE_OPEN_ALWAYS, GL_FILE_ACCESS_WRITE); // Open file in writing mode
    pcMsg = "File open failed";
    CHECK(hFile!=NULL, lblKOFile);
    
	// File length
    // ===========
    iLen = GL_File_GetSize(hFile);          // File length in bytes
    pcMsg = "File length failed";
	CHECK(iLen>=0, lblKOFile);

	// Offset initialization
	// =====================
	if (lMsgNbr == 3) 
		lOfs=3;                             // Update the second message
	else 
		lOfs = iLen;                        // Write message at the end of file
	iRet = GL_File_Seek(hFile, lOfs, GL_FILE_SEEK_BEGIN); // Seek given offset
    pcMsg = "File seek failed";
    CHECK(iRet==GL_SUCCESS, lblKOFile);
    
	// File write in progress
	// **********************
	strcpy (tcBufW, tzMsg[lMsgNbr]);
	iLen = strlen(tcBufW);                  // Write into file
	iRet = GL_File_Write(hFile, tcBufW, iLen);
    pcMsg = "File write failed";
	CHECK(iRet==iLen, lblKOFile);
	lMsgNbr++;                              // Next message to write

	// Flush buffer 512 bytes (8MB->16MB) or 2048 bytes (32MB->128MB) on file
	// ======================================================================
	iRet = GL_File_Flush(hFile);            // Flush buffer into file
    pcMsg = "File flush failed";
    CHECK(iRet==GL_SUCCESS, lblKOFile);

	// File close in progress
	// **********************
	iRet = GL_File_Close(hFile);            // Close file
    pcMsg = "File close failed";            // Done automatically the flush if missing
    CHECK(iRet==GL_SUCCESS, lblKOFile);

	// Display file contain
	// ====================
	iRet = GoalDspLine(hScreen, 0, "File name: MyFile.txt", &txFFMS[0], 0, false);
	CHECK(iRet>=0, lblKO);                  // Show file contain in bytes
    Telium_Sprintf(tcDisplay, "Write: %d bytes", iLen);
	iRet = GoalDspLine(hScreen, 1, tcDisplay, &txFFMS[1], 0, false);
	CHECK(iRet>=0, lblKO);                  // Show number of byte
	iRet = GoalDspLine(hScreen, 2, tcBufW, &txFFMS[2], 30*1000, true);
	CHECK(iRet>=0, lblKO);                  // Show bytes

	goto lblEnd;

	// Errors treatment 
    // ****************
lblKO:                                      // None-classified low level error
	GL_Dialog_Message(hGoal, NULL, "Processing Error", GL_ICON_ERROR, GL_BUTTON_VALID, 5*1000);
    goto lblEnd;
lblKOFile:                                  // File error
	GL_Dialog_Message(hGoal, NULL, pcMsg, GL_ICON_ERROR, GL_BUTTON_VALID, 5*1000);
	goto lblEnd;
lblEnd:
	if (hScreen)
		GoalDestroyScreen(&hScreen);        // Destroy screen
}



unsigned char *LoadBMP(char *BmpFileName) {
	int k;
	char Disk[50];
	char FileName[50];
	int NameOK=0;
	S_FS_FILE *fdin;
	unsigned long fileLength;
	S_FS_PARAM_CREATE ParamCreat;
	int rc;
	unsigned char *BufferBmp;

	// get disk name
	if(BmpFileName[0]=='/') {
		for(k=1;k<strlen(BmpFileName);k++) {
			if(BmpFileName[k]=='/') {
				strncpy(Disk,BmpFileName,k);
				Disk[k]=0x0;
				Telium_Sprintf(FileName,&BmpFileName[k+1]);
				NameOK=1;
				break;
			}
		}
	}
	if(NameOK==0)
		return(NULL);

	ParamCreat.Mode = FS_READMOD;
	rc = FS_mount(Disk,&ParamCreat.Mode);
	if(rc!=FS_OK) // unable to mount the disk
		return(NULL);

	fdin=FS_open(BmpFileName,"r");
	if(fdin==NULL) //unable to open the file
		return(NULL);

	// Sizing Buffer memory length
	FS_seek(fdin,0,SEEK_END);
	fileLength=FS_tell(fdin);
	BufferBmp = umalloc(fileLength);

	FS_seek(fdin,0,SEEK_SET);
	// Copy to Buffer memory
	FS_read(BufferBmp,1,fileLength,fdin);
	FS_close(fdin);

	return (BufferBmp);
}

