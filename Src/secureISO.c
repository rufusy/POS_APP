/*
 * secureISO.c
 *
 *  Created on: Jun 21, 2016
 *      Author: kevshake
 */

#include "sdk.h" // IngeDev automatic refactoring - 2017/02/07 09:32:51 - Replace '#include "sdk30.h"' to '#include "sdk.h"'
#include <string.h>
#include "pinpad.h"
#include "pp30_def.h"
#include "SEC_interface.h"
#include "tlvVar_def.h"
#include <globals.h>


extern T_GL_HGRAPHIC_LIB hGoal; // Handle of the graphics object library

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

static int secureISO_GenerateRandom_BD0(char *RandomNumber){
	char buffer[10], byteVal[3];
	int var = 0;
	word i = 0;

	memset(buffer,0,sizeof(buffer));

	for (var = 0; var < 4; var++) {
		memset(byteVal,0,sizeof(byteVal));
		i = rand()%255;

		num2hex(byteVal,i,2);

		strcat(buffer,byteVal);
	}

	strcpy(RandomNumber,buffer);

	return strlen(buffer);
}

static int secureISO_GetFullDateTime_BD2(char *Date_time){
	int ret = 0;
	char datetime[lenDatTim + 5];
	int hs = 0;

	memset(datetime,0,sizeof(datetime));

	// making the first YY of the date statically
	strcpy(datetime,"20");

	//	getting the date and time from the memory of the terminal in format YYMMDDhhmmss
	ret = getDateTime(datetime + 2);    //CC+YYMMDDhhmmss

	//generate the hs from random number
	hs = rand()%60;

	num2dec(datetime + 14,hs,2);
	strcpy(Date_time,datetime);

	ret = strlen(datetime);

	return ret;
}

static int secureISO_Generate_All_BasicData(char * BD0_Out, char * BD1_Out, char * BD2_Out,char * Header){
	int ret = 0,i = 0;
	char BD0Data[128];
	char BD1Data[128];
	char BD2Data[128];
	char BD2DataA[10];
	char BD2DataB[10];
	char HeaderData[32 +1];
	byte BD0[16 + 1];
	byte BD1a[16 + 1];
	byte BD1[16 + 1];
	byte BD2[16 + 1];

	//clean data locations
	memset(BD0,0,sizeof(BD0));
	memset(BD1,0,sizeof(BD1));
	memset(BD2,0,sizeof(BD2));
	memset(BD1a,0,sizeof(BD1a));
	memset(BD0Data,0,sizeof(BD0Data));
	memset(BD1Data,0,sizeof(BD1Data));
	memset(BD2Data,0,sizeof(BD2Data));
	memset(BD2DataA,0,sizeof(BD2DataA));
	memset(BD2DataB,0,sizeof(BD2DataB));
	memset(HeaderData,0,sizeof(HeaderData));

	//Prepare first field of Header
	strcpy(HeaderData,"00000000"); //i.e Protocol Descriptor

	//get generated data
	ret = secureISO_GenerateRandom_BD0(BD0Data);
	CHECK(ret > 0,lblKO);
	strcpy(BD0_Out,BD0Data);

	strcat(HeaderData,BD0Data); //i.e Header BD0 to Header

	ret = secureISO_GetFullDateTime_BD2(BD2Data);
	CHECK(ret > 0,lblKO);
	strcpy(BD2_Out,BD2Data);

	strcat(HeaderData,BD2Data); //i.e Header BD2 to Header

	strncpy(BD2DataA,BD2Data,8);
	strncpy(BD2DataB,&BD2Data[8],4);
	BD2DataB[4] = '0';
	BD2DataB[5] = '0';
	BD2DataB[6] = '0';
	BD2DataB[7] = '0';

	/////&&&&&&&&&&&&&&&& RESULT 1 &&&&&&&&&&&&&&&
	// =========== convert into binary data =============
	hex2bin(BD0,BD0Data,0);
	hex2bin(BD2,BD2DataA,0);
	// ============== do Xor calculation ================
	for ( i = 0 ; i < 8 ; i++){
		BD1a[i]  = BD0[i] ^ BD2[i];
	}

	/////&&&&&&&&&&&&&&&& BD1 &&&&&&&&&&&&&&&
	// =========== convert into binary data =============
	memset(BD2,0,sizeof(BD2));
	hex2bin(BD2,BD2DataB,0);
	// ============== do Xor calculation ================
	for ( i = 0 ; i < 8 ; i++){
		BD1[i]  = BD1a[i] ^ BD2[i];
	}

	bin2hex(BD1Data,BD1,8);

	strcpy(BD1_Out,BD1Data);

	return strlen(BD1Data);
	lblKO:
	return -1;
}

