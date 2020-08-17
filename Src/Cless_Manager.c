//! \file
//! Module that manages all the application menus

/////////////////////////////////////////////////////////////////
//// Includes ///////////////////////////////////////////////////

#include "Cless_Implementation.h"
#include "Globals.h"


/////////////////////////////////////////////////////////////////
//// Macros & preprocessor definitions //////////////////////////

#define DISK_NAME_SIZE			33			/*!< Size of disk name. */


/////////////////////////////////////////////////////////////////
//// Types //////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////
//// Global variables ///////////////////////////////////////////

static int g_TransactionLoop = FALSE;			/*!< Global data indicating if transaction loop is enabled or not (currently not used). */
static int g_DumpTransactionData = FALSE;		/*!< Global data indicating if transaction data dumping is enabled or not. */
static int g_DumpTransactionDatabase = FALSE;	/*!< Global data indicating if transaction database dumping is enabled or not. */
static int g_TracesActivated = FALSE;			/*!< Global data indicating if transaction traces (using trace tool) is enabled or not. */
static int g_VisaEuropeModeEnabled = FALSE;		/*!< Global data indicating if Visa Europe Mode is enabled or not. */
static int g_EnableInputTransactionType = FALSE;/*!< Global data indicating if TransactionType information must be inputed before each transaction. */
static int g_MerchantForcedOnline = FALSE;		/*!< Global data indicating if merchant forced transaction online (PayPass only). */

#ifdef __TELIUM3__
#define RESOURCE_FILE		"822450"
#else
#define RESOURCE_FILE		"844084"
#endif

//! \brief Main menu.
static const T_CLESS_SAMPLE_HELPER_MENU_ITEM MainMenu[] = {
		{STD_MESS_MENU_MAIN_DEBIT, "file://param/" RESOURCE_FILE "----.TAR/icons/CU_DEBIT.wgu"},
		{STD_MESS_MENU_MAIN_CASH, "file://param/" RESOURCE_FILE "----.TAR/icons/CU_CREDIT.wgu"},
		{STD_MESS_MENU_MAIN_REFUND, "file://param/" RESOURCE_FILE "----.TAR/icons/CU_CREDIT.wgu"},
		{STD_MESS_MENU_MAIN_CASHBACK, "file://param/" RESOURCE_FILE "----.TAR/icons/CU_CREDIT.wgu"},
		{STD_MESS_MENU_MAIN_TEST, "file://param/" RESOURCE_FILE "----.TAR/icons/CU_TOOLWARE.wgu"},
		{STD_MESS_MENU_MAIN_PARAMETERS, "file://param/" RESOURCE_FILE "----.TAR/icons/CL_PARAMS.wgu"},
		{STD_MESS_MENU_MAIN_MERCHANT, "file://param/" RESOURCE_FILE "----.TAR/icons/CU_MERCHANT.wgu"},
		{STD_MESS_MENU_MAIN_BATCH, "file://param/" RESOURCE_FILE "----.TAR/icons/CL_BATCH.wgu"}
};

//! \brief Transaction Type menu.
static const T_CLESS_SAMPLE_HELPER_MENU_ITEM TransactionTypeMenu[] = {
		{STD_MESS_MENU_MAIN_DEBIT, "file://param/" RESOURCE_FILE "----.TAR/icons/CU_DEBIT.wgu"},
		{STD_MESS_MENU_MAIN_CASH, "file://param/" RESOURCE_FILE "----.TAR/icons/CU_CREDIT.wgu"},
		{STD_MESS_MENU_MAIN_REFUND, "file://param/" RESOURCE_FILE "----.TAR/icons/CU_CREDIT.wgu"},
		{STD_MESS_MENU_MAIN_CASHBACK, "file://param/" RESOURCE_FILE "----.TAR/icons/CU_CREDIT.wgu"},
#ifndef DISABLE_PURE
		{PURE_MESS_MENU_MAIN_APPLI_AUTH, "file://param/" RESOURCE_FILE "----.TAR/icons/CU_CREDIT.wgu"},
		{PURE_MESS_MENU_MAIN_RETRIEVE_DATA, "file://param/" RESOURCE_FILE "----.TAR/icons/CU_CREDIT.wgu"},
		{PURE_MESS_MENU_MAIN_UPDATE_DATA, "file://param/" RESOURCE_FILE "----.TAR/icons/CU_CREDIT.wgu"}
#endif
};



//! \brief Batch menu.
static const T_CLESS_SAMPLE_HELPER_MENU_ITEM BatchMenu[] = {
		{STD_MESS_MENU_PARAMS_BATCH_TOTALS, "file://param/" RESOURCE_FILE "----.TAR/icons/CU_DEFAULT.wgu"},
		{STD_MESS_MENU_PARAMS_BATCH_PRINT_LOG, "file://param/" RESOURCE_FILE "----.TAR/icons/CU_DEFAULT.wgu"},
		{STD_MESS_MENU_PARAMS_BATCH_CLEAR, "file://param/" RESOURCE_FILE "----.TAR/icons/CU_DEFAULT.wgu"},
		{STD_MESS_MENU_PARAMS_BATCH_LAST_TR, "file://param/" RESOURCE_FILE "----.TAR/icons/CU_DEFAULT.wgu"}
};

