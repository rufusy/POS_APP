#include "globals.h"
#include "MenuManager.h"
#include "Sqlite.h"
#include "crypto.h"

extern T_GL_HGRAPHIC_LIB hGoal; // Handle of the graphics object library

static void ManageInv(void) {
	int ret = 0;
	char STAN_str[lenSTAN + 1];
	card STAN;

	memset(STAN_str, 0, sizeof(STAN_str));

	MAPGETCARD(appInvNum, STAN, lblKO);

	num2dec(STAN_str, STAN, lenSTAN);

	MAPPUTSTR(traInvNum, STAN_str, lblKO);

	lblKO:;
}


void ManageSTAN(void) {
	int ret = 0;
	char STAN_str[lenSTAN + 1];
	card STAN;

	memset(STAN_str, 0, sizeof(STAN_str));

	MAPGETCARD(appSTAN, STAN, lblKO);

	num2dec(STAN_str, STAN, lenSTAN);

	MAPPUTSTR(traSTAN, STAN_str, lblKO);

	lblKO:
	ManageInv();
}

static const char *tzConnModeMenu[] = {
		"GPRS",
		"ETHERNET",
		"WIFI",
		NULL
};


static const char *tzSLLModeMenu[] = {
		"Enable TLS and SSL",
		"Disable TLS and SSL",
		NULL
};


static const char *tzShortCutMenuBiller[] = {
		"PAY BILL",
		"COMTEST",
		"SETTLEMENT",
		NULL
};


static const char *tzShortCutMenuAdmin[] = {
		"Terminal Mode  Sel.",
		"Terminal Parameters",
		"Biller Configure",
		"Loaded Keys Check",
		"Generate TLS keys",
		"IP Setup",
		"CLESS Mode Off",
		"Connection Mode",
		"Show Control Panel",
		"Force PIN CVM",
		NULL
};

static const char *tzShortCutMenu[] = {
		"SALE",
		"VOID",
		"SETTLEMENT",
		"COMTEST",
		"DUPLICATE",
		"PREAUTHORIZATION",
		"COMPLETION",
		"REFUND",
		"POS MANAGEMENT", // user management(Add, Edit, Delete),   summary report, communication,
//		"MANUAL SALE",
		NULL
};


static const char *tzShortCutMenu_Management[] = {
		"USER MANAGEMENT",
		"REPORTS",
		"COMMUMNICATION",
		NULL
};


static const char *tzReports[] = {
		"SUMMARY REPORT",
		"DETAILED REPORT",
		NULL
};


static void ShortCutMenu_Management_Reports(void){
	int ret = 0;

	switch (ret=GL_Dialog_Menu(hGoal, "SELECT REPORT TYPE", tzReports, 0, GL_BUTTON_ALL, GL_KEY_0, GL_TIME_MINUTE)) {
	case 0: // Summary reports
		PrintSummaryLog();
		break;
	case 1: //Detailed reports
		PrintDetailedLog();
		break;
	default:
		break; // Default and
	}
}


static void ShortCutMenu_Management(void){
	char ConnMode[lenCommRoute + 1];
	int ret = 0;

	memset(ConnMode, 0, sizeof(ConnMode));

	switch (ret=GL_Dialog_Menu(hGoal, "SELECT MANAGEMENT MENU", tzShortCutMenu_Management, 0, GL_BUTTON_ALL, GL_KEY_0, GL_TIME_MINUTE)) {
	case 0: // User management
		fncUserManager();
		break;
	case 1: //Summary reports
		ShortCutMenu_Management_Reports();
		break;
	case 2: //Terminal IP configs
		PromptEthernet();
		break;
	default:
		break; // Default and
	}
}

