/**
 * \author	Ingenico
 * \author	Copyright (c) 2008 Ingenico, rue claude Chappe,\n
 *			07503 Guilherand-Granges, France, All Rights Reserved.
 *
 * \author	Ingenico has intellectual property rights relating to the technology embodied \n
 *			in this software. In particular, and without limitation, these intellectual property rights may\n
 *			include one or more patents.\n
 *			This software is distributed under licenses restricting its use, copying, distribution, and\n
 *			and decompilation. No part of this software may be reproduced in any form by any means\n
 *			without prior written authorization of Ingenico.
 */


/////////////////////////////////////////////////////////////////
//// Includes ///////////////////////////////////////////////////

#include "Cless_Implementation.h"


/////////////////////////////////////////////////////////////////
//// Macros & preprocessor definitions //////////////////////////


/////////////////////////////////////////////////////////////////
//// Global data definition /////////////////////////////////////

Telium_File_t* hSwipe2 = NULL;
Telium_File_t* hCam0 = NULL;
int bSwipe2DriverOpened;
int bCam0DriverOpened;

typedef struct
{
    unsigned short type_code;
	unsigned short service;
	unsigned long m_ulAmount;
	int m_nCurrencyCode;
} T_PERFORM_PAYMENT;


word g_ScanningTask;										// To store the different tasks ID.
t_topstack * g_tsScanning_task_handle;						// Handle of the scanning task.
int g_ListOfEvent;											// Global variable used by the scanning task to provide the event.

static word g_wCurrentTask;
static unsigned char g_bScanning_task_to_be_killed = FALSE;	// Global variable used to stop the scanning task.
static int g_ScanningTaskRunning = FALSE;					// Indicates if the scanning task is still running or if it is waiting to be killed.

static int swipeDetected = FALSE;

/////////////////////////////////////////////////////////////////
//// Static functions definition ////////////////////////////////

static int __Cless_Scan_ContactlessCancel (void);

// Data for task management (transparent mode)
static word __Cless_Scan_StartScanningTask (void);
static void __Cless_Scan_InitScanningVariables (void);


/////////////////////////////////////////////////////////////////
//// Functions //////////////////////////////////////////////////


static int __Cless_Scan_ContactlessCancel (void)
{
	int nResult;
	
	// Depending on the kernel used, send the cancel command
	switch (Cless_Customisation_GetUsedPaymentScheme())
	{
	case (CLESS_SAMPLE_CUST_PAYPASS):
		nResult = PayPass3_Cancel();
		break;

	case (CLESS_SAMPLE_CUST_PAYWAVE):
		nResult = payWave_Cancel ();
		break;

#ifndef DISABLE_PURE
	case (CLESS_SAMPLE_CUST_PURE):
		nResult = PURE_Cancel();
		break;
#endif

	default:
		break;
	}

	return (nResult);
}




//! \brief Open the CAM0 and SWIPE if not already opened.
void Cless_Scan_TransacOpenDrivers(void)
{
	unsigned char StatusSwipe;

	perflog("MG\tpW_CUST\tCless_Scan_TransacOpenDrivers");
	perflog("MG\tpW_CUST\tTelium_Stdperif(SWIPE2)");
	hSwipe2 = Telium_Stdperif("SWIPE2", NULL);
	perflog("MG\tpW_CUST\tExit Telium_Stdperif(SWIPE2)");
	if (hSwipe2 != NULL)
	{
		perflog("MG\tpW_CUST\tTelium_Status(SWIPE2)");
		Telium_Status(hSwipe2, &StatusSwipe);
		perflog("MG\tpW_CUST\tExit Telium_Status(SWIPE2)");
		if ((StatusSwipe & TRACK_READ) != 0)
			swipeDetected = TRUE;
		else swipeDetected = FALSE;
	}
	else swipeDetected = FALSE;

	perflog("MG\tpW_CUST\tTelium_Stdperif(CAM0)");
	hCam0 = Telium_Stdperif("CAM0", NULL);
	perflog("MG\tpW_CUST\tExit Telium_Stdperif(CAM0)");
	if (hCam0 != NULL)
	{
		perflog("MG\tpW_CUST\tTelium_Fclose(CAM0)");
		Telium_Fclose(hCam0);
		perflog("MG\tpW_CUST\tExit Telium_Fclose(CAM0)");
		hCam0 = NULL;
	}
	perflog("MG\tpW_CUST\tExit Cless_Scan_TransacOpenDrivers");
}