//! \brief Test menu.
static const T_CLESS_SAMPLE_HELPER_MENU_ITEM TestMenu[] = {
		{STD_MESS_MENU_TEST_DUMP_TR, "file://param/" RESOURCE_FILE "----.TAR/icons/CU_DEFAULT.wgu"},
		{STD_MESS_MENU_TEST_DUMP_DB, "file://param/" RESOURCE_FILE "----.TAR/icons/CU_DEFAULT.wgu"},
		{STD_MESS_MENU_TEST_LOOP, "file://param/" RESOURCE_FILE "----.TAR/icons/CU_DEFAULT.wgu"},
		{STD_MESS_MENU_IMPLICIT_SEL_LOOP, "file://param/" RESOURCE_FILE "----.TAR/icons/CU_DEFAULT.wgu"},
		{STD_MESS_MENU_TEST_CHANGE_OUTPUT, "file://param/" RESOURCE_FILE "----.TAR/icons/CU_DEFAULT.wgu"},
		{STD_MESS_MENU_TEST_TRACES, "file://param/" RESOURCE_FILE "----.TAR/icons/CU_DEFAULT.wgu"},
		{STD_MESS_MENU_TEST_VISA_EUROPE, "file://param/" RESOURCE_FILE "----.TAR/icons/CU_DEFAULT.wgu"},
		{STD_MESS_MENU_TEST_INPUT_TRANSACTION_TYPE, "file://param/" RESOURCE_FILE "----.TAR/icons/CU_DEFAULT.wgu"}
};

//! \brief Menu for output data change (with printer).
static const T_CLESS_SAMPLE_HELPER_MENU_ITEM TestMenuWithPrinter[] = {
		{STD_MESS_MENU_OUTPUT_COM0, "file://param/" RESOURCE_FILE "----.TAR/icons/CU_DEFAULT.wgu"},
		{STD_MESS_MENU_OUTPUT_USB, "file://param/" RESOURCE_FILE "----.TAR/icons/CU_DEFAULT.wgu"},
		{STD_MESS_MENU_OUTPUT_NONE, "file://param/" RESOURCE_FILE "----.TAR/icons/CU_DEFAULT.wgu"},
		{STD_MESS_MENU_OUTPUT_PRINTER, "file://param/" RESOURCE_FILE "----.TAR/icons/CU_DEFAULT.wgu"}
};

//! \brief Menu for output data change (without printer).
static const T_CLESS_SAMPLE_HELPER_MENU_ITEM TestMenuWithoutPrinter[] = {
		{STD_MESS_MENU_OUTPUT_COM0, "file://param/" RESOURCE_FILE "----.TAR/icons/CU_DEFAULT.wgu"},
		{STD_MESS_MENU_OUTPUT_USB, "file://param/" RESOURCE_FILE "----.TAR/icons/CU_DEFAULT.wgu"},
		{STD_MESS_MENU_OUTPUT_NONE, "file://param/" RESOURCE_FILE "----.TAR/icons/CU_DEFAULT.wgu"}
};

//! \brief Parameters menu.
static const T_CLESS_SAMPLE_HELPER_MENU_ITEM ParametersMenu[] = {
		{STD_MESS_MENU_PARAMS_DUMP, "file://param/" RESOURCE_FILE "----.TAR/icons/CU_DUMP.wgu"},
		{STD_MESS_MENU_PARAMS_DEFAULT, "file://param/" RESOURCE_FILE "----.TAR/icons/CU_DEF_CNF.wgu"},
		{STD_MESS_MENU_PARAMS_ERASE_ALL, "file://param/" RESOURCE_FILE "----.TAR/icons/CL_PARAMDEL.wgu"}
};

//! \brief Parameters dump menu.
static const T_CLESS_SAMPLE_HELPER_MENU_ITEM ParametersMenuDump[] = {
		{STD_MESS_MENU_PARAMS_DUMP_ID, "file://param/" RESOURCE_FILE "----.TAR/icons/CU_DEFAULT.wgu"},
		{STD_MESS_MENU_PARAMS_DUMP_AIDS, "file://param/" RESOURCE_FILE "----.TAR/icons/CU_DEFAULT.wgu"},
		{STD_MESS_MENU_PARAMS_DUMP_CA, "file://param/" RESOURCE_FILE "----.TAR/icons/CU_DEFAULT.wgu"},
		{STD_MESS_MENU_PARAMS_DUMP_GLOBALS, "file://param/" RESOURCE_FILE "----.TAR/icons/CU_DEFAULT.wgu"},
		{STD_MESS_MENU_PARAMS_DUMP_KERNEL_SPECIFIC, "file://param/" RESOURCE_FILE "----.TAR/icons/CU_DEFAULT.wgu"},
		{STD_MESS_MENU_PARAMS_DUMP_ALL, "file://param/" RESOURCE_FILE "----.TAR/icons/CU_DEFAULT.wgu"}
};

//! \brief Merchant menu.
static const T_CLESS_SAMPLE_HELPER_MENU_ITEM MerchantMenu[] = {
		{STD_MESS_MENU_PARAMS_MERCHANT_FORCE_ONLINE, "file://param/" RESOURCE_FILE "----.TAR/icons/CU_DEFAULT.wgu"},
		{STD_MESS_MENU_PARAMS_MERCHANT_RESET_TSC, "file://param/" RESOURCE_FILE "----.TAR/icons/CU_DEFAULT.wgu"}
};
static StructDebitEmv         s_debit;


/////////////////////////////////////////////////////////////////
//// Static function definitions ////////////////////////////////

static void __InitDebit(unsigned long *amount,S_MONEY *currency);


static int __Cless_Menu_BatchTotals(void);
static int __Cless_Menu_BatchPrintLog(void);
static int __Cless_Menu_BatchClear(void);
static int __Cless_Menu_BatchPrintLastTransaction(void);
static int __Cless_Menu_Batch(void);
static int __Cless_Menu_Test (void);
static int __Cless_Menu_Parameters (void);
static int __Cless_Menu_TestTransactionLoop(void);
static int __Cless_Menu_DebitImplicitSelectionLoop(void);
static int __Cless_Menu_TestVisaEuropeMode(void);
static int __Cless_Menu_TestInputTransactionType(void);
static int __Cless_Menu_MerchantForcedOnline(void);
static int __Cless_Menu_Merchant (void);