void ShortCutEnhanced(void){
	int ret = 0;
	ulong result = 0;
	word key = 0;
	int iHeader = 0, iFooter = 0;
	int iRet = 0, refreshed = 0;
	char Menu[lenMnu + 1];
	char amountVal[lenAmt + 1];
	byte Mode = 0;

	mapGetByte(appTerminalMode, Mode);
	memset(amountVal, 0, sizeof(amountVal));

	Cless_Goal_IsAvailable();

	// Open peripherals
	// ****************
	iRet = OpenPeripherals();

	iHeader = IsHeader();       // Save header state
	iFooter = IsFooter();       // Save footer state

	DisplayHeader(_OFF_);  // dissable Header
	DisplayFooter(_OFF_);  // dissable Footer

	// Read date and time
	// ******************

	if (!IsColorDisplay()) {    // B&W terminal?
		// Yes
		DisplayLeds(_OFF_);     // Disable Leds
		DisplayHeader(_OFF_);   // Disable Header
		DisplayFooter(_OFF_);   // Disable Footer
	}

	iRet = GetPpdDisplay();     // Get pinpad type
	CHECK(iRet>=0, lblKO);
	if (iRet == 1){             // Pinpad with character display
		PPS_firstline();        // Display pinpad idle message
		PPS_Display("    WELCOME     ");
	}

	MAPPUTWORD(traManualPan,0,lblDB_Corrupt);

	lblMENU:

	//check if there is need to do settlement
	fncAutoSettlementChecker();

	// Open standard peripherals just in case
	iRet = OpenPeripherals();
	MAPPUTSTR(appAppLoggedName, "----", lblKO);

	memset(Menu, 0, sizeof(Menu));

	switch (Mode) {
	///// > BILLER MODE
	/////---------------------------------------------------------------------------------------------------------------------------
	case 1:
		switch (result = GL_Dialog_Menu(hGoal, NULL, tzShortCutMenuBiller, 0, GL_BUTTON_ALL, GL_KEY_0, GL_TIME_MINUTE)) {

		case 0: // PAYBILL
			key = mnuBiller;
			break;
		case 1: // COMM TEST
			key = mnuEchoTest;
			break;
		case 2: // SETTLEMENT
			key = mnuSettlement;
			break;
			//		case 4:  // POS MANAGEMENT
			//			if(fncSecurityPassword("2") == 0){
			//				goto lblMENU;// Case where the password was wrong
			//			}
			//			ShortCutMenu_Management();
			//			key = 0;
			//			break;

		default:
			if (result == GL_KEY_CANCEL) {
				goto lblEnd;
			} else {
				goto lblMENU;
			}
			break;
		}

		break;
		///// > NORMAL MODE
		/////---------------------------------------------------------------------------------------------------------------------------
		case 0:
		default:
			switch (result = GL_Dialog_Menu(hGoal, NULL, tzShortCutMenu, 0, GL_BUTTON_ALL, GL_KEY_0, GL_TIME_MINUTE)) {

			case 0: // SALE
				MAPPUTWORD(traManualPan,0,lblKO);
				key = mnuSale;
				break;
			case 1: // VOID
				key = mnuVoid;
				break;
			case 2: // SETTLEMENT
				key = mnuSettlement;
				break;
			case 3: // SETTLEMENT
				key = mnuEchoTest;
				break;
			case 4: // REPRINT / DUPLICATE
				key = mnuDuplicateReceipt;
				break;
			case 5: // PREAUTHORIZATION
				if(fncSecurityPassword("1") == 0){
					goto lblMENU;// Case where the password was wrong
				}
				key = mnuPreaut;
				break;
			case 6: // COMPLETION
				if(fncSecurityPassword("1") == 0){
					goto lblMENU;// Case where the password was wrong
				}
				key = mnuCompletion;
				break;
			case 7: // REFUND
				if(fncSecurityPassword("1") == 0){
					goto lblMENU;// Case where the password was wrong
				}
				key = mnuRefund;
				break;
			case 8: // POS MANAGEMENT
				if(fncSecurityPassword("2") == 0){
					goto lblMENU;// Case where the password was wrong
				}
				ShortCutMenu_Management();
				key = 0;
				break;

//			case 8: // MANUAL SALE
//				if(fncSecurityPassword("1") == 0){
//					goto lblMENU;// Case where the password was wrong
//				}
//				MAPPUTWORD(traManualPan, 1, lblKO);
//				key = mnuSale;
//				break;

			default:
				if (result == GL_KEY_CANCEL) {
					goto lblEnd;
				}else {
					goto lblMENU;
				}
				break;
			}
			break;
			/////---------------------------------------------------------------------------------------------------------------------------
	}

	if ((result == GL_RESULT_INACTIVITY) || (result == GL_RESULT_TIMEOUT)) {
		// Close standard peripherals
		ClosePeripherals();
		ret = Application_Card_Transaction();

		if(ret == 0){ ///Means No card was detected
			iRet = OpenPeripherals(); // Open standard peripherals just in case
			goto lblMENU;
		} else {
			key = mnuSale;
		}
	} else if (result == GL_KEY_CANCEL) {
		goto lblEnd;
	}

	if(key != 0){
		ClosePeripherals();                                      // Close standard peripherals
		num2dec(Menu, key, 0);
		MAPPUTSTR(traMnuItm, Menu, lblKO);
		mapPutStr(traAmt, amountVal); //Clear the amount value
		MenuProcessingSelect(key); //ProcessMenu
	}

	goto lblMENU;

	lblDB_Corrupt:
	if (!refreshed) {
		GL_Dialog_Message(hGoal, NULL, "Please Wait...", GL_ICON_NONE,GL_BUTTON_ALL, GL_TIME_SECOND);
		refreshed = 1;
		RefreshDB();
		goto lblMENU;
	}

	lblKO:

	lblEnd:

	mapPutWord(traManualPan,0);

	// Open peripherals
	// ****************
	iRet = OpenPeripherals();

	DisplayLeds(_ON_);       // Enable Leds
	DisplayHeader(iHeader);  // Restore Header
	DisplayFooter(iFooter);  // Restore Footer

	ClosePeripherals();                                      // Close standard peripherals

	//clear dirty buffer
	MemoryManagerClearGarbage();
}


