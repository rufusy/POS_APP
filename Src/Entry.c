/**
 * \file
 * \brief This module implements all the Telium Manager services.
 *
 * \author Ingenico
 * \author Copyright (c) 2012 Ingenico, 28-32, boulevard de Grenelle,\n
 * 75015 Paris, France, All Rights Reserved.
 *
 * \author Ingenico has intellectual property rights relating to the technology embodied\n
 * in this software. In particular, and without limitation, these intellectual property rights may\n
 * include one or more patents.\n
 * This software is distributed under licenses restricting its use, copying, distribution, and\n
 * and decompilation. No part of this software may be reproduced in any form by any means\n
 * without prior written authorisation of Ingenico.
 **/

/////////////////////////////////////////////////////////////////
//// Includes ///////////////////////////////////////////////////

#include "sdk.h"
#include <globals.h>
#include <signal.h>
#include "SEC_interface.h"
#include "VGE_FMG.h"
#include "OSL_Layer.h"
#include "Sqlite.h"
#include "MenuManager.h"
#include "TagOS.h"
#include "_emvdctag_.h"
#include "EngineInterface.h"
#include "EPSTOOL_TlvTree.h"
#include "EPSTOOL_Convert.h"

#include "EMV_UserInterfaceDisplay.h"
#include "EMV_ServicesEmv.h"
#include "EMV_Manager.h"

#include "Common_Kernels_API.h"
#include "oem_public_def.h"
#include "dll_wifi.h"
#include "SSL_.h"

void reset_diagnostic(void);
void print_diagnostic();

//****************************************************************************
//      EXTERN
//****************************************************************************
const char *getAppCmpDat(void); // See Mapapp.c
const char *getAppCmpTim(void);

static int g_bDllTpassLoaded = FALSE;

//// Macros & preprocessor definitions //////////////////////////

//// Types //////////////////////////////////////////////////////

void entry(void);

//// Static function definitions ////////////////////////////////
///
#ifndef DISABLE_INTERAC
static int gs_bInteracTxnUnderway = FALSE;
#endif

static int gs_bCaKeyCheckingDone = FALSE;

static int gs_nHeaderEventMask = -1;
static unsigned long gs_ulNoCardTimeOut = 0; // No Card timeout (in milliseconds)

// Application selection
// =====================
#define AMOUNT_ENTRY           TRUE  // FALSE => swipe card from Manager
// then enter Amount into application selected
// TRUE => Enter Amount then swipe card from Manager
// before application selection

#define PRIORITY_SMART         TRUE  // FALSE => Priority to swipe (Dual reader iPP480)
// Swipe is read at insertion, the chip is not taken in account
// TRUE => Priority to chip (Dual reader iPP480)
// Chip is read at insertion, if fall back swipe is read at removal

// Idle Animation
// ==============
#define MAX_IMG   48
#define ANIMATION TRUE
//#define ANIMATION              FALSE // FALSE => Please Insert Card shows on Idle Display
// TRUE => Card Animation shows on Idle Display

// Time to call (see service call time_function)
// =============================================
#define TIME_TO_CALL           TRUE  // FALSE => Remove message "time to call" on screen every 1mn
// TRUE => Show message "time to call" on screen every 1mn

// File receive
// ============
#define MAX_RSP 256

#define __CLESS_SERVICES_PRIORITY_DEFAULT           150 + 10       //!< Default priority for services.
#define __CLESS_SERVICES_PRIORITY_IDLE_MSG          150 + 30       //!< Priority of the idle message service.
#define IDLE_HIGH_PRIORITY                          150 + 30 //   priority => 150 lowest and 255 highest

//// Types //////////////////////////////////////////////////////

//// Static function definitions ////////////////////////////////

static int __CLESS_ServicesManager(unsigned int size, StructPt *data);
static int __CLESS_ServicesCless(unsigned int size, StructPt *data);
static void __CLESS_GiveInterface(unsigned short appliId);

static T_SERVICE_CALL_SHARED_EXCHANGE_STRUCT* __GetSharedExchStructFromServiceCall(void *pData);
//****************************************************************************
//      PRIVATE TYPES
//****************************************************************************
typedef struct stParams // See is_change_init() / modif_param() service call
{
	char tcOldDate[24+1];
	char tcOldFmtDate[24+1];
	char tcOldLanguage[24+1];
	char tcOldPabx[24+1];
	char tcOldPPad[24+1];
	char tcOldPPadType[24+1];
	char tcOldISOreader[24+1];
	char tcOldTMSaccess[24+1];
} ST_PARAMS;

typedef struct stData // Swipe in case of fall_back() service call (iPP480 dual reader)
{
	int iRetIso1;                   // Track1
	byte tcTrk1[128];
	bool bFlagIso1;
	int iRetIso2;                   // Track2
	byte tcTrk2[128];
	bool bFlagIso2;
	int iRetIso3;                   // Track3
	byte tcTrk3[128];
	bool bFlagIso3;
} ST_DATA;

typedef struct // Structure Transaction info in/out to communicate with service call 100 (Manager)
{              // in case of fall_back() service call (iPP480 dual reader)
	S_TRANSIN     param_in;			// Input parameter
	S_TRANSOUT    param_out;	    // Output parameter
} ST_IO_TRANS;

//****************************************************************************
//      PRIVATE DATA
//****************************************************************************
// Properties of the Idle screen (Goal)
// ====================================
static const ST_DSP_LINE txIdle[] =
{
		{ {GL_ALIGN_CENTER,   GL_ALIGN_CENTER, GL_COLOR_WHITE,  GL_COLOR_BLACK,   0, FALSE, {0, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_LARGE}},  // Line1
				{GL_ALIGN_CENTER,   GL_ALIGN_CENTER, FALSE,   0, FALSE, {0, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}} },
				{ {GL_ALIGN_CENTER,   GL_ALIGN_CENTER, GL_COLOR_WHITE,  GL_COLOR_BLACK,     0, FALSE, {8, 2, 8, 2}, {1, 1, 1, 1, GL_COLOR_WHITE}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_XLARGE}}, // Line2
						{GL_ALIGN_CENTER,   GL_ALIGN_CENTER, TRUE,    0, FALSE, {8, 4, 8, 6}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_XLARGE}} },
						{ {GL_ALIGN_CENTER,   GL_ALIGN_CENTER, GL_COLOR_WHITE,  GL_COLOR_BLACK,   0, FALSE, {0, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_WHITE}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_XLARGE}}, // Line3
								{GL_ALIGN_CENTER,   GL_ALIGN_CENTER, FALSE,   0, FALSE, {0, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}} }
};

// Properties of the animated image (Goal)
// =======================================
static const ST_IMAGE xImage =
{
		{GL_COLOR_WHITE, TRUE, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_LARGE}, GL_DIRECTION_ALL, {0, 0, 0, 0, GL_COLOR_WHITE}, {0, 0, 0, 0} },
		{TRUE, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}, GL_DIRECTION_ALL, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0} }
};

// Properties of the default screen (Goal)
// =======================================
static const ST_DSP_LINE thScreen[] =
{
		{ {GL_ALIGN_LEFT, GL_ALIGN_CENTER, GL_COLOR_WHITE, GL_COLOR_BLACK, 100, FALSE, {1, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_WHITE}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}}, // Line0
				{GL_ALIGN_LEFT, GL_ALIGN_CENTER, FALSE,  100, FALSE, {2, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}} },
				{ {GL_ALIGN_LEFT, GL_ALIGN_CENTER, GL_COLOR_WHITE, GL_COLOR_BLACK, 100, FALSE, {1, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_WHITE}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}}, // Line1
						{GL_ALIGN_LEFT, GL_ALIGN_CENTER, FALSE,  100, FALSE, {2, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}} },
						{ {GL_ALIGN_LEFT, GL_ALIGN_CENTER, GL_COLOR_WHITE, GL_COLOR_BLACK, 100, FALSE, {1, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_WHITE}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}}, // Line2
								{GL_ALIGN_LEFT, GL_ALIGN_CENTER, FALSE,  100, FALSE, {2, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}} },
								{ {GL_ALIGN_LEFT, GL_ALIGN_CENTER, GL_COLOR_WHITE, GL_COLOR_BLACK, 100, FALSE, {1, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_WHITE}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}}, // Line3
										{GL_ALIGN_LEFT, GL_ALIGN_CENTER, FALSE,  100, FALSE, {2, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}} },
										{ {GL_ALIGN_LEFT, GL_ALIGN_CENTER, GL_COLOR_WHITE, GL_COLOR_BLACK, 100, FALSE, {1, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_WHITE}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}}, // Line4
												{GL_ALIGN_LEFT, GL_ALIGN_CENTER, FALSE,  100, FALSE, {2, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}} }
};


// Properties of the default printer (Goal)
// ========================================
static const ST_PRN_LINE xPrinter =
{
		GL_ALIGN_LEFT, GL_ALIGN_CENTER, FALSE, 100, {0, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {1, 0, 1, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_LARGE}
};


struct dateTMS {
	int month;
	int day;
	int year;
	int hour;
	int minute;
	int seconds;
};

// Some messages to display
// ========================
static const char zAppName[] = "Team Elephant";                      // Application name
static const char zColdReset[] = "COLD RESET";         // See after_reset() service call
static const char zWarmReset[] = "WARM RESET";
static const char zTimeToCall[] = "periodic check done";      // See time_function() service call
static const char zIdleMsg[] = "Tap, Insert or Swipe a Card";        // See idle_message() service call
static const char zDate[] = "Date:%.2s/%.2s/%.2s  %.2s:%.2s\n"; // See state() / consult() / mcall() service call
static const char zDate1[] = "%.2s/%.2s/20%.2s   %.2s:%.2s\n";  // See is_change_init() / modif_param() service call
static const char zDate2[] = "20%.2s/%.2s/%.2s   %.2s:%.2s\n";
static const char zDate3[] = "%.2s.%.2s.20%.2s   %.2s:%.2s\n";


// Some handles
// ============
T_GL_HGRAPHIC_LIB hGoal=NULL;      // Handle of the graphics object library
T_OSL_HDLL hBeepDll=NULL;          // Handle of the Beep Dll.

Telium_File_t *hDsp=NULL;          // Handle of the peripheral Display
Telium_File_t *hPrn=NULL;          // Handle of the peripheral Printer
Telium_File_t *hKbd=NULL;          // Handle of the peripheral Keyboard
Telium_File_t *hTsc=NULL;          // Handle of the peripheral Touch Screen
int kbdLocal = 0;
int localDisplay = 0;

T_OSL_HSEMAPHORE hSemS=NULL;       // Handle of the first semaphore (security SHARED)
T_OSL_HMESSAGE_QUEUE hQueueS=NULL; // Handle of the queue (security SHARED)
T_OSL_HSHARED_MEM hMemS=NULL;      // Handle of the shared memory (security SHARED)
T_OSL_HSEMAPHORE hSemS2=NULL;      // Handle of the second semaphore (security SHARED)

// Parameter disk configuration
// ============================
static S_FS_PARAM_CREATE xCfg;
static unsigned long ulSize;

// Animated image sequence
// =======================
const byte tcIdxImg[MAX_IMG] = {
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
		10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
		9,  8,  7,  6,  5,  4,  3,  2,  1, 0
};

static word usTimeout=10;     // Timeout between two images
static bool bPictureOk = false; // Flag to check if pictures loaded into HOST disk

// Currency table according to ISO 4217
// ====================================
static const S_MONEY_EXTENDED txCurrency[] = {
		{{{'T','Z','S'}, {'8','3','4'}, 3}, {'.', ',', CURRENCY_AFTER_AMOUNT}},  // Tanzanian Shillings
		{{{'U','S','D'}, {'8','4','0'}, 2}, {'.', ',', CURRENCY_AFTER_AMOUNT}} // United State Dollar
};


#define TRAINING_APPLI_TYPE    0x57C0 // Training application type
#define USER2_APPLI_TYPE	   0x57C1 // User2 application type

// Task2 used for IAM process
// ==========================
static word usMainTaskNbr;
//static S_MESSAGE_IAM xMessage;

// Manager parameters
// ==================
static ST_PARAMS xParams; // See is_change_init() / modif_param() service call


static void __CLESS_ServiceManager_GetAppName(NO_SEGMENT appliId, char *name, int nameBufferSize);


//// Global variables ///////////////////////////////////////////


//// Functions //////////////////////////////////////////////////




#ifdef __PREFORMANCE_LOG__
perflog_t perflog_data[1000];
int perflog_data_size = 0;

void perflog_dump(void) {
	int i;
	for(i = 0; i < perflog_data_size; i++)
		Sys_log(4, "%s\t%llu", perflog_data[i].string, perflog_data[i].time);

	perflog_data_size = 0;
}
#endif


int My_PSQ_Give_Language(void) {
	const char *lang;

	lang = GetManagerLanguage();
	if (lang != NULL) {
		if (strncmp(lang, "fr", 2) == 0)
			return 1;
	}

	return 2;
}

void confirmGraphicLibHandle(void){
	if (hGoal == NULL) {
		hGoal = GL_GraphicLib_Create(); // Create "graphic library" handle
	}
}

int Version_Function(void) {
	// Local variables *
	// *****************
	char Version[8 + 1], tcDisplay[100];
	char VVchar[2+1],RRchar[2+1],PPchar[2+1];
	card VV,RR,PP;
	int LocalDisplayOpened = 0;


	// Read date and time
	// ******************
	hDsp = Telium_Stdperif("DISPLAY", NULL);    // Check "display" peripheral already opened? (done by Manager)
	if (hDsp == NULL){                          // No, then open it
		LocalDisplayOpened = 1;
		hDsp = Telium_Fopen("DISPLAY", "w*");   // Open "display" peripheral
	}

	if (hDsp != NULL) {

		memset(tcDisplay, 0, sizeof(tcDisplay));
		memset(Version, 0, sizeof(Version));

		Version[0] = 'v';

		// process version
		memset(VVchar, 0, sizeof(VVchar));
		strncpy(VVchar, _ING_APPLI_TELIUM_TETRA_PACKAGE_VERSION, 2);
		dec2num(&VV, VVchar, 0);
		num2dec(&Version[1], VV, 0);

		Version[strlen(Version)] = '.';
		memset(RRchar, 0, sizeof(RRchar));
		strncpy(RRchar, &_ING_APPLI_TELIUM_TETRA_PACKAGE_VERSION[2], 2);
		dec2num(&RR, RRchar, 0);
		num2dec(&Version[strlen(Version)], RR, 0);

		Version[strlen(Version)] = '.';
		memset(PPchar, 0, sizeof(PPchar));
		strncpy(PPchar, &_ING_APPLI_TELIUM_TETRA_PACKAGE_VERSION[4], 2);
		dec2num(&PP, PPchar, 0);
		num2dec(&Version[strlen(Version)], PP, 0);

		Telium_Sprintf(tcDisplay, "App Version \n %s - D", Version);

		GL_Dialog_Message(hGoal, NULL, tcDisplay, GL_ICON_INFORMATION, GL_BUTTON_NONE, 3*1000);

		if (LocalDisplayOpened == 1) {
			Telium_Fclose(hDsp);                    // Close "display" peripheral
			hDsp=NULL;
		}
	}

	return FCT_OK;
}

//===========================================================================
//! \brief This function tests the presence of security DLL. And if it has
//! at least a release 0204.
//===========================================================================
static void	Test_DLL_Security_Presence(void) {
	int sec_ver;
	int ret;
	Telium_File_t * printer;

#ifndef _SIMULPC_
	ret = ObjectLoad (OBJECT_TYPE_DLL, "SECURITY");
#else
	ret =0;
#endif

	if ((ret != 0) && (ret != OL_ALREADY_LOADED)) {
		printer=Telium_Fopen("PRINTER","w-");
		if (printer!=NULL) {
			Telium_Pprintf ("\n""Custom \n");
			Telium_Pprintf ("Please LOAD DLL SECURITY\n");

			Telium_Pprintf ("\n\n\n\n\n");
			Telium_Ttestall(PRINTER,0);
			Telium_Fclose(printer);
		}
	} else {
#ifndef _SIMULPC_
		sec_ver = SEC_Version();
#else
		sec_ver =0x020400;
#endif

		if (sec_ver<0x020400) {
			printer=Telium_Fopen("PRINTER","w-");
			if (printer!= NULL) {
				Telium_Pprintf ("\n""Custom \n");
				Telium_Pprintf ("\n""Please LOAD DLL SECURITY\nWith VERSION >= 0204");

				Telium_Pprintf ("\n\n\n\n\n");
				Telium_Ttestall(PRINTER,0);
				Telium_Fclose(printer);
			}
		}
	}
}

//
//// ***********************************************************
//// Task to manage IAM process (See after_reset() service call)
//// ***********************************************************
//static word Task2(void) {
//	// Local variables
//	// ***************
//	tStatus usSta;
//	T_GL_HWIDGET hScreen=NULL;
//	ST_DSP_LINE xDspLine;
//	T_GL_HWIDGET hDocument=NULL;
//	ST_PRN_LINE xPrnLine;
//	Telium_File_t *hDsp=NULL;
//#ifndef __TELIUM3__
//	Telium_File_t *hPrn=NULL;
//#endif
//	char tcSnd[256];
//	char tcPrint[256+1];
//	byte p;  // Printer line index
//	int iRet;
//
//	// Assign a mailbox to this task
//	// *****************************
//	iRet = Register_Mailbox_User(TaskIam1);
//	CHECK(iRet>0, lblKO);
//
//	// Send a event to Main task
//	// *************************
//	usSta = Telium_SignalEvent(usMainTaskNbr, EVT_SYNCHRO_DIALOG);     // Signal to main task that task2 is initialized
//	CHECK(usSta==cOK, lblKO);
//
//	// Wait for incoming message
//	// *************************
//	while(1)
//	{
//		Telium_Ttestall(MSG, 0);                                       // Wait until messages received
//
//		while (1)
//		{
//			iRet = Read_Message(&xMessage, 0);                         // Read until no more message
//
//			if (iRet==0)                                               // Exit from the loop and wait for next messages
//				break;
//
//			switch (xMessage.type)
//			{
//			case 0:                                                    // *** Dialog between task and Training application ***
//				hDsp = Telium_Stdperif("DISPLAY", NULL);               // Check "display" peripheral already opened? (done by Manager)
//				if (hDsp == NULL)                                      // No, then open it
//					hDsp = Telium_Fopen("DISPLAY", "w*");              // Open "display" peripheral
//
//				if (hDsp != NULL)
//				{
//					// Get message received from TRAINING
//					// ==================================
//					hScreen = GoalCreateScreen(hGoal, thScreen, NUMBER_OF_LINES(thScreen), GL_ENCODING_UTF8);
//					GoalClrScreen(hScreen, GL_COLOR_BLACK, 0, false);  // Create and clear screen
//
//					xDspLine = thScreen[0];                            // Build screen and show it (IAM info, IAM message)
//					xDspLine.xColor.eTextAlign = GL_ALIGN_CENTER;
//					xDspLine.xColor.ulPen = GL_COLOR_YELLOW;
//					xDspLine.xBW.eTextAlign = GL_ALIGN_CENTER;
//					GoalDspLine(hScreen, 0, "TASK2 (Msg from TRAIN)", &xDspLine, 0, false);
//					GoalDspMultiLine(hScreen, 1, (char*)xMessage.value, xMessage.length, thScreen, NUMBER_OF_LINES(thScreen), 0, true);
//#ifndef __TELIUM3__
//					hPrn = Telium_Fopen("PRINTER", "w-*");             // Open "printer" peripheral
//					if (hPrn != NULL)
//					{
//#endif
//						p=0;                                           // Create document
//						hDocument = GoalCreateDocument(hGoal, GL_ENCODING_UTF8);
//
//						xPrnLine = xPrinter;                           // Build document (Task2, IAM info, Sender/Receiver, IAM type, IAM message)
//						xPrnLine.eTextAlign = GL_ALIGN_CENTER;
//						xPrnLine.xFont.eScale = GL_SCALE_XXLARGE;
//						GoalPrnLine(hDocument, p++, "TASK2", &xPrnLine);
//						xPrnLine.eTextAlign = GL_ALIGN_LEFT;
//						xPrnLine.xFont.eScale = GL_SCALE_LARGE;
//						GoalPrnLine(hDocument, p++, "Message IAM from Appli:", &xPrnLine);
//						Telium_Sprintf(tcPrint, "S:%04X R:%04X", xMessage.sender, xMessage.receiver);
//						GoalPrnLine(hDocument, p++, tcPrint, &xPrnLine);
//						Telium_Sprintf(tcPrint, "IAM Type : %04X\n\n", xMessage.type);
//						GoalPrnLine(hDocument, p++, tcPrint, &xPrnLine);
//						GoalPrnLine(hDocument, p++, "Msg from TRAINING:", &xPrnLine);
//						memset(tcPrint, 0, sizeof(tcPrint));
//						strncpy(tcPrint, (char*)xMessage.value, xMessage.length);
//						GoalPrnLine(hDocument, p++, tcPrint, &xPrnLine);
//						GoalPrnLine(hDocument, p++, "\n", &xPrnLine);
//						GoalPrnDocument(hDocument);                    // Print document
//
//						if (hDocument)
//							GoalDestroyDocument(&hDocument);           // Destroy document
//#ifndef __TELIUM3__
//						Telium_Fclose(hPrn);                           // Close "printer" peripheral
//					}
//#endif
//					Telium_Ttestall(0, 2*100);                         // Wait to see the message in synchronization with TrainingToTask()
//					// Send back message to TRAINING
//					// =============================
//					memset (tcSnd, 0, sizeof(tcSnd));
//					strcpy(tcSnd, "Hi, how are you doing?\n"           // Data to send
//							"Hi, how are you doing?\n"
//							"Hi, how are you doing?\n"
//							"Hi, how are you doing?");
//					// Send message
//					SendToApplication(TRAINING_APPLI_TYPE, 1, strlen(tcSnd), (byte*)tcSnd);
//
//					if (hScreen)
//						GoalDestroyScreen(&hScreen);                   // Destroy screen
//
//					Telium_Fclose(hDsp);                               // Close "display" peripheral
//				}
//				break;
//			default:
//				break;
//			}
//		}
//	}
//
//	lblKO:
//	return 0;
//}


// *****************************************************
// Progress bar counter (See after_reset() service call)
// *****************************************************
static T_GL_COORD Progress(T_GL_COORD sValue) // This function is called periodically to update the progress bar of the dialog box.
{                                             // The value received as parameter corresponds to the current value of the progress bar.
	Telium_Ttestall(0, 2);                    // When this value reaches the maximum, the dialog is automatically closed.

	return sValue+1;
}


//! \brief The Telium Manager calls this service to get information about the application.
//! \param[in] appliId The application ID.
//! \param[out] paramOut Output parameters.
//! \return Always \a FCT_OK.
//! \sa Telium Manager reference documentation.
int CLESS_ServicesManager_GiveYourSpecificContext(NO_SEGMENT appliId, S_SPECIFIC_CONTEXT *paramOut) {
	// To avoid warnings because 'appliId' is not used
	(void)appliId;

	ASSERT(paramOut != NULL);

	// Initialise the output parameter
	memclr(&paramOut->returned_state[paramOut->response_number],
			sizeof(paramOut->returned_state[paramOut->response_number]));

	// TODO: Set the following values to your needs

	// Give the resource file name that contains the icon of the application
	strncpy(paramOut->returned_state[paramOut->response_number].appname, _ING_APPLI_DATA_FILE_BINARY_NAME, sizeof(paramOut->returned_state[paramOut->response_number].appname) - 1);
	paramOut->returned_state[paramOut->response_number].no_appli = appliId;
	paramOut->returned_state[paramOut->response_number].mask = 0;
	paramOut->returned_state[paramOut->response_number].mask = 0;
	paramOut->returned_state[paramOut->response_number].type = 0;
	paramOut->returned_state[paramOut->response_number].support = 0;
	paramOut->returned_state[paramOut->response_number].cam = 0;
	paramOut->returned_state[paramOut->response_number].support = TRACK1_SUPPORT_MASK | TRACK2_SUPPORT_MASK | TRACK3_SUPPORT_MASK | CHIP_SUPPORT_MASK ;

	// To allow Manager look into the resource to get the icon of the application
	paramOut->returned_state[paramOut->response_number].cgui = CGUI_MASK;
	paramOut->response_number++;

	return FCT_OK;
}


//! \brief The Telium Manager calls this service to know the state of the application.
//! \param[in] appliId The application ID.
//! \param[out] paramOut Output parameters that contains the state of the application.
//! \return Always \a FCT_OK.
//! \sa Telium Manager reference documentation.
int CLESS_ServiceManager_IsState(NO_SEGMENT appliId, S_ETATOUT *paramOut) {
	ASSERT(paramOut != NULL);
#ifndef DISABLE_OTHERS_KERNELS
	S_STATE_PARAM state_param;
#endif

	// Initialise the output parameter
	memclr(&paramOut->returned_state[paramOut->response_number],
			sizeof(paramOut->returned_state[paramOut->response_number]));

	// Get the name of the application
	__CLESS_ServiceManager_GetAppName(appliId, paramOut->returned_state[paramOut->response_number].appname,
			sizeof(paramOut->returned_state[paramOut->response_number].appname));
	paramOut->returned_state[paramOut->response_number].no_appli = appliId;

	// TODO: Retrieve the state of the application
	paramOut->returned_state[paramOut->response_number].state.response = REP_OK;
	paramOut->response_number++;

	//// Parameters management
	// Parameters control checking
	if (!gs_bCaKeyCheckingDone) {
		Cless_Parameters_CheckCaKeysCrc(pTreeCurrentParam);
		Cless_Parameters_CreateSupportedCertificateListForKernel (pTreeCurrentParam, DEFAULT_EP_KERNEL_PAYPASS);
		Cless_Parameters_CreateSupportedCertificateListForKernel (pTreeCurrentParam, DEFAULT_EP_KERNEL_INTERAC);
		Cless_Parameters_CreateSupportedCertificateListForKernel (pTreeCurrentParam, DEFAULT_EP_KERNEL_DISCOVER_DPAS);
		Cless_Parameters_CreateSupportedCertificateListForKernel (pTreeCurrentParam, DEFAULT_EP_KERNEL_PURE);
		Cless_Parameters_BuildAIDNodes(pTreeCurrentParam);
		gs_bCaKeyCheckingDone = TRUE;

		// Here to be done just one time
		// Test if the VisaWave kernel is present
#ifndef DISABLE_OTHERS_KERNELS
		//		if(VisaWave_Clear() != KERNEL_STATUS_SERVICE_NOT_AVAILABLE)
		//			VisaWave_SetVisaWaveKernelPresence();

		Cless_VisaWave_SetVisaAsiaGuiMode();

		if(Cless_VisaWave_IsVisaAsiaGuiMode()) {
			if(Cless_VisaWave_GetNoCardTimeOut(&gs_ulNoCardTimeOut)) {
				// Set the timeout for the card detection process
				PSQ_read_state_param(&state_param);
				state_param.TimeOutWaitingCardAfterAmount = gs_ulNoCardTimeOut/10;
				PSQ_write_state_param(&state_param);
			}
		}
#endif
	}

	// Remove Application name on the header
	if (gs_nHeaderEventMask < 0) {
		gs_nHeaderEventMask = EventHeader (0); // To get the initial Header Event Mask and to set the current to 0
		EventHeader (gs_nHeaderEventMask); // Restore the Header Event mask
	}
	EventHeader (gs_nHeaderEventMask ^ _APPLI_STATE_);


	return FCT_OK;
}

//! \brief The Telium Manager calls this service each time it wants to delete an application.
//! \param[in] appliId The application ID.
//! \param[out] paramOut Output parameters that contains the application response:
//!      - DEL_YES : application authorises deletion process
//!      - DEL_NO :  application refuses any deletion process (Manager will display a message regarding the application which has refused the deletion)
//! \return Always \a FCT_OK.
//! \sa Telium Manager reference documentation.
int CLESS_ServiceManager_IsDelete(NO_SEGMENT appliId, S_DELETE *paramOut) {
	// To avoid warnings because 'appliId' is not used
	(void)appliId;

	// Initialise the output parameter
	memclr(paramOut, sizeof(*paramOut));

	// TODO: Check batch file status
	paramOut->deleting = DEL_YES;

	return FCT_OK;
}


/** Display a message "TPASS DLL NOT LOADED"
 */
static void Display_No_Dll_Msg (void) {
	MSGinfos tMsg;
	int lg_code;

	// Get the manager language (merchant language)
	lg_code = Cless_Term_GiveLangNumber((unsigned char *)GetManagerLanguage());

	Cless_Term_Read_Message(STD_MESS_DLL_TPASS, lg_code, &tMsg);
	Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_2, &tMsg, NOLEDSOFF);
	Cless_Term_Read_Message(STD_MESS_NOT_LOADED, lg_code, &tMsg);
	Helper_DisplayTextMerchant(NO_ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, NOLEDSOFF);

	Helper_RefreshScreen(WAIT, HELPERS_MERCHANT_SCREEN);
}

