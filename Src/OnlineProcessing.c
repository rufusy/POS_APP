#include <globals.h>

#include "GTL_Assert.h"
#include "EMV_Support.h"
#include "SSL_.h"

//****************************************************************************
//      EXTERN
//****************************************************************************
extern T_GL_HGRAPHIC_LIB hGoal; // Handle of the graphics object library

#define CHK CHECK(ret>=0,lblKO)

//****************************************************************************
//      PRIVATE CONSTANTS
//****************************************************************************
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



//! \brief Manage the online authorisation with the communication component.
//! \param[in,out] pSharedData Data to be sent to the host for onoline authorisation (input) and response tags are added to it (output).
//! \return
//!		- \ref TRUE if correctly performed.
//!		- \ref FALSE if an error occurred.

int Cless_FinancialCommunication_ManageAuthorisation (T_SHARED_DATA_STRUCT * pSharedData) {
	DataElement Elt;
	int cr, ret = 0;
	int nPosition;
	unsigned long ulReadTag, ulReadLength;
	unsigned char * pReadValue;
	int nResult = FALSE;
	word key,beg;
	char dat[999 + 1];

	nPosition = SHARED_EXCHANGE_POSITION_NULL;

	do {
		cr = GTL_SharedExchange_GetNext(pSharedData, &nPosition, &ulReadTag, &ulReadLength, (const unsigned char **)&pReadValue);

		if (cr == STATUS_SHARED_EXCHANGE_OK) {
			memset(dat, 0, sizeof(dat));
			Elt.tag = ulReadTag;
			Elt.length = (int)ulReadLength;
			Elt.ptValue = pReadValue;

			key = mapKeyTag(Elt.tag);

			if(key == 0){
				continue;
			}

			beg = begKey(key);
			switch (beg) {
			case traBeg:
			case appBeg: //Local to the application
				ret = Elt.length * 2;

				//append length to tag
				num2hex(dat,Elt.length,2);

				// NOTE: all local vars are in HEX
				bin2hex(&dat[1],Elt.ptValue, Elt.length);

				ret = mapPut(key,dat, ret);
				CHK;
				break;
			default:// All others come from Kernel
				dat[0] = Elt.length;
				memcpy(&dat[1], Elt.ptValue, Elt.length);

				ret = mapPut(key, Elt.ptValue, Elt.length);
				CHK;
				break;
			}

		}
	} while (cr == STATUS_SHARED_EXCHANGE_OK);



	nResult = TRUE;

	lblKO:
	return (nResult);
}

/*****
 *
 *
 */
