//****************************************************************************
//       INGENICO                                INGEDEV 7                   
//============================================================================
//       FILE  COMGPRS.C                          (Copyright INGENICO 2012)
//============================================================================
//  Created :       11-July-2012         Kassovic
//  Last modified : 06-December-2013     Kassovic
//  Module : TRAINING                                                          
//                                                                          
//  Purpose :                                                               
//                   *** GPRS communication management ***
//  How to use the linklayer to create a GPRS configuration to communicate.
//                   Transmission by GPRS
//               Test done with Hyper-terminal Tcp/Ip Winsock
//  !!! Link Layer application must be loaded 3628xxxx.SGN inside the terminal
//      Supports PPP, TCP/IP and SSL protocols.
//      TlvTree_TPlus.lib + LinkLayerExeInterface_TPlus.lib must be implemented to link properly.
//                                                                            
//  List of routines in file :  
//      GprsReport : Provide the state of the GPRS driver.
//      StartGPRS : Attach to the GPRS network.
//      OpenGPRS : Create the GPRS layer.
//      ConnectGPRS : Connect the GPRS layer.
//      SendGPRS : Send data through the GPRS layer.
//      ReceiveGPRS : Receive data through the GPRS layer.
//      DisconnectGPRS : Disconnect the GPRS layer.                                           
//      CloseGPRS : Delete the GPRS layer.
//      StopGPRS : Break the attachment to the GPRS network.
//      PromptGPRS : Prompt for GPRS's parameters.
//      ComGPRS : GPRS communication demo.
//                            
//  File history :
//  071112-BK : File created
//  120613-BK : Redesign software
//                                                                           
//****************************************************************************

//****************************************************************************
//      INCLUDES                                                            
//****************************************************************************
#include <globals.h>
#include "TlvTree.h"
#include "LinkLayer.h"
#include "ExtraGPRS.h"
#include "dll_wifi.h"

//****************************************************************************
//      EXTERN                                                              
//****************************************************************************
extern T_GL_HGRAPHIC_LIB hGoal; // Handle of the graphics object library

//****************************************************************************
//      PRIVATE CONSTANTS                                                   
//****************************************************************************
#define MAX_SND  2048
#define MAX_RSP  2048

#define GPRS_TIMEOUT  5*100
#define TCPIP_TIMEOUT 10*100

//****************************************************************************
//      PRIVATE TYPES                                                       
//****************************************************************************
// Empty

//****************************************************************************
//      PRIVATE DATA                                                        
//****************************************************************************
// Properties of the GPRS screen (Goal)
// ====================================
static const ST_DSP_LINE txGPRS[] =
{
		{ {GL_ALIGN_LEFT, GL_ALIGN_CENTER, GL_COLOR_WHITE, GL_COLOR_BLACK, 100, FALSE, {1, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_WHITE}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}}, // Line0
				{GL_ALIGN_LEFT, GL_ALIGN_CENTER, FALSE, 100, FALSE, {2, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}} },
				{ {GL_ALIGN_LEFT, GL_ALIGN_CENTER, GL_COLOR_WHITE, GL_COLOR_BLACK, 100, FALSE, {1, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_WHITE}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}}, // Line1
						{GL_ALIGN_LEFT, GL_ALIGN_CENTER, FALSE, 100, FALSE, {2, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}} },
						{ {GL_ALIGN_LEFT, GL_ALIGN_CENTER, GL_COLOR_WHITE, GL_COLOR_BLACK, 100, FALSE, {1, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_WHITE}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}}, // Line2
								{GL_ALIGN_LEFT, GL_ALIGN_CENTER, FALSE, 100, FALSE, {2, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}} },
								{ {GL_ALIGN_LEFT, GL_ALIGN_CENTER, GL_COLOR_WHITE, GL_COLOR_BLACK, 100, FALSE, {1, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_WHITE}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}}, // Line3
										{GL_ALIGN_LEFT, GL_ALIGN_CENTER, FALSE, 100, FALSE, {2, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}} },
										{ {GL_ALIGN_LEFT, GL_ALIGN_CENTER, GL_COLOR_WHITE, GL_COLOR_BLACK, 100, FALSE, {1, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_WHITE}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}}, // Line4
												{GL_ALIGN_LEFT, GL_ALIGN_CENTER, FALSE, 100, FALSE, {2, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}} }
};