void ShortCutAdminEnhanced(void){
	int ret = 0;
	ulong result = 0;
	word key = 0;
	int iHeader, iFooter;
	int iRet;
	char Menu[lenMnu + 1];
	char amountVal[lenAmt + 1];
	byte Mode = 0;

	mapGetByte(appTerminalMode, Mode);
	memset(amountVal, 0, sizeof(amountVal));

	if(Mode == 0){
		return;
	}

	Cless_Goal_IsAvailable();

	// Open peripherals
	// ****************
	iRet = OpenPeripherals();

	iHeader = IsHeader();       // Save header state
	iFooter = IsFooter();       // Save footer state

	DisplayHeader(_OFF_);  // dissable Header
	DisplayFooter(_OFF_);  // dissable Footer

	// Read date and time
	// ******************

	if (!IsColorDisplay()) {    // B&W terminal?
		// Yes
		DisplayLeds(_OFF_);     // Disable Leds
		DisplayHeader(_OFF_);   // Disable Header
		DisplayFooter(_OFF_);   // Disable Footer
	}

	iRet = GetPpdDisplay();     // Get pinpad type
	CHECK(iRet>=0, lblKO);
	if (iRet == 1){             // Pinpad with character display
		PPS_firstline();        // Display pinpad idle message
		PPS_Display("    WELCOME     ");
	}

	MAPPUTWORD(traManualPan,0,lblKO);


	//check if there is need to do settlement
	fncAutoSettlementChecker();

	// Open standard peripherals just in case
	iRet = OpenPeripherals();
	MAPPUTSTR(appAppLoggedName, "----", lblKO);

	memset(Menu, 0, sizeof(Menu));

	switch (Mode) {
	///// > BILLER MODE
	/////---------------------------------------------------------------------------------------------------------------------------
	case 1:
		switch (result = GL_Dialog_Menu(hGoal, NULL, tzShortCutMenuAdmin, 0, GL_BUTTON_ALL, GL_KEY_0, GL_TIME_MINUTE)) {

		case 0: // PAYBILL
			key = mnuTerminalMode;
			break;
		case 1: // Terminal Mode  Sel.
			key = mnuTerminalPar;
			break;
		case 2: // Terminal Parameters
			key = mnuBillerConfig;
			break;
		case 3: // Loaded Keys check
			key = mnuTMKey;
			break;
		case 4: // Generate TLS keys
			key = mnuGenerateTLSkey;
			break;
		case 5: // IP Setup
			key = mnuCmmIS;
			break;
		case 6: // CLESS Mode Off
			key = mnuClessModeOff;
			break;
		case 7: // Connection Mode
			key = mnuConnMode;
			break;
		case 8: // Show Control Panel
			key = mnuControlPanel;
			break;
		case 9:  // Force PIN CVM
			key = mnuCvmMode;
			break;
		default:
			goto lblEnd;
			break;
		}

		break;
		default:
			goto lblEnd;
			break;
			/////---------------------------------------------------------------------------------------------------------------------------
	}


	if ((result != GL_RESULT_INACTIVITY) && (result != GL_RESULT_TIMEOUT)) {
		ClosePeripherals();                                      // Close standard peripherals
		num2dec(Menu, key, 0);
		MAPPUTSTR(traMnuItm, Menu, lblKO);
		mapPutStr(traAmt, amountVal); //Clear the amount value
		MenuProcessingSelect(key); //ProcessMenu
	}


	lblKO:;

	lblEnd:

	mapPutWord(traManualPan,0);

	// Open peripherals
	// ****************
	iRet = OpenPeripherals();

	DisplayLeds(_ON_);       // Enable Leds
	DisplayHeader(iHeader);  // Restore Header
	DisplayFooter(iFooter);  // Restore Footer

	ClosePeripherals();                                      // Close standard peripherals

	//clear dirty buffer
	MemoryManagerClearGarbage();
}




