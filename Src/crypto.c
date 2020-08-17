/*!
------------------------------------------------------------------------------
  INGENICO Technical Software Department
  ------------------------------------------------------------------------------
  Copyright (c) 2012, Ingenico.
  28-32 boulevard de Grenelle 75015 Paris, France.
  All rights reserved.

  This source program is the property of INGENICO Company and may not be copied
  in any form or by any means, whether in part or in whole, except under license
  expressly granted by INGENICO company

  All copies of this program, whether in part or in whole, and
  whether modified or not, must display this and all other
  embedded copyright and ownership notices in full.
  ------------------------------------------------------------------------------

  Project : Dll Security Sample
  Module  : FUN - Demo Features Unit

  @file      fun_dukpt.c
  @brief     This file contains the definition of processes that is implemented
  by this sample.

  @date      12/08/2014

------------------------------------------------------------------------------
 */

#include "sdk.h"
#include "SEC_interface.h"
#include "GL_GraphicLib.h"
#include "schVar_def.h"
#include "tlvVar_def.h"

#include "trace.h"
#include "globals.h"
#include "crypto.h"
#include "dukpt.h"

#define VAR_NUMBER_SIGN          0x015C
#define CARD_NUMBER_SIGN         0x10000856

/// ROOT DEFINITION
#define ID_BANK_XXX_ROOT         0x800002C0 // BK_SAGEM   /* BANK ID TO USE */ same as KEY ID
#define ID_SCR_XXX_BANK_ROOT     0x015C2132 /* SECRET AREA ID TO USE */
#define KEY_NO_XXX_ROOT          768        /* ROOT KEY NUMBER */// C0 = 192 -> 192*4 = 768

/// DUKPT DEFINITION
#define ID_BANK_XXX              0x800006FE // BK_SAGEM   /* BANK ID TO USE */ same as KEY ID
#define ID_SCR_XXX_BANK          0x015C2134 /* SECRET AREA ID TO USE */
#define KEY_NO_XXX               1016       /* ROOT KEY NUMBER */// FE = 254 -> 254*4 = 1016

//
//#define VAR_NUMBER_SIGN          0x006F
//#define CARD_NUMBER_SIGN         0x10000856
//
///// ROOT DEFINITION
//#define ID_BANK_XXX_ROOT         0x800002C0 // BK_SAGEM   /* BANK ID TO USE */ same as KEY ID
//#define ID_SCR_XXX_BANK_ROOT     0x006F2132 /* SECRET AREA ID TO USE */
//#define KEY_NO_XXX_ROOT          768        /* ROOT KEY NUMBER */// C0 = 192 -> 192*4 = 768
//
///// DUKPT DEFINITION
//#define ID_BANK_XXX              0x800006FE // BK_SAGEM   /* BANK ID TO USE */ same as KEY ID
//#define ID_SCR_XXX_BANK          0x006F2134 /* SECRET AREA ID TO USE */
//#define KEY_NO_XXX               1016       /* ROOT KEY NUMBER */// FE = 254 -> 254*4 = 1016




#define MASTER_SERIAL_KEY_NO_XXX 8          /* MASTER KEY SERIAL NUMBER */
#define PIN_MASTER_KEY_NO_XXX    72 /* PIN MASTER KEY NUMBER */
#define MAC_MASTER_KEY_NO_XXX    88 /* MAC MASTER KEY NUMBER */
#define PIN_KEY_NO_XXX           224 /* PIN KEY NUMBER */
#define MAC_KEY_NO_XXX           24 /* MAC KEY NUMBER */

#define C_NB_PARTS 3
#define ECHO_PIN '*'

typedef struct stPin {
	unsigned char dly;
	int state;
} ST_PIN;


static int _FUN_PinEncrypt(const unsigned char *pucAcc, unsigned char *pucBlk);
static void _FUN_SetRootKeyId(T_SEC_DATAKEY_ID * ptstRootKey);

static unsigned long _FUN_RefreshScheme(T_GL_HSCHEME_INTERFACE interfac, T_GL_HWIDGET label);
static unsigned long _FUN_OpenScheme (T_GL_HSCHEME_INTERFACE interfac);
static void _FUN_CloseScheme (T_GL_HSCHEME_INTERFACE interfac);

