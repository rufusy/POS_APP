/**
 * \file Cless_PinManagement.c
 * \brief This module contains the functions that perform the entry of the PIN code.
 * The PIN code can be entered directly on the terminal, or on a pinpad.
 * It is kept in the booster.
 * In case of enciphered PIN for online presentation, this module also
 * encipher the PIN code, and returns the result.
 *
 * \author Ingenico France
 * \author Copyright (c) 2009 Ingenico France, 1 rue claude Chappe,\n
 * 07503 Guilherand-Granges, France, All Rights Reserved.
 *
 * \author Ingenico France has intellectual property rights relating to the technology embodied\n
 * in this software. In particular, and without limitation, these intellectual property rights may\n
 * include one or more patents.\n
 * This software is distributed under licenses restricting its use, copying, distribution, and\n
 * and decompilation. No part of this software may be reproduced in any form by any means\n
 * without prior written authorization of Ingenico France.
 **/

/* ======================================================================== */
/* Include Files                                                            */
/* ======================================================================== */
#include "Cless_Implementation.h"
#include "Globals.h"

//! \addtogroup Group_cu_pin
//! @{

/* ======================================================================== */
/* Macros & preprocessor definitions                                        */
/* ======================================================================== */
typedef enum PIN_RESULT
{
	C_PIN_INPUTED = 0,
	C_PIN_NO_INPUTED,
	C_PIN_NO_INPUTED_RECALL,
	C_PIN_CANCELED,
} PIN_RESULT;



#define CU_INPUT_PIN_OK               0

/* Type of PinPad witch must be tested to display text */
#define CU_PPTEST_NOTEST              0
#define CU_PPTEST_CARDONPPAD          1
#define CU_PPTEST_ISPINPAD            2

// Constant used for the DISPLAY peripheral

#define PINPAD						1<< 14

#define C_DEFAULT_TIMEOUT_BEFORE_PIN_INPUT				30000  // 30 seconds
#define C_DEFAULT_TIMEOUT_BETWEEN_PIN_DIGIT_INPUT		10000  // 10 seconds

/* ========================================================================	*/
/* Global variables                                                         */
/* ========================================================================	*/
///////////////////////////////////////////////////////////////////////////////////////////
// Constant variables

static unsigned short * ptr_Bypass_Required;
static PIN_TXT_STRUCT PIN_UserInterfaceStructure;
static T_Bool isPinpad = B_NON_INIT;

int PinpadValue;
unsigned long ul_InputAmount = 0;

///extern MSGinfos CurrentMSG;


/* ========================================================================	*/
/* Static function definitions                                              */
/* ========================================================================	*/
// Service to manage Display
static int _Cless_DisplayEnterPinMsg(int nb_try, int *PtrUsePinpad, int nCardHolderLang, char* pGoalText, char* pGoalHelp);
// Services to manage Schemes
static int  _Cless_SetPinEntry ( short TO_inter_car, short TO_first_car,unsigned int * ihid_key);
static unsigned char _Cless_EnterPin(int nb_try,BUFFER_SAISIE *buffer, int nCardHolderLang);
static PIN_RESULT _Cless_SchGetKey(  T_CUTERMkey *xp_key,unsigned int ihid_key);
static unsigned char _Cless_ManageKeys(BUFFER_SAISIE *buffer,int isPinpad);
static PIN_RESULT _Cless_StopSchGetKey(void);
static void _Cless_SetConfPinpad (int * PinpadValue);
static unsigned char _Cless_EnterPinOnLine(unsigned char nb_try,
		BUFFER_SAISIE *buffer,
		unsigned char * ptrPrimaryAccountNumber,
		unsigned short * ptrBypass,
		int nCardHolderLang);
static void _Cless_SetPinUserInterfaceParameters (unsigned long ulTimeoutBefore, unsigned long ulTimeoutInterCharacter);
//char Cless_UseGraphicalFunctionOnPPAD(void);

static unsigned char _Cless_ManageKeysGoal(BUFFER_SAISIE *buffer, const char* text, const char* help);

/* ======================================================================== */
/* Functions                                                                */
/* ======================================================================== */