//****************************************************************************
//                     int secureISO_ChkPpdConfig (void)
//  This function checks the pinpad configuration from the Manager.
//  This function has no parameters.
//  This function has return value.
//    >=0 : Right configuration (No pinpad=0 or pinpad present=1)
//     <0 : Wrong configuration (Wrong pinpad or pinpad missing but required=-1)
//****************************************************************************

static int secureISO_ChkPpdConfig(void) {
	// Local variables
	// ***************
	char tcReference[17+1];
	int iRet;

	// Check pinpad configuration from Manager
	// ***************************************
	iRet = PSQ_Is_pinpad();
	if (iRet != 0){                                             // Manager asks for a pinpad

		memset(tcReference, 0, sizeof(tcReference));
		iRet = IsPPSConnected(); CHECK(iRet==RET_PP_OK, lblKO); // Check if pinpad is connected
		iRet=1;                                                 // Pinpad is connected
#ifndef WIN32
		PPS_SendTerminalRefRequestCommand((unsigned char *)tcReference);         // Get pinpad reference (Bug ret is always 0)
		switch (PSQ_Pinpad_Value())
		{
		case PP30_PPAD:                                         // Pinpad characters no reader
			CHECK (memcmp(&tcReference[8], "PP30 ", 5)==0, lblKO);
			break;
		case PP30S_PPAD:                                        // Pinpad characters no reader
			CHECK (memcmp(&tcReference[8], "PP30S", 5)==0, lblKO);
			break;
		case PPC30_PPAD:                                        // Pinpad graphic no reader
			CHECK (memcmp(&tcReference[8], "PPC30", 5)==0, lblKO);
			break;
		case PPR30_PPAD:                                        // Pinpad graphic reader
			CHECK (memcmp(&tcReference[8], "PPR30", 5)==0, lblKO);
			break;
		case P30_PPAD:                                          // Pinpad graphic reader
			CHECK (memcmp(&tcReference[8], "P30  ", 5)==0, lblKO);
			break;
		default:                                                // Pinpad unknown
			iRet=-1;
			break;
		}
#endif
	}

	goto lblEnd;                                                // Right pinpad configuration (No pinpad or pinpad present)

	// Errors treatment
	// ****************
	lblKO:
	//	Telium_Putchar('\x1B');                                            // Clear screen
	//	Telium_Printf ("Pinpad Required\n"                                 // Pinpad required (pinpad disconnected or missing)
	//			"Check Ppd Config");
	//	Telium_Pprintf("\x1b""E""     Pinpad problem!!!\n\n" "\x1b""F");
	//	Telium_Pprintf("------------------------");
	//	Telium_Pprintf("Pinpad disconnected\n");
	//	Telium_Pprintf("=> plug-it again\n");
	//	Telium_Pprintf("---------- OR ----------");
	//	Telium_Pprintf("Wrong pinpad connected\n");
	//	Telium_Pprintf("=> Check MANAGER config\n");
	//	Telium_Pprintf("---------- OR ----------");
	//	Telium_Pprintf("Pinpad required\n");
	//	Telium_Pprintf("=> Check MANAGER config\n");
	//	Telium_Pprintf("------------------------");
	//	Telium_Pprintf("\n\n\n\n\n\n");
	//	Telium_Ttestall(PRINTER, 2*100);                                   // Wait until everything printed
	iRet=-1;
	lblEnd:
	return iRet;
}

////****************************************************************************
////  void secureISO__ASession_BSession_KEYS(void)
////  This function Generates the Asession Key and the Bsession key.
////****************************************************************************

// Secret area identification
// ==========================
#define secureISO_AREA_ID	           0x00002030   // Secret area identification from 00002030 to 3FFF2F30

// Key(s) Location                      // Location into secred area
// ===============
#define  secureISO_ROOT_KEY_LOC	   126*8        // Root Key                   126 slot => 1008 position bit)
#define  secureISO_MAC_KEY_LOC         11*8        // Mac Key                     11 slot =>   88 position bit)