card outputPINLen;
unsigned char outputData[8];
static T_GL_HGRAPHIC_LIB cryHdlGoal = NULL;

static int StopSchGetKey(void) {
	int iret;
	unsigned char ucdataout;
	unsigned int uiEventToWait = 0;
	int ibToContinue = FALSE;


	iret = SEC_PinEntry (&uiEventToWait,&ucdataout, &ibToContinue);

	return(iret);
}


/** Refresh the pin entry scheme, and update goal display during the process
 * depending on the action performed by the user.
 *
 * \return
 *      - \ref key entered
 */
static unsigned long _FUN_RefreshScheme(T_GL_HSCHEME_INTERFACE interfac, T_GL_HWIDGET label) {
	int ret=GL_RESULT_SUCCESS;
	unsigned int event;
	ST_PIN* pin;
	static char enteredPin[16+1];
	int cont=TRUE;
	unsigned char key = TRUE;
	int dummyPinLength = 12;

	pin = (ST_PIN*)interfac->privateData;                                             // Retrieve pointer on my private data

	//Pin entry code management
	if (pin->state == -1) {
		memset(enteredPin, 0, sizeof(enteredPin));
		pin->state++;
		return ret;
	}
	//Next steps: Enter Pin
	event = 0;                                                                        // Event time-out
	ret = SEC_PinEntry(&event, &key, &cont);

	if (ret == OK) {
		if (key == ECHO_PIN) {
			// Enter pin in progress
			if (pin->state < MAX_PIN_CODE_SIZE) {
				enteredPin[pin->state] = key;
				enteredPin[pin->state+1] = 0;
				GL_Widget_SetText(label, (char*)enteredPin);                                // Display pin code entry
				pin->state++;
				outputPINLen++;
			}
			event=0;                                                                // Event time-out
		} else {
			// Pin confirmation
			switch (key) {
			case 0x00:
				ret=GL_RESULT_INACTIVITY; //Response to stop pin entry by receiving an event
				break;
			case 0x01:
				ret=GL_RESULT_INACTIVITY;
				break;
			case 0x03:
			case T_VAL:
				StopSchGetKey();
				mapPutCard(traOnlinePinLen, outputPINLen);
				ret=GL_KEY_VALID; //Valid key from Pinpad or Terminal when enter pin => cont=FALSE if >= Min pin entry
				break;
			case T_ANN:
				mapPutCard(traOnlinePinLen, dummyPinLength);
				StopSchGetKey();
				ret=GL_KEY_CANCEL;
				break;  //Cancel key from Pinpad or Terminal when enter pin => cont=FALSE
			case T_CORR: //Correction from Pinpad or Terminal when enter pin => cont=TRUE
				if (pin->state != 0) {
					pin->state--;
					enteredPin[pin->state] = ' ';
					enteredPin[pin->state+1] = 0;
					GL_Widget_SetText(label, (char*)enteredPin);// Pin entry code correction
				}
				event=0; // Event time-out
				break;
			default:
				break;
			}
		}
	} else if (ret == ERR_TIMEOUT) {// Pin entry on pinpad and canceled by terminal
		ret = GL_RESULT_INACTIVITY;
	} else { // Pin entry on pinpad already in progress

		ret=GL_RESULT_INACTIVITY;
	}

	return ret;
}

/** Initialise the Pin Entry Scheme
 *
 * \return
 *      - \ref OK if success otherwise error
 */