//! \brief The Telium Manager calls this service each time the application is selected with the "F" menu.
//! \param[in] appliId The application ID.
//! \return Always \a FCT_OK.
//! \sa Telium Manager reference documentation.
int CLESS_ServiceManager_MoreFunction(NO_SEGMENT appliId) {
	int iHeader, iFooter;
	int iRet, Manage_ApplicationResult = 0;

	// To avoid warnings because 'appliId' is not used
	(void)appliId;

	// Open peripherals
	// ****************
	iHeader = IsHeader();       // Save header state
	iFooter = IsFooter();       // Save footer state

	DisplayHeader(_OFF_);  // dissable Header
	DisplayFooter(_OFF_);  // dissable Footer

	DisplayHeader(_OFF_);  // dissable Header
	DisplayFooter(_OFF_);  // dissable Footer

	if (!IsColorDisplay()) {    // B&W terminal?
		// Yes
		DisplayLeds(_OFF_);     // Disable Leds
		DisplayHeader(_OFF_);   // Disable Header
		DisplayFooter(_OFF_);   // Disable Footer
	}

	iRet = GetPpdDisplay();     // Get pinpad type
	CHECK(iRet>=0, lblKO);
	if (iRet == 1){             // Pinpad with character display
		PPS_firstline();        // Display pinpad idle message
		PPS_Display("    WELCOME     ");
	}

	if (!g_bDllTpassLoaded)
		Display_No_Dll_Msg();

	//	Main Menu
	Manage_ApplicationResult = Manage_Application_Menu(hGoal);
	if (Manage_ApplicationResult == CLESS_CR_MANAGER_RESTART_DOUBLE_TAP) {
		goto lblCleanExit;
	}

	goto lblEnd;

	// Errors treatment
	// ****************
	lblKO:                       // None-classified low level error
	GL_Dialog_Message(hGoal, NULL, "Processing Error", GL_ICON_ERROR, GL_BUTTON_VALID, 5*1000);
	goto lblEnd;
	lblEnd:

	DisplayLeds(_ON_);       // Enable Leds
	DisplayHeader(iHeader);  // Restore Header
	DisplayFooter(iFooter);  // Restore Footer

	ClosePeripherals();

	lblCleanExit:
	return FCT_OK;
}

#ifdef __TELIUM3__
static T_SERVICE_CALL_SHARED_EXCHANGE_STRUCT* __GetSharedExchStructFromServiceCall(void *pData) {
	T_KERNEL_SERVICE_CALL_LOCAL_MEMORY *param = pData;

	if (pData != NULL) {

		if(param->sc.pDataStruct != NULL) {
			param->shared.nPtrData = param->data;
			param->sc.pDataStruct = &param->shared;
			return &param->sc;
		}
		else
			return NULL;
	}
	else
		return NULL;

}
#else
static T_SERVICE_CALL_SHARED_EXCHANGE_STRUCT* __GetSharedExchStructFromServiceCall(void *pData)
{
	return (T_SERVICE_CALL_SHARED_EXCHANGE_STRUCT *)pData;
}
#endif




#ifndef DISABLE_OTHERS_KERNELS

static int custom_selection_gui (unsigned int nSize, void * pData) {
	T_SERVICE_CALL_SHARED_EXCHANGE_STRUCT * pSharedStruct;

	(void)nSize;
	//	pSharedStruct = (T_SERVICE_CALL_SHARED_EXCHANGE_STRUCT *)pData;
	pSharedStruct = __GetSharedExchStructFromServiceCall(pData);

	if(Cless_VisaWave_IsVisaAsiaGuiMode())
		return (Cless_Customisation_SelectionGui(pSharedStruct->pDataStruct));
#ifndef DISABLE_INTERAC
	else if(Cless_Interac_IsInteracGuiMode())
		return (Cless_Customisation_Interac_SelectionGui(pSharedStruct->pDataStruct));
#endif
	else
		return(CLESS_CUST_DEFAULT);
}

static int custom_appli_selection_proc (unsigned int nSize, void * pData) {
	T_SHARED_DATA_STRUCT * pDataStruct;
	int result;

	(void)nSize;
	perflog("MG\tpW_CUST\tcustom_appli_selection_proc");
	pDataStruct = __GetSharedExchStructFromServiceCall(pData)->pDataStruct;

	result = (Cless_Customisation_CustAsProc(pDataStruct));
	perflog("MG\tpW_CUST\tExit custom_appli_selection_proc");
	return result;
}
/**
 * \brief This service is called by the contactless kernels for customisation processing.
 * This is the application that provide kernel the service and the application type to call for customisation.
 * \param[in] nSize Size of \a pData (not used as \a pData is a shared service call struct).
 * \param[in,out] pData Data buffer to be used to get and provide data to the kernel.
 * \return
 *  	- \ref FCT_OK always.
 */
static int custom_kernel_ (unsigned int nSize, void * pData) {
	int nResult;
	T_SHARED_DATA_STRUCT * pSharedStruct;

	(void) nSize;

	perflog("MG\tpW_CUST\tcustom_kernel_");
	pSharedStruct = __GetSharedExchStructFromServiceCall(pData)->pDataStruct;

	// Call the customisation depending on the kernel used
	nResult = Cless_Customisation_Process (pSharedStruct);

	perflog("MG\tpW_CUST\tExit custom_kernel_");
	return (nResult);
}

#endif

//! \brief The Telium Manager calls this service to get the name of the application.
//! \param[in] appliId The application ID.
//! \param[out] paramOut Output parameters that contains the application name amongst other things.
//! \return Always \a FCT_OK.
//! \sa Telium Manager reference documentation.
int CLESS_ServiceManager_IsName(NO_SEGMENT appliId, S_ETATOUT *paramOut) {
	ASSERT(paramOut != NULL);

	// Initialise the output parameter
	memclr(&paramOut->returned_state[paramOut->response_number],
			sizeof(paramOut->returned_state[paramOut->response_number]));

	// Get the name of the application
	__CLESS_ServiceManager_GetAppName(appliId, paramOut->returned_state[paramOut->response_number].appname,
			sizeof(paramOut->returned_state[paramOut->response_number].appname));
	paramOut->returned_state[paramOut->response_number].no_appli = appliId;
	paramOut->returned_state[paramOut->response_number].state.response = REP_OK;
	paramOut->response_number++;

	return FCT_OK;
}

//! \brief The Telium Manager calls this service to let the application to customise the Telium Manager behaviour.
//! \param[in] appliId The application ID.
//! \param[out] paramOut Output parameters.
//! \return Always \a FCT_OK.
//! \sa Telium Manager reference documentation.
int CLESS_ServiceManager_GiveYourDomain(NO_SEGMENT appliId, S_INITPARAMOUT *paramOut) {
	// To avoid warnings because 'appliId' is not used
	(void)appliId;

	ASSERT(paramOut != NULL);

	// Initialise the output parameter
	memclr(&paramOut->returned_state[paramOut->response_number],
			sizeof(paramOut->returned_state[paramOut->response_number]));

	// Allow Telium Manager to modify all the parameters
	// TODO: Set the value to your needs
	paramOut->returned_state[paramOut->response_number].mask = MSK_ALL_PARAM;

	// It is a standard application
	paramOut->returned_state[paramOut->response_number].application_type = TYP_EXPORT;
	paramOut->response_number++;

	return FCT_OK;
}

void RefreshDB(void){
	char tcAppDat[lenCmpDat+10], tcAppTim[lenCmpTim+10];
	char tcPath[100];
	Telium_Date_t xDate;
	doubleword uiMode;
	int iRet = 0;
	int retried = 0;
	char tcDate[lenDatTim + 1];

	RETRY:
	// Create Parameter Disk (See Mapapp.c)
	// ====================================
	// ** Parameters Disk used by FMG library **
	strcpy(xCfg.Label, PARAM_DISK);            // Disk name
	xCfg.Mode = FS_WRITEONCE;                  // Disk on Flash
	xCfg.AccessMode	= FS_WRTMOD;               // r/w access
	xCfg.NbFichierMax = 20;                     // Max files number
	xCfg.IdentZone	= FS_WO_ZONE_DATA;         // Zone id
	ulSize = xCfg.NbFichierMax*32768;          // Disk size in bytes

	// ** All disks created by FFMS and used by FMG library must be mounted before the call to FMG_init() **
	Telium_Sprintf(tcPath, "/%s", PARAM_DISK);
	iRet = FS_mount (tcPath, &uiMode);         // Check if disk already activated (!!! second parameter returns (FS_WRITEONCE or FS_WRITEMANY))

	if (iRet != FS_OK) {
		// == Application added ==
		iRet = FS_dskcreate(&xCfg, &ulSize);   // Create and format the none volatile disk
		CHECK(iRet==FS_OK, lblKO);
		iRet = FS_mount(tcPath, &uiMode);      // Activate a disk (!!! second parameter returns (FS_WRITEONCE or FS_WRITEMANY))
		CHECK(iRet==FS_OK, lblKO);
		iRet = FMG_Init();                     // Initialize File ManaGement
		CHECK(iRet==FMG_SUCCESS, lblKO);

		iRet = appReset();                     // Reset application parameters (Flash)
		CHECK(iRet>=0, lblKO);

		iRet = traReset();                     // Reset Transaction Buffers (Flash)
		CHECK(iRet>=0, lblKO);

		iRet = appPut(appCmpDat, (char*)getAppCmpDat(), lenCmpDat);  // Store compiler date/time (See Mapapp.c)
		CHECK(iRet>=0, lblKO);
		iRet = appPut(appCmpTim, (char*)getAppCmpTim(), lenCmpTim);
		CHECK(iRet>=0, lblKO);

		//		SqliteDB_Init();
		//		__Cless_Menu_EraseParameters ();
		//		__Cless_Menu_DefaultParameters();

	} else {

		iRet = FMG_Init();                                // Initialize File ManaGement
		CHECK(iRet==FMG_INIT_OK, lblKO);

		iRet = appGet(appCmpDat, tcAppDat, lenCmpDat+1);  // Retrieve compiler date/time (See Mapapp.c)
		CHECK(iRet>=0, lblKO);
		iRet = appGet(appCmpTim, tcAppTim, lenCmpTim+1);
		CHECK(iRet>=0, lblKO);
		// Check compiler date/time
		if( (strcmp(tcAppDat, getAppCmpDat()) == 0) && (strcmp(tcAppTim, getAppCmpTim()) == 0)) {
			// == Application already upto date ==
		} else {
			// == Application updated ==
			if ((strcmp(tcAppDat, getAppCmpDat()) != 0) || (strcmp(tcAppTim, getAppCmpTim()) != 0)) {
				iRet = appReset();                        // Reset application parameters (Flash)
				CHECK(iRet>=0, lblKO);

				iRet = traReset();                        // Reset Transaction Buffer (Flash)
				CHECK(iRet>=0, lblKO);
			}

			iRet = appPut(appCmpDat, (char*)getAppCmpDat(), lenCmpDat); // Store compiler date/time (See Mapapp.c)
			CHECK(iRet>=0, lblKO);
			iRet = appPut(appCmpTim, (char*)getAppCmpTim(), lenCmpTim);
			CHECK(iRet>=0, lblKO);

			//			SqliteDB_Init();
			//			__Cless_Menu_EraseParameters ();
			//			__Cless_Menu_DefaultParameters();
		}
	}


	goto lblEnd;

	// Errors treatment
	// ****************
	lblKO:
	traReset();                                     // Reset Transaction Buffer
	iRet = appReset();                              // Reset one more time

	if (iRet>=0) {                                  // Memory OK
		appPut(appCmpDat, (char*)getAppCmpDat(), lenCmpDat); // Store compiler date/time (See Mapapp.c)
		appPut(appCmpTim, (char*)getAppCmpTim(), lenCmpTim);

		//		SqliteDB_Init();
		//		__Cless_Menu_EraseParameters ();
		//		__Cless_Menu_DefaultParameters();

	} else {  // Memory problem => Terminal problem

		if(retried == 0){
			// delete the FS
			FS_format(tcPath,uiMode,10);

			//set flag as retied
			retried = 1;

			//redo the Init
			goto RETRY;
		}
	}

	goto lblEnd;
	lblEnd:

	fncReadConfigFile();

	memset(tcDate, 0, sizeof(tcDate));
	Telium_Read_date (&xDate);
	Telium_Sprintf (tcDate, "%2.2s%2.2s20%2.2s", xDate.day, xDate.month, xDate.year); // Retrieve date
	mapPut(appLastSettlementDate, tcDate, 0);

}

//! \brief The Telium Manager calls this service at startup.
//! \param[in] appliId The application ID.
//! \param[out] paramOut Output parameters that contains the application ID.
//! \return Always \a FCT_OK.
//! \sa Telium Manager reference documentation.
int CLESS_ServiceManager_AfterReset(NO_SEGMENT appliId, S_TRANSOUT *paramOut) {
	S_STATE_PARAM xParam;
	char tcAppDat[lenCmpDat+10], tcAppTim[lenCmpTim+10];
	char tcPath[100];
	byte ucNew, ucType;
	doubleword uiMode;  // Bug in prototype => 2nd parameter (output) on FS_mount returns Mode (FS_WRITEONCE or FS_WRITEMANY)
	// and NOT AccessMode (FS_NOFLAGS or FS_RONLYMOD or FS_WRTMOD)
	T_GL_COORD sValue;
	//	byte ucDum1;
	//	int iDum2=0;
	int i, iRet;
	int retried = 0;
	int LocalDisplay = 0;

	ASSERT(paramOut != NULL);

	// Security DLL open
	SEClib_Open();

#ifdef __TELIUM3__
	ClessEmv_IsDriverOpened();
#endif

	if (IsRadioWifi()) {

		wifilib_open(); //enable wifi
		if (Wifi_IsPowerOn()!=TRUE) {
			Wifi_PowerOn();
		}
		Wifi_SetBootproto(DLL_WIFI_BOOT_PROTO_DHCP);

	}

	g_bDllTpassLoaded = (TPass_IsLoaded() == ERR_TPASS_OK);	///< TPass DLL Library

	RETRY:
	// Initialization
	// **************
	hDsp = Telium_Stdperif("DISPLAY", NULL);   // Check "display" peripheral already opened? (done by Manager)
	if (hDsp == NULL) {                         // No, then open it
		LocalDisplay = 1;
		hDsp = Telium_Fopen("DISPLAY", "w*");  // Open "display" peripheral
		CHECK(hDsp!=NULL, lblKO);
	}

	// For IPP480 with dual reader (priority to the chip)
	// ==================================================
	PSQ_read_state_param (&xParam);
	if (IsIPP480() != 0) {
		if (PRIORITY_SMART)
			xParam.CamSwipePriority = CARD_EXTENDED_SUPPORT;
		else
			xParam.CamSwipePriority = SWIPE_EXTENDED_SUPPORT;
		PSQ_write_state_param (&xParam);       // If smart failed => Fall back to swipe out
	}

	// Check if Cold or Warm reset
	// ===========================
	first_init(appliId, &ucNew, &ucType);           // NEW SOFTWARE LOADED ?
	if (ucNew == 0xFF) {                       // YES, just loaded with first execution
		GL_Dialog_Message(hGoal, NULL, (char*) zColdReset, GL_ICON_INFORMATION, GL_BUTTON_NONE, 2*1000);
		raz_init(appliId);                          // Reset indicator downloading
	}
	else                                       // NO, already loaded and executed
		GL_Dialog_Message(hGoal, NULL, (char*) zWarmReset, GL_ICON_INFORMATION, GL_BUTTON_NONE, 2*1000);

	// Check if pictures are present in HOST for animation
	// ===================================================
	for (i=0; i<11; i++) {
		if (IsColorDisplay())                  // Color terminal?
			Telium_Sprintf(tcPath, "file://flash/HOST/CCARDT%d.JPG", i);  // Yes, check if JPG present
		else
			Telium_Sprintf(tcPath, "file://flash/HOST/BWCARDT%d.BMP", i); // No, check if BMP present

		iRet = GL_File_Exists(tcPath);         // Picture present?
		if (iRet==GL_SUCCESS)
			bPictureOk=true;                   // Yes, set the flag to yes
		else {
			bPictureOk=false;                  // No, set the flag to no
			break;                             // and exit
		}
	}

	// Create Parameter Disk (See Mapapp.c)
	// ====================================
	// ** Parameters Disk used by FMG library **
	strcpy(xCfg.Label, PARAM_DISK);            // Disk name
	xCfg.Mode = FS_WRITEONCE;                  // Disk on Flash
	xCfg.AccessMode	= FS_WRTMOD;               // r/w access
	xCfg.NbFichierMax = 20;                     // Max files number
	xCfg.IdentZone	= FS_WO_ZONE_DATA;         // Zone id
	ulSize = xCfg.NbFichierMax*32768;          // Disk size in bytes

	// ** All disks created by FFMS and used by FMG library must be mounted before the call to FMG_init() **
	Telium_Sprintf(tcPath, "/%s", PARAM_DISK);
	iRet = FS_mount (tcPath, &uiMode);         // Check if disk already activated (!!! second parameter returns (FS_WRITEONCE or FS_WRITEMANY))

	if (iRet != FS_OK) {
		// == Application added ==
		iRet = FS_dskcreate(&xCfg, &ulSize);   // Create and format the none volatile disk
		CHECK(iRet==FS_OK, lblKO);
		iRet = FS_mount(tcPath, &uiMode);      // Activate a disk (!!! second parameter returns (FS_WRITEONCE or FS_WRITEMANY))
		CHECK(iRet==FS_OK, lblKO);
		iRet = FMG_Init();                     // Initialize File ManaGement
		CHECK(iRet==FMG_SUCCESS, lblKO);

		iRet = appReset();                     // Reset application parameters (Flash)
		CHECK(iRet>=0, lblKO);

		iRet = traReset();                     // Reset Transaction Buffers (Flash)
		CHECK(iRet>=0, lblKO);

		iRet = appPut(appCmpDat, (char*)getAppCmpDat(), lenCmpDat);  // Store compiler date/time (See Mapapp.c)
		CHECK(iRet>=0, lblKO);
		iRet = appPut(appCmpTim, (char*)getAppCmpTim(), lenCmpTim);
		CHECK(iRet>=0, lblKO);

		GL_Dialog_Progress(hGoal, NULL, "New Software\nReset In Progress", NULL, NULL, 0, 100, Progress, GL_BUTTON_NONE, GL_TIME_INFINITE);
		sValue = 100;
		SqliteDB_Init();
		__Cless_Menu_EraseParameters ();
		__Cless_Menu_DefaultParameters();
		GL_Dialog_Progress(hGoal, NULL, "New Software\nReset Done", NULL, &sValue, 1, 101, NULL, GL_BUTTON_NONE, 3*1000);

	} else {

		iRet = FMG_Init();                                // Initialize File ManaGement
		CHECK(iRet==FMG_INIT_OK, lblKO);

		iRet = appGet(appCmpDat, tcAppDat, lenCmpDat+1);  // Retrieve compiler date/time (See Mapapp.c)
		CHECK(iRet>=0, lblKO);
		iRet = appGet(appCmpTim, tcAppTim, lenCmpTim+1);
		CHECK(iRet>=0, lblKO);
		// Check compiler date/time
		if( (strcmp(tcAppDat, getAppCmpDat()) == 0) && (strcmp(tcAppTim, getAppCmpTim()) == 0)) {
			// == Application already upto date ==
		} else {
			// == Application updated ==
			if ((strcmp(tcAppDat, getAppCmpDat()) != 0) || (strcmp(tcAppTim, getAppCmpTim()) != 0)) {
				iRet = appReset();                        // Reset application parameters (Flash)
				CHECK(iRet>=0, lblKO);

				iRet = traReset();                        // Reset Transaction Buffer (Flash)
				CHECK(iRet>=0, lblKO);
			}

			iRet = appPut(appCmpDat, (char*)getAppCmpDat(), lenCmpDat); // Store compiler date/time (See Mapapp.c)
			CHECK(iRet>=0, lblKO);
			iRet = appPut(appCmpTim, (char*)getAppCmpTim(), lenCmpTim);
			CHECK(iRet>=0, lblKO);

			GL_Dialog_Progress(hGoal, NULL, "Update Software\nUpdate In Progress",
					NULL, NULL, 0, 100, Progress, GL_BUTTON_NONE, GL_TIME_INFINITE);
			sValue = 100;
			SqliteDB_Init();
			__Cless_Menu_EraseParameters ();
			__Cless_Menu_DefaultParameters();
			GL_Dialog_Progress(hGoal, NULL, "Update Software\nUpdate Done", NULL, &sValue, 1, 101, NULL, GL_BUTTON_NONE, 3*1000);
		}
	}

	//	// Create Queue, Shared Memory and Semaphore objects shared between Training/User2 applications
	//	// ********************************************************************************************
	//	hQueueS = OSL_MessageQueue_Create("QUEUE_S", OSL_OPEN_CREATE, OSL_SECURITY_SHARED); // Create a message queue object (security shared)
	//	hMemS = OSL_SharedMem_Create("MEM_S", 1024, OSL_OPEN_CREATE, OSL_SECURITY_SHARED);  // Create a shared memory object (security shared)
	//	hSemS = OSL_Semaphore_Create("SEM_S", 0, OSL_OPEN_CREATE, OSL_SECURITY_SHARED);     // Create a semaphore object (security shared)
	//	hSemS2 = OSL_Semaphore_Create("SEM_S2", 0, OSL_OPEN_CREATE, OSL_SECURITY_SHARED);   // Create a second semaphore object used for mutex (security shared)

	// Fork a task used for IAM
	// ************************
	usMainTaskNbr = Telium_CurrentTask();          // Retrieve main task number
	//	Telium_Fork(Task2, &ucDum1, iDum2);            // Fork second task
	//	iRet = Telium_Ttestall(SYNCHRO_DIALOG, 1*100); // Wait until second task started (Synchronization)
	//
	//	if (iRet==0) {
	//		buzzer(10);
	//		GL_Dialog_Message(hGoal, NULL, "Task 2 Failed\nTRAINING", GL_ICON_WARNING, GL_BUTTON_NONE, 1*1000);
	//	}

	// Clear the output parameter
	memclr(paramOut, sizeof(*paramOut));
	paramOut->noappli = appliId;

	goto lblEnd;

	// Errors treatment
	// ****************
	lblKO:
	traReset();                                     // Reset Transaction Buffer
	iRet = appReset();                              // Reset one more time

	if (iRet>=0) {                                  // Memory OK
		appPut(appCmpDat, (char*)getAppCmpDat(), lenCmpDat); // Store compiler date/time (See Mapapp.c)
		appPut(appCmpTim, (char*)getAppCmpTim(), lenCmpTim);

		GL_Dialog_Progress(hGoal, NULL, "Initialization\nReset In Progress", NULL, NULL, 0, 100, Progress, GL_BUTTON_NONE, GL_TIME_INFINITE);
		sValue = 100;
		SqliteDB_Init();
		__Cless_Menu_EraseParameters ();
		__Cless_Menu_DefaultParameters();
		GL_Dialog_Progress(hGoal, NULL, "Initialization\nReset Done", NULL, &sValue, 1, 101, NULL, GL_BUTTON_NONE, 3*1000);

	} else {  // Memory problem => Terminal problem

		GL_Dialog_Message(hGoal, NULL, "Processing Error\nMemory Failed", GL_ICON_ERROR, GL_BUTTON_NONE, 5*1000);
		if(retried == 0){
			// delete the FS
			FS_format(tcPath,uiMode,10);

			//set flag as retied
			retried = 1;

			//redo the Init
			goto RETRY;
		}
	}

	goto lblEnd;
	lblEnd:
	/////
	/// CLESS
	///
	// Try to restore parameters from sav file
	if (Cless_Parameters_RestoreParam (&pTreeCurrentParam) != 0) {
		GTL_Traces_TraceDebug("after_reset_ : Cless_Parameters_RestoreParam ko");

		// Read default parameters
		Cless_Parameters_ReadDefaultParameters(&pTreeDefaultParam);

		if (Cless_Parameters_SaveParam (pTreeDefaultParam) != 0) {
			GTL_Traces_TraceDebug("after_reset_ : Cless_Parameters_SaveParam ko");
		} else { // Parameter has been correctly loaded/saved
			Cless_Parameters_InitParameters(&pTreeCurrentParam);
			pTreeCurrentParam = pTreeDefaultParam; // Update pTreeCurrentParam for later use...
			pTreeDefaultParam = NULL;
		}
	}

	// Create or restore the batch
	Cless_Batch_Restore ();

	Cless_Disk_Open_MyDisk();

	// Try to load black list parameters if existing
	Cless_BlackListLoad();

	// Indicate CA checking has to be done
	gs_bCaKeyCheckingDone = FALSE;

	// Init the transaction sequence counter
	Cless_Batch_InitTransactionSeqCounter(FALSE);

	Test_DLL_Security_Presence();

	Cless_Term_Initialise();

	Cless_DumpData_InitOutput();

	// Set the LEDs into the idle state
	HelperLedsIdleState();

	Cless_Goal_Init();

	//////
	/// End CLESS
	///
	appPut(appAppName,zAppName,strlen(zAppName));
	if(hDsp) {
		if (LocalDisplay == 1) {
			Telium_Fclose(hDsp);                       // Close "display" peripheral
			hDsp=NULL;
		}
	}

	return FCT_OK;
}

