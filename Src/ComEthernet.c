//****************************************************************************
//       INGENICO                                INGEDEV 7                   
//============================================================================
//       FILE  COMETHERNET.C                      (Copyright INGENICO 2012)
//============================================================================
//  Created :       11-July-2012         Kassovic
//  Last modified : 06-December-2013     Kassovic
//  Module : TRAINING                                                          
//                                                                          
//  Purpose :                                                               
//                   *** Ethernet communication management ***
//             Transmission by Ethernet interface ETH
//  How to use the linklayer to create an Ethernet configuration to communicate.
//             Test done with Hyper-terminal Tcp/Ip Winsock
//  !!! Link Layer application must be loaded 3628xxxx.SGN inside the terminal
//      Supports PPP, TCP/IP and SSL protocols.
//      TlvTree_TPlus.lib + LinkLayerExeInterface_TPlus.lib must be implemented to link properly.
//                                                                            
//  List of routines in file :  
//      OpenEthernet : Create the Ethernet configuration.
//      ConnectEthernet : Connect the Ethernet layer.
//      SendEthernet : Send data through the Ethernet layer.
//      ReceiveEthernet : Receive data through the Ethernet layer.
//      DisconnectEthernet : Disconnect the Ethernet layer.
//      CloseEthernet : Delete the Ethernet configuration.
//      PromptEthernet : Prompt for Ethernet's parameters.
//      ComEthernet : Ethernet communication demo.
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
#include "SSL_.h"

//****************************************************************************
//      EXTERN                                                              
//****************************************************************************
extern T_GL_HGRAPHIC_LIB hGoal; // Handle of the graphics object library

//****************************************************************************
//      PRIVATE CONSTANTS                                                   
//****************************************************************************
#define MAX_SND  2048
#define MAX_RSP  2048

#define TCPIP_TIMEOUT 40*100

//****************************************************************************
//      PRIVATE TYPES                                                       
//****************************************************************************
// Empty

//****************************************************************************
//      PRIVATE DATA                                                        
//****************************************************************************
// Properties of the Ethernet screen (Goal)
// ========================================
static const ST_DSP_LINE txEthernet[] =
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


typedef struct sComChn {
	byte chn;                   // communication channel used
	byte chnRole;
	struct {
		LL_HANDLE hdl;
		char separator;
		unsigned long int conn_delay;   // 4 Second delay
		unsigned long int ll_conn_delay;
		unsigned int send_timeout;
		int ifconn;
		SSL_HANDLE hdlSsl;
		SSL_PROFILE_HANDLE hdlProfile;
		char SslProfName[PROFILE_NAME_SIZE + 1];
	} prm;
	TLV_TREE_NODE hCfg;
	TLV_TREE_NODE hPhyCfg;
	TLV_TREE_NODE hDLinkCfg;
	TLV_TREE_NODE hTransCfg;
} tComChn;
static tComChn comEth;

const char *parseStr_Eth(char *dst, const char *src, int dim, char separator) {
	VERIFY(dst);
	VERIFY(src);

	while(*src) {
		if(*src == separator) {
			src++;              // skip separator
			break;
		}
		VERIFY(dim > 1);        // dim includes ending zero
		*dst++ = *src++;
		dim--;
	}

	*dst = 0;
	return src;
}