static unsigned long _FUN_OpenScheme (T_GL_HSCHEME_INTERFACE interfac) {
	int ret;
	ST_PIN* pin;
	T_SEC_ENTRYCONF cfg;
	T_SEC_PARTTYPE SecPart = C_SEC_CIPHERING;
	char CardAID[100];

	pin = (ST_PIN*)interfac->privateData;                                            // Retrieve pointer on my private data

	outputPINLen = 0;
	mapPutCard(traOnlinePinLen, outputPINLen);

	memset(CardAID, 0, sizeof(CardAID));
	mapGet(traAID, CardAID, 1);
//	if (strncmp(CardAID, "A000000333010101", 16) == 0) {
		cfg.ucMinDigits = 4;                                                            // Min pin entry 4 digits
//	}else{
//		cfg.ucMinDigits = 0;                                                            // Min pin entry 0 digits
//	}

	// Pin entry code initialization
	cfg.ucEchoChar = ECHO_PIN;                                                      // '*' echo character
	cfg.ucMaxDigits = 12;                                                           // Max pin entry 12 digits
	if (pin->dly < 60) {
		cfg.iFirstCharTimeOut = pin->dly*1000;                                      // Wait for first digit < 60s
	} else {
		cfg.iFirstCharTimeOut = 60*1000;                                            // Wait for first digit 60s (Max supported)
	}
	cfg.iInterCharTimeOut = 30*1000;                                                // Wait for next digits 10s

	// Secure part relative to the Pin entry function
	ret = SEC_PinEntryInit(&cfg, SecPart); // C_SEC_PINCODE secure part recommended to pin entry (PCI/PED)

	(void)(interfac);
	(void)ret;
	return GL_RESULT_SUCCESS;
}

/** Close the Pin Entry Scheme
 */
static void _FUN_CloseScheme (T_GL_HSCHEME_INTERFACE interfac){
	(void)(interfac);
}


int PAR_KSN_Get(unsigned char * buf) {
	int ret = 0;
	int len;
	char data[100];
	int i;
	char tmp[32];
	char str[32];

	memset(data, 0, sizeof(data));

	MAPGET(appDUKPT_KSN, data, lblKO);

	///------------------------

	memset(buf, 0, sizeof(buf));
	len = hex2bin(buf, data, 0);
	if (len > 0) {
		memset(str, 0, sizeof(str));
		for (i=0; i<len; i++) {
			memset(tmp, 0, sizeof(tmp));
			Telium_Sprintf(tmp, "%02x", buf[i]);
			strcat(str, tmp);
		}
	}

	lblKO:
	return ret;
}


int PAR_PADDED_KSN_Get(unsigned char * buf) {
	int ret = 0;
	int len;
	char data[100];
	int i;
	char tmp[32];
	char str[32];

	memset(data, 0, sizeof(data));

	strcpy(data, "FFFF");

	MAPGET(appDUKPT_KSN, &data[4], lblKO);

	///------------------------

	memset(buf, 0, sizeof(buf));
	len = hex2bin(buf, data, strlen(data)/2);
	if (len > 0) {
		memset(str, 0, sizeof(str));
		for (i=0; i<len; i++) {
			memset(tmp, 0, sizeof(tmp));
			Telium_Sprintf(tmp, "%02x", buf[i]);
			strcat(str, tmp);
		}
	}

	lblKO:
	return ret;
}


int PAR_INITIAL_KEY_Get(unsigned char * buf) {
	int ret = 0;
	int len;
	char data[100];
	int i;
	char tmp[32];
	char str[32];

	memset(data, 0, sizeof(data));

	MAPGET(appDUKPT_IPEK, data, lblKO);

	memset(buf, 0, sizeof(buf));
	len = hex2bin(buf, data, 0);
	if (len > 0) {
		memset(str, 0, sizeof(str));
		for (i=0; i<len; i++) {
			memset(tmp, 0, sizeof(tmp));
			Telium_Sprintf(tmp, "%02x", buf[i]);
			strcat(str, tmp);
		}
	}

	lblKO:
	return ret;
}

