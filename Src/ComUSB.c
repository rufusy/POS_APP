//****************************************************************************
//       INGENICO                                INGEDEV 7                  
//============================================================================
//       FILE  COMSERIAL.C                        (Copyright INGENICO 2012)
//============================================================================
//  Created :       11-July-2012         Kassovic
//  Last modified : 06-December-2013     Kassovic
//  Module : TRAINING                                                          
//                                                                          
//  Purpose :                                                               
//                 *** USB communication management ***
//  How to use the linklayer to create an USB configuration to communicate.
//                 Transmission by USB communication
//                   Test done with Hyper-terminal
//  !!! Link Layer application must be loaded 3628xxxx.SGN inside the terminal
//      TlvTree_TPlus.lib + LinkLayerExeInterface_TPlus.lib must be implemented to link properly.
//                                                                            
//  List of routines in file :  
//      OpenUSB : Create the USB port.
//      ConnectUSB : Connect the USB port.
//      SendUSB : Send data through the USB port.
//      ReceiveUSB : Receive data through the USB port.
//      DisconnectUSB : Disconnect the USB port.                                           
//      CloseUSB : Delete the USB port.
//      ComUSB : USB communication demo.
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
#include "LinkLayer.h"
#include "TlvTree.h"

//****************************************************************************
//      EXTERN                                                              
//****************************************************************************
extern T_GL_HGRAPHIC_LIB hGoal; // Handle of the graphics object library

//****************************************************************************
//      PRIVATE CONSTANTS                                                   
//****************************************************************************
#define MAX_SND  512
#define MAX_RSP  512

//****************************************************************************
//      PRIVATE TYPES                                                       
//****************************************************************************
/* */