//! \brief The entry point for all the Telium Manager services.
//! \param[in] size Size of the data \a data.
//! \param[in,out] data Data related to the called service.
static int __CLESS_ServicesManager(unsigned int size, StructPt *data) {
	NO_SEGMENT appliId;
	int result;

	// To avoid warnings because 'size' is not used
	(void)size;

	// Get the application number
	appliId = (NO_SEGMENT)ApplicationGetCurrent();
	// Execute the requested service
	switch(data->service) {

	///// -----------------------    EMV    ----------------------------

	case IDLE_MESSAGE :                   // Activated when Manager goes back to idle, to display its message
		result = idle_message(appliId, NULL, NULL);
		break;

	case KEYBOARD_EVENT :                 // Activated when key is pressed
		result = keyboard_event(appliId, &data->Param.KeyboardEvent.param_in, &data->Param.KeyboardEvent.param_out);
		break;

	case GET_IDLE_STATE_PARAMETER :       // Gives manager parameters to customize idle state
		result = get_idle_state_parameter(appliId, NULL, &data->Param.GetIdleStateParameter.param_out);
		break;

	case GIVE_MONEY_EXTENDED :            // Get the currencies supported by the application
		result = give_money_extended(appliId, NULL, &data->Param.GiveMoneyExtended.param_out);
		break;

	case STATE :                          // Activated on "F" key: Consultation->State, to print terminal content receipt
		result = state(appliId, NULL, NULL);
		break;

	case CONSULT :                        // Activated on "F" key: Consultation->Transactions, to print transaction totals receipt
		result = consult(appliId, NULL, NULL);
		break;

	case MCALL :                          // Activated on "F" key: Consultation->Call->Planning of Call, to print host call planning receipt
		result = mcall(appliId, NULL, NULL);
		break;

	case IS_TIME_FUNCTION :               // Activated every minute, do you need the peripherals at the next call of time_function()?
		result = is_time_function(appliId, NULL, &data->Param.IsTimeFunction.param_out);
		break;

	case TIME_FUNCTION :                  // Activated every minute, to execute automatic periodic functions
		result = time_function(appliId, NULL, NULL);
		break;

	case IS_CHANGE_INIT :                 // Activated on "F" key: Initialization->Parameters->List, Conditions for changing Manager parameters?
		result = is_change_init(appliId,NULL,&data->Param.IsChangeInit.param_out);
		break;

	case MODIF_PARAM :                    // Activated on "F" key: Initialization->Parameters->List, Manager reports parameters changed.
		result = modif_param(appliId, &data->Param.ModifParam.param_in, NULL);
		break;

	case IS_EVOL_PG :                     // Activated on "F" key: Evolution->Load->Local or RemoteLoad, Conditions for application downloading?
		result = is_evol_pg(appliId,NULL,&data->Param.IsEvolPg.param_out);
		break;

	case FILE_RECEIVED :                  // Activated each time Manager received a file from a "parameters" downloading session
		result = file_received(appliId, &data->Param.FileReceived.param_in, NULL);
		break;

	case MESSAGE_RECEIVED:                // Activated each time Manager received a message in its own mailbox for this application
		result = message_received(appliId, &data->Param.MessageReceived.param_in, NULL);
		break;

	case IS_CARD_SPECIFIC:                // Activated when card inserted card swiped or manually entry, do you want to process the card?
		//		result = is_card_specific(appliId, &data->Param.IsCardSpecific.param_in, &data->Param.IsCardSpecific.param_out);
		result = REP_KO;
		break;

	case CARD_INSIDE:		              // Activated when the card is specific, the application process the card in transparent mode
		result = card_inside(appliId, &data->Param.CardInside.param_in, &data->Param.CardInside. param_out);
		break;

	case IS_FOR_YOU_BEFORE:               // Activated when chip card inserted, ask application to recognise the chip card in order to a candidate
		result = is_for_you_before(appliId, &data->Param.IsForYouBefore.param_in, &data->Param.IsForYouBefore.param_out);
		break;

	case IS_FOR_YOU_AFTER :
		result = is_for_you_after(appliId,&data->Param.IsForYouAfter.param_in,&data->Param.IsForYouAfter.param_out);
		break;

	case DEBIT_NON_EMV :
		result = debit_non_emv(appliId,&data->Param.DebitNonEmv.param_in,&data->Param.DebitNonEmv.param_out);
		break;

	case SELECT_FUNCTION:
		result = select_function(appliId,&data->Param.SelectFunction.param_in,&data->Param.SelectFunction.param_out);
		break;

	case FALL_BACK:
		result = fall_back(appliId,&data->Param.FallBack.param_in,&data->Param.FallBack.param_out);
		break;

	case GIVE_AID:
		result = give_aid(appliId,&data->Param.GiveAid.param_in,&data->Param.GiveAid.param_out);
		break;

	case IS_CARD_EMV_FOR_YOU:
		result = is_card_emv_for_you(appliId,&data->Param.IsCardEmvForYou.param_in,&data->Param.IsCardEmvForYou.param_out);
		break;

	case DEBIT_EMV:
		result = debit_emv(appliId,&data->Param.DebitEmv.param_in,&data->Param.DebitEmv.param_out);
		break;


		///// -----------------------    CLESS BASED    ----------------------------
	case AFTER_RESET:
		result = CLESS_ServiceManager_AfterReset(appliId, &data->Param.AfterReset.param_out);
		fncReadConfigFile();
		break;

	case IS_NAME:
		result = CLESS_ServiceManager_IsName(appliId, &data->Param.IsName.param_out);
		break;

	case GIVE_YOUR_SPECIFIC_CONTEXT:
		result = CLESS_ServicesManager_GiveYourSpecificContext(appliId, &data->Param.GiveYourSpecificContext.param_out);
		break;

	case IS_DELETE:
		result = CLESS_ServiceManager_IsDelete(appliId, &data->Param.IsDelete.param_out);
		break;

	case GIVE_YOUR_DOMAIN:
		result = CLESS_ServiceManager_GiveYourDomain(appliId, &data->Param.GiveYourType.param_out);
		break;

	case IS_STATE:
		result = CLESS_ServiceManager_IsState(appliId, &data->Param.IsState.param_out);
		break;

		///// -----------------------    OTHER    ----------------------------
	case MORE_FUNCTION:
		result = CLESS_ServiceManager_MoreFunction(appliId);
		break;

	case TIME_FUNCTION_CHAINE :           // French Bank Domain
	case GIVE_INFOS_CX:		              // French Bank Domain
	case DEBIT_OVER:
	case AUTO_OVER:
	case IS_ORDER:			              // French Health Care Domain
	case ORDER:				              // French Health Care Domain
	case IS_SUPPR_PG:		              // French Health Care Domain
	case IS_INSTALL_PG:		              // French Health Care Domain
	case GET_ORDER:			              // French Health Care Domain
	case IS_LIBELLE:		              // French Health Care Domain
	case EVOL_CONFIG:		              // French Bank Domain
	case GIVE_MONEY:		              // French Bank Domain
	case COM_EVENT:
	case MODEM_EVENT:
	case GIVE_INTERFACE:
	case IS_BIN_CB:			              // French Bank Domain
	case SELECT_FUNCTION_EMV:	          // French Bank Domain
	default:
		result = FCT_OK;
		ASSERT(FALSE);
		break;
	}

	Cless_Goal_Destroy();
	return result;
}

//! \brief Retrieves the name of the application.
//! \param[in] appliId The application ID.
//! \param[out] name Buffer of size \a nameBufferSize which will contain the application name.
//! \param[in] nameBufferSize The size of the \a name buffer.
static void __CLESS_ServiceManager_GetAppName(NO_SEGMENT appliId, char *name, int nameBufferSize) {
	// To avoid warnings because 'nameBufferSize' is not used
	(void)nameBufferSize;

	ASSERT(name != NULL);
	ASSERT(nameBufferSize >= 0);

	// Build the application value
	// TODO: Set your application name
	VERIFY(Telium_Sprintf(name, "BSEAPP Merchant") < nameBufferSize);
}



/**
 * ¨Get the contactless information for transaction.
 * \param    param_out (-O)
 * \param    param_out (-O)
 *    - rc_payment :
 *      PAY_OK (Transaction done), PAY_KO (Transaction rejected)
 * \return
 *  FCT_OK
 * \header sdk30.h
 * \source entry.c
 */
static int cless_give_info_ (NO_SEGMENT no, S_TRANSIN * param_in, S_CLESS_GIVEINFO * param_out)
{
	unsigned char ucCurrencyCode[2];
	int nTransactionType;
	int nTransactionCashBack;
	unsigned long ulAmountOtherBin = 0;

	(void) no;

	perflog("MG\tpW_CUST\tcless_give_info_");

	nTransactionType = 0;
	nTransactionCashBack = 0;

	Cless_Customisation_SetUsedPaymentScheme (CLESS_SAMPLE_CUST_UNKNOWN);

	// Prepare maximum transaction data for kernel purpose before card presentation
	// Input parameters
	if (!Cless_Fill_InitSharedBufferWithKernels (&g_pKernelSharedBuffer)) {
		GTL_Traces_TraceDebug("cless_give_info_ : Cless_Fill_InitSharedBufferWithKernels failed");
		goto End;
	}

	// Add transaction data (amount, date, time, etc).
	ucCurrencyCode[0] = param_in->currency.code[0] - 0x30;
	ucCurrencyCode[1] = ((param_in->currency.code[1] - 0x30) << 4) + (param_in->currency.code[2] - 0x30);

	if (Cless_Menu_IsEnableInputTransactionType()) { // Is transactionType must be inputed
		// Input nTransactionType
		Cless_Menu_TransactionTypeMenuDisplay(param_in->currency.nom, &(param_in->amount), &ulAmountOtherBin, &nTransactionType, &nTransactionCashBack);

		// Anti-bug:
		if (ulAmountOtherBin > param_in->amount) {
			MSGinfos tMsg;
			int nMerchLang;

			// Display : bad amount other inputed for Cless transaction
			nMerchLang = PSQ_Give_Language();

			Cless_Term_Read_Message(STD_MESS_ERROR, nMerchLang, &tMsg);
			Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_2, &tMsg, LEDSOFF);
			Helper_RefreshScreen(WAIT, HELPERS_MERCHANT_SCREEN);

			nTransactionType = -1; // Error
		}
	} else {
		// Get the transaction type
		if (param_in->transaction == CREDIT_TR)
			nTransactionType = CLESS_SAMPLE_TRANSACTION_TYPE_REFUND;
		else
			nTransactionType = CLESS_SAMPLE_TRANSACTION_TYPE_DEBIT;
	}

	// Error management
	if (nTransactionType == -1)
		goto End;

	// Save CLESS_GIVE_INFO input data for future use
	Cless_Fill_PrepareAndSaveClessGiveInfoTransactionData ((param_in->entry != NO_ENTRY), (unsigned char)nTransactionType, (unsigned char)nTransactionCashBack, ucCurrencyCode, param_in->currency.posdec, param_in->currency.nom, param_in->amount, ulAmountOtherBin);

	// Fill the output structure with the data to be used for application selection
	if (!Cless_Fill_GiveInfo (param_out, &pTreeCurrentParam)) {
		GTL_Traces_TraceDebug("cless_give_info_ : Cless_Fill_GiveInfo failed");
		goto End;
	}

#ifndef DISABLE_OTHERS_KERNELS
	if (Cless_VisaWave_IsVisaAsiaGuiMode()) {
		// Start timer
		VisaWave_StartDetectionTimer();
		Cless_VisaWave_GetLastTransactionData();
	}

#ifndef DISABLE_INTERAC
	if(Cless_Interac_IsInteracGuiMode()) {
		// Initialize the card detection timer
		Interac_StartDetectionTimer();
	}
#endif

#endif

	End:
	perflog("MG\tpW_CUST\tExit cless_give_info_");
	return (FCT_OK);
}



/**
 * ¨Process a Cless debit with AID already selected.
 * \param    param_out (-O)
 * \param    param_out (-O)
 *    - rc_payment :
 *      PAY_OK (Transaction done), PAY_KO (Transaction rejected)
 * \return
 *  FCT_OK
 * \header sdk30.h
 * \source entry.c
 */
static int cless_debit_aid_ (NO_SEGMENT no, unsigned int nSize, void * pData) {
	// Specific variables to manage share buffer
	int nResult = C_CLESS_CR_END;
	int nKernelToUse;
	T_SHARED_DATA_STRUCT * pSharedStruct;


	(void) no;
	(void) nSize;

	perflog("MG\tpW_CUST\tcless_debit_aid_");

#ifndef DISABLE_OTHERS_KERNELS
	VisaWave_UnsetpayWaveFallBack();
#endif
	Cless_payWave_SetQVSDCNotAllowedForAmount(FALSE);
	perflog("MG\tpW_CUST\tcless_debit_aid_ 01");


#ifndef DISABLE_OTHERS_KERNELS
	if(Cless_VisaWave_IsVisaAsiaGuiMode()) {
		// "No Card" message will not be displayed
		VisaWave_SetDisplayNoCard(FALSE);
	}

#ifndef DISABLE_INTERAC
	if(Cless_Interac_IsInteracGuiMode()) {
		// "No Card" message will not be displayed
		Interac_SetDisplayNoCard(FALSE);
	}
#endif
#endif

#ifndef DISABLE_OTHERS_KERNELS
	Restart:
#endif

	// Input parameters
	pSharedStruct = __GetSharedExchStructFromServiceCall(pData)->pDataStruct;
	perflog("MG\tpW_CUST\tcless_debit_aid_ 02");

	// Indicates the cless kernel is not yet known
	Cless_Customisation_SetUsedPaymentScheme (CLESS_SAMPLE_CUST_UNKNOWN);
	perflog("MG\tpW_CUST\tcless_debit_aid_ 03");

	// Input parameters (g_pKernelSharedBuffer has been initialised in the CLESS_GIVE_INFO service, just clear it)
	GTL_SharedExchange_ClearEx (g_pKernelSharedBuffer, FALSE);

	perflog("MG\tpW_CUST\tcless_debit_aid_ 10");
	// Fill the shared buffer with the transaction data
	if (!Cless_Fill_TransactionGenericData (g_pKernelSharedBuffer)) {
		GTL_Traces_TraceDebug("cless_debit_aid_ : Cless_Fill_TransactionGenericData failed");
		perflog("MG\tpW_CUST\tExit cless_debit_aid_ 1");
		return (nResult);
	}
	perflog("MG\tpW_CUST\tcless_debit_aid_ 11");

	// Dump data provided on CLESS_DEBIT_AID service (content of shared buffer)
	// Cless_DumpData_DumpSharedBufferWithTitle(pSharedStruct, "CLESS_DEBIT_AID");

	// Fill buffer with AID related data
	if (!Cless_Fill_AidRelatedData (g_pKernelSharedBuffer, pSharedStruct, &nKernelToUse)) {
		GTL_Traces_TraceDebug("cless_debit_aid_ : Cless_Fill_AidRelatedData failed");
		perflog("MG\tpW_CUST\tExit cless_debit_aid_ 2");
		return (nResult);
	}

	perflog("MG\tpW_CUST\tcless_debit_aid_ 12");
	if (Cless_payWave_IsQVSDCNotAllowedForAmount()) {
		GTL_Traces_TraceDebug("cless_debit_aid_ : Cless_Fill_AidRelatedData failed Amount greater than TAG_EP_CLESS_TRANSACTION_LIMIT");
		perflog("MG\tpW_CUST\tExit cless_debit_aid_ 3");
		return (CLESS_CR_MANAGER_REMOVE_AID);
	}

#ifndef DISABLE_INTERAC
	if ((gs_bInteracTxnUnderway) && (nKernelToUse != DEFAULT_EP_KERNEL_INTERAC)) {
		gs_bInteracTxnUnderway = FALSE;
		// clear Interac kernel transaction data
		Interac_Clear ();
	}
#endif // DISABLE_OTHERS_KERNELS

	mapPutCard(traCardKernel, nKernelToUse);
	perflog("MG\tpW_CUST\tcless_debit_aid_ 13");
	// Call kernel in relationship with AID
	switch (nKernelToUse) {
#ifndef DISABLE_OTHERS_KERNELS
	case DEFAULT_EP_KERNEL_PAYPASS :
		// Clear input data for next call
		GTL_SharedExchange_Clear(pSharedStruct);

		nResult = Cless_PayPass_PerformTransaction(g_pKernelSharedBuffer);
		break;
#endif // DISABLE_OTHERS_KERNELS

	case DEFAULT_EP_KERNEL_VISA :
#ifndef DISABLE_OTHERS_KERNELS
		if(VisaWave_isVisaWaveProcessing()) {
			nResult = Cless_VisaWave_PerformTransaction(g_pKernelSharedBuffer);
			if(nResult == VISAWAVE_STATUS_PAYWAVE_CARD) {
				VisaWave_SetpayWaveFallBack();
				goto Restart;
			} else {
				// Clear input data for next call
				GTL_SharedExchange_Clear(pSharedStruct);
			}
		} else {
			GTL_SharedExchange_Clear(pSharedStruct);
			nResult = Cless_payWave_PerformTransaction(g_pKernelSharedBuffer);
		}

#else // DISABLE_OTHERS_KERNELS
		GTL_SharedExchange_Clear(pSharedStruct);
		nResult = Cless_payWave_PerformTransaction(g_pKernelSharedBuffer);
#endif // DISABLE_OTHERS_KERNELS
		break;

	case DEFAULT_EP_KERNEL_QUICKPASS :
		GTL_SharedExchange_Clear(pSharedStruct);
		nResult = Cless_QuickPass_PerformTransaction(g_pKernelSharedBuffer);
		break;

#ifndef DISABLE_OTHERS_KERNELS
	case DEFAULT_EP_KERNEL_AMEX :
		GTL_SharedExchange_Clear(pSharedStruct);
		nResult = Cless_ExpressPay_PerformTransaction(g_pKernelSharedBuffer);
		break;

	case DEFAULT_EP_KERNEL_DISCOVER :
		GTL_SharedExchange_Clear(pSharedStruct);
		nResult = Cless_Discover_PerformTransaction(g_pKernelSharedBuffer);
		break;

#ifndef DISABLE_INTERAC
	case DEFAULT_EP_KERNEL_INTERAC :
		GTL_SharedExchange_Clear(pSharedStruct);
		nResult = Cless_Interac_PerformTransaction(g_pKernelSharedBuffer);

		if (nResult == CLESS_CR_MANAGER_RESTART_NO_MESSAGE_BEFORE_RETRY) {
			gs_bInteracTxnUnderway = TRUE;
		}
		break;
#endif

	case DEFAULT_EP_KERNEL_DISCOVER_DPAS :
		GTL_SharedExchange_Clear(pSharedStruct);
		nResult = Cless_DiscoverDPAS_PerformTransaction(g_pKernelSharedBuffer);
		break;

#ifndef DISABLE_PURE
	case DEFAULT_EP_KERNEL_PURE :
		GTL_SharedExchange_Clear(pSharedStruct);
		nResult = Cless_PURE_PerformTransaction(g_pKernelSharedBuffer);
		break;
#endif
#endif

	default:
		GTL_Traces_TraceDebug("Unknown Kernel to use in cless_debit_aid() : %x", nKernelToUse);
		break;
	}

	perflog("MG\tpW_CUST\tExit cless_debit_aid_");
	return (nResult);
}



/**
 * \brief This entry point is called after the contact’ less transaction (or when a contact transaction is initiated).
 * It can be used to close peripheral (that had been opened in the CLESS_GIVE_INFO entry point for example) or doing
 * any other treatments.
 * \param[in] nAppliNum Application Number.
 * \return
 *  	- \ref FCT_OK always.
 */
