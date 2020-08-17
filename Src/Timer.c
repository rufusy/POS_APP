//****************************************************************************
//       INGENICO                                INGEDEV 7                   
//============================================================================
//       FILE  TIMER.C                            (Copyright INGENICO 2012)
//============================================================================
//  Created :       13-July-2012     Kassovic
//  Last modified : 13-July-2012     Kassovic
//  Module : TRAINING                                                          
//                                                                          
//  Purpose :                                                               
//                          *** Timer management ***
//  How to start, get and stop a timer.
//  It is recommended to use:
//  - OS LAYER APIs to manage time stamp based on the function OSL_TimeStp_Now()
//    instead of OS API get_tick_counter().
//                                                                            
//  List of routines in file :  
//      TimerStart : Starts a timer number.
//      TimerGet : Returns the state of a timer number.
//      TimerStop : Stops a timer number.
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
#define TMR_NBR 4

//****************************************************************************
//      PRIVATE TYPES                                                       
//****************************************************************************
    /* */

//****************************************************************************
//      PRIVATE DATA                                                        
//****************************************************************************
// Properties of the Counter screen (Goal)
// =======================================
static const ST_DSP_LINE txCounter[] =
{
	{ {GL_ALIGN_CENTER, GL_ALIGN_CENTER, GL_COLOR_WHITE, GL_COLOR_BLACK, 100, FALSE, {0, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_WHITE}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_XLARGE}}, // Line0
	  {GL_ALIGN_CENTER, GL_ALIGN_CENTER, FALSE, 0, FALSE, {2, 0, 2, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_XLARGE}} },
	{ {GL_ALIGN_CENTER, GL_ALIGN_CENTER, GL_COLOR_WHITE, GL_COLOR_BLUE,   40, FALSE, {0, 3, 0, 3}, {1, 1, 1, 1, GL_COLOR_WHITE}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_LARGE}}, // Line1
	  {GL_ALIGN_CENTER, GL_ALIGN_CENTER, TRUE, 60, FALSE, {0, 4, 0, 6}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_XLARGE}} },
	{ {GL_ALIGN_CENTER, GL_ALIGN_CENTER, GL_COLOR_RED,   GL_COLOR_BLACK,   0, FALSE, {0, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_WHITE}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_LARGE}}, // Line3
	  {GL_ALIGN_CENTER, GL_ALIGN_CENTER, FALSE, 0, FALSE, {0, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}} }
};

static T_OSL_TIMESTP tllTimer[TMR_NBR];

//****************************************************************************
//             int TimerStart (byte ucTimerNbr, long lDelay)
//  This function starts a timer number for lDelay/1000 seconds.
//  After starting a timer a function TimerGet() should be called to know
//  whether it is over or not.
//  The timer should be over after iDelay/1000 seconds.
//   - OSL_TimeStp_Now() : Return number of elapsed ticks since startup(1tick=1ns).
//  This function has parameters.
//     ucTimerNbr (I-) : Timer number 0..3
//     iDelay (I-) : Initial timer value in thousandth of second (ms)
//  This function has return value.
//     >=0 : TimerStart done
//     <0  : TimerStart failed
//****************************************************************************

long TimerStart(byte ucTimerNbr, long lDelay)
{
	// Local variables 
    // ***************
	T_OSL_TIMESTP llTimer;

    // Start timer in progress
    // ***********************
    CHECK(ucTimerNbr<TMR_NBR, lblKO);

    // Get the current timer value
    // ===========================
    llTimer = OSL_TimeStp_Now();    // In 1ns ticks.

    // Set the timer to a 'period' into the future
    // ===========================================
    llTimer += (T_OSL_TIMESTP) lDelay*1000*1000;

    // Assign it to the proper timer
    // =============================
    tllTimer[ucTimerNbr]= llTimer;

    goto lblEnd;

	// Errors treatment 
    // ****************
lblKO:
    lDelay=-1;
lblEnd:
    return lDelay;
}

//****************************************************************************
//             int TimerGet (byte ucTimerNbr)                            
//  This function returns the state of the timer number. 
//   - OSL_TimeStp_Now() : Returns number of elapsed ticks since startup(1tick=1ns).
//   - OSL_TimeStp_ElapsedMs() : Gets elapsed time between 2 time stamp (end-begin in ms)
//  This function has parameters.
//     ucTimerNbr (I-) : Timer number 0..3
//  This function has return value.
//     >=0 : The number of milliseconds rest
//     <0  : TimerGet failed
//****************************************************************************