//===========================================================================
//! \brief Enter pin on-line.
//! \param[in] nb_try : number of PIN entry attempts.
//! \param[out] buffer : buffer containing the cyphered PIN code.
//! \param[in] pan card PAN.
//! \param[in] nCardHolderLang : client lang
//! \param[out] bypass to make, pin code has been finished by a VALID key press.
//! \return
//!		- \ref entre pin state.
//===========================================================================
static unsigned char _Cless_EnterPinOnLine(unsigned char nb_try, BUFFER_SAISIE *buffer, unsigned char * ptrPrimaryAccountNumber, unsigned short * ptrBypass, int nCardHolderLang) {
	int previousHeader;
	unsigned char cr_enter_pin;
	int iRes = 0, iLenEnc = 0;
	card outputPINLen = 0;
	char CardAID[100];
#ifndef __TEST_VERSION__
	byte PIN[1 + lenPinBlk];

	mapGet(traPinBlk, PIN, sizeof(PIN));
	if(PIN[0] != 0){
		cr_enter_pin = INPUT_PIN_ON;
		goto lblEnd;
	}
#endif

	// Header management / display on X07
	previousHeader = StateHeader (0);

	RenterPin:

	// Global variable affectation for later use
	ptr_Bypass_Required = ptrBypass;
	*ptr_Bypass_Required = TRUE; // Delault value, no bypass needed

	// Inject a dummy secret key in the booster
	_Cless_SetConfPinpad (&PinpadValue); 

	cr_enter_pin = _Cless_EnterPin(nb_try, buffer, nCardHolderLang);
	if ( cr_enter_pin == CU_INPUT_PIN_OK ) {
		mapGetCard(traOnlinePinLen, outputPINLen);

		// Encipher the entered PIN
		iRes = Cless_OnlineEncipher();
		if (iRes) {
			cr_enter_pin = INPUT_PIN_ON;
			buffer->nombre = (char) iLenEnc;
		} else if (outputPINLen == 0) {

			memset(CardAID, 0, sizeof(CardAID));
			mapGet(traAID, CardAID, 1);
			if (strncmp(CardAID, "A000000333010101", 16) == 0) {
			}
			goto RenterPin;

			cr_enter_pin = INPUT_PIN_ON;
			buffer->nombre = (char) iLenEnc;
		} else {
			// cr_enter_pin = PP_HS;
			cr_enter_pin = TO_INPUT;
		}
	}

	// Header management / display on X07
	StateHeader (previousHeader);

#ifndef __TEST_VERSION__
	lblEnd:
#endif

	return(cr_enter_pin);
}

//===========================================================================
//! \brief This function tests if graphical functions can be used on PPAD.
//! \return
//!		- \ref TRUE : graphical functions can be used on PPAD.
//!		- \ref FALSE : graphical functions cannot be used on PPAD.
//===========================================================================
char Cless_UseGraphicalFunctionOnPPAD(void) {
	// On terminal, we can use graphical function with long text
	/*unsigned int cr;
	// Default value : no use of pinpad
	char ret = FALSE;*/

#ifndef __TELIUM3__
	return (TPass_GetCustomerDisplay (NULL, 0) != -1);
#else
	return 0;
#endif

	/*cr = PSQ_Pinpad_Type();

	switch (cr)
	{
	case PP30_PPAD:
	case pinpad_pp30s:
	case IPP220_PPAD:
	case IPP250_PPAD:
		ret = FALSE;
		break;
	default :
		ret = TRUE;
		break;
	}
	return (ret);*/
}