//****************************************************************************
//      PRIVATE DATA                                                        
//****************************************************************************
// Properties of the USB screen (Goal)
// ===================================
static const ST_DSP_LINE txUSB[] = {
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
static const ST_PRN_LINE xPrinter = {
		GL_ALIGN_LEFT, GL_ALIGN_CENTER, FALSE, 100, {0, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {1, 0, 1, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_LARGE}
};

//****************************************************************************
//                         LL_HANDLE OpenUSB (void)
//  This function configures the USB port.     
//       LL_Configure() : Create Link Layer configuration
//  This function has no parameters.  
//  This function has return value
//    !NULL : Handle of the session
//     NULL : Session failed
//****************************************************************************

static LL_HANDLE OpenUSB(void) {
	// Local variables 
	// ***************
	TLV_TREE_NODE piConfig=NULL;
	TLV_TREE_NODE piPhysicalConfig=NULL;
	LL_HANDLE hSession = NULL;                                        // Session handle
	int iRet;

	// Create the LinkLayer configuration parameters tree 
	// **************************************************

	// Create parameters tree
	// ======================
	piConfig = TlvTree_New(LL_TAG_LINK_LAYER_CONFIG);                 // LinkLayer parameters Root tag of the configuration tree
	CHECK(piConfig!=0, lblKOConfigure);

	// Physical layer parameters  
	// =========================
	piPhysicalConfig = TlvTree_AddChild(piConfig,                        
			LL_TAG_PHYSICAL_LAYER_CONFIG, // TAG Physical layer parameters
			NULL,                         // VALUE (Null)
			0);                           // LENGTH 0
	CHECK(piPhysicalConfig!=0, lblKOConfigure);

	// Port USB
	// --------
	TlvTree_AddChildInteger(piPhysicalConfig, 
			LL_PHYSICAL_T_LINK,                       // TAG
			LL_PHYSICAL_V_USB,                        // VALUE
			LL_PHYSICAL_L_LINK);                      // LENGTH 1 byte

	// Link Layer configuration
	// ************************
	iRet = LL_Configure(&hSession, piConfig);                         // Initialize the handle of the session
	CHECK(iRet==LL_ERROR_OK, lblKOConfigure);

	goto lblEnd;

	// Errors treatment
	// ****************
	lblKOConfigure:                                                       // Configuration failed
	hSession=NULL;
	goto lblEnd;
	lblEnd:
	if (piConfig)
		TlvTree_Release(piConfig);                                    // Release tree to avoid memory leak
	return hSession;                                                  // Return the handle of the session
}


//****************************************************************************
//                   int ConnectUSB (LL_HANDLE hSession)
//  This function connects the USB port.     
//       LL_Connect() : Connect Link Layer
//  This function has no parameters.  
//    hSession (I-) : Handle of the session
//  This function has return value
//    >=0 : Connect done
//     <0 : Connect failed                                        
//****************************************************************************

static int ConnectUSB(LL_HANDLE hSession) {
	// Local variables 
	// ***************
	int iRet;

	// Link Layer connection
	// *********************
	iRet = LL_Connect(hSession);

	return iRet;
}

//****************************************************************************
//     int SendUSB (LL_HANDLE hSession, const char *pcMsg, word usLen)
//  This function sends data through the USB port.      
//       LL_Send() : Send data
//       LL_GetLastError() : Retrieve the last error
//  This function has parameters.
//    hSession (I-) : Handle of the session
//    pcMsg (I-) : Data to send
//    usLen (I-) : Number of byte to send
//  This function has return value
//    >=0 : Number of bytes sent
//     <0 : Transmission failed                                       
//****************************************************************************

static int SendUSB(LL_HANDLE hSession, const char *pcMsg, word usLen) {
	// Local variables 
	// ***************
	int iRet;

	// Send data
	// *********
	iRet = LL_Send(hSession, usLen, pcMsg, LL_INFINITE);
	if(iRet != usLen)
		iRet = LL_GetLastError(hSession);

	return iRet;
}

//****************************************************************************
//        int ReceiveUSB (LL_HANDLE hSession, T_GL_HWIDGET hScreen,
//                        char *pcMsg, word usLen, byte ucDly)
//  This function receives data through the USB port.      
//       LL_ClearReceiveBuffer() : Clear receiving buffer
//       LL_Receive() : Wait and receive data
//       LL_GetLastError() : Retrieve the last error
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

static int ReceiveUSB(LL_HANDLE hSession, T_GL_HWIDGET hScreen, char *pcMsg, word usLen, byte ucDly) {
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
	if ((iNbrBytes != 0) && (iNbrBytes < usLen)) {
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
	lblTimeOut:                                                                // Configuration failed
	iRet = LL_ERROR_TIMEOUT;
	goto lblEnd;
	lblEnd:
	TimerStop(0);
	return iRet;
}

//****************************************************************************
//                int DisconnectUSB (LL_HANDLE hSession)
//  This function disconnects the USB port.      
//       LL_Disconnect() : Disconnect Link Layer
//  This function has no parameters.
//    hSession (I-) : Handle of the session
//  This function has return value
//    >=0 : Disconnect done
//     <0 : Disconnect failed                                       
//****************************************************************************

static int DisconnectUSB(LL_HANDLE hSession)
{	
	// Local variables 
	// ***************
	int iRet;

	// Link Layer disconnection
	// ************************
	iRet = LL_Disconnect(hSession);

	return iRet;
}

//****************************************************************************
//                  int CloseUSB(LL_HANDLE hSession)
//  This function deletes the handle of the USB port.   
//       LL_Configure() : Delete Link Layer configuration   
//  This function has parameters.
//    hSession (IO) : Handle of the session
//  This function has return value
//    >=0 : Configuration deleted
//     <0 : Processing failed                                        
//****************************************************************************

static int CloseUSB(LL_HANDLE hSession)
{
	// Local variables 
	// ***************
	int iRet;

	// Delete the configuration
	// ************************
	iRet = LL_Configure(&hSession, NULL);

	return iRet;
}

//****************************************************************************
//                      void ComUSB(void)                            
//  This function communicates through USB port.   
//  This function has parameters.
//  This function has no return value
//****************************************************************************

int ComUSB(tBuffer * req,tBuffer * rsp, word SSL){
	// Local variables 
	// ***************
	T_GL_HWIDGET hScreen=NULL;    // Screen handle
	T_GL_HWIDGET hDocument=NULL;  // Document handle
	ST_PRN_LINE xLine;
	LL_HANDLE hUSB=NULL;
	char tcSnd[MAX_SND+1];
	char tcRsp[MAX_RSP+1];
	char tcDisplay[50+1];
	byte p; // Printer line index
	int iRet=0, iStatus=0,RetVal = -1;

	// Transmission through USB port in progress
	// *****************************************
	memset(tcSnd, 0, sizeof(tcSnd));
	memset(tcRsp, 0, sizeof(tcRsp));

	hScreen = GoalCreateScreen(hGoal, txUSB, NUMBER_OF_LINES(txUSB), GL_ENCODING_UTF8);
	CHECK(hScreen!=NULL, lblKO);
	iRet = GoalClrScreen(hScreen, GL_COLOR_BLACK, KEY_CANCEL, false);
	CHECK(iRet>=0, lblKO);                                            // Create screen and clear it

	// Open USB port
	// =============
	iRet = GoalDspLine(hScreen, 0, "1-comOpen...", &txUSB[0], 0, true);
	CHECK(iRet>=0, lblKO);
	hUSB = OpenUSB();                                                 // ** Open **
	CHECK(iRet>=0, lblComKO);
	iRet = GoalDspLine(hScreen, 1, "1-comOpen OK", &txUSB[1], 0, true);
	CHECK(iRet>=0, lblKO);

	// Connect USB port
	// ================
	iRet = GoalDspLine(hScreen, 2, "2-comConnect...", &txUSB[2], 0, true);
	CHECK(iRet>=0, lblKO);
	iRet = ConnectUSB(hUSB);                                          // ** Connect **
	CHECK(iRet>=0, lblComKO);
	iRet = GoalDspLine(hScreen, 3, "2-comConnect OK", &txUSB[3], 0, true);
	CHECK(iRet>=0, lblKO);

	// Clear sending/receiving buffers
	// ===============================
	iRet = LL_ClearSendBuffer(hUSB);
	CHECK(iRet==LL_ERROR_OK, lblComKO);
	iRet = LL_ClearReceiveBuffer(hUSB);
	CHECK(iRet==LL_ERROR_OK, lblComKO);

	// Send data through USB port
	// ==========================
	iRet = GoalDspLine(hScreen, 4, "3-comSend...", &txUSB[4], 0, true);
	CHECK(iRet>=0, lblKO);
	strcpy(tcSnd, "Hello, do you hear me?\r\n"                        // Data to send
			"Hello, do you hear me?\r\n"
			"Hello, do you hear me?\r\n"
			"Hello, do you hear me?\r\n"
			"Hello, do you hear me?\r\n");
	iRet = SendUSB(hUSB, tcSnd, (word) strlen(tcSnd));                // ** Send data **
	CHECK(iRet>=0, lblComKO);
	iRet = GoalClrScreen(hScreen, GL_COLOR_BLACK, KEY_CANCEL, false); // Clear screen
	CHECK(iRet>=0, lblKO);
	iRet = GoalDspLine(hScreen, 0, "3-comSend OK", &txUSB[0], 0, true);
	CHECK(iRet>=0, lblKO);

	// Receive data through USB port
	// =============================
	iRet = GoalDspLine(hScreen, 1, "4-comRec...", &txUSB[1], 0, true);
	CHECK(iRet>=0, lblKO);
	iRet = ReceiveUSB(hUSB, hScreen, tcRsp, sizeof(tcRsp), 30);       // ** Receive data **
	CHECK(iRet>=0, lblComKO);
	RetVal = iRet;

	if (iRet > MAX_RSP) strcpy (tcRsp, "Buffer overflow Max=512");    // Data overflow
	iRet = GoalDspLine(hScreen, 2, "4-comRec OK", &txUSB[2], 0, true);
	CHECK(iRet>=0, lblKO);

	// Disconnection
	// =============
	iRet = DisconnectUSB(hUSB);                                       // ** Disconnect **
	CHECK(iRet>=0, lblComKO);
	iRet = GoalDspLine(hScreen, 3, "5-comDisconn OK", &txUSB[3], 0, true);
	CHECK(iRet>=0, lblKO);
	iRet = CloseUSB(hUSB);                                            // ** Close **
	CHECK(iRet>=0, lblComKO);

	// Display transmission data
	// =========================
	iRet = GoalClrScreen(hScreen, GL_COLOR_BLACK, KEY_CANCEL | KEY_VALID, false);  // Clear screen
	CHECK(iRet>=0, lblKO);
	iRet = GoalDspMultiLine(hScreen, 0, tcRsp, strlen(tcRsp), txUSB, NUMBER_OF_LINES(txUSB), 0, true);
	CHECK(iRet>=0, lblKO);                                            // Show data to receive

	// Print transmission data
	// =======================
	p=0;
	hDocument = GoalCreateDocument(hGoal, GL_ENCODING_UTF8);          // Create document
	CHECK(hDocument!=NULL, lblKO);

	xLine = xPrinter;                                                 // Build document (Demo, Tx/Rx data)
	xLine.eTextAlign = GL_ALIGN_CENTER;
	xLine.bReverse = TRUE;
	xLine.xMargin.usBottom = PIXEL_BOTTOM;
	xLine.xFont.eScale = GL_SCALE_XXLARGE;
	iRet = GoalPrnLine(hDocument, p++, "USB Demo", &xLine);
	CHECK(iRet>=0, lblKO);

	xLine = xPrinter;
	iRet = GoalPrnLine(hDocument, p++, "Sending:", &xLine);           // Data to send
	CHECK(iRet>=0, lblKO);
	iRet = GoalPrnLine(hDocument, p++, tcSnd, &xLine);
	CHECK(iRet>=0, lblKO);
	iRet = GoalPrnLine(hDocument, p++, "\nReceiving:", &xLine);       // Data to receive
	CHECK(iRet>=0, lblKO);
	iRet = GoalPrnLine(hDocument, p++, tcRsp, &xLine);
	CHECK(iRet>=0, lblKO);
	iRet = GoalPrnLine(hDocument, p++, "\n\n\n\n", &xLine);
	CHECK(iRet>=0, lblKO);

	iRet = GoalPrnDocument(hDocument);
	CHECK(iRet>=0, lblKO);

	GoalGetKey(hScreen, hGoal, true, 30*1000, true);                  // Wait for key pressed/touched (shortcut)

	goto lblEnd;

	// Errors treatment 
	// ****************
	lblKO:                                                                // None-classified low level error
	GL_Dialog_Message(hGoal, NULL, "Processing Error", GL_ICON_ERROR, GL_BUTTON_VALID, 5*1000);
	goto lblEnd;
	lblComKO:                                                             // Communication error
	strcpy(tcDisplay, LL_ErrorMsg(iRet));                             // Link Layer error
	if (iRet == LL_Network_GetStatus(LL_PHYSICAL_V_USB, &iStatus)) {
		switch(iStatus) {
		// General physical status
		case LL_STATUS_PERIPHERAL_BUSY:             iStatus=LL_STATUS_PHY_PERIPHERAL_BUSY;         break;
		case LL_STATUS_PERIPHERAL_OUT_OF_BASE:      iStatus=LL_STATUS_PHY_PERIPHERAL_OUT_OF_BASE;  break;
		case LL_STATUS_CONVERTER_NOT_PLUGGED:       iStatus=LL_STATUS_PHY_CONVERTER_NOT_PLUGGED;   break;
		// Bluetooth status
		case LL_BT_STATUS_NO_BASE_ASSOCIATED:       iStatus=LL_STATUS_BT_NO_BASE_ASSOCIATED;       break;
		case LL_BT_STATUS_PERIPHERAL_NOT_AVAILABLE: iStatus=LL_STATUS_BT_PERIPHERAL_NOT_AVAILABLE; break;
		case LL_BT_STATUS_PERIPHERAL_BUSY:          iStatus=LL_STATUS_BT_PERIPHERAL_BUSY;          break;
		case LL_BT_STATUS_BASE_NOT_AVAILABLE:       iStatus=LL_STATUS_BT_BASE_NOT_AVAILABLE;       break;
		default:                                    iStatus=-1;                                    break;
		}
		strcat(tcDisplay, "\n");
		strcat(tcDisplay, LL_ErrorMsg(iStatus));                      // Link Layer status
	}
	if (iRet != LL_ERROR_TIMEOUT)                                     // Cancel or timeout ?
		GL_Dialog_Message(hGoal, NULL, tcDisplay, GL_ICON_ERROR, GL_BUTTON_VALID, 5*1000);
	goto lblEnd;
	lblEnd:
	if (hUSB) {
		DisconnectUSB(hUSB);                                          // ** Disconnect **
		CloseUSB(hUSB);                                               // ** Close **
	}
	if (hScreen)
		GoalDestroyScreen(&hScreen);                                  // Destroy screen
	if (hDocument)
		GoalDestroyDocument(&hDocument);                              // Destroy document
	return RetVal;
}