void guiStart(void) {
	static T_GL_WCHAR key_1 = '1';
	static T_GL_WCHAR key_7[] = {'7','p','q', 'r', 's', 'P', 'Q', 'R', 'S'};
	static T_GL_WCHAR key_9[] = {'9','w','x', 'y', 'z', 'W', 'X', 'Y', 'Z'};

	static T_GL_WCHAR isc_key_0[] = {'0','*','#', ',', '.'};
	static T_GL_WCHAR isc_key_1[] = {'1','_','q', 'z', 'Q', 'Z'};
	static T_GL_WCHAR isc_key_minus[] = {'-',':','?','@'};
	static T_GL_WCHAR isc_key_default[] = {'.','-',',',':','?','@','!'};

	if(cryHdlGoal==NULL) {
		cryHdlGoal = GL_GraphicLib_Create();
		GL_GraphicLib_SetCharset(cryHdlGoal, GL_ENCODING_UTF8);

		GL_GraphicLib_BindManyKey(cryHdlGoal, GL_KEY_DOT, isc_key_default, 6);

		if (IsIMP350() || IsIMP3xxCompanion() || IsICM122() || IsIPP480()) {
			GL_GraphicLib_BindSingleKey(cryHdlGoal, GL_KEY_1, key_1);
			GL_GraphicLib_BindManyKey(cryHdlGoal, GL_KEY_7, key_7, 9);
			GL_GraphicLib_BindManyKey(cryHdlGoal, GL_KEY_9, key_9, 9);
		}

		if (IsISC250() || IsISC350() || IsISC480()) {
			GL_GraphicLib_BindManyKey(cryHdlGoal, GL_KEY_0, isc_key_0, 5);
			GL_GraphicLib_BindManyKey(cryHdlGoal, GL_KEY_1, isc_key_1, 6);
			GL_GraphicLib_BindManyKey(cryHdlGoal, GL_KEY_DOT, isc_key_minus, 4);
		}
	}
}

void guiStop(void) {
	if(cryHdlGoal!=NULL) {
		GL_GraphicLib_Destroy(cryHdlGoal);
		cryHdlGoal=NULL;
	}
}

//static int guiScreen(char *title, char *text, int icon, int delay) {
//	int ret=0;
//	int button;
//	unsigned long result;
//
//	guiStart();
//
//	button = GL_BUTTON_NONE;
//	switch (icon) {
//	case GL_ICON_INFORMATION:
//	case GL_ICON_WARNING:
//	case GL_ICON_ERROR:
//		button = GL_BUTTON_VALID;
//		break;
//	case GL_ICON_QUESTION:
//		button = GL_BUTTON_VALID_CANCEL;
//		break;
//	default:
//		break;
//	}
//
//	result = GL_Dialog_Message(cryHdlGoal, title, text, icon, button, delay);
//	ret = result;
//	return ret;
//}



int FUN_FreeSecretArea(int idx) {
	int ret;
	T_SEC_DATAKEY_ID keyId;
	T_SEC_PARTTYPE SecPart = C_SEC_CIPHERING;

	switch (idx) {
	case 0: //C_SEC_PINCODE
		//ret = PAR_SecurePartGet(idx, &SecPart);
		keyId.iSecretArea = ID_SCR_XXX_BANK;
		keyId.cAlgoType = TLV_TYPE_KDES; /* This key is a DES Key */
		keyId.usNumber = PIN_MASTER_KEY_NO_XXX;
		keyId.uiBankId = ID_BANK_XXX;
		ret = SEC_FreeSecret(SecPart, &keyId);
		if (ret == OK) {
			keyId.iSecretArea = ID_SCR_XXX_BANK;
			keyId.cAlgoType = TLV_TYPE_KDES; /* This key is a DES Key */
			keyId.usNumber = PIN_KEY_NO_XXX;
			keyId.uiBankId = ID_BANK_XXX;
			ret = SEC_FreeSecret(SecPart, &keyId);
		}
		break;

	case 2: //C_SEC_CIPHERING
		//ret = PAR_SecurePartGet(idx, &SecPart);
		keyId.iSecretArea = ID_SCR_XXX_BANK;
		keyId.cAlgoType = TLV_TYPE_KDES; /* This key is a DES Key */
		keyId.usNumber = MAC_MASTER_KEY_NO_XXX;
		keyId.uiBankId = ID_BANK_XXX;
		ret = SEC_FreeSecret(SecPart, &keyId);
		if (ret == OK) {
			keyId.iSecretArea = ID_SCR_XXX_BANK;
			keyId.cAlgoType = TLV_TYPE_KDES; /* This key is a DES Key */
			keyId.usNumber = MAC_KEY_NO_XXX;
			keyId.uiBankId = ID_BANK_XXX;
			ret = SEC_FreeSecret(SecPart, &keyId);
		}
		break;

	default: //free whole area
		//ret = PAR_SecurePartGet(0, &SecPart);
		keyId.iSecretArea = ID_SCR_XXX_BANK;
		keyId.cAlgoType = 0;
		keyId.usNumber = 0;
		keyId.uiBankId = 0;
		ret = SEC_FreeSecret(SecPart, &keyId);
		break;
	}

	return (ret);
}

