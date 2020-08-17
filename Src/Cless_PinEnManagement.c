//! \file
//! Module that manages Encipher online PIN.
//
/////////////////////////////////////////////////////////////////
#include "Cless_Implementation.h"
#include "Globals.h"
#include "schVar_def.h"


//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

/* ======================================================================== */
/* Include Files                                                            */
/* ======================================================================== */

//! \addtogroup Group_cu_pin
//! @{

/* ======================================================================== */
/* Macros & preprocessor definitions                                        */
/* ======================================================================== */
#define  ISO9564_F0_DES                  1


/******************************************************/
/*                VAR PARAMETERS TO SET               */
/******************************************************/
/*!       
 *For your tests : Initialise with your Card Number !!!!

 * - \a  1)	ID_SCR_XXX_BANK  4 bytes to define
 * - \a  2)	ROOT_KEY_NO_XXX  1 byte to define
 * - \a  3)	PIN_KEY_NO_XXX   1 byte : your choice, in the range of [1,F]
 */

/* SECRET AREA ID TO USE */
#define ID_SCR_XXX_BANK				0x00012030	/*!<  4 bytes to define depending on VAR_NUMBER_SIGN. */ 

/* ROOT KEY NUMBER = */
#define ROOT_KEY_NO_XXX				0x00  		/*!< Root Key number: must be multiple of 4 (from 0 through 1023). */
/*!< This number is the last byte of the KEY id used to load first key is secret area */

/* PIN  KEY NUMBER */
#define PIN_KEY_NO_XXX				8			/*!<  ISO9564 Pin Bloc scheme uses  a key always located at offset 8. */

/* BANK ID TO USE */
#define ID_BANK_XXX					BK_SAGEM	/*!<  Bank ID is 4 bytes including 2 zeros LSB and 2 MSB from 0x8000 to 0xFFFF. */


/* ========================================================================	*/
/* Global variables                                                         */
/* ========================================================================	*/
/*! "TBD" You should modify this value. */ 
const unsigned char G_c_uacTheRootKey			[DES_KEY_SIZE+1]	= {0x0D,0xA0,0x61,0x56,0xD0,0x95,0x94,0xC3};

/* ========================================================================	*/
/* Static function definitions                                              */
/* ========================================================================	*/
static int _Cless_LoadRootKey(void);

/* ======================================================================== */
/* Functions                                                                */
/* ======================================================================== */
/* ========================================================================	*/
/* Global variables                                                         */
/* ========================================================================	*/
/* Definitions of secure parts */


#if 0
//! \brief This function can be used to erase the secret area.
//! \return
//!		- \ref OK (0) is successful, otherwise error.
static int _Cless_FreeSecretArea(void)
{
	int L_iRet;

	T_SEC_DATAKEY_ID stKeyId;

	GTL_Traces_TraceDebug("_Cless_FreeSecretArea");

	/* To free Whole secret area */
	stKeyId.iSecretArea = ID_SCR_XXX_BANK;
	stKeyId.cAlgoType	= 0;
	stKeyId.usNumber	= 0;
	stKeyId.uiBankId	= 0;
	GTL_Traces_TraceDebug("SEC_FreeSecret ...");
	L_iRet = SEC_FreeSecret (C_SEC_PINCODE, &stKeyId);

	if(L_iRet == OK)
	{
		// Display, Trace and Print an error message
		GTL_Traces_TraceDebug("SEC_FreeSecretArea ok");
	}
	else 
	{ 
		// Display, Trace and Print an error message
		GTL_Traces_TraceDebug("SEC_FreeSecretArea error : %x",L_iRet); 
	}

	return L_iRet; 
}
#endif // 0

//! \brief Load Root Key.
//! \return
//!		- \ref OK (0) is successful, otherwise error.
static int _Cless_LoadRootKey(void)
{
	int L_iRet, L_iUsage;
	T_SEC_DATAKEY_ID L_stRootKey;
	T_SEC_DATAKEY_ID stCipherKey;

	// First key allready loaded in terminal
	stCipherKey.iSecretArea = ID_SCR_XXX_BANK;
	stCipherKey.cAlgoType	= TLV_TYPE_KDES;         // This key is a DES Key
	stCipherKey.usNumber	= ROOT_KEY_NO_XXX << 2;  // Warning, it's the end of the Key Id shift by 2
	stCipherKey.uiBankId	= ID_BANK_XXX;

	// Key to load that will be used for PIN encryption
	L_stRootKey.iSecretArea = ID_SCR_XXX_BANK;
	L_stRootKey.cAlgoType   = TLV_TYPE_KDES;        // This key is a DES Key
	L_stRootKey.usNumber    = PIN_KEY_NO_XXX;
	L_stRootKey.uiBankId    = ID_BANK_XXX;
	L_iUsage = CIPHERING_PIN; 					    // Key to cipher PIN

	// Warning, if a reset occurs at this step, a Scheme or secret area access has occurred
	// => check that you application his sign with same card than application used by create and load first key
	// => check embeded schemes (signed or not signed) used

	// To be compliant !!!!!!!!!!!!!!!!
	L_iRet = SEC_FreeSecret (C_SEC_PINCODE, &L_stRootKey);
	if(L_iRet != OK)
	{ 
		// Display, Trace and Print an error message
		GTL_Traces_TraceDebug("SEC_FreeSecret error : %x",L_iRet);
	}


	L_iRet = SEC_LoadKey (C_SEC_PINCODE, &stCipherKey, &L_stRootKey, (unsigned char*) &G_c_uacTheRootKey, L_iUsage); 

	if(L_iRet != OK)
	{ 
		// Display, Trace and Print an error message
		GTL_Traces_TraceDebug("SEC_LoadKey error : %x",L_iRet);
	}
	return L_iRet; 
}

//! \brief Inject Key : Check if secret area exist and Load a key.
//! \return
//!		- \ref OK (0) is successful, otherwise error.
int Cless_InjectKey (void) {
	int iret;
	unsigned char str[50];

	iret = SEC_isSecretArea(C_SEC_PINCODE, (SEG_ID) ID_SCR_XXX_BANK);

	if (iret != OK) {
		GTL_Traces_TraceDebug("SEC_isSecretArea error : %x", iret);
		GTL_Traces_TraceDebug("WARNING : secret AREA and SEC_FirstLoadKey must have been made by external tool");
		GTL_Traces_TraceDebug("please, use SKMT or specific application to do it !!!");

		Telium_Sprintf((char*)str, "SEC_isSecretArea %d\n",iret);
		upload_diagnostic_txt(str);
	} else {
		iret = _Cless_LoadRootKey();

		if (iret != OK) {
			Telium_Sprintf((char*)str, "_Cless_LoadRootKey %d\n",iret);
			upload_diagnostic_txt(str);
		}
	}
	return iret;
}


//! \brief Encypher a key previously inputed with Scheme method.
//! \param[in] card pan.
//! \param[in] Enciphered Pin.
//! \param[in] Enciphered Pin length.
//! \return
//!		- \ref TRUE Pin has been corectly encyphered.
//!		- \ref FALSE else.
int Cless_OnlineEncipher (void) {
	int ret = 0;

#ifndef __TEST_VERSION__

	mapPut(traCVMused, "ONL", 3);

	ret = FUN_EncryptPin();
	if (ret < 0) {
		return FALSE;
	}

#endif

	return TRUE;
}