//===========================================================================
//! \brief This function displays the amount message to the user.
//! \param[in] int nLedState       : LED Management State.
//! \param[in] int nCardHolderLang : card holder lang.
//! \return allways TRUE
//===========================================================================
/*int Cless_DisplayAmountMsg(int nLedState, int nCardHolderLang)
{
	int interfaceok;
	char buffer[40];
	MSGinfos tMSG;
	MSGinfos tMSGAmount;

	// Local variables initialisation
	interfaceok = FALSE;

	/////////////////////////////////////////////////////////
	// Set the amount message
	Cless_Term_Read_Message(STD_MESS_AMOUNT, nCardHolderLang, &tMSG);

//	if (TPass_GetDeviceType() == TPASS_CLESS_TYPE_IPP2XX)
        if ((TPass_GetDeviceType() == TPASS_CLESS_TYPE_IPP2XX) && (!Cless_UseGraphicalFunctionOnPPAD()))
	{
		if ((ul_InputAmount % 100) != 0)
		    Telium_Sprintf(buffer, "%s%lu.%02lu %s", tMSG.message, ul_InputAmount/100, ul_InputAmount%100, Cless_Fill_GiveMoneyLabel());
		else
		    Telium_Sprintf(buffer, "%s%lu %s", tMSG.message, ul_InputAmount/100, Cless_Fill_GiveMoneyLabel());
	}
	else
	{
		if ((ul_InputAmount % 100) != 0)
		    Telium_Sprintf(buffer, "%s%lu.%02lu %s\n", tMSG.message, ul_InputAmount/100, ul_InputAmount%100, Cless_Fill_GiveMoneyLabel());
		else
		    Telium_Sprintf(buffer, "%s%lu %s\n", tMSG.message, ul_InputAmount/100, Cless_Fill_GiveMoneyLabel());
	}

	tMSGAmount.coding = tMSG.coding;
	tMSGAmount.file = tMSG.file;
	tMSGAmount.message = buffer;

	Helper_DisplayPinText(ERASE, 0, &tMSGAmount, Cless_ALIGN_CENTER, nLedState);

	return (TRUE);
}*/


//===========================================================================
//! \brief This function displays the enter pin message to the user.
//! \param[in] nb_try : number of PIN entry attempts.
//! \param[in] nCardHolderLang : client lang
//! if the pinpad is here but out of service.
//! \param[out] PtrUsePinpad : boolean indicating if the pinpad can be used.
//! \return
//! - TRUE if a problem exists with the user interface (pinpad access ko).
//! - FALSE otherwise.
//===========================================================================
static int _Cless_DisplayEnterPinMsg(int nb_try, int *PtrUsePinpad, int nCardHolderLang, char* pGoalText, char* pGoalHelp)
{
	char buffer[40];
	char * pc_l_line1 = NULL;
	char * pc_l_line2 = NULL;
	char * pc_l_line3 = NULL;
	char * pc_l_line4 = NULL;
	MSGinfos tMSG;
	MSGinfos tMSGAmount;
	MSGinfos tMSGPin;
	MSGinfos tMSGSafe1;
	MSGinfos tMSGSafe2;

	if (pGoalText != NULL)
		*pGoalText = 0;
	if (pGoalHelp != NULL)
		*pGoalHelp = 0;

	// Local variables initialisation
	*PtrUsePinpad = FALSE;


	// Check If PinPad is present

	// If a pinpad is declared
	*PtrUsePinpad = (int)Cless_IsPinpadPresent();

	/////////////////////////////////////////////////////////
	// Set the amount message
	Cless_Term_Read_Message(STD_MESS_AMOUNT, nCardHolderLang, &tMSG);

	///if (TPass_GetDeviceType() == TPASS_CLESS_TYPE_IPP2XX) // Two lines display
	if ((TPass_GetDeviceType() == TPASS_CLESS_TYPE_IPP2XX) && (!Cless_UseGraphicalFunctionOnPPAD()))
	{
		if ((ul_InputAmount % 100) != 0)
			Telium_Sprintf(buffer, "%s%lu.%02lu %s", tMSG.message, ul_InputAmount/100, ul_InputAmount%100, Cless_Fill_GiveMoneyLabel());
		else
			Telium_Sprintf(buffer, "%s%lu %s", tMSG.message, ul_InputAmount/100, Cless_Fill_GiveMoneyLabel());
	} else {
		if ((ul_InputAmount % 100) != 0)
			Telium_Sprintf(buffer, "%s%lu.%02lu %s\n", tMSG.message, ul_InputAmount/100, ul_InputAmount%100, Cless_Fill_GiveMoneyLabel());
		else
			Telium_Sprintf(buffer, "%s%lu %s\n", tMSG.message, ul_InputAmount/100, Cless_Fill_GiveMoneyLabel());
	}

	tMSGAmount.coding = tMSG.coding;
	tMSGAmount.file = tMSG.file;
	tMSGAmount.message = buffer;
	pc_l_line1 = tMSGAmount.message;

#ifndef DISABLE_OTHERS_KERNELS
	if(!Cless_VisaWave_IsVisaAsiaGuiMode())
#endif
		Helper_DisplayPinText(ERASE, 0, &tMSGAmount, Cless_ALIGN_CENTER, LEDSOFF);
#ifndef DISABLE_OTHERS_KERNELS
	else
		Helper_DisplayPinText(ERASE, 0, &tMSGAmount, Cless_ALIGN_CENTER, NOLEDSOFF);
#endif
	if (nb_try == 1) {
		/////////////////////////////////////////////////////////
		// Set the Pin entry message
		// message = ENTER PIN
#ifndef DISABLE_OTHERS_KERNELS
		if(!Cless_VisaWave_IsVisaAsiaGuiMode())
#endif
			Cless_Term_Read_Message(STD_MESS_ENTER_PIN, nCardHolderLang, &tMSGPin);
#ifndef DISABLE_OTHERS_KERNELS
		else
			Cless_Term_Read_Message(VISAWAVE_MESS_ENTER_PIN, nCardHolderLang, &tMSGPin);
#endif
	} else {
		/////////////////////////////////////////////////////////
		// Set the retry message
		// message = TRY AGAIN
		Cless_Term_Read_Message(STD_MESS_TRY_AGAIN, nCardHolderLang, &tMSGPin);
	}
	Helper_DisplayPinText(NO_ERASE, 1, &tMSGPin, Cless_ALIGN_CENTER, NOLEDSOFF);
	pc_l_line2 = tMSGPin.message;


	/////////////////////////////////////////////////////////
	// Set the protect message
	Cless_Term_Read_Message(STD_MESS_ENTER_PIN, nCardHolderLang, &tMSGSafe1);
	Helper_DisplayPinTextLong(NO_ERASE, 2, &tMSGSafe1, Cless_ALIGN_CENTER, NOLEDSOFF);
	pc_l_line3 = tMSGSafe1.message;

	Cless_Term_Read_Message(STD_MESS_SAFE, nCardHolderLang, &tMSGSafe2);
	Helper_DisplayPinTextLong(NO_ERASE, 3, &tMSGSafe2, Cless_ALIGN_CENTER, NOLEDSOFF);
	pc_l_line4 = tMSGSafe2.message;

	if (!Cless_Goal_IsAvailable()) {
		Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
	} else if (*PtrUsePinpad == FALSE) { // If Pin isn't inputed on Pinpad

		if (pGoalText != NULL) {
			strcpy(pGoalText, pc_l_line1);
			strcat(pGoalText, "\n");
			strcat(pGoalText, pc_l_line2);
		}
		if (pGoalHelp != NULL) {
			strcpy(pGoalHelp, pc_l_line3);
			strcat(pGoalHelp, "\n");
			strcat(pGoalHelp, pc_l_line4);
		}
		//Cless_Goal_DisplayEnterPin (pc_l_line1, pc_l_line2, pc_l_line3, pc_l_line4);
	}

	return (TRUE);
}