/** Set the default values for the Root Key details
 *
 * \return
 *      - \ref OK if success otherwise error
 */
static void _FUN_SetRootKeyId(T_SEC_DATAKEY_ID * ptstRootKey) {
	ptstRootKey->iSecretArea = ID_SCR_XXX_BANK_ROOT;
	ptstRootKey->cAlgoType   = TLV_TYPE_KTDES;   /* This ROOT key is a TDES Key */
	ptstRootKey->usNumber    = KEY_NO_XXX_ROOT;  //KEY_NO_XXX_ROOT;
	ptstRootKey->uiBankId    = ID_BANK_XXX_ROOT;
}

int FUN_LoadRootKey	(int idx) {
	int ret, iUsage;
	T_SEC_PARTTYPE SecPart = C_SEC_CIPHERING;
	T_SEC_DATAKEY_ID key;
	char TheRootKey[(TDES_KEY_SIZE*2)+1];
	byte TheRootKeyBin[TDES_KEY_SIZE+1];

	memset(TheRootKey, 0, sizeof(TheRootKey));
	memset(TheRootKeyBin, 0, sizeof(TheRootKeyBin));

	_FUN_SetRootKeyId(&key);
	iUsage = CIPHERING_KEY; /* Key to cipher PIN keys and MAC keys */

	strcpy(TheRootKey, "6C1E054182422481BE2221780399BB72");
	hex2bin(TheRootKeyBin, TheRootKey, 16);

	//SEC_FreeSecret(SecPart, &key);
	ret = SEC_LoadKey(SecPart, NULL, &key, (unsigned char*) &TheRootKeyBin, iUsage);
	return (ret);
}


int FUN_FreeDukptSecretArea(int idx) {
	int ret;
	int sta;
	T_SEC_DATAKEY_ID keyId;
	T_SEC_PARTTYPE SecPart= C_SEC_CIPHERING;

	switch (idx) {
	case 0: //C_SEC_PINCODE
		///////////---- DUKPT KEY LOCATION
		//ret = PAR_SecurePartGet(idx, &SecPart);
		keyId.iSecretArea = ID_SCR_XXX_BANK;
		keyId.cAlgoType = TLV_TYPE_KTDES; /* This key is a DES Key */
		keyId.usNumber = KEY_NO_XXX;
		keyId.uiBankId = ID_BANK_XXX;

		ret = SEC_DukptStopDelete(SecPart, &keyId);
		if (ret >=0) {
			sta = 0;
			ret = SEC_DukptStatus(SecPart, &keyId, &sta);
			if(sta==0) {
				ret = 0;
			} else {
				ret = -1;
			}
		} else {
			ret = -1;
		}


		///////////---- ROOT KEY LOCATION
		//ret = PAR_SecurePartGet(idx, &SecPart);
		keyId.iSecretArea = ID_SCR_XXX_BANK_ROOT;
		keyId.cAlgoType = TLV_TYPE_KTDES; /* This key is a DES Key */
		keyId.usNumber = KEY_NO_XXX_ROOT;
		keyId.uiBankId = ID_BANK_XXX_ROOT;

		ret = SEC_DukptStopDelete(SecPart, &keyId);
		if (ret >=0) {
			sta = 0;
			ret = SEC_DukptStatus(SecPart, &keyId, &sta);
			if(sta==0) {
				ret = 0;
			} else {
				ret = -1;
			}
		} else {
			ret = -1;
		}
		break;

	default: //free whole area

		///////////---- DUKPT KEY LOCATION
		//ret = PAR_SecurePartGet(0, &SecPart);
		keyId.iSecretArea = ID_SCR_XXX_BANK;
		keyId.cAlgoType = 0;
		keyId.usNumber = 0;
		keyId.uiBankId = 0;
		ret = SEC_FreeSecret(SecPart, &keyId);

		///////////---- ROOT KEY LOCATION
		//ret = PAR_SecurePartGet(0, &SecPart);
		keyId.iSecretArea = ID_SCR_XXX_BANK_ROOT;
		keyId.cAlgoType = 0;
		keyId.usNumber = 0;
		keyId.uiBankId = 0;
		ret = SEC_FreeSecret(SecPart, &keyId);
		break;
	}

	return (ret);
}

