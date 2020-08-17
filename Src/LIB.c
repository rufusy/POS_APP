//****************************************************************************
//       INGENICO                                INGEDEV 7                   
//============================================================================
//       FILE  LIB.C                             (Copyright INGENICO 2012)
//============================================================================
//  Created :       13-July-2012     Kassovic
//  Last modified : 13-July-2012     Kassovic
//  Module : TRAINING                                                          
//                                                                          
//  Purpose :                                                               
//                         *** Static LIB management ***
//      Show how to call a LIB.
//      The static library BeepLib is generated during the training.
//                                                                            
//  List of routines in file :  
//      BeepLib : Beep LIB demo.                                            
//                            
//  File history :
//  071312-BK : File created
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
// Properties of the Lib screen (Goal)
// ===================================
static const ST_DSP_LINE txLib[] =
{
	{ {GL_ALIGN_CENTER, GL_ALIGN_CENTER, GL_COLOR_WHITE, GL_COLOR_BLACK, 0, FALSE, {0, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_WHITE}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_XLARGE}}, // Line0
	  {GL_ALIGN_CENTER, GL_ALIGN_CENTER, FALSE,  0, FALSE, {0, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_XLARGE}} },
	{ {GL_ALIGN_CENTER, GL_ALIGN_CENTER, GL_COLOR_WHITE, GL_COLOR_BLACK, 0, FALSE, {0, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_WHITE}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_XLARGE}}, // Line1
	  {GL_ALIGN_CENTER, GL_ALIGN_CENTER, FALSE,  0, FALSE, {0, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_XLARGE}} },
	{ {GL_ALIGN_CENTER, GL_ALIGN_CENTER, GL_COLOR_WHITE, GL_COLOR_BLACK, 0, FALSE, {0, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_WHITE}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_XLARGE}}, // Line2
	  {GL_ALIGN_CENTER, GL_ALIGN_CENTER, FALSE,  0, FALSE, {0, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_XLARGE}} }
};

//****************************************************************************
//                          void BeepLib (void)                            
//  This function runs the LIB called Beep.      
//  This function has no parameters.    
//  This function has no return value.
//****************************************************************************

void BeepLib(void) 
{
	// Local variables 
    // ***************
	T_GL_HWIDGET hScreen=NULL;
    int iRet;

	// Call the functions from the LIB
	// ===============================
	hScreen = GoalCreateScreen(hGoal, txLib, NUMBER_OF_LINES(txLib), GL_ENCODING_UTF8);
	CHECK(hScreen!=NULL, lblKO);       // Create screen and clear it
	iRet = GoalClrScreen(hScreen, GL_COLOR_BLACK, KEY_CANCEL | KEY_VALID, false);
	CHECK(iRet>=0, lblKO);

	iRet = GoalDspLine(hScreen, 0, "Press Valid", &txLib[0], 0, false);
	CHECK(iRet>=0, lblKO);
	iRet = GoalDspLine(hScreen, 1, "To hear 1 Beep", &txLib[1], 30*1000, true);
	CHECK(iRet>=0, lblKO);
	CHECK(iRet==GL_KEY_VALID, lblEnd); // Exit on cancel key or timeout

//	OneBeep();                         // Call function OneBeep()

	iRet = GoalClrScreen(hScreen, GL_COLOR_BLACK, KEY_CANCEL | KEY_VALID, false); // Clear screen
	CHECK(iRet>=0, lblKO);
	iRet = GoalDspLine(hScreen, 0, "Press Valid", &txLib[0], 0, false);
	CHECK(iRet>=0, lblKO);
	iRet = GoalDspLine(hScreen, 1, "To hear 3 Beeps", &txLib[1], 30*1000, true);
	CHECK(iRet>=0, lblKO);
	CHECK(iRet==GL_KEY_VALID, lblEnd); // Exit on cancel key or timeout

//	iRet = ManyBeep(3);                // Call function ManyBeep()
//	CHECK(iRet==2*3, lblKO);

    goto lblEnd;

	// Errors treatment 
    // ****************
lblKO:                                 // None-classified low level error
	GL_Dialog_Message(hGoal, NULL, "Processing Error", GL_ICON_ERROR, GL_BUTTON_VALID, 5*1000);
	goto lblEnd;
lblEnd:
	if (hScreen)
		GoalDestroyScreen(&hScreen);
}