static int cless_end_ (NO_SEGMENT nAppliNum) {
	(void) nAppliNum;
#ifndef DISABLE_OTHERS_KERNELS
	MSGinfos tMsg;
	int nTimeout, nLang;

	if(Cless_VisaWave_IsVisaAsiaGuiMode() && VisaWave_GetDisplayNoCard()) {
		nLang = PSQ_Give_Language();

		nTimeout = GTL_StdTimer_GetRemaining(VisaWave_GetDetectionTimer(), gs_ulNoCardTimeOut/10);
		if (nTimeout == 0) {
			Cless_Term_Read_Message(VISAWAVE_MESS_NO_CARD, nLang, &tMsg);
			Helper_DisplayTextCustomer(ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, LEDSOFF);
			Helper_RefreshScreen(WAIT, HELPERS_CUSTOMER_SCREEN);
		}
	}

#ifndef DISABLE_INTERAC
	if(Cless_Interac_IsInteracGuiMode() && Interac_GetDisplayNoCard()) {
		nLang = PSQ_Give_Language();

		nTimeout = GTL_StdTimer_GetRemaining(Interac_GetDetectionTimer(), gs_ulNoCardTimeOut/10);
		if (nTimeout == 0) {
			HelperErrorSequence (WITHBEEP);
			Cless_Term_Read_Message(INTERAC_MESS_NO_CARD, nLang, &tMsg);
			Helper_DisplayTextCustomer(ERASE, HELPERS_CUSTOMER_LINE_2, &tMsg, Cless_ALIGN_CENTER, LEDSOFF);
			Helper_RefreshScreen(WAIT, HELPERS_CUSTOMER_SCREEN);
		}
	}
#endif

#ifndef DISABLE_INTERAC
	if (gs_bInteracTxnUnderway) {
		gs_bInteracTxnUnderway = FALSE;
		// clear Interac kernel transaction data
		Interac_Clear ();
	}
#endif
#endif

	Cless_Customisation_SetDoubleTapInProgress(FALSE);

	perflog("MG\tpW_CUST\tcless_end_");
	// Set the LEDs into the idle state
	HelperLedsIdleState();

	perflog("MG\tpW_CUST\tExit cless_end_");
	perflog_dump();
	return (FCT_OK);
}




//! \brief The entry point for all the Contactless services.
//! \param[in] size Size of the data \a data.
//! \param[in,out] data Data related to the called service.
static int __CLESS_ServicesCless(unsigned int size, StructPt *data) {
	NO_SEGMENT appliId;
	int result;

	// To avoid warnings because 'size' is not used
	(void)size;

	// Get the application number
	appliId = (NO_SEGMENT)ApplicationGetCurrent();
	// Execute the requested service
	switch(data->service) {
	// Implicit Selection Services
	case CLESS_GIVE_INFO:
		result = cless_give_info_ (appliId, &data->Param.ClessGiveInfo.param_in, &data->Param.ClessGiveInfo.param_out);
		break;

	case CLESS_DEBIT_AID:
		result = cless_debit_aid_ (appliId, size, data);
		break;

	case CLESS_END:
		result = cless_end_ (appliId);
		break;

	case CLESS_SERVICE_CUST_IMPSEL_GUI:
		result = custom_selection_gui(size, data);
		break;

	case CLESS_SERVICE_CUST_IMPSEL_AS_PROC:
		result = custom_appli_selection_proc(size, data);
		break;

		// Kernel services   CLESS_ServicesKernel_Custom
	case SERVICE_CUSTOM_KERNEL:
		result = custom_kernel_(size, data);
		break;


	default:
		result = FCT_OK;
		ASSERT(FALSE);
		break;
	}

	return result;
}


////////////////  Added later  //////////////////

void IdleImageDisplay(void){

	// Local variables *
	// *****************
	T_GL_HWIDGET hPicture=NULL;
	Telium_Date_t xDate;
	char tcDisplay[100];
	char tcPath[100];
	char tcDate[10+1];
	char tcTime[5+1];
	word  usCt;
	int LocalDisplay = 0;

	fncWriteStatusOfConnection('0');//Notify TMS transaction is in session

	//Cless_Goal_IsAvailable();

	memset(tcDisplay, 0, sizeof(tcDisplay));

	// Idle message management
	// ***********************
	hDsp = Telium_Stdperif("DISPLAY", NULL);                                 // Check "display" peripheral already opened? (done by Manager)
	if (hDsp == NULL) {                                                      // No, then open it
		hDsp = Telium_Fopen("DISPLAY", "w*");                                // Open "display" peripheral
		LocalDisplay = 1;
	}
	if (hDsp != NULL) {
		Telium_Read_date (&xDate);                                           // Read terminal Date&Time
		Telium_Sprintf (tcDate, "%2.2s/%2.2s/20%2.2s",                              // Retrieve date
				xDate.day, xDate.month, xDate.year);
		Telium_Sprintf (tcTime, "%2.2s:%2.2s",                                      // Retrieve time
				xDate.hour, xDate.minute);
		// *** Show idle screen ***
		hPicture = GoalCreatePicture(hGoal);                             // Create and clear screen
		GoalClrPicture(hPicture, GL_COLOR_BLACK, 0, false);

		usCt=0;
		do {
			if (usCt==0)                                                 // When counter==0, change image to display
			{

				Telium_Sprintf(tcDisplay, "%s              %s", tcDate, tcTime);

				strcpy(tcPath, "file://flash/HOST/COL_BSEAPP.JPG");
				//				if (IsHeader()==0) {                                      // No header (show Date&Time on user screen)
				//					GoalDspPicture(hPicture, tcDisplay, tcPath, &xImage, 0, true);
				//				} else

				GoalDspPicture(hPicture, NULL, tcPath, &xImage, 0, true);
			}
			usCt++;                                                      // Count until time to change image
			if (Telium_Ttestall(TIMER, 1) == TIMER)
				break;                                                   // if TIMER event (All other events), exit from IDLE_MESSAGE to start the service call
			if (usCt==usTimeout) usCt=0;                                 // Time to change image => counter=0
		} while(1);


		if (hPicture)
			GoalDestroyPicture(&hPicture);                               // Destroy screen

		if (LocalDisplay == 1) {
			Telium_Fclose(hDsp);                                                 // Close "display" peripheral
			hDsp=NULL;
		}
	}

}

// ***************************************************************************
//          int idle_message(NO_SEGMENT no, void *p1, void *p2)
/** Allows the application to display its idle message when Manager goes back
 *  to idle (the application should have the higher priority).
 *  In term of conflict, Manager displays its own idle message.
 * \return
 *  FCT_OK
 * \header sdk30.h
 * \source entry.c
 */
// ***************************************************************************
int idle_message (NO_SEGMENT no, void *p1, void *p2){
	IdleImageDisplay();
	return FCT_OK;
}


// ***************************************************************************
//        int keyboard_event(NO_SEGMENT no, S_KEY *key_in, SKEY *key_out)
/** It's activated when key is pressed and terminal is in idle mode.
 * \param    key_in (I-)
 *    - keycode : Key pressed.
 * \param    key_out (-O)
 *    - keycode : Key pressed, new key, 0=disable.
 * ---------------------------------------------------------------------------
 *  case T_F :                   // Filter F key (Menu manager is disabled)
 *      key_out->keycode=0;         // On multi-applications environment, if an application disables
 *      break;                      // the F key, the treatment is done for all applications
 * 	case N0:                     // Key 0 will access the menu manager
 *      key_out->keycode=T_F;       // On multi-applications environment, if an application redirects
 *      break;	                    // the F key, the treatment is done for all applications
 * ---------------------------------------------------------------------------
 * \return
 *  FCT_OK
 * \header sdk30.h
 * \source entry.c
 */
// ***************************************************************************
int keyboard_event(NO_SEGMENT noappli,S_KEY *key_in,S_KEY *key_out)
{
	// Local variables *
	// *****************
	int result = 0, ret = 0;
	word ShowControlPanel = 0;

	// Keyboard management
	// *******************
	switch (key_in->keycode) {
	case N1:
		confirmGraphicLibHandle(); //// === Make sure Goal is up
		Version_Function();
		key_out->keycode=T_ANN;
		break;
	case N0:
		confirmGraphicLibHandle(); //// === Make sure Goal is up
		ShortCutEnhanced();
		key_out->keycode=T_ANN;
		break;
	case N2: case N3: case N4:
	case N5: case N6: case N7: case N8: case N9:
	case T_VAL:
		//		key_out->keycode=key_in->keycode;  // Amount entry managed by Manager
		key_out->keycode=T_ANN;
		break;
	case T_POINT: //Access the Admin Menu if on biller mode
		if(fncSecurityPassword("5") == 0){
			goto lblSkipT_POINT;// Case where the password was wrong
		}
		confirmGraphicLibHandle(); //// === Make sure Goal is up
		ShortCutAdminEnhanced();
		lblSkipT_POINT:
	case F1: case F2: case F3: case F4:
	case T_CORR: case T_ANN: case NAVI_CLEAR: case NAVI_OK:
	case UP: case DOWN:
		//		key_out->keycode=key_in->keycode;
		key_out->keycode=T_ANN;
		break;
	case T_F:                              // do not filter F key and return the same key !
		MAPGETWORD(appShowControlPanel, ShowControlPanel, lblNothing);
		if (ShowControlPanel == 1) {
			key_out->keycode=key_in->keycode;  // Return the same key value for keys above !
		} else {
			confirmGraphicLibHandle(); //// === Make sure Goal is up
			result = CLESS_ServiceManager_MoreFunction(noappli);
			key_out->keycode=T_ANN;
			return result;
		}
		break;

		lblNothing:
		key_out->keycode=key_in->keycode;  // Return the same key value for keys above !
		break;
	default:
		key_out->keycode=key_in->keycode;
		break;
	}

	return FCT_OK;
}



// ***************************************************************************
//   int get_idle_state_parameter(NO_SEGMENT no, void *p1, S_STATEOUT *param_out)
/** Ask application to define peripherals accepted by the application in selection
 *  process (CAM0, SWIPE3, SWIPE2, SWIPE31 etc...) and to inhibit some keyboard
 *  functions.
 * \param    param_out (-O)
 *    - mask : CAM0 | SWIPE3 | SWIPE2 | SWIPE31 | etc...
 *    - key : INHIBIT_AMOUNT_ENTRY | INHIBIT_IDLE_MANUAL_ENTRY | INHIBIT_LAST_AMOUNT | etc...
 *            On multi-applications environment, if an application inhibits a treatment,
 *            the treatment is inhibited for all applications.
 *    - response_number : should be incremented

 * \return
 *  FCT_OK
 * \header sdk30.h
 * \source entry.c
 */
// ***************************************************************************
int get_idle_state_parameter(NO_SEGMENT no, void *p1, S_STATEOUT *param_out) {
	// Local variables *
	// *****************
	// Empty

	// Return idle configuration to Manager
	// ************************************
	// ** Support field to accept application selection **
	param_out->returned_state[param_out->response_number].no_appli = no;
	param_out->returned_state[param_out->response_number].mask = CAM0 | SWIPE3 | SWIPE2 | SWIPE31;
	// ** Inhibit some keyboard functions **
	if (AMOUNT_ENTRY)
		param_out->returned_state[param_out->response_number].key = INHIBIT_IDLE_MANUAL_ENTRY;
	else
		param_out->returned_state[param_out->response_number].key = INHIBIT_AMOUNT_ENTRY;
	param_out->response_number++;

	return (FCT_OK);
}


// ***************************************************************************
//   int give_money_extended(NO_SEGMENT no, void *p1, S_MONEYOUT_EXTENDED *param_out)
/** Get the currencies supported by the application.
 *  It's activated when currency table is updated by the Manager (after_reset,
 *  more_function, time function).
 * \param    param_out (-O)
 *    - money : Table of currency
 *    - nb_money : Number of currency.
 *    - nb_reponse : should be incremented
 * \return
 *  FCT_OK
 * \header sdk30.h
 * \source entry.c
 */
// ***************************************************************************
int give_money_extended (NO_SEGMENT no, void *p1, S_MONEYOUT_EXTENDED *param_out) {
	S_MONEYOUT_EXTENDED money_out;

	// Unused parameters
	(void)no;
	(void)p1;

	int n_NbMoney;
	S_MONEY_EXTENDED * ps_x_Money;

	if (gs_bCaKeyCheckingDone == FALSE)
		Cless_Parameters_BuildAIDNodes(pTreeCurrentParam);

	memcpy (&money_out, param_out, sizeof(S_MONEYOUT_EXTENDED));
	strcpy (money_out.etat_retour[money_out.nb_reponse].libelle , (char*)zAppName ) ;

	Cless_Parameters_GetMoneyExtended(&n_NbMoney, &ps_x_Money);
	memcpy(&money_out.etat_retour[money_out.nb_reponse].money[0], ps_x_Money, n_NbMoney * sizeof(S_MONEY_EXTENDED));

	money_out.etat_retour[money_out.nb_reponse].nb_money = n_NbMoney;
	money_out.nb_reponse++;
	memcpy ( param_out , &money_out , sizeof(S_MONEYOUT_EXTENDED));
	return (FCT_OK) ;
}


// ***************************************************************************
//             int state(NO_SEGMENT no, void *p1, void *p2)
/** It's activated on "F" key: Consultation->State.
 *  To print terminal content.
 * \return
 *  FCT_OK
 * \header sdk30.h
 * \source entry.c
 */
// ***************************************************************************
int state (NO_SEGMENT no, void *p1, void *p2) {
	// Local variables *
	// *****************
	Telium_Date_t xDate;
	object_info_t xInfos;

	// Print application info
	// **********************
	ObjectGetInfo(OBJECT_TYPE_APPLI, no, &xInfos);          // Retrieve application info

	hPrn = Telium_Fopen("PRINTER", "w-*");                  // Open "printer" peripheral
	if (hPrn != NULL)
	{
		Telium_Pprintf("\x1b""E%s\n""\x1b""F", zAppName);   // Print application name
		Telium_Read_date(&xDate);                           // Print date and time
		Telium_Pprintf(zDate, xDate.day, xDate.month, xDate.year, xDate.hour, xDate.minute);
		Telium_Pprintf("File    : %s\n", xInfos.file_name); // Print application file name
		Telium_Pprintf("CRC     : %04x\n", xInfos.crc);     // Print application CRC
		Telium_Ttestall(PRINTER, 1*100);	                // Wait until the end of printing

		Telium_Fclose(hPrn);                                // Close "printer" peripheral
		hPrn=NULL;
	}

	return FCT_OK;
}

// ***************************************************************************
//             int consult(NO_SEGMENT no, void *p1, void *p2)
/** It's activated on "F" key: Consultation->Transactions.
 *  To print transactions total receipt.
 * \return
 *  FCT_OK
 * \header sdk30.h
 * \source entry.c
 */
// ***************************************************************************
int consult (NO_SEGMENT no, void *p1, void *p2) {
	// Local variables *
	// *****************
	Telium_Date_t xDate;

	// Print daily totals
	// ******************
	hPrn = Telium_Fopen("PRINTER", "w-*");                // Open "printer" peripheral
	if (hPrn != NULL)
	{
		Telium_Pprintf("\x1b""E%s\n""\x1b""F", zAppName); // Print application name
		Telium_Pprintf("        CONSULT        \n"
				"Print daily totals here\n"
				"Number of Debit/Credit \n"
				"Totals of Debit/Credit \n"
				"Number of Cancel\n\n");
		Telium_Read_date(&xDate);                         // Print date and time
		Telium_Pprintf(zDate, xDate.day, xDate.month, xDate.year, xDate.hour, xDate.minute);

		Telium_Ttestall(PRINTER, 1*100);                  // Wait until the end of printing
		Telium_Fclose(hPrn);                              // Close "printer" peripheral
		hPrn=NULL;
	}

	return FCT_OK;
}

// ***************************************************************************
//             int mcall(NO_SEGMENT no, void *p1, void *p2)
/** It's activated on "F" key: Consultation->Call->Planning of Call.
 *  To print call schedule receipt.
 * \return
 *  FCT_OK
 * \header sdk30.h
 * \source entry.c
 */
// ***************************************************************************
int mcall (NO_SEGMENT no, void *p1, void *p2) {
	// Local variables *
	// *****************
	Telium_Date_t xDate;

	// Print call schedule
	// *******************
	hPrn = Telium_Fopen("PRINTER", "w-*");                // Open "printer" peripheral
	if (hPrn != NULL) {
		Telium_Pprintf("\x1b""E%s\n""\x1b""F", zAppName); // Print application name
		Telium_Pprintf("         MCALL         \n"
				"Planning of call here  \n"
				"Time release batch     \n"
				"Time loading parameters\n"
				"Time loading blacklist\n\n");
		Telium_Read_date(&xDate);                         // Print date and time
		Telium_Pprintf(zDate, xDate.day, xDate.month, xDate.year, xDate.hour, xDate.minute);

		Telium_Ttestall(PRINTER, 1*100);                  // Wait until the end of printing
		Telium_Fclose(hPrn);                              // Close "printer" peripheral
		hPrn=NULL;
	}

	return FCT_OK;
}

// ***************************************************************************
//     int is_time_function(NO_SEGMENT no, void *p1, S_ETATOUT *param_out)
/** Do you need the peripherals at the next call of time_function()?.
 *  It's call every minute.
 * \param    param_out (-O)
 *    - response :
 *      REP_OK (Manager closes all peripherals), REP_KO (Manager keeps all peripherals opened)
 * \return
 *  FCT_OK
 * \header sdk30.h
 * \source entry.c
 */
// ***************************************************************************
int is_time_function(NO_SEGMENT no, void *p1, S_ETATOUT *param_out) {
	// Local variables *
	// *****************
	int iRet;

	confirmGraphicLibHandle(); //// === Make sure Goal is up

	// Peripherals needed?
	// *******************
	param_out->returned_state[param_out->response_number].state.response=REP_OK;
	iRet = CLESS_ServiceManager_IsName(no, param_out);

	return iRet;
}


static struct dateTMS BuildDateTMS(char * dateString){
	struct dateTMS dt;
	char tmp[2];
	int position = 0;
	int maxLoop = strlen(dateString);

	if (maxLoop%2 != 0)
		maxLoop++;

	for (position = 0; position < maxLoop; position+=2) {
		memset(tmp, 0, sizeof(tmp));
		strncpy(tmp,&dateString[position],2);

		switch (position) {
		case 0://Day
			dt.day = atoi(tmp);
			break;
		case 2://Month
			dt.month = atoi(tmp);
			break;
		case 4://Year
			dt.year = atoi(tmp);
			break;
		case 6://Hour
			dt.hour = atoi(tmp);
			break;
		case 8://Minutes
			dt.minute = atoi(tmp);
			break;
		case 10://Seconds
			dt.seconds = atoi(tmp);
			break;
		}
	}

	return dt;
}

int compare_dates (char * Date1, char * Date2) {
	struct dateTMS d1; struct dateTMS d2;
	char LocalDate1[12 + 1]; //ddmmyyhhmmss
	char LocalDate2[12 + 1]; //ddmmyyhhmmss

	memset(LocalDate1, 0, sizeof(LocalDate1));
	memset(LocalDate2, 0, sizeof(LocalDate2));

	strcpy(LocalDate1, Date1);
	strcpy(LocalDate2, Date2);

	d1 = BuildDateTMS(LocalDate1);
	d2 = BuildDateTMS(LocalDate2);

	if (d1.year < d2.year)
		return -1;

	else if (d1.year > d2.year)
		return 1;

	if (d1.year == d2.year) {
		//---- Months compare
		if (d1.month<d2.month)
			return -1;
		else if (d1.month>d2.month)
			return 1;
		//---- Days compare
		else if (d1.day<d2.day)
			return -1;
		else if(d1.day>d2.day)
			return 1;
		//---- Hours compare
		else if (d1.hour<d2.hour)
			return -1;
		else if(d1.hour>d2.hour)
			return 1;
		//---- Minutes compare
		else if (d1.minute<d2.minute)
			return -1;
		else if(d1.minute>d2.minute)
			return 1;
		//---- Minutes compare
		else if (d1.seconds<d2.seconds)
			return -1;
		else if(d1.seconds>d2.seconds)
			return 1;
		else
			return 0;
	}
	return 0;
}


// ***************************************************************************
//            int time_function(NO_SEGMENT no, void *p1, void *p2)
/** Allow application to execute its own periodical process.
 *  It's call every minute.
 * \return
 *  FCT_OK
 * \header sdk30.h
 * \source entry.c
 */
// ***************************************************************************
int time_function(NO_SEGMENT no, void *p1, void *p2) {
	int LocalDisplay = 0;

	hDsp = Telium_Stdperif("DISPLAY", NULL);    // Check "display" peripheral already opened? (done by Manager)
	if (hDsp == NULL) {                         // No, then open it
		hDsp = Telium_Fopen("DISPLAY", "w*");   // Open "display" peripheral
		LocalDisplay = 1;
	}
	confirmGraphicLibHandle(); //// === Make sure Goal is up

	if (hDsp != NULL) {
		if (fncTMSConnectionSession() == 0) { //check if the TMS is already doing something
			if (isApp_Already_in_Session() == 0) {//At times the previous session that was started took too long so to prevent overlapping we check if

				fncAutoSettlementChecker();

				TaskSimSlot();
			}
		}
		if(LocalDisplay == 1){
			Telium_Fclose(hDsp);                    // Close "display" peripheral
			hDsp=NULL;
		}

	}

	return FCT_OK;
}

// ***************************************************************************
//     int is_change_init(NO_SEGMENT no, void *p1, S_ETATOUT *param_out)
/** It's activated on "F" key: Initialisation->Parameters->List.
 *  Each time Manager wants to change its own parameters.
 * \param    param_out (-O)
 *    - mask : Key "F" 031 -> Parameters modification (0:accepting, 1:refusing)
 *                                                       MSK_NULL  / MSK_ALL
 *             Manager prints which applications have refused the modification
 * \return
 *  FCT_OK
 * \header sdk30.h
 * \source entry.c
 */
// ***************************************************************************
int is_change_init(NO_SEGMENT no, void *p1, S_ETATOUT *param_out) {
	// Local variables *
	// *****************
	T_STANDARD xPabx;
	bool bIso2=FALSE, bIso1=FALSE, bIso3=FALSE;
	Telium_Date_t xDate;
	char* pcLang;
	int iRet;

	// Parameters modification allowed?
	// ********************************
	param_out->returned_state[param_out->response_number].state.mask=MSK_NULL;  // Accept all

	iRet = CLESS_ServiceManager_IsName(no, param_out);

	// Save actual parameters
	// **********************
	memset (&xParams, 0, sizeof(ST_PARAMS));
	Telium_Read_date(&xDate);             // Read date and time
	switch (PSQ_Give_Date_Format())       // Retrieve date format
	{
	case 0:
		Telium_Sprintf (xParams.tcOldDate, zDate1, xDate.day, xDate.month, xDate.year, xDate.hour, xDate.minute);
		strcpy (xParams.tcOldFmtDate, "DD/MM/YYYY\n");
		break;
	case 1:
		Telium_Sprintf (xParams.tcOldDate, zDate1, xDate.month, xDate.day, xDate.year, xDate.hour, xDate.minute);
		strcpy (xParams.tcOldFmtDate, "MM/DD/YYYY\n");
		break;
	case 2:
		Telium_Sprintf (xParams.tcOldDate, zDate2, xDate.year, xDate.month, xDate.day, xDate.hour, xDate.minute);
		strcpy (xParams.tcOldFmtDate, "YYYY/MM/DD\n");
		break;
	case 3:
		Telium_Sprintf (xParams.tcOldDate, zDate3, xDate.day, xDate.month, xDate.year, xDate.hour, xDate.minute);
		strcpy (xParams.tcOldFmtDate, "DD.MM.YYYY\n");
		break;
	default:
		Telium_Sprintf (xParams.tcOldDate, zDate2, xDate.year, xDate.month, xDate.day, xDate.hour, xDate.minute);
		strcpy (xParams.tcOldFmtDate, "YYYY/MM/DD\n");
		break;
	}
	pcLang = GetManagerLanguage();       // Retrieve terminal language
	if (strcmp(pcLang, "EN") == 0)
		strcpy (xParams.tcOldLanguage, "English\n");
	else if (strcmp(pcLang, "FR") == 0)
		strcpy (xParams.tcOldLanguage, "French\n");
	else if (strcmp(pcLang, "RU") == 0)
		strcpy (xParams.tcOldLanguage, "Russian\n");
	else
		strcpy (xParams.tcOldLanguage, "Unknown\n");
	PSQ_No_standard (xPabx);             // Retrieve Pabx prefix
	strcpy (xParams.tcOldPabx, xPabx);
	bIso2=TRUE;                          // Retrieve Swipe
	if (PSQ_Is_ISO1() != 0)
		bIso1=TRUE;
	if (PSQ_Is_ISO3() != 0)
		bIso3=TRUE;
	if (bIso2)
		strcpy (xParams.tcOldISOreader, "Iso2\n");
	if (bIso2 & bIso1)
		strcpy (xParams.tcOldISOreader, "Iso2 + Iso1\n");
	if (bIso2 & bIso3)
		strcpy (xParams.tcOldISOreader, "Iso2 + Iso3\n");
	if (bIso1 & bIso2 & bIso3)
		strcpy (xParams.tcOldISOreader, "Iso1 + Iso2 + Iso3\n");
	strcpy (xParams.tcOldPPad, "No");
	if (PSQ_Is_pinpad())
	{
		strcpy (xParams.tcOldPPad, "Yes");
		switch (PSQ_Pinpad_Value())
		{
		case PP30_PPAD:   strcpy(xParams.tcOldPPadType, "PP30 ");  break; // Pinpad characters no-reader Booster1
		case PP30S_PPAD:  strcpy(xParams.tcOldPPadType, "PP30S");  break; // Pinpad characters no-reader Booster2
		case PPC30_PPAD:  strcpy(xParams.tcOldPPadType, "PPC30");  break; // Pinpad graphic no-reader Booster1
		case PPR30_PPAD:  strcpy(xParams.tcOldPPadType, "PPR30");  break; // Pinpad graphic reader Booster1
		case P30_PPAD:    strcpy(xParams.tcOldPPadType, "P30  ");  break; // Pinpad graphic reader Booster2
		case IPP220_PPAD: strcpy(xParams.tcOldPPadType, "IPP220"); break; // Pinpad characters no-reader Booster3
		case IPP250_PPAD: strcpy(xParams.tcOldPPadType, "IPP250"); break; // Pinpad characters no-reader color Booster3
		case IPP280_PPAD: strcpy(xParams.tcOldPPadType, "IPP280"); break; // Pinpad graphic no-reader color Booster3
		case IPP320_PPAD: strcpy(xParams.tcOldPPadType, "IPP320"); break; // Pinpad graphic reader Booster3
		case IPP350_PPAD: strcpy(xParams.tcOldPPadType, "IPP350"); break; // Pinpad graphic reader color Booster3
		default:                                                   break; // Pinpad unknown
		}
	}

	return iRet;
}