int CreateIpek(void) {
	int ret = 0;
	int var = 0;
	byte bdk[32 + 1];
	byte ksn[32 + 1];
	byte ksnMask[32 + 1];
	byte keyMask[32 + 1];
	byte BDK_key[32 + 1]; //BDK
	byte InitialKSN[32 + 1]; //iksn
	byte PaddedKSN[32 + 1]; //Padded KSN
	byte IPEK[32 + 1];
	char IPEKKEY[32 + 1];
	char bdkSTR[32 + 1];

	memset(BDK_key, 0, sizeof(BDK_key));
	memset(ksnMask, 0, sizeof(ksnMask));
	memset(keyMask, 0, sizeof(keyMask));
	memset(IPEKKEY, 0, sizeof(IPEKKEY));
	memset(PaddedKSN, 0, sizeof(PaddedKSN));
	memset(InitialKSN, 0, sizeof(InitialKSN));
	memset(bdkSTR, 0, sizeof(bdkSTR));
	memset(bdk, 0, sizeof(bdk));

	ret = PAR_PADDED_KSN_Get(ksn);
	MAPGET(appDUKPT_BDK, bdkSTR, lblKO);

	hex2bin(bdk, bdkSTR, 16);

	memcpy(ksnMask, "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xE0\x00\x00", 10);
	memcpy(keyMask, "\xC0\xC0\xC0\xC0\x00\x00\x00\x00\xC0\xC0\xC0\xC0\x00\x00\x00\x00", 16);

	for (var = 0; var < 10; var++) {
		// ksn & FFFFFFFFFFFFFFE00000  = iksn
		InitialKSN[var] = ksn[var] & ksnMask[var];
	}

	xor(bdk, keyMask, BDK_key, 8);
	xor(&bdk[8], &keyMask[8], &BDK_key[8], 8);

	memset(IPEKKEY, 0, sizeof(IPEKKEY));
	memset(IPEK, 0, sizeof(IPEK));
	stdEDE(IPEK, InitialKSN, bdk);
	stdEDE(IPEK+8, InitialKSN, BDK_key);

	bin2hex(IPEKKEY, IPEK, 16);

	MAPPUTSTR(appDUKPT_IPEK, IPEKKEY, lblKO);

	return ret;

	lblKO:
	return -1;
	//	return TripleDesEncrypt(bdk, (ksn & KsnMask) >> 16) << 64 | TripleDesEncrypt(bdk ^ KeyMask, (ksn & KsnMask) >> 16)
}

