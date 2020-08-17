//****************************************************************************
//       INGENICO                                INGEDEV 7                   
//============================================================================
//       FILE  TASKSEMAPHORE.C                    (Copyright INGENICO 2013)
//============================================================================
//  Created :       26-November-2013     Kassovic
//  Last modified : 26-November-2013     Kassovic
//  Module : TRAINING
//                                                                          
//  Purpose :                                                               
//  A Mutex is a technique used in computer science to get exclusive access
//  to shared resources or critical section.
//  A mutex can be used only for inter-thread protection. To use a mutex for
//  inter-processing synchronization, prefer Semaphore (see below).
//  It is recommended to use :
//  - OS LAYER APIs to manage mutex (inter-processing synchronization) instead
//    of RTOS APIs.
//  How to create a mutex using semaphore through 2 tasks fork from the
//  Training application and 2 tasks fork from User2 application.
//  The critical zone of each task is shown by a spinning bar and protected by
//  a mutex.
//                                                                            
//  List of routines in file :  
//      TaskA : Manage the first task (Training) protected by a mutex.
//      TaskB : Manage the second task (Training) protected by a mutex.
//      TaskSemaphore : Mutex demo (inter-processing synchronization).
//                            
//  File history :
//  231113-BK : File created
//                                                                           
//****************************************************************************

//****************************************************************************
//      INCLUDES                                                            
//****************************************************************************
#include <globals.h>

//****************************************************************************
//      EXTERN                                                              
//****************************************************************************
// Handles created at application start-up (see Entry.c)
// =====================================================
extern T_GL_HGRAPHIC_LIB hGoal;  // Handle of the graphics object library

extern T_OSL_HSEMAPHORE hSemS2;  // Handle of the second semaphore (security SHARED)

//****************************************************************************
//      PRIVATE CONSTANTS                                                   
//****************************************************************************
#define MAX_STATE 4
#define MAX_LOOP 48

//****************************************************************************
//      PRIVATE TYPES                                                       
//****************************************************************************
	/* */

//****************************************************************************
//      PRIVATE DATA                                                        
//****************************************************************************
// Properties of the Semaphore screen (Goal)
// =========================================
static const ST_DSP_LINE txSemaphore[] =
{
	{ {GL_ALIGN_CENTER, GL_ALIGN_CENTER, GL_COLOR_WHITE, GL_COLOR_BLACK,  0, FALSE, {0, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_WHITE}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_SMALL}}, // Line0
	  {GL_ALIGN_CENTER, GL_ALIGN_CENTER, FALSE, 0, FALSE, {0, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}} },
	{ {GL_ALIGN_LEFT, GL_ALIGN_CENTER, GL_COLOR_WHITE, GL_COLOR_RED,    100, FALSE, {1, 0, 0, 0}, {1, 1, 1, 1, GL_COLOR_WHITE}, {3, 0, 3, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_SMALL}}, // Line1
	  {GL_ALIGN_LEFT, GL_ALIGN_CENTER, TRUE,  100, FALSE, {2, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}} },
	{ {GL_ALIGN_LEFT, GL_ALIGN_CENTER, GL_COLOR_WHITE, GL_COLOR_GREEN,  100, FALSE, {1, 0, 0, 0}, {1, 1, 1, 1, GL_COLOR_WHITE}, {3, 0, 3, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_SMALL}}, // Line2
	  {GL_ALIGN_LEFT, GL_ALIGN_CENTER, TRUE,  100, FALSE, {2, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}} },
	{ {GL_ALIGN_LEFT, GL_ALIGN_CENTER, GL_COLOR_WHITE, GL_COLOR_BLUE,   100, FALSE, {1, 0, 0, 0}, {1, 1, 1, 1, GL_COLOR_WHITE}, {3, 0, 3, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_SMALL}}, // Line3
	  {GL_ALIGN_LEFT, GL_ALIGN_CENTER, TRUE,  100, FALSE, {2, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}} },
	{ {GL_ALIGN_LEFT, GL_ALIGN_CENTER, GL_COLOR_WHITE, GL_COLOR_ORANGE, 100, FALSE, {1, 0, 0, 0}, {1, 1, 1, 1, GL_COLOR_WHITE}, {3, 0, 3, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_SMALL}}, // Line3
	  {GL_ALIGN_LEFT, GL_ALIGN_CENTER, TRUE,  100, FALSE, {2, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}} }
};