int performOlineTransaction(void){
	byte CommRoute = 0;
	byte TLS_Enabled = 0;
	byte bcdLReq[lenBCDMsg];
	byte bcdNii[lenNii + 1];
	char Nii[6 + 1];
	char tpduHead[4 + 1];
	//	char tpduModem[8 + 1];
	char tpduTCPIP[10 + 1];
	//	char tpduRS232[8 + 1];
	char strTPDU[64 + 1];
	tBuffer bReq;    // Request Buffer
	byte dReq[(1024 * 3) + 1]; // Request data
	tBuffer bRsp;    // Response Buffer
	byte dRsp[(1024 * 3) + 3]; // Response data
	tBuffer bTPDUReq;// TPDU Request Buffer
	byte dTPDUReq[lenTPDU + lenBCDMsg + 1];
	byte bytTPDU[6 + 1];
	int ret = 0;
	word TLS_SSL = 0;
	byte byteTemp = 0;
	word wordTemp = 0;
	T_GL_HWIDGET hScreen=NULL;    // Screen handle

	hScreen = GoalCreateScreen(hGoal, txGPRS, NUMBER_OF_LINES(txGPRS), GL_ENCODING_UTF8);
	CHECK(hScreen!=NULL, lblKO);                                    // Create screen and clear it

	ret = GoalClrScreen(hScreen, GL_COLOR_BLACK, KEY_CANCEL, false);
	CHECK(ret>=0, lblKO);

	memset(dRsp, 0, sizeof(dRsp));
	memset(dReq, 0, sizeof(dReq));
	memset(bytTPDU, 0, sizeof(bytTPDU));
	memset(strTPDU, 0, sizeof(strTPDU));
	memset(dTPDUReq, 0, sizeof(dTPDUReq));
	memset(tpduHead, 0, sizeof(tpduHead));

	//initialize request buffer
	bufInit(&bRsp, dRsp, sizeof(dRsp));
	bufInit(&bReq, dReq, sizeof(dReq));
	bufInit(&bTPDUReq, dTPDUReq, sizeof(dTPDUReq));

	ret = GoalDspLine(hScreen, 2, "Building Request...", &txGPRS[3], 0, true);
	CHECK(ret>=0, lblKO);
	//Telium_Ttestall(0, 2*100);

	MAPGET(appNII, Nii, lblKO);
	fmtPad(Nii, -(lenNII + 1), '0');
	hex2bin(bcdNii, Nii, 0);

	MAPPUTSTR(traRspCod, "100", lblKO);
	ret = reqBuild(&bReq);
	CHECK(ret > 0, lblKO);

	// Prepare for any reversal if need be
	isReversibleSend();

	num2bin(bcdLReq, bufLen(&bReq) + 5 , sizeof(bcdLReq));
	ret = bufApp(&bTPDUReq, bcdLReq, 2);  //Message Length

	//Build TPDU
	ret = 0;
	MAPGET(apptpduHead, tpduHead, lblKO);
	MAPGET(apptpduTCPIP, tpduTCPIP, lblKO);

	strcat(strTPDU,tpduHead);
	strcat(strTPDU,Nii);
	strcat(strTPDU,tpduTCPIP);

	hex2bin(bytTPDU,strTPDU,5);  //Standard
	ret = bufApp(&bTPDUReq, bytTPDU, 5);    //append bin data

	///Get the communication route
	mapGetByte(appCommRoute,CommRoute);

	///Get the Secure route
	mapGetByte(appCommSSL,TLS_Enabled);

	switch (TLS_Enabled) {
	case 'Y':
		TLS_SSL = 1;
//		///----------------------------------
//		switch (CommRoute) {
//		case 'T':
//			CommRoute = 'S';
//			break;
//		default:
//			break;
//		}
//		///----------------------------------
		break;
		default:
			break;
	}


	//make sure the SSL configs are okay
	comCheckSslProfile();

	ret = GoalDspLine(hScreen, 2, "Please Wait...", &txGPRS[3], 0, true);
	CHECK(ret>=0, lblKO);

	if (hScreen)
		GoalDestroyScreen(&hScreen);                                  // Destroy screen

	ret = bufIns(&bReq, 0, bufPtr(&bTPDUReq), bufLen(&bTPDUReq));
	CHECK(ret > 0, lblKO);

	/// Perform the transaction by route
	switch (CommRoute) {
	case 'T'://Ethernet or TCP/IP

		ret = ComEthernet(&bReq,&bRsp, TLS_SSL);
		CHECK(ret >= 10, lblKO);

		break;
	case 'P'://PPP

		ret = ComPPP(&bReq,&bRsp, TLS_SSL);
		CHECK(ret == bufLen(&bReq), lblKO);

		break;
	case 'M'://Modem

		ret = ComModem(&bReq,&bRsp, TLS_SSL);
		CHECK(ret >= 10, lblKO);

		break;
	case 'R'://Serial

		ret = ComSerial(&bReq,&bRsp, TLS_SSL);
		CHECK(ret >= 10, lblKO);

		break;
	case 'U'://USB

		ret = ComUSB(&bReq,&bRsp, TLS_SSL);
		CHECK(ret == bufLen(&bReq), lblKO);

		break;
	case 'S'://SSL

		ret = ComSSL(&bReq,&bRsp);
		CHECK(ret >= 10, lblKO);

		break;
	case 'W'://WIFI

		ret = comWifiConnect(&bReq,&bRsp, TLS_SSL);
		CHECK(ret >= 10, lblKO);

		break;
	case 'G': //GPRS
	default:  //GPRS

		ret = ComGPRS(&bReq,&bRsp, TLS_SSL);
		CHECK(ret >= 10, lblKO);

		break;
	}

	ret = bufDel(&bRsp, 0, lenBCDMsg + lenTPDU);  //remove Message Length and TPDU from the message
	CHECK(ret >= 0, lblKO);

	ret = rspParse(bufPtr(&bRsp), bufLen(&bRsp));   //parse response message
	//CHECK(ret >= 0, lblKO); // AJ note: dont know why this is commented out

	byteTemp = 0;
	wordTemp = 0;

	mapPutByte(appReversalFlag, byteTemp);
	mapPutWord(appShowControlPanel, wordTemp); ///Make sure the CPANEL is hidden from user No error
	return TRUE;

	lblKO:
	wordTemp = 0;
	mapPutWord(appShowControlPanel, wordTemp); ///Make sure the CPANEL is hidden from user even with error
	return FALSE;
}



/*****
 *
 *
 */
int checkOlineServer(void){
	byte CommRoute = 0;
	byte SecuredComms = 0;
	int ret = 0;
	byte tlsSsl = 0;

	///Get the communication route
	mapGetByte(appCommRoute,CommRoute);

	///Get the Secure route
	mapGetByte(appCommSSL,SecuredComms);

	switch (SecuredComms) {
	case 'Y':
		tlsSsl = 1;
		break;
	default:
		break;
	}


	/// Perform the transaction by route
	switch (CommRoute) {
	case 'T'://Ethernet or TCP/IP

		//		ComEthernetCheck(tlsSsl);
		//		CHECK(ret > 0, lblKO);

		break;
	case 'P'://PPP

		//		ret = ComPPPCheck();
		//		CHECK(ret > 0, lblKO);

		break;
	case 'M'://Modem

		//		ret = ComModemCheck();
		//		CHECK(ret > 0, lblKO);

		break;
	case 'S'://SSL

		//		ret = ComSSLCheck();
		//		CHECK(ret > 0, lblKO);

		break;
	case 'G': //GPRS
	default:  //GPRS

		ret = ComGPRSCheck(tlsSsl);
		CHECK(ret > 0, lblKO);

		break;
	}

	return TRUE;

	lblKO:
	return FALSE;
}