extern int IsPrinter(void);

/////////////////////////////////////////////////////////////////
//// Functions //////////////////////////////////////////////////

//##################################################################
//################### DEBIT MENU ###################################
//##################################################################


static void __InitDebit(unsigned long *amount,S_MONEY *currency) {

	s_debit.param_out.noappli     = 0xFF;
	s_debit.param_out.rc_payment  = PAY_KO;
	memclr(s_debit.param_out.card_holder_nb,sizeof(s_debit.param_out.card_holder_nb));

	s_debit.param_in.support              = UNKNOWN_SUPPORT;
	s_debit.param_in.payment              = UNKNOWN_PAYMENT;

	s_debit.param_in.entry                = MANUAL_ENTRY;	

	s_debit.param_in.transaction          = DEBIT_TR;
	s_debit.param_in.amount               = *amount;

	memcpy((char *)&s_debit.param_in.currency,(char *)currency,sizeof(S_MONEY));
}


//##################################################################
//################### BATCH MENU ###################################
//##################################################################


//! \brief Action to do if TOTALS is selected in the Batch menu
//! \return Always -1.

static int __Cless_Menu_BatchTotals(void) {
	// TODO: Print a receipt with the total of the transactions in the batch
	Cless_Batch_PrintTotals();
	return -1;
}



//! \brief Action to do if PRINT LOG is selected in the Batch menu
//! \return Always -1.

static int __Cless_Menu_BatchPrintLog(void)
{
	// TODO: Print the batch content
	Cless_Batch_PrintBatchOverview();
	return -1;
}



//! \brief Action to do if PRINT LOG is selected in the Batch menu
//! \return Always -1.

static int __Cless_Menu_BatchPrintLastTransaction(void)
{
	// TODO: Print the last transaction saved in the batch
	Cless_Batch_PrintLastTransaction();
	return -1;
}



//! \brief  Action to do if UPLOAD is selected in the Batch menu
//! \return Always -1.

static int __Cless_Menu_BatchClear(void)
{
	// TODO: Updload the batch content and empty it
	Cless_Batch_Destroy();
	return -1;
}



//! \brief Batch menu management
//! \return Always -1.

static int __Cless_Menu_Batch(void)
{
	int nChoice;
	int nResult;

	nChoice = 0;
	do
	{
		nChoice = HelperDisplayMenu("BATCH", nChoice, NUMBER_OF_ITEMS(BatchMenu), BatchMenu, FALSE);
		if (nChoice >= 0)
		{
			switch(nChoice)
			{
			case 0:
				nResult = __Cless_Menu_BatchTotals();
				break;
			case 1:
				nResult = __Cless_Menu_BatchPrintLog();
				break;
			case 2:
				nResult = __Cless_Menu_BatchClear();
				break;
			case 3:
				nResult = __Cless_Menu_BatchPrintLastTransaction();
				break;
			default:
				nResult = 0;
				break;
			}

			if (nResult == -1)
				nChoice = -1;
		}
	} while(nChoice >= 0);

	return nChoice;
}



//##################################################################
//################### TEST MENU ####################################
//##################################################################

//! \brief Action to do if DUMP TR DATA is selected in the TEST menu
//! \return Always -1.

int __Cless_Menu_TestTransactionData(void) {
	MSGinfos tMsg;
	int lg_code;

	// Get the manager language (merchant language)
	lg_code = PSQ_Give_Language();

	Cless_Term_Read_Message(STD_MESS_MENU_TEST_DUMP_TR, lg_code, &tMsg);
	Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_2, &tMsg, LEDSOFF);

	if (g_DumpTransactionData == FALSE) {
		g_DumpTransactionData = TRUE;
		Cless_Term_Read_Message(STD_MESS_ENABLE, lg_code, &tMsg);
		Helper_DisplayTextMerchant(NO_ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, LEDSOFF);
	} else {
		g_DumpTransactionData = FALSE;
		Cless_Term_Read_Message(STD_MESS_DISABLE, lg_code, &tMsg);
		Helper_DisplayTextMerchant(NO_ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, LEDSOFF);
	}

	Helper_RefreshScreen(WAIT, HELPERS_MERCHANT_SCREEN);

	return -1;
}


//! \brief Action to do if "DUMP DATA DB" is selected in the TEST menu (it enables/diables the dabase dumping).
//! \return Always -1.

int __Cless_Menu_TestTransactionDumpDatabase(void) {
	MSGinfos tMsg;
	int lg_code;

	// Get the manager language (merchant language)
	lg_code = PSQ_Give_Language();

	Cless_Term_Read_Message(STD_MESS_MENU_TEST_DUMP_DB, lg_code, &tMsg);
	Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_2, &tMsg, LEDSOFF);

	if (g_DumpTransactionDatabase == FALSE) {
		g_DumpTransactionDatabase = TRUE;
		Cless_Term_Read_Message(STD_MESS_ENABLE, lg_code, &tMsg);
		Helper_DisplayTextMerchant(NO_ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, LEDSOFF);
	} else {
		g_DumpTransactionDatabase = FALSE;
		Cless_Term_Read_Message(STD_MESS_DISABLE, lg_code, &tMsg);
		Helper_DisplayTextMerchant(NO_ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, LEDSOFF);
	}

	Helper_RefreshScreen(WAIT, HELPERS_MERCHANT_SCREEN);

	return -1;
}



