//****************************************************************************
//       INGENICO                                INGEDEV 7                   
//============================================================================
//       FILE  FONTUTF8.C                         (Copyright INGENICO 2012)
//============================================================================
//  Created :       16-July-2012     Kassovic
//  Last modified : 16-July-2012     Kassovic
//  Module : TRAINING                                                          
//                                                                          
//  Purpose :                                                               
//                         *** UTF8 fonts coding ***
//  This file uses the Graphic Object Advanced Library (GOAL) for UTF font:
//    # Customized mode
//      . By API(s) (used inside this file)
//        Build your screen with font using API(s) widgets
//                                                                            
//  List of routines in file :
//      FontUtf8 : Demo UTF8 character set.
//                            
//  File history :
//  071612-BK : File created
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
#ifndef __TELIUM3__
	#define FONT_GOAL_CYRGRE_NO   0xACD8
	#define FONT_GOAL_ORIENTA_NO  0xACDC
#else
	#define FONT_GOAL_CYRGRE_NO   0x3E7ACD8
	#define FONT_GOAL_ORIENTA_NO  0x3E7ACDC
#endif

//****************************************************************************
//      PRIVATE TYPES                                                       
//****************************************************************************
    /* */

//****************************************************************************
//      PRIVATE DATA                                                        
//****************************************************************************
// Properties of the Font screen (Goal)
// ====================================
static const ST_DSP_LINE txFont[] =
{
	{ {GL_ALIGN_CENTER, GL_ALIGN_CENTER, GL_COLOR_WHITE, GL_COLOR_BLACK,  0,  TRUE,  {0, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_WHITE}, {0, 0, 0, 0}, {NULL,  GL_FONT_STYLE_NORMAL, GL_SCALE_LARGE}}, // Line0
	  {GL_ALIGN_CENTER, GL_ALIGN_CENTER, FALSE,  0,  TRUE, {0, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL,  GL_FONT_STYLE_NORMAL, GL_SCALE_XLARGE}} },
	{ {GL_ALIGN_LEFT,   GL_ALIGN_CENTER, GL_COLOR_WHITE, GL_COLOR_BLACK, 100, FALSE, {1, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_WHITE}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}}, // Line0
	  {GL_ALIGN_LEFT,   GL_ALIGN_CENTER, FALSE, 100, FALSE, {2, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}} },
	{ {GL_ALIGN_LEFT,   GL_ALIGN_CENTER, GL_COLOR_WHITE, GL_COLOR_BLACK, 100, FALSE, {1, 0, 1, 0}, {0, 0, 0, 0, GL_COLOR_WHITE}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}}, // Line1
	  {GL_ALIGN_LEFT,   GL_ALIGN_CENTER, FALSE, 100, FALSE, {2, 0, 2, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}} },
	{ {GL_ALIGN_LEFT,   GL_ALIGN_CENTER, GL_COLOR_WHITE, GL_COLOR_BLACK, 100, FALSE, {1, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_WHITE}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}}, // Line2
	  {GL_ALIGN_LEFT,   GL_ALIGN_CENTER, FALSE, 100, FALSE, {2, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}} },
};

