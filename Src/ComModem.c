//****************************************************************************
//       INGENICO                                INGEDEV 7                    
//============================================================================
//       FILE  COMMODEM.C                         (Copyright INGENICO 2012)
//============================================================================
//  Created :       11-July-2008         Kassovic
//  Last modified : 06-December-2013     Kassovic
//  Module : TRAINING                                                          
//                                                                          
//  Purpose :                                                               
//                   *** Modem communication management ***
//  How to use the linklayer to create a modem configuration to communicate.
//                   Transmission by Asynchronous Modem
//               Test done with Hyper-terminal (8-N-1-19200)
//  !!! Link Layer application must be loaded 3628xxxx.SGN inside the terminal
//      TlvTree_TPlus.lib + LinkLayerExeInterface_TPlus.lib must be implemented to link properly.
//                                                                            
//  List of routines in file :  
//      OpenModem : Create the modem port.
//      ConnectModem : Connect the modem port.
//      SendModem : Send data through the modem port.
//      ReceiveModem : Receive data through the modem port.
//      DisconnectModem : Disconnect the modem port.    
//      CloseModem : Delete the modem port.
//      PromptModem : Prompt for modem's parameters.
//      ComModem : Modem communication demo.
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

#define DIAL_TIMEOUT 30*100
#define HDLC_TIMEOUT 30*100

//****************************************************************************
//      PRIVATE TYPES                                                       
//****************************************************************************
enum eChn {        // Com channels
    chnMdm,        // Internal Modem
    chnHdlc,       // Hdlc
};

enum comDialMode_t
{
   COM_DIAL_DEFAULT, // Mode dialing
   COM_DIAL_PULSE,   // Decimal
   COM_DIAL_TONE     // DTMF
};

typedef struct sComChn 
{
	byte chn;                   // communication channel used
	byte bits;                  // data bits
	byte parity;                // parity 
} tComChn;

//****************************************************************************
//      PRIVATE DATA                                                        
//****************************************************************************
// Properties of the Modem screen (Goal)
// =====================================
static const ST_DSP_LINE txModem[] =
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

// List of dialog choice
// =====================
static const char *tzDataBits[] =
{
	"Seven",
	"Eight",
	NULL
};

static const char *tzParity[] =
{
	"None",
	"Even",
	"Odd",
	NULL
};

static const char *tzBaudRate[] =
{
	"300",
	"1200",
	"2400",
	"4800",
	"9600",
	"19200",
	"38400",
	"57600",
	"115200",
	NULL
};

static tComChn xCom; // Communication channel

//****************************************************************************
//         int OpenModem (const char *pcInit, const char *pcPhone,
//                        enum eChn eComType)
//  This function configures the modem port.  
//  For Synchronous pcInit is forced to "8N11200". 
//   - LL_Configure() : Create Link Layer configuration
//  This function has parameters.  
//    pcInit (I-) :  .?#MDPSB   (ex: ".?#T8N12400") => O=Optional M=Mandatory
//       O   . = Data compression
//       O   ? = Error correction
//       O   # = DTR management
//       O   M = T/P (Tone/Pulse fast connect)  t/p (tone/pulse NO fast connect)
//       M   D = Number of data bits (7,8)
//       M   P = Parity (None, Even, Odd)
//       M   S = Stop bits (1, 2=>is not supported)
//       M   B = Baud Rate (300,1200,2400,9600,19200)
//    pcPhone (I-)    ex: "0-00-33-169012345"
//           Pabx                  0
//           Prefix                00
//           Country code          33
//           Phone Number          169012345
//    eCommType (I-) : Communication channel used
//  This function has return value
//    !NULL : Handle of the session
//     NULL : Session failed
//****************************************************************************

