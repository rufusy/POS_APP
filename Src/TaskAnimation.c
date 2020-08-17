//****************************************************************************
//       INGENICO                                INGEDEV 7
//============================================================================
//       FILE  TASKANIMATION.C                    (Copyright INGENICO 2012)
//============================================================================
//  Created :       13-July-2012         Kassovic
//  Last modified : 30-November-2013     Kassovic
//  Module : TRAINING                                                        
//                                                                          
//  Purpose :                                                               
//                       *** Tasks management ***
//  The Real Time Operating System provides a whole set of services allowing
//  the applications to manage tasks, events, semaphores, message queues and
//  shared memory.
//  It is recommended to use:
//  - RTOS APIs to manage tasks and events (not supported by OS LAYER library).
//  - OS LAYER APIs to manage semaphores, message queues and shared memory
//    instead of RTOS APIs.
//  How to implement task, event and semaphore creation and the way to send
//  messages between 2 tasks.
//                                                                            
//  List of routines in file :  
//      Play : Animation used by Main and Second Task.
//      SecondTask : Second Task forks by Main Task.
//      TaskAnimation : Animated Tasks Demo.
//                            
//  File history :
//  071312-BK : File created
//  113013-BK : Real Time Operating System using OS LAYER
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
extern T_GL_HGRAPHIC_LIB hGoal;      // Handle of the graphics object library

extern T_OSL_HSEMAPHORE hSemS;       // Handle of the semaphore (security SHARED)
extern T_OSL_HMESSAGE_QUEUE hQueueS; // Handle of the queue (security SHARED)
extern T_OSL_HSHARED_MEM hMemS;      // Handle of the shared memory (security SHARED)

//****************************************************************************
//      PRIVATE CONSTANTS                                                   
//****************************************************************************
#define DSPW  16

//****************************************************************************
//      PRIVATE TYPES                                                       
//****************************************************************************
#define SIZE_BUF 50 // Size max buffer reception for Queue and Shared memory

//****************************************************************************
//      PRIVATE DATA                                                        
//****************************************************************************
// Properties of the Task screen (Goal)
// ====================================
static const ST_DSP_LINE txTask[] =
{
	{ {GL_ALIGN_LEFT, GL_ALIGN_CENTER, GL_COLOR_WHITE, GL_COLOR_BLACK, 100, FALSE, {1, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_WHITE}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}}, // Line0
	  {GL_ALIGN_LEFT, GL_ALIGN_CENTER, FALSE, 100, FALSE, {2, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}} },
	{ {GL_ALIGN_LEFT, GL_ALIGN_CENTER, GL_COLOR_WHITE, GL_COLOR_RED,   100, FALSE, {1, 3, 0, 3}, {1, 1, 1, 1, GL_COLOR_WHITE}, {3, 0, 3, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}}, // Line1
	  {GL_ALIGN_LEFT, GL_ALIGN_CENTER, TRUE,  100, FALSE, {2, 2, 0, 2}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}} },
	{ {GL_ALIGN_LEFT, GL_ALIGN_CENTER, GL_COLOR_WHITE, GL_COLOR_BLACK, 100, FALSE, {1, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_WHITE}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}}, // Line2
	  {GL_ALIGN_LEFT, GL_ALIGN_CENTER, FALSE, 100, FALSE, {2, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}} },
	{ {GL_ALIGN_LEFT, GL_ALIGN_CENTER, GL_COLOR_WHITE, GL_COLOR_BLUE,  100, FALSE, {1, 3, 0, 3}, {1, 1, 1, 1, GL_COLOR_WHITE}, {3, 0, 3, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}}, // Line3
	  {GL_ALIGN_LEFT, GL_ALIGN_CENTER, TRUE,  100, FALSE, {2, 2, 0, 2}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}} }
};

static T_GL_HWIDGET hScreen=NULL; // Handle of the screen (Goal)