long TimerGet(byte ucTimerNbr)
{
	// Local variables 
    // ***************
	T_OSL_TIMESTP llTimerEnd, llTimer;
    long lRet;
    
    // Get timer in progress
    // *********************
    CHECK(ucTimerNbr<TMR_NBR, lblKO);

    // Get the remaining timer value before expiration
    // ===============================================
    llTimerEnd = tllTimer[ucTimerNbr];  // Retrieve the timer value to reach (ns)

    llTimer = OSL_TimeStp_Now();        // Get the current timer value (ns).

    if(llTimer < llTimerEnd)
    	lRet = OSL_TimeStp_ElapsedMs(llTimer, llTimerEnd);  // Return the remaining value (ms)
    else
    	lRet = 0;                                           // Timer expired
    
    goto lblEnd;

	// Errors treatment 
    // ****************
lblKO:
    lRet=-1;
lblEnd:
    return lRet;
}
          
//****************************************************************************
//               int TimerStop (byte ucTimerNbr)                            
//  This function should be called when the timer number is no more needed. 
//  This function has parameters.
//     ucTimerNbr (I-) : Timer number 0..3
//  This function has return value.
//     >=0 : TimerStop done
//     <0  : TimerStop failed
//****************************************************************************

int TimerStop(byte ucTimerNbr)
{
	// Local variables 
    // ***************
    int iRet;
	
	// Stop timer in progress
	// **********************
    CHECK(ucTimerNbr<TMR_NBR, lblKO);
    
    tllTimer[ucTimerNbr] = 0;
    
    iRet=0;
    goto lblEnd;

	// Errors treatment 
    // ****************
lblKO:
    iRet=-1;
lblEnd:
    return iRet; 
}

//****************************************************************************
//                            void Timer(void)                            
//  This function manages a timer. A timer counts thousandth of second.
//  When the timer expires after 15s, the terminal displays "Timer expired !!!".
//  This function has no parameters.
//  This function has no return value.
//****************************************************************************

void Timer(void)
{
	// Local variables 
    // ***************
	T_GL_HWIDGET xScreen=NULL;
	char tcDisplay[16+1];
	long lRet0;
    int iRet;
    
    // Timer demo in progress
    // **********************
	xScreen = GoalCreateScreen(hGoal, txCounter, NUMBER_OF_LINES(txCounter), GL_ENCODING_UTF8);
	CHECK(xScreen!=NULL, lblKO);       // Create screen and clear it
	iRet = GoalClrScreen(xScreen, GL_COLOR_BLACK, KEY_CANCEL, false);
	CHECK(iRet>=0, lblKO);

    iRet = GoalDspLine(xScreen, 0, "Expiration Time", &txCounter[0], 0, false);
	CHECK(iRet>=0, lblKO);
    lRet0 = TimerStart(0, 15*1000);    // Timer0 starts to 15s
    CHECK(lRet0>=0, lblKO);
    while ((lRet0=TimerGet(0)) > 0)    // Check Timer0
    {
    	CHECK(lRet0>=0, lblKO);
    	
        // Display Remaining Time
    	// ======================
        Telium_Sprintf(tcDisplay, "%2ld.%03ld Sec", lRet0/1000, lRet0%1000);
    	iRet = GoalDspLine(xScreen, 1, tcDisplay, &txCounter[1], 0, true);
		CHECK(iRet>=0, lblKO);
		CHECK(iRet!=GL_KEY_CANCEL, lblEnd);    // Exit on cancel key
    }

	iRet = GoalDspLine(xScreen, 1, " 0.000 Sec", &txCounter[1], 0, false);
	CHECK(iRet>=0, lblKO);
    buzzer(10);
	iRet = GoalDspLine(xScreen, 2, "Timer expired !!!", &txCounter[2], 5*1000, true);
	CHECK(iRet>=0, lblKO);

	goto lblEnd;

	// Errors treatment 
    // ****************
lblKO:                                 // None-classified low level error
	GL_Dialog_Message(hGoal, NULL, "Processing Error", GL_ICON_ERROR, GL_BUTTON_VALID, 5*1000);
lblEnd:
    TimerStop(0);                      // Stop Timer0

	if (xScreen)
		GoalDestroyScreen(&xScreen);   // Destroy screen
}

	