int comCheckSslProfile(void) {
	int ret = 0;
	char ProfNum[2 + 1];
	const char *init = "|/HOST/CLIENT_KEY.PEM|/HOST/CLIENT.CRT|/HOST/SERVER.CRT|/HOST/SERVER_INT.CRT";
	char keyFile[64 + 1];
	char crtFile[64 + 1];
	char caFile[64 + 1];

	memset(keyFile, 0, sizeof(keyFile));
	memset(crtFile, 0, sizeof(crtFile));
	memset(caFile, 0, sizeof(caFile));

	VERIFY(init);

	comEth.prm.separator = '|';    //common for all types of chn
	init = parseStr_Eth(ProfNum, init, sizeof(ProfNum), comEth.prm.separator);
	init = parseStr_Eth(keyFile, init, sizeof(keyFile), comEth.prm.separator);
	init = parseStr_Eth(crtFile, init, sizeof(crtFile), comEth.prm.separator);
	init = parseStr_Eth(caFile, init, sizeof(caFile), comEth.prm.separator);

	comEth.prm.hdlSsl = 0;
	comEth.prm.hdlProfile = 0;
	ret = ssllib_open();

	if(*ProfNum) {
		Telium_Sprintf(comEth.prm.SslProfName, "BSEAPPTLS4%s", ProfNum);
	} else {                    //default
		memcpy(comEth.prm.SslProfName, SSL_PROFILE_NAME, strlen(SSL_PROFILE_NAME));
	}

	comEth.prm.hdlProfile = SSL_NewProfile(comEth.prm.SslProfName, &ret);
	if(comEth.prm.hdlProfile == NULL) {
		SSL_UnloadProfile(comEth.prm.SslProfName);
		SSL_DeleteProfile(comEth.prm.SslProfName);
		comEth.prm.hdlProfile = SSL_NewProfile(comEth.prm.SslProfName, &ret);
	}
	CHECK(comEth.prm.hdlProfile != NULL, lblLoad);

	ret = SSL_ProfileSetProtocol(comEth.prm.hdlProfile, TLSv1_2);
	//	ret = SSL_ProfileSetProtocol(comEth.prm.hdlProfile, TLSv1);
	CHECK(ret == SSL_PROFILE_EOK, lblKO);

	ret = SSL_ProfileSetCipher(comEth.prm.hdlProfile, SSL_RSA | SSL_DSS| SSL_DES| SSL_3DES | SSL_RC4 | SSL_RC2 |SSL_MD5 | SSL_SHA1 | SSL_AES | SSL_SHA256| SSL_SHA384| SSL_aECDSA | SSL_kEDH | SSL_kECDHE | SSL_RC2 , SSL_HIGH | SSL_NOT_EXP );
	//	ret = SSL_ProfileSetCipher(comEth.prm.hdlProfile, SSL_RSA | SSL_DSS| SSL_DES| SSL_3DES | SSL_RC4 | SSL_RC2 | SSL_AES | SSL_SHA256| SSL_SHA384| SSL_aECDSA | SSL_kEDH | SSL_kECDHE | SSL_RC2 , SSL_HIGH | SSL_NOT_EXP );
	CHECK(ret == SSL_PROFILE_EOK, lblKO);

	/* The created profile will use the following certificates in the terminal */
	ret = SSL_ProfileSetKeyFile(comEth.prm.hdlProfile, keyFile, FALSE);
	ret = SSL_ProfileSetCertificateFile(comEth.prm.hdlProfile, crtFile);

	ret = SSL_ProfileAddCertificateCA(comEth.prm.hdlProfile, caFile);
	CHECK(ret == SSL_PROFILE_EOK, lblKO);

	//Add other CA certificates if any
	while(*init) {
		memset(caFile, 0, sizeof(caFile));
		init = parseStr_Eth(caFile, init, sizeof(caFile), comEth.prm.separator);
		if(*caFile) {
			ret = SSL_ProfileAddCertificateCA(comEth.prm.hdlProfile, caFile);
			CHECK(ret == SSL_PROFILE_EOK, lblKO);
		}
	}

	/* Save the profile.*/
	ret = SSL_SaveProfile(comEth.prm.hdlProfile);
	CHECK(ret == SSL_PROFILE_EOK, lblKO);

	ret = SSL_ProfileTestFile(comEth.prm.hdlProfile);

	lblLoad:
	//	comEth.prm.hdlProfile = SSL_LoadProfile(comEth.prm.SslProfName);
	//	CHECK(comEth.prm.hdlProfile != NULL, lblKO);

	ret = 1;
	goto lblEnd;
	lblKO:
	ret = -1;

	lblEnd:
	return ret;
}


