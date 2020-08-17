//****************************************************************************
//       INGENICO                                INGEDEV 7                   
//============================================================================
//       FILE  BUZZER.C                            (Copyright INGENICO 2012)
//============================================================================
//  Created :       13-July-2012     Kassovic
//  Last modified : 13-July-2012     Kassovic
//  Module : TRAINING                                                          
//                                                                          
//  Purpose :                                                               
//                  *** Buzzer management ***
//  How to set a frequency and adjust the volume.
//                                                                            
//  List of routines in file :  
//      Buzzer : Frequency 872Hz, increase or decrease volume.
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
// Properties of the Buzzer screen (Goal)
// ======================================
static const ST_DSP_LINE txBuzzer[] =
{
	{ {GL_ALIGN_CENTER, GL_ALIGN_RIGHT,  GL_COLOR_WHITE, GL_COLOR_GREEN, 25, TRUE,  {0, 0, 0, 0}, {1, 1, 1, 1, GL_COLOR_WHITE}, {0, 2, 3, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_XSMALL}}, // Line0
	  {GL_ALIGN_CENTER, GL_ALIGN_RIGHT,  FALSE,  0,  TRUE, {0, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 1, 2, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_XXSMALL}} },
	{ {GL_ALIGN_CENTER, GL_ALIGN_CENTER, GL_COLOR_WHITE, GL_COLOR_RED,   60, FALSE, {0, 4, 0, 4}, {1, 1, 1, 1, GL_COLOR_WHITE}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_LARGE}},  // Line1
	  {GL_ALIGN_CENTER, GL_ALIGN_CENTER, TRUE,  74, FALSE, {0, 2, 0, 4}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_XLARGE  }} },
	{ {GL_ALIGN_CENTER, GL_ALIGN_CENTER, GL_COLOR_WHITE, GL_COLOR_BLUE,  60, FALSE, {0, 4, 0, 4}, {1, 1, 1, 1, GL_COLOR_WHITE}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_LARGE}},  // Line2
	  {GL_ALIGN_CENTER, GL_ALIGN_CENTER, TRUE,  74, FALSE, {0, 2, 0, 4}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_XLARGE  }} },
	{ {GL_ALIGN_CENTER, GL_ALIGN_RIGHT,  GL_COLOR_WHITE, GL_COLOR_GREEN, 25, TRUE,  {0, 0, 0, 0}, {1, 1, 1, 1, GL_COLOR_WHITE}, {0, 0, 3, 2}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_XSMALL}}, // Line3
	  {GL_ALIGN_CENTER, GL_ALIGN_RIGHT,  FALSE,  0,  TRUE, {0, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 1, 2, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_XXSMALL}} },
};

//****************************************************************************
//                         void Buzzer (void)                            
//  This function sets a buzzer to 872Hz and shows how to decrease or increase
//  the volume.
//   - StartBuzzer() : start buzzer
//   - StopBuzzer() : stop buzzer
//   Divisor=0 => TIC=2/57MHZ = 0.035us
//   Delay_total=0x8000 => total period = 0x8000*0.035us = 1.147ms (872Hz)
//   Delay_high=0 => Volume Min
//   Delay_high=9*500 => Volume Max
//  This function has no parameters.    
//  This function has no return value.                                      
//****************************************************************************

void Buzzer(void) {
	// Local variables 
    // ***************
	T_GL_HWIDGET hScreen=NULL;
    char cVal='%';
    char tcDisplay[50+1];
	int iIdx, iLevel;	
    int iKey, iRet;

    // Buzzer management
	// *****************
	hScreen = GoalCreateScreen(hGoal, txBuzzer, NUMBER_OF_LINES(txBuzzer), GL_ENCODING_UTF8);
	CHECK(hScreen!=NULL, lblKO);                                             // Create screen and clear it
	iRet = GoalClrScreen(hScreen, GL_COLOR_BLACK, KEY_CANCEL | KEY_UP | KEY_DOWN, false);
	CHECK(iRet>=0, lblKO);

    iRet = GoalDspLine(hScreen, 0, "Up to inc", &txBuzzer[0], 0, false);     // Show press up to increase buzzer
    CHECK(iRet>=0, lblKO);
	iRet = GoalDspLine(hScreen, 1, "Sound:  872Hz", &txBuzzer[1], 0, false); // Show frequency used
	CHECK(iRet>=0, lblKO);
	iRet = GoalDspLine(hScreen, 3, "Down to dec", &txBuzzer[3], 0, false);   // Show press down to decrease buzzer
    CHECK(iRet>=0, lblKO);

	iIdx=0;
	iLevel=0x8000;
	iRet = StartBuzzer(0, iLevel, 0x8000);                                   // Set buzzer 872Hz
	while(1)
	{
    	Telium_Sprintf (tcDisplay, "Volume: %2d%c", 10*iIdx, cVal);
    	iRet = GoalDspLine(hScreen, 2, tcDisplay, &txBuzzer[2], 0, false);   // Show sound percentage
    	CHECK(iRet>=0, lblKO);

		iKey = GoalGetKey(hScreen, hGoal, true, 30*1000, true);              // Wait for a key pressed/touched (shortcut)
		CHECK(iKey>=0, lblKO);

        if(iKey==0)
            break;                                                           // Exit on timeout
        if(iKey==GL_KEY_CANCEL)
            break;                                                           // Exit on cancel/valid key

        if (iKey==GL_KEY_UP)                                                 // *** Key UP ***
		{
			if(iIdx < 10)
			{
				iIdx++;                                               
				iLevel = iLevel-153;                                         // Increase volume
#ifdef __TELIUM3__
				StopBuzzer();
#endif
				StartBuzzer(0, iLevel, 0x8000);                              // Set buzzer 872Hz
			}
		}

		if (iKey==GL_KEY_DOWN)                                               // *** Key DOWN ***
		{
			if (iIdx > 0)
			{
				iIdx--;
				iLevel = iLevel+153;                                         // Decrease volume
#ifdef __TELIUM3__
				StopBuzzer();
#endif
				StartBuzzer(0, iLevel, 0x8000);                              // Set buzzer 872Hz
			}
		}
    }

	goto lblEnd;

	// Errors treatment
    // ****************
lblKO:                                                                       // None-classified low level error
	GL_Dialog_Message(hGoal, NULL, "Processing Error", GL_ICON_ERROR, GL_BUTTON_VALID, 3*1000);
	goto lblEnd;
lblEnd:
	StopBuzzer();                                                            // Stop buzzer
	if (hScreen)
		GoalDestroyScreen(&hScreen);                                         // Destroy screen
}