/**
 * Work with embedded keys
 * @return
 */
int getOnlinePin(void){
	byte pinCode[lenPIN + 1];
	byte panBis[17];
	char Pin[16+1];
	char Pan[lenPan+1];
	byte PinBlok[9];
	byte pcc[8]; //pin
	byte acc[8]; //account extracted from pan
	byte blk[8],b[16]; //pinblock calculated by cryptomodule
	byte vlPin[3];
	byte Padding[11]="FFFFFFFFFF";
	int  Plen;
	int ret;
	int i;
	int lenP;
	char tcMask[256];
	T_GL_HWIDGET hScreen=NULL;    // Screen handle



	//byte dftManufactKey[24] = {0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01};
	byte tucTpk[16+1],tucMpk[16+1],tpk[32+1]; //="\xF4\x08\x86\xAB\x8C\x40\x94\xBC\xF4\x08\x86\xAB\x8C\x40\x94\xBC\xF4\x08\x86\xAB\x8C\x40\x94\xBC";
	char mDisp[100],tucTpkTemp[32+1],tucMpkTemp[32+1];

	memset(tpk,0,sizeof(tpk));
	memset(tcMask,0,sizeof(tcMask));
	memset(tucTpk,0,sizeof(tucTpk));
	memset(tucMpk,0,sizeof(tucMpk));
	memset(tucTpkTemp,0,sizeof(tucTpkTemp));
	memset(tucMpkTemp,0,sizeof(tucMpkTemp));
	memset(Pan, 0, sizeof(Pan));
	memset(Pin, 0, sizeof(Pin));
	memset(pcc, 0, 8);
	memset(PinBlok, 0, sizeof(PinBlok));
	memset(panBis, 0, 17);
	memset(pinCode,0,sizeof(pinCode));
	memset(vlPin,0,sizeof(vlPin));
	memset(mDisp,0,sizeof(mDisp));

	//Make sure the pripherals are open
	OpenPeripherals();

	MAPGET(appMkey,tucMpkTemp,lblKO); // Get the Key from the database
	hex2bin(tucMpk,tucMpkTemp,0);

	MAPGET(appTkey,tucTpkTemp,lblKO); // Get the Key from the database
	hex2bin(tucTpk,tucTpkTemp,0);

	//Decrypt Key for feild 52
	stdDED(tpk,tucTpk,tucMpk);
	stdDED(tpk+8,tucTpk+8,tucMpk);
	//strncat(tpk,tpk,8);
	bin2hex((char *)b,tpk,8);
	bin2hex((char *)b,tpk+8,8);

	hScreen = GoalCreateScreen(hGoal, txGPRS, NUMBER_OF_LINES(txGPRS), GL_ENCODING_UTF8);
	CHECK(hScreen!=NULL, lblKO);                                          // Create screen and clear it

	ret = GoalClrScreen(hScreen, GL_COLOR_BLACK, KEY_CANCEL, false);
	CHECK(ret>=0, lblKO);

	// Prepare header of pin entry
	MAPGET(traCtx, mDisp, lblDBA);
	strcat(mDisp, " ONLINE PIN");

	memset(tcMask, 0, sizeof(tcMask));
	for (i=0; i<lenPIN; i++)
		strcat(tcMask, "/d");

	//Enter PIN
	ret = GL_Dialog_Password(hGoal, mDisp, "Enter PIN# :", tcMask, (char *)pinCode, sizeof(pinCode), GL_TIME_MINUTE);
	CHECK((ret != GL_KEY_CANCEL) && (ret != GL_RESULT_INACTIVITY), lblDBA); // Exit on cancel/timeout

	Plen = strlen((char *)pinCode);

	CHECK(Plen>=4,lblKO);
	ret = num2dec((char *)vlPin,Plen,2);

	//Format PIN
	memcpy(&Pin[0],vlPin,2);
	memcpy(&Pin[2],&pinCode[0],Plen);
	memcpy(&Pin[2+Plen],Padding,(14-Plen));
	hex2bin(pcc,Pin,8);

	//Format PAN
	//MAPPUTSTR(traPan,"4407830039255022",lblKO);
	ret= mapGet(traPan,Pan,lenPan+1); CHECK(ret>=0,lblKO);
	fncPanRemove_F(Pan);
	memcpy(&panBis[0],"0000",4);
	lenP= strlen(Pan);

	memcpy(&panBis[4],&Pan[lenP-13],12);
	hex2bin(acc, (char *)panBis,8);

	//CALCULATE CRYPTOMODULE
	for ( i = 0 ; i < 8 ; i++)
		PinBlok[i]  = pcc[i] ^ acc[i];

	//Encrypt PinBlok for feild 52
	stdEDE(blk,PinBlok,tpk);

	// Save feild 52
	ret= mapPut(traPinBlk,blk,8); CHECK(ret>=0,lblDBA);

	ClosePeripherals(); //close all peripherals
	return  1;

	lblDBA:
	ClosePeripherals(); //close all peripherals
	return -2;

	lblKO :
	ClosePeripherals(); //close all peripherals
	return -1;
}