static void ChangeConnectionMode(void){
	byte ConnMode;
	byte SecuredComms;
	int ret = 0;

	switch (ret = GL_Dialog_Menu(hGoal, "SERVER CONNECTION MODE", tzConnModeMenu, 0, GL_BUTTON_ALL, GL_KEY_0, GL_TIME_MINUTE)) {
	case 1:
		ConnMode = 'T';
		break; // Ethernet MODE active
	case 2:
		ConnMode = 'W';
		break;// WIFI MODE active
	case 0:
	default:
		ConnMode = 'G';
		break; // Default and GPRS/3G/4G MODE active
	}
	mapPutByte(appCommRoute, ConnMode);

	switch (ret = GL_Dialog_Menu(hGoal, "SECURED CONNECTION", tzSLLModeMenu, 0, GL_BUTTON_ALL, GL_KEY_0, GL_TIME_MINUTE)) {
	case 0:
		SecuredComms = 'Y';//SSL and TLS MODE active
		break;
	case 1:
	default:
		SecuredComms = 'X';//SSL and TLS MODE Inactive
		break;
	}
	mapPutByte(appCommSSL,SecuredComms);

	GL_Dialog_Message(hGoal, "", "Done.", GL_ICON_NONE,GL_BUTTON_ALL, GL_TIME_SECOND);

}