// Properties of the default printer (Goal)
// ========================================
static const ST_PRN_LINE xPrinter =
{
		GL_ALIGN_LEFT, GL_ALIGN_CENTER, FALSE, 100, {0, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {1, 0, 1, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_LARGE}
};

// Dialog choice
// =============
static const char *tzRequirePin[] =
{
		"Yes",
		"No",
		NULL
};


#ifdef __TELIUM3__

//****************************************************************************
//    int GprsReport (T_GL_HWIDGET hScreen, char *pcApn, char *pcUserName)
//  This function provides the state of the GPRS driver.
//  The state of the GPRS driver includes the "start report" and the
//  "sim status", these informations are displayed on screen to show the
//  progress.
//  A low level "fioctl" function should be used (DGPRS_FIOCTL_GET_INFORMATION)
//  to get the information on a structure.
//  A second low level "fioctl" function is also used (DGPRS_FIOCTL_SET_PIN_MODE_FREE)
//  to ask for the pin entry each time the terminal has been switched off.
//   - LL_GPRS_Start() : Start GPRS modem and enable facility
//  This function has parameters.
//    hScreen (I-) : Handle of the screen
//    pcApn (I-) : Gprs Apn string (max 30 bytes)
//    pcUsername (I-) : Gprs Username string (max 30 bytes)
//  This function has return value
//    >=0 : Report done
//     <0 : Report failed
//****************************************************************************

static int GprsReport(T_GL_HWIDGET hScreen, char *pcApn, char *pcUsername) {
	// Local variables
	// ***************
	Telium_File_t *hGprs = NULL;
	char tcMask[256];
	int iRequirePin;
	char tcRequirePin[lenGprsRequirePin+1]; // Pin required ?
	char tcPin[lenGprsPin+1];
	char tcPin1[lenGprsPin+1];
	char tcPuk[lenGprsPuk+1];
	char tcReport[100+1], tcStatus[100+1];
	T_EGPRS_GET_INFORMATION xInfo;
	int i, iRet;


	memset(tcStatus, 0, sizeof(tcStatus));
	memset(tcReport, 0, sizeof(tcReport));

	// Get state of GPRS driver
	// ************************
	hGprs = Telium_Stdperif((char*)"DGPRS", NULL);
	//CHECK(hGprs!=NULL, lblKO);

	iRet = appGet(appGprsRequirePin, tcRequirePin, lenGprsRequirePin+1);
	CHECK(iRet>=0, lblKO);

	iRequirePin=atoi(tcRequirePin); // False => 0 (pin required), True => 1 (pin saved)
	iRet = gprs_SetPinModeFree(hGprs,iRequirePin);
	CHECK(iRet==0, lblKO);

	iRet = gprs_GetInformation(hGprs,&xInfo,sizeof(xInfo));
	CHECK(iRet==0, lblKO);

	// GPRS start report
	// =================
	switch (xInfo.start_report)
	{
	case EGPRS_REPORT_NOT_RECEIVED_YET: strcpy(tcReport, "NETWORK NOT READY"); break;
	case EGPRS_REPORT_READY:           /* strcpy(tcReport, "Rpt: READY"); */           break;
	case EGPRS_REPORT_WAITING_CODE:     strcpy(tcReport, "ENTER SIM CODE");     break;
	case EGPRS_REPORT_SIM_NOT_PRESENT:  strcpy(tcReport, "SIM NOT PRESENT");  break;
	case EGPRS_REPORT_SIMLOCK_STATE:    strcpy(tcReport, "SIMLOCK STATE");    break;
	case EGPRS_REPORT_FATAL_ERROR:      strcpy(tcReport, "FATAL ERROR");      break;
	default:                            strcpy(tcReport, "UNKNOWN ERROR");    break;
	}

	// GPRS sim status
	// ===============
	switch (xInfo.sim_status)
	{
	case EGPRS_SIM_NOT_RECEIVED_YET:
		//		strcpy(tcStatus, "Sta: NOT_RECEIVED_YET");

		// *** Start GSM session ***
		iRet = LL_GSM_Start(NULL);
		CHECK(iRet==LL_ERROR_OK, lblKO);

		break;
	case EGPRS_SIM_OK:                                         // Sim ready, pin already checked

		// *** Start GPRS session ***
		iRet = LL_GPRS_Start(NULL, pcApn);
		CHECK(iRet==LL_ERROR_OK, lblKO);

		break;
	case EGPRS_SIM_KO:
		strcpy(tcStatus, "SIM ERROR");
		break;
	case EGPRS_SIM_PIN_REQUIRED:                              // Pin required, enter it
		strcpy(tcStatus, "SIM PIN REQUIRED");

		// *** Enter Pin code ***
		memset(tcMask, 0, sizeof(tcMask));
		for (i=0; i<lenGprsPin; i++)
			strcat(tcMask, "/d");                             // Accept only decimal character
		memset (tcPin, 0, sizeof(tcPin));
		iRet = GL_Dialog_Password(hGoal, "Pin Code", "Enter Pin :", tcMask, tcPin, sizeof(tcPin), GL_TIME_MINUTE);
		if ((iRet==GL_KEY_CANCEL) || (iRet==GL_RESULT_INACTIVITY))
			iRet=0;                                           // Exit on cancel/timeout
		CHECK(iRet!=0, lblEnd);

		// *** Start GSM session ***
		iRet = LL_GSM_Start(tcPin[0] ? tcPin : NULL);
		CHECK(iRet==LL_ERROR_OK, lblKO);

		// *** Start GPRS session ***
		iRet = LL_GPRS_Start(tcPin[0] ? tcPin : NULL, pcApn);
		CHECK(iRet==LL_ERROR_OK, lblKO);

		break;
	case EGPRS_SIM_PIN2_REQUIRED:
		strcpy(tcStatus, "Sta: PIN2 REQUIRED");
		break;
	case EGPRS_SIM_PIN_ERRONEOUS:                             // Wrong Pin, enter it
		strcpy(tcStatus, "Sta: PIN ERRONEOUS   ");

		// *** Enter Pin code ***
		memset(tcMask, 0, sizeof(tcMask));
		for (i=0; i<lenGprsPin; i++)
			strcat(tcMask, "/d");                             // Accept only decimal character
		memset (tcPin, 0, sizeof(tcPin));
		iRet = GL_Dialog_Password(hGoal, "Pin Code", "Enter Pin :", tcMask, tcPin, sizeof(tcPin), GL_TIME_MINUTE);
		if ((iRet==GL_KEY_CANCEL) || (iRet==GL_RESULT_INACTIVITY))
			iRet=0;                                           // Exit on cancel/timeout
		CHECK(iRet!=0, lblEnd);

		// *** Start GSM session ***
		LL_GSM_Start(tcPin[0] ? tcPin : NULL);

		// *** Start GPRS session ***
		iRet = LL_GPRS_Start(tcPin[0] ? tcPin : NULL, pcApn);
		CHECK(iRet==LL_ERROR_OK, lblKO);

		break;
	case EGPRS_SIM_NOT_INSERTED:
		strcpy(tcStatus, "SIM NOT INSERTED");
		break;
	case EGPRS_SIM_PUK_REQUIRED:                               // Puk required, enter it
		strcpy(tcStatus, "SIM PUK_REQUIRED    ");

		// *** Enter Puk code ***
		memset(tcMask, 0, sizeof(tcMask));
		for (i=0; i<lenGprsPuk; i++)
			strcat(tcMask, "/d");                              // Accept only decimal character
		memset (tcPuk, 0, sizeof(tcPuk));
		iRet = GL_Dialog_Password(hGoal, "Puk Code", "Enter Puk :", tcMask, tcPuk, sizeof(tcPuk), GL_TIME_MINUTE);
		if ((iRet==GL_KEY_CANCEL) || (iRet==GL_RESULT_INACTIVITY))
			iRet=0;                                            // Exit on cancel/timeout
		CHECK(iRet!=0, lblEnd);

		// *** Enter new Pin code and confirm ***
		do {
			// *** Enter new Pin code ***
			memset(tcMask, 0, sizeof(tcMask));
			for (i=0; i<lenGprsPin; i++)
				strcat(tcMask, "/d");                          // Accept only decimal character
			memset (tcPin1, 0, sizeof(tcPin1));                // Enter new Pin
			iRet = GL_Dialog_Password(hGoal, "New Pin Code", "Enter New Pin :", tcMask, tcPin1, sizeof(tcPin1), GL_TIME_MINUTE);
			if ((iRet==GL_KEY_CANCEL) || (iRet==GL_RESULT_INACTIVITY))
				iRet=0;                                        // Exit on cancel/timeout
			CHECK(iRet!=0, lblEnd);
			// *** Confirm new Pin code ***
			memset (tcPin, 0, sizeof(tcPin));                  // Confirm new Pin
			iRet = GL_Dialog_Password(hGoal, "New Pin Code", "Confirm New Pin :", tcMask, tcPin, sizeof(tcPin), GL_TIME_MINUTE);
			if ((iRet==GL_KEY_CANCEL) || (iRet==GL_RESULT_INACTIVITY))
				iRet=0;                                        // Exit on cancel/timeout
			CHECK(iRet!=0, lblEnd);
		} while (memcmp(tcPin1, tcPin, lenGprsPin) != 0);

		iRet = gprs_SubmitPuk(hGprs, tcPuk, tcPin);
		if (iRet == 0){                                         // Puk code OK
			Telium_Ttestall(0, 1*100);                         // Wait a little bit before restarting a session

			// *** Start GSM session ***
			LL_GSM_Start(tcPin[0] ? tcPin : NULL);

			// *** Start GPRS session ***
			iRet = LL_GPRS_Start(tcPin[0] ? tcPin : NULL, pcApn);
			CHECK(iRet==LL_ERROR_OK, lblKO);
		}

		break;
	case EGPRS_SIM_PUK2_REQUIRED: 		strcpy(tcStatus, "SIM PUK2 REQUIRED"); break;
	case EGPRS_SIM_PIN_PRESENTED:       strcpy(tcStatus, "SIM PIN PRESENTED"); break;
	default:                            strcpy(tcStatus, "GPRS COMMUNICATION ERROR"); break;
	}

	if(strlen(tcReport)>3){
		iRet = GoalDspLine(hScreen, 2, tcReport, &txGPRS[2], 0, true); // Show start report
		CHECK(iRet>=0, lblKO);
	}

	if(strlen(tcStatus)>3){
		iRet = GoalDspLine(hScreen, 3, tcStatus, &txGPRS[3], 0, true); // Show sim status
		CHECK(iRet>=0, lblKO);
	}

	iRet=1;
	goto lblEnd;

	// Errors treatment
	// ****************
	lblKO:
	iRet=-1;
	goto lblEnd;
	lblEnd:
	return iRet;
}

#endif

//****************************************************************************
//          int StartGPRS (T_GL_HWIDGET hScreen, const char *pcInit)
//  This function handles the attachment to the GPRS network.
//   - LL_GPRS_Connect() : Connect the GPRS network
//   - LL_Network_GetStatus() : Return current status of the network
//  This function has parameters.
//    hScreen (I-) : Handle of the screen
//    pcInit (I-) : 
//           Apn          = a string (max 30 bytes)
//           Username     = a string (max 30 bytes)
//           Password     = a string (max 30 bytes)
//           The '|' is the separator
//           Ex: "1234|internet-entreprise|orange|orange     
//  This function has return value
//    >=0 : Attachment done
//     <0 : Attachment failed
//****************************************************************************

static int StartGPRS(T_GL_HWIDGET hScreen, const char *pcInit){
	// Local variables 
	// ***************
	char tcApn[lenGprsApn+1];       // Apn
	char tcUsername[lenGprsUser+1]; // Login
	char tcPassword[lenGprsPass+1]; // Password
	int iKey = GL_KEY_NONE;
	word usTimeOut=0;
	bool bExit=TRUE;
	int iStatus, iRet;

	// Network GPRS parameters
	// ************************
	CHECK(pcInit!=NULL, lblKOConfigure);                            // Configuration error
	memset(tcApn, 0, sizeof(tcApn));
	pcInit = parseStr('|', tcApn, pcInit, sizeof(tcApn));           // Extract apn
	CHECK(pcInit!=NULL, lblKOConfigure);

	memset(tcUsername, 0, sizeof(tcUsername));
	pcInit = parseStr('|', tcUsername, pcInit, sizeof(tcUsername)); // Extract username
	CHECK(pcInit!=NULL, lblKOConfigure);

	memset(tcPassword, 0, sizeof(tcPassword));
	pcInit = parseStr('|', tcPassword, pcInit, sizeof(tcPassword)); // Extract password
	CHECK(pcInit!=NULL, lblKOConfigure);

	// Connect the GPRS network
	// ************************

	ResetPeripherals(KEYBOARD | TSCREEN);           // Reset peripherals FIFO

	do {

#ifdef __TELIUM3__
		iRet = GprsReport(hScreen, tcApn, tcUsername);
		CHECK(iRet>=0, lblKOConfigure);
		CHECK(iRet!=0, lblEnd);                     // Exit on cancel/timeout
#endif

		iRet = LL_GPRS_Connect(tcApn, tcUsername, tcPassword, 4*100);
		switch (iRet) {
		//case LL_ERROR_NETWORK_NOT_READY:
		//	bExit = FALSE;
		//	break;
		case LL_ERROR_ALREADY_CONNECTED:
		case LL_ERROR_OK:
			iRet = 1;
			bExit=TRUE;
			break;
		case LL_ERROR_NETWORK_NOT_SUPPORTED:
		case LL_ERROR_SERVICE_CALL_FAILURE:
			bExit = TRUE;
			break;
		default:                                   // LL_ERROR_NETWORK_NOT_READY or LL_ERROR_NETWORK_ERROR
			iRet = LL_Network_GetStatus(LL_PHYSICAL_V_GPRS, &iStatus);
			switch (iStatus){                      // Check network status

			case LL_STATUS_GPRS_ERROR_SIM_LOCK:    // Pin locked => Wait Puk
			case LL_STATUS_GPRS_ERROR_BAD_PIN:     // Wrong Pin => Wait Pin
			case LL_STATUS_GPRS_ERROR_NO_PIN:      // Pin required => Wait Pin
			case LL_STATUS_GPRS_CONNECTING:        // Connection to GPRS Network in progress
			case LL_STATUS_GPRS_AVAILABLE:         // GPRS Network is available
			case LL_STATUS_GPRS_CONNECTING_PPP:    // PPP connection is in progress with the GPRS provider
			case LL_STATUS_GPRS_DISCONNECTED:      // Disconnection to GPRS Network, let's retry a connection
			case 0x2000600:                        // Wait until network ready
				bExit = FALSE;
				break;
			case LL_STATUS_GPRS_CONNECTED:
				iRet = 1;
				bExit=TRUE;
				break;
			case LL_STATUS_GPRS_ERROR_NO_SIM:      // No SIM card is inserted into the terminal
			case LL_STATUS_GPRS_ERROR_PPP:         // Error occurred during the PPP link establishment.
			case LL_STATUS_GPRS_ERROR_UNKNOWN:     // Error status unknown
			default:
				bExit = TRUE;
				break;
			}
			break;
		}

		if (!bExit) {                               // Keyboard and timeout management
			//			iKey = GoalGetKey(hScreen, hGoal, true, 1*1000, false); // Get key pressed/touched (shortcut)
			usTimeOut += 1*100;                    // Increment timeout
		}
	} while (!bExit && (usTimeOut < GPRS_TIMEOUT) && (iKey!=GL_KEY_CANCEL));

	if ((usTimeOut==GPRS_TIMEOUT) || (iKey==GL_KEY_CANCEL))         // Exit on timeout/cancel
		iRet=0;

	goto lblEnd;

	// Errors treatment 
	// ****************
	lblKOConfigure:
	iRet=LL_ERROR_INVALID_PARAMETER;
	goto lblEnd;
	lblEnd:
	return iRet;
}

//****************************************************************************
//           LL_HANDLE OpenGPRS (const char *pcServer)
//  This function configures the GPRS layer.
//   - LL_Configure() : Create Link Layer configuration
//  This function has no parameter.
//    pcServer (I-) : Tcp/Ip address and port number
//           IpAddress = xxx.xxx.xxx.xxx or url
//           PortNumber = a string (max 5 bytes)
//           The '|' is the separator
//           Ex: "192.168.1.3|2000
//  This function has no return value
//****************************************************************************
static LL_HANDLE OpenGPRS(const char *pcServer, int tlsSsl){
	// Local variables
	// ***************
	// Tlv tree nodes
	// ==============
	TLV_TREE_NODE piConfig=NULL;
	TLV_TREE_NODE piPhysicalConfig=NULL;
	TLV_TREE_NODE piTransportConfig=NULL;
	char tcAddr[lenGprsIpRemote+1];
	char tcPort[lenGprsPort+1];
	doubleword uiTimeout, uiRemotePort;
	LL_HANDLE hSession = NULL;
	//	char TLS_Enabled[10];
	int iRet;

	//	memset(TLS_Enabled, 0, sizeof(TLS_Enabled));

	// Create the LinkLayer configuration parameters tree
	// **************************************************
	CHECK(pcServer!=NULL, lblKOConfigure);                              // Configuration failed

	// Create parameters tree
	// ======================
	piConfig = TlvTree_New(LL_TAG_LINK_LAYER_CONFIG);                   // LinkLayer parameters Root tag of the configuration tree
	CHECK(piConfig!=NULL, lblKOConfigure);

	// Physical layer parameters
	// =========================
	piPhysicalConfig = TlvTree_AddChild(piConfig,
			LL_TAG_PHYSICAL_LAYER_CONFIG,   // TAG Physical layer parameters
			NULL,                           // VALUE (Null)
			0);                             // LENGTH 0
	CHECK(piPhysicalConfig!=NULL, lblKOConfigure);

	// GPRS
	// ----
	TlvTree_AddChildInteger(piPhysicalConfig,
			LL_PHYSICAL_T_LINK,                         // TAG
			LL_PHYSICAL_V_GPRS,                         // VALUE
			LL_PHYSICAL_L_LINK);                        // LENGTH 1

	// Transport and network layer parameters
	// ======================================
	piTransportConfig = TlvTree_AddChild(piConfig,
			LL_TAG_TRANSPORT_LAYER_CONFIG, // TAG Transport layer parameters
			NULL,                          // VALUE (Null)
			0);                            // LENGTH 0
	CHECK(piTransportConfig!=NULL, lblKOConfigure);

	memset(tcAddr, 0, sizeof(tcAddr));
	pcServer = parseStr('|', tcAddr, pcServer, sizeof(tcAddr));         // Parse Tcp/Ip address
	CHECK(pcServer!=NULL, lblKOConfigure);

	memset(tcPort, 0, sizeof(tcPort));
	pcServer = parseStr('|', tcPort, pcServer, sizeof(tcPort));         // Parse Tcp/Ip port
	CHECK(pcServer!=NULL, lblKOConfigure);

	// TCP/IP
	// ------
	TlvTree_AddChildInteger(piTransportConfig,
			LL_TRANSPORT_T_PROTOCOL,                    // TAG
			LL_TRANSPORT_V_TCPIP,                       // VALUE
			LL_TRANSPORT_L_PROTOCOL);                   // LENGTH 1 byte

	uiTimeout = TCPIP_TIMEOUT;

	///Get the communication cipher
	switch (tlsSsl) {
	case 1:

		// SSL for Ethernet
		TlvTree_AddChildString(piTransportConfig,
				LL_TCPIP_T_SSL_PROFILE,	                // TAG
				SSL_PROFILE_NAME);	                    // VALUE

		TlvTree_AddChildInteger(piTransportConfig,
				LL_TCPIP_T_SSL_TCP_CONNECT_TIMEOUT,  	// TAG
				uiTimeout/2,                            // VALUE
				LL_TCPIP_L_SSL_TCP_CONNECT_TIMEOUT);	// LENGTH 4

		break;
	default:
		break;
	}


	// Host Name
	// ---------
	TlvTree_AddChildString(piTransportConfig,
			LL_TCPIP_T_HOST_NAME,                        // TAG
			tcAddr);                                     // VALUE
	// LENGTH (strlen addr)

	// Port
	// ----
	uiRemotePort = atoi (tcPort);
	TlvTree_AddChildInteger(piTransportConfig,
			LL_TCPIP_T_PORT,                            // TAG
			uiRemotePort,                               // VALUE (Integer)
			LL_TCPIP_L_PORT);                           // LENGTH 4 bytes

	// Connection timeout
	// ------------------
	TlvTree_AddChildInteger(piTransportConfig,
			LL_TCPIP_T_CONNECT_TIMEOUT,                 // TAG
			uiTimeout,                                  // Value (Integer)
			LL_TCPIP_L_CONNECT_TIMEOUT);                // LENGTH 4 bytes

	// Link Layer configuration
	// ************************
	iRet = LL_Configure(&hSession, piConfig);                           // Initialize the handle of the session
	CHECK(iRet==LL_ERROR_OK, lblKOConfigure);

	goto lblEnd;

	// Errors treatment
	// ****************
	lblKOConfigure:                                                         // Configuration failed
	hSession=NULL;
	goto lblEnd;
	lblEnd:
	if (piConfig)
		TlvTree_Release(piConfig);                                      // Release tree to avoid memory leak
	return hSession;                                                    // Return the handle of the session
}

//****************************************************************************
//              int ConnectGPRS (LL_HANDLE hSession)
//  This function connects the GPRS layer.      
//   - LL_Connect() : Connect Link Layer
//  This function has parameters.
//    hSession (I-) : Handle of the session
//  This function has return value
//    >=0 : Connect done
//     <0 : Connect failed                                        
//****************************************************************************

static int ConnectGPRS(LL_HANDLE hSession){
	// Local variables 
	// ***************
	int iRet;

	// Link Layer connection
	// *********************
	iRet = LL_Connect(hSession);

	return iRet;
}

//****************************************************************************
//       int SendGPRS (LL_HANDLE hSession, const char *pcMsg, word usLen)
//  This function sends data through the GPRS layer.      
//   - LL_Send() : Send data
//   - LL_GetLastError() : Retrieve the last error
//  This function has parameters.
//    hSession (I-) : Handle of the session
//    pcMsg (I-) : Data to send
//    usLen (I-) : Number of byte to send
//  This function has return value
//    >=0 : Number of bytes sent
//     <0 : Transmission failed                                       
//****************************************************************************
static int SendGPRS(LL_HANDLE hSession, const byte *pcMsg, word usLen){
	// Local variables 
	// ***************
	int iRet;

	// Send data
	// *********
	iRet = LL_Send(hSession, usLen, (byte*)pcMsg, LL_INFINITE);
	if(iRet != usLen)
		iRet = LL_GetLastError(hSession);

	return iRet;
}

//****************************************************************************
//          int ReceiveGPRS (LL_HANDLE hSession, T_GL_HWIDGET hScreen,
//                           char *pcMsg, word usLen, byte ucDly)
//  This function receives data through the GPRS layer.      
//   - LL_ClearReceiveBuffer() : Clear receiving buffer
//   - LL_Receive() : Wait and receive data
//   - LL_GetLastError() : Retrieve the last error
//  This function has parameters.
//    hSession (I-) : Handle of the session
//    hScreen (I-) : Handle of the screen
//    pcMsg (-O) : Data to receive
//    usLen (I-) : Maximum number of bytes to receive
//    ucDly (I-) : Timeout reception (in second, 0xFF infinite)
//  This function has return value
//    >=0 : Number of bytes received
//     <0 : Reception failed                                       
//****************************************************************************
static int ReceiveGPRS(LL_HANDLE hSession, T_GL_HWIDGET hScreen, byte *pcMsg, word usLen, byte ucDly){
	// Local variables 
	// ***************
	int iKey;
	long lSec, lTimeOut=LL_INFINITE;
	int iRet, iLength=0, iNbrBytes;

	// Timeout setting
	// ***************
	if(ucDly != 0xFF)
		lSec = ucDly*1000;
	else
		lSec = LL_INFINITE;

	// Receiving first block
	// *********************
	ResetPeripherals(KEYBOARD | TSCREEN);                                  // Reset peripherals FIFO
	iRet = TimerStart(0, lSec);                                            // Timer0 starts
	CHECK(iRet>=0, lblTimeOut);
	do {
		iNbrBytes = LL_Receive(hSession, usLen, pcMsg+iLength, 1*100);     // Check reception
		if (iNbrBytes != 0)
			break;                                                         // Bytes received
		iKey = GoalGetKey(hScreen, hGoal, true, 0, false);                 // Get key pressed/touched (shortcut)
		CHECK(iKey!=GL_KEY_CANCEL, lblTimeOut);                            // Exit on cancel key
		if (lSec != LL_INFINITE)
			lTimeOut = TimerGet(0);                                        // Retrieve timer value
	} while (lTimeOut>0);

	CHECK(lTimeOut!=0, lblTimeOut);                                        // Exit on timeout

	iLength += iNbrBytes;
	iRet = LL_GetLastError(hSession);
	CHECK (iRet==LL_ERROR_OK, lblEnd);
	if ((iNbrBytes != 0) && (iNbrBytes < usLen)){
		// Receiving next block until timeout (Inter block 500ms)
		// ======================================================
		while(1) {
			iNbrBytes = LL_Receive(hSession, usLen-iLength, pcMsg+iLength, 50);
			iLength += iNbrBytes;
			iRet = LL_GetLastError(hSession);
			CHECK ((iRet==LL_ERROR_OK) || (iRet==LL_ERROR_TIMEOUT), lblEnd);
			if ((iRet==LL_ERROR_TIMEOUT) || (iNbrBytes==0))
				break;
		}
	}

	iRet = iLength;
	goto lblEnd;

	// Errors treatment 
	// ****************
	lblTimeOut:                                                                // Timeout expired
	iRet = LL_ERROR_TIMEOUT;
	goto lblEnd;
	lblEnd:
	TimerStop(0);
	return iRet;
}

//****************************************************************************
//                   int DisconnectGPRS (LL_HANDLE hSession)
//  This function disconnects the GPRS layer.
//   - LL_Disconnect() : Disconnect Link Layer
//  This function has no parameters.
//    hSession (I-) : Handle of the session
//  This function has return value
//    >=0 : Disconnect done
//     <0 : Disconnect failed
//****************************************************************************
static int DisconnectGPRS(LL_HANDLE hSession){
	// Local variables
	// ***************
	int iRet;

	// Link Layer disconnection
	// ************************
	iRet = LL_Disconnect(hSession);

	return iRet;
}

//****************************************************************************
//                 int CloseGPRS (LL_HANDLE hSession)
//  This function deletes the handle of the GPRS layer.
//   - LL_Configure() : Delete Link Layer configuration
//  This function has no parameters.
//    hSession (I-) : Handle of the session
//  This function has return value
//    >=0 : Configuration deleted
//     <0 : Processing failed
//****************************************************************************
static int CloseGPRS(LL_HANDLE hSession){
	// Local variables
	// ***************
	int iRet;

	// Delete the configuration
	// ************************
	iRet = LL_Configure(&hSession, NULL);

	return iRet;
}

//****************************************************************************
//                        int StopGPRS (void)
//  This function breaks the attachment to the GPRS network.
//   - LL_GPRS_Stop() : Disconnect the GPRS network
//  This function has no parameters.
//  This function has return value
//    >=0 : Break attachment done
//     <0 : Break attachment failed
//****************************************************************************
int StopGPRS(void){
	// Local variables
	// ***************
	int iRet;

	// GPRS disconnection
	// ******************
	iRet = LL_GPRS_Stop();

	return iRet;
}

//****************************************************************************
//                      void PromptGPRS (void)                            
//  This function asks for the GPRS's parameters.   
//  This function has no parameters.  
//  This function has no return value
//****************************************************************************
void PromptGPRS(void){
	// Local variables 
	// ***************
	char tcMask[256];
	char tcRequirePin[lenGprsRequirePin+1];
	char tcApnN[lenGprsApn+1], tcApnP[lenGprsApn+1];
	char tcUserN[lenGprsUser+1], tcUserP[lenGprsUser+1];
	char tcPassN[lenGprsPass+1], tcPassP[lenGprsPass+1];
	char tcIpAddressN[100+1], tcIpAddressP[100+1];
	char tcPortN[lenGprsPort+1], tcPortP[lenGprsPort+1];
	doubleword uiIp;
	char tcDisplay[50+1];
	int iHeader, iFooter;
	int i, iDefItemP, iDefItemN, iRet;

	// Pin Required ?
	// **************
	iRet = appGet(appGprsRequirePin, tcRequirePin, lenGprsRequirePin+1);
	CHECK(iRet>=0, lblDbaErr);
	iDefItemP = atoi(tcRequirePin);
	iDefItemN = GL_Dialog_Choice(hGoal, "Pin Required?", tzRequirePin, iDefItemP, GL_BUTTON_DEFAULT, GL_KEY_0, GL_TIME_MINUTE);
	CHECK((iDefItemN!=GL_KEY_CANCEL) && (iDefItemN!=GL_RESULT_INACTIVITY), lblEnd); // Exit cancel/timeout
	if (iDefItemN != iDefItemP)	{
		tcRequirePin[0] = iDefItemN + 0x30;
		iRet = appPut(appGprsRequirePin, tcRequirePin, lenGprsRequirePin); // Save pin required
		CHECK(iRet>=0, lblDbaErr);
	}

	// Enter APN 
	// *********
	iHeader = IsHeader();                                                  // Save header state
	iFooter = IsFooter();                                                  // Save footer state

	memset(tcMask, 0, sizeof(tcMask));
	for (i=0; i<lenGprsApn; i++)
		strcat(tcMask, "/c");                                              // Accept any character
	iRet = appGet(appGprsApn, tcApnP, lenGprsApn+1);
	CHECK(iRet>=0, lblDbaErr);
	strcpy(tcApnN, tcApnP);

	DisplayLeds(_OFF_);                                                    // Disable Leds
	DisplayHeader(_OFF_);                                                  // Disable Header
	DisplayFooter(_OFF_);                                                  // Disable Footer

	iRet = GL_Dialog_VirtualKeyboard(hGoal, NULL, "Enter Apn :", tcMask, tcApnN, sizeof(tcApnN), GL_TIME_MINUTE);

	if (IsColorDisplay()) {                                                // Color terminal?  Yes
		DisplayLeds(_ON_);                                                 // Enable Leds
		DisplayHeader(iHeader);                                            // Enable Header
		if(!GL_GraphicLib_IsTouchPresent(hGoal))                           // Check if it is a touch screen
			DisplayFooter(iFooter);                                        // No, restore Footer
	}

	CHECK((iRet!=GL_KEY_CANCEL) && (iRet!=GL_RESULT_INACTIVITY), lblEnd);  // Exit on cancel/timeout
	if (strcmp(tcApnN, tcApnP) != 0) {
		iRet = appPut(appGprsApn, tcApnN, lenGprsApn);                     // Save new apn
		CHECK(iRet>=0, lblDbaErr);
	}

	// Enter User Name 
	// ***************
	memset(tcMask, 0, sizeof(tcMask));
	for (i=0; i<lenGprsUser; i++)
		strcat(tcMask, "/c");                                              // Accept any character
	iRet = appGet(appGprsUser, tcUserP, lenGprsUser+1);
	CHECK(iRet>=0, lblDbaErr);
	strcpy(tcUserN, tcUserP);
	iRet = GL_Dialog_Text(hGoal, "User Name", "Enter User Name :", tcMask, tcUserN, sizeof(tcUserN), GL_TIME_MINUTE);
	CHECK((iRet!=GL_KEY_CANCEL) && (iRet!=GL_RESULT_INACTIVITY), lblEnd);  // Exit on cancel/timeout

	if (strcmp(tcUserN, tcUserP) != 0) {
		iRet = appPut(appGprsUser, tcUserN, lenGprsUser);                  // Save new user name
		CHECK(iRet>=0, lblDbaErr);
	}

	// Enter Password 
	// **************
	memset(tcMask, 0, sizeof(tcMask));
	for (i=0; i<lenGprsPass; i++)
		strcat(tcMask, "/c");                                              // Accept any character
	iRet = appGet(appGprsPass, tcPassP, lenGprsPass+1);
	CHECK(iRet>=0, lblDbaErr);
	strcpy(tcPassN, tcPassP);
	iRet = GL_Dialog_Password(hGoal, "Password", "Enter Password :", tcMask, tcPassN, sizeof(tcPassN), GL_TIME_MINUTE);
	CHECK((iRet!=GL_KEY_CANCEL) && (iRet!=GL_RESULT_INACTIVITY), lblEnd);  // Exit on cancel/timeout

	if (strcmp(tcPassN, tcPassP) != 0){
		iRet = appPut(appGprsPass, tcPassN, lenGprsPass);                  // Save new password
		CHECK(iRet>=0, lblDbaErr);
	}

	// Enter Remote IP address
	// ***********************
	iRet = appGet(appGprsIpRemote, tcIpAddressP, lenGprsIpRemote+1);
	CHECK(iRet>=0, lblDbaErr);
	strcpy(tcIpAddressN, tcIpAddressP);
	uiIp = IpToUint(tcIpAddressN);                                         // Convert Ip address to unsigned int
	CHECK(uiIp>=0, lblKO);
	iRet = GL_Dialog_Ip(hGoal, "Remote Ip Address", "Enter Ip Address :", (T_GL_HIP)&uiIp, GL_TIME_MINUTE);
	CHECK((iRet!=GL_KEY_CANCEL) && (iRet!=GL_RESULT_INACTIVITY), lblEnd);  // Exit on cancel/timeout
	strcpy(tcIpAddressN, UintToIp(uiIp));                                  // Convert unsigned int to Ip address

	if (strcmp(tcIpAddressN, tcIpAddressP) != 0){
		iRet = appPut(appGprsIpRemote, tcIpAddressN, lenGprsIpRemote);     // Save new Ip address
		CHECK(iRet>=0, lblDbaErr);
	}

	// Enter Port number 
	// *****************
	memset(tcMask, 0, sizeof(tcMask));
	for (i=0; i<lenGprsPort; i++)
		strcat(tcMask, "/d");                                              // Accept only decimal character
	iRet = appGet(appGprsPort, tcPortP, lenGprsPort+1);
	CHECK(iRet>=0, lblDbaErr);
	strcpy(tcPortN, tcPortP);
	iRet = GL_Dialog_Text(hGoal, "Port Number", "Enter Port# :", tcMask, tcPortN, sizeof(tcPortN), GL_TIME_MINUTE);
	CHECK((iRet!=GL_KEY_CANCEL) && (iRet!=GL_RESULT_INACTIVITY), lblEnd);  // Exit on cancel/timeout

	if (strcmp(tcPortN, tcPortP) != 0){
		iRet = appPut(appGprsPort, tcPortN, lenGprsPort);                  // Save new Port number
		CHECK(iRet>=0, lblDbaErr);
	}

	goto lblEnd;

	// Errors treatment 
	// ****************
	lblKO:                                                                     // None-classified low level error
	GL_Dialog_Message(hGoal, NULL, "Processing Error", GL_ICON_ERROR, GL_BUTTON_VALID, 5*1000);
	goto lblEnd;

	lblDbaErr:                                                                 // Data base error
	Telium_Sprintf(tcDisplay, "%s\n%s", FMG_ErrorMsg(iRet), "Software Reset Needed");
	GL_Dialog_Message(hGoal, NULL, tcDisplay, GL_ICON_ERROR, GL_BUTTON_VALID, 5*1000);
	goto lblEnd;

	lblEnd:
	return;
}


void CommsGetChannel(byte CommsChannelNow){
	//	T_EGPRS_GET_INFORMATION info;
	//	Telium_File_t *gprs = NULL;
	byte ConnMode = 0;

	ConnMode = 'G'; //Default mode is GPRS

	///========= Ethernet Terminal and is Connected
	if (IsRadioETHERNET()) {
		if (IsETHERNET()) {
			if (IsEthernetConnected()) {
				ConnMode = 'T';
				goto lblEnd;
			}
		}
	}

	///======= WIFI terminal and is connected
	if (IsRadioWifi()) {
		if (IsWifi()) {
			if (Wifi_IsConnected()) {
				ConnMode = 'W';
				goto lblEnd;
			}
		}
	}

	///======= GPRS terminal and is connected
	if (IsRadioGPRS()) {
		if (IsGPRS()) {
			ConnMode = 'G';
			goto lblEnd;
		}
	}

	lblEnd:
	CommsChannelNow = ConnMode;
	mapPutByte(appCommRoute, ConnMode);
}



void ComGPRS_Prepare(void) {
	// Local variables
	// ***************
	T_GL_HWIDGET hScreen=NULL;    // Screen handle
	char *pcStr, tcStr[128+1];
	int iRet = 0;

	hScreen = GoalCreateScreen(hGoal, txGPRS, NUMBER_OF_LINES(txGPRS), GL_ENCODING_UTF8);
	CHECK(hScreen!=NULL, lblKO);                                    // Create screen and clear it

	// Attachment to the GPRS network in progress
	// ******************************************
	pcStr = tcStr;                                                  // Build param string
	iRet = appGet(appGprsApn, pcStr, lenGprsApn+1);                 // Retrieve apn
	CHECK(iRet>=0, lblDbaErr);
	pcStr += strlen(pcStr);
	*pcStr = '|'; pcStr++;

	iRet = appGet(appGprsUser, pcStr, lenGprsUser+1);               // Retrieve username
	CHECK(iRet>=0, lblDbaErr);
	pcStr += strlen(pcStr);
	*pcStr = '|'; pcStr++;

	iRet = appGet(appGprsPass, pcStr, lenGprsPass+1);               // Retrieve password
	CHECK(iRet>=0, lblDbaErr);
	pcStr += strlen(pcStr);
	*pcStr = '|';

	iRet = StartGPRS(hScreen, tcStr);                               // ** Start **
	CHECK(iRet>=0, lblComKO);
	CHECK(iRet!=0, lblEnd);

	lblKO:
	lblComKO:
	lblEnd:
	lblDbaErr:
	if (hScreen)
		GoalDestroyScreen(&hScreen);                                  // Destroy screen
}

//****************************************************************************
//                      void ComGPRS (void)                            
//  This function communicates through the GPRS layer.   
//  This function has no parameters.
//  This function has no return value
//****************************************************************************

int ComGPRS(tBuffer * req,tBuffer * rsp, word SSL) {
	// Local variables 
	// ***************
	T_GL_HWIDGET hScreen=NULL;    // Screen handle
	LL_HANDLE hGPRS=NULL;
	char tcStr[128+1];
	byte RespBuffer[4096];
	char tcIpAddress[100+1];
	char tcPort[100+1];
	char tcDisplay[50+1];
	char TempData[5];
	int iRet=0, iStatus=0,RetVal = -1,ret = 0;
	card mnuItem = 0;

	memset(tcStr, 0, sizeof(tcStr));
	memset(tcPort, 0, sizeof(tcPort));
	memset(TempData, 0, sizeof(TempData));
	memset(tcDisplay, 0, sizeof(tcDisplay));
	memset(RespBuffer, 0, sizeof(RespBuffer));
	memset(tcIpAddress, 0, sizeof(tcIpAddress));

	MAPGET(traMnuItm,TempData,lblKO);
	dec2num(&mnuItem, TempData, 0);

	hScreen = GoalCreateScreen(hGoal, txGPRS, NUMBER_OF_LINES(txGPRS), GL_ENCODING_UTF8);
	CHECK(hScreen!=NULL, lblKO);                                    // Create screen and clear it

	iRet = GoalClrScreen(hScreen, GL_COLOR_BLACK, KEY_CANCEL, false);
	CHECK(iRet>=0, lblKO);

	//
	//	// Attachment to the GPRS network in progress
	//	// ******************************************
	//	pcStr = tcStr;                                                  // Build param string
	//	iRet = appGet(appGprsApn, pcStr, lenGprsApn+1);                 // Retrieve apn
	//	CHECK(iRet>=0, lblDbaErr);
	//	pcStr += strlen(pcStr);
	//	*pcStr = '|'; pcStr++;
	//
	//	iRet = appGet(appGprsUser, pcStr, lenGprsUser+1);               // Retrieve username
	//	CHECK(iRet>=0, lblDbaErr);
	//	pcStr += strlen(pcStr);
	//	*pcStr = '|'; pcStr++;
	//
	//	iRet = appGet(appGprsPass, pcStr, lenGprsPass+1);               // Retrieve password
	//	CHECK(iRet>=0, lblDbaErr);
	//	pcStr += strlen(pcStr);
	//	*pcStr = '|';
	//
	//
	//	iRet = StartGPRS(hScreen, tcStr);                               // ** Start **
	//	CHECK(iRet>=0, lblComKO);
	//	CHECK(iRet!=0, lblEnd);

	// Transmission through GPRS layer in progress
	// *******************************************

	// Open GPRS layer
	// ===============
	//	iRet = GoalDspLine(hScreen, 2, "Connecting.....", &txGPRS[3], 0, true);
	//	CHECK(iRet>=0, lblKO);

	iRet = appGet(appGprsIpRemote, tcIpAddress, lenGprsIpRemote+1);   // Retrieve remote IP
	CHECK(iRet>=0, lblDbaErr);

	iRet = appGet(appGprsPort, tcPort, lenGprsPort+1);                // Retrieve port number
	CHECK(iRet>=0, lblDbaErr);

	Telium_Sprintf (tcStr, "%s|%s", tcIpAddress, tcPort);
	hGPRS = OpenGPRS(tcStr, SSL);                                          // ** Open **
	CHECK(hGPRS!=NULL, lblKO);

	IsGPRS();

	// Connect GPRS layer
	// ==================
	iRet = ConnectGPRS(hGPRS);                                        // ** Connect **
	if(iRet == LL_ERROR_NETWORK_NOT_READY) { ComGPRS_Prepare(); iRet = ConnectGPRS(hGPRS); }
	CHECK(iRet>=0, lblComKO);

	// Clear sending/receiving buffers
	// ===============================
	iRet = LL_ClearSendBuffer(hGPRS);
	CHECK(iRet==LL_ERROR_OK, lblComKO);
	iRet = LL_ClearReceiveBuffer(hGPRS);
	CHECK(iRet==LL_ERROR_OK, lblComKO);

	// Send data through GPRS layer
	// ============================
	iRet = GoalClrScreen(hScreen, GL_COLOR_BLACK, KEY_CANCEL, false); // Clear screen
	CHECK(iRet>=0, lblKO);

	iRet = GoalDspLine(hScreen, 2, "Sending...", &txGPRS[1], 0, true);
	CHECK(iRet>=0, lblKO);

	/////iso message sending
	iRet = SendGPRS(hGPRS, bufPtr(req), bufLen(req));              // ** Send data **
	CHECK(iRet>=0, lblComKO);

	iRet = GoalClrScreen(hScreen, GL_COLOR_BLACK, KEY_CANCEL, false); // Clear screen
	CHECK(iRet>=0, lblKO);

	// Receive data through GPRS layer
	// ===============================
	iRet = GoalDspLine(hScreen, 2, "Receiving...", &txGPRS[4], 0, true);
	CHECK(iRet>=0, lblKO);

	buzzer(10);

	iRet = ReceiveGPRS(hGPRS, hScreen,  RespBuffer, (int) bufDim(rsp), 30);     // ** Receive data **
	CHECK(iRet>=0, lblNoresp);
	bufApp(rsp, RespBuffer, iRet);

	RetVal = iRet;
	if (iRet > MAX_RSP) {
		iRet = GoalDspLine(hScreen, 2, "Buffer overflow Max=512", &txGPRS[1], 0, true);
		CHECK(iRet>=0, lblKO);
	}

	// Disconnection
	// =============
	//	if (mnuItem != mnuSettlement) {
	iRet = DisconnectGPRS(hGPRS);                                     // ** Disconnect **
	CHECK(iRet>=0, lblComKO);

	iRet = CloseGPRS(hGPRS);                                          // ** Close **
	CHECK(iRet>=0, lblComKO);
	//	}

	goto lblEnd;

	// Errors treatment 
	// ****************
	lblKO:                                                                // None-classified low level error
	iRet = 0;
	GL_Dialog_Message(hGoal, NULL, "Connection FAILED!!", GL_ICON_ERROR, GL_BUTTON_VALID, 5*1000);
	goto lblEnd;
	lblComKO:                                                             // Communication error
	iRet = 0;
	strcpy(tcDisplay, LL_ErrorMsg(iRet));                             // Link Layer error
	if (iRet == LL_Network_GetStatus(LL_PHYSICAL_V_GPRS, &iStatus)) {
		switch(iStatus) {
		case LL_STATUS_GPRS_ERROR_NO_SIM:   iStatus=LL_STATUS_GPRS_NO_SIM;      break;
		case LL_STATUS_GPRS_ERROR_PPP:      iStatus=LL_STATUS_GPRS_ERR_PPP;     break;
		case LL_STATUS_GPRS_ERROR_UNKNOWN:  iStatus=LL_STATUS_GPRS_ERR_UNKNOWN; break;
		default:                            iStatus=-1;                         break;
		}
		strcat(tcDisplay, "\n");
		strcat(tcDisplay, LL_ErrorMsg(iStatus));                      // Link Layer status
	}
	if (iRet != LL_ERROR_TIMEOUT)                                     // Cancel or timeout ?
		GL_Dialog_Message(hGoal, NULL, tcDisplay, GL_ICON_ERROR, GL_BUTTON_VALID, 3*1000);
	goto lblEnd;

	lblNoresp:  // Communication error
	iRet = 0;
	GL_Dialog_Message(hGoal, NULL, "NO RESPONSE", GL_ICON_ERROR, GL_BUTTON_VALID, 3*1000);
	goto lblEnd;

	lblDbaErr:                                                            // Data base error
	iRet = 0;
	Telium_Sprintf(tcDisplay, "%s\n%s", FMG_ErrorMsg(iRet), "Software Reset Needed");
	GL_Dialog_Message(hGoal, NULL, tcDisplay, GL_ICON_ERROR, GL_BUTTON_VALID, 3*1000);
	goto lblEnd;
	lblEnd:
	if (hGPRS) {
		iRet = DisconnectGPRS(hGPRS);                                        // ** Disconnect **
		// ** Disconnect **
		if (iRet == LL_ERROR_OK) {
			CloseGPRS(hGPRS);                                             // ** Close **
		}
	}
	//StopGPRS();                                                       // ** Stop **
	if (hScreen)
		GoalDestroyScreen(&hScreen);                                  // Destroy screen

	return RetVal;
}



//****************************************************************************
//                      void ComGPRSCheck (void)
//  This function communicates through the GPRS layer.
//  This function has no parameters.
//  This function has no return value
//****************************************************************************
int ComGPRSCheck(int SSL) {
	// Local variables
	// ***************
	T_GL_HWIDGET hScreen=NULL;    // Screen handle
	LL_HANDLE hGPRS=NULL;
	char tcStr[128+1];
	char tcIpAddress[100+1];
	char tcPort[100+1];
	int iRet = 0, RetVal = -1;

	memset(tcStr, 0, sizeof(tcStr));
	memset(tcPort, 0, sizeof(tcPort));
	memset(tcIpAddress, 0, sizeof(tcIpAddress));


	iRet = appGet(appGprsIpRemote, tcIpAddress, lenGprsIpRemote+1);   // Retrieve remote IP
	CHECK(iRet>=0, lblKO);

	iRet = appGet(appGprsPort, tcPort, lenGprsPort+1);                // Retrieve port number
	CHECK(iRet>=0, lblKO);

	Telium_Sprintf (tcStr, "%s|%s", tcIpAddress, tcPort);
	hGPRS = OpenGPRS(tcStr, SSL);                                          // ** Open **
	CHECK(hGPRS!=NULL, lblKO);

	IsGPRS();

	// Connect GPRS layer
	// ==================
	iRet = ConnectGPRS(hGPRS);                                        // ** Connect **
	if(iRet == LL_ERROR_NETWORK_NOT_READY) {
		ComGPRS_Prepare();
		iRet = ConnectGPRS(hGPRS);
	}
	CHECK(iRet>=0, lblComKO);

	// Clear sending/receiving buffers
	// ===============================
	iRet = LL_ClearSendBuffer(hGPRS);
	CHECK(iRet==LL_ERROR_OK, lblComKO);
	iRet = LL_ClearReceiveBuffer(hGPRS);
	CHECK(iRet==LL_ERROR_OK, lblComKO);

	// Disconnection
	// =============
	iRet = DisconnectGPRS(hGPRS);                                     // ** Disconnect **
	CHECK(iRet>=0, lblComKO);

	//	iRet = CloseGPRS(hGPRS);                                          // ** Close **
	//	CHECK(iRet>=0, lblComKO);

	RetVal = 1;
	goto lblEnd;

	// Errors treatment
	// ****************
	lblKO:                                                                // None-classified low level error

	lblComKO:                                                             // Communication error

	lblEnd:
	if (hGPRS) {
		DisconnectGPRS(hGPRS);                                        // ** Disconnect **
		//		CloseGPRS(hGPRS);                                             // ** Close **
	}

	//StopGPRS();                                                       // ** Stop **
	if (hScreen)
		GoalDestroyScreen(&hScreen);                                  // Destroy screen

	return RetVal;
}


int comGPRS_SetDefaultsValues(void){
	char tcApnN[lenGprsApn+1];
	char tcUserN[lenGprsUser+1];
	char tcPassN[lenGprsPass+1];
	S_INFOS_RESEAU_SUP gprs_infos;

	//reset local var buffers
	memset(tcApnN, 0, sizeof(tcApnN));
	memset(tcUserN, 0, sizeof(tcUserN));
	memset(tcPassN, 0, sizeof(tcPassN));

	//get the default data from the manager/OS of terminal
	PSQ_Donner_infos_reseau_sup( &gprs_infos) ;
	strcpy(tcApnN,(char *)gprs_infos.infos_sup.apn);
	strcpy(tcUserN,(char *)gprs_infos.infos_sup.login);
	strcpy(tcPassN,(char *)gprs_infos.infos_sup.password);

	appPut(appGprsApn, tcApnN, lenGprsApn);                     // Save new apn
	appPut(appGprsUser, tcUserN, lenGprsUser);                  // Save new user name
	appPut(appGprsPass, tcPassN, lenGprsPass);                  // Save new password

	return 1;
}