//===========================================================================
//! \brief This function performs the PIN code entry.
//! This function must select cases where each key may be managed or
//! only final result with OK, CANCEL, CORRECT, TIME_OUT.
//! \param[in] nb_try : number of PIN entry attempts.
//! \param[out] buffer : buffer containing the cyphered PIN code.
//! \param[in] nCardHolderLang : client lang
//! if the pinpad is here but out of service.
//! \return The encipherement result :
//! - PP_HS : Problem with PINPAD, or with the booster.
//! - CU_INPUT_PIN_OK : PIN entry and encipherement was successfull.
//! - CANCEL_INPUT : PIN entry was canceled.
//! - TO_INPUT : Time-out during PIN entry.
//===========================================================================
static unsigned char _Cless_EnterPin(int nb_try, BUFFER_SAISIE *buffer, int nCardHolderLang) {
	unsigned char cr_enter_pin;
	int           PinPadUsed, pinpadok;
	char goalText[512];
	char goalHelp[512];

	pinpadok = _Cless_DisplayEnterPinMsg(nb_try, &PinPadUsed, nCardHolderLang, goalText, goalHelp);

	if (pinpadok != TRUE)
		// Error management
		return (PP_HS);


	if ((Cless_Goal_IsAvailable()) && (!PinPadUsed))
		cr_enter_pin = _Cless_ManageKeysGoal(buffer, goalText, goalHelp);
	else
		cr_enter_pin = _Cless_ManageKeys(buffer, PinPadUsed);	// pinpad must be used if present

	return(cr_enter_pin);
}