//****************************************************************************
//      LL_HANDLE OpenEthernet (const char *pcInit, const char *pcServer)
//  This function configures the Ethernet layer.
//   - LL_Configure() : Create Link Layer configuration
//  This function has parameters.  
//    pcInit (I-) : static IP address unused, DHCP done via Manager menu.
//           LocalAddress = xxx.xxx.xxx.xxx
//           NetMask      = xxx.xxx.xxx.xxx
//           Gateway      = xxx.xxx.xxx.xxx
//           The '|' is the separator
//           Ex: "192.168.1.2|255.255.255.0|192.168.1.1
//    pcServer (I-) : Tcp/Ip address and port number
//           IpAddress = xxx.xxx.xxx.xxx or url
//           PortNumber = a string (max 5 bytes)
//           The '|' is the separator
//           Ex: "192.168.1.3|2000
//  This function has return value
//    !NULL : Pointer to the handle of the session
//     NULL : Session failed
//****************************************************************************
static LL_HANDLE OpenEthernet(const char *pcInit, const char *pcServer,word tlsSSL){
	// Local variables 
	// ***************
	char tcLocalAddr[15+1];    // Ip address xxx.xxx.xxx.xxx
	char tcNetmask[15+1];      // Mask address xxx.xxx.xxx.xxx
	char tcGateway[15+1];      // Gateway address xxx.xxx.xxx.xxx
	//    doubleword uiLocalAddr=0;
	//    doubleword uiNetmask=0;
	//    doubleword uiGateway=0;
	TLV_TREE_NODE piConfig=NULL;
	TLV_TREE_NODE piPhysicalConfig=NULL;
	TLV_TREE_NODE piTransportConfig=NULL;
	char tcAddr[lenEthIpLocal+1];
	char tcPort[lenEthPort+1];
	doubleword uiTimeout, uiRemotePort;
	LL_HANDLE hSession = NULL;                                          // Session handle
	int iRet;

	// Configure Static Address (DHCP used instead)
	// ********************************************
	CHECK((pcInit!=NULL) && (pcServer!=NULL), lblKOConfigure);          // Configuration error

	memset(tcLocalAddr, 0, sizeof(tcLocalAddr));                        // Ip address
	pcInit = parseStr('|', tcLocalAddr, pcInit, sizeof(tcLocalAddr));
	CHECK(pcInit!=NULL, lblKOConfigure);

	memset(tcNetmask, 0, sizeof(tcNetmask));                            // Netmask address
	pcInit = parseStr('|', tcNetmask, pcInit, sizeof(tcNetmask));
	CHECK(pcInit!=NULL, lblKOConfigure);

	memset(tcGateway, 0, sizeof(tcGateway));                            // Gateway address
	pcInit = parseStr('|', tcGateway, pcInit, sizeof(tcGateway));
	CHECK(pcInit!=NULL, lblKOConfigure);

	/*  // Extra functions from TCP/IP
    if (tcLocalAddr[0]) {
        uiLocalAddr = __inet_addr(tcLocalAddr);                         // Force local address
        EthernetSetOption(ETH_IFO_ADDR, &uiLocalAddr);
    }

    if (tcNetmask[0]) {
        uiNetmask = __inet_addr(tcNetmask);                             // Force mask
        EthernetSetOption(ETH_IFO_NETMASK, &uiNetmask);
    }
    if(tcGateway[0]) {
        uiGateway = __inet_addr(tcGateway);                             // Force gateway
        EthernetSetOption(ETH_IFO_GATEWAY, &uiGateway);
    }
	 */

	// Create the LinkLayer configuration parameters tree 
	// **************************************************

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

	// Ethernet
	// --------
	TlvTree_AddChildInteger(piPhysicalConfig,
			LL_PHYSICAL_T_LINK,                         // TAG
			LL_PHYSICAL_V_ETHERNET,                     // VALUE
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
	CHECK(pcServer!=NULL, lblKOConfigure);                              // End string

	// TCP/IP
	// ------
	TlvTree_AddChildInteger(piTransportConfig, 
			LL_TRANSPORT_T_PROTOCOL,                    // TAG
			LL_TRANSPORT_V_TCPIP,                       // VALUE
			LL_TRANSPORT_L_PROTOCOL);                   // LENGTH 1 byte

	////-----------------  SSL  ------------------

	uiTimeout = TCPIP_TIMEOUT;
	///Get the communication cipher

	switch (tlsSSL) {
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

	////------------------------------------------

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
	uiTimeout = TCPIP_TIMEOUT;                                        
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
//                int ConnectEthernet(LL_HANDLE hSession)
//  This function connects the Ethernet layer.
//   - LL_Connect() : Connect Link Layer
//  This function has no parameters.
//    hSession (I-) : Handle of the session
//  This function has return value
//    >=0 : Connect done
//     <0 : Connect failed
//****************************************************************************
static int ConnectEthernet(LL_HANDLE hSession){
	// Local variables
	// ***************
	int iRet;

	// Link Layer connection
	// *********************
	iRet = LL_Connect(hSession);

	return iRet;
}

//****************************************************************************
//    int SendEthernet (LL_HANDLE hSession, const char *pcMsg, word usLen)
//  This function sends data through the Ethernet layer.
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
static int SendEthernet(LL_HANDLE hSession, const byte *pcMsg, word usLen){
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
//        int ReceiveEthernet (LL_HANDLE hSession, T_GL_HWIDGET hScreen,
//                             char *pcMsg, word usLen, byte ucDly)
//  This function receives data through the Ethernet layer.
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
static int ReceiveEthernet(LL_HANDLE hSession, T_GL_HWIDGET hScreen, byte *pcMsg, word usLen, byte ucDly){
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
	//	ResetPeripherals(KEYBOARD | TSCREEN);                                  // Reset peripherals FIFO
	iRet = TimerStart(0, lSec);                                            // Timer0 starts
	CHECK(iRet>=0, lblTimeOut);

	do{
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
	if ((iNbrBytes != 0) && (iNbrBytes < usLen))	{
		// Receiving next block until timeout (Inter block 500ms)
		// ======================================================
		while(1){
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
//                int DisconnectEthernet (LL_HANDLE hSession)
//  This function disconnects the Ethernet layer.
//   - LL_Disconnect() : Disconnect Link Layer
//  This function has no parameters.  
//    hSession (I-) : Handle of the session
//  This function has return value
//    >=0 : Disconnect done
//     <0 : Disconnect failed                                       
//****************************************************************************
static int DisconnectEthernet(LL_HANDLE hSession){
	// Local variables 
	// ***************
	int iRet;

	// Link Layer disconnection
	// ************************
	iRet = LL_Disconnect(hSession);

	return iRet;
}

//****************************************************************************
//                 int CloseEthernet (LL_HANDLE hSession)
//  This function deletes the handle of the Ethernet layer.
//   - LL_Configure() : Delete Link Layer configuration
//  This function has no parameters.
//    hSession (I-) : Handle of the session
//  This function has return value
//    >=0 : Configuration deleted
//     <0 : Processing failed                                        
//****************************************************************************
static int CloseEthernet(LL_HANDLE hSession){
	// Local variables 
	// ***************
	int iRet;

	// Delete the configuration
	// ************************
	iRet = LL_Configure(&hSession, NULL);

	return iRet;
}


static int comLogicFormat_Ip(char * srvrIP) {
	int ret = 0, lenIP = 0, var = 0, Position = 0;
	char ip_Part_A[3+1];
	char ip_Part_B[3+1];
	char ip_Part_C[3+1];
	char ip_Part_D[3+1];
	char ConstructedIP[15+1];

	char * array = "255.255.255.255 ";

	//------ Format the IP address
	memset(ip_Part_A, 0x00, sizeof(ip_Part_A));
	memset(ip_Part_B, 0x00, sizeof(ip_Part_B));
	memset(ip_Part_C, 0x00, sizeof(ip_Part_C));
	memset(ip_Part_D, 0x00, sizeof(ip_Part_D));
	memset(ConstructedIP, 0x00, sizeof(ConstructedIP));

	//check if format of IP has "." in it
	ret = count_chars(srvrIP,'.');
	if (ret < 3) {
		lenIP = strlen(srvrIP);
		for (var = 0; var < lenIP; var++) {
			if ((var == 3) || (var == 6) || (var == 9)) {
				ConstructedIP[Position] = '.';
				Position++;
			}
			ConstructedIP[Position] = srvrIP[var];
			Position++;
		}
		strcpy(srvrIP, ConstructedIP);
		memset(ConstructedIP, 0x00, sizeof(ConstructedIP));
	}

	array = srvrIP; //copy to buffer

	// TREAT EACH BIT WELL
	//A
	ret = fmtTok(ip_Part_A, array, "."); //extract  token
	array += ret;                        //skip token extracted
	array++;                             //skip separator

	lenIP = strlen(ip_Part_A);
	if (lenIP == 2) {
		strcat(ConstructedIP, "0");
	}else if(lenIP == 1){
		strcat(ConstructedIP, "00");
	}
	strcat(ConstructedIP, ip_Part_A);
	strcat(ConstructedIP, ".");
	//========================================================
	//B
	ret = fmtTok(ip_Part_B, array, "."); //extract  token
	array += ret;                        //skip token extracted
	array++;                             //skip separator

	lenIP = strlen(ip_Part_B);
	if (lenIP == 2) {
		strcat(ConstructedIP, "0");
	}else if(lenIP == 1){
		strcat(ConstructedIP, "00");
	}
	strcat(ConstructedIP, ip_Part_B);
	strcat(ConstructedIP, ".");
	//========================================================
	//A
	ret = fmtTok(ip_Part_C, array, "."); //extract  token
	array += ret;                        //skip token extracted
	array++;                             //skip separator

	lenIP = strlen(ip_Part_C);
	if (lenIP == 2) {
		strcat(ConstructedIP, "0");
	}else if(lenIP == 1){
		strcat(ConstructedIP, "00");
	}
	strcat(ConstructedIP, ip_Part_C);
	strcat(ConstructedIP, ".");
	//========================================================
	//A
	ret = fmtTok(ip_Part_D, array, "."); //extract  token
	array += ret;                        //skip token extracted
	array++;                             //skip separator

	lenIP = strlen(ip_Part_D);
	if (lenIP == 2) {
		strcat(ConstructedIP, "0");
	}else if(lenIP == 1){
		strcat(ConstructedIP, "00");
	}
	strcat(ConstructedIP, ip_Part_D);
	//========================================================

	strcpy(srvrIP, ConstructedIP);

	return ret;
}

//****************************************************************************
//                    void PromptEthernet (void)                            
//  This function asks for the Ethernet's parameters.   
//  This function has no parameters.  
//  This function has no return value
//****************************************************************************

void PromptEthernet(void) {
	// Local variables 
	// ***************
	char tcMask[256];
	char tcIpAddressN[100+1], tcIpAddressP[100+1];
	char tcPortN[100+1], tcPortP[100+1];
	//	doubleword uiIp;
	char tcDisplay[50+1];
	int i, iRet;

	memset(tcPortN, 0, sizeof(tcPortN));
	memset(tcPortP, 0, sizeof(tcPortP));
	memset(tcIpAddressN, 0, sizeof(tcIpAddressN));
	memset(tcIpAddressP, 0, sizeof(tcIpAddressP));

	// Enter Local IP address
	// **********************
	iRet = appGet(appEthIpLocal, tcIpAddressP, lenEthIpLocal+1);
	CHECK(iRet>=0, lblDbaErr);

	strcpy(tcIpAddressN, tcIpAddressP);                                 // Copy address
	iRet = GL_Dialog_Text(hGoal, "Host Ip Address", "Enter Ip Address :","/d/d/d./d/d/d./d/d/d./d/d/d", tcIpAddressN, sizeof(tcIpAddressN), GL_TIME_MINUTE);
	CHECK((iRet!=GL_KEY_CANCEL) && (iRet!=GL_RESULT_INACTIVITY), lblEnd); // Exit on cancel/timeout
	comLogicFormat_Ip(tcIpAddressN);

	if (strncmp(tcIpAddressN, tcIpAddressP, 15) != 0) {
		iRet = appPut(appEthIpLocal, tcIpAddressN, lenEthIpLocal);        // Save new Ip address
		CHECK(iRet>=0, lblDbaErr);

		iRet = appPut(appGprsIpRemote, tcIpAddressN, lenGprsIpRemote);    // Save new Ip address
		CHECK(iRet>=0, lblDbaErr);
	}


	// Enter Port number 
	// *****************
	memset(tcMask, 0, sizeof(tcMask));
	for (i=0; i<lenEthPort; i++)
		strcat(tcMask, "/d");                                             // Accept only decimal character
	iRet = appGet(appEthPort, tcPortP, lenEthPort+1);
	CHECK(iRet>=0, lblDbaErr);
	strcpy(tcPortN, tcPortP);
	iRet = GL_Dialog_Text(hGoal, "Port Number", "Enter Port# :", tcMask, tcPortN, sizeof(tcPortN), GL_TIME_MINUTE);
	CHECK((iRet!=GL_KEY_CANCEL) && (iRet!=GL_RESULT_INACTIVITY), lblEnd); // Exit on cancel/timeout
	if (strcmp(tcPortN, tcPortP) != 0) {
		iRet = appPut(appEthPort, tcPortN, lenEthPort);                   // Save new Port number
		CHECK(iRet>=0, lblDbaErr);

		iRet = appPut(appGprsPort, tcPortN, lenGprsPort);                 // Save new Port number
		CHECK(iRet>=0, lblDbaErr);
	}

	GL_Dialog_Message(hGoal, NULL, "Done.", GL_ICON_NONE, GL_BUTTON_ALL, 2*1000);
	goto lblEnd;

	// Errors treatment 
	// ****************

	lblDbaErr:                                                                // Data base error
	Telium_Sprintf(tcDisplay, "%s\n%s", FMG_ErrorMsg(iRet), "Software Reset Needed");
	GL_Dialog_Message(hGoal, NULL, tcDisplay, GL_ICON_ERROR, GL_BUTTON_VALID, 2*1000);
	goto lblEnd;
	lblEnd:
	return;
}



//****************************************************************************
//                      void ComEthernet (void)                            
//  This function communicates through the Ethernet layer.
//  This function has no parameters.
//  This function has no return value
//****************************************************************************

int ComEthernet(tBuffer * req,tBuffer * rsp, word SSL) {
	// Local variables 
	// ***************
	T_GL_HWIDGET hScreen=NULL;    // Screen handle
	LL_HANDLE hETH=NULL;
	char *pcStr, tcStr[128+1];
	char tcIpAddress[lenEthIpLocal+1];
	char tcPort[lenEthPort+1];
	char tcDisplay[50+1];
	int iRet=0, iStatus=0,RetVal = -1;
	byte RespBuffer[4096];

	// Transmission through Ethernet layer in progress
	// ************************************************
	memset(tcStr, 0, sizeof(tcStr));
	memset(tcPort, 0, sizeof(tcPort));
	memset(RespBuffer, 0, sizeof(RespBuffer));
	memset(tcIpAddress, 0, sizeof(tcIpAddress));

	hScreen = GoalCreateScreen(hGoal, txEthernet, NUMBER_OF_LINES(txEthernet), GL_ENCODING_UTF8);
	CHECK(hScreen!=NULL, lblKO);                                          // Create screen and clear it
	iRet = GoalClrScreen(hScreen, GL_COLOR_BLACK, KEY_CANCEL, false);
	CHECK(iRet>=0, lblKO);

	// Open Ethernet layer
	// ===================
	pcStr = "      Connecting...";
	iRet = GoalDspLine(hScreen, 2, pcStr, &txEthernet[1], 0, true);
	CHECK(iRet>=0, lblKO);

	iRet = appGet(appEthIpLocal, tcIpAddress, lenEthIpLocal+1);           // Retrieve local IP
	CHECK(iRet>=0, lblDbaErr);

	iRet = appGet(appEthPort, tcPort, lenEthPort+1);                      // Retrieve port number
	CHECK(iRet>=0, lblDbaErr);

	Telium_Sprintf (tcStr, "%s|%s", tcIpAddress, tcPort);

	pcStr = "DHCP";
	hETH = OpenEthernet(pcStr, tcStr, SSL);                                    // ** Open **
	CHECK(hETH!=NULL, lblComKO);

	// Connect Ethernet layer
	// ======================
	iRet = GoalClrScreen(hScreen, GL_COLOR_BLACK, KEY_CANCEL, false);
	CHECK(iRet>=0, lblKO);

	iRet = ConnectEthernet(hETH);                                         // ** Connect **
	CHECK(iRet>=0, lblComKO);

	iRet = GoalDspLine(hScreen, 2, "      Sending...", &txEthernet[1], 0, true);
	CHECK(iRet>=0, lblKO);

	// Clear sending/receiving buffers
	// ===============================
	iRet = LL_ClearSendBuffer(hETH);
	CHECK(iRet==LL_ERROR_OK, lblEnd);

	iRet = LL_ClearReceiveBuffer(hETH);
	CHECK(iRet==LL_ERROR_OK, lblEnd);

	// Send data through Ethernet layer
	// ================================
	iRet = SendEthernet(hETH, bufPtr(req), bufLen(req));               // ** Send data **
	CHECK(iRet>=0, lblComKO);

	// Receive data through Ethernet layer
	// ===================================
	iRet = GoalDspLine(hScreen, 2, "      Receiving...", &txEthernet[4], 0, true);
	CHECK(iRet>=0, lblKO);

	iRet = ReceiveEthernet(hETH, hScreen, RespBuffer, bufDim(rsp), 30);      // ** Receive data **
	CHECK(iRet>=0, lblComKO);
	bufApp(rsp, RespBuffer, iRet);
	RetVal = iRet;

	iRet = GoalClrScreen(hScreen, GL_COLOR_BLACK, KEY_CANCEL, false);
	CHECK(iRet>=0, lblKO);

	// Disconnection
	// =============
	iRet = DisconnectEthernet(hETH);                                      // ** Disconnect **
	CHECK(iRet>=0, lblComKO);

	iRet = CloseEthernet(hETH);                                           // ** Close **
	CHECK(iRet>=0, lblComKO);

	goto lblEnd;

	// Errors treatment 
	// ****************
	lblKO:                                                                    // None-classified low level error
	GL_Dialog_Message(hGoal, NULL, "Processing Error", GL_ICON_ERROR, GL_BUTTON_VALID, 5*1000);
	goto lblEnd;


	lblComKO:                                                                 // Communication error
	strcpy(tcDisplay, LL_ErrorMsg(iRet));                                 // Link Layer error
	if (iRet == LL_Network_GetStatus(LL_PHYSICAL_V_ETHERNET, &iStatus))	{

		switch(iStatus){
		// Ethernet status
		case LL_STATUS_ETHERNET_NO_DEFAULT_ROUTE:   iStatus=LL_STATUS_ETH_NO_DEFAULT_ROUTE;        break;
		case LL_STATUS_ETHERNET_NOT_PLUGGED:        iStatus=LL_STATUS_ETH_NOT_PLUGGED;             break;
		case LL_STATUS_ETHERNET_BASE_NOT_READY:     iStatus=LL_STATUS_ETH_BASE_NOT_READY;          break;
		case LL_STATUS_ETHERNET_OUT_OF_BASE:        iStatus=LL_STATUS_ETH_OUT_OF_BASE;             break;
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
	if (hETH) {
		DisconnectEthernet(hETH);                                         // ** Disconnect **
		CloseEthernet(hETH);                                              // ** Close **
	}
	if (hScreen)
		GoalDestroyScreen(&hScreen);                                      // Destroy screen

	return RetVal;
}



//****************************************************************************
//                      void ComEthernet (void)
//  This function communicates through the Ethernet layer.
//  This function has no parameters.
//  This function has no return value
//****************************************************************************
int ComEthernetCheck(int SSL) {
	// Local variables
	// ***************
	LL_HANDLE hETH=NULL;
	char *pcStr, tcStr[128+1];
	char tcIpAddress[lenEthIpLocal+1];
	char tcPort[lenEthPort+1];
	int iRet=0, RetVal = -1;

	// Transmission through Ethernet layer in progress
	// ************************************************
	memset(tcStr, 0, sizeof(tcStr));
	memset(tcPort, 0, sizeof(tcPort));
	memset(tcIpAddress, 0, sizeof(tcIpAddress));


	iRet = appGet(appEthIpLocal, tcIpAddress, lenEthIpLocal+1);           // Retrieve local IP
	CHECK(iRet>=0, lblComKO);

	iRet = appGet(appEthPort, tcPort, lenEthPort+1);                      // Retrieve port number
	CHECK(iRet>=0, lblComKO);

	Telium_Sprintf (tcStr, "%s|%s", tcIpAddress, tcPort);

	pcStr = "DHCP";
	hETH = OpenEthernet(pcStr, tcStr, SSL);                                    // ** Open **
	CHECK(hETH!=NULL, lblComKO);

	iRet = ConnectEthernet(hETH);                                         // ** Connect **
	CHECK(iRet>=0, lblComKO);

	// Clear sending/receiving buffers
	// ===============================
	iRet = LL_ClearSendBuffer(hETH);
	CHECK(iRet==LL_ERROR_OK, lblEnd);

	iRet = LL_ClearReceiveBuffer(hETH);
	CHECK(iRet==LL_ERROR_OK, lblEnd);

	// Disconnection
	// =============
	iRet = DisconnectEthernet(hETH);                                      // ** Disconnect **
	CHECK(iRet>=0, lblComKO);

	iRet = CloseEthernet(hETH);                                           // ** Close **
	CHECK(iRet>=0, lblComKO);

	RetVal = 1;
	goto lblEnd;

	// Errors Encountered
	// ****************
	lblComKO:                                                                 // Communication error

	lblEnd:
	if (hETH) {
		DisconnectEthernet(hETH);                                         // ** Disconnect **
		CloseEthernet(hETH);                                              // ** Close **
	}

	return RetVal;
}



