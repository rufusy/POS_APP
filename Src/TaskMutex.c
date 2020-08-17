//****************************************************************************
//       INGENICO                                INGEDEV 7                   
//============================================================================
//       FILE  TASKMUTEX.C                        (Copyright INGENICO 2013)
//============================================================================
//  Created :       26-November-2013     Kassovic
//  Last modified : 26-November-2013     Kassovic
//  Module : TRAINING
//
//  Purpose :
//  A Mutex is a technique used in computer science to get exclusive access
//  to shared resources or critical section.
//  A mutex can be used only for inter-thread protection. To use a mutex for
//  inter-processing synchronisation, prefer Semaphore (see TaskSemaphore.c)
//  It is recommended to use :
//  - OS LAYER APIs to manage mutex (inter-thread protection) instead of
//    RTOS APIs.
//  How to create a mutex using 3 tasks. The critical zone of each task is
//  shown by a spinning bar and protected by a mutex.
//                                                                            
//  List of routines in file :  
//      TaskA : Manage the first task which is protected by a mutex.
//      TaskB : Manage the second task which is protected by a mutex.
//      TaskC : Manage the third task which is protected by a mutex.
//      TaskMutex : Mutex demo (inter-thread protection).
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
// Handle created at application start-up (see Entry.c)
// =====================================================
extern T_GL_HGRAPHIC_LIB hGoal; // Handle of the graphics object library

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
// Properties of the Mutex screen (Goal)
// =====================================
static const ST_DSP_LINE txMutex[] =
{
	{ {GL_ALIGN_CENTER, GL_ALIGN_CENTER, GL_COLOR_WHITE, GL_COLOR_BLACK,  0, FALSE, {0, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_WHITE}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}}, // Line0
	  {GL_ALIGN_CENTER, GL_ALIGN_CENTER, FALSE, 0, FALSE, {0, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}} },
	{ {GL_ALIGN_LEFT, GL_ALIGN_CENTER, GL_COLOR_WHITE, GL_COLOR_RED,    100, FALSE, {1, 0, 0, 3}, {1, 1, 1, 1, GL_COLOR_WHITE}, {3, 0, 3, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}}, // Line1
	  {GL_ALIGN_LEFT, GL_ALIGN_CENTER, TRUE,  100, FALSE, {2, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}} },
	{ {GL_ALIGN_LEFT, GL_ALIGN_CENTER, GL_COLOR_WHITE, GL_COLOR_GREEN,  100, FALSE, {1, 0, 0, 3}, {1, 1, 1, 1, GL_COLOR_WHITE}, {3, 0, 3, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}}, // Line2
	  {GL_ALIGN_LEFT, GL_ALIGN_CENTER, TRUE,  100, FALSE, {2, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}} },
	{ {GL_ALIGN_LEFT, GL_ALIGN_CENTER, GL_COLOR_WHITE, GL_COLOR_BLUE,   100, FALSE, {1, 0, 0, 3}, {1, 1, 1, 1, GL_COLOR_WHITE}, {3, 0, 3, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}}, // Line3
	  {GL_ALIGN_LEFT, GL_ALIGN_CENTER, TRUE,  100, FALSE, {2, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}} }
};

static const char ttzLogoCL[MAX_STATE][4+1] = {"  | ", "  / ", " ---", "  \\ "};
static const char ttzLogoBW[MAX_STATE][4+1] = {"  | ", " /  ", "--  ", " \\  "};
static char ttcLogo[MAX_STATE][4+1];
static doubleword uiTimeOut;

static T_GL_HWIDGET hScreen=NULL; // Handle of the screen (Goal)

static word usMainTaskNbr;
static T_OSL_HMUTEX hMutex = NULL; // Handle of the mutex (always security shared)