static unsigned char _Cless_ManageKeysGoal(BUFFER_SAISIE *buffer, const char* text, const char* help) {
	int pinLength;
	int result;

	memset(buffer, 0, sizeof(BUFFER_SAISIE));
	switch(Cless_Goal_PinEntry("", text, help, &pinLength)) {
	case PINENTRY_SUCCESS:
		buffer->nombre = pinLength;
		memset(buffer->donnees, '*', pinLength);
		buffer->donnees[pinLength] = '\0';
		result = CU_INPUT_PIN_OK;
		break;
	case PINENTRY_BYPASS:
		*ptr_Bypass_Required = TRUE;
		result = CU_INPUT_PIN_OK;
		break;
	case PINENTRY_EVENT:
	case PINENTRY_CANCEL:
		result = CANCEL_INPUT;
		break;
	case PINENTRY_TIMEOUT:
		result = TO_INPUT;
		break;
	case PINENTRY_INVALID:
	case PINENTRY_ERROR:
	default:
		result = PP_HS;
		break;
	}

	return result;
}

//===========================================================================
//! \brief This function manages each key input.
//! \param[out] buffer : buffer to store keys.
//! \param[in] isPinpad : boolean indicating if pinpad can be used.
//! \return The encipherement result : 
//! - PP_HS : Problem with the pinpad, or with the booster.
//! - CU_INPUT_PIN_OK : PIN entry and encipherement was successfull.
//! - CANCEL_INPUT : PIN entry was canceled.
//! - TO_INPUT : Time-out during PIN entry.
//===========================================================================
static unsigned char _Cless_ManageKeys(BUFFER_SAISIE *buffer, int isPinpad)
{
	unsigned char  text[40];
	unsigned char  end_enter_pin;
	unsigned char  cr_enter_pin;
	int            indice;
	int            firstCar, column;
	unsigned int   ihid_key;
	MSGinfos       MSGTmp;
	T_CUTERMkey    key = 0;
	PIN_RESULT     cr;

	// Local variables initialisation
	end_enter_pin = FALSE;
	indice        = 0;
	buffer->nombre= 0;
	firstCar      = 0;
	cr_enter_pin  = CANCEL_INPUT;
	column        = 4;

	// SEC_PinEntryInit ...
	if (_Cless_SetPinEntry(PIN_UserInterfaceStructure.ulTimeoutInter, PIN_UserInterfaceStructure.ulTimeoutBefore, &ihid_key) != OK)
		return (CANCEL_INPUT); // A problem occurs

	MSGTmp.coding = _ISO8859_;
	MSGTmp.file = GetCurrentFont();

	while(!end_enter_pin)
	{
		cr = _Cless_SchGetKey( &key, ihid_key);
		if ( cr == C_PIN_NO_INPUTED)
		{
			cr_enter_pin  = TO_INPUT;
			end_enter_pin = TRUE;
		}
		else if ( cr == C_PIN_CANCELED)
		{
			cr_enter_pin  = CANCEL_INPUT;
			end_enter_pin = TRUE;
		}
		else if ( cr == C_PIN_INPUTED)
		{
			// Key has been typed
			if ( firstCar==0 )
			{
				firstCar=1;
			}

			switch (key)
			{
			case CU_VALID : 
				// end of the enter pin
				if (indice == 0)
				{
					*ptr_Bypass_Required = TRUE;
				}
				buffer->nombre=indice;
				cr_enter_pin  = CU_INPUT_PIN_OK;
				end_enter_pin = TRUE;
				break;

			case CANCEL   : 
				// cancel the enter pin
				buffer->nombre=0;
				cr_enter_pin  = CANCEL_INPUT;
				end_enter_pin = TRUE;
				break;

			case CORRECT:
				// correction
				if (indice!=0)
				{
					column = column-1;
					if ((isPinpad == B_TRUE) && (TPass_GetDeviceType() == TPASS_CLESS_TYPE_IPP2XX) && (!Cless_UseGraphicalFunctionOnPPAD()))
					{
						int index;

						memset( (char *)text, 0, 20 );
						PPS_clearline ();

						for (index = 0; index < indice - 1; index++)
						{
							strcat ((char *)text, "*");
						}
						PPS_Display ( (char *)text);
					}
					else
					{
						int index;

						strcpy((char *)text, " ");
						strcpy((char*)text, "   ");
						for (index = 4; index<column; index++)
							strcat((char*)text, "*");
						strcat((char*)text, " ");
						MSGTmp.message = (char*)text;

						Helper_DisplayPinText(NO_ERASE, 1, &MSGTmp, Cless_ALIGN_LEFT, NOLEDSOFF);
						Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
					}
					indice = indice-1;
				}
				break;

			default:
				// correction
				// Note : Numeric key sent by shemes is always 0x30 for input key 0 to 9
				if ( (key<=0x39) && (key>=0x30) && (indice < 13) )
				{
					if (indice ==0)
					{
						strcpy((char *)text, "                   ");
						if ((isPinpad == B_TRUE) && (TPass_GetDeviceType() == TPASS_CLESS_TYPE_IPP2XX) && (!Cless_UseGraphicalFunctionOnPPAD()))
						{
							PPS_clearline ();
						}
						else
						{
							MSGTmp.message = (char*)text;
							// If CGUI isn't managed
							Helper_DisplayPinText(NO_ERASE, 1, &MSGTmp, Cless_ALIGN_LEFT, NOLEDSOFF);
							Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
						}
						column = 4;
					}
					strcpy((char *)text, "*");

					if ((isPinpad == B_TRUE) && (TPass_GetDeviceType() == TPASS_CLESS_TYPE_IPP2XX) && (!Cless_UseGraphicalFunctionOnPPAD()))
					{
						PPS_Display ( (char *)text);
					}
					else
					{
						int index;

						strcpy((char*)text, "   *");
						for (index = 4; index<column; index++)
							strcat((char*)text, "*");
						strcat((char*)text, " ");

						MSGTmp.message = (char*)text;

						Helper_DisplayPinText(NO_ERASE, 1, &MSGTmp, Cless_ALIGN_LEFT, NOLEDSOFF);
						Helper_RefreshScreen(NOWAIT, HELPERS_CUSTOMER_SCREEN);
					}

					buffer->donnees[indice++] = (unsigned char) key;
					column ++;
				}
				break;
			}
		}   //End Else Key has been inputed */
		else if (cr == C_PIN_NO_INPUTED_RECALL)
		{
		}
	} // end while(!end_enter_pin)

	return (cr_enter_pin);
}