//! \brief Choose the output peripheral to get the dumps (parameters or transaction data).
//! \return Always -1.

int __Cless_Menu_TestChooseOutput (void) {
	int nChoice;
	int nResult;

	nChoice = 0;
	nResult = 0;

	do {
		if (!IsPrinter()) // IsPrinter() is not yet provided by the simulator
			nChoice = HelperDisplayMenu("OUTPUT", nChoice, NUMBER_OF_ITEMS(TestMenuWithoutPrinter), TestMenuWithoutPrinter, FALSE);
		else
			nChoice = HelperDisplayMenu("OUTPUT", nChoice, NUMBER_OF_ITEMS(TestMenuWithPrinter), TestMenuWithPrinter, FALSE);

		if (nChoice >= 0) {
			switch(nChoice) {
			case 0: // COM0
				Cless_DumpData_DumpSetOutputId (CUSTOM_OUTPUT_COM0);
				nResult = -1;
				break;

			case 1: // USB
				Cless_DumpData_DumpSetOutputId (CUSTOM_OUTPUT_USB);
				nResult = -1;

			case 2: // NONE
				Cless_DumpData_DumpSetOutputId (CUSTOM_OUTPUT_NONE);
				nResult = -1;
				break;

			case 3: //Printer (if present)
				if (IsPrinter()) {
					Cless_DumpData_DumpSetOutputId (CUSTOM_OUTPUT_PRINTER);
					nResult = -1;
				} else {
					nResult = 0;
				}
				break;

			default:
				nResult = 0;
				break;
			}

			if (nResult == -1)
				nChoice = -1;
		}
	} while(nChoice >= 0);

	return nChoice;
}



//! \brief Action to do if TRANSAC LOOP is selected in the TEST menu
//! \return Always -1.

static int __Cless_Menu_TestTransactionLoop(void) {
	MSGinfos tMsg;
	int lg_code;

	// Get the manager language (merchant language)
	lg_code = PSQ_Give_Language();

	Cless_Term_Read_Message(STD_MESS_MENU_TEST_LOOP, lg_code, &tMsg);
	Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_2, &tMsg, LEDSOFF);

	if (g_TransactionLoop == FALSE) {
		g_TransactionLoop = TRUE;
		Cless_Term_Read_Message(STD_MESS_ENABLE, lg_code, &tMsg);
		Helper_DisplayTextMerchant(NO_ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, LEDSOFF);
	} else {
		g_TransactionLoop = FALSE;
		Cless_Term_Read_Message(STD_MESS_DISABLE, lg_code, &tMsg);
		Helper_DisplayTextMerchant(NO_ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, LEDSOFF);
	}

	Helper_RefreshScreen(WAIT, HELPERS_MERCHANT_SCREEN);

	return -1;
}

//===============================================================
//! \brief Begin several payment cycles
//! \return Always -1.

static int __Cless_Menu_DebitImplicitSelectionLoop(void) {
	unsigned char    priority;
	unsigned char    cr;
	unsigned int     retour;
	int nEvent;
	unsigned long    ulAmount;
	ENTRY_BUFFER     buf;
	S_MONEY          currency;
	TAB_ENTRY_AMOUNT tab_montant = {0x6FC0,MINUTE};
	Telium_File_t            *hcam0;

	TAB_ENTRY_STRING Options = {0x0000, 2*MINUTE, 1, 0, ECHO_NORMAL, 6, 1}; 
	BUFFER_SAISIE buffer ;
	int nbLoop;
	int nTrLoopState; // To memorise an information
	Telium_File_t * hKeyboard;


	Telium_Fopen("DISPLAY","w");
	Telium_Printf ("Number of loops?\n");
	Telium_Fclose(Telium_Stdout());
	Saisie_Numerique ((TAB_ENTRY_STRING *)&Options);
	Telium_Ttestall (SAISIE,0);
	cr = Read_Saisie (&buffer);
	if (cr == CR_SAISIE_OK) {
		buffer.donnees[buffer.nombre] = '\0';
		nbLoop = atoi((char*)(buffer.donnees));
	}
	else
		nbLoop = 1;

	// Get the amount 
	G_Saisie_montant (&currency,		// Contain the currency selected in return   
			&buf,							// Contain the amount in return
			&cr,							// Contain the result of entry in return
			"AMOUNT",						// The message above the amount
			(TAB_ENTRY_AMOUNT *)&tab_montant, // Parameters of entry
			NULL,							// Currency table
			0,								// Number of currency
			0,								// mask of periph to wait
			_AFF_FRANCE_);					// type of displaying

	if (cr == CR_ENTRY_OK) {
		buf.d_entry[buf.d_len] = '\0';
		ulAmount=atoi((char*)(buf.d_entry));

		// Check if the Service is available 
		if (Telium_ServiceGet(TYPE_GESTIONNAIRE,100,&priority)==0) {
			do {
				/* Open if CAM0 if not */
				hcam0 = stdcam0();
				if (hcam0==NULL) {
					hcam0 = Telium_Fopen("CAM0","rw");
				}

				__InitDebit(&ulAmount,&currency);

				// To forbid ticket
				nTrLoopState = g_TransactionLoop; // Memorise state 
				g_TransactionLoop = TRUE;

				Telium_ServiceCall(TYPE_GESTIONNAIRE,100,sizeof(StructDebitEmv),&s_debit,(int *)&retour);

				g_TransactionLoop = nTrLoopState; // Restore state

				/* Close CAM0 if opened */
				if (hcam0==NULL) {
					Telium_Power_down(stdcam0());Telium_Fclose(stdcam0());
				}

				nbLoop--;

				// Keyboard test to check if ANNulation key has been pressed
				hKeyboard = Telium_Fopen("KEYBOARD", "r");
				// Wait some seconds to check if no interruption is asked (RED button) 
				nEvent = Telium_Ttestall (KEYBOARD, 300);
				if (nEvent == KEYBOARD) {
					if (Telium_Getc(hKeyboard) == T_ANN)
					{
						nbLoop = 0; // End loop
					}
				}
				if (hKeyboard != NULL)
					Telium_Fclose (hKeyboard);

			}while(nbLoop > 0); 
		}
	}

	return -1;
}