//****************************************************************************
//                          word TaskA (void)
//  This function manages the TaskA which is protected by a mutex.
//  The critical zone is shown by a spinning bar.
//   - OSL_Mutex_Lock : Takes the mutex if possible or waits for timeout.
//   - OSL_Mutex_Unlock : Releases the mutex to let access to other process.
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

    // Task protected by a mutex (security LOCAL)
 	// ******************************************
	while(1)
    {
	    usState=0;
	    usCt=0;
        bExit=FALSE;

		iRet = OSL_Mutex_Lock(hMutex, OSL_TIMEOUT_INFINITE);   // Take the mutex
		CHECK(iRet==OSL_SUCCESS, lblEnd);

		do
		{                                                      // Show a spinning bar
			Telium_Sprintf(tcDisplay, "TaskA  (Training)        %s", ttcLogo[usState]);
			GoalDspLine(hScreen, 1, tcDisplay, &txMutex[1], 0, true);

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

		OSL_Mutex_Unlock(hMutex);                              // Release the mutex
		Telium_Ttestall(0, 1);                                 // The smallest wait needed for Telium3
    }

lblEnd:
	return 0;
}

//****************************************************************************
//                          word TaskB (void)
//  This function manages the TaskB which is protected by a mutex.
//  The critical zone is shown by a spinning bar.
//   - OSL_Mutex_Lock : Takes the mutex if possible or waits for timeout.
//   - OSL_Mutex_Unlock : Releases the mutex to let access to other process.
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

    // Task protected by a mutex (security LOCAL)
 	// ******************************************
	while(1)
    {
	    usState=0;
	    usCt=0;
        bExit=FALSE;

		iRet = OSL_Mutex_Lock(hMutex, OSL_TIMEOUT_INFINITE);   // Take the mutex
		CHECK(iRet==OSL_SUCCESS, lblEnd);

		do
		{                                                      // Show a spinning bar
			Telium_Sprintf(tcDisplay, "TaskB  (Training)        %s", ttcLogo[usState]);
			GoalDspLine(hScreen, 2, tcDisplay, &txMutex[2], 0, true);

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

		OSL_Mutex_Unlock(hMutex);                              // Release the mutex
		Telium_Ttestall(0, 1);                                 // The smallest wait needed for Telium3
    }

lblEnd:
	return 0;
}

//****************************************************************************
//                          word TaskC (void)
//  This function manages the TaskC which is protected by a mutex.
//  The critical zone is shown by a spinning bar.
//   - OSL_Mutex_Lock : Takes the mutex if possible or waits for timeout.
//   - OSL_Mutex_Unlock : Releases the mutex to let access to other process.
//  This function has no parameters.
//  This function has no return value.
//****************************************************************************

static word TaskC(void)
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
	usSta = Telium_SignalEvent(usMainTaskNbr, 15);             // Signal to main task that TaskC is initialized
	CHECK(usSta==cOK, lblEnd);

    // Task protected by a mutex (security LOCAL)
	// ******************************************
	while(1)
    {
	    usState=0;
	    usCt=0;
        bExit=FALSE;

		iRet = OSL_Mutex_Lock(hMutex, OSL_TIMEOUT_INFINITE);   // Take the mutex
		CHECK(iRet==OSL_SUCCESS, lblEnd);

		do
		{                                                      // Show a spinning bar
			Telium_Sprintf(tcDisplay, "TaskC  (Training)        %s", ttcLogo[usState]);
			GoalDspLine(hScreen, 3, tcDisplay, &txMutex[3], 0, true);

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

		OSL_Mutex_Unlock(hMutex);                              // Release the mutex
		Telium_Ttestall(0, 1);                                 // The smallest wait needed for Telium3
    }

lblEnd:
	return 0;
}

//****************************************************************************
//                         void TaskMutex (void)
//  This function shows how to implement a mutex (inter-thread).
//  The critical zone of each task is shown by a spinning bar and protected by
//  a mutex.
//  This function has no parameters.
//  This function has no return value.                                      
//****************************************************************************