//===========================================================================
//! \brief This function reads a byte on the IAPP link coming from the input PIN scheme.
//! \li Thunder Booster
//! \li ------- -------
//! \li     <--- 'x' several    times
//! \li      ...
//! \li     <--- 'y'
//! \li 'x' is the typed key : K_HID_NUM for numeric key, K_CORRECT for CORRECT key.
//! \li 'y' is the last typed key : K_VALID for VALID key, K_CANCEL for CANCEL key, K_TIMEOUT for TimeOut.
//! \param xp_key : the read byte K_HID_NUM, K_CORRECT, K_VALID, K_CANCEL, or K_TIMEOUT.
//! \param ihid_key : character displayed to hide real character entered.
//! \return
//! - B_NON_INIT key not inputed but fonction release allows to manage html browser : this function must be called back later.
//! - TRUE a key has been read.
//! - FALSE timeout expired or K_TIMEOUT has been read.
//===========================================================================
static PIN_RESULT _Cless_SchGetKey(  T_CUTERMkey *xp_key,unsigned int ihid_key)
{
	int iret;
	unsigned int Key; 
	unsigned char ucdataout;
	unsigned int uiEventToWait = 0;
	int ibToContinue;
	PIN_RESULT res_key;


	// Initialisation
	res_key = C_PIN_INPUTED; // Default value
	ibToContinue = TRUE;

	// Warning, if a pinPad is present, KEYBOARD event is allowed to stop the SEC_PinEntry execution by a key pressed on the terminal keyboard
	if (Cless_IsPinpadPresent())
		uiEventToWait = KEYBOARD;

	// USER_EVENT_PIN is given by periodic task. This event allows to suspend the SEC_PinEntry scheme execution.
	uiEventToWait |= USER_EVENT_PIN; // USER_EVENT_PIN is given by periodic task


	iret = SEC_PinEntry (&uiEventToWait,&ucdataout, &ibToContinue);


	Key = ucdataout;
	if (iret == OK)
	{
		if (Key == ihid_key)
		{
			// This is a numeric key
			*xp_key = ZERO;
		}
		else
			switch (Key) {
			case 0x00:
				res_key = C_PIN_NO_INPUTED;
				GTL_Traces_TraceDebug("_Cless_SchGetKey : Time out\n");
				break;
			case 0x01:
				GTL_Traces_TraceDebug("_Cless_SchGetKey : PIN CANCEL\n");	//
				res_key = C_PIN_NO_INPUTED;
				break;
			case T_VAL : // Nominal case : no trace
				*xp_key = CU_VALID;
				GTL_Traces_TraceDebug("_Cless_SchGetKey : PIN valid)\n");
				break;
			case T_ANN:
				*xp_key = CANCEL;
				GTL_Traces_TraceDebug("_Cless_SchGetKey : PIN Annulation\n");
				break;
			case T_CORR:
				*xp_key = CORRECT;
				GTL_Traces_TraceDebug("_Cless_SchGetKey : PIN Correction\n");
				break;
			}
	}
	else if (iret == ERR_TIMEOUT)
	{
		if ((uiEventToWait & USER_EVENT_PIN) != 0) // No information inputed and no time-out, it's an event used to suspend SEC_PinEntry temporary
		{
			GTL_Traces_TraceDebug("_Cless_SchGetKey : release");
			res_key = C_PIN_NO_INPUTED_RECALL; // Please, call back this function later
		}
		if ((uiEventToWait & KEYBOARD) != 0) // Keyboard event (comming from terminal when ppinpad is used)
		{
			Key = Telium_Getchar(); // Which key ?

			GTL_Traces_TraceDebug("_Cless_SchGetKey : keyboard evt %x", Key);
			if (Key == T_ANN) // If key is cancel => stop PinPad Pin input
			{
				res_key = C_PIN_CANCELED; // Pin input stopped by Cancel key pressed on terminal
				_Cless_StopSchGetKey(); // To stop SEC_PinEntry scheme
			}
			else
			{
				// Key not managed
				res_key = C_PIN_NO_INPUTED_RECALL; // Please, call back this function later
			}
		}
		if (uiEventToWait == 0)
		{
			GTL_Traces_TraceDebug("_Cless_SchGetKey : Time out %x\n", uiEventToWait);
			res_key = C_PIN_NO_INPUTED;
		}
	}
	else
	{
		GTL_Traces_TraceDebug("_Cless_SchGetKey : Errors detected\n"); 
		res_key = C_PIN_NO_INPUTED;
	}

	return(res_key);
}

