//****************************************************************************
//       INGENICO                                INGEDEV 7                   
//============================================================================
//       FILE  MAPAPP.C                          (Copyright INGENICO 2012)
//============================================================================
//  Created :       09-July-2012     Kassovic
//  Last modified : 09-July-2012     Kassovic
//  Module : TRAINING
//                                                                          
//  Purpose :                                                               
//  Supply access by integer key number to data by using the functions
//  appGet() and appPut(). It contains all the application parameters
//  saved inside none-volatile memory (DFS).
//                                                                            
//  List of routines in file :  
//      appReset : Reset none-volatile application parameters.
//      appPut : Store application parameter.
//      appGet : Retrieve application parameter.
//                            
//  File history :
//  070912-BK : File created
//                                                                           
//****************************************************************************

//****************************************************************************
//      INCLUDES                                                            
//****************************************************************************
#include <globals.h>
#include "VGE_FMG.h"


//****************************************************************************
//      EXTERN                                                              
//****************************************************************************
const char *getAppCmpDat(void){ return __DATE__;} // Return the date of compilation of the file Mapapp.c
const char *getAppCmpTim(void){ return __TIME__;} // Return the time of compilation of the file Mapapp.c

//****************************************************************************
//      PRIVATE CONSTANTS                                                   
//****************************************************************************
/* */

//****************************************************************************
//      PRIVATE TYPES                                                       
//****************************************************************************
// Parameter row
// =============
typedef struct stParamRow
{
	word usKey;                // Parameter key
	word usLen;                // Parameter length
	void *pvDefault;           // Parameter default
} ST_PARAM_ROW;