int MenuProcessingSelect(word MnuItm) {
	int ret = 0, CardTransaction = FALSE;
	int  NoCard_But_Online = FALSE;
	int  No_Currency = FALSE;
	int  OnlineSaveLog = TRUE;
	int  OnlinePrintReceipt = TRUE;
	char Statement[128];
	char buf[lenMnu + 1];
	int TxnResult = 0;
	char ucBuffer[20];
	word retWord = 0;
	byte ConnDetails = 0;
	char InvoiceNum[12];
	char VoidedSTAN_Val[lenSTAN + 1];

	memset(buf, 0, sizeof(buf));
	memset(InvoiceNum, 0, sizeof(InvoiceNum));
	memset(Statement, 0, sizeof(Statement));
	memset(VoidedSTAN_Val, 0, sizeof(VoidedSTAN_Val));

	//get current transaction date and time
	getDateTime(Statement); //Temporarily used var:Statement for usability

	//SaveTransaction type if Dr or Cr
	memset(Statement, 0, sizeof(Statement));
	Telium_Sprintf (Statement, "SELECT DrCr FROM AppMenus WHERE MenuId = '%d';", MnuItm);
	ret = Sqlite_Run_Statement_MultiRecord(Statement, buf);
	MAPPUTSTR(traDrCr, buf, lblKO);

	memset(Statement, 0, sizeof(Statement));
	memset(buf, 0, sizeof(buf));
	Telium_Sprintf (Statement, "SELECT MenuName FROM AppMenus WHERE MenuId = '%d';", MnuItm);
	ret = Sqlite_Run_Statement_MultiRecord(Statement, buf);
	CHECK(ret > 0, lblKO);
	MAPPUTSTR(traCtx, buf, lblKO);

	///--------- INITIAL DATA ---------------
	num2dec(isoMnuItm, MnuItm, 0);	//this is current selected menu item/original menu item
	orgMnuItm = MnuItm; //this is used to hold the original menu item incase of a reversal
	OnlineSaveLog = TRUE;
	OnlinePrintReceipt = TRUE;

	MAPPUTSTR(traCurrencyLabel, "TZS", lblKO);
	MAPPUTSTR(emvTrnCurCod,"020834",lblKO);
	MAPPUTBYTE(appReversalFlag, 0, lblKO);
	ManageSTAN();
	///--------------------------------------

	CommsGetChannel(ConnDetails);

	switch (MnuItm) {

	// *** Items regarding customer ***
	case mnuSale:
		MAPPUTSTR(traRqsBitMap, "083038078020C80006",lblKO);
		MAPPUTSTR(traRqsMTI, "020200",lblKO);
		MAPPUTSTR(traRqsProcessingCode, "000000",lblKO);
		CardTransaction = TRUE;
		NoCard_But_Online = FALSE;
		break;
	case mnuSaleCB:
		MAPPUTSTR(traRqsBitMap, "083038058020C80006",lblKO);
		MAPPUTSTR(traRqsMTI, "020200",lblKO);
		MAPPUTSTR(traRqsProcessingCode, "090000",lblKO);
		CardTransaction = TRUE;
		NoCard_But_Online = FALSE;
		break;
	case mnuCompletion:
		MAPPUTSTR(traRqsBitMap, "08703C078024C00006",lblKO);
		MAPPUTSTR(traRqsMTI, "020220",lblKO);
		MAPPUTSTR(traRqsProcessingCode, "000000",lblKO);
		CardTransaction = TRUE;
		NoCard_But_Online = FALSE;
		Application_Completion();
		break;
	case mnuDeposit:
		MAPPUTSTR(traRqsBitMap, "083200068000C00200",lblKO);
		MAPPUTSTR(traRqsMTI, "020200",lblKO);
		MAPPUTSTR(traRqsProcessingCode, "210000",lblKO);
		CardTransaction = TRUE;
		NoCard_But_Online = FALSE;
		Application_Get_Destination_Card(traPanContext);
		break;
	case mnuWithdrawal:
		MAPPUTSTR(traRqsBitMap, "083038068020C00206",lblKO);
		MAPPUTSTR(traRqsMTI, "020200",lblKO);
		MAPPUTSTR(traRqsProcessingCode, "010000",lblKO);
		CardTransaction = TRUE;
		NoCard_But_Online = FALSE;
		break;
	case mnuPreaut:
		MAPPUTSTR(traRqsBitMap, "083038058020C80002",lblKO);
		MAPPUTSTR(traRqsMTI, "020100",lblKO);
		//MAPPUTSTR(traRqsProcessingCode, "370000",lblKO);//Old
		MAPPUTSTR(traRqsProcessingCode, "000000",lblKO);//New
		CardTransaction = TRUE;
		NoCard_But_Online = FALSE;
		break;
	case mnuBalanceEnquiry:
		MAPPUTSTR(traRqsBitMap, "08202C078020C00206",lblKO);
		MAPPUTSTR(traRqsMTI, "020100",lblKO);
		MAPPUTSTR(traRqsProcessingCode, "300000",lblKO);
		CardTransaction = TRUE;
		NoCard_But_Online = FALSE;
		break;
	case mnuAgencyDeposit:
		MAPPUTSTR(traRqsBitMap, "083200068000C00200",lblKO);
		MAPPUTSTR(traRqsMTI, "020200",lblKO);
		MAPPUTSTR(traRqsProcessingCode, "010000",lblKO);
		CardTransaction = FALSE;
		NoCard_But_Online = TRUE;
		break;
	case mnuRefund:
		MAPPUTSTR(traRqsBitMap, "083238078020C00006",lblKO);
		MAPPUTSTR(traRqsMTI, "020200",lblKO);
		MAPPUTSTR(traRqsProcessingCode, "200000",lblKO);
		CardTransaction = TRUE;
		NoCard_But_Online = FALSE;
		break;
	case mnuVoid:
		ret = ApplicationVoid(VoidedSTAN_Val);
		CHECK(ret > 0 , lblKO);
		ManageSTAN(); //Make sure the stan on field 11 is consistent
		MAPPUTSTR(traRqsBitMap, "08303805802CC80016",lblKO);
		MAPPUTSTR(traRqsMTI, "020200",lblKO);
		MAPPUTSTR(traRqsProcessingCode, "020000",lblKO);
		CardTransaction = FALSE;
		NoCard_But_Online = TRUE;
		OnlinePrintReceipt = TRUE;
		OnlineSaveLog = TRUE;
		break;
	case mnuReversal:
		MAPPUTSTR(traRqsBitMap, "08303805802CC80016",lblKO);
		MAPPUTSTR(traRqsMTI, "020400",lblKO);
		MAPPUTSTR(traRqsProcessingCode, "000000",lblKO);
		CardTransaction = TRUE;
		NoCard_But_Online = FALSE;
		break;
	case mnuOffline:
		MAPPUTSTR(traRqsBitMap, "083020058020C00404",lblKO);
		MAPPUTSTR(traRqsMTI, "020200",lblKO);
		MAPPUTSTR(traRqsProcessingCode, "000000",lblKO);
		CardTransaction = TRUE;
		NoCard_But_Online = FALSE;
		break;

		// *** Items regarding merchant ***
	case mnuSettlement:
	case mnuCustSettlement:
		//Print traces and logs
		PrintDetailedLog();
		PrintSummaryLog();

		//Now do the settlement
		MAPPUTSTR(traRqsBitMap, "082020030000C00016",lblKO);
		MAPPUTSTR(traRqsMTI, "020500",lblKO);
		MAPPUTSTR(traRqsProcessingCode, "920000",lblKO);
		NoCard_But_Online = FALSE;
		CardTransaction = FALSE;
		OnlinePrintReceipt = FALSE;
		OnlineSaveLog = FALSE;
		logSettlement();
		logReset();
		logIncrementBatch();
		break;
	case mnuMrcChgPwd:
		NoCard_But_Online = FALSE;
		CardTransaction = FALSE;
		//          fncMerchantPwd();
		break;
	case mnuAdmChgPwd:
		NoCard_But_Online = FALSE;
		//          fncAdminPwd();
		break;
	case mnuMrcReset:
		NoCard_But_Online = FALSE;
		CardTransaction = FALSE;
		//          fncReset();
		break;
	case mnuControlPanel:
		NoCard_But_Online = FALSE;
		CardTransaction = FALSE;
		fncShowControlPanel();
		break;
	case mnuSwapSimSlot:
		NoCard_But_Online = FALSE;
		CardTransaction = FALSE;
		fncSwitchSimSlot();
		break;
	case mnuMrcResetRev:
		NoCard_But_Online = FALSE;
		CardTransaction = FALSE;
		//          fncRevReset();
		break;
	case mnuMngUsers:
		NoCard_But_Online = FALSE;
		CardTransaction = FALSE;
		fncUserManager();
		break;
	case mnuClessModeOff:
		NoCard_But_Online = FALSE;
		CardTransaction = FALSE;
		applicationClessMagMode();
		break;
	case mnuCvmMode:
		NoCard_But_Online = FALSE;
		CardTransaction = FALSE;
		applicationCVM_Mode();
		break;
	case mnuUsbTraces:
		NoCard_But_Online = FALSE;
		CardTransaction = FALSE;
		applicationTraces();
		break;

		// *** Items regarding administrator ***
		// *** Items regarding Terminal ***
	case mnuTerminalPar:
		NoCard_But_Online = FALSE;
		CardTransaction = FALSE;
		fncPromptTerminalParameters();
		break;
	case mnuTMKey:
		CardTransaction = FALSE;
		NoCard_But_Online = FALSE;
		//          admMasterKey();
		break;
	case mnuGenerateTLSkey:
		NoCard_But_Online = FALSE;
		CardTransaction = FALSE;
		GenerateKeyAndCSR();
		break;
	case mnuCmmIS:
		NoCard_But_Online = FALSE;
		CardTransaction = FALSE;
		PromptEthernet();
		break;
	case mnuDetailedReport:
		NoCard_But_Online = FALSE;
		CardTransaction = FALSE;
		PrintDetailedLog();
		break;
	case mnuSummaryReport:
		NoCard_But_Online = FALSE;
		CardTransaction = FALSE;
		PrintSummaryLog();
		break;
	case mnuDuplicateReceipt:
		NoCard_But_Online = FALSE;
		CardTransaction = FALSE;
		PrintDuplicateReceipt();
		break;
	case mnuReprintReceipt:
		NoCard_But_Online = FALSE;
		CardTransaction = FALSE;
		PrintReprintReceipt();
		break;
	case mnuLogOn:
		MAPPUTSTR(traRqsBitMap, "082020010000800000",lblKO);
		MAPPUTSTR(traRqsMTI, "020800",lblKO);
		MAPPUTSTR(traRqsProcessingCode, "920000",lblKO);
		NoCard_But_Online = TRUE;
		CardTransaction = FALSE;
		OnlinePrintReceipt = FALSE;
		OnlineSaveLog = FALSE;

		FUN_Initialize_DUKPT();
		break;
	case mnuEchoTest:
		MAPPUTSTR(traRqsBitMap, "082000010000800000",lblKO);
		MAPPUTSTR(traRqsMTI, "020800",lblKO);
		MAPPUTSTR(traRqsProcessingCode, "990000",lblKO);
		NoCard_But_Online = TRUE;
		CardTransaction = FALSE;
		OnlinePrintReceipt = FALSE;
		OnlineSaveLog = FALSE;
		break;
	case mnuConnMode:
		NoCard_But_Online = FALSE;
		CardTransaction = FALSE;
		OnlinePrintReceipt = FALSE;
		OnlineSaveLog = FALSE;
		ChangeConnectionMode();
		break;
	case mnuBiller:
		//Inquiry
		fncBillerInquiry();
		ret = valRspCod();
		CHECK(ret > 0, lblKO);

		//Start Payment
		CardTransaction = TRUE;
		NoCard_But_Online = FALSE;
		No_Currency = TRUE;

		ret = fncBillerBeforePayment();
		CHECK(ret > 0, lblKO);
		break;
	case mnuBillerConfig:
		NoCard_But_Online = FALSE;
		CardTransaction = FALSE;
		OnlinePrintReceipt = FALSE;
		OnlineSaveLog = FALSE;

		fncBillerConfig();
		break;
	case mnuTerminalMode:
		NoCard_But_Online = FALSE;
		CardTransaction = FALSE;
		OnlinePrintReceipt = FALSE;
		OnlineSaveLog = FALSE;

		fncTerminalModeConfig();
		fncProcessTemrinalModes();
		break;
	case mnuBillerResend:
		MAPPUTSTR(traRqsBitMap, "082000010000800000",lblKO);
		MAPPUTSTR(traRqsMTI, "020200",lblKO);
		MAPPUTSTR(traRqsProcessingCode, "000000",lblKO);
		MAPPUTSTR(traPan, "000000",lblKO);

		NoCard_But_Online = TRUE;
		CardTransaction = FALSE;
		OnlinePrintReceipt = TRUE;
		OnlineSaveLog = FALSE;
		break;
	default:
		break;
	}

	if ((CardTransaction || NoCard_But_Online) && (!No_Currency)) {
		retWord = ApplicationSelectCurrency();
		ret = (int)retWord;
		if (retWord < 1) {
			GL_Dialog_Message(hGoal, NULL, "Transaction Cancelled!!", GL_ICON_ERROR, GL_BUTTON_NONE, 100);
			goto lblKO;
		}
	}
	TxnResult = 1;

	if(CardTransaction){

		RestartTxn:
		fncWriteStatusOfConnection('1');//Notify TMS transaction is in session

		TxnResult = Application_Do_Transaction();
		if (TxnResult == CLESS_CR_MANAGER_RESTART_DOUBLE_TAP) {
			goto lblEnd;
		}

		memset(ucBuffer, 0, sizeof(ucBuffer));
		mapGet(traRspCod, ucBuffer, 2);
		if (strncmp((char * )ucBuffer, "55",2) == 0) {

			GL_Dialog_Message(hGoal, NULL, "RE RUNNING TRANSACTION", GL_ICON_INFORMATION, GL_BUTTON_NONE, 100);

			TaskInitiateGPRS();
			memset(ucBuffer, 0, sizeof(ucBuffer));
			mapPut(traPinBlk, ucBuffer,8);
			goto RestartTxn;
		}

		//GOVT Biller payments
		if(MnuItm == mnuBiller) {
			fncBillerAdvice();
		}
	}

	if(NoCard_But_Online) {
		fncWriteStatusOfConnection('1');//Notify TMS transaction is in session

		//Make sure STAN is same in APP and in TRA
		ManageSTAN();

		//Send Online
		onlSession();

		//AB: return code processing and reversal check to be done here
		if(MnuItm != mnuOffline) {
			ret = valRspCod();
			CHECK(ret > 0, lblDeclined);
		}

		if (OnlineSaveLog) {
			ret = logSave();         // Save transaction into log table (Batch)
			CHECK(ret > 0, lblKO);
		}

		//GOVT Biller payments
		if(MnuItm == mnuBiller) {
			fncBillerAdvice();
		}

		lblDeclined:
		if (OnlinePrintReceipt) {
			ret = PrintReceipt();
			CHECK(ret > 0, lblKO);    // Print transaction receipt
		}

		ret = incCard(appSTAN); //Increment Trace number even if its an Offline Transaction
		CHECK(ret >= 0, lblKO);

		if(MnuItm != mnuVoid) {
			ret = incCard(appInvNum);   //Increment Invoice number / ROC when trx is approved
			CHECK(ret > 0, lblKO);
		} else {
			sqlite_CloseVoid(VoidedSTAN_Val);
		}


//		Telium_Read_date (&xDate);
//		Telium_Sprintf (InvoiceNum, "%2.2s%2.2s20%2.2s", xDate.hour, xDate.minute, xDate.second); // Retrieve date
//		mapPut(appInvNum, InvoiceNum, 6);
	}

	// Wait card removal
	if(CardTransaction)
		message_RemoveCard();

	goto lblEnd;
	lblKO:

	goto lblEnd;


	lblEnd:

	//Clear the transaction Buffers of the transaction
	traReset();
	fncWriteStatusOfConnection('0');//Notify TMS transaction is in session
	Cless_Goal_IsAvailable();//Makesure the goal for cless is okay
	return TxnResult;
}