//===========================================================================
//! \brief This function stop SEC_PinEntry scheme.
//! \return
//! - B_NON_INIT key not inputed but fonction release allows to manage html browser : this function must be called back later.
//! - TRUE a key has been read.
//! - FALSE timeout expired or K_TIMEOUT has been read.
//===========================================================================
static PIN_RESULT _Cless_StopSchGetKey(void)
{
	int iret;
	unsigned char ucdataout;
	unsigned int uiEventToWait = 0;
	int ibToContinue = FALSE;


	iret = SEC_PinEntry (&uiEventToWait,&ucdataout, &ibToContinue);

	return(iret);
}



//===========================================================================
//! \brief This function sets the value of the the parameter PinpadValue, according 
//! to the pinpad type.
//! \param[out] PinpadValue : type of the pinpad
//===========================================================================
static void _Cless_SetConfPinpad (int * PinpadValue)
{
	*PinpadValue=0xff;
	*PinpadValue = PSQ_Pinpad_Value();
}

//===========================================================================
//! \brief Set the PIN UI parameters.
//! \param[in] ulTimeoutBefore         = Timeout for first PIN key entry (in milliseconds).
//! \param[in] ulTimeoutInterCharacter = Timeout between two PIN key entries (in milliseconds).
//! \return
//! - void.
//===========================================================================
static void _Cless_SetPinUserInterfaceParameters (unsigned long ulTimeoutBefore, unsigned long ulTimeoutInterCharacter)
{
	/////////////////////////////////////////////////////////
	// Set the Timeout before any key for PIN entry is pressed
	if (ulTimeoutBefore != 0)
		PIN_UserInterfaceStructure.ulTimeoutBefore = ulTimeoutBefore;
	else
		PIN_UserInterfaceStructure.ulTimeoutBefore = C_DEFAULT_TIMEOUT_BEFORE_PIN_INPUT;

	/////////////////////////////////////////////////////////
	// Set the Timeout between two digits entry for PIN
	if (ulTimeoutInterCharacter != 0)
		PIN_UserInterfaceStructure.ulTimeoutInter = ulTimeoutInterCharacter;
	else
		PIN_UserInterfaceStructure.ulTimeoutInter = C_DEFAULT_TIMEOUT_BETWEEN_PIN_DIGIT_INPUT;
}