// ***************************************************************************
//        int modif_param(NO_SEGMENT no, S_MODIF_P *param_in, void *p2)
/** It's activated on "F" key: Initialization->Parameters->List.
 *  Each time Manager changed its own parameters.
 * \param    param_in (I-)
 *    - mask : Key "F" 031 -> Parameters modification (0:not modified, 1:modified)
 * \return
 *  FCT_OK
 * \header sdk30.h
 * \source entry.c
 */
// ***************************************************************************
int modif_param(NO_SEGMENT noappli, S_MODIF_P *param_in, void *p2)
{
	// Local variables *
	// *****************
	doubleword uiMask;
	T_STANDARD tcPabx;
	char tcISOreader[24+1];
	bool bIso2=FALSE, bIso1=FALSE, bIso3=FALSE;
	bool bReboot=FALSE;
	char* pcLang;
	Telium_Date_t xDate;

	// Display parameters modified
	// ***************************
	hPrn = Telium_Fopen("PRINTER", "w-*");        // Open "printer" peripheral
	Telium_Read_date(&xDate);                     // Read date and time
	if (hPrn != NULL)
	{
		uiMask = param_in->etatout.returned_state[param_in->etatout.response_number].state.mask;
		switch (uiMask)
		{
		case MSK_DATE:                            // Date and Time (0x20000)
			Telium_Pprintf("\x1b""E%s\n""\x1b""F", zAppName);
			Telium_Pprintf(xParams.tcOldFmtDate);
			Telium_Pprintf(xParams.tcOldDate);
			switch (PSQ_Give_Date_Format())       // Retrieve date format
			{
			case 0:
				Telium_Pprintf("DD/MM/YYYY\n");
				Telium_Pprintf (zDate1, xDate.day, xDate.month, xDate.year, xDate.hour, xDate.minute);
				break;
			case 1:
				Telium_Pprintf("MM/DD/YYYY\n");
				Telium_Pprintf (zDate1, xDate.month, xDate.day, xDate.year, xDate.hour, xDate.minute);
				break;
			case 2:
				Telium_Pprintf("YYYY/MM/DD\n");
				Telium_Pprintf (zDate2, xDate.year, xDate.month, xDate.day, xDate.hour, xDate.minute);
				break;
			case 3:
				Telium_Pprintf("DD.MM.YYYY\n");
				Telium_Pprintf (zDate3, xDate.day, xDate.month, xDate.year, xDate.hour, xDate.minute);
				break;
			default:
				Telium_Pprintf("YYYY/MM/DD\n");
				Telium_Pprintf (zDate2, xDate.year, xDate.month, xDate.day, xDate.hour, xDate.minute);
				break;
			}
			break;
			case MSK_FRMT_DATE:                       // Date format (0x10000 removed => see MSK_DATE)
				break;
			case MSK_LANGUE:                          // Language (0x08000)
				Telium_Pprintf("\x1b""E%s\n""\x1b""F", zAppName);
				Telium_Pprintf (xParams.tcOldLanguage);
				pcLang = GetManagerLanguage();        // Retrieve terminal language
				if (strcmp(pcLang, "EN") == 0)
					Telium_Pprintf ("English\n");
				else if (strcmp(pcLang, "FR") == 0)
					Telium_Pprintf ("French\n");
				else if (strcmp(pcLang, "RU") == 0)
					Telium_Pprintf ("Russian\n");
				else
					Telium_Pprintf ("Unknown\n");
				break;
			case MSK_STANDARD:                        // Pabx (0x01000)
				Telium_Pprintf("\x1b""E%s\n""\x1b""F", zAppName);
				Telium_Pprintf ("%s\n", xParams.tcOldPabx);
				PSQ_No_standard (tcPabx);             // Retrieve Pabx prefix
				Telium_Pprintf("%s\n", tcPabx);
				break;
			case MSK_PINPAD:                          // PinPad (0x00800)
				Telium_Pprintf("\x1b""E%s\n""\x1b""F", zAppName);
				Telium_Pprintf ("%s %s\n", xParams.tcOldPPad, xParams.tcOldPPadType);
				if (PSQ_Is_pinpad())
				{
					switch (PSQ_Pinpad_Value())
					{
					case PP30_PPAD:                   // Pinpad characters no-reader Booster1
						Telium_Pprintf("Yes PP30\n");
						if (memcmp(xParams.tcOldPPadType, "PP30 ", 5) != 0)
							bReboot=TRUE;
						break;
					case PP30S_PPAD:                  // Pinpad characters no-reader Booster2
						Telium_Pprintf("Yes PP30S\n");
						if (memcmp(xParams.tcOldPPadType, "PP30S", 5) != 0)
							bReboot=TRUE;
						break;
					case PPC30_PPAD:                  // Pinpad graphic no-reader Booster1
						Telium_Pprintf("Yes PPC30\n");
						if (memcmp(xParams.tcOldPPadType, "PPC30", 5) != 0)
							bReboot=TRUE;
						break;
					case PPR30_PPAD:                  // Pinpad graphic reader Booster1
						Telium_Pprintf ("Yes PPR30\n");
						if (memcmp(xParams.tcOldPPadType, "PPR30", 5) != 0)
							bReboot=TRUE;
						break;
					case P30_PPAD:                    // Pinpad graphic reader Booster2
						Telium_Pprintf ("Yes P30\n");
						if (memcmp(xParams.tcOldPPadType, "P30  ", 5) != 0)
							bReboot=TRUE;
						break;
					case IPP220_PPAD:                 // Pinpad characters no-reader Booster3
						Telium_Pprintf ("Yes IPP220\n");
						if (memcmp(xParams.tcOldPPadType, "IPP220", 6) != 0)
							bReboot=TRUE;
						break;
					case IPP250_PPAD:                 // Pinpad characters no-reader color Booster3
						Telium_Pprintf ("Yes IPP250\n");
						if (memcmp(xParams.tcOldPPadType, "IPP250", 6) != 0)
							bReboot=TRUE;
						break;
					case IPP280_PPAD:                 // Pinpad graphic no-reader color Booster3
						Telium_Pprintf ("Yes IPP280\n");
						if (memcmp(xParams.tcOldPPadType, "IPP280", 6) != 0)
							bReboot=TRUE;
						break;
					case IPP320_PPAD:                 // Pinpad graphic reader Booster3
						Telium_Pprintf ("Yes IPP320\n");
						if (memcmp(xParams.tcOldPPadType, "IPP320", 6) != 0)
							bReboot=TRUE;
						break;
					case IPP350_PPAD:                 // Pinpad graphic reader color Booster3
						Telium_Pprintf ("Yes IPP350\n");
						if (memcmp(xParams.tcOldPPadType, "IPP350", 6) != 0)
							bReboot=TRUE;
						break;
					default:                          // Pinpad unknown
						Telium_Pprintf ("Yes Unknown\n");
						break;
					}
				}
				else
				{
					Telium_Pprintf ("No\n");
					if (memcmp(xParams.tcOldPPad, "No", 2) != 0)
						bReboot=TRUE;
				}
				break;
			case MSK_SWIPE:                           // Swipe (0x00200)
				Telium_Pprintf("\x1b""E%s\n""\x1b""F", zAppName);
				Telium_Pprintf (xParams.tcOldISOreader);
				bIso2=TRUE;
				if (PSQ_Is_ISO1() != 0)
					bIso1=TRUE;
				if (PSQ_Is_ISO3() != 0)
					bIso3=TRUE;
				if (bIso2)
					strcpy (tcISOreader,"Iso2");
				if (bIso2 & bIso1)
					strcpy (tcISOreader, "Iso2 + Iso1");
				if (bIso2 & bIso3)
					strcpy (tcISOreader, "Iso2 + Iso3");
				if (bIso1 & bIso2 & bIso3)
					strcpy (tcISOreader, "Iso1 + Iso2 + Iso3");
				Telium_Pprintf ("%s\n", tcISOreader);
				break;
			case MSK_MDP:                             // T.M.S Menu (0x00002)
				break;
			default:                                  // Others
				break;
		}

		Telium_Ttestall(PRINTER, 1*100);          // Wait until the end of printing
		Telium_Fclose(hPrn);                      // Close "printer" peripheral
		hPrn=NULL;
	}

	if (bReboot) {                                // Reboot if pinpad configuration changed

		hDsp = Telium_Stdperif("DISPLAY", NULL);  // Check "display" peripheral already opened? (done by Manager)
		if (hDsp == NULL)                         // No, then open it
			hDsp = Telium_Fopen("DISPLAY", "w*"); // Open "display" peripheral

		if (hDsp != NULL)
			GL_Dialog_Message(hGoal, NULL, "TERMINAL\nRE-INIT", GL_ICON_INFORMATION, GL_BUTTON_NONE, 1*1000);

		Telium_Exit(0);                           // Reboot terminal to reinitialize
	}

	return FCT_OK;
}

// ***************************************************************************
//       int is_evol_pg(NO_SEGMENT no, void *p1, S_ETATOUT *param_out)
/** It's activated each time Manager wants to run a downloading session (local or remote).
 *  "F" key: Evolution->Load->Local or Evolution->Remote Load
 * \param    param_out (-O)
 *    - response :
 *      REP_OK (App authorizes downloading process), REP_KO (App refuses any downloading process)
 *      Manager prints which applications have refused the downloading session
 * \return
 *  FCT_OK
 * \header sdk30.h
 * \source entry.c
 */
// ***************************************************************************
int is_evol_pg(NO_SEGMENT no, void *p1, S_ETATOUT *param_out)
{
	// Local variables *
	// *****************
	int iRet;

	// Downloading process allowed?
	// ****************************
	param_out->returned_state[param_out->response_number].state.response=REP_OK;

	iRet = CLESS_ServiceManager_IsName(no, param_out);


	return iRet;
}


//! \brief Load a black list file in memory.
//! \param[in] szPath file path for the black list file.
//! \param[in] szfile file name for the black list file.
//! \return
//!		- TRUE if a black list file has been loaded, FALSE else.
int ReadConfigFile(const char* szPath, const char* szFile) {
	unsigned int nMountMode;
	char DirName[1 + FS_DISKNAMESIZE + 1];
	char szFullFileName[1 + FS_DISKNAMESIZE + 1 + FS_FILENAMESIZE + 1];
	unsigned long ulFileSize;
	unsigned int Mode, rc;
	S_FS_FILE* hFile;
	char* pBuffer;

	if (strcmp(szFile, "CONFIG.txt") == 0) {
		Telium_Sprintf(DirName, "/%s", szPath);

		// Mount the disk
		nMountMode = 0;

		if (FS_mount(DirName, &nMountMode) == FS_OK) {

			Telium_Sprintf(szFullFileName, "/%s/%s", szPath, szFile);

			Mode = FS_READMOD;
			rc = FS_mount(szFullFileName, &Mode);


			hFile = FS_open(szFullFileName, "r");
			if (hFile != NULL){

				// Get the size of the file
				ulFileSize = FS_length(hFile);


				pBuffer = umalloc(ulFileSize + 1);
				if (pBuffer != NULL) {
					if (FS_read(pBuffer, 1, ulFileSize, hFile) == (int)ulFileSize){
						fncDigestConfigFile(pBuffer);
					}

					ufree(pBuffer);
				}


				// Close the file
				FS_close(hFile);

				if (rc == FS_OK)
					FS_unmount(szFullFileName);
			}

			// Unmount the disk
			FS_unmount(DirName);
		}

		return (STOP);
	} else
		return (FCT_OK);
}

// ***************************************************************************
//        int file_received(NO_SEGMENT no, S_FILE *param_in, void *p2)
/** Manager reports parameters file received from LLT and located on HOST disk.
 *  It's activated upon reception of a parameter file by the manager.
 *  This parameter file must have the extension .PAR, and will be deleted
 *  by manager when leaving the function file_received().
 * \param    param_in (I-)
 *    - volume_name : HOST (File loaded in DFS).
 *    - file_name : Application file name
 * \return
 *  FCT_OK
 * \header sdk30.h
 * \source entry.c
 */
// ***************************************************************************
int file_received(NO_SEGMENT no, S_FILE *param_in, void *p2) {

	unsigned char * pSha;
	int nCRCLength;
	int nResult;
	int nIndex;
	char StrBuff[100];
	char TmpStr[10];

	(void) no;


	memset(StrBuff, 0, sizeof(StrBuff));
	Telium_Sprintf(StrBuff, "%s", param_in->file_name);

	if (strncmp(StrBuff, "CLESSCUST.PAR", 13) == 0) {
		memset(StrBuff, 0, sizeof(StrBuff));

		__Cless_Menu_EraseParameters();//First Errase all parameters

		// Try to read parameter file and store it into an internal TLV tree structure
		nResult = Cless_Parameters_ReadXMLFile((char*)param_in->volume_name, (char*)param_in->file_name, &pTreeCurrentParam);
		if (nResult == STOP) {// Inputed file is a parameter file

			// File has been received
			GTL_Traces_DiagnosticText ("CLESSCUST.PAR file received\n");

			// Prepare all the information (before the transaction) for each AID

			// Trace prepared data
			Cless_Parameters_Trace_Parameter_AID_Structure();

			// Compute the configuration CRC
			Cless_Parameters_Compute_Config_CRC(&pTreeCurrentParam, &nCRCLength, &pSha); {
				Telium_Sprintf(StrBuff, "Parameters CRC : \n");

				for (nIndex=0; nIndex<nCRCLength; nIndex++) {
					Telium_Sprintf(TmpStr, "%02x", pSha[nIndex]);
					strcat(StrBuff, TmpStr);
				}
				GTL_Traces_TraceDebug("%s", StrBuff);
			}
			return (nResult);
		}

	}

	// Try to read black List parameter
	nResult = Cless_BlackListGetNewFile(param_in->volume_name, param_in->file_name);
	if (nResult == STOP) {// Inputed file is a Black list parameter file
		GTL_Traces_DiagnosticText ("CLESSBLACK.PAR file received\n");
		return (nResult);
	}

	// Try to read configfile
	nResult = ReadConfigFile(param_in->volume_name, param_in->file_name);
	if (nResult == STOP) {// Inputed file is a Black list parameter file
		GTL_Traces_DiagnosticText ("CONFIG.txt file received\n");
		return (nResult);
	}

	return (nResult);
}

// ***************************************************************************
//   int message_received(NO_SEGMENT no, S_MESSAGE_IAM *param_in, void *p2)
/** Inter application messaging.
 *  It's activated each time Manager received a message in its mailbox for this
 *  application or task.
 * \param    param_in (I-)
 *    - sender : Sender ID.
 *    - receiver : Receiver ID.
 *    - type : IAM type.
 *    - length : Message length.
 *    - value : Message received.
 * \return
 *  FCT_OK
 * \header sdk30.h
 * \source entry.c
 */
// ***************************************************************************
int message_received(NO_SEGMENT no, S_MESSAGE_IAM *param_in, void *p2)
{
	// Local variables *
	// *****************
	T_GL_HWIDGET hScreen=NULL;
	ST_DSP_LINE xDspLine;
	T_GL_HWIDGET hDocument=NULL;
	ST_PRN_LINE xPrnLine;
	char tcPrint[256+1];
	byte p; // Printer line index
	int iHeader, iFooter;
	int iRet;

	// Open peripherals
	// ****************
	iHeader = IsHeader();     // Save header state
	iFooter = IsFooter();     // Save footer state
	if (!IsColorDisplay())    // B&W terminal?
	{                         // Yes
		DisplayLeds(_OFF_);   // Disable Leds
		DisplayHeader(_OFF_); // Disable Header
		DisplayFooter(_OFF_); // Disable Footer
	}

	iRet = OpenPeripherals(); // Open standard peripherals
	CHECK(iRet>=0, lblKO);

	// Get message received from TRAINING itself or USER2 or TASK2
	// ***********************************************************
	hScreen = GoalCreateScreen(hGoal, thScreen, NUMBER_OF_LINES(thScreen), GL_ENCODING_UTF8);
	CHECK(hScreen!=NULL, lblKO);
	iRet = GoalClrScreen(hScreen, GL_COLOR_BLACK, 0, false); // Create and clear screen
	CHECK(iRet>=0, lblKO);

	xDspLine = thScreen[0];                                  // Build screen and show it (Appli name, msg received)
	xDspLine.xColor.eTextAlign = GL_ALIGN_CENTER; xDspLine.xColor.ulPen = GL_COLOR_YELLOW;
	xDspLine.xBW.eTextAlign = GL_ALIGN_CENTER;

	p=0;                                                     // Create the document
	hDocument = GoalCreateDocument(hGoal, GL_ENCODING_UTF8);
	CHECK(hDocument!=NULL, lblKO);

	xPrnLine = xPrinter;                                     // Build the document (Appli name, IAM info, Sender/Receiver, IAM type, IAM message)
	xPrnLine.eTextAlign = GL_ALIGN_CENTER;
	xPrnLine.xFont.eScale = GL_SCALE_XXLARGE;
	iRet = GoalPrnLine(hDocument, p++, (char*)zAppName, &xPrnLine);
	CHECK(iRet>=0, lblKO);
	xPrnLine.eTextAlign = GL_ALIGN_LEFT;
	xPrnLine.xFont.eScale = GL_SCALE_LARGE;

	if (param_in->type == 0) {                               // Message received from Training
		iRet = GoalDspLine(hScreen, 0, "TRAIN (Msg from TRAIN)", &xDspLine, 0, false);
		CHECK(iRet>=0, lblKO);
		iRet = GoalDspMultiLine(hScreen, 1, (char*)param_in->value, param_in->length, thScreen, NUMBER_OF_LINES(thScreen), 0, true);
		CHECK(iRet>=0, lblKO);

		iRet = GoalPrnLine(hDocument, p++, "Message IAM from Appli:", &xPrnLine);
		CHECK(iRet>=0, lblKO);
		Telium_Sprintf (tcPrint, "S:%04X R:%04X", param_in->sender, param_in->receiver);
		iRet = GoalPrnLine(hDocument, p++, tcPrint, &xPrnLine);
		CHECK(iRet>=0, lblKO);
		Telium_Sprintf (tcPrint, "IAM Type : %04X\n\n", param_in->type);
		iRet = GoalPrnLine(hDocument, p++, tcPrint, &xPrnLine);
		CHECK(iRet>=0, lblKO);
		iRet = GoalPrnLine(hDocument, p++, "Msg from TRAINING:", &xPrnLine);
		CHECK(iRet>=0, lblKO);
		memset(tcPrint, 0, sizeof(tcPrint));
		strncpy(tcPrint, (char*)param_in->value, param_in->length);
		iRet = GoalPrnLine(hDocument, p++, tcPrint, &xPrnLine);
		CHECK(iRet>=0, lblKO);
	} else if (param_in->type == 1) {                          // Message received from Task2

		iRet = GoalDspLine(hScreen, 0, "TRAIN (Msg from TASK2)", &xDspLine, 0, false);
		CHECK(iRet>=0, lblKO);
		iRet = GoalDspMultiLine(hScreen, 1, (char*)param_in->value, param_in->length, thScreen, NUMBER_OF_LINES(thScreen), 0, true);
		CHECK(iRet>=0, lblKO);

		iRet = GoalPrnLine(hDocument, p++, "Message IAM from Task:", &xPrnLine);
		CHECK(iRet>=0, lblKO);
		Telium_Sprintf (tcPrint, "S:%04X R:%04X", param_in->sender, param_in->receiver);
		iRet = GoalPrnLine(hDocument, p++, tcPrint, &xPrnLine);
		CHECK(iRet>=0, lblKO);
		Telium_Sprintf (tcPrint, "IAM Type : %04X\n\n", param_in->type);
		iRet = GoalPrnLine(hDocument, p++, tcPrint, &xPrnLine);
		CHECK(iRet>=0, lblKO);
		iRet = GoalPrnLine(hDocument, p++, "Msg from TASK2:", &xPrnLine);
		CHECK(iRet>=0, lblKO);
		memset(tcPrint, 0, sizeof(tcPrint));
		strncpy(tcPrint, (char*)param_in->value, param_in->length);
		iRet = GoalPrnLine(hDocument, p++, tcPrint, &xPrnLine);
		CHECK(iRet>=0, lblKO);
	} else if (param_in->type == 4) {/* Msg from EMV Custom */
	} else {                                                 // Message received from User2

		iRet = GoalDspLine(hScreen, 0, "TRAIN (Msg from USER2)", &xDspLine, 0, false);
		CHECK(iRet>=0, lblKO);
		iRet = GoalDspMultiLine(hScreen, 1, (char*)param_in->value, param_in->length, thScreen, NUMBER_OF_LINES(thScreen), 0, true);
		CHECK(iRet>=0, lblKO);

		iRet = GoalPrnLine(hDocument, p++, "Message IAM from Appli:", &xPrnLine);
		CHECK(iRet>=0, lblKO);
		Telium_Sprintf (tcPrint, "S:%04X R:%04X", param_in->sender, param_in->receiver);
		iRet = GoalPrnLine(hDocument, p++, tcPrint, &xPrnLine);
		CHECK(iRet>=0, lblKO);
		Telium_Sprintf (tcPrint, "IAM Type : %04X\n\n", param_in->type);
		iRet = GoalPrnLine(hDocument, p++, tcPrint, &xPrnLine);
		CHECK(iRet>=0, lblKO);
		iRet = GoalPrnLine(hDocument, p++, "Msg from USER2:", &xPrnLine);
		CHECK(iRet>=0, lblKO);
		memset(tcPrint, 0, sizeof(tcPrint));
		strncpy(tcPrint, (char*)param_in->value, param_in->length);
		iRet = GoalPrnLine(hDocument, p++, tcPrint, &xPrnLine);
		CHECK(iRet>=0, lblKO);
	}

	iRet = GoalPrnLine(hDocument, p++, "----------------------------------------------""\n\n\n\n", &xPrnLine);
	CHECK(iRet>=0, lblKO);

	iRet = GoalPrnDocument(hDocument);                       // Print document
	CHECK(iRet>=0, lblKO);

	Telium_Ttestall(0, 2*100);                               // Wait to see the message received

	goto lblEnd;

	// Errors treatment
	// ****************
	lblKO:                                                       // None-classified low level error
	GL_Dialog_Message(hGoal, NULL, "Processing Error", GL_ICON_ERROR, GL_BUTTON_VALID, 5*1000);
	goto lblEnd;
	lblEnd:
	if (hScreen)
		GoalDestroyScreen(&hScreen);                         // Destroy screen

	if (hDocument)
		GoalDestroyDocument(&hDocument);                     // Destroy document

	ClosePeripherals();                                      // Close standard peripherals

	DisplayLeds(_ON_);                                       // Enable Leds
	DisplayHeader(iHeader);                                  // Restore Header
	DisplayFooter(iFooter);                                  // Restore Footer

	return FCT_OK;
}