//! \brief Close the CAM0 driver.
void Cless_Scan_TransacCloseDriversExceptSwipe(void)
{
	hCam0 = Telium_Stdperif("CAM0", NULL);
	if (hCam0 != NULL)
	{
		Telium_Fclose(hCam0);
		hCam0 = NULL;
	}
}


//! \brief Close the drivers opened by the application.
void Cless_Scan_TransacCloseDrivers(void)
{
	swipeDetected = FALSE;

	hSwipe2 = Telium_Stdperif("SWIPE2", NULL);
	if (hSwipe2 != NULL)
	{
		Telium_Fclose(hSwipe2);
		hSwipe2 = NULL;
	}
	hCam0 = Telium_Stdperif("CAM0", NULL);
	if (hCam0 != NULL)
	{
		Telium_Fclose(hCam0);
		hCam0 = NULL;
	}
}


//! \brief Start an EMV contact transaction.
//! \param[in] ulAmount Transaction amount.
//! \param[in] nCurrencyCode Transaction currency code.
//! \return
//!	- \a TRUE if EMV contact component correctly called.
//!	- \a FALSE if EMV contact component cannot be called.
int Cless_Scan_TransacCam0(unsigned long ulAmount, int nCurrencyCode)
{
	int bResult;
	unsigned char priority;
	int  ret_code;
	T_PERFORM_PAYMENT Data;

	HelperLedsOff();
	{
		MSGinfos MsgInfo;
		MsgInfo.coding = _ISO8859_;
		MsgInfo.file = GetCurrentFont();
		MsgInfo.message = "CHIP\nTRANSACTION";
		Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &MsgInfo, LEDSOFF);
		Helper_RefreshScreen(WAIT, HELPERS_MERCHANT_SCREEN);
	}


	bResult = FALSE;

	//TransacStopCless();
	Cless_Scan_TransacCloseDrivers();

	if (Telium_ServiceGet(0x5D,120,&priority)==0)
	{
		Data.type_code = 0x5D;
		Data.service = 120;
		Data.m_ulAmount = ulAmount;
		Data.m_nCurrencyCode = nCurrencyCode;
		Telium_ServiceCall(0x5D,120,sizeof(Data),&Data,&ret_code);

		bResult = TRUE;
	}
	else if (Telium_ServiceGet(0x51,120,&priority)==0)
	{
		Data.type_code = 0x51;
		Data.service = 120;
		Data.m_ulAmount = ulAmount;
		Data.m_nCurrencyCode = nCurrencyCode;
		Telium_ServiceCall(0x51,120,sizeof(Data),&Data,&ret_code);

		bResult = TRUE;
	}
#ifdef __TELIUM3__
	else if (Telium_ServiceGet(0x57b3,120,&priority)==0)
	{
		Data.type_code = 0x57b3;
		Data.service = 120;
		Data.m_ulAmount = ulAmount;
		Data.m_nCurrencyCode = nCurrencyCode;
		Telium_ServiceCall(0x57b3,120,sizeof(Data),&Data,&ret_code);

		bResult = TRUE;
	}
#endif

	return bResult;
}

//! \brief Start an SWIPE transaction.
//! \param[in] ulAmount Transaction amount.
//! \param[in] nCurrencyCode Transaction currency code.
//! \return
//!	- \a TRUE if SWIPE component correctly called.
//!	- \a FALSE if SWIPE component cannot be called.
int Cless_Scan_TransacSwipe2(unsigned long ulAmount, int nCurrencyCode)
{
#ifndef __TELIUM3__
	int bResult;
	unsigned char priority;
	int  ret_code;
	T_PERFORM_PAYMENT Data;
#endif
	MSGinfos MsgInfo;

	HelperLedsOff();

	MsgInfo.coding = _ISO8859_;
	MsgInfo.file = GetCurrentFont();
	MsgInfo.message = "SWIPE\nTRANSACTION";
	Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_3, &MsgInfo, LEDSOFF);
	Helper_RefreshScreen(WAIT, HELPERS_MERCHANT_SCREEN);