int FUN_LoadKeySerialNumber(void) {
	int ret;
	byte bdk[32 + 1];
	byte InitialKey[64 + 1];
	byte KSN[32 + 1]; // InitSNKey = KET SET ID (987654) + TRMS ID (3210E0) + TRANSACTION COUNTER (0000)
	char bdkSTR[32 + 1];
	T_SEC_DATAKEY_ID stMasterKey, stRootKey;
	T_SEC_PARTTYPE SecPart= C_SEC_CIPHERING;
	byte IPEK_Encrypted[32 + 1];
	unsigned int len = 16;

	memset(IPEK_Encrypted,0, sizeof(IPEK_Encrypted));
	memset(InitialKey,0, sizeof(InitialKey));
	memset(bdkSTR, 0, sizeof(bdkSTR));
	memset(bdk, 0, sizeof(bdk));
	memset(KSN,0, sizeof(KSN));

	stRootKey.iSecretArea = ID_SCR_XXX_BANK_ROOT;
	stRootKey.cAlgoType   = TLV_TYPE_KTDES; /* This ROOT key is a TDES Key */
	stRootKey.usNumber    = KEY_NO_XXX_ROOT;
	stRootKey.uiBankId    = ID_BANK_XXX_ROOT;

	memset(KSN,0, sizeof(KSN));
	PAR_PADDED_KSN_Get(KSN);

	//// manage the initial key by root
	memset(InitialKey,0, sizeof(InitialKey));

	MAPGET(appDUKPT_BDK, bdkSTR, lblKO);
	hex2bin(bdk, bdkSTR, 16);
	DukptGenerateIpek(InitialKey, bdk, KSN);

	ret = SEC_ECBCipher(  //DES/TDES ciphering/deciphering data in ECB mode (Electronic Code Book)
			&stRootKey,  //Key Id of the key to use
			C_SEC_CIPHER_FUNC, //C_SEC_CIPHER_FUNC or C_SEC_DECIPHER_FUNC
			(unsigned char *)InitialKey,   //Data buffer to cipher/decipher
			TDES_KEY_SIZE, //Length in bytes of the Input Data buffer
			IPEK_Encrypted,   //Output Data buffer
			&len   //Length in bytes of the Output Data buffer
	);

	if(len<16) {//Backup step
		stdEDE(IPEK_Encrypted, InitialKey, bdk);
		stdEDE(IPEK_Encrypted+8, InitialKey+8, bdk);
	}


	////---- Now Load the KSN and the Initial Key
	ret = SEC_isSecretArea(SecPart, (SEG_ID) ID_SCR_XXX_BANK);
	if ( ret == OK ) {

		DukptIncrementKsn();

		stMasterKey.iSecretArea = ID_SCR_XXX_BANK;
		stMasterKey.cAlgoType =  TLV_TYPE_TDESDUKPT; /* This key is a TDES Key */
		stMasterKey.usNumber =  KEY_NO_XXX;
		stMasterKey.uiBankId = ID_BANK_XXX;


		ret = SEC_DukptLoadKSN(SecPart, &stMasterKey, KSN);
		if (ret != OK) {
			return -1;
		}

		ret = SEC_DukptLoadInitialKey(SecPart, &stMasterKey, &stRootKey, IPEK_Encrypted);
		if (ret != OK) {
			return -1;
		}

	}
	lblKO:
	return (ret);
}

static void RemoveSpaces(char* source) {
	char* i = source;
	char* j = source;
	while(*j != 0) {
		*i = *j++;
		if(*i != ' ')
			i++;
	}
	*i = 0;
}

/** Obtain the pinblock using DUKPT Pin encryption
 *
 * \param[in] pucAcc Account Number
 * \param[out] pucBlk Generated Pin Block

 * \return
 *      - \ref OK if success otherwise error
 */
static int _FUN_PinEncrypt(const unsigned char *pucAcc, unsigned char *pucBlk) {
	int i;
	int ret;
	char tmp[32];
	char buf[32];
	T_SEC_DATAKEY_ID dscKey;
	unsigned char SerialNumberKey[10+1];  // SNKey return at pin = KET SET ID + TRMS ID + TRANSACTION COUNTER
	T_SEC_PARTTYPE SecPart= C_SEC_CIPHERING;

	dscKey.iSecretArea = ID_SCR_XXX_BANK;
	dscKey.cAlgoType =  TLV_TYPE_TDESDUKPT; /* This key is a TDES Key */
	dscKey.usNumber =  KEY_NO_XXX;
	dscKey.uiBankId = ID_BANK_XXX;

	memset(SerialNumberKey, 0, sizeof(SerialNumberKey));
	ret = SEC_DukptEncryptPin(SecPart, &dscKey, DUKPT_ENCRYPT_PIN, (unsigned char *) pucAcc, &pucBlk[0], &SerialNumberKey[0]);
	memset(buf, 0, sizeof(buf));
	for (i=0; i<10; i++) {
		memset(tmp, 0, sizeof(tmp));
		Telium_Sprintf(tmp, "%02x", SerialNumberKey[i]);
		strcat(buf, tmp);
	}

	RemoveSpaces(buf);
	strtouppercase(buf);

	mapPut(appDUKPT_KSN, &buf[4], (strlen(buf)-4));

	return ret;
}