// Bank(s) Id                            // Bank identification from 80000200 to FFFF0200
// ==========                            // related to a key loaded into secret area (2 last bytes)
#define secureISO_BANK_ROOT_ID 		0x800002FC   // 126*8 => 128*2*4 => 256*4 => 0xFC*4
#define secureISO_BANK_MAC_ID         0x80000216   // 11*8  => 11*2*4  => 22*4  => 0x16*4

void secureISO__ASession_BSession_KEYS(void) {
	// Local variables
	// ***************
	char BD0[32 + 1],BD1[32 + 1],BD2[32 + 1],Header[32 + 1],Asession_Key[16 + 1],Bsession_Key[16 + 1],Session_Key[32 + 1];
	T_SEC_DATAKEY_ID xMacKey;
	byte tucMsg[24] = "\x11\x11\x11\x11\x11\x11\x11\x11"
			"\x22\x22\x22\x22\x22\x22\x22\x22"
			"\x33\x33\x33\x33\x33\x33\x33\x33";
	byte tucMac[8];
	byte tucSesB[8 + 1];
	int iRet = 0,ret = 0;
	T_GL_HWIDGET hScreen=NULL;    // Screen handle

	// Initialisations
	// ***************

	hScreen = GoalCreateScreen(hGoal, txGPRS, NUMBER_OF_LINES(txGPRS), GL_ENCODING_UTF8);
	CHECK(hScreen!=NULL, lblKO);                                    // Create screen and clear it

	iRet = GoalClrScreen(hScreen, GL_COLOR_BLACK, KEY_CANCEL, false);
	CHECK(iRet>=0, lblKO);

	memset(BD0,0,sizeof(BD0));
	memset(BD1,0,sizeof(BD1));
	memset(BD2,0,sizeof(BD2));
	memset(Header,0,sizeof(Header));
	memset(tucSesB,0,sizeof(tucSesB));

	secureISO_Generate_All_BasicData(BD0,BD1,BD2,Header);

	//	MAPPUTSTR(traBDO,BD0,lblKO);
	//	MAPPUTSTR(traBD1,BD1,lblKO);
	//	MAPPUTSTR(traBD2,BD2,lblKO);
	//	MAPPUTSTR(traHeader,Header,lblKO);

	memset(tucMsg,0,sizeof(tucMsg));
	hex2bin(tucMsg,BD1,0);

	iRet = DLLExist("SECURITY");
	CHECK(iRet==TRUE, lblNoSecDll);    								// Security DLL loaded?

	iRet = secureISO_ChkPpdConfig();
	CHECK(iRet>=0, lblEnd);          							     // Pinpad problem? => check config from Manager

	// Mac calculation Generate A-session Key
	// ***************
	xMacKey.iSecretArea = secureISO_AREA_ID;                                   // Secret area identification
	xMacKey.cAlgoType = TLV_TYPE_KTDES;                               // MAC key is a DES Key
	xMacKey.usNumber = secureISO_MAC_KEY_LOC;                                  // MAC key location inside the secret area
	xMacKey.uiBankId = secureISO_BANK_MAC_ID;                                  // Bank id related to this MAC key

	//	iRet = SEC_ComputeMAC (&xMacKey,                                 // MAC key parameters
	//			(unsigned char *) tucMsg,                 // Message to compute
	//			sizeof(tucMsg),
	//			NULL,
	//			tucMac);

	iRet = SEC_ComputeMAC_AC (&xMacKey,                                 // MAC key parameters
			(unsigned char *)tucMsg,                 // Message to compute
			strlen((char *)tucMsg),                          //sizeof(tucMsg)minus the MAC value
			NULL,									 // IV has to be null or ZERO
			tucMac);                                 // MAC result

	memset(Asession_Key,0,sizeof(Asession_Key));
	memset(Bsession_Key,0,sizeof(Bsession_Key));

	bin2hex(Asession_Key,tucMac,8);

	///Generate B-Session Key
	BD2[12] = '0';
	BD2[13] = '0';
	BD2[14] = '0';
	BD2[15] = '0';

	stdDES(tucSesB,(byte *) BD2, tucMac);
	bin2hex(Bsession_Key,tucSesB,8);

	//save the whole session doublekey length
	strcpy(Session_Key,Asession_Key);
	strcat(Session_Key,Bsession_Key);
	MAPPUTSTR(appSessionKey_SecureIso,Session_Key,lblKO);

	CHECK(iRet==0, lblKO);

	goto lblEnd;

	// Errors treatment
	// ****************
	lblNoSecDll:

	iRet = GoalDspLine(hScreen, 2, "Security DLL is Missing!!!", &txGPRS[1], 0, true);
	CHECK(iRet>=0, lblKO);

	goto lblEnd;

	lblKO:

	iRet = GoalDspLine(hScreen, 2, "Processing Error!!!", &txGPRS[1], 0, true);
	CHECK(iRet>=0, lblKO);

	lblEnd:;
}