static LL_HANDLE OpenModem(const char *pcInit, const char *pcPhone, enum eChn eComType)
{
	// Local variables 
    // ***************
    byte ucDatasize;                      // Implemented 
    byte ucParity;                        // Implemented 
    byte ucStopbits;                      // Implemented  
//  enum comDialMode_t eDialmode;         // NOT IMPLEMENTED
    int iBlindDial;                       // Implemented  
    byte ucFast;                          // Implemented  
    byte ucCompression;                   // Implemented                                  
    byte ucCorrection;                    // Implemented  
//   byte ucHangUpDtr;                    // NOT IMPLEMENTED
    doubleword uiBps, uiTimeout=0;
	char tcInitString[50];
    char *pcPabx, *pcPrefix, *pcCountry;
    int iLen;
    char tcTmp[64+1];
    char tcBuf[64+1];
	TLV_TREE_NODE piConfig=NULL;
	TLV_TREE_NODE piPhysicalConfig=NULL;
	TLV_TREE_NODE piDataLinkConfig=NULL;
	LL_HANDLE hSession = NULL;                                        // Session handle
    int iRet;

	// Create the LinkLayer configuration parameters tree 
	// **************************************************
    CHECK((pcInit!=NULL) && (pcPhone!=NULL), lblKOConfigure);         // Configuration error

    // Parsing initialization parameters
    // =================================
    if(eComType == chnHdlc)                                           // *** Hdlc parameters ***
        pcInit = "8N11200";                                           // 8Bits ParityNone 1Stop 1200 

    if(*pcInit == '.') {                                              // *** Compression field (optional) ***
		ucCompression=1; pcInit++;                                    // Compression 
    } else
        ucCompression=0;                                              // No compression

    if(*pcInit == '?') {                                              // *** Correction field (optional) ***
        ucCorrection=1; pcInit++;                                     // Correction
    } else
        ucCorrection=0;                                               // No correction

/*
    if(*pcInit == '#') {                                              // *** HangUp DTR (optional) ***
        ucHangUpDtr=1; pcInit++;                                      // DTR
    } else
        ucHangUpDtr=0;                                                // No DTR
*/

    switch (*pcInit)                                                  // *** Fast dialing ***
	{
    case 'P': case 'T': ucFast=0; pcInit++; break;                    // Upper case means baud rate negotiation
	case 'p': case 't': ucFast=1; pcInit++; break;                    // Lower case means baud rate without negotiation
    default:            ucFast=0; break;
    }

/*
    switch (*pcInit)                                                  // *** Dial mode (unused in Telium) ***                   
	{
	case 'T': case 't': eDialmode=COM_DIAL_TONE; pcInit++;  break;     // Dtmf
    case 'P': case 'p': eDialmode=COM_DIAL_PULSE; pcInit++; break;     // Decimal
    default:            eDialmode=COM_DIAL_DEFAULT;         break;     // Default
    }
*/

            // !!! Data format is 8 bits no parity 1 stop (Other framing/parity must be handled at application level)
    ucDatasize=LL_PHYSICAL_V_8_BITS;                                  // *** Number of data bits ***
    switch (*pcInit++) 
	{     
    case '7': xCom.bits='7'; break;                                   // 7 data bits (Must be handled at application level)
    case '8': xCom.bits='8'; break;                                   // 8 data bits
    default:  goto lblKOConfigure;                                    // Configuration error
    }
    
    ucParity=LL_PHYSICAL_V_NO_PARITY;                                 // *** Parity ***
    switch (*pcInit++)              
	{        
    case 'N': xCom.parity=0;   break;                                 // No parity
    case 'E': xCom.parity='E'; break;                                 // Even parity (Must be handled at application level)
    case 'O': xCom.parity='O'; break;                                 // Odd parity (Must be handled at application level)
	default:  goto lblKOConfigure;                                    // Configuration error
    }
    
    ucStopbits=LL_PHYSICAL_V_1_STOP;                                  // *** Number of stop bits ***
    switch (*pcInit++) 
	{          
    case '1': ucStopbits=LL_PHYSICAL_V_1_STOP; break;                 // 1 stop bit
    case '2':                                                         // 2 stop bits (Not supported by Telium modem)
    default:  goto lblKOConfigure;                                    // Configuration error
    }
    
    uiBps=LL_PHYSICAL_V_BAUDRATE_1200;                                // *** Baud rate ***
    if(memcmp(pcInit, "300", 3) == 0) {                               // 300 bauds
		uiBps = (ucFast ? LL_PHYSICAL_V_BAUDRATE_1200 : LL_PHYSICAL_V_BAUDRATE_300); pcInit += 3; 
	} else if(memcmp(pcInit, "1200", 4) == 0) {                       // 1200 bauds  
		uiBps = (ucFast ? LL_PHYSICAL_V_BAUDRATE_1200 : LL_PHYSICAL_V_BAUDRATE_1200); pcInit += 4;
    } else if(memcmp(pcInit, "2400", 4) == 0) {                       // 2400 bauds  
        uiBps =  (ucFast ? LL_PHYSICAL_V_BAUDRATE_1200 : LL_PHYSICAL_V_BAUDRATE_2400); pcInit += 4;
    } else if(memcmp(pcInit, "4800", 4) == 0) {                       // 2400 bauds  
        uiBps =  (ucFast ? LL_PHYSICAL_V_BAUDRATE_1200 : LL_PHYSICAL_V_BAUDRATE_4800); pcInit += 4;  
    } else if(memcmp(pcInit, "9600", 4) == 0) {                       // 9600 bauds 
        uiBps = (ucFast ? LL_PHYSICAL_V_BAUDRATE_1200 : LL_PHYSICAL_V_BAUDRATE_9600); pcInit += 4;
    } else if(memcmp(pcInit, "19200", 5) == 0) {                      // 19200 bauds
        uiBps = (ucFast ? LL_PHYSICAL_V_BAUDRATE_1200 : LL_PHYSICAL_V_BAUDRATE_19200); pcInit += 5;
    } else if(memcmp(pcInit, "38400", 5) == 0) {                      // 38400 bauds
        uiBps = (ucFast ? LL_PHYSICAL_V_BAUDRATE_1200 : LL_PHYSICAL_V_BAUDRATE_38400); pcInit += 5;
    } else if(memcmp(pcInit, "57600", 5) == 0) {                      // 57600 bauds
        uiBps = (ucFast ? LL_PHYSICAL_V_BAUDRATE_1200 : LL_PHYSICAL_V_BAUDRATE_57600); pcInit += 5;
    } else if(memcmp(pcInit, "115200", 6) == 0) {                     // 115200 bauds
        uiBps = (ucFast ? LL_PHYSICAL_V_BAUDRATE_1200 : LL_PHYSICAL_V_BAUDRATE_115200); pcInit += 6;
    } else
		goto lblKOConfigure;                                          // Configuration error

    iBlindDial = (*pcInit == 'D' ? 1: 0);                             // *** Blind dialing ***
         
    uiTimeout = DIAL_TIMEOUT;                                         // *** Dial timeout ***
 
	// Parsing phone parameters
	// ========================
    iRet=0;
    iLen=0;
    strcpy(tcTmp, pcPhone);
    pcPabx = "";
    pcPrefix = "";
    pcCountry = "";
    pcPhone = tcTmp;

	// Extract pabx if found
	// ---------------------
    while(tcTmp[iLen]) {
        if(tcTmp[iLen] != '-') {
            iLen++;
            continue;
        }
        pcPabx = &tcTmp[iRet];
        tcTmp[iLen++] = 0;
        iRet = iLen;
        pcPhone = &tcTmp[iLen];
        break;
    }

	// Extract prefix if found
	// -----------------------
    while(tcTmp[iLen]) {
        if(tcTmp[iLen] != '-') {
            iLen++;
            continue;
        }
        pcPrefix = &tcTmp[iRet];
        tcTmp[iLen++] = 0;
        iRet = iLen;
        pcPhone = &tcTmp[iLen];
        break;
    }

	// Extract country code if found
    // -----------------------------
    while(tcTmp[iLen]) {
        if(tcTmp[iLen] != '-') {
            iLen++;
            continue;
        }
        pcCountry = &tcTmp[iRet];
        tcTmp[iLen++] = 0;
        iRet = iLen;
        pcPhone = &tcTmp[iLen];
        break;
    }

    memset(tcBuf, 0, sizeof(tcBuf));
    strcpy(tcBuf, pcPabx);
    strcat(tcBuf, pcPrefix);
    if(*pcCountry) {
        strcat(tcBuf, ",");
        strcat(tcBuf, pcCountry);
        strcat(tcBuf, ",");
    }
    strcat(tcBuf, pcPhone);

    // Create parameters tree
    // ======================
	piConfig = TlvTree_New(LL_TAG_LINK_LAYER_CONFIG);                 // LinkLayer parameters Root tag of the configuration tree
	CHECK(piConfig!=NULL, lblKOConfigure);

    // Physical layer parameters (part1)
	// =================================
	piPhysicalConfig = TlvTree_AddChild(piConfig,                        
		                                LL_TAG_PHYSICAL_LAYER_CONFIG, // TAG Physical layer parameters 
									    NULL,                         // VALUE (Null)
									    0);                           // LENGTH 0
	CHECK(piPhysicalConfig!=NULL, lblKOConfigure);

    // Modem
	// -----
	TlvTree_AddChildInteger(piPhysicalConfig, 
		                    LL_PHYSICAL_T_LINK,                       // TAG
							LL_PHYSICAL_V_MODEM,                      // VALUE
							LL_PHYSICAL_L_LINK);                      // LENGTH 1 byte

    // Baud Rate
	// ---------
	TlvTree_AddChildInteger(piPhysicalConfig,
		                    LL_PHYSICAL_T_BAUDRATE,                   // TAG
	                        uiBps,                                    // VALUE (Integer)
                            LL_PHYSICAL_L_BAUDRATE);                  // LENGTH 4 bytes                                               

    // Data bits
	// ---------
	TlvTree_AddChildInteger(piPhysicalConfig, 
		                    LL_PHYSICAL_T_BITS_PER_BYTE,              // TAG
							ucDatasize,                               // VALUE
							LL_PHYSICAL_L_BITS_PER_BYTE);             // LENGTH 1 byte

    // Stop bits
	// ---------
	TlvTree_AddChildInteger(piPhysicalConfig, 
		                    LL_PHYSICAL_T_STOP_BITS,                  // TAG
							ucStopbits,                               // VALUE
							LL_PHYSICAL_L_STOP_BITS);                 // LENGTH 1 byte
    
    // Parity
	// ------
	TlvTree_AddChildInteger(piPhysicalConfig, 
		                    LL_PHYSICAL_T_PARITY,                     // TAG
							ucParity,                                 // VALUE
							LL_PHYSICAL_L_PARITY);                    // LENGTH 1 byte

    // Modem type
    // ----------
	TlvTree_AddChildInteger(piPhysicalConfig, 
		                    LL_MODEM_T_TYPE,                          // TAG
							LL_MODEM_V_TYPE_STANDARD,                 // VALUE
							LL_MODEM_L_TYPE);                         // LENGTH 1 byte
	
    // Dial Timeout
	// ------------
	TlvTree_AddChildInteger(piPhysicalConfig,
		                    LL_MODEM_T_DIAL_TIMEOUT,                  // TAG
	                        uiTimeout,                                // VALUE (Integer)
                            LL_MODEM_L_DIAL_TIMEOUT);                 // LENGTH 4 bytes                                               

    // Command Line Terminator
	// -----------------------
    if (xCom.chn == chnMdm) {    	
	    TlvTree_AddChildInteger(piPhysicalConfig, 
		                    LL_MODEM_T_CMD_TERMINATOR,                // TAG
							LL_MODEM_V_CMD_TERMINATOR_CR,             // VALUE
							LL_MODEM_L_CMD_TERMINATOR);               // LENGTH 1 byte
    }

    // Modem initialization string
	// ---------------------------
    if(eComType == chnHdlc) {                                         // Hdlc string
		Telium_Sprintf(tcInitString, "ATE0%sS6=1$M249$M251F4S144=16", iBlindDial ? "X1":"X4");
    } else                                                            // Modem string 
        Telium_Sprintf(tcInitString, "ATE0\\N%c%%C%c%s", ucCorrection ? '3':'0', ucCompression ? '1':'0', iBlindDial ? "X1":"X4");
    TlvTree_AddChildString(piPhysicalConfig,
		                   LL_MODEM_T_INIT_STRING,                    // TAG
						   tcInitString);                             // VALUE
	                                                                  // LENGTH (strlen initString)


	// Phone number
	// ------------
    TlvTree_AddChildString(piPhysicalConfig,
		                   LL_MODEM_T_PHONE_NUMBER,                   // TAG
						   tcBuf);                                    // VALUE
	                                                                  // LENGTH (strlen buf)
    
    if(eComType == chnHdlc) {

		// Data link layer parameters
		// ==========================
		piDataLinkConfig = TlvTree_AddChild(piConfig,                        
		                                    LL_TAG_DATA_LINK_LAYER_CONFIG, // TAG Data link layer parameters 
									        NULL,                     // VALUE (Null)
									        0);                       // LENGTH 0
		CHECK(piDataLinkConfig!=NULL, lblKOConfigure);
        

		// HDLC
		// ----
	    TlvTree_AddChildInteger(piDataLinkConfig, 
		                        LL_DATA_LINK_T_PROTOCOL,              // TAG
			    				LL_DATA_LINK_V_HDLC,                  // VALUE
				    			LL_DATA_LINK_L_PROTOCOL);             // LENGTH 1 byte


		// Connection timeout
		// ------------------
		uiTimeout = HDLC_TIMEOUT;
        TlvTree_AddChildInteger(piDataLinkConfig,         
                                LL_HDLC_T_CONNECT_TIMEOUT,            // TAG
	                            uiTimeout,                            // Value (Integer)
                                LL_HDLC_L_CONNECT_TIMEOUT);           // LENGTH 4 bytes
		
   
        // Minimum tries for sending
		// -------------------------
	    TlvTree_AddChildInteger(piDataLinkConfig, 
		                        LL_HDLC_T_MIN_RESEND_REQUESTS,        // TAG
			    				2,                                    // VALUE
				    			LL_HDLC_L_MIN_RESEND_REQUESTS);       // LENGTH 1 byte
    }
    
	// Link Layer configuration
	// ************************
    iRet = LL_Configure(&hSession, piConfig);                         // Initialize the handle of the session
	CHECK (iRet==LL_ERROR_OK, lblKOConfigure); 

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
//                   int ConnectModem (LL_HANDLE hSession)
//  This function dials a phone number through the modem port.
//   - LL_Connect() : Connect Link Layer
//  This function has parameters.
//    hSession (I-) : Handle of the session
//  This function has return value
//    >=0 : Connect done
//     <0 : Connect failed
//****************************************************************************

static int ConnectModem(LL_HANDLE hSession)
{
	// Local variables
    // ***************
    int iRet;

	// Link Layer connection
	// *********************
    iRet = LL_Connect(hSession);

	return(iRet);
}

//****************************************************************************
//       int SendModem (LL_HANDLE hSession, char *pcMsg, word usLen)
//  This function sends data through the modem port.      
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

static int SendModem(LL_HANDLE hSession, char *pcMsg, word usLen)
{
	// Local variables 
    // ***************
    word usIdx;
    int iRet;

    // Set parity (7bits Even/Odd only)
    // ********************************
    if (xCom.parity) 
    	SetParity(pcMsg, usLen, 1, xCom.parity);

	// Send data
	// *********
    iRet = LL_Send(hSession, usLen, pcMsg, LL_INFINITE);
    if (xCom.parity) 
    	for (usIdx=0; usIdx<usLen; usIdx++)
    		*(pcMsg+usIdx) = *(pcMsg+usIdx) & 0x7F;
    if(iRet != usLen)
		iRet = LL_GetLastError(hSession);
	    
    return iRet;
}

//****************************************************************************
//        int ReceiveModem (LL_HANDLE hSession, T_GL_HWIDGET hScreen,
//                           char *pcMsg, word usLen, byte ucDly)
//  This function receives data through the modem port.      
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

static int ReceiveModem(LL_HANDLE hSession, T_GL_HWIDGET hScreen, char *pcMsg, word usLen, byte ucDly)
{
	// Local variables 
    // ***************
    int iKey;
    long lSec, lTimeOut=LL_INFINITE;
    int iRet, iLength=0, iNbrBytes, iIdx;

	// Timeout setting
	// ***************
    if(ucDly != 0xFF)  
        lSec = ucDly * 1000;
	else
		lSec = LL_INFINITE;

	// Receiving first block
	// *********************
	ResetPeripherals(KEYBOARD | TSCREEN);                                // Reset peripherals FIFO
    iRet = TimerStart(0, lSec);                                          // Timer0 starts
    CHECK(iRet>=0, lblTimeOut);
	do {
    	iNbrBytes = LL_Receive(hSession, usLen, pcMsg+iLength, 1*100);   // Check reception
    	if (iNbrBytes != 0)
    		break;                                                       // Bytes received
		iKey = GoalGetKey(hScreen, hGoal, true, 0, false);               // Get key pressed/touched
		CHECK(iKey!=GL_KEY_CANCEL, lblTimeOut);                          // Exit on cancel key
		if (lSec != LL_INFINITE)
			lTimeOut = TimerGet(0);                                      // Retrieve timer value
    } while (lTimeOut>0);

    CHECK(lTimeOut!=0, lblTimeOut);                                      // Exit on timeout

	iLength += iNbrBytes;
	iRet = LL_GetLastError(hSession);
	CHECK (iRet==LL_ERROR_OK, lblEnd);
	if ((iNbrBytes != 0) && (iNbrBytes < usLen))
	{
		// Receiving next block until timeout (Inter block 500ms)
		// ======================================================
		while(1)
		{
			iNbrBytes = LL_Receive(hSession, usLen-iLength, pcMsg+iLength, 50);
			iLength += iNbrBytes;
			iRet = LL_GetLastError(hSession);
			CHECK ((iRet==LL_ERROR_OK) || (iRet==LL_ERROR_TIMEOUT), lblEnd);
			if ((iRet==LL_ERROR_TIMEOUT) || (iNbrBytes==0))
				break;
		}
	}

	// Check parity (7bits Even/Odd only)
	// **********************************
	if (xCom.parity)
    {
    	for (iIdx=0; iIdx<iLength; iIdx++)
    	{
    		iRet = (int) CheckParity(pcMsg+iIdx, 1, 1, xCom.parity);
    		CHECK(iRet==1, lblParityKO);
    		*(pcMsg+iIdx) = *(pcMsg+iIdx) & 0x7F;
    	}
    }

	iRet = iLength;
    goto lblEnd;

	// Errors treatment 
    // ****************	
lblParityKO:                                                             // Error modem parity
    iRet=LL_ERROR_PHYSICAL_PARITY;
    goto lblEnd;
lblTimeOut:                                                              // Timeout expired
	iRet=LL_ERROR_TIMEOUT;
	goto lblEnd;
lblEnd:
	TimerStop(0);
	return iRet;
}

//****************************************************************************
//                 int DisconnectModem (LL_HANDLE hSession)
//  This function disconnects the modem port.      
//   - LL_Disconnect() : Disconnect Link Layer
//  This function has no parameters.
//    hSession (I-) : Handle of the session
//  This function has return value
//    >=0 : Disconnect done
//     <0 : Disconnect failed                                       
//****************************************************************************

static int DisconnectModem(LL_HANDLE hSession)
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
//                 int CloseModem (LL_HANDLE hSession)
//  This function deletes the handle of the modem port.   
//   - LL_Configure() : Delete Link Layer configuration
//  This function has no parameters.  
//  This function has return value
//    >=0 : Configuration deleted
//     <0 : Processing failed                                        
//****************************************************************************

int CloseModem(LL_HANDLE hSession)
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
//                      void PromptModem (void)                            
//  This function asks for the modem's parameters.   
//  This function has no parameters.  
//  This function has no return value
//****************************************************************************

void PromptModem(void)
{
	// Local variables 
    // ***************
	char tcMask[256];
	char tcModemItem[lenModemItem];
	char tcModemInit[lenModemInit+1];
	char tcPabxN[lenModemPabx+1], tcPabxP[lenModemPabx+1];
	char tcPrefixN[lenModemPrefix+1], tcPrefixP[lenModemPrefix+1];
	char tcCountryN[lenModemCountry+1], tcCountryP[lenModemCountry+1];
	char tcPhoneN[lenModemPhone+1], tcPhoneP[lenModemPhone+1];
	char tcDisplay[50+1];
    int i, iDefItemP, iDefItemN, iRet;
    
    // Retrieve modem parameters
    // *************************
	iRet = appGet(appModemItem, tcModemItem, lenModemItem);
	CHECK(iRet>=0, lblDbaErr);
	iRet = appGet(appModemInit, tcModemInit, lenModemInit+1);
	CHECK(iRet>=0, lblDbaErr);

    // Select dial mode (Tone by default)
    // **********************************
    tcModemInit[0]='T'; 
    
    // Select stop bits (1 only supported)
    // ***********************************
    tcModemInit[3]='1';    
        
    // Select data bits
    // ****************
	iDefItemP = tcModemItem[0];                                                     // Retrieve previous item selected
	iDefItemN = GL_Dialog_Choice(hGoal, "Data Bits", tzDataBits, iDefItemP, GL_BUTTON_DEFAULT, GL_KEY_0, GL_TIME_MINUTE);
	CHECK((iDefItemN!=GL_KEY_CANCEL) && (iDefItemN!=GL_RESULT_INACTIVITY), lblEnd); // Exit on cancel/timeout
	if (iDefItemN != iDefItemP)
	{
		tcModemInit[1] = iDefItemN+'0'+7;                                           // Save data bits
		tcModemItem[0] = iDefItemN;                                                 // Save new item selected
		iRet = appPut(appModemItem, tcModemItem, lenModemItem);                     // Save modem parameters
		CHECK(iRet>=0, lblDbaErr);
		iRet = appPut(appModemInit, tcModemInit, lenModemInit);
		CHECK(iRet>=0, lblDbaErr);
	}

    // Select parity
    // *************
	iDefItemP = tcModemItem[1];                                                     // Retrieve previous item selected
	iDefItemN = GL_Dialog_Choice(hGoal, "Parity", tzParity, iDefItemP, GL_BUTTON_DEFAULT, GL_KEY_0, GL_TIME_MINUTE);
	CHECK((iDefItemN!=GL_KEY_CANCEL) && (iDefItemN!=GL_RESULT_INACTIVITY), lblEnd); // Exit on cancel/timeout
	if (iDefItemN != iDefItemP)
	{
		tcModemInit[2] = *tzParity[iDefItemN];                                      // Save parity
		tcModemItem[1] = iDefItemN;                                                 // Save new item selected
		iRet = appPut(appModemItem, tcModemItem, lenModemItem);                     // Save modem parameters
		CHECK(iRet>=0, lblDbaErr);
		iRet = appPut(appModemInit, tcModemInit, lenModemInit);
		CHECK(iRet>=0, lblDbaErr);
	}

    // Select baud rate
    // ****************
	iDefItemP = tcModemItem[2];                                                     // Retrieve previous item selected
	iDefItemN = GL_Dialog_Choice(hGoal, "Baud Rate", tzBaudRate, iDefItemP, GL_BUTTON_DEFAULT, GL_KEY_0, GL_TIME_MINUTE);
	CHECK((iDefItemN!=GL_KEY_CANCEL) && (iDefItemN!=GL_RESULT_INACTIVITY), lblEnd); // Exit on cancel/timeout
	if (iDefItemN != iDefItemP)
	{
		strcpy (&tcModemInit[4], tzBaudRate[iDefItemN]);                            // Save baud rate
		tcModemItem[2] = iDefItemN;                                                 // Save new item selected
		iRet = appPut(appModemItem, tcModemItem, lenModemItem);                     // Save modem parameters
		CHECK(iRet>=0, lblDbaErr);
		iRet = appPut(appModemItem, tcModemInit, lenModemInit);
		CHECK(iRet>=0, lblDbaErr);
	}

    // Enter pabx
    // **********
	memset(tcMask, 0, sizeof(tcMask));
	for (i=0; i<lenModemPabx; i++)
		strcat(tcMask, "/d");                                                       // Accept only decimal character
	iRet = appGet(appModemPabx, tcPabxP, lenModemPabx+1);
	CHECK(iRet>=0, lblDbaErr);
	strcpy(tcPabxN, tcPabxP);
	iRet = GL_Dialog_Text(hGoal, "Pabx", "Enter Pabx :", tcMask, tcPabxN, sizeof(tcPabxN), GL_TIME_MINUTE);
	CHECK((iRet!=GL_KEY_CANCEL) && (iRet!=GL_RESULT_INACTIVITY), lblEnd);           // Exit on cancel/timeout
	if (strcmp(tcPabxN, tcPabxP) != 0)
	{
		iRet = appPut(appModemPabx, tcPabxN, lenModemPabx);                         // Save new pabx
		CHECK(iRet>=0, lblDbaErr);
	}

	// Enter prefix
	// ************
	memset(tcMask, 0, sizeof(tcMask));
	for (i=0; i<lenModemPrefix; i++)
		strcat(tcMask, "/d");                                                       // Accept only decimal character
	iRet = appGet(appModemPrefix, tcPrefixP, lenModemPrefix+1);
	CHECK(iRet>=0, lblDbaErr);
	strcpy(tcPrefixN, tcPrefixP);
	iRet = GL_Dialog_Text(hGoal, "Prefix", "Enter Prefix :", tcMask, tcPrefixN, sizeof(tcPrefixN), GL_TIME_MINUTE);
	CHECK((iRet!=GL_KEY_CANCEL) && (iRet!=GL_RESULT_INACTIVITY), lblEnd);           // Exit on cancel/timeout
	if (strcmp(tcPrefixN, tcPrefixP) != 0)
	{
		iRet = appPut(appModemPrefix, tcPrefixN, lenModemPrefix);                   // Save new prefix
		CHECK(iRet>=0, lblDbaErr);
	}
		
	// Enter country code
	// ******************
	memset(tcMask, 0, sizeof(tcMask));
	for (i=0; i<lenModemCountry; i++)
		strcat(tcMask, "/d");                                                       // Accept only decimal character
	iRet = appGet(appModemCountry, tcCountryP, lenModemCountry+1);
	CHECK(iRet>=0, lblDbaErr);
	strcpy(tcCountryN, tcCountryP);
	iRet = GL_Dialog_Text(hGoal, "Country Code", "Enter Country Code :", tcMask, tcCountryN, sizeof(tcCountryN), GL_TIME_MINUTE);
	CHECK((iRet!=GL_KEY_CANCEL) && (iRet!=GL_RESULT_INACTIVITY), lblEnd);           // Exit on cancel/timeout
	if (strcmp(tcCountryN, tcCountryP) != 0)
	{
		iRet = appPut(appModemCountry, tcCountryN, lenModemCountry);                // Save new country code
		CHECK(iRet>=0, lblDbaErr);
	}
		
	// Enter phone number
	// ******************
	memset(tcMask, 0, sizeof(tcMask));
	for (i=0; i<lenModemPhone; i++)
		strcat(tcMask, "/d");                                                       // Accept only decimal character
	iRet = appGet(appModemPhone, tcPhoneP, lenModemPhone+1);
	CHECK(iRet>=0, lblDbaErr);
	strcpy(tcPhoneN, tcPhoneP);
	iRet = GL_Dialog_Text(hGoal, "Phone Number", "Enter Phone# :", tcMask, tcPhoneN, sizeof(tcPhoneN), GL_TIME_MINUTE);
	CHECK((iRet!=GL_KEY_CANCEL) && (iRet!=GL_RESULT_INACTIVITY), lblEnd);           // Exit on cancel/timeout
	if (strcmp(tcPhoneN, tcPhoneP) != 0)
	{
		iRet = appPut(appModemPhone, tcPhoneN, lenModemPhone);                      // Save new phone number
		CHECK(iRet>=0, lblDbaErr);
	}

	goto lblEnd;

	// Errors treatment 
	// ****************
lblDbaErr:                                                                          // Data base error
	Telium_Sprintf(tcDisplay, "%s\n%s", FMG_ErrorMsg(iRet), "Software Reset Needed");
	GL_Dialog_Message(hGoal, NULL, tcDisplay, GL_ICON_ERROR, GL_BUTTON_VALID, 5*1000);
	goto lblEnd;
lblEnd:
	return;
}

//****************************************************************************
//                       void ComModem (void)                            
//  This function communicates through the modem (Async) at 8-N-1-19200.   
//             Test done with Hyper-terminal.   
//  This function has no parameters.  
//  This function has no return value
//****************************************************************************

int ComModem(tBuffer * req,tBuffer * rsp, word SSL) {
	// Local variables 
    // ***************
	T_GL_HWIDGET hScreen=NULL;    // Screen handle
	T_GL_HWIDGET hDocument=NULL;  // Document handle
	ST_PRN_LINE xLine;
	LL_HANDLE hCOM=NULL;
	char tcModemInit[lenModemInit+1];
	char *pcStr, tcPhone[128+1];
    char tcSnd[MAX_SND+1];
    char tcRsp[MAX_RSP+1];
    char tcDisplay[50+1];
    byte p; // Printer line index
	int iRet=0, iStatus=0,RetVal = -1;

	// Transmission through internal Modem in progress
	// ***********************************************
    memset(tcSnd, 0, sizeof(tcSnd));
	memset(tcRsp, 0, sizeof(tcRsp));

	hScreen = GoalCreateScreen(hGoal, txModem, NUMBER_OF_LINES(txModem), GL_ENCODING_UTF8);
	CHECK(hScreen!=NULL, lblKO);                                          // Create screen and clear it
	iRet = GoalClrScreen(hScreen, GL_COLOR_BLACK, KEY_CANCEL, false);
	CHECK(iRet>=0, lblKO);

	// Open modem 
	// ==========
	iRet = GoalDspLine(hScreen, 0, "1-comOpen...", &txModem[0], 0, true);
	CHECK(iRet>=0, lblKO);
	iRet = appGet(appModemInit, tcModemInit, lenModemInit+1);             // Retrieve init string
	CHECK(iRet>=0, lblDbaErr);
    iRet = GoalDspLine(hScreen, 1, tcModemInit, &txModem[1], 0, true);    // Show init string
	CHECK(iRet>=0, lblKO);
	pcStr = tcPhone;                                                      // Build dial string
	iRet = appGet(appModemPabx, pcStr, lenModemPabx+1);                   // Retrieve pabx
	CHECK(iRet>=0, lblDbaErr);
	if (strlen(pcStr) != 0)
	{
		pcStr+=strlen(pcStr);
		*pcStr='-'; pcStr++;
	}
	iRet = appGet(appModemPrefix, pcStr, lenModemPrefix+1);               // Retrieve prefix
	CHECK(iRet>=0, lblDbaErr);
	if (strlen(pcStr) != 0)
	{
		pcStr+=strlen(pcStr);
		*pcStr='-'; pcStr++;
	}
	iRet = appGet(appModemCountry, pcStr, lenModemCountry+1);             // Retrieve country number
	CHECK(iRet>=0, lblDbaErr);
	if (strlen(pcStr) != 0)
	{
		pcStr+=strlen(pcStr);
		*pcStr='-'; pcStr++;
	}
	iRet = appGet(appModemPhone, pcStr, lenModemPhone+1);                 // Retrieve phone number
	CHECK(iRet>=0, lblDbaErr);
    iRet = GoalDspLine(hScreen, 2, tcPhone, &txModem[2], 0, true);        // Show dial string
 	CHECK(iRet>=0, lblKO);
    hCOM = OpenModem(tcModemInit, tcPhone, chnMdm);                       // ** Open **
    CHECK(hCOM!=NULL, lblKO);
    iRet = GoalDspLine(hScreen, 3, "1-comOpen OK", &txModem[3], 0, true);
	CHECK(iRet>=0, lblKO);

    // Dialing
	// =======
    iRet = GoalDspLine(hScreen, 4, "2-comDial...", &txModem[4], 0, true);
	CHECK(iRet>=0, lblKO);
    iRet = ConnectModem(hCOM);                                            // ** Connect **
    CHECK(iRet>=0, lblComKO);
	iRet = GoalClrScreen(hScreen, GL_COLOR_BLACK, KEY_CANCEL, false);     // Clear screen
	CHECK(iRet>=0, lblKO);
    iRet = GoalDspLine(hScreen, 0, "2-comDial OK", &txModem[0], 0, true);
 	CHECK(iRet>=0, lblKO);

	// Clear sending/receiving buffers
	// ===============================
	iRet = LL_ClearSendBuffer(hCOM);
    CHECK(iRet==LL_ERROR_OK, lblComKO);
    iRet = LL_ClearReceiveBuffer(hCOM);
    CHECK(iRet==LL_ERROR_OK, lblComKO);

    // Send data through modem
	// =======================
    iRet = GoalDspLine(hScreen, 1, "3-comSend...", &txModem[1], 0, true);
	CHECK(iRet>=0, lblKO);
    strcpy(tcSnd, "Hello, do you hear me?\n"                              // Data to send
		          "Hello, do you hear me?\n"
		    	  "Hello, do you hear me?\n"
				  "Hello, do you hear me?\n"
				  "Hello, do you hear me?\n");
	iRet = SendModem(hCOM, tcSnd, (word) strlen(tcSnd));                  // ** Send data **
	CHECK(iRet>=0, lblComKO);
	iRet = GoalDspLine(hScreen, 2, "3-comSend OK", &txModem[2], 0, true);
	CHECK(iRet>=0, lblKO);
    
	// Receive data through modem
	// ==========================
    iRet = GoalDspLine(hScreen, 3, "4-comRec...", &txModem[3], 0, true);
	CHECK(iRet>=0, lblKO);
    iRet = ReceiveModem(hCOM, hScreen, tcRsp, sizeof(tcRsp), 30);         // ** Receive data **
	CHECK(iRet>=0, lblComKO);
	RetVal = iRet;
    if (iRet > MAX_RSP) strcpy (tcRsp, "Buffer overflow Max=512");        // Data overflow
    iRet = GoalDspLine(hScreen, 4, "4-comRec OK", &txModem[4], 0, true);
	CHECK(iRet>=0, lblKO);

	// Disconnection
	// =============
    iRet = DisconnectModem(hCOM);                                         // ** Disconnect **
	CHECK(iRet>=0, lblComKO);
	iRet = GoalClrScreen(hScreen, GL_COLOR_BLACK, KEY_CANCEL, false);     // Clear screen
	CHECK(iRet>=0, lblKO);
    iRet = GoalDspLine(hScreen, 0, "5-comDisconn OK", &txModem[0], 0, true);
	CHECK(iRet>=0, lblKO);
    iRet = CloseModem(hCOM);                                              // ** Close **
	CHECK(iRet>=0, lblComKO);

	// Display transmission data
	// =========================
	iRet = GoalClrScreen(hScreen, GL_COLOR_BLACK, KEY_CANCEL | KEY_VALID, false); // Clear screen
	CHECK(iRet>=0, lblKO);
	iRet = GoalDspMultiLine(hScreen, 0, tcRsp, strlen(tcRsp), txModem, NUMBER_OF_LINES(txModem), 0, true);
	CHECK(iRet>=0, lblKO);                                                // Show data to receive

	// Print transmission data
	// =======================
	p=0;
	hDocument = GoalCreateDocument(hGoal, GL_ENCODING_UTF8);              // Create document
	CHECK(hDocument!=NULL, lblKO);

	xLine = xPrinter;                                                     // Build document (Demo, Tx/Rx data)
	xLine.eTextAlign = GL_ALIGN_CENTER;
	xLine.bReverse = TRUE;
	xLine.xMargin.usBottom = PIXEL_BOTTOM;
	xLine.xFont.eScale = GL_SCALE_XXLARGE;
	iRet = GoalPrnLine(hDocument, p++, "Modem Demo", &xLine);
	CHECK(iRet>=0, lblKO);

	xLine = xPrinter;
	iRet = GoalPrnLine(hDocument, p++, "Sending:", &xLine);               // Data to send
	CHECK(iRet>=0, lblKO);
    iRet = GoalPrnLine(hDocument, p++, tcSnd, &xLine);
	CHECK(iRet>=0, lblKO);
	iRet = GoalPrnLine(hDocument, p++, "\nReceiving:", &xLine);           // Data to receive
	CHECK(iRet>=0, lblKO);
    iRet = GoalPrnLine(hDocument, p++, tcRsp, &xLine);
	CHECK(iRet>=0, lblKO);
	iRet = GoalPrnLine(hDocument, p++, "\n\n\n\n", &xLine);
	CHECK(iRet>=0, lblKO);

	iRet = GoalPrnDocument(hDocument);                                    // Print document
	CHECK(iRet>=0, lblKO);

	GoalGetKey(hScreen, hGoal, true, 30*1000, true);                      // Wait for key pressed/touched (shortcut)

    goto lblEnd;

	// Errors treatment 
    // ****************
lblKO:                                                                    // None-classified low level error
	GL_Dialog_Message(hGoal, NULL, "Processing Error", GL_ICON_ERROR, GL_BUTTON_VALID, 5*1000);
	goto lblEnd;
lblComKO:                                                                 // Communication error
	strcpy(tcDisplay, LL_ErrorMsg(iRet));                                 // Link Layer error
	if (iRet == LL_Network_GetStatus(LL_PHYSICAL_V_MODEM, &iStatus))
	{
		switch(iStatus)
		{
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
		strcat(tcDisplay, LL_ErrorMsg(iStatus));                          // Link Layer status
	}
	if (iRet != LL_ERROR_TIMEOUT)                                         // Cancel or timeout ?
		GL_Dialog_Message(hGoal, NULL, tcDisplay, GL_ICON_ERROR, GL_BUTTON_VALID, 5*1000);
    goto lblEnd;
lblDbaErr:                                                                // Data base error
	Telium_Sprintf(tcDisplay, "%s\n%s", FMG_ErrorMsg(iRet), "Software Reset Needed");
	GL_Dialog_Message(hGoal, NULL, tcDisplay, GL_ICON_ERROR, GL_BUTTON_VALID, 5*1000);
	goto lblEnd;
lblEnd:
    if (hCOM) {
    	DisconnectModem(hCOM);                                            // ** Disconnect **
    	CloseModem(hCOM);                                                 // ** Close **
	}
	if (hScreen)
		GoalDestroyScreen(&hScreen);                                      // Destroy screen
    if (hDocument)
    	GoalDestroyDocument(&hDocument);                                  // Destroy document
    return RetVal;
}