#ifndef __TELIUM3__
	bResult = FALSE;

	//TransacStopCless();
	Cless_Scan_TransacCloseDriversExceptSwipe(); // Close just the CAM0

	if (Telium_ServiceGet(0x68,200,&priority)==0)
	{
		Data.type_code = 0x68;
		Data.service = 200;
		Data.m_ulAmount = ulAmount;
		Data.m_nCurrencyCode = nCurrencyCode;
		Telium_ServiceCall(0x68,200,sizeof(Data),&Data,&ret_code);

		bResult = TRUE;
	}
	Cless_Scan_TransacCloseDrivers();

	return bResult;
#else
	Cless_Scan_TransacCloseDrivers();
	return TRUE;
#endif
}



/////////////////////////////////////////////////////////////////////////////////////
//    ####  ####  ###  #   # #   # #  #   #  ####        #####  ###   #### #   #   //
//   #     #     #   # ##  # ##  # #  ##  # #              #   #   # #     #  #    //
//    ###  #     ##### # # # # # # #  # # # #  ##          #   #####  ###  ###     //
//       # #     #   # #  ## #  ## #  #  ## #   #          #   #   #     # #  #    //
//   ####   #### #   # #   # #   # #  #   #  ####          #   #   # ####  #   #   //
/////////////////////////////////////////////////////////////////////////////////////

/**
 * function:    __Cless_Scan_StartScanningTask
 * @brief: Waits an event on keyboard, swipe or chip.
 * @param	void
 * @return	void
 */

static word __Cless_Scan_StartScanningTask (void)
{
	int nEvent, bSendCancel, bWaitToBeKilled;
	unsigned char bStartIndication = TRUE;
	Telium_File_t * hKeyboard = NULL;
	Telium_File_t * hCam0Task = NULL;
	Telium_File_t * hSwipeTask = NULL;
	unsigned char StatusSwipe,StatusCam0;
	
	bWaitToBeKilled = FALSE;
	bSendCancel = FALSE;
	g_ScanningTaskRunning = TRUE; // Indicates the task is running
	g_ScanningTask = Telium_CurrentTask(); // get the Scanning task ID and store it in a global variable

	// Open the keyboard driver
	hKeyboard = Telium_Fopen("KEYBOARD", "r*");

	while (!g_bScanning_task_to_be_killed) // While the task is not to be killed by the custom application
	{
		if (bWaitToBeKilled) // A cancel has been sent to the Engine, task is waiting to be killed
		{
			if (hKeyboard != NULL)
			{
				Telium_Fclose (hKeyboard); // Close the keyboard driver
				hKeyboard = NULL;
			}
			g_ScanningTaskRunning = FALSE; // Indicate to the custom application the task is waiting to be killed
			Telium_Ttestall (0,0); // Wait to be killed
		}

		StatusSwipe = 0;
		StatusCam0 = 0;
		nEvent = 0;

		if (bStartIndication)
		{
			bStartIndication = FALSE;

			// Allows to 
			if (Telium_SignalEvent (g_wCurrentTask, (tEvent)E_USER_EVENT_START) != cOK)
				GTL_Traces_DiagnosticText("Scanning task 2: Error when setting the event\n");
		}
		
		hCam0Task = Telium_Fopen("CAM0", "rw*");
		if (hCam0Task != NULL)
		{
			Telium_Status(hCam0Task, &StatusCam0);
			if ((StatusCam0 & CAM_PRESENT) != 0)
				nEvent = CAM0;
		}

		hSwipeTask = Telium_Fowner("SWIPE2", g_ScanningTask);
		if (hSwipeTask != NULL)
		{
			Telium_Status(hSwipeTask, &StatusSwipe);
			if ((StatusSwipe & TRACK_READ) != 0)
				nEvent = SWIPE2;
		}
		if (swipeDetected)
			nEvent = SWIPE2;

		// No previous events (SWIPE2 or CAM0), make a ttestall during 10ms
		if (nEvent == 0)
			nEvent = Telium_Ttestall (KEYBOARD | CAM0 | SWIPE2 | USER_EVENT, 0);

		// According to the event that may have occurred
		switch(nEvent)
		{
		case (0): // Timeout, no event, continue scanning
		case USER_EVENT: // User event (from main task), it indicates the task is going to be killed (because g_bScanning_task_to_be_killed has been set to TRUE by the custom application
			break;

		case KEYBOARD: // Keyboard event
			if (Telium_Getchar() == T_ANN) // Cancel only if red key pressed
			{
				bSendCancel = TRUE;
			}
			break;

		default: // Other event (swipe, chip, etc).
			bSendCancel = TRUE;
			break;
		}

		// If cless transaction shall be cancelled
		if (bSendCancel)
		{
			__Cless_Scan_ContactlessCancel (); // Cancel the transaction. It is also possible to use ContactlessCancelClessDC(NULL, NULL)
			if (hCam0Task != NULL)
			{
				Telium_Fclose(hCam0Task);
				hCam0Task = NULL;
			}
			Telium_Fowner("SWIPE2", g_wCurrentTask);
			g_ListOfEvent = nEvent; // Update the event
			bWaitToBeKilled = TRUE; // Task is going to wait to be killed
		}
		
	} // End While

	if (hCam0Task != NULL)
	{
		Telium_Fclose(hCam0Task);
		hCam0Task = NULL;
	}
	Telium_Fowner("SWIPE2", g_wCurrentTask);

	// The task is stopped by the main application, so no event occurred
	g_ListOfEvent = 0; // Clear the list of event (because of timeout)
	if (hKeyboard != NULL)
	{
		Telium_Fclose (hKeyboard); // Close the keyboard driver
		hKeyboard = NULL;
	}
	g_ScanningTaskRunning = FALSE; // Update global variable

	// Send the event to the main task (application that calls the TPass DLL)
	if (Telium_SignalEvent (g_wCurrentTask, (tEvent)E_USER_EVENT) != cOK)
		GTL_Traces_DiagnosticText ("Scanning task: Error when setting the event\n");

	Telium_Ttestall (0,0); // Wait to be killed
	return TRUE;
}