void TaskMutex(void)
{
	// Local variables 
    // ***************
	tStatus usSta;
	t_topstack *hTskA=NULL, *hTskB=NULL, *hTskC=NULL;
	byte dum;
	tEventList usEve;
    int iRet;

	// Create a screen
	// ===============
	hScreen = GoalCreateScreen(hGoal, txMutex, NUMBER_OF_LINES(txMutex), GL_ENCODING_UTF8);
	CHECK(hScreen!=NULL, lblKO);                          // Create screen and clear it
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

	iRet = GoalDspLine(hScreen, 0, "MUTEX", &txMutex[0], 0, false);
	CHECK(iRet>=0, lblKO);
	iRet = GoalDspLine(hScreen, 1, "TaskA  (Training)          |", &txMutex[1], 0, false);
	CHECK(iRet>=0, lblKO);
	iRet = GoalDspLine(hScreen, 2, "TaskB  (Training)          |", &txMutex[2], 0, false);
	CHECK(iRet>=0, lblKO);
	iRet = GoalDspLine(hScreen, 3, "TaskC  (Training)          |", &txMutex[3], 0, true);
	CHECK(iRet>=0, lblKO);

    // Mutex management
	// ****************
	hMutex = OSL_Mutex_Create (0, OSL_SECURITY_LOCAL);    // Create a mutex object
	CHECK(hMutex!=NULL, lblKO);

    usMainTaskNbr=Telium_CurrentTask();                   // Get the Main Task number

	hTskA=Telium_Fork(TaskA, &dum, 0);                    // Fork TaskA
	CHECK(hTskA!=NULL, lblKO);
	usSta = Telium_WaitEvents(0x00008000, 0, &usEve);     // Wait event 15 (mask bit 1..32) from TaskA
	CHECK(usSta==cOK, lblKO);
	usSta = Telium_ClearEvents(0x00008000);               // The event bit MUST be cleared when using "WaitEvents"
	CHECK(usSta==cOK, lblKO);

	hTskB=Telium_Fork(TaskB, &dum, 1);                    // Fork TaskB
	CHECK(hTskB!=NULL, lblKO);
	usSta = Telium_WaitEvents(0x00008000, 0, &usEve);     // Wait event 15 (mask bit 1..32) from TaskB
	CHECK(usSta==cOK, lblKO);
	usSta = Telium_ClearEvents(0x00008000);               // The event bit MUST be cleared when using "WaitEvents"
	CHECK(usSta==cOK, lblKO);

	hTskC=Telium_Fork(TaskC, &dum, 2);                    // Fork TaskC
	CHECK(hTskC!=NULL, lblKO);
	usSta = Telium_WaitEvents(0x00008000, 0, &usEve);     // Wait event 15 (mask bit 1..32) from TaskC
	CHECK(usSta==cOK, lblKO);
	usSta = Telium_ClearEvents(0x00008000);               // The event bit MUST be cleared when using "WaitEvents"
	CHECK(usSta==cOK, lblKO);

	GoalGetKey(hScreen, hGoal, true, GL_TIME_INFINITE, true);  // Wait for key pressed/touched (shortcut)

	goto lblEnd;

	// Errors treatment
    // ****************
lblKO:                                                    // None-classified low level error
	GL_Dialog_Message(hGoal, NULL, "Processing Error", GL_ICON_ERROR, GL_BUTTON_VALID, 5*1000);
	goto lblEnd;
lblEnd:
	if(hTskA)
		Telium_SignalEvent(Telium_GiveNoTask(hTskA), 15); // Kill TaskA
	if(hTskB)
		Telium_SignalEvent(Telium_GiveNoTask(hTskB), 15); // Kill TaskB
	if(hTskB)
		Telium_SignalEvent(Telium_GiveNoTask(hTskC), 15); // Kill TaskC
	Telium_Ttestall(0, 5);                                // Wait a little bit to be sure that all tasks are killed

	if (hMutex)
		OSL_Mutex_Destroy(hMutex);                        // Destroy the mutex object

	if (hScreen)
		GoalDestroyScreen(&hScreen);                      // Destroy screen
}