// Properties of the default printer (Goal)
// ========================================
static const ST_PRN_LINE xPrinter =
{
	GL_ALIGN_CENTER, GL_ALIGN_CENTER, FALSE, 0, {0, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {1, 0, 1, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_XXLARGE}
};

//****************************************************************************
//                          void FontUtf8(void)
//  This function uses the UTF8 character set to show multi-fonts on screen:
//   German, Polish, Turkish, Russian, Arabic and Greek.
//  This function has no parameters.
//  This function has no return value.                                      
//****************************************************************************

void FontUtf8(void)
{
	// Local variables 
    // ***************
	T_GL_HWIDGET hScreen=NULL;
	ST_DSP_LINE xDspLine;
	T_GL_HWIDGET hDocument=NULL;
	ST_PRN_LINE xPrnLine;
	byte p; // Printer line index
	bool bFontOk;
	int iRet;
	
	                           // *********************
	                           //         UTF-8       *
	                           // *********************

	hScreen = GoalCreateScreen(hGoal, txFont, NUMBER_OF_LINES(txFont), GL_ENCODING_UTF8);
	CHECK(hScreen!=NULL, lblKO);                            // Create screen to UTF8

	// Display "Welcome" UTF8 coded (Latin)
	// ************************************
	iRet = GoalClrScreen(hScreen, GL_COLOR_BLACK, KEY_VALID | KEY_CANCEL, false);
	CHECK(iRet>=0, lblKO);                                  // Clear screen

	iRet = GoalDspLine(hScreen, 0, "FONT UTF-8", &txFont[0], 0, false);
	CHECK(iRet>=0, lblKO);
	                                                        // *** German utf-8 ***
 	iRet = GoalDspLine(hScreen, 1, "Mit freundlichen Grüßen", &txFont[1], 0, false);
	CHECK(iRet>=0, lblKO);
	                                                        // *** Polish utf-8 ***
	iRet = GoalDspLine(hScreen, 2, "Pożądany", &txFont[2], 0, false);
	CHECK(iRet>=0, lblKO);
	                                                        // *** Turkish utf-8 ***
	iRet = GoalDspLine(hScreen, 3, "Günaydin", &txFont[3], 0, true);
	CHECK(iRet>=0, lblKO);

	// Print "Welcome" UTF8 coded (Latin)
	// **********************************
	hDocument = GoalCreateDocument(hGoal, GL_ENCODING_UTF8);
	CHECK(hDocument!=NULL, lblKO);                          // Create document to UTF8

	p=0;                                                    // Build document (Demo, Multi-fonts msg)
	xPrnLine = xPrinter;
	xPrnLine.bReverse = TRUE;
	xPrnLine.usSizeWidth = 100;
	xPrnLine.xMargin.usBottom = PIXEL_BOTTOM;
	iRet = GoalPrnLine(hDocument, p++, "UTF-8 Demo", &xPrnLine);
	CHECK(iRet>=0, lblKO);

	xPrnLine = xPrinter;
	xPrnLine.eBackAlign = GL_ALIGN_LEFT;
	xPrnLine.xFont.eScale = GL_SCALE_LARGE;                 // *** German utf-8 ***
	iRet = GoalPrnLine(hDocument, p++, "Mit freundlichen Grüßen\n", &xPrnLine);
	CHECK(iRet>=0, lblKO);
	                                                        // *** Polish utf-8 ***
	iRet = GoalPrnLine(hDocument, p++, "Pożądany\n", &xPrnLine);
	CHECK(iRet>=0, lblKO);
	                                                        // *** Turkish utf-8 ***
	iRet = GoalPrnLine(hDocument, p++, "Günaydin\n\n\n\n", &xPrnLine);
	CHECK(iRet>=0, lblKO);

	iRet = GoalPrnDocument(hDocument);                      // Print document
	CHECK(iRet>=0, lblKO);

    if (hDocument)
    {
    	GoalDestroyDocument(&hDocument);                    // Destroy document
    	hDocument=NULL;
    }

	iRet = GoalGetKey(hScreen, hGoal, true, 30*1000, true); // Wait for key pressed/touched (shortcut)
	CHECK(iRet>=0, lblKO);
	CHECK(iRet!=GL_KEY_CANCEL, lblEnd);                     // Exit on cancel key

	// Display "Welcome" UTF8 coded (Cyrillic/Arabic)
	// **********************************************
	iRet = GoalClrScreen(hScreen, GL_COLOR_BLACK, KEY_CANCEL | KEY_VALID, false);
	CHECK(iRet>=0, lblKO);

	iRet = GoalDspLine(hScreen, 0, "FONT UTF-8", &txFont[0], 0, false);
	CHECK(iRet>=0, lblKO);

	bFontOk = IngenicoFontExist(FONT_GOAL_CYRGRE_NO, "GOAL_CYRGRE_NO"); // Check if Cyrillic/Greek (Iso8859-5/7 No) font loaded?
	if (bFontOk)
	{                                                       // *** Russian utf-8 ***
		iRet = GoalDspLine(hScreen, 1, "ДОБРО ПОЖАЛОВАТЬ", &txFont[1], 0, true);
		CHECK(iRet>=0, lblKO);
	}
	else
	{                                                       // Font Russian missing
		iRet = GoalDspLine(hScreen, 1, "Font Russian missing", &txFont[1], 0, true);
		CHECK(iRet>=0, lblKO);
	}

	bFontOk = IngenicoFontExist(FONT_GOAL_ORIENTA_NO, "GOAL_ORIENTA_NO"); // Check if Arabic/Hebrew (ISO 8859-6/8 No) font loaded?
	if (bFontOk)
	{
		xDspLine = txFont[2];
		xDspLine.xColor.eTextAlign=GL_ALIGN_RIGHT;
		xDspLine.xBW.eTextAlign=GL_ALIGN_RIGHT;             // *** Arabic utf-8 ***
		iRet = GoalDspLine(hScreen, 2, "السلام عليكم", &xDspLine, 0, true);
		CHECK(iRet>=0, lblKO);
	}
	else
	{                                                       // Font Arabic missing
		iRet = GoalDspLine(hScreen, 2, "Font Arabic missing", &txFont[2], 0, true);
		CHECK(iRet>=0, lblKO);
	}

	bFontOk = IngenicoFontExist(FONT_GOAL_CYRGRE_NO, "GOAL_CYRGRE_NO"); // Check if Cyrillic/Greek (Iso8859-5/7 No) font loaded?
	if (bFontOk)
	{                                                       // *** Greek utf-8 ***
		iRet = GoalDspLine(hScreen, 3, "καλημέρα", &txFont[3], 0, true);
		CHECK(iRet>=0, lblKO);
	}
	else
	{                                                       // Font Greek missing
		iRet = GoalDspLine(hScreen, 3, "Font Greek missing", &txFont[3], 0, true);
		CHECK(iRet>=0, lblKO);
	}

	// Print "Welcome" UTF8 coded (Cyrillic/Arabic)
	// ********************************************
	hDocument = GoalCreateDocument(hGoal, GL_ENCODING_UTF8);
	CHECK(hDocument!=NULL, lblKO);                          // Create document to UTF8

	p=0;                                                    // Build document (Demo, Multi-fonts msg)
	xPrnLine = xPrinter;
	xPrnLine.bReverse = TRUE;
	xPrnLine.usSizeWidth = 100;
	xPrnLine.xMargin.usBottom = PIXEL_BOTTOM;
	iRet = GoalPrnLine(hDocument, p++, "UTF-8 Demo", &xPrnLine);
	CHECK(iRet>=0, lblKO);

	xPrnLine = xPrinter;
	xPrnLine.eBackAlign = GL_ALIGN_LEFT;
	xPrnLine.xFont.eScale = GL_SCALE_LARGE;

	bFontOk = IngenicoFontExist(FONT_GOAL_CYRGRE_NO, "GOAL_CYRGRE_NO"); // Check if Cyrillic/Greek (Iso8859-5/7 No) font loaded?
	if (bFontOk)
	{                                                       // *** Russian utf-8 ***
		iRet = GoalPrnLine(hDocument, p++, "ДОБРО ПОЖАЛОВАТЬ\n", &xPrnLine);
		CHECK(iRet>=0, lblKO);
	}
	else
	{                                                       // Font Russian missing
		iRet = GoalPrnLine(hDocument, p++, "Font GOAL_CYRGRE_NO missing\n\n", &xPrnLine);
		CHECK(iRet>=0, lblKO);
	}

	bFontOk = IngenicoFontExist(FONT_GOAL_ORIENTA_NO, "GOAL_ORIENTA_NO"); // Check if Arabic/Hebrew (ISO 8859-6/8 No) font loaded?
	if (bFontOk)
	{                                                       // *** Arabic utf-8 ***
		xPrnLine.eBackAlign = GL_ALIGN_RIGHT;
		iRet = GoalPrnLine(hDocument, p++, "السلام عليكم""\n", &xPrnLine);
		CHECK(iRet>=0, lblKO);
	}
	else
	{                                                       // Font Arabic missing
		iRet = GoalPrnLine(hDocument, p++, "Font GOAL_ORIENTAL_NO missing\n\n", &xPrnLine);
		CHECK(iRet>=0, lblKO);
	}

	xPrnLine.eBackAlign = GL_ALIGN_LEFT;
	bFontOk = IngenicoFontExist(FONT_GOAL_CYRGRE_NO, "GOAL_CYRGRE_NO"); // Check if Cyrillic/Greek (Iso8859-5/7 No) font loaded?
	if (bFontOk)
	{                                                       // *** Greek utf-8 ***
		iRet = GoalPrnLine(hDocument, p++, "καλημέρα\n\n\n\n", &xPrnLine);
		CHECK(iRet>=0, lblKO);
	}
	else
	{                                                       // Font Greek missing
		iRet = GoalPrnLine(hDocument, p++, "Font GOAL_CYRGRE_NO missing\n\n\n\n\n", &xPrnLine);
		CHECK(iRet>=0, lblKO);
	}

	iRet = GoalPrnDocument(hDocument);                      // Print document
	CHECK(iRet>=0, lblKO);

	GoalGetKey(hScreen, hGoal, true, 30*1000, true);        // Wait for key pressed/touched (shortcut)

	goto lblEnd;

	// Errors treatment 
    // ****************
lblKO:                                                      // None-classified low level error
	GL_Dialog_Message(hGoal, NULL, "Processing Error", GL_ICON_ERROR, GL_BUTTON_VALID, 5*1000);
	goto lblEnd;
lblEnd:
	if (hScreen)
		GoalDestroyScreen(&hScreen);                        // Destroy screen
    if (hDocument)
    	GoalDestroyDocument(&hDocument);                    // Destroy document
}