//===========================================================================
//! \brief This function initialises the structure for the following secured PIN code entry.
//! \param[in] TO_inter_car : inter character timeout.
//! \param[in] TO_first_car : first character timeout.
//! \param[out] ihid_key : character displayed to hide real character entered.
//! \return
//! TRUE if the parameters allow to enter the PIN code on the terminal 
//! when a card is inserted in the terminal, whereas a p30 is present.
//===========================================================================
static int _Cless_SetPinEntry ( short TO_inter_car, short TO_first_car,unsigned int * ihid_key)
{
	T_SEC_ENTRYCONF stEntryConfig;
	int iret,pinplace;
	char CardAID[100];

	*ihid_key= 0x2A;

	memset(CardAID, 0, sizeof(CardAID));
	mapGet(traAID, CardAID, 1);
//	if (strncmp(CardAID, "A000000333010101", 16) == 0) {
		stEntryConfig.ucMinDigits = 4;
//	}else{
//		stEntryConfig.ucMinDigits = 0;
//	}
	stEntryConfig.ucEchoChar = *ihid_key;
	stEntryConfig.ucMaxDigits = 12;
	stEntryConfig.iFirstCharTimeOut = TO_first_car;
	stEntryConfig.iInterCharTimeOut = TO_inter_car;

	pinplace = C_SEC_PINCODE;


	iret = SEC_PinEntryInit (&stEntryConfig,pinplace);
	if (iret != OK)
		GTL_Traces_TraceDebug("SEC_PinEntryInit error %d", iret);

	return iret;
}


//===========================================================================
//! \brief This function manage the online pin.
//! \param[in] pan card PAN.
//! \param[in] nb_try : number of PIN entry attempts.
//! \param[in] ulAmount amount to display.
//! \param[in] ulTimeoutBefore         = Timeout for first PIN key entry (in milliseconds).
//! \param[in] ulTimeoutInterCharacter = Timeout between two PIN key entries (in milliseconds).
//! \param[in] nCardHolderLang : client lang
//! \param[out] buffer buffer containing the cyphered PIN code.
//! \return
//! TRUE if the parameters allow to enter the PIN code on the terminal 
//! when a card is inserted in the terminal, whereas a p30 is present.
//===========================================================================
unsigned char Cless_PinManagement_OnLinePinManagement (unsigned char * ucPAN,
		unsigned char nb_try,
		unsigned long ulAmount,
		unsigned long ulTimeoutBefore,
		unsigned long ulTimeoutInterCharacter,
		int nCardHolderLang,
		BUFFER_SAISIE * buffer)
{
	unsigned char  cr_enter_pin;
	unsigned short Bypass_Required;

	_Cless_SetPinUserInterfaceParameters (ulTimeoutBefore, ulTimeoutInterCharacter);

	// To update : ul_InputAmount
	ul_InputAmount = ulAmount;

	// Online PIN management
	cr_enter_pin = _Cless_EnterPinOnLine(nb_try, buffer, ucPAN, &Bypass_Required, nCardHolderLang);
	return(cr_enter_pin);
}



//! @}

/********
 * TERM *
 ********/

//===========================================================================
//! \brief check if a PinPad is declared in the manager.
//! return 
//! - B_TRUE if an initialised PinPad is connected
//! - B_FALSE else.
//===========================================================================
T_Bool Cless_IsPinpadPresent(void)
{
	int cr;

	if ( isPinpad == B_NON_INIT)
	{
#ifndef __TELIUM3__
		isPinpad = B_FALSE;
		return(isPinpad);
#endif // __TELIUM3__

		cr = PSQ_Is_pinpad();
		if (cr == 1)
		{
			if(IsPPSConnected() == 0)
				isPinpad = B_TRUE;
			else
				isPinpad = B_FALSE;
		}
		else
		{
			isPinpad = B_FALSE;
		}
	}

	return(isPinpad);

}	// end of Cless_IsPinpadPresent