//****************************************************************************
//                          void LoadMacKey(void)
//  This function loads the Mac key inside the secret area by using the Root key.
//       SEC_LoadKey() : To load a key in secret area
//  This function has no parameters.
//  This function has no return value.
//****************************************************************************
void secureISO_LoadMacKey(void) {
	// Local variables
	// ***************
	byte tucChkSum[3];
	T_SEC_DATAKEY_ID /*xRootKey,*/ xMacKey;
	doubleword uiLen;
	int iRet;
	char MacKey[32 + 1];
	T_GL_HWIDGET hScreen=NULL;    // Screen handle


	// Initialisations
	// ***************

	hScreen = GoalCreateScreen(hGoal, txGPRS, NUMBER_OF_LINES(txGPRS), GL_ENCODING_UTF8);
	CHECK(hScreen!=NULL, lblKO);                                    // Create screen and clear it

	iRet = GoalClrScreen(hScreen, GL_COLOR_BLACK, KEY_CANCEL, false);
	CHECK(iRet>=0, lblKO);

	iRet = DLLExist("SECURITY"); CHECK(iRet==TRUE, lblNoSecDll); // Security DLL loaded?
	iRet = secureISO_ChkPpdConfig(); CHECK(iRet>=0, lblEnd);               // Pinpad problem? => check config from Manager


	// Load Mac Key using Root Key
	// ***************************
//	xRootKey.iSecretArea = secureISO_AREA_ID;                              // Secret area identification
//	xRootKey.cAlgoType   = TLV_TYPE_KTDES;                          // This ROOT key is a TDES Key
//	xRootKey.usNumber    = secureISO_ROOT_KEY_LOC;                         // ROOT key location inside the secret area
//	xRootKey.uiBankId    = secureISO_BANK_ROOT_ID;                         // Bank id related to this ROOT key

	xMacKey.iSecretArea = secureISO_AREA_ID;                               // Secret area identification
	xMacKey.cAlgoType   = TLV_TYPE_KTDES;                           // This MAC key is a DES Key
	xMacKey.usNumber    = secureISO_MAC_KEY_LOC;                           // MAC key location inside the secret area
	xMacKey.uiBankId    = secureISO_BANK_MAC_ID;                           // Bank id related to this MAC key

	iRet = SEC_LoadKey (C_SEC_CIPHERING,                         // C_SEC_CIPHERING secure part chosen without any recommendation to load MAC key
			//&xRootKey,                               // ROOT key parameters
			NULL,                                    // No ROOT key parameters
			&xMacKey,                                // MAC key parameters
			(unsigned char*) MacKey,                // The MAC key value
			CIPHERING_DATA);                         // MAC key usage : to cipher Data Message
	CHECK(iRet==OK, lblKO);

	// MAC Key CheckSum
	// ****************
	iRet = SEC_KeyVerify (C_SEC_PINCODE,                          // C_SEC_PINCODE secure part recommended (PCI/PED)
			&xMacKey,                               // MAC key parameters
			tucChkSum,                              // MAC key checksum
			&uiLen);                                // Length checksum




//	iRet = Telium_Printf("Key loaded...\n"                               // Display message
//			"CheckSum: %02X%02X%02X", tucChkSum[0], tucChkSum[1], tucChkSum[2]);
//	CHECK(iRet>=0, lblKO);
//	Telium_Ttestall(0, 5*100);                                           // Delay 5s

	goto lblEnd;

	// Errors treatment
	// ****************
	lblNoSecDll:

	iRet = GoalDspLine(hScreen, 2, "Security DLL is Missing!!!", &txGPRS[1], 0, true);
	CHECK(iRet>=0, lblKO);

	goto lblEnd;

	lblKO:

	iRet = GoalDspLine(hScreen, 2, "Processing Error!!!", &txGPRS[1], 0, true);
	CHECK(iRet>=0, lblKO);

	lblEnd:
	return;
}