//! \brief Action to do if TRACES is selected in the TEST menu
//! \return Always -1.

int __Cless_Menu_TestTraces(void) {
	MSGinfos tMsg;
	int lg_code;

	// Get the manager language (merchant language)
	lg_code = PSQ_Give_Language();

	Cless_Term_Read_Message(STD_MESS_MENU_TEST_TRACES, lg_code, &tMsg);
	Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_2, &tMsg, LEDSOFF);

	if (g_TracesActivated == FALSE) {
		g_TracesActivated = TRUE;
		Cless_Term_Read_Message(STD_MESS_ENABLE, lg_code, &tMsg);
		Helper_DisplayTextMerchant(NO_ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, LEDSOFF);
	} else {
		g_TracesActivated = FALSE;
		Cless_Term_Read_Message(STD_MESS_DISABLE, lg_code, &tMsg);
		Helper_DisplayTextMerchant(NO_ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, LEDSOFF);
	}

	Helper_RefreshScreen(WAIT, HELPERS_MERCHANT_SCREEN);

	// Debug features management
#ifndef DISABLE_OTHERS_KERNELS
	Cless_PayPass_DebugActivation (g_TracesActivated);
#endif
	Cless_payWave_DebugActivation (g_TracesActivated);
#ifndef DISABLE_OTHERS_KERNELS
	Cless_VisaWave_DebugActivation (g_TracesActivated);
	Cless_ExpressPay_DebugActivation (g_TracesActivated);
	//	Cless_Discover_DebugActivation (g_TracesActivated);
#ifndef DISABLE_INTERAC
	Cless_Interac_DebugActivation (g_TracesActivated);
#endif
	//	Cless_DiscoverDPAS_DebugActivation (g_TracesActivated);
#endif // DISABLE_OTHERS_KERNELS
#ifndef DISABLE_PURE
	Cless_PURE_DebugActivation (g_TracesActivated);
#endif // DISABLE_PURE
	return -1;
}



//! \brief Action to do if "VISA EUROPE" is selected in the TEST menu (activate/deactivate the "Visa Europe Mode")
//! \return Always -1.

static int __Cless_Menu_TestVisaEuropeMode(void) {
	MSGinfos tMsg;
	int lg_code;

	// Get the manager language (merchant language)
	lg_code = PSQ_Give_Language();

	Cless_Term_Read_Message(STD_MESS_MENU_TEST_VISA_EUROPE, lg_code, &tMsg);
	Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_2, &tMsg, LEDSOFF);

	if (g_VisaEuropeModeEnabled == FALSE) {
		g_VisaEuropeModeEnabled = TRUE;
		Cless_Term_Read_Message(STD_MESS_ENABLE, lg_code, &tMsg);
		Helper_DisplayTextMerchant(NO_ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, LEDSOFF);
	} else {
		g_VisaEuropeModeEnabled = FALSE;
		Cless_Term_Read_Message(STD_MESS_DISABLE, lg_code, &tMsg);
		Helper_DisplayTextMerchant(NO_ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, LEDSOFF);
	}

	Helper_RefreshScreen(WAIT, HELPERS_MERCHANT_SCREEN);

	return -1;
}

//! \brief Action enable/disable "TransactionType" information to input.
//! \return Always -1.

static int __Cless_Menu_TestInputTransactionType(void) {
	MSGinfos tMsg;
	int lg_code;

	// Get the manager language (merchant language)
	lg_code = PSQ_Give_Language();

	Cless_Term_Read_Message(STD_MESS_MENU_TEST_INPUT_TRANSACTION_TYPE, lg_code, &tMsg);
	Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_2, &tMsg, LEDSOFF);


	if (g_EnableInputTransactionType == FALSE) {
		g_EnableInputTransactionType = TRUE;
		Cless_Term_Read_Message(STD_MESS_ENABLE, lg_code, &tMsg);
		Helper_DisplayTextMerchant(NO_ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, LEDSOFF);
	} else {
		g_EnableInputTransactionType = FALSE;
		Cless_Term_Read_Message(STD_MESS_DISABLE, lg_code, &tMsg);
		Helper_DisplayTextMerchant(NO_ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, LEDSOFF);
	}

	Helper_RefreshScreen(WAIT, HELPERS_MERCHANT_SCREEN);

	return -1;
}

//! \brief Test if EnableTransactionType is on.
//! \return
//!		- \ref TRUE if loop is activated.
//!		- \ref FALSE if not.
unsigned char Cless_Menu_IsEnableInputTransactionType (void)
{
	return (unsigned char)g_EnableInputTransactionType;
}	



//! \brief Test if transaction loop mode has been activated.
//! \return
//!		- \ref TRUE if loop is activated.
//!		- \ref FALSE if not.

unsigned char Cless_Menu_IsTransactionLoopOn (void)
{
	return (unsigned char)g_TransactionLoop;
}	



//! \brief Test if transaction data dumping is TRUE or FALSE.
//! \return
//!		- \ref TRUE if dump mode is activated.
//!		- \ref FALSE if not.