/**
 * function:    Cless_Scan_LaunchScanningTask
 * @brief: Launch the scanning task.
 * @param	void
 * @return	OK if task correctly launch, KO else.
 */

int Cless_Scan_LaunchScanningTask (void) {
	// Init global variables
	__Cless_Scan_InitScanningVariables();

	// Get the main task id
	g_wCurrentTask = Telium_CurrentTask();

	// Launch the scanning task
	g_tsScanning_task_handle = Telium_Fork (&__Cless_Scan_StartScanningTask, NULL, -1);

	// The task cannot be created
	if (g_tsScanning_task_handle == NULL)
		return KO;
	
	return OK;
}



/**
 * function:    CustomKillScanningTask
 * @brief: Kill the scanning task.
 * @param	void
 * @return	void
 */
void Cless_Scan_KillScanningTask (void)
{
	if (g_tsScanning_task_handle != NULL) // If the task is lunched
	{
		g_bScanning_task_to_be_killed = TRUE; // To Stop the task (if not already waiting to be killed because of an event)

		// Send an event to make the task ready to be killed
		if (Telium_SignalEvent (g_ScanningTask, (tEvent)E_USER_EVENT) != cOK)
			GTL_Traces_DiagnosticText ("Main task: Error when setting the event\n");

		while (g_ScanningTaskRunning == TRUE) // While the task has not terminated processing
		{
			// Waits a little
			Telium_Ttestall (USER_EVENT,1);
		}

		// The task is ready to be killed, kill it
		//Telium_Kill(g_tsScanning_task_handle, "-*");
		g_tsScanning_task_handle = NULL;
	}
}



/**
 * function:    __Cless_Scan_InitScanningVariables
 * @brief: Initialise scanning global variables
 * @param	void
 * @return	void
 */
static void __Cless_Scan_InitScanningVariables (void)
{
	g_wCurrentTask = 0xFF; // Init the custom application task number
	g_ScanningTask = 0xFF; // Init the scanning task number
	g_tsScanning_task_handle = NULL; // init the scanning task handle
	g_bScanning_task_to_be_killed = FALSE; // Task has not to be killed
	g_ScanningTaskRunning = FALSE; //Task is not running
}