static const char ttzLogoCL[MAX_STATE][4+1] = {"  | ", "  / ", " ---", "  \\ "};
static const char ttzLogoBW[MAX_STATE][4+1] = {"  | ", " /  ", "--  ", " \\  "};
static char ttcLogo[MAX_STATE][4+1];
static doubleword uiTimeOut;

static T_GL_HWIDGET hScreen=NULL; // Handle of the screen (Goal)

static word usMainTaskNbr;

//****************************************************************************
//                          word TaskA (void)
//  This function manages the TaskA which is protected by a mutex using a
//  semaphore. The critical zone is shown by a spinning bar.
//  This function has no parameters.
//  This function has no return value.
//****************************************************************************

static word TaskA(void)
{
	// Local variables
    // ***************
	tStatus usSta;
	word usState, usCt;
	char tcDisplay[50+1];
	tEventList usEve;
	bool bExit;
	int iRet;

    // Send a event to Main task
	// *************************
    usSta = Telium_SignalEvent(usMainTaskNbr, 15);             // Signal to main task that TaskA is initialized
    CHECK(usSta==cOK, lblEnd);

	// Task protected by a mutex using a semaphore (security SHARED)
	// *************************************************************
	while(1)
    {
	    usState=0;
	    usCt=0;
        bExit=FALSE;

        iRet = OSL_Semaphore_Acquire(hSemS2, OSL_TIMEOUT_INFINITE); // Take a semaphore unit
        CHECK(iRet==OSL_SUCCESS, lblEnd);

		do
		{
			Telium_Sprintf(tcDisplay, "TaskA  (Training)        %s", ttcLogo[usState]); // Show the spinning bar
			GoalDspLine(hScreen, 1, tcDisplay, &txSemaphore[1], 0, true);

			Telium_WaitEvents (0x00008000, uiTimeOut, &usEve); // Wait event 15 (mask bit 1..32) from main task
			if (usEve == 0x00008000)
			{
				Telium_ClearEvents (0x00008000);               // The event bit MUST be cleared when using "WaitEvents"
				OSL_Semaphore_Release(hSemS2);
				goto lblEnd;
			}

			usState++;
			if (usState == MAX_STATE)
				usState=0;

			if (usCt == MAX_LOOP)
				bExit=TRUE;
			usCt++;

		} while (!bExit);

		OSL_Semaphore_Release(hSemS2);                         // Release a semaphore unit
		Telium_Ttestall(0, 1);                                 // The smallest wait needed for Telium3
    }

lblEnd:
	return 0;
}

//****************************************************************************
//                          word TaskB (void)
//  This function manages the TaskB which is protected by a mutex using a
//  semaphore. The critical zone is shown by a spinning bar.
//  This function has no parameters.
//  This function has no return value.
//****************************************************************************

static word TaskB(void)
{
	// Local variables
    // ***************
	tStatus usSta;
	word usState, usCt;
	char tcDisplay[50+1];
	tEventList usEve;
	bool bExit;
	int iRet;

    // Send a event to Main task
	// *************************
    usSta = Telium_SignalEvent(usMainTaskNbr, 15);             // Signal to main task that TaskB is initialized
    CHECK(usSta==cOK, lblEnd);

	// Task protected by a mutex using a semaphore (security SHARED)
	// *************************************************************
	while(1)
    {
	    usState=0;
	    usCt=0;
        bExit=FALSE;

        iRet = OSL_Semaphore_Acquire(hSemS2, OSL_TIMEOUT_INFINITE); // Take a semaphore unit
        CHECK(iRet==OSL_SUCCESS, lblEnd);

		do
		{
			Telium_Sprintf(tcDisplay, "TaskB  (Training)        %s", ttcLogo[usState]); // Show spinning bar
			GoalDspLine(hScreen, 2, tcDisplay, &txSemaphore[2], 0, true);

			Telium_WaitEvents (0x00008000, uiTimeOut, &usEve); // Wait event 15 (mask bit 1..32) from main task
			if (usEve == 0x00008000)
			{
				Telium_ClearEvents (0x00008000);               // The event bit MUST be cleared when using "WaitEvents"
				goto lblEnd;
			}

			usState++;
			if (usState == MAX_STATE)
				usState=0;

			if (usCt == MAX_LOOP)
				bExit=TRUE;
			usCt++;

		} while (!bExit);

		OSL_Semaphore_Release(hSemS2);                         // Release a semaphore unit
		Telium_Ttestall(0, 1);                                 // The smallest wait needed for Telium3
    }

lblEnd:
	return 0;
}