//****************************************************************************
//      PRIVATE DATA                                                        
//****************************************************************************
// Parameter table
// ===============
static const ST_PARAM_ROW tzApp[appEnd-appBeg] = {
		{ appCmpDat,             lenCmpDat,            "" },                      // Compile date Mapapp.c
		{ appCmpTim,             lenCmpTim,            "" },                      // Compile time Mapapp.c
		{ appAppName,            49,                   "" },                      // Compile time Mapapp.c

		{ appSerialItem,         lenSerialItem,        "\x00\x01\x00\x00\x08" },  // Com Port, Data Bits, Parity, Stop Bits, Baud Rate
		{ appSerialInit,         lenSerialInit,        "8N1115200" },             //              8         N        1         115200

		{ appModemItem,          lenModemItem,         "\x01\x00\x08" },          // Correction,   , Data Bits, Parity,   ,Baud Rate
		{ appModemInit,          lenModemInit,         "?T8N1115200" },           //     ?       T       8         N    1    115200
		{ appModemPabx,          lenModemPabx,         "" },                      // Pabx
		{ appModemPrefix,        lenModemPrefix,       "" },                      // Prefix
		{ appModemCountry,       lenModemCountry,      "" },                      // Country code
		{ appModemPhone,         lenModemPhone,        "0169012345" },            // Phone number

		{ appEthIpLocal,         lenEthIpLocal,        "192.168.254.084" },       // Local Ip Address
		{ appEthPort,            lenEthPort,           "7011" },                  // Port number

		{ appGprsRequirePin,     lenGprsRequirePin,    "0" },                     // Pin required?
		{ appGprsApn,            lenGprsApn,           "safaricom" },             // Apn
		{ appGprsUser,           lenGprsUser,          "saf" },                   // User name
		{ appGprsPass,           lenGprsPass,          "data" },                  // Password
		{ appGprsIpRemote,       lenGprsIpRemote,      "010.200.217.028" },       // Remote Ip Address
		{ appGprsPort,           lenGprsPort,          "7009" },                  // Port number

		{ appPppPabx,            lenPppPabx,           "" },                      // Pabx
		{ appPppPrefix,          lenPppPrefix,         "" },                      // Prefix
		{ appPppCountry,         lenPppCountry,        "" },                      // Country code
		{ appPppPhone,           lenPppPhone,          "0169012345" },            // Phone number
		{ appPppUser,            lenPppUser,           "Test" },                  // User name
		{ appPppPass,            lenPppPass,           "1234" },                  // Password
		{ appPppIpLocal,         lenPppIpLocal,        "192.168.1.2" },           // Local Ip Address
		{ appPppPort,            lenPppPort,           "1000" },                  // Port number
		{ appCommRoute,          3,                    "G" },
		{ appCommSSL,            3,                    "X" },
		{ appNII,                lenNii,               "0003" },
		{ apptpduHead,           2,                    "60" },
		{ apptpduTCPIP,          4,                    "0000" },
		{ apptpduModem,          4,                    "8000" },
		{ apptpduRS232,          4,                    "0665" },
		{ appSTAN,               6,                    "" },
		{ appCurBat,             6,                    "" },
		{ appCurrCodeAlpha,      3,                    "TZS" },
		{ appExp,                1,                    "2" },
		{ appTID,                lenTID,               "INGTST2K" },
		{ appMID,                lenMid,               "000000000000001" },
		{ appMerchantName,       512,                   "CHEF AT WORK LTD" },
		{ appHeader1,            512,                   "Chef at work Ltd" },
		{ appHeader2,            512,                   "DAR-ES-SALAAM, Tanzania" },
		{ appHeader3,            512,                   "" },
		{ appAppLoggedName,      128,                   "----" },
		{ appAppVersionNumber,   6,                    "0007" },
		{ appAppVersionName,     32,                   "Ver ECMP0007" },
		{ appAppVersion,         32,                   "Ver ECMP" },
		{ appFooterText,         512,                  "I AGREE TO PAY ABOVE TOTAL AMOUNT ACCORDING TO CARD ISSUER AGREEMENT" },
		//		 ///// ---  all EMV tags are stored as binary (the first byte is the data length)  ---
		//		{ emvTrnCurCod,          lenTrnCurCod + 2,        "\x02\x04\x04"                  },// 5F2A Transaction Currency Code />
		//		{ emvTrnCurExp,          lenTrnCurExp + 2,        "\x01\x02"                      },// 5F36 Transaction Currency Exponent />
		//		{ emvMrcCatCod,          lenMrcCatCod + 2,        "\x02\x53\x11"                  },// 9F15 Merchant Category Code />
		//		{ emvTrmCntCod,          lenTrmCntCod + 2,        "\x02\x06\x08"                  },// 9F1A Terminal Country Code />
		//		{ emvTrmId,              lenTID + 2,              "\x08""00000211"                },// 9F1C Terminal Identification />
		//		{ emvTrmCap,             lenTrmCap + 2,           "\x03\xE0\xF8\xC8"              },// 9F33 Terminal Capabilities />
		//		{ emvTrmTyp,             lenEmvTrmTyp + 2,        "\x01\x22"                      },// 9F35 Terminal Type />
		//		{ emvAddTrmCap,          lenAddTrmCap + 2,        "\x05\xF1\x00\xF0\xA0\x01"      },// 9F40 Additional Terminal Capabilities />
		//		{ emvVlpSupInd,          lenVlpSupInd + 2,        "\x01\x00"                      },// 9F7A VLP Terminal Support Indicator />
		//		{ emvVlpTrmLim,          lenVlpTrmLim + 2,        "\x06\x00\x00\x00\x00\x00\x00"  },// 9F7B VLP Terminal Transaction Limit />
		//		{ emvLstRspCod,          lenLstRspCod + 2,        "\x14""Y1Z1Y2Z2Y3Z300050104"    },// DF16 List of possible response codes />
		//		{ emvIFDSerNum,          lenIFDSerNum + 2,        "\x08""28725422"                }// 9F1E Interface Device (IFD) Serial Number />

		///// ---  all EMV tags are stored as binary (the first byte is the data length)  ---
		{ emvTrnCurCod,           (lenTrnCurCod * 2) +2,        "020404"                  },// 5F2A Transaction Currency Code />
		{ emvTrnCurExp,           (lenTrnCurExp * 2) +2,        "0102"                      },// 5F36 Transaction Currency Exponent />
		{ emvMrcCatCod,           (lenMrcCatCod * 2) +2,        "025311"                  },// 9F15 Merchant Category Code />
		{ emvTrmCntCod,           (lenTrmCntCod * 2) +2,        "020255"                  },// 9F1A Terminal Country Code />
		{ emvTrmId,               (lenTID * 2) +2,              "0800000211"                },// 9F1C Terminal Identification />
		{ emvTrmCap,              (lenTrmCap * 2) +2,           "03E0F8C8"              },// 9F33 Terminal Capabilities />
		{ emvTrmTyp,              (lenEmvTrmTyp * 2) +2,        "0122"                      },// 9F35 Terminal Type />
		{ emvAddTrmCap,           (lenAddTrmCap * 2) +2,        "05F100F0A001"      },// 9F40 Additional Terminal Capabilities />
		{ emvVlpSupInd,           (lenVlpSupInd * 2) +2,        "0100"                      },// 9F7A VLP Terminal Support Indicator />
		{ emvVlpTrmLim,           (lenVlpTrmLim * 2) +2,        "06000000000000"  },// 9F7B VLP Terminal Transaction Limit />
		{ emvLstRspCod,           (lenLstRspCod * 2) +2,        "14Y1Z1Y2Z2Y3Z300050104"    },// DF16 List of possible response codes />
		{ emvIFDSerNum,           (lenIFDSerNum * 2) +2,        "0828725422"                },// 9F1E Interface Device (IFD) Serial Number />
		{ appBatchNumber,         7,                           "000001"                    },
		{ appSessionKey_SecureIso,64,                          ""},
		{ appMkey,                64,                          "0123456789ABCDEFFEDCBA9876543210"},
		{ appTkey,                64,                          "1A4D672DCA6CB3351FD1B02B237AF9AE"},
		{ appTerminalSerial,      21,                          "12345678"},

		//{ appDUKPT_KSI,           21,                          "9876540000100001"},
		{ appDUKPT_KSN,           21,                          "9876540000100001"}, //"9876543210E00000" 5270482233E00001
		{ appDUKPT_BDK,           32,                          "6C1E054182422481BE2221780399BB72"},
		{ appDUKPT_IPEK,          32,                          "6C1E054182422481BE2221780399BB72"}, // same as Initial Key
		{ appFallback,            2,                           ""},
		{ appInvNum,              7,                           ""},
		{ appTID_1,                lenTID+1,                   "MP241007" },
		{ appMID_1,                lenMid+1,                   "639673042100001" },
		{ appTID_2,                lenTID+1,                   "INGTST4K" },
		{ appMID_2,                lenMid+1,                   "000000000000001" },
		{ appKeyPart,              5,                          "1234"},
		{ appReversalFlag,         6,                          "" },
		{ appAutoReversal,         6,                          "" },
		{ appClessMagMode,         6,                          "" },
		{ appCVM_ForcePIN,         6,                          "" },
		{ appClessModeOff,         6,                          "" },
		{ appAdminPass,            10,                         "0000" },
		{ appSupervPass,           10,                         "0000" },
		{ appMerchPass,            10,                         "0000" },
		{ appCurrCodeAlpha1,       4,                          "TZS" },
		{ appCurrCodeNumerc1,      4,                          "834" },
		{ appCurrExp1,             2,                          "2" },
		{ appCurrCodeAlpha2,       4,                          "USD" },
		{ appCurrCodeNumerc2,      4,                          "840" },
		{ appCurrExp2,             2,                          "2" },
		{ appShowControlPanel,     3,                          "" },
		{ appLastSettlementDate,   lenDatTim,                  "00000000000000" },
		{ appCardName,             32,                         ""},
		{ appSwitchLoops,          3,                          "" },

		///BILLER GOVT PAYMENTS
		{ appBillerPrefix,                lenDatTim,                    "00000000000000" },
		{ appBillerServiceProductCodes,   512,                          "00000000000000" },
		{ appBillerServiceProduct,        lenDatTim,                    "00000000000000" },
		{ appIsRef_ON,                    6,                            "" },
		{ appBillerSurcharge,             6,                            "0" },
		{ appTerminalMode,                6,                            "" },

};