// ***************************************************************************
//   int is_card_specific(NO_SEGMENT no, S_TRANSIN *param_in, S_ETATOUT *param_out)
/**
 *  It's activated when a card is inserted, swiped or manually entry.
 *  Ask the application if the card needs a specific processing.
 * \param    param_out (-O)
 *    - response :
 *      REP_OK (card processing), REP_KO (no card processing)
 *  Only one application wants to process the card, manager calls CARD_INSIDE entry.
 *  More application wants to process the card, manager asks for card removal.
 *  If no application wants to process the card, manager goes on with selection process.
 * \return
 *  FCT_OK
 * \header sdk30.h
 * \source entry.c
 */
// ***************************************************************************
int is_card_specific(NO_SEGMENT no, S_TRANSIN *param_in, S_ETATOUT *param_out)
{
	// Local variables *
	// *****************
	int iRet;
	int localDisplay = 0;

	// Card needs a specific processing
	// ********************************
	hDsp = Telium_Stdperif("DISPLAY", NULL);     // Check "display" peripheral already opened? (done by Manager)
	if (hDsp == NULL) {                          // No, then open it
		hDsp = Telium_Fopen("DISPLAY", "w*");    // Open "display" peripheral
		localDisplay = 1;
	}

	if (hDsp != NULL) {
		if (AMOUNT_ENTRY && (param_in->amount == 0)) {                                        // Enter amount first before card process
			param_out->returned_state[param_out->response_number].state.response = REP_OK;
			if ((IsIPP480() == 0) || (param_in->extended_support == 1)) // Display this message only at card insertion (for iPP480 Dual-reader)
				GL_Dialog_Message(hGoal, NULL, "Enter Amount First", GL_ICON_WARNING, GL_BUTTON_NONE, 2*1000);
		}
		else
			param_out->returned_state[param_out->response_number].state.response = REP_KO;

		if(localDisplay == 1){
			Telium_Fclose(hDsp);                     // Close "display" peripheral
			hDsp=NULL;
		}
	}

	iRet = CLESS_ServiceManager_IsName(no, param_out);

	return iRet;
}

// ***************************************************************************
//   int card_inside(NO_SEGMENT no, S_TRANSIN *param_in, S_TRANSOUT *param_out)
/**
 *  It's activated when an application has chosen to treat this card has specific.
 *  The transaction is done here.
 * \param    param_out (-O)
 *    - rc_payment :
 *      PAY_OK (Transaction done), PAY_KO (Transaction rejected)
 *  If an application returns STOP, polling is stopped and manager asks for card removal.
 *  The application is in charge to ask for amount and currency if needed.
 * \return
 *  STOP    Card accepted and transaction process done, polling is stop.
 *  FCT_OK  Card refused and poll the next application.
 * \header sdk30.h
 * \source entry.c
 */
// ***************************************************************************
int card_inside(NO_SEGMENT no, S_TRANSIN *param_in, S_TRANSOUT *param_out)
{
	// Local variables *
	// *****************
	bool card_accepted = TRUE;

	if (card_accepted)
	{
		// Return transaction status
		// *************************
		param_out->rc_payment = PAY_OK; // Transaction done, polling is stop
		return STOP;
	}
	else
		return FCT_OK;                  // Card refused, poll the next application
}

// ***************************************************************************
//   int is_for_you_before(NO_SEGMENT no, void *p1, S_ETATOUT *param_out)
/**
 *  Only for application handling "old" chip cards not supporting software reset.
 *  Such as synchronous card, the application should power-on the card then
 *  read it to analyze which kind of synchronous card then power off.
 *  Regarding the card, the application should return accepted or rejected.
 * \param    param_out (-O)
 *    - response :
 *      Priority level (>=0 Accepted, 0=Rejected)
 *    - appname : Application name
 *    - no : Application number
 *    - response_number : should be incremented
 *  Applications being declared candidate will be placed before EMV applications
 *  in the list.
 * \return
 *  FCT_OK
 * \header sdk30.h
 * \source entry.c
 */
// ***************************************************************************
int is_for_you_before(NO_SEGMENT no, S_TRANSIN *param_in, S_ETATOUT *param_out)
{
	// Local variables *
	// *****************
	// Empty

	// Case for a synchronous card which needs a selection process
	// ***********************************************************

	// Power on the card
	// =================
	// Implement here the function to power-on the synchronous card.

	// Analyze chip card
	// =================
	// Read the card to analyze which synchronous card it is and return accepted or rejected.
	param_out->returned_state[param_out->response_number].state.response = 0;


	// Power off the card
	// ==================
	// Implement here the function to power-off the synchronous card.

	// Give my application name
	strcpy (param_out->returned_state[param_out->response_number].appname, zAppName);
	// Give my application number
	param_out->returned_state[param_out->response_number].no_appli = no;
	// Increment the response number
	param_out->response_number++;

	return FCT_OK;
}

// ***************************************************************************
//   int is_for_you_after(NO_SEGMENT no, S_TRANSIN *param_in, S_CARDOUT *param_out)
/**
 *  Ask application to recognize the mag, smart or manually card in order to be
 *  a candidate.
 * \param    param_out (-O)
 *    - cardappnumber : 1 = Card accepted, 0 = Card rejected
 *    - cardapp (Selection priority):
 *      CARD_PROCESSED (low priority)
 *      CARD_RECOGNIZED (medium priority)
 *      CARD_PRIORITY (high priority)
 *    - appname : Application name
 *    - no : Application number
 *    - response_number : should be incremented
 *  If an application returns STOP, polling is stopped and manager asks for card removal.
 *  The application is in charge to ask for amount and currency if needed.
 * \return
 *  STOP    Card accepted and transaction process done, polling is stop.
 *  FCT_OK  Card refused and poll the next application.
 * \header sdk30.h
 * \source entry.c
 */
// ***************************************************************************
int is_for_you_after(NO_SEGMENT no, S_TRANSIN *param_in, S_CARDOUT *param_out)
{
	//	//	// Local variables *
	//	//	// *****************
	//	S_STATE_PARAM xParam;
	//
	//	// ####################
	//	// Case of SMART card #
	//	// ####################
	//	if (param_in->support == CHIP_SUPPORT) {
	//		if (IsIPP480() == 0) {
	//			// *** ALL TERMINALS except IPP480 (Single reader) ***
	//			if(param_in->power_on_result == 0) {
	//				// Accept this card
	//				param_out->returned_state[param_out->response_number].cardappnumber = 1;
	//				param_out->returned_state[param_out->response_number].cardapp [0].priority = CARD_PROCESSED;
	//			} else
	//				// Reject the card
	//				param_out->returned_state[param_out->response_number].cardappnumber = 0;
	//		} else {
	//			// *** Only IPP480 (Dual reader) ***
	//			if(param_in->power_on_result == 0) {
	//				PSQ_read_state_param (&xParam); // Read the priority of payment (chip or track)
	//				if (xParam.CamSwipePriority == CARD_EXTENDED_SUPPORT) { // priority to Cam
	//					// accept this card
	//					param_out->returned_state[param_out->response_number].cardappnumber = 1;
	//					param_out->returned_state[param_out->response_number].cardapp [0].priority = CARD_PROCESSED;
	//				} else
	//					// reject the card due to priority to swipe => Fall back to swipe out
	//					param_out->returned_state[param_out->response_number].cardappnumber = 0;
	//			} else
	//				// reject the card due to problem => Fall back to swipe out
	//				param_out->returned_state[param_out->response_number].cardappnumber = 0;
	//		}
	//
	//		// Give my application name
	//		strcpy (param_out->returned_state[param_out->response_number].appname, zAppName) ;
	//		// Give my application number
	//		param_out->returned_state[param_out->response_number].no_appli = no;
	//		// Give my card name
	//		strcpy (param_out->returned_state[param_out->response_number].cardapp [0].cardappname, "_______________") ;
	////		strcpy (param_out->returned_state[param_out->response_number].cardapp [0].cardappname, "Chip Card") ;
	//		// Increment the response number
	//		param_out->response_number++;
	//	}

	// ############################################
	// Case of MAGNETIC 2 or 12 or 32 or 123 card #
	// ############################################
	if (    (param_in->support == TRACK2_SUPPORT)
			|| (param_in->support == TRACK12_SUPPORT)
			|| (param_in->support == TRACK32_SUPPORT)
			|| (param_in->support == TRACK123_SUPPORT) ) {
		// Accept this card
		param_out->returned_state[param_out->response_number].cardappnumber = 1;
		param_out->returned_state[param_out->response_number].cardapp [0].priority = CARD_PRIORITY;

		// Give my application name
		strcpy (param_out->returned_state[param_out->response_number].appname, zAppName) ;
		// Give my application number
		param_out->returned_state[param_out->response_number].no_appli = no;
		// Give my card name
		strcpy (param_out->returned_state[param_out->response_number].cardapp [0].cardappname, "Mag Card") ;
		// Increment the response number
		param_out->response_number++;
	}

	//	// ####################################
	//	// Case of Card Number MANUALLY entry #
	//	// ####################################
	//	if (param_in->support == OPERATOR_SUPPORT) {
	//		// Accept this card
	//		param_out->returned_state[param_out->response_number].cardappnumber = 0;
	//		param_out->returned_state[param_out->response_number].cardapp [0].priority = CARD_PROCESSED;
	//
	//		// Give my application name
	//		strcpy (param_out->returned_state[param_out->response_number].appname, zAppName) ;
	//		// Give my application number
	//		param_out->returned_state[param_out->response_number].no_appli = no;
	//		// Give my card name
	//		strcpy (param_out->returned_state[param_out->response_number].cardapp [0].cardappname, "Manual PAN") ;
	//		// Increment the response number
	//		param_out->response_number++;
	//
	//	}


	return FCT_OK;
}

// ***************************************************************************
//   int debit_non_emv(NO_SEGMENT no, S_TRANSIN *param_in, S_CARDOUT *param_out)
/**
 *  Process a non EMV chip card or a magnetic card or manual entry transaction.
 *  Support only DEBIT transaction.
 * \param    param_out (-O)
 * \param    param_out (-O)
 *    - rc_payment :
 *      PAY_OK (Transaction done), PAY_KO (Transaction rejected)
 * \return
 *  FCT_OK
 * \header sdk30.h
 * \source entry.c
 */
// ***************************************************************************
int debit_non_emv (NO_SEGMENT no, S_TRANSIN * param_in, S_TRANSOUT * param_out) {
	// Local variables *
	// *****************
	T_GL_HWIDGET hScreen=NULL;
	int iHeader, iFooter;
	char *pcSrc, *pcDst;
	char tcTrk1[128], tcTrk2[128], tcTrk3[128];
	char tcPan[19+1];
	int iRet, ret = 0;
	card key;
	word retWord = 0;
	char Amount[lenAmt + 1];
	char parentMenuSTR[3];
	char Statement[128];
	char buf[lenMnu + 1];
	char buffer[(128 * 3) + 1];

	char Pan[lenPan + 1];
	char CardHolderName[40 + 1];
	char ExpDat[lenExpDat + 1];
	char srvCd[lenSrvCd + 1];
	char *pTrk2;
	byte FallBack = 0;
	char Trk1[128 + 1];

	memset(Pan, 0, sizeof(Pan));
	memset(buf, 0, sizeof(buf));
	memset(Trk1, 0, sizeof(Trk1));
	memset(buffer, 0, sizeof(buffer));
	memset(Amount, 0, sizeof(Amount));
	memset(parentMenuSTR, 0, sizeof(parentMenuSTR));

	// Open peripherals
	// ****************
	iHeader = IsHeader();               // Save header state
	iFooter = IsFooter();               // Save footer state
	if (!IsColorDisplay()) {            // B&W terminal? // Yes
		DisplayLeds(_OFF_);             // Disable Leds
		DisplayHeader(_OFF_);           // Disable Header
		DisplayFooter(_OFF_);           // Disable Footer
	}

	hScreen = GoalCreateScreen(hGoal, thScreen, NUMBER_OF_LINES(thScreen), GL_ENCODING_UTF8);
	CHECK(hScreen!=NULL, lblKO);
	iRet = GoalClrScreen(hScreen, GL_COLOR_BLACK, 0, false);                     // Create and clear screen
	CHECK(iRet>=0, lblKO);


	memset(buf, 0, sizeof(buf));
	num2dec(buf, mnuSale, 0);
	MAPPUTSTR(traMnuItm, buf, lblKO);

	memset(buf, 0, sizeof(buf));

	//get current transaction date and time
	getDateTime(Statement); //Temporarily used var:Statement for usability

	//SaveTransaction type if Dr or Cr
	memset(Statement, 0, sizeof(Statement));
	Telium_Sprintf (Statement, "SELECT DrCr FROM AppMenus WHERE MenuId = '%d';", mnuSale);
	ret = Sqlite_Run_Statement_MultiRecord(Statement, buf);
	MAPPUTSTR(traDrCr, buf, lblKO);

	memset(Statement, 0, sizeof(Statement));
	memset(buf, 0, sizeof(buf));
	Telium_Sprintf (Statement, "SELECT MenuName FROM AppMenus WHERE MenuId = '%d';", mnuSale);
	ret = Sqlite_Run_Statement_MultiRecord(Statement, buf);
	CHECK(ret > 0, lblKO);
	MAPPUTSTR(traCtx, buf, lblKO);

	//default initial data
	MAPPUTSTR(traCurrencyLabel, "TZS", lblKO);
	MAPPUTSTR(emvTrnCurCod,"020834",lblKO);
	MAPPUTBYTE(appReversalFlag, 0, lblKO);
	ManageSTAN();

	//sale data
	MAPPUTSTR(traRqsBitMap, "083038078020C80006",lblKO);
	MAPPUTSTR(traRqsMTI, "020200",lblKO);
	MAPPUTSTR(traRqsProcessingCode, "000000",lblKO);

	// ####################
	// Case of SMART card #
	// ####################
	if ( param_in->support == CHIP_SUPPORT ) {
		MAPPUTSTR(traEntMod, "c", lblKO);

		////SELECT TRANSACTION
		if (param_in->transaction == DEBIT_TR) {             // Supported by debit_non_emv
			key = mnuSale;
		} else if (param_in->transaction == CREDIT_TR) {     // Supported by select_function
			key = mnuWithdrawal;
		} else if (param_in->transaction == CANCEL_TR) {     // Supported by select_function
			key = mnuVoid;
		} else if (param_in->transaction == EXT_CANCEL_TR) { // Supported by select_function
			key = mnuReversal;
		} else {
			key = mnuPreaut;                                 // Supported by select_function
		}

		memset(parentMenuSTR, 0, sizeof(parentMenuSTR));
		num2dec(parentMenuSTR, key,0);
		MAPPUTSTR(traMnuItm, parentMenuSTR, lblKO);

		////PREPARE AMOUNT IF AVAILABLE
		if (param_in->amount != 0) {                       // Print amount and currency
			Telium_Sprintf(Amount, "%d", param_in->amount);
			mapPutStr(traAmt, Amount);
			//Telium_Pprintf("Currency: %.3s %.3s %d\n", param_in->currency.nom, param_in->currency.code, param_in->currency.posdec);
		}

		goto lblFallBack;

		////KILL EMV PREVIOUS SELECTION
		emvStop();

		////PROCESS TRANSACTION AS NORMAL
		Application_Do_Transaction_Idle();

		goto lblNoMag;
	}

	// ############################################
	// Case of MAGNETIC 2 or 12 or 32 or 123 card #
	// ############################################
	if (    (param_in->support == TRACK2_SUPPORT) || (param_in->support == TRACK12_SUPPORT) || (param_in->support == TRACK32_SUPPORT) || (param_in->support == TRACK123_SUPPORT) ) {
		MAPPUTSTR(traEntMod, "m", lblKO);

		// Retrieve and analyze track1
		// ===========================
		if (param_in->cr_iso1 == ISO_OK) {              // *** Magnetic track1 ***

			memset(Pan, 0, sizeof(Pan));
			memset(ExpDat, 0, sizeof(ExpDat));
			memset(srvCd, 0, sizeof(srvCd));
			memset(CardHolderName, 0, sizeof(CardHolderName));
			pcSrc = (char*)param_in->track1;
			memset(tcTrk1, 0, sizeof(tcTrk1));
			pcDst = tcTrk1;                            // Return track1
			while(*pcSrc) {                            // Find start sentinel
				if(*pcSrc++ == '%')
					break;
			}
			while(*pcSrc) {                            // Copy all data between start and end sentinels
				if(*pcSrc == '?')
					break;
				*pcDst++ = *pcSrc++;
			}

		}
		MAPPUTSTR(traTrk1, tcTrk1, lblKO);
		strncat(buffer, tcTrk1, 128);

		// Retrieve and analyze track2
		// ===========================
		if (param_in->cr_iso2 == ISO_OK) {              // *** Magnetic track2 ***
			pcSrc = (char*)param_in->track2;
			memset(tcTrk2, 0, sizeof(tcTrk2));
			pcDst = tcTrk2;                            // Return track2
			while(*pcSrc) {                            // Find start sentinel
				if(*pcSrc++ == 'B')
					break;
			}
			while(*pcSrc) {                            // Copy all data between start and end sentinels
				if(*pcSrc == 'F')
					break;
				if(*pcSrc == 'D')
					*pcSrc = '=';
				*pcDst++ = *pcSrc++;
			}
		}
		MAPPUTSTR(traTrk2, tcTrk2, lblKO);
		MAPPUTSTR(traTrk22, tcTrk2, lblKO);
		strncat(&buffer[128], tcTrk2, 128);

		// Retrieve and analyze track3
		// ===========================
		if (param_in->cr_iso3 == ISO_OK) {              // *** Magnetic track3 ***
			pcSrc = (char*)param_in->track3;
			memset(tcTrk3, 0, sizeof(tcTrk3));
			pcDst = tcTrk3;                            // Return track3
			while(*pcSrc) {                            // Find start sentinel
				if(*pcSrc++ == 'B')
					break;
			}
			while(*pcSrc) {                            // Copy all data between start and end sentinels
				if(*pcSrc == 'F')
					break;
				if(*pcSrc == 'D')
					*pcSrc = '=';
				*pcDst++ = *pcSrc++;
			}
		}
		MAPPUTSTR(traTrk21, tcTrk3, lblKO);
		strncat(&buffer[256], tcTrk3, 128);
	}

	// ####################################
	// Case of Card Number MANUALLY entry #
	// ####################################
	if ( param_in->support == OPERATOR_SUPPORT ) {
		MAPPUTSTR(traEntMod, "k", lblKO);

		// Retrieve and analyze entry (track2)
		// ===================================
		pcSrc = (char*)param_in->track2;
		pcDst = tcTrk2;                                // Return track2
		while(*pcSrc) {                                // Find start sentinel
			if(*pcSrc++ == 'B')
				break;
		}
		while(*pcSrc) {                                // Copy all data between start and end sentinels
			if(*pcSrc == 'F')
				break;
			if(*pcSrc == 'D')
				*pcSrc = '=';
			*pcDst++ = *pcSrc++;
		}

		strncat(buffer, tcTrk1, 128);
		MAPPUTSTR(traTrk2, tcTrk2, lblKO);
		MAPPUTSTR(traTrk22, tcTrk2, lblKO);
		strncat(&buffer[128], tcTrk2, 128);
		strncat(&buffer[256], tcTrk3, 128);
	}


	///================== DO Magnetic transaction ================
	if(*buffer != 0) {
		memcpy(Trk1, buffer, 128);
		ret = fmtTok(0, Trk1, "^");
		CHECK(ret <= sizeof(Trk1), lblInvalidTrk);
		memset(Trk1, 0, sizeof(Trk1));
		memcpy(Trk1, &buffer[ret + 1], 128 - ret);
		ret = fmtTok(CardHolderName, Trk1, "^");  // Retrieve cardholder name from track1
	}

	MAPPUTSTR(traTrk1, CardHolderName, lblKO);

	pTrk2 = &buffer[128];
	MAPPUTSTR(traTrk2, pTrk2, lblKO);
	ret = fmtTok(0, pTrk2, "=");
	CHECK(ret <= lenPan, lblInvalidTrk);  // Search separator '='
	ret = fmtTok(Pan, pTrk2, "=");    // Retrieve Pan from track 2
	VERIFY(ret <= lenPan);
	MAPPUTSTR(traPan, Pan, lblKO);

	pTrk2 += ret;         // Continue to analyse track 2
	CHECK(*pTrk2 == '=', lblInvalidTrk);  // Should be separator
	pTrk2++;
	ret = fmtSbs(ExpDat, pTrk2, 0, lenExpDat);    // Retrieve expiration date from track 2
	VERIFY(ret == 4);
	MAPPUTSTR(traExpDat, ExpDat, lblKO);

	pTrk2++;pTrk2++;pTrk2++;pTrk2++;
	ret = fmtSbs(srvCd, pTrk2, 0, 3);    // Retrieve expiration date from track 2
	VERIFY(ret == 3);
	MAPPUTSTR(traSrvCd,srvCd, lblKO);
	mapGetByte(appFallback, FallBack);

	if(strncmp(srvCd,"2",1)==0 ||strncmp(srvCd,"6",1)==0) {   //first character indicates presence or absence of chip
		if (FallBack != 1) {
			goto lblUseChip;
		}
	}

	// Close standard peripherals
	ClosePeripherals();

	retWord = ApplicationSelectCurrency();
	ret = (int)retWord;
	if (retWord < 1) {
		GL_Dialog_Message(hGoal, NULL, "Transaction Cancelled!!", GL_ICON_ERROR, GL_BUTTON_NONE, 100);
		goto lblKO;
	}

	//process Magstripe Transaction
	Magnetic_Card_Transaction();

	///===========================================================

	lblNoMag:
	// ****************************************************************
	// Information returned to Cash Register
	// (See ServiceCall100 to communicate with connected Cash Register)
	// ****************************************************************
	param_out->noappli = no;                              // Return application number
	param_out->rc_payment = PAY_OK;                       // Transaction done

	FallBack = 0;
	mapPutByte(appFallback, FallBack);

	goto lblEnd;

	// Errors treatment
	// ****************
	lblUseChip:
	GL_Dialog_Message(hGoal, NULL, "USE CHIP", GL_ICON_ERROR, GL_BUTTON_NONE, 2*1000);
	ret = -1;
	param_out->noappli = no;         // Return application number
	param_out->rc_payment = PAY_OK;  // Transaction failed
	goto lblEnd;

	lblFallBack:
	FallBack = 1;
	mapPutByte(appFallback, FallBack);
	GL_Dialog_Message(hGoal, NULL, "FALL BACK \nUSE MAG STRIPE", GL_ICON_ERROR, GL_BUTTON_NONE, 2*1000);
	ret = -1;
	param_out->noappli = no;         // Return application number
	param_out->rc_payment = PAY_OK;  // Transaction failed
	goto lblEnd;


	lblInvalidTrk:
	GL_Dialog_Message(hGoal, NULL, "INVALID TRACK DATA", GL_ICON_ERROR, GL_BUTTON_NONE, 2*1000);
	ret = -1;
	param_out->noappli = no;         // Return application number
	param_out->rc_payment = PAY_OK;  // Transaction failed
	goto lblEnd;

	lblKO:                               // None-classified low level error
	GL_Dialog_Message(hGoal, NULL, "PROCESSING ERROR", GL_ICON_ERROR, GL_BUTTON_NONE, 2*1000);
	param_out->noappli = no;         // Return application number
	param_out->rc_payment = PAY_KO;  // Transaction failed
	goto lblEnd;
	lblEnd:

	DisplayLeds(_ON_);               // Enable Leds
	DisplayHeader(iHeader);          // Restore Header
	DisplayFooter(iFooter);          // Restore Footer

	memset(tcPan, 0, sizeof(tcPan));
	parseStr('=', tcPan, tcTrk2, sizeof(tcPan));
	memcpy(param_out->card_holder_nb, tcPan, 19);         // Return card holder number (Pan)

	//Clear the transaction Buffers of the transaction
	traReset();

	return FCT_OK;
}

// ***************************************************************************
//   int select_function(NO_SEGMENT no, S_TRANSIN *param_in, S_CARDOUT *param_out)
/**
 * Process a non EMV chip card or a magnetic card or manual entry transaction.
 * Support CREDIT, CANCEL and PRE AUTHORIZATION Transactions.
 * \param    param_out (-O)
 *    - rc_payment :
 *      PAY_OK (Transaction done), PAY_KO (Transaction rejected)
 * \return
 *  FCT_OK
 * \header sdk30.h
 * \source entry.c
 */
// ***************************************************************************
int select_function (NO_SEGMENT no, S_TRANSIN *param_in, S_TRANSOUT *param_out) {
	// Local variables *
	// *****************
	int iRet;

	// Same prototype as debit_non_emv transaction
	// *******************************************
	iRet = debit_non_emv (no, param_in, param_out);

	return iRet;
}

// ***************************************************************************
//   int fall_back (NO_SEGMENT no, S_TRANSIN *param_in, S_TRANSOUT *param_out)
/**
 * This function is used to process specific fallback (chip to stripe) instead
 * of EMV standard fallback (for iPP480 only dual reader)
 * \return
 *  FCT_OK
 * \header sdk30.h
 * \source entry.c
 */