//****************************************************************************
//                         void TaskSempahore (void)
//  This function shows how to implement a mutex (inter-process) using a semaphore.
//  The critical zone of each task is shown by a spinning bar and protected by
//  a mutex.
//  This function has no parameters.
//  This function has no return value.                                      
//****************************************************************************

void TaskSemaphore(void)
{
	// Local variables 
    // ***************
	tStatus usSta;
	t_topstack *hTskA=NULL, *hTskB=NULL;
	byte dum;
	tEventList uiEve;
    int iRet;

	// Create a screen
	// ===============
	hScreen = GoalCreateScreen(hGoal, txSemaphore, NUMBER_OF_LINES(txSemaphore), GL_ENCODING_UTF8);
	CHECK(hScreen!=NULL, lblKO);                      // Create screen and clear it

	iRet = GoalClrScreen(hScreen, GL_COLOR_BLACK, KEY_CANCEL, false);
	CHECK(iRet>=0, lblKO);

	if (IsColorDisplay())
	{
		memcpy(ttcLogo, ttzLogoCL, sizeof(ttcLogo));
		uiTimeOut=2;
	}
	else
	{
		memcpy(ttcLogo, ttzLogoBW, sizeof(ttcLogo));
		uiTimeOut=8;
	}

	iRet = GoalDspLine(hScreen, 0, "SEMAPHORE", &txSemaphore[0], 0, false);
	CHECK(iRet>=0, lblKO);
	iRet = GoalDspLine(hScreen, 1, "TaskA  (Training)          |", &txSemaphore[1], 0, false);
	CHECK(iRet>=0, lblKO);                            // TaskA from Training application
	iRet = GoalDspLine(hScreen, 2, "TaskB  (Training)          |", &txSemaphore[2], 0, false);
	CHECK(iRet>=0, lblKO);                            // TaskB from Training application
	iRet = GoalDspLine(hScreen, 3, "TaskC  (User2)             |", &txSemaphore[3], 0, false);
	CHECK(iRet>=0, lblKO);                            // TaskC from User2 application
	iRet = GoalDspLine(hScreen, 4, "TaskD  (User2)             |", &txSemaphore[4], 0, true);
	CHECK(iRet>=0, lblKO);                            // TaskD from User2 application

    // Semaphore management
	// ********************
    usMainTaskNbr=Telium_CurrentTask();               // Get the main task number

	hTskA=Telium_Fork(TaskA, &dum, 0);                // Fork TaskA
	CHECK(hTskA!=NULL, lblKO);
	usSta = Telium_WaitEvents(0x00008000, 0, &uiEve); // Wait event 15 (mask bit 1..32) from TaskA
	CHECK(usSta==cOK, lblKO);
	usSta = Telium_ClearEvents(0x00008000);           // The event bit MUST be cleared when using "WaitEvents"
	CHECK(usSta==cOK, lblKO);

	hTskB=Telium_Fork(TaskB, &dum, 1);                // Fork TaskB
	CHECK(hTskB!=NULL, lblKO);
	usSta = Telium_WaitEvents(0x00008000, 0, &uiEve); // Wait event 15 (mask bit 1..32) from TaskB
	CHECK(usSta==cOK, lblKO);
	usSta = Telium_ClearEvents(0x00008000);           // The event bit MUST be cleared when using "WaitEvents"
	CHECK(usSta==cOK, lblKO);

	OSL_Semaphore_Release(hSemS2);                    // Release a semaphore unit to start the mutex process

	GoalGetKey(hScreen, hGoal, true, GL_TIME_INFINITE, false);  // Wait for key pressed/touched (shortcut)

	goto lblEnd;

	// Errors treatment
    // ****************
lblKO:                                                // None-classified low level error
	GL_Dialog_Message(hGoal, NULL, "Processing Error", GL_ICON_ERROR, GL_BUTTON_VALID, 5*1000);
	goto lblEnd;
lblEnd:
	if(hTskA)
		Telium_SignalEvent(Telium_GiveNoTask(hTskA), 15); // Kill TaskA
	if(hTskB)
		Telium_SignalEvent(Telium_GiveNoTask(hTskB), 15); // Kill TaskB
	Telium_Ttestall(0, 20);                           // Wait a little bit to be sure that all tasks are killed

	if (hScreen)
		GoalDestroyScreen(&hScreen);                  // Destroy screen
}
