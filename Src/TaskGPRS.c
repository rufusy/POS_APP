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

//****************************************************************************
//      PRIVATE CONSTANTS
//****************************************************************************
#define DSPW  16

//****************************************************************************
//      PRIVATE TYPES
//****************************************************************************
#define SIZE_BUF 50 // Size max buffer reception for Queue and Shared memory


// Task management
// ===============
static char tcBufM[DSPW+1];
static char tcBufS[DSPW+1];
static word usMainTaskNbr;                // Task number of the main task

// Semaphore, Queue, Shared memory management (Security local context)
// ===================================================================
static T_OSL_HSEMAPHORE hSemL=NULL;       // Handle of the semaphore (security LOCAL)
static T_OSL_HSEMAPHORE hSemL_Sim=NULL;       // Handle of the semaphore (security LOCAL)

static word DualSimTask(void) {
	// Local variables
	// ***************
	tStatus usSta;

	// Task animation, synchronization and messages exchanged
	// ******************************************************

	// Signal an event to Main Task
	// ============================
	usSta=Telium_SignalEvent(usMainTaskNbr, 16);  // Send event 15 (0..31) to Main Task before starting animation
	CHECK(usSta==cOK, lblKO);

	AutoSwitchSimSlots();                               // Second Task under animation

	// Errors treatment
	// ****************
	lblKO:
	memset(tcBufS, 0, sizeof(tcBufS));

	return 0;                                     // Kill the Second Task
}

static word SecondTask(void) {
	// Local variables
	// ***************
	tStatus usSta;

	// Task animation, synchronization and messages exchanged
	// ******************************************************

	// Signal an event to Main Task
	// ============================
	usSta=Telium_SignalEvent(usMainTaskNbr, 15);  // Send event 15 (0..31) to Main Task before starting animation
	CHECK(usSta==cOK, lblKO);

	ComGPRS_Prepare();                               // Second Task under animation

	// Errors treatment
	// ****************
	lblKO:
	memset(tcBufS, 0, sizeof(tcBufS));

	return 0;                                     // Kill the Second Task
}

//static word ThirdTask(void) {
//	// Local variables
//	// ***************
//	tStatus usSta;
//
//	// Task animation, synchronization and messages exchanged
//	// ******************************************************
//
//	// Signal an event to Main Task
//	// ============================
//	usSta=Telium_SignalEvent(usMainTaskNbr, 15);  // Send event 15 (0..31) to Main Task before starting animation
//	CHECK(usSta==cOK, lblKO);
//
//	ComSSL_Prepare();                               // Second Task under animation
//
//	// Errors treatment
//	// ****************
//	lblKO:
//	memset(tcBufS, 0, sizeof(tcBufS));
//
//	return 0;                                     // Kill the Second Task
//}

static word FourthTask(void) {
	// Local variables
	// ***************
	tStatus usSta;

	// Task animation, synchronization and messages exchanged
	// ******************************************************

	// Signal an event to Main Task
	// ============================
	usSta=Telium_SignalEvent(usMainTaskNbr, 15);  // Send event 15 (0..31) to Main Task before starting animation
	CHECK(usSta==cOK, lblKO);

	comWifi_Prepare();                               // Second Task under animation

	// Errors treatment
	// ****************
	lblKO:
	memset(tcBufS, 0, sizeof(tcBufS));

	return 0;                                     // Kill the Second Task
}