static const char zAppTab[] = "appTSLTab.par";

//****************************************************************************
//                          int appReset(void)
// This function creates "app" table (\PARAMDISK\appTab.par).
//  If the file already exist, delete it then re-create it.
//  Finally "app" table is built with parameters initialized to default value.
// This function has no parameters.
// This function has return value.
//	 >=0 : Initialization done (size of bytes reseted).
//   <0  : Initialization failed (FMG failed).
//****************************************************************************

int appReset (void) {
	// Local variables 
	// ***************
	FMG_t_file_info xFileInfo;
	word usIdx;
	int iByteNbr=0, iRet, ret =0;

	// Create "app" table
	// ******************
	iRet = FMG_CreateFile(PARAM_DISK, (char*)zAppTab, FMG_VARIABLE_LENGTH, FMG_WITH_CKECKSUM);
	CHECK((iRet==FMG_SUCCESS)||(iRet==FMG_FILE_ALREADY_EXIST), lblKO);

	if (iRet==FMG_FILE_ALREADY_EXIST)                    // File already exist?
	{
		iRet = FMG_DeleteFile(PARAM_DISK, (char*)zAppTab);
		CHECK(iRet==FMG_SUCCESS, lblKO);                 // Delete it
		iRet = FMG_CreateFile(PARAM_DISK, (char*)zAppTab, FMG_VARIABLE_LENGTH, FMG_WITH_CKECKSUM);
		CHECK(iRet==FMG_SUCCESS, lblKO);                 // Re-create it
	}

	// Reset "app" table
	// *****************
	xFileInfo.eCreationType = FMGPathAndName;            // File type with Path and Name
	strcpy((char*)xFileInfo.ucFilePath, PARAM_DISK);     // \PARAMDISK
	strcpy((char*)xFileInfo.ucFileName, zAppTab);        // \appTab.par
	for (usIdx=0; usIdx<appEnd-appBeg; usIdx++) {        // Build "app" table with parameters filled with default value

		CHECK(tzApp[usIdx].usKey==usIdx+appBeg, lblKO);  // Check if it is the right key

		iByteNbr += (int)tzApp[usIdx].usLen;
		iRet = FMG_AddRecord(&xFileInfo, tzApp[usIdx].pvDefault, (long)tzApp[usIdx].usLen, FMGMiddle, usIdx);
		CHECK(iRet==FMG_SUCCESS, lblKO);
	}
	iRet = iByteNbr;                                     // Size of bytes reseted

	//Initialize all base values
	mapPutCard(appInvNum, 1);
	mapPutCard(appSTAN, 1);
	mapPutCard(appCVM_ForcePIN, 0);
	mapPutCard(appIsRef_ON, 0);

	mapPutByte(appClessMagMode, 0);
	mapPutByte(appClessModeOff, 0);
	mapPutByte(appTerminalMode, 0);

	MAPPUTSTR(appBillerPrefix, "QT",lblEnd);
	MAPPUTSTR(appBillerServiceProduct, "NCAA",lblEnd);

	comGPRS_SetDefaultsValues();

	goto lblEnd;

	// Errors treatment 
	// ****************
	lblKO:                                                   // Initialization failed
	iRet=-1;
	goto lblEnd;
	lblEnd:
	return iRet;
}