// ***************************************************************************
int fall_back (NO_SEGMENT no, S_TRANSIN *param_in, S_TRANSOUT *param_out) {
	// Local variables *
	// *****************
	int iHeader, iFooter;
	Telium_File_t *hMag31=NULL, *hMag2=NULL, *hMag3=NULL;
	Telium_File_t *hSmc=NULL;
	byte ucSta;
	int iSta;
	ST_DATA xData;
	ST_IO_TRANS xIOTrans;
	byte ucLen=0;
	int iCode;
	int localDisplay = 0;

	// Open peripherals
	// ****************
	iHeader = IsHeader();                       // Save header state
	iFooter = IsFooter();                       // Save footer state
	if (!IsColorDisplay()) {                    // B&W terminal?
		// Yes
		DisplayLeds(_OFF_);                     // Disable Leds
		DisplayHeader(_OFF_);                   // Disable Header
		DisplayFooter(_OFF_);                   // Disable Footer
	}

	hDsp = Telium_Stdperif("DISPLAY", NULL);    // Check "display" peripheral already opened? (done by Manager)
	if (hDsp == NULL) {                          // No, then open it
		hDsp = Telium_Fopen("DISPLAY", "w*");   // Open "display" channel
		localDisplay = 1;
	}

	if (IsISO1() == 1)
		hMag31 = Telium_Fopen("SWIPE31", "r*"); // Open "mag1" peripheral
	if (IsISO2() == 1)
		hMag2 = Telium_Fopen("SWIPE2", "r*");   // Open "mag2" peripheral
	if (IsISO3() == 1)
		hMag3 = Telium_Fopen("SWIPE3", "r*");   // Open "mag3" peripheral

	hSmc = Telium_Stdperif("CAM0", NULL);
	if (hSmc == NULL) {
		hSmc = Telium_Fopen("CAM0", "rw*");     // Open the main Cam peripheral
	}

	// Display remove card and check if card removed
	// *********************************************
	Telium_Status (hSmc, &ucSta);
	switch (ucSta)
	{
	case CAM_PRESENT:                           // Card present
		GL_Dialog_Message(hGoal, NULL, "REMOVE CARD\nPLEASE", GL_ICON_NONE, GL_BUTTON_NONE, 0);
		Telium_Power_down(hSmc);                // Card power off
		Telium_Ttestall(CAM0, 0);               // Wait for removal
		break;
	default:                                    // Card absent
		break;
	}
	Telium_Fclose(hSmc);                        // Close "cam" channel

	// ISO events are already recognized on forced removed Card which keeps the event (if occurred)
	// ********************************************************************************************
	iSta = Telium_Ttestall(SWIPE31 | SWIPE2 | SWIPE3, 50);           // Wait for the first event ISO1
	iSta |= Telium_Ttestall(iSta ^ (SWIPE31 | SWIPE2 | SWIPE3), 10); // Wait for the second event ISO2
	iSta |= Telium_Ttestall(iSta ^ (SWIPE31 | SWIPE2 | SWIPE3), 10); // Wait for the third event ISO3

	// Retrieve and analyze ISO1
	// =========================
	xData.bFlagIso1=FALSE;
	if(iSta & SWIPE31)
	{
		xData.iRetIso1 = Telium_Is_iso1(hMag31, &ucLen, xData.tcTrk1);
		xData.bFlagIso1=TRUE;
	}

	// Retrieve and analyze ISO2
	// =========================
	xData.bFlagIso2=FALSE;
	if(iSta & SWIPE2)
	{
		xData.iRetIso2 = Telium_Is_iso2(hMag2, &ucLen, xData.tcTrk2);
		xData.bFlagIso2=TRUE;
	}

	// Retrieve and analyze ISO3
	// =========================
	xData.bFlagIso3=FALSE;
	if(iSta & SWIPE3)
	{
		xData.iRetIso3 = Telium_Is_iso3(hMag3, &ucLen, xData.tcTrk3);
		xData.bFlagIso3=TRUE;
	}

	if(hMag2)
		Telium_Fclose(hMag2);                  // Close "mag2" channel
	if(hMag3)
		Telium_Fclose(hMag3);                  // Close "mag3" channel
	if(hMag31)
		Telium_Fclose(hMag31);                 // Close "mag31" channel

	// Call servicecall 100
	// ********************
	memcpy (&xIOTrans.param_in, param_in, sizeof(S_TRANSIN));
	memcpy (&xIOTrans.param_out, param_out, sizeof(S_TRANSOUT));

	xIOTrans.param_in.cr_iso1 = xData.iRetIso1;
	xIOTrans.param_in.cr_iso2 = xData.iRetIso2;
	xIOTrans.param_in.cr_iso3 = xData.iRetIso3;

	memcpy(xIOTrans.param_in.track1, xData.tcTrk1, sizeof(TRACK1_BUFFER));
	memcpy(xIOTrans.param_in.track2, xData.tcTrk2, sizeof(TRACK2_BUFFER));
	memcpy(xIOTrans.param_in.track3, xData.tcTrk3, sizeof(TRACK3_BUFFER));

	xIOTrans.param_in.support = TRACK123_SUPPORT;
	xIOTrans.param_in.fallback = 0;

	Telium_ServiceCall(TYPE_GESTIONNAIRE, 100, sizeof(ST_IO_TRANS), &xIOTrans, &iCode);

	// ****************************************************************
	// Information returned to Cash Register
	// (See ServiceCall100 to communicate with connected Cash Register)
	// ****************************************************************
	param_out->noappli = xIOTrans.param_out.noappli;                          // Return application number
	param_out->rc_payment = xIOTrans.param_out.rc_payment;                    // Transaction done
	memcpy(param_out->card_holder_nb, xIOTrans.param_out.card_holder_nb, 19); // Return card holder number (Pan)

	DisplayLeds(_ON_);                          // Enable Leds
	DisplayHeader(iHeader);                     // Restore Header
	DisplayFooter(iFooter);                     // Restore Footer

	if (localDisplay == 1) {
		Telium_Fclose(hDsp);                     // Close "display" peripheral
		hDsp=NULL;
	}
	return FCT_OK;
}

//===========================================================================
//! \brief This function tests if the EMV transaction can be run.
//! \param[in] AppliNum : application number.
//! \param[in] pParamIn : data received from manager. (not used).
//! \param[out] pParamOut : structure containing the response :
//! - if the field cardappnumber is != from 0 then custom application wants to
//! process the EMV card. In this case, the fields cardapp.priority and cardapp.cardappname
//! have to be fulfilled.
//! - if the field cardappnumber = 0 then custom application doesn't wants to process the EMV card.
//===========================================================================
int is_card_emv_for_you(NO_SEGMENT appliId, S_AID *paramIn, S_CARDOUT *paramOut) {
	// To avoid warnings because 'paramIn' is not used
	(void)paramIn;
	byte emvBillerMode = 0;

	mapGetByte(appTerminalMode, emvBillerMode);

	// Initialise the output parameter
	memclr(&paramOut->returned_state[paramOut->response_number], sizeof(paramOut->returned_state[paramOut->response_number]));

	strcpy(paramOut->returned_state[paramOut->response_number].appname, zAppName);
	paramOut->returned_state[paramOut->response_number].no_appli = appliId;


	switch (emvBillerMode) {
	case 1: //Terminal behavior of a Biller terminal

		// This function is only called when the AID is supported by the application
		// So we always what to manage the AID with a normal priority
		paramOut->returned_state[paramOut->response_number].cardappnumber = 0;
		//	paramOut->returned_state[paramOut->response_number].cardapp[0].priority = CARD_RECOGNIZED;
		paramOut->returned_state[paramOut->response_number].cardapp[0].priority = CARD_REJECTED;

		break;
	case 0:// Terminal behavior of normal Terminal
	default:

		// This function is only called when the AID is supported by the application
		// So we always what to manage the AID with a normal priority
		paramOut->returned_state[paramOut->response_number].cardappnumber = 1;
		//	paramOut->returned_state[paramOut->response_number].cardapp[0].priority = CARD_RECOGNIZED;
		paramOut->returned_state[paramOut->response_number].cardapp[0].priority = CARD_PRIORITY;

		break;
	}

	strcpy(paramOut->returned_state[paramOut->response_number].cardapp[0].cardappname, zAppName);
	paramOut->response_number++;

	return FCT_OK;
}

//! \brief The Telium Manager calls this service when an EMV card is inserted to know the list of supported AIDs.
//! \param[in] appliId The application ID.
//! \param[in] paramIn Parameters of the transaction (amount, ...).
//! \param[in] paramOut The list of supported AIDs.
//! \return Always \a FCT_OK.
//! \sa Telium Manager reference documentation.
int give_aid(NO_SEGMENT appliId, S_TRANSIN *paramIn, _DEL_ *paramOut) {
	static const unsigned char aid1[] =  { 0xA0, 0x00, 0x00, 0x00, 0x03, 0x10, 0x10};
	static const unsigned char aid2[] =  { 0xA0, 0x00, 0x00, 0x00, 0x03, 0x20, 0x10};
	static const unsigned char aid3[] =  { 0xA0, 0x00, 0x00, 0x00, 0x03, 0x30, 0x10};
	static const unsigned char aid4[] =  { 0xA0, 0x00, 0x00, 0x00, 0x03, 0x80, 0x10, 0x01};
	static const unsigned char aid14[] = { 0xA0, 0x00, 0x00, 0x00, 0x03, 0x80, 0x10, 0x02};
	static const unsigned char aid15[] = { 0xA0, 0x00, 0x00, 0x00, 0x03, 0x80, 0x10, 0x08};

	static const unsigned char aid5[] =  { 0xA0, 0x00, 0x00, 0x00, 0x04, 0x10, 0x10};
	static const unsigned char aid6[] =  { 0xA0, 0x00, 0x00, 0x00, 0x04, 0x10, 0x10, 0x01};
	static const unsigned char aid7[] =  { 0xA0, 0x00, 0x00, 0x00, 0x04, 0x10, 0x10, 0x02};
	static const unsigned char aid8[] =  { 0xA0, 0x00, 0x00, 0x00, 0x04, 0x30, 0x60};
	//	static const unsigned char aid9[] =  { 0xA0, 0x00, 0x00, 0x00, 0x04, 0x60, 0x00};

	static const unsigned char aid10[] = { 0xA0, 0x00, 0x00, 0x00, 0x25, 0x01};
	static const unsigned char aid11[] = { 0xA0, 0x00, 0x00, 0x00, 0x42, 0x10, 0x10};
	static const unsigned char aid12[] = { 0xA0, 0x00, 0x00, 0x05, 0x76, 0x01, 0x01};
	static const unsigned char aid13[] = { 0xA0, 0x00, 0x00, 0x05, 0x76, 0x10, 0x10};

	///CUP and UPI AID
	static const unsigned char aid16[] = { 0xA0, 0x00, 0x00, 0x03, 0x33, 0x01, 0x01, 0x01};
	static const unsigned char aid17[] = { 0xA0, 0x00, 0x00, 0x03, 0x33, 0x01, 0x01, 0x02};
	static const unsigned char aid18[] = { 0xA0, 0x00, 0x00, 0x03, 0x33, 0x01, 0x01, 0x03};
	static const unsigned char aid19[] = { 0xA0, 0x00, 0x00, 0x03, 0x33, 0x01, 0x01, 0x06};
	static const unsigned char aid20[] = { 0xA0, 0x00, 0x00, 0x03, 0x33, 0x01, 0x01, 0x08};


	TLV_TREE_NODE outputTlvTree;
	unsigned char byte;
	int result;

	// To avoid warnings because 'paramIn' is not used
	(void)paramIn;

	ASSERT(paramIn != NULL);
	ASSERT(paramOut != NULL);

	// Initialise the output DEL
	_DEL_Init(paramOut);

	// TODO: Check input parameters 'paramIn'

	// Fill the output DEL
	outputTlvTree = TlvTree_New(0);
	if (outputTlvTree != NULL) {
		result = TRUE;

		// Add the application ID
		if (TlvTree_AddChild(outputTlvTree, TAG_TERM_APP_NUMBER, &appliId, 1) == NULL)
			result = FALSE;

		// Do not force manual selection
		byte = VALUE_GIVE_AID_MANUAL_SELECTION_FORCE_CONFIRMATION;	// Possible values are: VALUE_GIVE_AID_MANUAL_SELECTION_FORCE_CONFIRMATION | VALUE_GIVE_AID_MANUAL_SELECTION_FORCE_SELECTION
		if (TlvTree_AddChild(outputTlvTree, TAG_GIVE_AID_MANUAL_SELECTION, &byte, 1) == NULL)
			result = FALSE;

		///---------------- Add the Aid with siffixes the standard ones --------------

		// Add the Mastercard AID
		if (TlvTree_AddChild(outputTlvTree, TAG_GIVE_AID_AID_VALUE, &aid14, sizeof(aid14)) == NULL) result = FALSE;
		byte = 0;	// Allow partial matching. Set it to 0x80 to forbid partial matching.
		if (TlvTree_AddChild(outputTlvTree, TAG_GIVE_AID_ASI, &byte, 1) == NULL) result = FALSE;

		// Add the Mastercard AID
		if (TlvTree_AddChild(outputTlvTree, TAG_GIVE_AID_AID_VALUE, &aid15, sizeof(aid15)) == NULL) result = FALSE;
		byte = 0;	// Allow partial matching. Set it to 0x80 to forbid partial matching.
		if (TlvTree_AddChild(outputTlvTree, TAG_GIVE_AID_ASI, &byte, 1) == NULL) result = FALSE;

		// Add the Mastercard AID
		if (TlvTree_AddChild(outputTlvTree, TAG_GIVE_AID_AID_VALUE, &aid4, sizeof(aid4)) == NULL) result = FALSE;
		byte = 0;	// Allow partial matching. Set it to 0x80 to forbid partial matching.
		if (TlvTree_AddChild(outputTlvTree, TAG_GIVE_AID_ASI, &byte, 1) == NULL) result = FALSE;

		// Add the Mastercard AID
		if (TlvTree_AddChild(outputTlvTree, TAG_GIVE_AID_AID_VALUE, &aid6, sizeof(aid6)) == NULL) result = FALSE;
		byte = 0x80;	// Allow partial matching. Set it to 0x80 to forbid partial matching.
		if (TlvTree_AddChild(outputTlvTree, TAG_GIVE_AID_ASI, &byte, 1) == NULL) result = FALSE;

		// Add the Mastercard AID
		if (TlvTree_AddChild(outputTlvTree, TAG_GIVE_AID_AID_VALUE, &aid7, sizeof(aid7)) == NULL) result = FALSE;
		byte = 0x80;	// Allow partial matching. Set it to 0x80 to forbid partial matching.
		if (TlvTree_AddChild(outputTlvTree, TAG_GIVE_AID_ASI, &byte, 1) == NULL) result = FALSE;


		///---------------- Now add the standard ones --------------
		// Add the VISA AID
		if (TlvTree_AddChild(outputTlvTree, TAG_GIVE_AID_AID_VALUE, &aid1, sizeof(aid1)) == NULL) result = FALSE;
		byte = 0;	// Allow partial matching. Set it to 0x80 to forbid partial matching.
		if (TlvTree_AddChild(outputTlvTree, TAG_GIVE_AID_ASI, &byte, 1) == NULL) result = FALSE;

		// Add the Mastercard AID
		if (TlvTree_AddChild(outputTlvTree, TAG_GIVE_AID_AID_VALUE, &aid2, sizeof(aid2)) == NULL) result = FALSE;
		byte = 0;	// Allow partial matching. Set it to 0x80 to forbid partial matching.
		if (TlvTree_AddChild(outputTlvTree, TAG_GIVE_AID_ASI, &byte, 1) == NULL) result = FALSE;

		// Add the Mastercard AID
		if (TlvTree_AddChild(outputTlvTree, TAG_GIVE_AID_AID_VALUE, &aid3, sizeof(aid3)) == NULL) result = FALSE;
		byte = 0;	// Allow partial matching. Set it to 0x80 to forbid partial matching.
		if (TlvTree_AddChild(outputTlvTree, TAG_GIVE_AID_ASI, &byte, 1) == NULL) result = FALSE;

		// Add the Mastercard AID
		if (TlvTree_AddChild(outputTlvTree, TAG_GIVE_AID_AID_VALUE, &aid5, sizeof(aid5)) == NULL) result = FALSE;
		byte = 0;	// Allow partial matching. Set it to 0x80 to forbid partial matching.
		if (TlvTree_AddChild(outputTlvTree, TAG_GIVE_AID_ASI, &byte, 1) == NULL) result = FALSE;

		// Add the Mastercard AID
		if (TlvTree_AddChild(outputTlvTree, TAG_GIVE_AID_AID_VALUE, &aid8, sizeof(aid8)) == NULL) result = FALSE;
		byte = 0;	// Allow partial matching. Set it to 0x80 to forbid partial matching.
		if (TlvTree_AddChild(outputTlvTree, TAG_GIVE_AID_ASI, &byte, 1) == NULL) result = FALSE;

		//		// Add the Cirrus AID
		//		if (TlvTree_AddChild(outputTlvTree, TAG_GIVE_AID_AID_VALUE, &aid9, sizeof(aid9)) == NULL) result = FALSE;
		//		byte = 0;	// Allow partial matching. Set it to 0x80 to forbid partial matching.
		//		if (TlvTree_AddChild(outputTlvTree, TAG_GIVE_AID_ASI, &byte, 1) == NULL) result = FALSE;

		// Add the Mastercard AID
		if (TlvTree_AddChild(outputTlvTree, TAG_GIVE_AID_AID_VALUE, &aid10, sizeof(aid10)) == NULL) result = FALSE;
		byte = 0;	// Allow partial matching. Set it to 0x80 to forbid partial matching.
		if (TlvTree_AddChild(outputTlvTree, TAG_GIVE_AID_ASI, &byte, 1) == NULL) result = FALSE;

		// Add the Mastercard AID
		if (TlvTree_AddChild(outputTlvTree, TAG_GIVE_AID_AID_VALUE, &aid11, sizeof(aid11)) == NULL) result = FALSE;
		byte = 0;	// Allow partial matching. Set it to 0x80 to forbid partial matching.
		if (TlvTree_AddChild(outputTlvTree, TAG_GIVE_AID_ASI, &byte, 1) == NULL) result = FALSE;

		// Add the Mastercard AID
		if (TlvTree_AddChild(outputTlvTree, TAG_GIVE_AID_AID_VALUE, &aid12, sizeof(aid12)) == NULL) result = FALSE;
		byte = 0;	// Allow partial matching. Set it to 0x80 to forbid partial matching.
		if (TlvTree_AddChild(outputTlvTree, TAG_GIVE_AID_ASI, &byte, 1) == NULL) result = FALSE;

		// Add the Mastercard AID
		if (TlvTree_AddChild(outputTlvTree, TAG_GIVE_AID_AID_VALUE, &aid13, sizeof(aid13)) == NULL) result = FALSE;
		byte = 0;	// Allow partial matching. Set it to 0x80 to forbid partial matching.
		if (TlvTree_AddChild(outputTlvTree, TAG_GIVE_AID_ASI, &byte, 1) == NULL) result = FALSE;


		///---------------- Now add the CUP / UPI --------------
		// Add the UPI AID
		if (TlvTree_AddChild(outputTlvTree, TAG_GIVE_AID_AID_VALUE, &aid16, sizeof(aid16)) == NULL) result = FALSE;
		byte = 0;	// Allow partial matching. Set it to 0x80 to forbid partial matching.
		if (TlvTree_AddChild(outputTlvTree, TAG_GIVE_AID_ASI, &byte, 1) == NULL) result = FALSE;

		// Add the UPI AID
		if (TlvTree_AddChild(outputTlvTree, TAG_GIVE_AID_AID_VALUE, &aid17, sizeof(aid17)) == NULL) result = FALSE;
		byte = 0;	// Allow partial matching. Set it to 0x80 to forbid partial matching.
		if (TlvTree_AddChild(outputTlvTree, TAG_GIVE_AID_ASI, &byte, 1) == NULL) result = FALSE;

		// Add the UPI AID
		if (TlvTree_AddChild(outputTlvTree, TAG_GIVE_AID_AID_VALUE, &aid18, sizeof(aid18)) == NULL) result = FALSE;
		byte = 0;	// Allow partial matching. Set it to 0x80 to forbid partial matching.
		if (TlvTree_AddChild(outputTlvTree, TAG_GIVE_AID_ASI, &byte, 1) == NULL) result = FALSE;

		// Add the UPI AID
		if (TlvTree_AddChild(outputTlvTree, TAG_GIVE_AID_AID_VALUE, &aid19, sizeof(aid19)) == NULL) result = FALSE;
		byte = 0;	// Allow partial matching. Set it to 0x80 to forbid partial matching.
		if (TlvTree_AddChild(outputTlvTree, TAG_GIVE_AID_ASI, &byte, 1) == NULL) result = FALSE;
		// Add the UPI AID
		if (TlvTree_AddChild(outputTlvTree, TAG_GIVE_AID_AID_VALUE, &aid20, sizeof(aid20)) == NULL) result = FALSE;
		byte = 0;	// Allow partial matching. Set it to 0x80 to forbid partial matching.
		if (TlvTree_AddChild(outputTlvTree, TAG_GIVE_AID_ASI, &byte, 1) == NULL) result = FALSE;

		if (result) {
			// TODO: Be careful if 'outputTlvTree' contains more than 50 tags.
			// TODO: In such case, a special treatment must be applied (see Telium Manager documentation).

			// Copy the TlvTree into the output DEL
			EPSTOOL_TlvTree_AddToDelValue(paramOut, outputTlvTree);
		}

		EPSTOOL_TlvTree_Release(&outputTlvTree);
	}

	return FCT_OK;

}