unsigned char Cless_Menu_IsTransactionDataDumpingOn (void)
{
	return (unsigned char)g_DumpTransactionData;
}	



//! \brief Test if transaction database dumping is TRUE or FALSE.
//! \return
//!		- \ref TRUE if database dumping is activated.
//!		- \ref FALSE if not.

unsigned char Cless_Menu_IsTransactionDatabaseDumpingOn (void)
{
	return (unsigned char)g_DumpTransactionDatabase;
}	



//! \brief Test if Visa Europe Mode is activated or not.
//! \return
//!		- \ref TRUE if database dumping is activated.
//!		- \ref FALSE if not.

unsigned char Cless_Menu_IsVisaEuropeModeOn (void)
{
	return (unsigned char)g_VisaEuropeModeEnabled;
}	



//! \brief Test menu management
//! \return Always -1.

static int __Cless_Menu_Test(void)
{
	int nChoice;
	int nResult;

	nChoice = 0;
	do
	{
		nChoice = HelperDisplayMenu("TEST MENU", nChoice, NUMBER_OF_ITEMS(TestMenu), TestMenu, FALSE);
		if (nChoice >= 0)
		{
			switch(nChoice)
			{
			case 0:
				nResult = __Cless_Menu_TestTransactionData();
				break;

			case 1:
				nResult = __Cless_Menu_TestTransactionDumpDatabase();
				break;

			case 2:
				nResult = __Cless_Menu_TestTransactionLoop();
				break;

			case 3:
				nResult = __Cless_Menu_DebitImplicitSelectionLoop();
				break;

			case 4:
				nResult = __Cless_Menu_TestChooseOutput();
				break;

			case 5:
				nResult = __Cless_Menu_TestTraces();
				break;

			case 6:
				nResult = __Cless_Menu_TestVisaEuropeMode();
				break;

			case 7:
				nResult = __Cless_Menu_TestInputTransactionType();
				break;

			default:
				nResult = 0;
				break;
			}

			if (nResult == -1)
				nChoice = -1;
		}
	} while(nChoice >= 0);

	return nChoice;
}



//##################################################################
//################### PARAMETERS MENU ##############################
//##################################################################


//! \brief Action to do if ERASE ALL is selected in the PARAMETERS menu. It erase all the application parameters.
//! \return Always -1.

int __Cless_Menu_EraseParameters (void) {
	MSGinfos tMsg, tGlobalMsg;
	int lg_code;

	// Get the manager language (merchant language)
	lg_code = PSQ_Give_Language();

	Cless_Term_Read_Message(STD_MESS_ERASING_PARAMS, lg_code, &tGlobalMsg);
	//	Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_2, &tGlobalMsg, LEDSOFF);
	//	Cless_Term_Read_Message(STD_MESS_IN_PROGRESS, lg_code, &tMsg);
	//	Helper_DisplayTextMerchant(NO_ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, LEDSOFF);
	Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);

	Cless_Parameters_InitParameters(&pTreeCurrentParam); // Reset TLVTree parameters
	pTreeCurrentParam = TlvTree_New(0);
	if (Cless_Parameters_SaveParam (pTreeCurrentParam) != 0) { // Save dummy TLVTree parameters in file
		Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_2, &tGlobalMsg, LEDSOFF);
		Cless_Term_Read_Message(STD_MESS_ERROR, lg_code, &tMsg);
		Helper_DisplayTextMerchant(NO_ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, LEDSOFF);
	} else {
		//		Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_2, &tGlobalMsg, LEDSOFF);
		//		Cless_Term_Read_Message(STD_MESS_COMPLETE, lg_code, &tMsg);
		//		Helper_DisplayTextMerchant(NO_ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, LEDSOFF);
	}

	Helper_RefreshScreen(WAIT, HELPERS_MERCHANT_SCREEN);

	return -1;
}


//! \brief Action read default parameters and update .SAV file with it
//! \return Always -1.

int __Cless_Menu_DefaultParameters(void) {

	MSGinfos tMsg, tGlobalMsg;
	int lg_code;

	// Get the manager language (merchant language)
	lg_code = PSQ_Give_Language();

	Cless_Term_Read_Message(STD_MESS_UPDATING_PARAMS, lg_code, &tGlobalMsg);
	//	Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_2, &tGlobalMsg, LEDSOFF);
	//	Cless_Term_Read_Message(STD_MESS_IN_PROGRESS, lg_code, &tMsg);
	//	Helper_DisplayTextMerchant(NO_ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, LEDSOFF);
	//	Helper_RefreshScreen(NOWAIT, HELPERS_MERCHANT_SCREEN);

	// Read default parameters
	Cless_Parameters_ReadDefaultParameters(&pTreeCurrentParam);

	if (Cless_Parameters_SaveParam (pTreeCurrentParam) != 0) { // Save dummy TLVTree parameters in file

		Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_2, &tGlobalMsg, LEDSOFF);
		Cless_Term_Read_Message(STD_MESS_ERROR, lg_code, &tMsg);
		Helper_DisplayTextMerchant(NO_ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, LEDSOFF);
	} else {
		//		Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_2, &tGlobalMsg, LEDSOFF);
		//		Cless_Term_Read_Message(STD_MESS_COMPLETE, lg_code, &tMsg);
		//		Helper_DisplayTextMerchant(NO_ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, LEDSOFF);
	}

	Helper_RefreshScreen(WAIT, HELPERS_MERCHANT_SCREEN);

	// Check parameters CA keys
	Cless_Parameters_CheckCaKeysCrc(pTreeCurrentParam);

	return -1;
}



//! \brief Parameters menu management
//! \return Always -1.