// Properties of the default printer (Goal)
// ========================================
static const ST_PRN_LINE xPrinter =
{
	GL_ALIGN_LEFT, GL_ALIGN_CENTER, FALSE, 100, {0, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {1, 0, 1, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_LARGE}
};

// Task management
// ===============
static char tcBufM[DSPW+1];
static char tcBufS[DSPW+1];
static word usMainTaskNbr;                // Task number of the main task

// Semaphore, Queue, Shared memory management (Security local context)
// ===================================================================
static T_OSL_HMESSAGE_QUEUE hQueueL=NULL; // Handle of the queue (security LOCAL)
static T_OSL_HSHARED_MEM hMemL=NULL;      // Handle of the shared memory (security LOCAL)
static T_OSL_HSEMAPHORE hSemL=NULL;       // Handle of the semaphore (security LOCAL)
static char tcRcv1MT[SIZE_BUF];           // Queue reception buffer (Main Task -> Second Task)
static char tcRcv2MT[SIZE_BUF];           // Shared memory reception buffer (Main Task -> Second Task)
static char tcRcv1T2[SIZE_BUF];           // Queue reception buffer (Second Task -> Main Task)
static char tcRcv2T2[SIZE_BUF];           // Shared memory reception buffer (Second Task -> Main Task)

// Semaphore, Queue, Shared memory management (Security shared context)
// ====================================================================
// Handles defined and objects created in after_reset() => See Entry.c
static char tcRcv1T3[SIZE_BUF];           // Queue reception buffer (Third Task[user2] -> Main Task[Training])
static char tcRcv2T3[SIZE_BUF];           // Shared memory reception buffer (Third Task[user2] -> Main Task[Training])

//****************************************************************************
//                          void Play (char cBeg)
//  This function animates main and second tasks.
//  This function has  parameters.
//       cBeg (I-) : Start character for animation
//  This function has no return value.                                      
//****************************************************************************

static void Play(char cBeg)
{
	// Local variables 
    // ***************
    char tcBuf[DSPW+1];
    word usIdx;

	// Play tasks under animation
	// **************************
    memset(tcBuf, 0, sizeof(tcBuf));
    for (usIdx=0; usIdx<DSPW*10; usIdx++)
	{
    	// Prepare line1 and line3 to display
    	// ==================================
        tcBuf[usIdx%DSPW] = usIdx/DSPW+cBeg;
        
		if((cBeg=='A') || (cBeg=='0'))
		{
			if (cBeg=='A')
				strcpy(tcBufM, tcBuf); // First line Main Task
			else
				strcpy(tcBufS, tcBuf); // Third line Second Task
			GoalDspLine(hScreen, 1, tcBufM, &txTask[1], 0, false);
			GoalDspLine(hScreen, 3, tcBufS, &txTask[3], 0, false);
		}
		else 
			strcpy(tcBufS, tcBuf);     // Third line Second Task

		// Refresh screen
		// ==============
		GoalRefreshScreen(hScreen);    // Refresh screen
		if (IsColorDisplay())
			Telium_Ttestall(0, 1);
		else
			Telium_Ttestall(0, 4);
	}
}

//****************************************************************************
//                          word SecondTask (void)                            
//  This function manages the Second Task to:
//  1) Sends the event 15 to main task to start animation.
//  2) Waits and sends a message to main task using a queue (security LOCAL).
//  3) Gets and puts a message to main task using a shared memory (security LOCAL).
//  4) Releases a semaphore unit in main task for synchronization (security LOCAL).
//   - Telium_SignalEvent() : Signal an event to a task.
//   - OSL_MessageQueue_Receive() : Wait a message from a queue.
//   - OSL_MessageQueue_Send() : Send a message to a queue.
//   - OSL_SharedMem_GetPointer() : Pointer to get/put a message to a shared memory.
//   - OSL_Semaphore_Release() : Release a semaphore unit.
//  This function has no parameters.    
//  This function has no return value.                                      
//****************************************************************************

static word SecondTask(void) 
{
	// Local variables 
    // ***************
	tStatus usSta;
	void *pvMemL;
	int iRet;

    // Task animation, synchronization and messages exchanged
	// ******************************************************
	
	// Signal an event to Main Task
	// ============================
	usSta=Telium_SignalEvent(usMainTaskNbr, 15);  // Send event 15 (0..31) to Main Task before starting animation
	CHECK(usSta==cOK, lblKO);
	
    Play('a');                                    // Second Task under animation

	// Wait a message from Main Task using a queue
	// ===========================================
    memset(tcRcv1MT, 0, sizeof(tcRcv1MT));
    iRet = OSL_MessageQueue_Receive(hQueueL, tcRcv1MT, sizeof(tcRcv1MT)-1, OSL_TIMEOUT_INFINITE);
    CHECK(iRet==OSL_SUCCESS, lblKO);              // Receive Msg1 from Main Task

	// Get a message from Main Task using a shared memory
	// ==================================================
    memset(tcRcv2MT, 0, sizeof(tcRcv2MT));
	pvMemL = OSL_SharedMem_GetPointer(hMemL);     // Retrieve pointer to access shared memory
	CHECK(pvMemL!=NULL, lblKO);
	memcpy(tcRcv2MT, pvMemL, sizeof(tcRcv2MT)-1); // Get Msg2 from Main Task

	// Send a message to Main Task using a queue
	// =========================================
	const char tzMsg1MT[] = "Hi, Main Task";      // Send Msg1 to Main Task
    iRet = OSL_MessageQueue_Send(hQueueL, tzMsg1MT, sizeof(tzMsg1MT), OSL_TIMEOUT_INFINITE);
    CHECK(iRet==OSL_SUCCESS, lblKO);

    // Put a message to Main Task using a shared memory
    // ================================================
    const char tzMsg2MT[] = "Yes, Second Task is ready";
    pvMemL = OSL_SharedMem_GetPointer(hMemL);     // Retrieve pointer to access shared memory
    CHECK(pvMemL!=NULL, lblKO);
    memcpy(pvMemL, tzMsg2MT, sizeof(tzMsg2MT));   // Put Msg2 to Main Task

	Play('0');                                    // Second Task under animation

	// Errors treatment 
    // ****************
lblKO:
	GoalDspLine(hScreen, 2, "SecondTask Killed !", &txTask[2], 0, true);
	memset(tcBufS, 0, sizeof(tcBufS));

	OSL_Semaphore_Release(hSemL);                 // Release a semaphore unit in Main Task
	return 0;                                     // Kill the Second Task
}

//****************************************************************************
//                          void TaskAnimation (void)
//  This function manages the Main Task to:
//  1) Creates a queue, a shared memory and a semaphore (security LOCAL).
//  2) Forks a second task.
//  3) Waits an event from Second Task to start animation.
//  4) Sends and waits a message from second task using a queue (security LOCAL).
//  5) Puts and gets a message from second task using a shared memory (security LOCAL).
//  6) Sends and waits a message from third task[User2] using a queue (security SAHRED).
//  7) Puts and gets a message from third task[User2] using a shared memory (security SHARED).
//  8) Releases a semaphore unit into third task[User2] for synchronization (security SHARED).
//   - OSL_MessageQueue_Create() : Create a message queue.
//   - OSL_SharedMem_Create() : Create a shared memory.
//   - OSL_Semaphore_Create() : Create a semaphore.
//   - Telium_CurrentTask : Get the ID of the current task.
//   - Telium_Fork() : Fork a task.
//   - Telium_WaitEvents() : Wait for a list of events.
//   - Telium_ClearEvents() : Clear the bit event.
//   - OSL_MessageQueue_Send() : Send a message to queue.
//   - OSL_SharedMem_GetPointer() : Put a message to a shared memory.
//   - OSL_Semaphore_Release() : Release a semaphore unit.
//   - OSL_Semaphore_Acquire() : Acquire a semaphore unit.
//   - OSL_MessageQueue_Receive() : Wait a message from a queue.
//   - OSL_SharedMem_GetPointer() : Get a message from a shared memory.
//   - OSL_Semaphore_Destroy() : Destroy a semaphore.
//   - OSL_SharedMem_Destroy() : Destroy a shared memory.
//   - OSL_MessageQueue_Destroy() : Destroy a message queue.
//  This function has no parameters.
//  This function has no return value.                                      
//****************************************************************************

void TaskAnimation(void){
	// Local variables 
    // ***************
	T_GL_HWIDGET hDocument=NULL;
	ST_PRN_LINE xLine;
	t_topstack *hTsk=NULL; // Handle of the task
	byte dum1;
	int dum2=0;
	tStatus usSta;
	tEventList uiEve;
	void *pvMemL, *pvMemS;
	char tcBuffer[100+1];
	byte p; // Printer line index
    int iRet;

	// Create a screen
	// ===============
	hScreen = GoalCreateScreen(hGoal, txTask, NUMBER_OF_LINES(txTask), GL_ENCODING_UTF8);
	CHECK(hScreen!=NULL, lblKO);                      // Create screen and clear it
	iRet = GoalClrScreen(hScreen, GL_COLOR_BLACK, 0, false);
	CHECK(iRet>=0, lblKO);

	iRet = GoalDspLine(hScreen, 0, "MainTask Running... ", &txTask[0], 0, false);
	CHECK(iRet>=0, lblKO);

	// Task animation, semaphore, queue, and shared memory management
	// **************************************************************
	memset(tcBufM, 0, sizeof(tcBufM));
	memset(tcBufS, 0, sizeof(tcBufS));

	hQueueL = OSL_MessageQueue_Create("QUEUE_L", OSL_OPEN_CREATE, OSL_SECURITY_LOCAL); // Create a message queue object local to the Training application only
	CHECK(hQueueL!=NULL, lblKO);
	hMemL = OSL_SharedMem_Create("MEM_L", 1024, OSL_OPEN_CREATE, OSL_SECURITY_LOCAL);  // Create a shared memory object local to the Training application only
	CHECK(hMemL!=NULL, lblKO);
	hSemL = OSL_Semaphore_Create("SEM_L", 0, OSL_OPEN_CREATE, OSL_SECURITY_LOCAL);     // Create a semaphore object local to the Training application only
	CHECK(hSemL!=NULL, lblKO);

	usMainTaskNbr=Telium_CurrentTask();               // Get the Main Task number

	// Second Task creation
	// ====================
	iRet = GoalDspLine(hScreen, 2, "SecondTask Running...", &txTask[2], 0, false);
	CHECK(iRet>=0, lblKO);

	hTsk=Telium_Fork(SecondTask, &dum1, dum2);        // Fork Second Task
	CHECK(hTsk!=NULL, lblKO);
	
	// Wait an event from Second Task
	// ==============================
    usSta=Telium_WaitEvents (0x00008000, 0, &uiEve);  // Wait event 15 (mask bit 1..32) from Second Task before starting animation
    CHECK(usSta==cOK, lblKO);
    usSta=Telium_ClearEvents (0x00008000);            // The event bit MUST be cleared when using "WaitEvents"
    CHECK(usSta==cOK, lblKO);
    // ttestall (0x00008000, 0);                      // Same effect as "WaitEvents", the event bit is cleared by "ttestall"

    Play('A');                                        // Main Task under animation

    // Sending messages
    // ================

	// Send a message to Third Task (User2) using a queue
	// --------------------------------------------------
	const char tzMsg1T3[] = "Hello, Third Task";
	iRet = OSL_MessageQueue_Send(hQueueS, tzMsg1T3, sizeof(tzMsg1T3), OSL_TIMEOUT_INFINITE);
	CHECK(iRet==OSL_SUCCESS, lblKO);                  // Send Msg1 to Third Task (User2 application)

	// Put a message to Third Task (User2) using a shared memory
	// ---------------------------------------------------------
	const char tzMsg2T3[] = "Are you ready?";
	pvMemS = OSL_SharedMem_GetPointer(hMemS);         // Retrieve pointer to access shared memory
	CHECK(pvMemS!=NULL, lblKO);
	memcpy(pvMemS, tzMsg2T3, sizeof(tzMsg2T3));       // Put Msg2 to Third Task (User2 application)

	OSL_Semaphore_Release(hSemS);                     // Release a semaphore unit to Third Task (User2 application)

    // Send a message to Second Task using a queue
	// -------------------------------------------
    const char tzMsg1T2[] = "Hello, Second Task";
    iRet = OSL_MessageQueue_Send(hQueueL, tzMsg1T2, sizeof(tzMsg1T2), OSL_TIMEOUT_INFINITE);
    CHECK(iRet==OSL_SUCCESS, lblKO);                  // Send Msg1 to Second Task

    // Put a message to Second Task using a shared memory
    // --------------------------------------------------
    const char tzMsg2T2[] = "Are you ready?";
    pvMemL = OSL_SharedMem_GetPointer(hMemL);         // Retrieve pointer to access shared memory
    CHECK(pvMemL!=NULL, lblKO);
    memcpy(pvMemL, tzMsg2T2, sizeof(tzMsg2T2));       // Put Msg2 to Second Task

	iRet = GoalDspLine(hScreen, 0, "MainTask Waits !", &txTask[0], 0, true);
	CHECK(iRet>=0, lblKO);

	// Main Task is in waiting state until the Second Task calls Semaphore release
	// ===========================================================================
    buzzer(10);
    iRet = OSL_Semaphore_Acquire(hSemL, OSL_TIMEOUT_INFINITE); // Acquire a semaphore unit and wait
    CHECK(iRet==OSL_SUCCESS, lblKO);

    buzzer(10);
	iRet = GoalDspLine(hScreen, 0, "MainTask Restarts...", &txTask[0], 0, false);
	CHECK(iRet>=0, lblKO);

    // Main Task restarts, Second Task is killed
	// =========================================

	// Receiving messages
	// ==================

	// Wait a message from Second Task using a queue
	// ---------------------------------------------
    memset(tcRcv1T2, 0, sizeof(tcRcv1T2));
    iRet = OSL_MessageQueue_Receive(hQueueL, tcRcv1T2, sizeof(tcRcv1T2)-1, OSL_TIMEOUT_INFINITE);
    CHECK(iRet==OSL_SUCCESS, lblKO);                  // Receive Msg1 from Second Task

	// Get a message from a Second Task using shared memory
	// ----------------------------------------------------
    memset(tcRcv2T2, 0, sizeof(tcRcv2T2));
	pvMemL = OSL_SharedMem_GetPointer(hMemL);         // Retrieve pointer to access shared memory
	CHECK(pvMemL!=NULL, lblKO);
	memcpy(tcRcv2T2, pvMemL, sizeof(tcRcv2T2)-1);     // Get Msg2 from Second Task

	if (AppliExist(0x57C1))                           // User2 application loaded?
	{
		// Wait a message from Third Task (User2) using a queue
		// ----------------------------------------------------
		memset(tcRcv1T3, 0, sizeof(tcRcv1T3));
		iRet = OSL_MessageQueue_Receive(hQueueS, tcRcv1T3, sizeof(tcRcv1T3)-1, OSL_TIMEOUT_INFINITE);
		CHECK(iRet==OSL_SUCCESS, lblKO);              // Receive Msg1 from Third Task (User2 application)

		// Get a message from a Third Task (User2) using shared memory
		// -----------------------------------------------------------
    	memset(tcRcv2T3, 0, sizeof(tcRcv2T3));
    	pvMemS = OSL_SharedMem_GetPointer(hMemS);     // Retrieve pointer to access shared memory
    	CHECK(pvMemS!=NULL, lblKO);
    	memcpy(tcRcv2T3, pvMemS, sizeof(tcRcv2T3)-1); // Get Msg2 from Third Task (User2 application)
    }

    Play('A');                                        // Main Task under animation

	buzzer(10);
	iRet = GoalDspLine(hScreen, 0, "MainTask Ended", &txTask[0], 1*1000, true);
	CHECK(iRet>=0, lblKO);

	// Print messages exchanged
	// ========================
	p=0;
	hDocument = GoalCreateDocument(hGoal, GL_ENCODING_UTF8);  // Create document
	CHECK(hDocument!=NULL, lblKO);

	xLine = xPrinter;                                         // Build document (Demo, Msg Second Task, Msg Main Task)
	xLine.eTextAlign = GL_ALIGN_CENTER;
	xLine.bReverse = TRUE;
	xLine.xMargin.usBottom = PIXEL_BOTTOM;
	xLine.xFont.eScale = GL_SCALE_XXLARGE;
	iRet = GoalPrnLine(hDocument, p++, "Task Demo", &xLine);
	CHECK(iRet>=0, lblKO);

	xLine = xPrinter;
	iRet = GoalPrnLine(hDocument, p++, "***** Main Task to Second Task *****", &xLine);
	CHECK(iRet>=0, lblKO);
	Telium_Sprintf(tcBuffer, "- Msg received from queue :\n%s", tcRcv1MT);
	iRet = GoalPrnLine(hDocument, p++, tcBuffer, &xLine);     // Message received from Main Task (Queue)
	CHECK(iRet>=0, lblKO);
	Telium_Sprintf(tcBuffer, "- Msg received from shared memory :\n%s\n\n", tcRcv2MT);
	iRet = GoalPrnLine(hDocument, p++, tcBuffer, &xLine);     // Message received from Main Task (Shared Memory)
	CHECK(iRet>=0, lblKO);
	iRet = GoalPrnLine(hDocument, p++, "***** Second Task to Main Task *****", &xLine);
	CHECK(iRet>=0, lblKO);
    Telium_Sprintf(tcBuffer, "- Msg received from queue :\n%s", tcRcv1T2);
	iRet = GoalPrnLine(hDocument, p++, tcBuffer, &xLine);     // Message received from Second Task (Queue)
	CHECK(iRet>=0, lblKO);
	Telium_Sprintf(tcBuffer, "- Msg received from shared memory :\n%s", tcRcv2T2);
	iRet = GoalPrnLine(hDocument, p++, tcBuffer, &xLine);     // Message received from Second Task (Shared Memory)
	CHECK(iRet>=0, lblKO);
	if (AppliExist(0x57C1))                                   // User2 application loaded?
	{
		iRet = GoalPrnLine(hDocument, p++, "\n------------------------------------------------", &xLine);
		CHECK(iRet>=0, lblKO);
		iRet = GoalPrnLine(hDocument, p++, "\n***** Main Task to Third Task *****", &xLine);
		CHECK(iRet>=0, lblKO);
		Telium_Sprintf(tcBuffer, "- Msg received from queue :\n%s", tzMsg1T3);
		iRet = GoalPrnLine(hDocument, p++, tcBuffer, &xLine); // Message received from Main Task (Queue)
		CHECK(iRet>=0, lblKO);
		Telium_Sprintf(tcBuffer, "- Msg received from shared memory :\n%s\n\n", tzMsg2T3);
		iRet = GoalPrnLine(hDocument, p++, tcBuffer, &xLine); // Message received from Main Task (Shared Memory)
		CHECK(iRet>=0, lblKO);
		iRet = GoalPrnLine(hDocument, p++, "***** Third Task to Main Task *****", &xLine);
		CHECK(iRet>=0, lblKO);
	    Telium_Sprintf(tcBuffer, "- Msg received from queue :\n%s", tcRcv1T3);
		iRet = GoalPrnLine(hDocument, p++, tcBuffer, &xLine); // Message received from Third Task (Queue)
		CHECK(iRet>=0, lblKO);
		Telium_Sprintf(tcBuffer, "- Msg received from shared memory :\n%s", tcRcv2T3);
		iRet = GoalPrnLine(hDocument, p++, tcBuffer, &xLine); // Message received from Third Task (Shared Memory)
		CHECK(iRet>=0, lblKO);
	}
	iRet = GoalPrnLine(hDocument, p++, "\n\n\n\n", &xLine);   // Line feed
	CHECK(iRet>=0, lblKO);

	iRet = GoalPrnDocument(hDocument);
	CHECK(iRet>=0, lblKO);

	// Display messages exchanged
	// ==========================
	iRet = GoalDspLine(hScreen, 0, "MainTask to SecondTask", &txTask[0], 0, false);
	CHECK(iRet>=0, lblKO);
	Telium_Sprintf(tcBuffer, "%s", tcRcv1MT);                // Message received from Main Task (Queue)
	iRet = GoalDspLine(hScreen, 1, tcBuffer, &txTask[1], 0, false);
	CHECK(iRet>=0, lblKO);
	iRet = GoalDspLine(hScreen, 2, "SecondTask to MainTask", &txTask[2], 0, false);
	CHECK(iRet>=0, lblKO);
	Telium_Sprintf(tcBuffer, "%s", tcRcv1T2);                // Message received from Second Task (Queue)
	iRet = GoalDspLine(hScreen, 3, tcBuffer, &txTask[3], 10*1000, true);
	CHECK(iRet>=0, lblKO);

	goto lblEnd;

	// Errors treatment
    // ****************
lblKO:                                                // None-classified low level error
	GL_Dialog_Message(hGoal, NULL, "Processing Error", GL_ICON_ERROR, GL_BUTTON_VALID, 5*1000);
	goto lblEnd;
lblEnd:
	if (hSemL)
		OSL_Semaphore_Destroy(hSemL);                 // Destroy the semaphore object
	if (hMemL)
		OSL_SharedMem_Destroy(hMemL);                 // Destroy the shared memory object
	if (hQueueL)
		OSL_MessageQueue_Destroy(hQueueL);            // Destroy the message queue object

	if (hScreen)
		GoalDestroyScreen(&hScreen);                  // Destroy screen
    if (hDocument)
    	GoalDestroyDocument(&hDocument);              // Destroy document
}

