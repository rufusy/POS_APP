/**
 * \file	Cless_Scan.h
 * \brief	Contains several common functions.
 *
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

#ifndef __CLESS_SAMPLE_SCAN_H__INCLUDED__
#define __CLESS_SAMPLE_SCAN_H__INCLUDED__

/////////////////////////////////////////////////////////////////
//// Macros & preprocessor definitions //////////////////////////
#define E_USER_EVENT_PIN    29							// User event offset
#define USER_EVENT_PIN     	1 << E_USER_EVENT_PIN  		// User event	
#define E_USER_EVENT		30							// User event offset
#define USER_EVENT			1 << E_USER_EVENT			// User event	

#define E_USER_EVENT_START	31							// User event offset
#define USER_EVENT_START	1 << E_USER_EVENT_START		// User event	

#define E_USER_EVENT_START_DISPLAY	25									// 25 is reserved for bluetooth (not used here)
#define USER_EVENT_START_DISPLAY	1 << E_USER_EVENT_START_DISPLAY		// User event
#define E_USER_EVENT_DO_NOT_DISPLAY	26									// 26 is reserved for barcode (not used here)
#define USER_EVENT_DO_NOT_DISPLAY	1 << E_USER_EVENT_DO_NOT_DISPLAY	// User event

/////////////////////////////////////////////////////////////////
//// Types //////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////
//// Global variables ///////////////////////////////////////////
extern word g_ScanningTask;						// To store the different tasks ID.
extern t_topstack * g_tsScanning_task_handle;	// Handle of the scanning task.
extern int g_ListOfEvent;						// Global variable used by the scanning task to provide the event.


/////////////////////////////////////////////////////////////////
//// Functions //////////////////////////////////////////////////
int Cless_Scan_TransacCam0(unsigned long ulAmount, int nCurrencyCode);
int Cless_Scan_TransacSwipe2(unsigned long ulAmount, int nCurrencyCode);

int  Cless_Scan_LaunchScanningTask(void);
void Cless_Scan_KillScanningTask(void);
void Cless_Scan_TransacOpenDrivers(void);
void Cless_Scan_TransacCloseDriversExceptSwipe(void);
void Cless_Scan_TransacCloseDrivers(void);
int Cless_Scan_TransacCam0(unsigned long ulAmount, int nCurrencyCode);
int Cless_Scan_TransacSwipe2(unsigned long ulAmount, int nCurrencyCode);

#endif // __CLESS_SAMPLE_SCAN_H__INCLUDED__