static int __Cless_Menu_Parameters (void)
{
	int nChoice;
	int nResult;


	nChoice = 0;
	do
	{
		nChoice = HelperDisplayMenu("PARAMETERS", nChoice, NUMBER_OF_ITEMS(ParametersMenu), ParametersMenu, FALSE);
		if (nChoice >= 0)
		{
			switch(nChoice)
			{
			case 0:			// Trace parameters on printer
				nResult = __Cless_Menu_ParametersDump();
				break;
			case 1:			// Read default parameters and save it in file (for next start)
				nResult = __Cless_Menu_DefaultParameters();
				break;
			case 2:			// Scratch parameters 
				nResult = __Cless_Menu_EraseParameters();
				break;
			default:		// ?
				nResult = 0;
				break;
			}

			if (nResult == -1)
				nChoice = -1;
		}
	} while(nChoice >= 0);

	return nChoice;
}



//! \brief Parameters menu management for data dumping
//! \return Always -1.

int __Cless_Menu_ParametersDump (void) {
	int nChoice;
	int nResult;


	nChoice = 0;
	do {
		nChoice = HelperDisplayMenu("PARAMETERS DUMP", nChoice, NUMBER_OF_ITEMS(ParametersMenuDump), ParametersMenuDump, FALSE);
		if (nChoice >= 0) {
			switch(nChoice)
			{
			case 0:			// Identifier
				Cless_Parameters_DumpIdentificationLabel(pTreeCurrentParam);
				nResult = 0;
				break;
			case 1:			// AIDs
				Cless_Parameters_DumpAids (pTreeCurrentParam);
				nResult = 0;
				break;
			case 2:			// CA Keys
				Cless_Parameters_DumpCaKeys (pTreeCurrentParam);
				nResult = 0;
				break;
			case 3:			// Globals
				Cless_Parameters_DumpGlobals (pTreeCurrentParam);
				nResult = 0;
				break;
			case 4:			// Kernel specific
				Cless_Parameters_DumpKernelSpecific (pTreeCurrentParam);
				nResult = 0;
				break;
			case 5:			// All
				Cless_Parameters_DumpIdentificationLabel(pTreeCurrentParam);
				Cless_Parameters_DumpAids (pTreeCurrentParam);
				Cless_Parameters_DumpCaKeys (pTreeCurrentParam);
				Cless_Parameters_DumpGlobals (pTreeCurrentParam);
				Cless_Parameters_DumpKernelSpecific (pTreeCurrentParam);
				nResult = 0;
				break;
			default:		// ?
				nResult = 0;
				break;
			}

			if (nResult == -1)
				nChoice = -1;
		}
	} while(nChoice >= 0);

	return nChoice;
}



//##################################################################
//################### GLOBAL MENU ##################################
//##################################################################

//! \brief Action to do if "Forced Online" is selected in the MERCHANT menu
//! \return Always -1.

static int __Cless_Menu_MerchantForcedOnline(void) {
	MSGinfos tMsg;
	int lg_code;

	// Get the manager language (merchant language)
	lg_code = PSQ_Give_Language();

	Cless_Term_Read_Message(STD_MESS_MENU_PARAMS_MERCHANT_FORCE_ONLINE, lg_code, &tMsg);
	Helper_DisplayTextMerchant(ERASE, HELPERS_MERCHANT_LINE_2, &tMsg, LEDSOFF);

	if (g_MerchantForcedOnline == FALSE) {
		g_MerchantForcedOnline = TRUE;
		Cless_Term_Read_Message(STD_MESS_ENABLE, lg_code, &tMsg);
		Helper_DisplayTextMerchant(NO_ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, LEDSOFF);
	} else {
		g_MerchantForcedOnline = FALSE;
		Cless_Term_Read_Message(STD_MESS_DISABLE, lg_code, &tMsg);
		Helper_DisplayTextMerchant(NO_ERASE, HELPERS_MERCHANT_LINE_3, &tMsg, LEDSOFF);
	}

	Helper_RefreshScreen(WAIT, HELPERS_MERCHANT_SCREEN);

	return -1;
}


//! \brief Merchant menu management for data dumping
//! \return Always -1.

static int __Cless_Menu_Merchant (void) {
	int nChoice;
	int nResult;

	nChoice = 0;
	do {
		nChoice = HelperDisplayMenu("MERCHANT", nChoice, NUMBER_OF_ITEMS(MerchantMenu), MerchantMenu, FALSE);
		if (nChoice >= 0) {
			switch(nChoice) {
			case 0:			// Force online
				nResult = __Cless_Menu_MerchantForcedOnline();
				break;
			case 1:			// Reset TSC
				Cless_Batch_InitTransactionSeqCounter (TRUE);
				nResult = -1;
				break;
			default:		// ?
				nResult = 0;
				break;
			}

			if (nResult == -1)
				nChoice = -1;
		}
	} while(nChoice >= 0);

	return nChoice;
}



//! \brief Test if merchant forced transaction online or not.
//! \return
//!		- \ref TRUE if merchant forced transaction online.
//!		- \ref FALSE if not.

unsigned char Cless_Menu_IsMerchantForcedOnline (void) {
	int g_MerchantForcedOnline = FALSE;
	//TODO: CLESS-NEW make sure you get this from the DB
	return (unsigned char)g_MerchantForcedOnline;
}	



//##################################################################
//################### GLOBAL MENU ##################################
//##################################################################

