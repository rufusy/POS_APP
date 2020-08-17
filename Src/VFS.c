//****************************************************************************
//       INGENICO                                INGEDEV 7                   
//============================================================================
//       FILE  VFS.C                        (Copyright INGENICO 2012)
//============================================================================
//  Created :       13-July-2012         Kassovic
//  Last modified : 30-November-2013     Kassovic
//  Module : TRAINING                                                          
//                                                                          
//  Purpose :                                                               
//                  *** Virtual File System management ***
//     The VFS library provides a whole set of services allowing the applications
//     to manage MMC/SD/uSD card and USB key. It is recommended to use:
//     - GOAL API(s) to manage File and Directory (Url: protocol://drive/path).
//       [instead of VFS library].
//     To show how to open a file for writing and reading inside the MMC/SD/uSD
//     card or a USB key.
//
//  1) Load file VFS.TXT inside HOST disk with LLT.
//  2) Demo USB Key
//      - Write file VFS.TXT into USB key 
//      - Read file VFS.TXT from USB key
//      - Delete file VFS.TXT from USB key
//  3) Demo MMC card
//      - Write file VFS.TXT into MMC card 
//      - Read file VFS.TXT from MMC card
//      - Delete file VFS.TXT from MMC card
//                                                                                       
//  List of routines in file :  
//      VFSWrite : Write file VFS.TXT from HOST to VFS (USB/MMC).
//      VFSRead : Read file VFS.TXT from VFS (USB/MMC).
//      VFSDelete : Delete file VFS.TXT from VFS (USB/MMC). 
//                            
//  File history :
//  071312-BK : File created
//  113013-BK : VFS using GOAL
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
// Properties of the Ums screen (Goal)
// ===================================
static const ST_DSP_LINE txUms[] =
{
	{ {GL_ALIGN_CENTER, GL_ALIGN_CENTER, GL_COLOR_WHITE, GL_COLOR_BLACK, 0, FALSE, {0, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_WHITE}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_XLARGE}}, // Line0
	  {GL_ALIGN_CENTER, GL_ALIGN_CENTER, FALSE,  0, FALSE, {0, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_XLARGE}} },
	{ {GL_ALIGN_CENTER, GL_ALIGN_CENTER, GL_COLOR_WHITE, GL_COLOR_BLACK, 0, FALSE, {0, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_WHITE}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_XLARGE}}, // Line1
	  {GL_ALIGN_CENTER, GL_ALIGN_CENTER, FALSE,  0, FALSE, {0, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_XLARGE}} },
	{ {GL_ALIGN_CENTER, GL_ALIGN_CENTER, GL_COLOR_WHITE, GL_COLOR_BLACK, 0, FALSE, {0, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_WHITE}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}}, // Line2
	  {GL_ALIGN_CENTER, GL_ALIGN_CENTER, FALSE,  0, FALSE, {0, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_XLARGE}} }
};

// Properties of the File screen (Goal)
// ====================================
static const ST_DSP_LINE txFile[] =
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