void TaskInitiateGPRS(void){
	// Local variables
	// ***************
	t_topstack *hTsk=NULL; // Handle of the task
	byte dum1;
	int dum2=0;
	byte CommRoute;
	byte TLS_Enabled;

	// Task animation, semaphore, queue, and shared memory management
	// **************************************************************
	memset(tcBufM, 0, sizeof(tcBufM));
	memset(tcBufS, 0, sizeof(tcBufS));

	///Get the communication route
	mapGetByte(appCommRoute,CommRoute);

	///Get the Secure route
	mapGetByte(appCommSSL,TLS_Enabled);


	switch (TLS_Enabled) {
	case 'Y':
		///----------------------------------
		switch (CommRoute) {
		case 'T':
			CommRoute = 'S';
			break;
		default:
			break;
		}
		///----------------------------------
		break;
		default:
			break;
	}


	switch (CommRoute) {

	case 'G':
		//////// ---------------------------------------------------------------

		hSemL = OSL_Semaphore_Create("SEM_GPRS", 0, OSL_OPEN_CREATE, OSL_SECURITY_LOCAL);     // Create a semaphore object local to the Training application only
		CHECK(hSemL!=NULL, lblKO);

		usMainTaskNbr=Telium_CurrentTask();               // Get the Main Task number

		// Second Task creation
		// ====================

		hTsk=Telium_Fork(SecondTask, &dum1, dum2);        // Fork Second Task
		CHECK(hTsk!=NULL, lblKO);

		goto lblEnd;

		//////// ---------------------------------------------------------------
		break;

//	case 'S':
//		//////// ---------------------------------------------------------------
//
//		hSemL = OSL_Semaphore_Create("SEM_L", 0, OSL_OPEN_CREATE, OSL_SECURITY_LOCAL);     // Create a semaphore object local to the Training application only
//		CHECK(hSemL!=NULL, lblKO);
//
//		usMainTaskNbr=Telium_CurrentTask();               // Get the Main Task number
//
//		// Second Task creation
//		// ====================
//
//		hTsk=Telium_Fork(ThirdTask, &dum1, dum2);        // Fork Second Task
//		CHECK(hTsk!=NULL, lblKO);
//
//		goto lblEnd;
//
//		//////// ---------------------------------------------------------------
//		break;

	case 'W':
		//////// ---------------------------------------------------------------

		hSemL = OSL_Semaphore_Create("SEM_WIFI", 0, OSL_OPEN_CREATE, OSL_SECURITY_LOCAL);     // Create a semaphore object local to the Training application only
		CHECK(hSemL!=NULL, lblKO);

		usMainTaskNbr=Telium_CurrentTask();               // Get the Main Task number

		// Second Task creation
		// ====================

		hTsk=Telium_Fork(FourthTask, &dum1, dum2);        // Fork Second Task
		CHECK(hTsk!=NULL, lblKO);

		goto lblEnd;

		//////// ---------------------------------------------------------------
		break;
	default:
		return;
		break;
	}
	// Errors treatment
	// ****************
	lblKO:                                                // None-classified low level error
	GL_Dialog_Message(hGoal, NULL, "NOTE! GPRS might be offline", GL_ICON_NONE, GL_BUTTON_VALID, 5*1000);
	goto lblEnd;
	lblEnd:
	if (hSemL)
		OSL_Semaphore_Destroy(hSemL);                 // Destroy the semaphore object
	hSemL = NULL;
}

void TaskSimSlot(void){
	// Local variables
	// ***************
	t_topstack *hTsk=NULL; // Handle of the task
	byte dum1;
	int dum2=0;
	byte CommRoute = 0;

	// Task animation, semaphore, queue, and shared memory management
	// **************************************************************
	memset(tcBufM, 0, sizeof(tcBufM));
	memset(tcBufS, 0, sizeof(tcBufS));


	///Get the communication route
	mapGetByte(appCommRoute, CommRoute);

	switch (CommRoute) {

	case 'G':
		//////// ---------------------------------------------------------------

		hSemL_Sim = OSL_Semaphore_Create("SEM_SIM", 1, OSL_OPEN_CREATE, OSL_SECURITY_LOCAL);     // Create a semaphore object local to the Training application only

		CHECK(hSemL_Sim!=NULL, lblKO);
		usMainTaskNbr = Telium_CurrentTask();               // Get the Main Task number

		// Second Task creation
		// ====================

		hTsk=Telium_Fork(DualSimTask, &dum1, dum2);        // Fork Second Task
		CHECK(hTsk!=NULL, lblKO);

		goto lblEnd;

		//////// ---------------------------------------------------------------
		break;

	default:
		return;
		break;
	}
	// Errors treatment
	// ****************
	lblKO:                                                // None-classified low level error
	GL_Dialog_Message(hGoal, NULL, "NOTE! GPRS might be offline", GL_ICON_NONE, GL_BUTTON_VALID, 5*1000);
	goto lblEnd;
	lblEnd:
	if (hSemL_Sim)
		OSL_Semaphore_Destroy(hSemL_Sim);                 // Destroy the semaphore object
	hSemL_Sim = NULL;
}