//===========================================================================
//! \brief This function performs a debit transaction on an EMV chip card.
//! \param[in] AppliNum : application number.
//! \param[in] param_in : DEL from manager. This DEL contains mandatory parameters
//! selected AID, application label. And may contain optionally parameters
//! application preferred name, language preference, issuer code table index,
//! application priority indicator.
//! \param[out] param_out : structure containing the payment return code :
//! - PAY_OK : if the payment successfully performed.
//! - PAY_KO : otherwise.
//===========================================================================
int debit_emv(NO_SEGMENT appliId, S_TRANSIN *paramIn, S_TRANSOUT *paramOut) {
	int doTransaction;
	TLV_TREE_NODE inputTlvTree;
	TLV_TREE_NODE node;
	TLV_TREE_NODE tmpNode;
	EPSTOOL_Data_t data;
	int deleteTag;
	unsigned char buffer[4];
	int previousUiState;
	char Statement[128];
	char buf[lenMnu + 1];
	int ret = 0;
	byte BillerMode = 0;

	ret = OpenPeripherals(); // Open standard peripherals just in case

	// Initialise the output parameter
	memclr(paramOut, sizeof(*paramOut));
	paramOut->noappli = appliId;
	paramOut->rc_payment = PAY_KO;

	doTransaction = FALSE;
	// Convert the input DEL with Telium Manager tags into a TlvTree containing EMV tags
	if (EPSTOOL_TlvTree_NewFromDel(&inputTlvTree, 0, &paramIn->del)) {
		node = EPSTOOL_TlvTree_GetFirstChildData(inputTlvTree, &data);
		while(node != NULL) {
			deleteTag = FALSE;
			switch(data.tag) {
			case TAG_GIVE_AID_AID_VALUE:
				TlvTree_SetTag(node, TAG_AID_ICC);
				// The AID is given by the Manager, so we can do the transaction
				doTransaction = TRUE;

				memset(Statement, 0, sizeof(Statement)); //Temporarily used this VAR
				bin2hex(Statement, data.value, data.length);
				mapPut(traPreselectedAid, Statement, strlen(Statement));
				break;
			case TAG_GIVE_AID_APP_NAME:
				TlvTree_SetTag(node, TAG_APPLICATION_LABEL);
				break;
			case TAG_GIVE_AID_APP_PRIO:
				TlvTree_SetTag(node, TAG_APPLI_PRIOR_IND);
				break;
			case TAG_GIVE_AID_PREF_LANGUAGE:
				TlvTree_SetTag(node, TAG_LANGUAGE_PREFERENCE);
				break;
			default:
				// We don't know this tag => delete it
				deleteTag = TRUE;
				break;
			}

			if (deleteTag) {
				// Delete the tag and go to the next tag
				tmpNode = EPSTOOL_TlvTree_GetNextData(node, &data);
				TlvTree_Release(node);
				node = tmpNode;
			} else {
				// Next tag
				node = EPSTOOL_TlvTree_GetNextData(node, &data);
			}
		}

		// Add the amount
		if ((doTransaction) && (paramIn->entry != NO_ENTRY)) {
			// TODO: Check that the currency is allowed and coherent with parameters

			EPSTOOL_Convert_ULongToEmvBin(paramIn->amount, buffer);
			if (TlvTree_AddChild(inputTlvTree, TAG_AMOUNT_AUTH_BIN, buffer, 4) == NULL) {
				doTransaction = FALSE;
			}

			buffer[0] = (paramIn->currency.code[0] - '0');
			buffer[1] = ((paramIn->currency.code[1] - '0') << 4) | (paramIn->currency.code[2] - '0');
			if (TlvTree_AddChild(inputTlvTree, TAG_TRANSACTION_CURRENCY_CODE, buffer, 2) == NULL) {
				doTransaction = FALSE;
			}

			buffer[0] = (unsigned char)paramIn->currency.posdec;
			if (TlvTree_AddChild(inputTlvTree, TAG_TRANSACTION_CURRENCY_EXP, buffer, 1) == NULL) {
				doTransaction = FALSE;
			}

			//  - TAG_AMOUNT_OTHER_BIN (set to 0)

			// TODO: Check the amount if required (to forbid zero amount...)
		}

		// By default, we start a standard transaction.
		// Cash or cashback may be known later in the transaction
		buffer[0] = TYPE_GOODS_SERVICES;
		if (TlvTree_AddChild(inputTlvTree, TAG_INT_TRANSACTION_TYPE, buffer, 1) == NULL) {
			doTransaction = FALSE;
		}

		// Set the transaction type
		switch(paramIn->transaction) {
		case DEBIT_TR:
			// Purchase transaction
			buffer[0] = 0x00;	// Standard ISO8583:1983 transaction type for 'debit' is 00
			if (TlvTree_AddChild(inputTlvTree, TAG_TRANSACTION_TYPE, buffer, 1) == NULL) {
				doTransaction = FALSE;
			}
			break;

		case CANCEL_TR:
		case EXT_CANCEL_TR:
			// Void transaction
			buffer[0] = 0x02;	// Standard ISO8583:1983 transaction type for 'void' is 02
			if (TlvTree_AddChild(inputTlvTree, TAG_TRANSACTION_TYPE, buffer, 1) == NULL) {
				doTransaction = FALSE;
			}
			break;

		case CREDIT_TR:
			// Refund transaction
			buffer[0] = 0x20;	// Standard ISO8583:1983 transaction type for 'refund' is 20
			if (TlvTree_AddChild(inputTlvTree, TAG_TRANSACTION_TYPE, buffer, 1) == NULL) {
				doTransaction = FALSE;
			}
			break;

		default:
			// Purchase transaction
			buffer[0] = 0x00;	// Standard ISO8583:1983 transaction type for 'debit' is 00
			if (TlvTree_AddChild(inputTlvTree, TAG_TRANSACTION_TYPE, buffer, 1) == NULL) {
				doTransaction = FALSE;
			}
			break;
		}

		// Add the reader ID
		buffer[0] = paramIn->peri;
		if (TlvTree_AddChild(inputTlvTree, TAG_CHIP_READER_ID, buffer, 1) == NULL) {
			doTransaction = FALSE;
		}

		///---------- Biller mode terminal check ----------
		mapGetByte(appTerminalMode, BillerMode);
		switch (BillerMode) {
		case 1: //Modify Terminal behavior for Biller terminal
			doTransaction = FALSE;
			break;
		}


		if (doTransaction) {
			// Setup the user interface environment
			previousUiState = EMV_UI_TransactionBegin();

			//			// Perform the transaction !
			//			switch(EMV_ServicesEmv_DoTransaction("CAM0", 4, FALSE, inputTlvTree)) {
			//			case ERR_ENG_OK:
			//				// Payment approved
			//				paramOut->rc_payment = PAY_OK;
			//				break;
			//			case ERR_ENG_TRY_ANOTHER_AID:
			//				// Another AID must be selected
			//				paramOut->rc_payment = PAY_OTHER_AID;
			//				break;
			//			default:
			//				// Payment not performed (declined ...)
			//				paramOut->rc_payment = PAY_KO;
			//				break;
			//			}

			////===========  New Implementation  =========

			memset(buf, 0, sizeof(buf));
			num2dec(buf, mnuSale, 0);
			MAPPUTSTR(traMnuItm, buf, lblKO);

			memset(buf, 0, sizeof(buf));

			//get current transaction date and time
			memset(Statement, 0, sizeof(Statement));
			getDateTime(Statement); //Temporarily used var:Statement for usability

			//SaveTransaction type if Dr or Cr
			memset(Statement, 0, sizeof(Statement));
			Telium_Sprintf (Statement, "SELECT DrCr FROM AppMenus WHERE MenuId = '%d';", mnuSale);
			ret = Sqlite_Run_Statement_MultiRecord(Statement, buf);
			MAPPUTSTR(traDrCr, buf, lblKO);

			memset(Statement, 0, sizeof(Statement));
			memset(buf, 0, sizeof(buf));
			Telium_Sprintf (Statement, "SELECT MenuName FROM AppMenus WHERE MenuId = '%d';", mnuSale);
			ret = Sqlite_Run_Statement_MultiRecord(Statement, buf);
			CHECK(ret > 0, lblKO);
			MAPPUTSTR(traCtx, buf, lblKO);

			//default initial data
			MAPPUTSTR(traCurrencyLabel, "TZS", lblKO);
			MAPPUTSTR(emvTrnCurCod,"020834",lblKO);
			MAPPUTBYTE(appReversalFlag, 0, lblKO);
			ManageSTAN();

			//sale data
			MAPPUTSTR(traRqsBitMap, "083038078020C80006",lblKO);
			MAPPUTSTR(traRqsMTI, "020200",lblKO);
			MAPPUTSTR(traRqsProcessingCode, "000000",lblKO);

			ClosePeripherals();                                      // Close standard peripherals

			Application_Do_Transaction_Idle();
			paramOut->rc_payment = PAY_OK;

			lblKO:
			////===========================================
			// Restore the user interface environment
			EMV_UI_TransactionEnd(previousUiState);
		}
	}

	// Release the memory
	EPSTOOL_TlvTree_Release(&inputTlvTree);

	//Clear the transaction Buffers of the transaction
	traReset();

	return FCT_OK;
}


// ***************************************************************************
//   int customize_message(int size, InfosMSG_CUST *param)
/**
 *  To customized the Manager messages.
 *  Refer to SDK chm in TELIUM Manager User Guide.
 *  Section 11: Manager customization (see list of messages customizable).
 * \return
 *  FCT_OK
 * \header sdk30.h
 * \source entry.c
 */
// ***************************************************************************
int customize_message (int size, InfosMSG_CUST *param) {
	// Local variables *
	// *****************
	int localDisplay = 0;

	// Customizable messages from Manager
	// **********************************
	hDsp = Telium_Stdperif("DISPLAY", NULL);  // Check "display" peripheral already opened? (done by Manager)
	if (hDsp == NULL) {                       // No, then open it
		hDsp = Telium_Fopen("DISPLAY", "w*"); // Open "display" peripheral
		localDisplay = 1;
	}

	if (hDsp != NULL) {
		switch (param->num) {
		case MESS214: // "    NO MORE\n     PAPER\n"
			GL_Dialog_Message(hGoal, NULL, "   PAPER ROLL   \n    MISSING     ", GL_ICON_NONE, GL_BUTTON_NONE, 0);
			param->cr_treatment = CUSTOMIZED_MESSAGE;    // Accept this new message
			break;
		case MESS262: // "     REMOVE\n  CARD PLEASE"
			GL_Dialog_Message(hGoal, NULL, " PLEASE REMOVE  \n   CHIP CARD    ", GL_ICON_NONE, GL_BUTTON_NONE, 0);
			param->cr_treatment = CUSTOMIZED_MESSAGE;    // Accept this new message
			break;
		default:
			param->cr_treatment = NO_CUSTOMIZED_MESSAGE; // Don't change the default Manager message
			break;
		}

		if(localDisplay == 1) {
			Telium_Fclose(hDsp);                  // Close "display" peripheral
			hDsp=NULL;
		}
	}

	return FCT_OK;
}



//// Global variables ///////////////////////////////////////////

//! \brief Array used to declare all the supported services to the system.
static Telium_service_desc_t __CLESS_theServices[] = {
		{ 0, IDLE_MESSAGE,                  (Telium_callback)__CLESS_ServicesManager, IDLE_HIGH_PRIORITY },                  // Service Manager => Dedicated to display idle message
		{ 0, KEYBOARD_EVENT,                (Telium_callback)__CLESS_ServicesManager, __CLESS_SERVICES_PRIORITY_DEFAULT },              // Service Manager => Return the key pressed
		{ 0, GET_IDLE_STATE_PARAMETER,      (Telium_callback)__CLESS_ServicesManager, __CLESS_SERVICES_PRIORITY_DEFAULT },              // Service Manager => Gives manager parameters to customize idle state
		{ 0, GIVE_MONEY_EXTENDED,           (Telium_callback)__CLESS_ServicesManager, __CLESS_SERVICES_PRIORITY_DEFAULT },              // Service Manager => Get the currencies supported by the application
		{ 0, STATE,                         (Telium_callback)__CLESS_ServicesManager, __CLESS_SERVICES_PRIORITY_DEFAULT },              // Service Manager => Print terminal content
		{ 0, CONSULT,                       (Telium_callback)__CLESS_ServicesManager, __CLESS_SERVICES_PRIORITY_DEFAULT },              // Service Manager => Print daily totals
		{ 0, MCALL,                         (Telium_callback)__CLESS_ServicesManager, __CLESS_SERVICES_PRIORITY_DEFAULT },              // Service Manager => Print call schedule
		{ 0, IS_TIME_FUNCTION,              (Telium_callback)__CLESS_ServicesManager, __CLESS_SERVICES_PRIORITY_DEFAULT },              // Service Manager => Do you need the peripherals at the next call of time_function()?
		{ 0, TIME_FUNCTION,                 (Telium_callback)__CLESS_ServicesManager, __CLESS_SERVICES_PRIORITY_DEFAULT },              // Service Manager => Dedicated to execute automatic periodic functions
		{ 0, IS_CHANGE_INIT,                (Telium_callback)__CLESS_ServicesManager, __CLESS_SERVICES_PRIORITY_DEFAULT },              // Service Manager => Conditions for changing Manager parameters?
		{ 0, MODIF_PARAM,                   (Telium_callback)__CLESS_ServicesManager, __CLESS_SERVICES_PRIORITY_DEFAULT },              // Service Manager => Report Manager parameters changing
		{ 0, IS_EVOL_PG,                    (Telium_callback)__CLESS_ServicesManager, __CLESS_SERVICES_PRIORITY_DEFAULT },              // Service Manager => Conditions for application downloading?
		{ 0, FILE_RECEIVED,                 (Telium_callback)__CLESS_ServicesManager, __CLESS_SERVICES_PRIORITY_IDLE_MSG },              // Service Manager => Report Manager parameters file received from LLT
		{ 0, MESSAGE_RECEIVED,              (Telium_callback)__CLESS_ServicesManager, __CLESS_SERVICES_PRIORITY_DEFAULT },              // Service Manager => Message received from another application inside the mailbox
		{ 0, IS_CARD_SPECIFIC,              (Telium_callback)__CLESS_ServicesManager, __CLESS_SERVICES_PRIORITY_DEFAULT },              // Service Manager => Do you want to process this specific card?
		{ 0, CARD_INSIDE,                   (Telium_callback)__CLESS_ServicesManager, __CLESS_SERVICES_PRIORITY_DEFAULT },              // Service Manager => Call after IS_CARD_SPECIFIC to start transaction process
		{ 0, IS_FOR_YOU_BEFORE,             (Telium_callback)__CLESS_ServicesManager, __CLESS_SERVICES_PRIORITY_DEFAULT },              // Service Manager => Ask application to recognize chip card in order to be a candidate
		{ 0, IS_FOR_YOU_AFTER,              (Telium_callback)__CLESS_ServicesManager, __CLESS_SERVICES_PRIORITY_DEFAULT },              // Service Manager => Ask application to recognize mag, smart, manually card in order to be a candidate
		{ 0, DEBIT_NON_EMV,                 (Telium_callback)__CLESS_ServicesManager, __CLESS_SERVICES_PRIORITY_DEFAULT },              // Service Manager => Process a non EMV chip card or Mag stripe or card manual entry transaction (DEBIT)
		{ 0, SELECT_FUNCTION,               (Telium_callback)__CLESS_ServicesManager, __CLESS_SERVICES_PRIORITY_DEFAULT },              // Service Manager => Process a non EMV chip card or Mag stripe or card manual entry transaction (CREDIT/CANCEL/PRE AUTH)
		{ 0, FALL_BACK,                     (Telium_callback)__CLESS_ServicesManager, __CLESS_SERVICES_PRIORITY_DEFAULT },              // Service Manager => Fallback switch from Chip to Swipe (for iPP480 only - dual reader)
		{ 0, GIVE_AID,                      (Telium_callback)__CLESS_ServicesManager, __CLESS_SERVICES_PRIORITY_DEFAULT },
		{ 0, IS_CARD_EMV_FOR_YOU,           (Telium_callback)__CLESS_ServicesManager, __CLESS_SERVICES_PRIORITY_DEFAULT },
		{ 0, DEBIT_EMV,                     (Telium_callback)__CLESS_ServicesManager, __CLESS_SERVICES_PRIORITY_DEFAULT },
		{ 0, CUSTOMIZE_MESSAGE,             (Telium_callback)customize_message, __CLESS_SERVICES_PRIORITY_DEFAULT }, // Service Manager => Reserved to customize the manager messages

		{ 0, AFTER_RESET,                    (Telium_callback)__CLESS_ServicesManager, __CLESS_SERVICES_PRIORITY_DEFAULT },
		{ 0, IS_NAME ,                       (Telium_callback)__CLESS_ServicesManager, __CLESS_SERVICES_PRIORITY_DEFAULT },
		{ 0, GIVE_YOUR_DOMAIN,               (Telium_callback)__CLESS_ServicesManager, __CLESS_SERVICES_PRIORITY_DEFAULT },
		{ 0, GIVE_YOUR_SPECIFIC_CONTEXT,     (Telium_callback)__CLESS_ServicesManager, __CLESS_SERVICES_PRIORITY_DEFAULT },
		{ 0, IS_STATE,                       (Telium_callback)__CLESS_ServicesManager, __CLESS_SERVICES_PRIORITY_DEFAULT },
		{ 0, IS_DELETE,                      (Telium_callback)__CLESS_ServicesManager, __CLESS_SERVICES_PRIORITY_DEFAULT },
		{ 0, MORE_FUNCTION,                  (Telium_callback)__CLESS_ServicesManager, __CLESS_SERVICES_PRIORITY_DEFAULT },

		{ 0, CLESS_GIVE_INFO,                (Telium_callback)__CLESS_ServicesCless, __CLESS_SERVICES_PRIORITY_DEFAULT },
		{ 0, CLESS_DEBIT_AID,                (Telium_callback)__CLESS_ServicesCless, __CLESS_SERVICES_PRIORITY_DEFAULT },
		{ 0, CLESS_END,                      (Telium_callback)__CLESS_ServicesCless, __CLESS_SERVICES_PRIORITY_DEFAULT },

		{ 0, CLESS_SERVICE_CUST_IMPSEL_GUI,  (Telium_callback)__CLESS_ServicesCless, __CLESS_SERVICES_PRIORITY_DEFAULT },
		{ 0, CLESS_SERVICE_CUST_IMPSEL_GUI,  (Telium_callback)__CLESS_ServicesCless, __CLESS_SERVICES_PRIORITY_DEFAULT },
		{ 0, SERVICE_CUSTOM_KERNEL,          (Telium_callback)__CLESS_ServicesCless, __CLESS_SERVICES_PRIORITY_DEFAULT },

};



//! \brief This function registers all the services of the application to the system.
//! \param[in] appliId Application system identifier.
static void __CLESS_GiveInterface(unsigned short appliId) {
	int index;

	// Initialise the application type in the table of supported services
	for(index = 0; index < (int)(sizeof(__CLESS_theServices) / sizeof(__CLESS_theServices[0])); index++)
		__CLESS_theServices[index].appli_id = appliId;

	// Register services
	Telium_ServiceRegister((sizeof(__CLESS_theServices) / sizeof(__CLESS_theServices[0])), __CLESS_theServices);
}


//! \brief This function is the main function of the application.
//! The objective of this function is to declare the supported services.
void entry(void) {
	object_info_t info;

	// TODO: Initialize the global variables
	// Do It here to ensure that global variables are initialized before any service call
	// Be careful to only INITIALISE the global variables and not doing treatments (loading files, use display, ...)

	ObjectGetInfo(OBJECT_TYPE_APPLI, ApplicationGetCurrent(), &info);
	__CLESS_GiveInterface(info.application_type);

	// Open all the external libraries
	// *******************************
	hGoal = GL_GraphicLib_Create(); // Create "graphic library" handle
	hBeepDll = BeepDll_Open();      // Beep DLL handle
	SEClib_Open();                  // Security Boster1 & 2 & 3

	// Open the SSL library.
	ssllib_open();
}

//****************************************************************************
// To open standard peripherals
//****************************************************************************
int OpenPeripherals(void) {
	// Local variables
	// ***************
	int iRet;

	// Open peripherals
	// ****************
	hDsp = Telium_Stdperif("DISPLAY", NULL);  // Check "display" peripheral already opened? (done by Manager)
	if (hDsp == NULL) {                       // No, then open it
		localDisplay = 1;
		hDsp = Telium_Fopen("DISPLAY", "w*"); // Open "display" peripheral
		CHECK(hDsp!=NULL, lblKO);
	}

#ifndef __TELIUM3__
	hPrn = Telium_Stdperif("PRINTER", NULL);  // Check "PRINTER" peripheral already opened? (done by Manager)
	if (hPrn == NULL) {                       // No, then open it

		hPrn = Telium_Fopen("PRINTER", "w-*");// Open "printer" peripheral
		CHECK(hPrn!=NULL, lblKO);
	}
#endif

	hKbd = Telium_Stdperif("KEYBOARD", NULL);             // Check "keyboard" peripheral opened?
	if(hKbd == NULL){
		kbdLocal = 1;
		hKbd = Telium_Fopen("KEYBOARD", "r*");            // Open "keyboard" peripheral
		CHECK(hKbd!=NULL, lblKO);
	}


	if(GL_GraphicLib_IsTouchPresent(hGoal)) { // Check if it is a touch screen
		DisplayFooter(_OFF_);                 // Remove footer otherwise touch does not work
		hTsc = Telium_Stdperif("TSCREEN", NULL);             // Check "TSCREEN" peripheral opened?
		if(hTsc == NULL){
			hTsc = Telium_Fopen("TSCREEN", "r*"); // Open "touch" peripheral
			CHECK(hTsc!=NULL, lblKO);
		}
	}

	iRet=0;
	goto lblEnd;

	// Errors treatment
	// ****************
	lblKO:                                        // Open peripherals failed
	iRet=-1;
	goto lblEnd;
	lblEnd:
	return iRet;
}

//****************************************************************************
// To close standard peripherals
//****************************************************************************
void ClosePeripherals(void) {
	// Local variables
	// ***************
	// Empty

	// Close peripherals
	// *****************
	if(hTsc) {
		Telium_Fclose(hTsc); // Close "touch" peripheral
		hTsc=NULL;
	}

	if((hKbd) && (kbdLocal)) {
		kbdLocal = 0;
		Telium_Fclose(hKbd); // Close "keyboard" peripheral
		hKbd=NULL;
	}

	if(hPrn) {
		Telium_Fclose(hPrn); // Close "printer" peripheral
		hPrn=NULL;
	}

	if((hDsp) && (localDisplay)) {
		localDisplay = 0;
		Telium_Fclose(hDsp); // Close "display" peripheral
		hDsp=NULL;
	}
}

//****************************************************************************
// To reset standard peripherals
//****************************************************************************
void ResetPeripherals(unsigned int uiEvents) {
	// Local variables
	// ***************
	// Empty

	// Reset FIFO buffers
	// ******************
	if(((uiEvents & KEYBOARD) == KEYBOARD) && hKbd) // FIFO keyboard
		Telium_Reset_buf(hKbd, _receive_id);

	if(((uiEvents & TSCREEN) == TSCREEN) && hTsc)   // FIFO touch screen
		Telium_Reset_buf(hTsc, _receive_id);

	if(((uiEvents & PRINTER) == PRINTER) && hPrn)   // FIFO printer
		Telium_Reset_buf(hPrn, _receive_id);
}

//****************************************************************************
//                       void CrashScenario(void)
// This function crashes the terminal using different scenarios.
// This function has no parameters.
// This function has no return value.
//****************************************************************************

void CrashScenario(void) {
	// Local variables
	// ***************

	// **************************************
	// MMU DATA ABORT
	// Read or write unauthorized data area
	// **************************************

	// ** CRASH 1 => Write at null address
	//char* pt = NULL;
	//*pt = 0;

	// ** CRASH 2 => Buffer overflow
	//char buffer[2];
	//int i;
	//for (i=0; i<1024; i++)
	//	buffer[i] = 0;

	// ** CRASH 3 => Stack crash
	//CrashScenario();

	// *****************************************
	// MMU UNDDEFINED INSTRUCTION ABORT
	// Trying to run an unexisting instruction
	// *****************************************

	// ** CRASH 4 => illegal instruction
	//static const void* ptr = SoftwareReset;
	//ptr++;
	//((void(*)(void))ptr)();

	// ***************************************
	// MMU PREFETCH ABORT
	// Trying to reach an unexisting address
	// ***************************************

	// ** CRASH 5 => unexisting address
	//static const void* ptr = SoftwareReset;
	//ptr += 0x12345678;
	//((void(*)(void))ptr)();
}

//****************************************************************************
//                       void SoftwareReset(void)
// This function resets the application parameters with the default values.
//  Then stores the compiler date and time into the table.
// This function has no parameters.
// This function has no return value.
//****************************************************************************

void SoftwareReset(void) {
	// Local variables
	// ***************
	char tcPath[100];
	doubleword uiMode;  // Bug in prototype => 2nd parameter (output) on FS_mount returns Mode (FS_WRITEONCE or FS_WRITEMANY)
	// and NOT AccessMode (FS_NOFLAGS or FS_RONLYMOD or FS_WRTMOD)
	T_GL_COORD sValue;
	int iRet;

	// Software reset in progress
	// **************************
	iRet = GL_Dialog_Message(hGoal, "Software Reset", "No=Cancel / Yes=Valid", GL_ICON_QUESTION, GL_BUTTON_VALID_CANCEL, 30*1000);
	CHECK((iRet!=GL_KEY_CANCEL) && (iRet!=GL_RESULT_INACTIVITY), lblEnd);  // Cancel/Timeout

	// Kill the parameter disk in any case
	Telium_Sprintf(tcPath, "/%s", PARAM_DISK);
	FS_unmount(tcPath);                                         // Release resources on disk
	FS_dskkill(tcPath);                                         // Disk suppression

	// Create a fresh parameter disk
	iRet = FS_dskcreate(&xCfg, &ulSize);                        // Create and format the none volatile disk
	CHECK(iRet==FS_OK, lblKO);
	iRet = FS_mount(tcPath, &uiMode);                           // Activate a disk
	CHECK(iRet==FS_OK, lblKO);

	iRet = appReset();                                          // Reset application parameters (Flash)
	CHECK(iRet>=0, lblKO);

	iRet = traReset();                                          // Reset Transaction Buffers (Flash)
	CHECK(iRet>=0, lblKO);

	iRet = appPut(appCmpDat, (char*)getAppCmpDat(), lenCmpDat); // Store compiler date/time Mapapp.c
	CHECK(iRet>=0, lblKO);
	iRet = appPut(appCmpTim, (char*)getAppCmpTim(), lenCmpTim);
	CHECK(iRet>=0, lblKO);

	GL_Dialog_Progress(hGoal, "Software Reset", "Init Software\nReset In Progress",
			NULL, NULL, 0, 100, Progress, GL_BUTTON_NONE, GL_TIME_INFINITE);
	__Cless_Menu_EraseParameters ();
	__Cless_Menu_DefaultParameters();
	sValue = 100;
	GL_Dialog_Progress(hGoal, "Software Reset", "Init Software\nReset Done",
			NULL, &sValue, 1, 101, NULL, GL_BUTTON_NONE, 3*1000);

	goto lblEnd;

	// Errors treatment
	// ****************
	lblKO:                                                          // None-classified low level error
	GL_Dialog_Message(hGoal, NULL, "Processing Error\nReset Failed", GL_ICON_ERROR, GL_BUTTON_VALID, 5*1000);
	goto lblEnd;
	lblEnd:
	return;
}