// Properties of the default printer (Goal)
// ========================================
static const ST_PRN_LINE xPrinter =
{
	GL_ALIGN_LEFT, GL_ALIGN_CENTER, FALSE, 100, {0, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {1, 0, 1, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_LARGE}
};

static const char *tpcDirFile[] = { "file://umsd0a/VFS.TXT", "file://mmcd0a/VFS.TXT" }; // Path for VFS.TXT file
static const char *tpcInsert[] = { "USB Key", "MMC Card" };               // Prompt for USB key or MMC card
static const char *tpcVfsKO[] = { "USB Disk Failed", "MMC Disk Failed" }; // VFS problem

//****************************************************************************
//                    void VFSWrite (int VFSType)                             
//  This function writes the file VFS.TXT from the HOST disk to the USB key
//  or MMC/SD/uSD card.
//   - WaitForUms() : Wait for USB key or MMC/SD/uSD card.
//   - GL_File_Open() : Create/Open a file.
//        - "file://flash/HOST/VFS.TXT" => VFS.TXT (Host disk)
//        - "file://umsd0a/VFS.TXT" => VFS.TXT (USB key)
//        - "file://mmcd0a/VFS.TXT" => VFS.TXT (MMC/SD/uSD card)
//   - GL_File_GetSize() : Size of the file in bytes.
//   - GL_File_Read() : Read from a file.
//   - GL_File_Write() : Write into a file.
//   - GL_File_Close() : Close a file.
//  This function has parameters.  
//    VFSType (I-) : TYPE_USB or TYPE_MMC
//  This function has no return value.
//****************************************************************************

void VFSWrite(int VFSType) 
{
	// Local variables 
    // ***************
	T_GL_HWIDGET hScreen=NULL;
	T_GL_HFILE hFile;                              // File handle
	char tcData[512];
    int iLenData;
	int iLen, iRet;
    
	// Wait for USB key or MMC card
	// ****************************
	hScreen = GoalCreateScreen(hGoal, txUms, NUMBER_OF_LINES(txUms), GL_ENCODING_UTF8);
	CHECK(hScreen!=NULL, lblKO);                   // Create screen and clear it
	iRet = GoalClrScreen(hScreen, GL_COLOR_BLACK, KEY_CANCEL, true);
	CHECK(iRet>=0, lblKO);

	iRet = GoalDspLine(hScreen, 0, "Please Insert",  &txUms[0], 0, false);
	CHECK(iRet>=0, lblKO);                     	   // Insert USB key or MMC card

    iRet = TimerStart(0, 60*1000);                 // Timer0 starts to 60s
    CHECK(iRet>=0, lblKO);
	while(1)
	{
		iRet = WaitForUms(0, VFSType);             // Wait for USB key or MMC card

		CHECK(iRet!=CR_ENTRY_NOK, lblKO);
		if (iRet == CR_ENTRY_OK)                   // Device present?
			break;                                 // Yes, start the writing process

		iRet = TimerGet(0);                        // Retrieve timer value
		CHECK(iRet!=0, lblEnd);                    // Exit on timeout

		iRet = GoalDspLine(hScreen, 1, (char*)tpcInsert[VFSType], &txUms[1],  1000, true);
		CHECK(iRet!=GL_KEY_CANCEL, lblEnd);        // Exit on cancel key
	}

	// Retrieve data from file VFS.TXT located in HOST disk
	// ****************************************************
	hFile = GL_File_Open("file://flash/HOST/VFS.TXT", GL_FILE_OPEN_EXISTING, GL_FILE_ACCESS_READ); // Open the file VFS.TXT
	CHECK(hFile!=NULL, lblFileMissing);
	iLenData = GL_File_GetSize(hFile);             // File length in bytes
    memset (tcData, 0, sizeof(tcData));
    iRet = GL_File_Read(hFile, tcData, iLenData);  // Read data from file
    CHECK(iRet==iLenData, lblHostKO);
    iRet = GL_File_Close(hFile);                   // Close file
	CHECK(iRet==GL_SUCCESS, lblHostKO);
	
	GoalDestroyScreen(&hScreen);                   // Destroy screen

    // Copy data into file VFS.TXT located in USB/MMC disk
    // ***************************************************
	hScreen = GoalCreateScreen(hGoal, txFile, NUMBER_OF_LINES(txFile), GL_ENCODING_UTF8);
	CHECK(hScreen!=NULL, lblKO);                   // Create a new screen and clear it
	iRet = GoalClrScreen(hScreen, GL_COLOR_BLACK, 0, false);
	CHECK(iRet>=0, lblKO);
	iRet = GoalDspLine(hScreen, 0, "1-Open File", &txFile[1], 500, true);
	CHECK(iRet>=0, lblKO);                         // Create if not exist and open the file VFS.TXT
    hFile = GL_File_Open(tpcDirFile[VFSType], GL_FILE_OPEN_ALWAYS, GL_FILE_ACCESS_WRITE);
    CHECK(hFile!=NULL, lblVfsKO);

	iRet = GoalDspLine(hScreen, 1, "2-Write File", &txFile[2], 500, true);
	CHECK(iRet>=0, lblKO);
	iLen = GL_File_Write(hFile, tcData, iLenData); // Write data into file
    CHECK(iLen==iLenData, lblVfsKO);

	iRet = GoalDspLine(hScreen, 2, "3-Close File", &txFile[3], 10*100, true);
	CHECK(iRet>=0, lblKO);
	iRet = GL_File_Close(hFile);                   // Close file
    CHECK(iRet==GL_SUCCESS, lblVfsKO);

    goto lblEnd;

	// Errors treatment 
    // ****************
lblKO:                                             // None-classified low level error
	GL_Dialog_Message(hGoal, NULL, "Processing Error", GL_ICON_ERROR, GL_BUTTON_VALID, 5*1000);
    goto lblEnd;
lblHostKO:                                         // HOST disk failed
	GL_Dialog_Message(hGoal, NULL, "HOST Disk Failed", GL_ICON_ERROR, GL_BUTTON_VALID, 5*1000);
    goto lblEnd;
lblVfsKO:                                          // USB/MMC disk failed
	GL_Dialog_Message(hGoal, NULL, tpcVfsKO[VFSType], GL_ICON_ERROR, GL_BUTTON_VALID, 5*1000);
    goto lblEnd;
lblFileMissing:                                    // File not found
	GL_Dialog_Message(hGoal, NULL, "File VFS.TXT\nMissing from Host", GL_ICON_ERROR, GL_BUTTON_VALID, 5*1000);
	goto lblEnd;
lblEnd:
	TimerStop(0);                                  // Stop Timer0

	if (hScreen)
		GoalDestroyScreen(&hScreen);               // Destroy screen
}

//****************************************************************************
//                       void VFSRead (int VFSType)                             
//  This function reads the file VFS.TXT from the the USB key or MMC/SD/uSD
//  card.
//   - WaitForUms() : Wait for USB key or MMC/SD/uSD card.
//   - GL_File_Open() : Create/Open a file.
//        - "file://umsd0a/VFS.TXT" => VFS.TXT (USB key)
//        - "file://mmcd0a/VFS.TXT" => VFS.TXT (MMC/SD/uSD card)
//   - GL_File_GetSize() : Size of the file in bytes.
//   - GL_File_Read() : Read from a file.
//   - GL_File_Close() : Close a file.
//  This function has parameters.  
//    VFSType (I-) : TYPE_USB or TYPE_MMC
//  This function has no return value.
//****************************************************************************

void VFSRead(int VFSType) 
{
	// Local variables 
    // ***************
	T_GL_HWIDGET hScreen=NULL;
	T_GL_HWIDGET hDocument=NULL;
	ST_PRN_LINE xLine;
	T_GL_HFILE hFile;                                // File handle
	char tcData[512];
	char tcDisplay[50+1];
	char tcPrint[50+1];
    int iLenData;
    byte p; // Printer line index
	int iLen, iRet;
    	
	// Wait for USB key or MMC card
	// ****************************
	hScreen = GoalCreateScreen(hGoal, txUms, NUMBER_OF_LINES(txUms), GL_ENCODING_UTF8);
	CHECK(hScreen!=NULL, lblKO);                     // Create screen and clear it
	iRet = GoalClrScreen(hScreen, GL_COLOR_BLACK, KEY_CANCEL, true);
	CHECK(iRet>=0, lblKO);

	iRet = GoalDspLine(hScreen, 0, "Please Insert",  &txUms[0], 0, false);
	CHECK(iRet>=0, lblKO);                     	     // Insert USB key or MMC card

    iRet = TimerStart(0, 60*1000);                   // Timer0 starts to 60s
    CHECK(iRet>=0, lblKO);
	while(1)
	{
		iRet = WaitForUms(0, VFSType);               // Wait for USB key or MMC card

		CHECK(iRet!=CR_ENTRY_NOK, lblKO);
		if (iRet == CR_ENTRY_OK)                     // Device present?
			break;                                   // Yes, start the writing process

		iRet = TimerGet(0);                          // Retrieve timer value
		CHECK(iRet!=0, lblEnd);                      // Exit on timeout

		iRet = GoalDspLine(hScreen, 1, (char*)tpcInsert[VFSType], &txUms[1],  1000, true);
		CHECK(iRet!=GL_KEY_CANCEL, lblEnd);          // Exit on cancel key
	}

    GoalDestroyScreen(&hScreen);                     // Destroy screen

    // Read file VFS.TXT located in USB/MMC disk
    // *****************************************
	hScreen = GoalCreateScreen(hGoal, txFile, NUMBER_OF_LINES(txFile), GL_ENCODING_UTF8);
	CHECK(hScreen!=NULL, lblKO);                     // Create a new screen and clear it
	iRet = GoalClrScreen(hScreen, GL_COLOR_BLACK, KEY_CANCEL | KEY_VALID, false);
	CHECK(iRet>=0, lblKO);
	iRet = GoalDspLine(hScreen, 0, "1-Open File", &txFile[1], 500, true);
	CHECK(iRet>=0, lblKO);                           // Open the file VFS.TXT
	hFile = GL_File_Open(tpcDirFile[VFSType], GL_FILE_OPEN_EXISTING, GL_FILE_ACCESS_READ);
    CHECK(hFile!=NULL, lblFileMissing);

	iRet = GoalDspLine(hScreen, 1, "2-Read File", &txFile[2], 500, true);
	CHECK(iRet>=0, lblKO);
    iLenData = GL_File_GetSize(hFile);               // Get size of the file
	CHECK(iLenData>=0, lblVfsKO);
    memset (tcData, 0, sizeof(tcData));
	iLen = GL_File_Read(hFile, tcData, iLenData);    // Read data from file
    CHECK(iLen==iLenData, lblVfsKO);

	iRet = GoalDspLine(hScreen, 2, "3-Close File", &txFile[3], 500, true);
	CHECK(iRet>=0, lblKO);
	iRet = GL_File_Close(hFile);                     // Close file
	CHECK(iRet==GL_SUCCESS, lblVfsKO);

    // Display file VFS.TXT
    // ********************
	iRet = GoalClrScreen(hScreen, GL_COLOR_BLACK, KEY_CANCEL | KEY_VALID, false);
	CHECK(iRet>=0, lblKO);
	iRet = GoalDspMultiLine(hScreen, 0, tcData, strlen(tcData), txFile, NUMBER_OF_LINES(txFile), 0, true);
	CHECK(iRet>=0, lblKO);

    // Print file VFS.TXT
	// ******************
	p=0;                                             // Create document
	hDocument = GoalCreateDocument(hGoal, GL_ENCODING_UTF8);
	CHECK(hDocument!=NULL, lblKO);

	xLine = xPrinter;                                // Build document (Demo, File contain, data)
	xLine.eTextAlign = GL_ALIGN_CENTER;
	xLine.bReverse = TRUE;
	xLine.xMargin.usBottom = PIXEL_BOTTOM;
	xLine.xFont.eScale = GL_SCALE_XXLARGE;
	iRet = GoalPrnLine(hDocument, p++, "VFS Demo", &xLine);
	CHECK(iRet>=0, lblKO);

	xLine = xPrinter;                                // Data to print
    Telium_Sprintf(tcPrint, "%s\n", tpcDirFile[VFSType]);
	iRet = GoalPrnLine(hDocument, p++, tcPrint, &xLine);
	CHECK(iRet>=0, lblKO);
    iRet = GoalPrnLine(hDocument, p++, tcData, &xLine);
	CHECK(iRet>=0, lblKO);
	iRet = GoalPrnLine(hDocument, p++, "\n\n\n\n", &xLine);
	CHECK(iRet>=0, lblKO);

	iRet = GoalPrnDocument(hDocument);               // Print document
	CHECK(iRet>=0, lblKO);

	GoalGetKey(hScreen, hGoal, true, 30*1000, true); // Wait for key pressed/touched (shortcut)
    
    goto lblEnd;

	// Errors treatment 
    // ****************
lblKO:                                               // None-classified low level error
	GL_Dialog_Message(hGoal, NULL, "Processing Error", GL_ICON_ERROR, GL_BUTTON_VALID, 5*1000);
    goto lblEnd;
lblVfsKO:                                            // USB/MMC disk failed
	GL_Dialog_Message(hGoal, NULL, tpcVfsKO[VFSType], GL_ICON_ERROR, GL_BUTTON_VALID, 5*1000);
    goto lblEnd;
lblFileMissing:                                      // File not found
	Telium_Sprintf(tcDisplay, "FILE NOT FOUND\n%s", tpcDirFile[VFSType]);
	GL_Dialog_Message(hGoal, NULL, tcDisplay, GL_ICON_ERROR, GL_BUTTON_VALID, 5*1000);
	goto lblEnd;
lblEnd: 
	TimerStop(0);                                    // Stop Timer0

	if (hScreen)
		GoalDestroyScreen(&hScreen);                 // Destroy screen
    if (hDocument)
    	GoalDestroyDocument(&hDocument);             // Destroy document
}

//****************************************************************************
//                       void VFSDelete (int VFSType)                             
//  This function deletes the file VFS.TXT from the the USB key or MMC/SD/uSD
//  card.
//   - WaitForUms() : Wait for USB key or MMC/SD/uSD card.
//   - GL_File_Delete() : Delete a file.
//  This function has parameters.  
//    VFSType (I-) : TYPE_USB or TYPE_MMC
//  This function has no return value.
//****************************************************************************

void VFSDelete(int VFSType) 
{
	// Local variables 
    // ***************
	T_GL_HWIDGET hScreen=NULL;
	char tcDisplay[50+1];
	int iRet;
	
	// Wait for USB key or MMC card
	// ****************************
	hScreen = GoalCreateScreen(hGoal, txUms, NUMBER_OF_LINES(txUms), GL_ENCODING_UTF8);
	CHECK(hScreen!=NULL, lblKO);               // Create screen and clear it
	iRet = GoalClrScreen(hScreen, GL_COLOR_BLACK, KEY_CANCEL, true);
	CHECK(iRet>=0, lblKO);

	iRet = GoalDspLine(hScreen, 0, "Please Insert",  &txUms[0], 0, false);
	CHECK(iRet>=0, lblKO);                     // Insert USB key or MMC card

    iRet = TimerStart(0, 60*1000);             // Timer0 starts to 60s
    CHECK(iRet>=0, lblKO);
	while(1)
	{
		iRet = WaitForUms(0, VFSType);         // Wait for USB key or MMC card

		CHECK(iRet!=CR_ENTRY_NOK, lblKO);
		if (iRet == CR_ENTRY_OK)               // Device present?
			break;                             // Yes, start the writing process

		iRet = TimerGet(0);                    // Retrieve timer value
		CHECK(iRet!=0, lblEnd);                // Exit on timeout

		iRet = GoalDspLine(hScreen, 1, (char*)tpcInsert[VFSType], &txUms[1], 1000, true);
		CHECK(iRet!=GL_KEY_CANCEL, lblEnd);    // Exit on cancel key
	}

    GoalDestroyScreen(&hScreen);               // Destroy screen

    // Delete file VFS.TXT located in USB/MMC disk
    // *******************************************
	hScreen = GoalCreateScreen(hGoal, txFile, NUMBER_OF_LINES(txFile), GL_ENCODING_UTF8);
	CHECK(hScreen!=NULL, lblKO);               // Create a new screen and clear it
	iRet = GoalClrScreen(hScreen, GL_COLOR_BLACK, 0, false);
	CHECK(iRet>=0, lblKO);

	iRet = GoalDspLine(hScreen, 0, "1-Delete File", &txFile[1], 500, true);
	CHECK(iRet>=0, lblKO);
	iRet= GL_File_Delete(tpcDirFile[VFSType]); // Delete file VFS.TXT
	CHECK(iRet==GL_SUCCESS, lblFileMissing);
    
    goto lblEnd;

	// Errors treatment 
    // ****************
lblKO:                                         // None-classified low level error
	GL_Dialog_Message(hGoal, NULL, "Processing Error", GL_ICON_ERROR, GL_BUTTON_VALID, 5*1000);
    goto lblEnd;
lblFileMissing:                                // File not found
	Telium_Sprintf(tcDisplay, "ALREADY DELETED\n%s", tpcDirFile[VFSType]);
	GL_Dialog_Message(hGoal, NULL, tcDisplay, GL_ICON_ERROR, GL_BUTTON_VALID, 5*1000);
	goto lblEnd;
lblEnd: 
	TimerStop(0);                              // Stop Timer0

	if (hScreen)
		GoalDestroyScreen(&hScreen);           // Destroy screen
}