//! \brief Global menu management
void Cless_Menu_MainMenuDisplay(void) {
	int nChoice;
	int nResult;

	nChoice = 0;
	do
	{
		nChoice = HelperDisplayMenu("CONTACTLESS", nChoice, NUMBER_OF_ITEMS(MainMenu), MainMenu, FALSE);
		if (nChoice >= 0)
		{
			switch(nChoice)
			{
			//			case 0:
			//				nResult = __Cless_Menu_Debit();
			//				break;
			//			case 1:
			//				nResult = __Cless_Menu_Cash();
			//				break;
			//			case 2:
			//				nResult = __Cless_Menu_Refund();
			//				break;
			//			case 3:
			//				nResult = __Cless_Menu_CashBack();
			//				break;
			//			case 4:
			case 0:
				nResult = __Cless_Menu_Test();
				break;
			case 5:
				//			case 1:
				nResult = __Cless_Menu_Parameters();
				break;
				//			case 6:
			case 2:
				nResult = __Cless_Menu_Merchant();
				break;
				//			case 7:
			case 3:
				nResult = __Cless_Menu_Batch();
				break;
			default:
				nResult = 0;
				break;
			}

			if (nResult == -1)
				nChoice = -1;
		}
	} while(nChoice >= 0);
}

//! \brief Transaction Type menu management
void Cless_Menu_TransactionTypeMenuDisplay(const unsigned char *pCurrencyLabel, unsigned long * pulAmount, unsigned long * pulAmountOther, int * pnTransactionType, int * pnTransactionCashBack) {
	int nChoice;
	int nInputAmountOther = 0;

	*pnTransactionType = -1;
	*pnTransactionCashBack = 0;

	nChoice = 0;

	do {
		nChoice = HelperDisplayMenu("CONTACTLESS", nChoice, NUMBER_OF_ITEMS(TransactionTypeMenu), TransactionTypeMenu, FALSE);

		if (nChoice >= 0) {
			switch(nChoice) {
			case 0:
				*pnTransactionType = CLESS_SAMPLE_TRANSACTION_TYPE_DEBIT;
				break;
			case 1:
				*pnTransactionType = CLESS_SAMPLE_TRANSACTION_TYPE_CASH;
				break;
			case 2:
				*pnTransactionType = CLESS_SAMPLE_TRANSACTION_TYPE_REFUND;
				break;
			case 3:
				//			    *pnTransactionType = CLESS_SAMPLE_TRANSACTION_TYPE_DEBIT;
				*pnTransactionType = CLESS_SAMPLE_TRANSACTION_TYPE_PURSH_WITH_CASHBACK;
				nInputAmountOther = TRUE;
				*pnTransactionCashBack = TRUE;
				break;
#ifndef DISABLE_PURE
			case 4:
				*pnTransactionType = PURE_TRANSACTION_TYPE_APPLICATION_ATHENTICATION;
				break;
			case 5:
				*pnTransactionType = PURE_TRANSACTION_TYPE_RETRIEVE_DATA_ELEMENTS;
				break;
			case 6:
				*pnTransactionType = PURE_TRANSACTION_TYPE_UPDATE_DATA_ELEMENTS;
				break;
#endif
			default:
				*pnTransactionType = -1;
				break;
			}
		}
	} while (*pnTransactionType == -1);

	if (nInputAmountOther) {
		// If Amount = 0
		if (*pulAmount == 0) {
			*pulAmountOther = 0; // No Amount other entry possible or must be set to 0 (Initial amount inputed must be egal to amount + amount other)
		} else {
			unsigned char ucFormattedAmountMessage[32];

			// => amount other must inputed
			if (!Cless_ExplicitSelection_AmountEntry(pCurrencyLabel, pulAmountOther, ucFormattedAmountMessage)) {
				*pnTransactionType = -1;
			}
		}
	}
}

int CLESS_Explicit_DoTransaction(void){
	unsigned char ucTransactionType = CLESS_SAMPLE_TRANSACTION_TYPE_DEBIT;
	char ucAmountType = CLESS_SAMPLE_AMOUNT_TYPE_BIN;
	int ret = 0 ,Cless_result = 0;
	char MENU[lenMnu + 1];
	card mnuitem = 0;

	if(!ClessEmv_IsDriverOpened())
		ClessEmv_OpenDriver();


	memset(MENU,0,sizeof(MENU));

	MAPGET(traMnuItm, MENU, lblKO);
	dec2num(&mnuitem, MENU,0);

	switch (mnuitem) {
	case mnuDeposit:
		ucTransactionType = CLESS_SAMPLE_TRANSACTION_TYPE_CASH_DEPOSIT;
		ucAmountType = CLESS_SAMPLE_AMOUNT_TYPE_BIN;
		break;
	case mnuWithdrawal:
		ucTransactionType = CLESS_SAMPLE_TRANSACTION_TYPE_CASH;
		ucAmountType = CLESS_SAMPLE_AMOUNT_TYPE_BIN;
		break;
	case mnuVoid:
		ucTransactionType = CLESS_SAMPLE_TRANSACTION_TYPE_REFUND;
		ucAmountType = CLESS_SAMPLE_AMOUNT_TYPE_BIN;
		break;
	case mnuSaleCB:
		ucTransactionType = CLESS_SAMPLE_TRANSACTION_TYPE_PURSH_WITH_CASHBACK;
		ucAmountType = CLESS_SAMPLE_AMOUNT_TYPE_OTHER_BIN;
		break;
	case mnuSale:
	default:
		ucTransactionType = CLESS_SAMPLE_TRANSACTION_TYPE_DEBIT;
		ucAmountType = CLESS_SAMPLE_AMOUNT_TYPE_BIN;
		break;
	}


	/// Now do the transaction
	Cless_result = Cless_ExplicitSelection_Process (ucTransactionType, ucAmountType);


	lblKO:;

	if(ClessEmv_IsDriverOpened())
		ClessEmv_CloseDriver();


	return Cless_result;
}