int FUN_EncryptPin(void) {
	int ret;
	int i;
	int lenP;
	unsigned char acc[8];
	unsigned char blk[8];
	char tmp[32];
	char buf[32];
	byte panBis[17];
	char Pan[19+1];

	memset(panBis, 0, sizeof(panBis));
	memset(Pan, 0, sizeof(Pan));

	//Format PAN
	ret= mapGet(traPan,Pan,19+1); CHECK(ret>=0,lblKO);

	fncPanRemove_F(Pan);

	memcpy(&panBis[0],"0000",4);
	lenP = strlen(Pan);

	memcpy(&panBis[4],&Pan[lenP-13],12);
	hex2bin(acc,(char *)panBis,8);

	// PIN ciphering to get pinblock
	ret = _FUN_PinEncrypt(acc, blk);
	if (ret!=OK) {
		return -1;
	}

	memset(buf, 0, sizeof(buf));
	for (i=0; i<8; i++) {
		memset(tmp, 0, sizeof(tmp));
		Telium_Sprintf(tmp, "%02x", acc[i]);
		strcat(buf, tmp);
	}

	memset(buf, 0, sizeof(buf));
	for (i=0; i<8; i++) {
		memset(tmp, 0, sizeof(tmp));
		Telium_Sprintf(tmp, "%02x", blk[i]);
		strcat(buf, tmp);
	}

	mapPut(traPinBlk, blk, 8);

	lblKO:
	return (ret);
}


int FUN_Initialize_DUKPT(void){
	int ret = 0;

	//		//Delete all secret area
	//	FUN_FreeDukptSecretArea(0);
	//	FUN_FreeDukptSecretArea(1);
	//	FUN_FreeDukptSecretArea(0);
	//	FUN_FreeDukptSecretArea(1);
	//	FUN_FreeDukptSecretArea(0);
	//	FUN_FreeDukptSecretArea(1);
	//	FUN_FreeDukptSecretArea(0);
	//	FUN_FreeDukptSecretArea(1);

	//Load RootKey
	ret = FUN_LoadRootKey(0);

	//Load KSN
	ret = FUN_LoadKeySerialNumber();

	return ret;
}

int FUN_PinEntry(void) {
	int ret;
	unsigned long res;
	T_GL_SCHEME_INTERFACE sch;
	ST_PIN pin;
	char amountFormatted[lenAmt + 1];
	char amount[lenAmt + 1];
	char PIN[1 + lenPIN];
	char MnuStr[5];
	card MnuItem = 0;

	memset(MnuStr, 0, sizeof(MnuStr));

	memset(PIN, 0, sizeof(PIN));
	memset(amount, 0, sizeof(amount));
	memset(amountFormatted, 0, sizeof(amountFormatted));

	MAPGET(traMnuItm, MnuStr, lblKO);
	dec2num(&MnuItem, MnuStr,0);

	//	MAPGET(traAmt, amount, lblKO);
	MAPGET(traTotAmt, amount, lblKO);

	mapGet(traPinBlk, PIN, sizeof(PIN));
	if(PIN[0] != 0){
		ret = TRUE;
		goto lblEnd;
	}

	fmtAmt(amountFormatted, amount, 2, ".,");

	// Check Pinpad
	ret = DLLExist("SECURITY");

	// Enter PIN
	// Initializes interface scheme management
	sch.open     = _FUN_OpenScheme;
	sch.close    = _FUN_CloseScheme;
	sch.refresh  = _FUN_RefreshScheme;

	// Pointer on my private data
	pin.dly = 60;
	pin.state = -1;
	sch.privateData = &pin;

	guiStart();

	// Pin entry
	if((MnuItem == mnuBalanceEnquiry) || (MnuItem == mnuMiniStatement))
		res = GL_Dialog_Scheme(cryHdlGoal, "PIN ENTRY", NULL, "Enter your PIN\nfrom prying eyes", &sch);
	else
		res = GL_Dialog_Scheme(cryHdlGoal, "PIN ENTRY", amountFormatted, "Enter your PIN\nfrom prying eyes", &sch);

	guiStop();

	if (res==GL_KEY_CANCEL) {
		return -1;
	} else if (res==GL_RESULT_INACTIVITY) {
		return -2;
	} else {
		//guiScreen("Dll Security", "Pin is entered", GL_ICON_NONE, 3000);
	}


	FUN_EncryptPin();

	mapPut(traCVMused, "ONL", 3);

	lblKO:
	(void)ret;

	lblEnd:
	return OK;
}