//****************************************************************************
//              int appPut(word usKey, void *pvDat, word usLen)
// This function stores the parameter related to the key into the appTab
//  file.
// This function has parameters.
//     (I-) usKey : Key from enum
//     (I-) pvDat : Parameter to be stored
//     (I-) usLen : Parameter length
// This function has return value.
//   >=0 : Storage done (size of bytes stored).
//   <0  : Storage failed (FMG failed).
//****************************************************************************

int appPut (word usKey,const void *pvDat, word usLen) {
	// Local variables 
	// ***************
	FMG_t_file_info xFileInfo;
	long lLength;
	int iRet;

	// Store parameter
	// ***************
	CHECK(tzApp[usKey-appBeg].usKey==usKey, lblKO);     // Check if it is the right key

	lLength = (long)tzApp[usKey-appBeg].usLen;
	if (lLength > usLen)
		lLength = (long)usLen;
	xFileInfo.eCreationType = FMGPathAndName;           // File type with Path and Name
	strcpy((char*)xFileInfo.ucFilePath, PARAM_DISK);    // \PARAMDISK
	strcpy((char*)xFileInfo.ucFileName, zAppTab);       // \appTab.par
	iRet = FMG_ModifyRecord(&xFileInfo, (void *)pvDat, lLength, FMGMiddle, usKey-appBeg);
	CHECK(iRet==FMG_SUCCESS, lblKO);                    // Store the parameter related to this key

	iRet = (int)lLength;                                // Size of bytes stored.
	goto lblEnd;

	// Errors treatment 
	// ****************
	lblKO:                                                  // Storage parameter failed
	iRet=-1;
	goto lblEnd;
	lblEnd:
	return iRet;
}


word appLen (word usKey) {
	// Local variables
	// ***************

	// Store parameter
	// ***************
	CHECK(tzApp[usKey-appBeg].usKey==usKey, lblKO);     // Check if it is the right key

	return tzApp[usKey-appBeg].usLen;

	// Errors treatment
	// ****************
	lblKO:                                                   // Retrieve parameter failed
	return -1;
}

//****************************************************************************
//              int appGet(word usKey, void *pvDat, word usLen)
// This function retrieves the parameter related to the key from the appTab
//  file.
// This function has parameters.
//     (I-) usKey : Key from enum
//     (-O) pvDat : Parameter to be retrieved
//     (I-) usLen : Parameter length
// This function has return value.
//   >=0 : Retrieve done (size of bytes retrieved)
//   <0  : Retrieve failed (FMG failed)
//****************************************************************************

int appGet (word usKey, void *pvDat, word usLen)
{
	// Local variables 
	// ***************
	FMG_t_file_info xFileInfo;
	long lLength;
	int iRet;

	// Retrieve parameter
	// ******************
	CHECK(tzApp[usKey-appBeg].usKey==usKey, lblKO);

	memset(pvDat, 0, usLen);
	lLength = (long) tzApp[usKey-appBeg].usLen;
	if (lLength > usLen)
		lLength = (long)usLen;
	xFileInfo.eCreationType = FMGPathAndName;            // File type with Path and Name
	strcpy((char*)xFileInfo.ucFilePath, PARAM_DISK);     // \PARAMDISK
	strcpy((char*)xFileInfo.ucFileName, zAppTab);        // \appTab.par
	iRet = FMG_ReadRecord(&xFileInfo, pvDat, &lLength, FMGMiddle, usKey-appBeg);
	CHECK(iRet==FMG_SUCCESS, lblKO);	                 // Retrieve the parameter related to this key

	iRet = (int)lLength;                                 // Size of bytes retrieved.
	goto lblEnd;

	// Errors treatment 
	// ****************
	lblKO:                                                   // Retrieve parameter failed
	iRet=-1;
	goto lblEnd;
	lblEnd:
	return iRet;
}
