
/////////////////////////////////////////////////////////////////
//// Includes ///////////////////////////////////////////////////

#include "sdk.h"
#include "SEC_Interface.h"
#include "GL_GraphicLib.h"
#include "TlvTree.h"
#include "GTL_Assert.h"
#include "GTL_Convert.h"
#include "del_lib.h"
#include "TagOS.h"

#include "EPSTOOL_Convert.h"
#include "EPSTOOL_TlvTree.h"
#include "EPSTOOL_PinEntry.h"

#include "EmvLib_Tags.h"
#include "EMV_Status.h"
#include "EMV_ApiTags.h"
#include "EMV_Api.h"

#include "EMV_ServicesEmv.h"
#include "EMV_UserInterfaceDisplay.h"
#include "EMV_ServicesEmv_Stop.h"

#include "MenuManager.h"
#include "Sqlite.h"
#include "globals.h"

#include "crypto.h"


//****************************************************************************
//      EXTERN
//****************************************************************************
extern T_GL_HGRAPHIC_LIB hGoal; // Handle of the graphics object library
extern const char *HypercomResp;

//// Macros & preprocessor definitions //////////////////////////

// Values of EMV Transaction Types
#define TYPE_GOODS_SERVICES     0	//!< Standard transaction.
#define TYPE_CASH 				1	//!< Cash transaction.
#define TYPE_CASHBACK 			2	//!< Cashback transaction.

//// Types //////////////////////////////////////////////////////

//// Static function definitions ////////////////////////////////

static Telium_File_t *hMag13 = NULL;
static Telium_File_t *hMag2 = NULL;
static Telium_File_t *hMag3 = NULL;

///// Old menu Items

// Main menu and sub-menu(s)
// =========================

//static const char *tzCurrencyMenu[] = {
//		"                            ",
//		"                            ",
//		NULL
//};

static const char *tzCurrencyMenu[] = {
		"KES",
		NULL
};


static const char *tzClessModeMenu[] = {
		"Contactless OFF",
		"Contactless ON",
		NULL
};


static const char *tzTracesMenu[] = {
		"ENABLE / DISABLE TRACES",
		"DUMP CLESS CFG",
		NULL
};

static const char *tzPinModeMenu[] = {
		"Force Online PIN",
		"Use card CVM",
		NULL
};

/// new menu Items
static const char *appMenuContent[] = {
		"                                                                                                    ","                                                                                                    ","                                                                                                    ","                                                                                                    ",
		"                                                                                                    ","                                                                                                    ","                                                                                                    ","                                                                                                    ",
		"                                                                                                    ","                                                                                                    ","                                                                                                    ","                                                                                                    ",
		"                                                                                                    ","                                                                                                    ","                                                                                                    ","                                                                                                    ",
		"                                                                                                    ","                                                                                                    ","                                                                                                    ","                                                                                                    ",
		"                                                                                                    ","                                                                                                    ","                                                                                                    ","                                                                                                    ",
		"                                                                                                    ","                                                                                                    ","                                                                                                    ","                                                                                                    ",
		"                                                                                                    ","                                                                                                    ","                                                                                                    ","                                                                                                    ",
		"                                                                                                    ","                                                                                                    ","                                                                                                    ","                                                                                                    ",
		"                                                                                                    ","                                                                                                    ","                                                                                                    ","                                                                                                    ",
		"                                                                                                    ","                                                                                                    ","                                                                                                    ","                                                                                                    ",
		"                                                                                                    ","                                                                                                    ","                                                                                                    ","                                                                                                    ",
		"                                                                                                    ","                                                                                                    ","                                                                                                    ","                                                                                                    ",
		"                                                                                                    ","                                                                                                    ","                                                                                                    ","                                                                                                    ",
		"                                                                                                    ","                                                                                                    ","                                                                                                    ","                                                                                                    ",
		"                                                                                                    ","                                                                                                    ","                                                                                                    ","                                                                                                    ",
		"                                                                                                    ","                                                                                                    ","                                                                                                    ","                                                                                                    ",
		"                                                                                                    ","                                                                                                    ","                                                                                                    ","                                                                                                    ",
		"                                                                                                    ","                                                                                                    ","                                                                                                    ","                                                                                                    ",
		"                                                                                                    ","                                                                                                    ","                                                                                                    ","                                                                                                    ",
		"                                                                                                    ","                                                                                                    ","                                                                                                    ","                                                                                                    ",
		"                                                                                                    ","                                                                                                    ","                                                                                                    ","                                                                                                    ",
		NULL
};

//// Global variables ///////////////////////////////////////////

//// Functions //////////////////////////////////////////////////
static void ResetMenuVars(void){

	memset(var_MnuCustomer, 0, sizeof(var_MnuCustomer));
	memset(var_MnuSale, 0, sizeof(var_MnuSale));
	memset(var_MnuSaleCB, 0, sizeof(var_MnuSaleCB));
	memset(var_MnuDeposit, 0, sizeof(var_MnuDeposit));
	memset(var_MnuWithdrawal, 0, sizeof(var_MnuWithdrawal));
	memset(var_MnuPreaut, 0, sizeof(var_MnuPreaut));
	memset(var_MnuCompletion, 0, sizeof(var_MnuCompletion));
	memset(var_MnuBalanceInquiry, 0, sizeof(var_MnuBalanceInquiry));
	memset(var_MnuMiniSatement, 0, sizeof(var_MnuMiniSatement));
	memset(var_MnuRefund, 0, sizeof(var_MnuRefund));
	memset(var_MnuOffline , 0, sizeof(var_MnuOffline ));
	memset(var_MnuBiller , 0, sizeof(var_MnuBiller ));
	memset(var_MnuVoid, 0, sizeof(var_MnuVoid));
	memset(var_MnuAdjust, 0, sizeof(var_MnuAdjust));
	memset(var_MnuReversal, 0, sizeof(var_MnuReversal));
	memset(var_MnuLogOn , 0, sizeof(var_MnuLogOn ));
	memset(var_MnuEchoTest, 0, sizeof(var_MnuEchoTest));
	memset(var_MnuCustSettlement, 0, sizeof(var_MnuCustSettlement));

	memset(var_MnuAgent , 0, sizeof(var_MnuAgent ));
	memset(var_MnuAgencyDeposit , 0, sizeof(var_MnuAgencyDeposit ));

	memset(var_MnuMerchant, 0, sizeof(var_MnuMerchant));
	memset(var_MnuSettlement, 0, sizeof(var_MnuSettlement));
	memset(var_MnuDetailedRpt , 0, sizeof(var_MnuDetailedRpt ));
	memset(var_MnuSummaryRpt , 0, sizeof(var_MnuSummaryRpt ));
	memset(var_MnuDuplicateReceipt, 0, sizeof(var_MnuDuplicateReceipt));
	memset(var_MnuReprintReceipt, 0, sizeof(var_MnuReprintReceipt));
	memset(var_MnuAdmChgPwd , 0, sizeof(var_MnuAdmChgPwd ));
	memset(var_MnuMrcChgPwd , 0, sizeof(var_MnuMrcChgPwd ));
	memset(var_MnuMrcReset, 0, sizeof(var_MnuMrcReset));
	memset(var_MnuMrcResetRev , 0, sizeof(var_MnuMrcResetRev));
	memset(var_MnuMngUsers , 0, sizeof(var_MnuMngUsers));
	memset(var_MnuBillerResend , 0, sizeof(var_MnuBillerResend));

	memset(var_MnuSupervisor , 0, sizeof(var_MnuSupervisor));

	memset(var_MnuAdmin , 0, sizeof(var_MnuAdmin ));
	memset(var_MnuTerminalPar , 0, sizeof(var_MnuTerminalPar ));
	memset(var_MnuBillerConfig , 0, sizeof(var_MnuBillerConfig ));
	memset(var_MnuTMKey , 0, sizeof(var_MnuTMKey ));
	memset(var_MnuGenerateTLSkey , 0, sizeof(var_MnuGenerateTLSkey ));
	memset(var_MnuCmmIS , 0, sizeof(var_MnuCmmIS ));
	memset(var_MnuClessModeOff , 0, sizeof(var_MnuClessModeOff ));
	memset(var_MnuConnMode , 0, sizeof(var_MnuConnMode));
	memset(var_MnuControlPanel , 0, sizeof(var_MnuControlPanel));
	memset(var_MnuSwapSimSlot , 0, sizeof(var_MnuSwapSimSlot));
	memset(var_MnuCvmMode , 0, sizeof(var_MnuCvmMode));
	memset(var_MnuTraces , 0, sizeof(var_MnuTraces));
	memset(var_MnuTerminalMode, 0, sizeof(var_MnuTerminalMode));
}

void Generate_Menu_Content(void){
	ResetMenuVars();

#ifdef ICONMENU
	//-- Customer Menu --
	/// level 0
	//Telium_Sprintf(var_MnuCustomer,       "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'Transaction>        ', '0', '0','0' ,'1', ' ', 'file://flash/HOST/TU.TAR/icones/customers.png');",       mnuCustomer);             // CUSTOMER>
	/// level 1
	//Telium_Sprintf(var_MnuSale,           "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'Sale             ', '%d', '0','0' ,'1', 'D', 'file://flash/HOST/TU.TAR/icones/purchasing.png');",     mnuSale,mnuCustomer);
	//Telium_Sprintf(var_MnuSaleCB,         "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'Sale + CASHBACK  ', '%d', '1','0' ,'1', 'D', 'file://flash/HOST/TU.TAR/icones/purchasecb.png');",     mnuSaleCB,mnuCustomer);                 // SALE
	Telium_Sprintf(var_MnuDeposit,        "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'Deposit          ', '%d', '1','0' ,'1', 'C', 'file://flash/HOST/TU.TAR/icones/deposit.png');",        mnuDeposit,mnuCustomer);              // DEPOSIT
	Telium_Sprintf(var_MnuWithdrawal,     "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'Withdrawal       ', '%d', '1','0' ,'1', 'D', 'file://flash/HOST/TU.TAR/icones/withdrawal.png');",     mnuWithdrawal,mnuCustomer);           // WITHDRAWAL
//	Telium_Sprintf(var_MnuPreaut,         "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'Preauth          ', '%d', '0','0' ,'1', ' ', 'file://flash/HOST/TU.TAR/icones/preauth.png');",        mnuPreaut,mnuCustomer);               // PREAUTHORIZATION
//	Telium_Sprintf(var_MnuCompletion,     "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'Completion       ', '%d', '0','0' ,'1', 'D', 'file://flash/HOST/TU.TAR/icones/completion.png');",     mnuCompletion,mnuCustomer);                 // COMPLETION
//	Telium_Sprintf(var_MnuBalanceInquiry, "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'Balance Inq      ', '%d', '0','0' ,'1', ' ', 'file://flash/HOST/TU.TAR/icones/balanceinquiry.png');", mnuBalanceEnquiry,mnuCustomer);                 // COMPLETION
//	Telium_Sprintf(var_MnuMiniSatement,   "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'Mini stat        ', '%d', '1','0' ,'1', ' ', 'file://flash/HOST/TU.TAR/icones/ministatement.png');",  mnuMiniStatement,mnuCustomer);                 // COMPLETION
//	Telium_Sprintf(var_MnuPayment,        "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'Payment          ', '%d', '1','0' ,'1', ' ', 'file://flash/HOST/TU.TAR/icones/payment.png');",        mnuPayment,mnuCustomer);                 // COMPLETION
//	Telium_Sprintf(var_MnuRefund,         "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'Refund           ', '%d', '0','1' ,'1', 'C', 'file://flash/HOST/TU.TAR/icones/refund.png');",         mnuRefund,mnuCustomer);               // REFUND
//	Telium_Sprintf(var_MnuOffline,        "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'Offline          ', '%d', '1','0' ,'1', 'D', 'file://flash/HOST/TU.TAR/icones/offline.png');",        mnuOffline,mnuCustomer);              // OFFLINE
//	Telium_Sprintf(var_MnuBiller,         "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'Pay Bill         ', '%d', '0','0' ,'1', 'D', 'file://flash/HOST/TU.TAR/icones/agent.png');",        mnuBiller,mnuCustomer);              // OFFLINE
//	Telium_Sprintf(var_MnuVoid,           "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'Void             ', '%d', '0','0' ,'1', 'C', 'file://flash/HOST/TU.TAR/icones/void.png');",           mnuVoid,mnuCustomer);                 // VOID
//	Telium_Sprintf(var_MnuAdjust,         "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'Adjust           ', '%d', '1','0' ,'1', ' ', 'file://flash/HOST/TU.TAR/icones/adjust.png');",         mnuAdjust,mnuCustomer);               // ADJUST
//	Telium_Sprintf(var_MnuReversal,       "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'Reversal         ', '%d', '1','0' ,'1', 'D', 'file://flash/HOST/TU.TAR/icones/reversal.png');",       mnuReversal,mnuCustomer);             // REVERSAL
//	Telium_Sprintf(var_MnuLogOn,          "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'Logon           ', '%d', '0','0' ,'1', ' ', 'file://flash/HOST/TU.TAR/icones/logon.png');",          mnuLogOn,mnuCustomer);                // LOG ON
//	Telium_Sprintf(var_MnuEchoTest,       "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'Echo Test        ', '%d', '0','0' ,'1', ' ', 'file://flash/HOST/TU.TAR/icones/echotest.png');",       mnuEchoTest,mnuCustomer);                // LOG ON
//	Telium_Sprintf(var_MnuCustSettlement, "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'Settlement       ', '%d', '0','0' ,'2', ' ', 'file://flash/HOST/TU.TAR/icones/settlement.png');",     mnuCustSettlement,mnuCustomer);           // SETTLEMENT
//
//	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//	//-- Merchant Menu --
//	/// level 0
//	Telium_Sprintf(var_MnuMerchant,           "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'Merchant>        ', '0', '0','1' ,'2', ' ', 'file://flash/HOST/TU.TAR/icones/merchant.png');",        mnuMerchant);             // MERCHANT>
//	/// level 1
//	Telium_Sprintf(var_MnuSettlement,         "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'Settlement       ', '%d', '0','0' ,'2', ' ', 'file://flash/HOST/TU.TAR/icones/settlement.png');",     mnuSettlement,mnuMerchant);           // SETTLEMENT
//	Telium_Sprintf(var_MnuDetailedRpt,        "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'Detailed rpt     ', '%d', '0','0' ,'2', ' ', 'file://flash/HOST/TU.TAR/icones/report.png');",         mnuDetailedReport,mnuMerchant);       // DETAILED REPORT
//	Telium_Sprintf(var_MnuSummaryRpt,         "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'Summary rpt      ', '%d', '0','0' ,'2', ' ', 'file://flash/HOST/TU.TAR/icones/summaryreport.png');",  mnuSummaryReport,mnuMerchant);        // SUMMARY REPORT
//	Telium_Sprintf(var_MnuDuplicateReceipt,   "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'Dupl Receipt     ', '%d', '0','0' ,'2', ' ', 'file://flash/HOST/TU.TAR/icones/detailreport.png');",   mnuDuplicateReceipt,mnuMerchant);     // DUPLICATE RECEIPT
//	Telium_Sprintf(var_MnuReprintReceipt,     "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'Reprint Receipt  ', '%d', '0','0' ,'2', ' ', 'file://flash/HOST/TU.TAR/icones/detailreport.png');",   mnuReprintReceipt,mnuMerchant);       // REPRINT RECEIPT
//	Telium_Sprintf(var_MnuAdmChgPwd,          "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'Admin Pass       ', '%d', '0','0' ,'5', ' ', 'file://flash/HOST/TU.TAR/icones/adminpwd.png');",       mnuAdmChgPwd,mnuMerchant);            // ADMIN PASSWORD
//	Telium_Sprintf(var_MnuMrcChgPwd,          "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'Merch Pass       ', '%d', '0','0' ,'2', ' ', 'file://flash/HOST/TU.TAR/icones/merchantpwd.png');",    mnuMrcChgPwd,mnuMerchant);            // MERCH PASSWORD
//	Telium_Sprintf(var_MnuMrcReset,           "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'DEL Batch        ', '%d', '0','0' ,'2', ' ', 'file://flash/HOST/TU.TAR/icones/batchclear.png');",     mnuMrcReset,mnuMerchant);             // DELETE BATCH
//	Telium_Sprintf(var_MnuMrcResetRev,        "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'DEL Reversal     ', '%d', '0','0' ,'2', ' ', 'file://flash/HOST/TU.TAR/icones/deletereversal.png');", mnuMrcResetRev,mnuMerchant);          // DELETE REVERSAL
//	Telium_Sprintf(var_MnuMngUsers,           "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'Manage Users     ', '%d', '0','0' ,'2', ' ', 'file://flash/HOST/TU.TAR/icones/supervisor.png');",     mnuMngUsers,mnuMerchant);             // MANAGE USERS
//	Telium_Sprintf(var_MnuBillerResend,       "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'Biller Resend Adv', '%d', '0','0' ,'2', ' ', 'file://flash/HOST/TU.TAR/icones/agent.png');",          mnuBillerResend,mnuMerchant);         // BILLER RESEND ADVICE
//
//	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//	//-- Supervisor Menu --
//	/// level 0
//	Telium_Sprintf(var_MnuSupervisor,           "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'Supervisor>       ', '0', '1','0' ,'3', ' ', 'file://flash/HOST/TU.TAR/icones/supervisor.png');",     mnuSupervisor);                      // SUPERVISOR>
//
//
//	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//	//-- Agent Menu --
//	/// level 0
//	Telium_Sprintf(var_MnuAgent,         "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'Agent>           ', '0', '1','0' ,'4', ' ', 'file://flash/HOST/TU.TAR/icones/agent.png');",         mnuAgent);                            // AGENT>
//	/// level 1
//	Telium_Sprintf(var_MnuAgencyDeposit, "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'Agency deposit   ', '%d', '0','0' ,'1', 'C', 'file://flash/HOST/TU.TAR/icones/deposit.png');",      mnuAgencyDeposit,mnuAgent);          // SETTLEMENT
//
//
//	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//	//-- Admin Menu --
//	/// level 0
//	Telium_Sprintf(var_MnuAdmin,          "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'Admin>           ', '0', '0','1' ,'5', ' ', 'file://flash/HOST/TU.TAR/icones/administrator.png');",  mnuAdmin);                            // ADMIN>
//	/// level 1
//	Telium_Sprintf(var_MnuTerminalMode,   "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'Terminal Mode  Sel.', '%d', '0','0' ,'1', ' ', 'file://flash/HOST/TU.TAR/icones/clessmode.png');",   mnuTerminalMode,mnuAdmin);
//	Telium_Sprintf(var_MnuTerminalPar,    "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'Terminal Parameters', '%d', '0','0' ,'1', ' ', 'file://flash/HOST/TU.TAR/icones/parameters.png');",  mnuTerminalPar,mnuAdmin);    	           // Master key load
//	Telium_Sprintf(var_MnuBillerConfig,   "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'Biller Configure   ', '%d', '0','0' ,'1', ' ', 'file://flash/HOST/TU.TAR/icones/adjust.png');",      mnuBillerConfig,mnuAdmin);    	           // Master key load
//	Telium_Sprintf(var_MnuTMKey,          "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'Loaded Keys Check  ', '%d', '0','0' ,'1', ' ', 'file://flash/HOST/TU.TAR/icones/pinkey.png');",      mnuTMKey,mnuAdmin);    	           // Master key load
//	Telium_Sprintf(var_MnuGenerateTLSkey, "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'Generate TLS key   ', '%d', '0','0' ,'1', ' ', 'file://flash/HOST/TU.TAR/icones/pinkey.png');",      mnuGenerateTLSkey,mnuAdmin);    	           // Master key load
//	Telium_Sprintf(var_MnuCmmIS,          "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'IP Setup           ', '%d', '0','0' ,'1', ' ', 'file://flash/HOST/TU.TAR/icones/IP.png');",          mnuCmmIS,mnuAdmin);
//	Telium_Sprintf(var_MnuClessModeOff,   "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'CLESS Mode Off     ', '%d', '0','0' ,'1', ' ', 'file://flash/HOST/TU.TAR/icones/clessmode.png');",   mnuClessModeOff,mnuAdmin);
//	Telium_Sprintf(var_MnuConnMode,       "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'Connection Mode    ', '%d', '0','0' ,'1', ' ', 'file://flash/HOST/TU.TAR/icones/connection.png');",  mnuConnMode,mnuAdmin);
//	Telium_Sprintf(var_MnuControlPanel,   "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'Show Control Panel ', '%d', '0','0' ,'1', ' ', 'file://flash/HOST/TU.TAR/icones/controlpanel.png');",mnuControlPanel,mnuAdmin);
//	Telium_Sprintf(var_MnuSwapSimSlot,    "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'Manual SIM Slot Swap', '%d', '0','0' ,'1', ' ', 'file://flash/HOST/TU.TAR/icones/supervisor.png');", mnuSwapSimSlot,mnuAdmin);
//	Telium_Sprintf(var_MnuCvmMode,        "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'Force PIN CVM      ', '%d', '0','0' ,'1', ' ', 'file://flash/HOST/TU.TAR/icones/cvm.png');",         mnuCvmMode,mnuAdmin);
//	Telium_Sprintf(var_MnuTraces,         "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'Trace Cless to USB ', '%d', '1','0' ,'1', ' ', 'file://flash/HOST/TU.TAR/icones/1.png');",           mnuUsbTraces,mnuAdmin);


	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

#else

	//-- Customer Menu --
	/// level 0
	//Telium_Sprintf(var_MnuCustomer,       "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'TRANSACTION>        ', '0', '0','0' ,'1', ' ', 'file://flash/HOST/TU.TAR/icones/customers.png');",       mnuCustomer);             // CUSTOMER>
	/// level 1
	//Telium_Sprintf(var_MnuSale,           "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'SALE             ', '%d', '0','0' ,'1', 'D', 'file://flash/HOST/TU.TAR/icones/purchasing.png');",     mnuSale,mnuCustomer);
	//Telium_Sprintf(var_MnuSaleCB,         "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'SALE and CASHBACK', '%d', '1','0' ,'1', 'D', 'file://flash/HOST/TU.TAR/icones/purchasecb.png');",     mnuSaleCB,mnuCustomer);                 // SALE
	Telium_Sprintf(var_MnuDeposit,        "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'DEPOSIT          ', '%d', '1','0' ,'1', 'C', 'file://flash/HOST/TU.TAR/icones/deposit.png');",        mnuDeposit,mnuCustomer);              // DEPOSIT
	Telium_Sprintf(var_MnuWithdrawal,     "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'WITHDRAWAL       ', '%d', '1','0' ,'1', 'D', 'file://flash/HOST/TU.TAR/icones/withdrawal.png');",     mnuWithdrawal,mnuCustomer);           // WITHDRAWAL
//	Telium_Sprintf(var_MnuPreaut,         "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'PREAUTHORIZATION ', '%d', '0','0' ,'1', ' ', 'file://flash/HOST/TU.TAR/icones/preauth.png');",        mnuPreaut,mnuCustomer);               // PREAUTHORIZATION
//	Telium_Sprintf(var_MnuCompletion,     "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'COMPLETION       ', '%d', '0','0' ,'1', 'D', 'file://flash/HOST/TU.TAR/icones/completion.png');",     mnuCompletion,mnuCustomer);                 // COMPLETION
//	Telium_Sprintf(var_MnuBalanceInquiry, "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'BALANCE ENQUIRY  ', '%d', '0','0' ,'1', ' ', 'file://flash/HOST/TU.TAR/icones/balanceinquiry.png');", mnuBalanceEnquiry,mnuCustomer);                 // COMPLETION
//	Telium_Sprintf(var_MnuMiniSatement,   "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'MINI STATEMENT   ', '%d', '1','0' ,'1', ' ', 'file://flash/HOST/TU.TAR/icones/ministatement.png');",  mnuMiniStatement,mnuCustomer);                 // COMPLETION
//	Telium_Sprintf(var_MnuPayment,        "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'PAYMENT          ', '%d', '1','0' ,'1', ' ', 'file://flash/HOST/TU.TAR/icones/payment.png');",        mnuPayment,mnuCustomer);                 // COMPLETION
//	Telium_Sprintf(var_MnuRefund,         "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'REFUND           ', '%d', '0','1' ,'1', 'C', 'file://flash/HOST/TU.TAR/icones/refund.png');",         mnuRefund,mnuCustomer);               // REFUND
//	Telium_Sprintf(var_MnuBiller,         "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'PAY BILL         ', '%d', '1','0' ,'1', 'D', 'file://flash/HOST/TU.TAR/icones/agent.png');",          mnuBiller,mnuCustomer);              // OFFLINE
//	Telium_Sprintf(var_MnuOffline,        "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'OFFLINE          ', '%d', '1','0' ,'1', 'D', 'file://flash/HOST/TU.TAR/icones/offline.png');",        mnuOffline,mnuCustomer);              // OFFLINE
//	Telium_Sprintf(var_MnuVoid,           "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'VOID             ', '%d', '0','0' ,'1', 'C', 'file://flash/HOST/TU.TAR/icones/void.png');",           mnuVoid,mnuCustomer);                 // VOID
//	Telium_Sprintf(var_MnuAdjust,         "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'ADJUST           ', '%d', '1','0' ,'1', ' ', 'file://flash/HOST/TU.TAR/icones/adjust.png');",         mnuAdjust,mnuCustomer);               // ADJUST
//	Telium_Sprintf(var_MnuReversal,       "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'REVERSAL         ', '%d', '1','0' ,'1', 'D', 'file://flash/HOST/TU.TAR/icones/reversal.png');",       mnuReversal,mnuCustomer);             // REVERSAL
//	Telium_Sprintf(var_MnuLogOn,          "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'LOG ON           ', '%d', '0','0' ,'1', ' ', 'file://flash/HOST/TU.TAR/icones/logon.png');",          mnuLogOn,mnuCustomer);                // LOG ON
//	Telium_Sprintf(var_MnuEchoTest,       "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'ECHO TEST        ', '%d', '0','0' ,'1', ' ', 'file://flash/HOST/TU.TAR/icones/echotest.png');",       mnuEchoTest,mnuCustomer);                // LOG ON
//	Telium_Sprintf(var_MnuCustSettlement, "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'SETTLEMENT       ', '%d', '0','0' ,'2', ' ', 'file://flash/HOST/TU.TAR/icones/settlement.png');",     mnuCustSettlement,mnuCustomer);           // SETTLEMENT
//
//	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//	//-- Merchant Menu --
//	/// level 0
//	Telium_Sprintf(var_MnuMerchant,           "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'MERCHANT>        ', '0', '0','1' ,'2', ' ', 'file://flash/HOST/TU.TAR/icones/merchant.png');",        mnuMerchant);             // MERCHANT>
//	/// level 1
//	Telium_Sprintf(var_MnuSettlement,         "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'SETTLEMENT       ', '%d', '0','0' ,'2', ' ', 'file://flash/HOST/TU.TAR/icones/settlement.png');",     mnuSettlement,mnuMerchant);           // SETTLEMENT
//	Telium_Sprintf(var_MnuDetailedRpt,        "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'DETAILED REPORT  ', '%d', '0','0' ,'2', ' ', 'file://flash/HOST/TU.TAR/icones/report.png');",         mnuDetailedReport,mnuMerchant);       // DETAILED REPORT
//	Telium_Sprintf(var_MnuSummaryRpt,         "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'SUMMARY REPORT   ', '%d', '0','0' ,'2', ' ', 'file://flash/HOST/TU.TAR/icones/summaryreport.png');",  mnuSummaryReport,mnuMerchant);        // SUMMARY REPORT
//	Telium_Sprintf(var_MnuDuplicateReceipt,   "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'DUPLICATE RECEIPT', '%d', '0','0' ,'2', ' ', 'file://flash/HOST/TU.TAR/icones/detailreport.png');",   mnuDuplicateReceipt,mnuMerchant);     // DUPLICATE RECEIPT
//	Telium_Sprintf(var_MnuReprintReceipt,     "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'REPPRINT RECEIPT ', '%d', '0','0' ,'2', ' ', 'file://flash/HOST/TU.TAR/icones/detailreport.png');",   mnuReprintReceipt,mnuMerchant);       // REPRINT RECEIPT
//	Telium_Sprintf(var_MnuAdmChgPwd,          "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'ADMIN PASSWORD   ', '%d', '0','0' ,'5', ' ', 'file://flash/HOST/TU.TAR/icones/adminpwd.png');",       mnuAdmChgPwd,mnuMerchant);            // ADMIN PASSWORD
//	Telium_Sprintf(var_MnuMrcChgPwd,          "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'MERCH PASSWORD   ', '%d', '0','0' ,'2', ' ', 'file://flash/HOST/TU.TAR/icones/merchantpwd.png');",    mnuMrcChgPwd,mnuMerchant);            // MERCH PASSWORD
//	Telium_Sprintf(var_MnuMrcReset,           "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'DELETE BATCH     ', '%d', '0','0' ,'2', ' ', 'file://flash/HOST/TU.TAR/icones/batchclear.png');",     mnuMrcReset,mnuMerchant);             // DELETE BATCH
//	Telium_Sprintf(var_MnuMrcResetRev,        "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'DELETE REVERSAL  ', '%d', '0','0' ,'2', ' ', 'file://flash/HOST/TU.TAR/icones/deletereversal.png');", mnuMrcResetRev,mnuMerchant);          // DELETE REVERSAL
//	Telium_Sprintf(var_MnuMngUsers,           "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'MANAGE USERS     ', '%d', '0','0' ,'2', ' ', 'file://flash/HOST/TU.TAR/icones/supervisor.png');",     mnuMngUsers,mnuMerchant);             // MANAGE USERS
//	Telium_Sprintf(var_MnuBillerResend,       "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'BILLER RESEND ADV', '%d', '1','0' ,'2', ' ', 'file://flash/HOST/TU.TAR/icones/agent.png');",          mnuBillerResend,mnuMerchant);         // Biller Resend Advice
//
//	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//	//-- Supervisor Menu --
//	/// level 0
//	Telium_Sprintf(var_MnuSupervisor,           "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'SUPERVISOR>       ', '0', '1','0' ,'3', ' ', 'file://flash/HOST/TU.TAR/icones/supervisor.png');",     mnuSupervisor);                      // SUPERVISOR>
//
//
//	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//	//-- Agent Menu --
//	/// level 0
//	Telium_Sprintf(var_MnuAgent,         "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'AGENT>           ', '0', '1','0' ,'4', ' ', 'file://flash/HOST/TU.TAR/icones/agent.png');",         mnuAgent);                            // AGENT>
//	/// level 1
//	Telium_Sprintf(var_MnuAgencyDeposit, "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'AGENCY DEPOSIT   ', '%d', '0','0' ,'1', 'C', 'file://flash/HOST/TU.TAR/icones/deposit.png');",      mnuAgencyDeposit,mnuAgent);          // SETTLEMENT
//
//
//	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//	//-- Admin Menu --
//	/// level 0
//	Telium_Sprintf(var_MnuAdmin,        "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'ADMIN>           ', '0', '0','1' ,'5', ' ', 'file://flash/HOST/TU.TAR/icones/administrator.png');",  mnuAdmin);                            // ADMIN>
//	/// level 1
//	Telium_Sprintf(var_MnuTerminalMode,   "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'Terminal Mode  Sel.', '%d', '0','0' ,'1', ' ', 'file://flash/HOST/TU.TAR/icones/clessmode.png');",   mnuTerminalMode,mnuAdmin);
//	Telium_Sprintf(var_MnuTerminalPar,    "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'Terminal Parameters', '%d', '0','0' ,'1', ' ', 'file://flash/HOST/TU.TAR/icones/parameters.png');",  mnuTerminalPar,mnuAdmin);    	           // Master key load
//	Telium_Sprintf(var_MnuBillerConfig,   "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'Biller Configure   ', '%d', '0','0' ,'1', ' ', 'file://flash/HOST/TU.TAR/icones/adjust.png');",      mnuBillerConfig,mnuAdmin);    	           // Master key load
//	Telium_Sprintf(var_MnuTMKey,          "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'Loaded Keys Check  ', '%d', '0','0' ,'1', ' ', 'file://flash/HOST/TU.TAR/icones/pinkey.png');",      mnuTMKey,mnuAdmin);    	           // Master key load
//	Telium_Sprintf(var_MnuGenerateTLSkey, "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'Generate TLS keys  ', '%d', '0','0' ,'1', ' ', 'file://flash/HOST/TU.TAR/icones/pinkey.png');",      mnuGenerateTLSkey,mnuAdmin);    	           // Master key load
//	Telium_Sprintf(var_MnuCmmIS,          "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'IP Setup           ', '%d', '0','0' ,'1', ' ', 'file://flash/HOST/TU.TAR/icones/IP.png');",          mnuCmmIS,mnuAdmin);
//	Telium_Sprintf(var_MnuClessModeOff,   "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'CLESS Mode Off     ', '%d', '0','0' ,'1', ' ', 'file://flash/HOST/TU.TAR/icones/clessmode.png');",   mnuClessModeOff,mnuAdmin);
//	Telium_Sprintf(var_MnuConnMode,       "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'Connection Mode    ', '%d', '0','0' ,'1', ' ', 'file://flash/HOST/TU.TAR/icones/connection.png');",  mnuConnMode,mnuAdmin);
//	Telium_Sprintf(var_MnuControlPanel,   "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'Show Control Panel ', '%d', '0','0' ,'1', ' ', 'file://flash/HOST/TU.TAR/icones/controlpanel.png');",mnuControlPanel,mnuAdmin);
//	Telium_Sprintf(var_MnuSwapSimSlot,    "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'Manual SIM Slot Swap', '%d', '0','0' ,'1', ' ', 'file://flash/HOST/TU.TAR/icones/supervisor.png');", mnuSwapSimSlot,mnuAdmin);
//	Telium_Sprintf(var_MnuCvmMode,        "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'Force PIN CVM      ', '%d', '0','0' ,'1', ' ', 'file://flash/HOST/TU.TAR/icones/cvm.png');",         mnuCvmMode,mnuAdmin);
//	Telium_Sprintf(var_MnuTraces,         "INSERT INTO AppMenus (MenuId, MenuName, MenuIdParent, Hidden, SecureMenu, SecureMenuLevel, DrCr, IconPathName) VALUES ('%d', 'Trace Cless to USB ', '%d', '1','0' ,'1', ' ', 'file://flash/HOST/TU.TAR/icones/1.png');",           mnuUsbTraces,mnuAdmin);

	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

#endif
}



//! \brief The Telium Manager calls this service which allows applications to execute their own functions via menus.
//! \param[in] appliId The application ID.
//! \return Always \a FCT_OK. \a result of CUMORE_Debit execution
static int Start_Emv_Transaction(void){
	int doTransaction;
	TLV_TREE_NODE inputTlvTree;
	unsigned char byteData;
	int previousUiState;
	byte txnType[2];

	memset(txnType, 0, sizeof(txnType));

	doTransaction = TRUE;
	inputTlvTree = TlvTree_New(0);
	if (inputTlvTree != NULL) {
		// By default, we start a standard transaction.
		// Cash or cashback may be known later in the transaction
		byteData = 0x00;
		if (TlvTree_AddChild(inputTlvTree, TAG_EMV_INT_TRANSACTION_TYPE, &byteData, 1) == NULL)
			doTransaction = FALSE;

		// Set the transaction type selected
		byteData = 0x00;
		mapGet(emvTrnTyp, txnType, 2);
		byteData = (char) txnType[1];
		if (TlvTree_AddChild(inputTlvTree, TAG_EMV_TRANSACTION_TYPE, &byteData, 1) == NULL)
			doTransaction = FALSE;

		// Input the transaction amount
		if (doTransaction){
			// Setup the user interface environment
			previousUiState = EMV_UI_TransactionBegin();

			// Perform the transaction
			EMV_ServicesEmv_DoTransaction("CAM0", 4, TRUE, inputTlvTree);

			// Restore the user interface environment
			EMV_UI_TransactionEnd(previousUiState);
		}
	}

	// Release the memory
	EPSTOOL_TlvTree_Release(&inputTlvTree);

	return FCT_OK;
}



static int magTrack1(char *trk1, char *src, char *dst) {
	int ret = 0;                //to be returned: number of tracks read
	unsigned char len = 0;
	char tmp[128];

	if(trk1) {
		memset(tmp, 0, 128);
		ret = Telium_Is_iso1(hMag13, &len, (unsigned char*)tmp);
		CHECK(ret >= ISO_OK, lblEnd);

		if(ret < DEF_SEP) {
			VERIFY(strlen(tmp) < 128);
			src = tmp;
			dst = trk1;
			while(*src) {       //find start sentinel
				if(*src++ == '%')
					break;
			}
			while(*src) {       //copy all data between start and end sentinels
				if(*src == '?')
					break;
				*dst++ = *src++;
			}
		}
	}

	lblEnd:
	return ret;
}

static int magTrack2(char *trk2, char *src, char *dst) {
	int ret = 0;                //to be returned: number of tracks read
	unsigned char len = 0;
	char tmp[128];

	if(trk2) {
		memset(tmp, 0, 128);
		ret = Telium_Is_iso2(hMag2, &len, (unsigned char*)tmp);
		CHECK(ret >= ISO_OK, lblKO);

		switch (ret) {
		case DEF_SEP:
		case DEF_PAR:
		case DEF_LRC:
#ifndef __NON_PAYMENT_CARD__
		case DEF_LUH:
#endif
		case DEF_NUM:
		case NO_DATA:
			goto lblKO;
			break;
		};
#ifndef __NON_PAYMENT_CARD__
		CHECK(ret < DEF_SEP, lblEnd);
#else
		if(ret) {
			CHECK(ret == DEF_LUH, lblEnd);
		} else {
			CHECK(ret < DEF_SEP, lblEnd);
		}
#endif
		VERIFY(strlen(tmp) < 128);
		src = tmp;
		dst = trk2;
		while(*src) {           //find start sentinel
			if(*src++ == 'B')
				break;
		}
		while(*src) {           //copy all data between start and end sentinels
			if(*src == 'F')
				break;
			if(*src == 'D')
				*src = '=';
			*dst++ = *src++;
		}
		CHECK(ret >= 0, lblKO);
	}

	goto lblEnd;

	lblKO:
	return -1;
	lblEnd:
	return ret;
}

static int magTrack3(char *trk3, char *src, char *dst) {
	int ret = 0;                //to be returned: number of tracks read
	unsigned char len = 0;
	char tmp[128];

	if(trk3) {
		memset(tmp, 0, 128);
		ret = Telium_Is_iso3(hMag3, &len, (unsigned char*)tmp);
		CHECK(ret >= ISO_OK, lblEnd);

		if(ret < DEF_SEP) {
			VERIFY(strlen(tmp) < 128);
			src = tmp;
			dst = trk3;
			while(*src) {       //find start sentinel
				if(*src++ == 'B')
					break;
			}
			while(*src) {       //copy all data between start and end sentinels
				if(*src == 'F')
					break;
				if(*src == 'D')
					*src = '=';
				if(*src == '?')
					break;
				*dst++ = *src++;
			}
		}
	}

	lblEnd:
	return ret;
}

static void getKeyPressed(char * key){
	byte keyTemp = 0;

	keyTemp = Telium_Getchar();
	key[0] = (char)keyTemp;
}



////! \brief Wait card insertion before an EMV transaction.
//static int Application_Get_Card(byte * buf){
//	int chipCardPresent = FALSE, ret = 0, num = 0;
//	int clessCardPresent = FALSE;
//	int magCardPresent = FALSE;
//	int kbdPresent = FALSE;
//	Telium_File_t* hCam;
//	Telium_File_t* keyboard;
//	unsigned char ucStatus;
//	unsigned int sta;
//	char trk1[128 + 1],trk2[128 + 1],trk3[128 + 1];
//	char *src = 0; //magcard definition
//	char *dst = 0; //magcard definition
//	int returnVal = 0;
//	char PAN[lenPan + 1];
//	card keySelected = 0;
//	byte fallBack = 0;
//	byte clessOff = 0;
//	card IdleTransaction = 0;
//	word ManualPan = 0;
//	int kbdInternal = 0;
//
//	hCam = NULL;
//	keyboard = NULL;
//
//	chipCardPresent = FALSE;
//	clessCardPresent = FALSE;
//	magCardPresent = FALSE;
//
//	MAPGETWORD(traManualPan, ManualPan,lblKO);
//
////	if (ManualPan == 1) {
////		GL_Dialog_Message(hGoal, NULL, "Press 1 to Enter\n PAN, Insert, Swipe or\n tap card", GL_ICON_INFORMATION, GL_BUTTON_NONE, 0);
////	} else {
////		GL_Dialog_Message(hGoal, NULL, "Insert, Swipe or\n tap card", GL_ICON_INFORMATION, GL_BUTTON_NONE, 0);
////	}
////
//	if (ManualPan == 1) {
//		GL_Dialog_Message(hGoal, NULL, "Press 1 to Enter\n PAN, Insert, Swipe or\n tap card", GL_ICON_INFORMATION, GL_BUTTON_NONE, 0);
//	} else {
//		GL_Dialog_Message(hGoal, NULL, "Swipe card", GL_ICON_INFORMATION, GL_BUTTON_NONE, 0);
//	}
//
//	mapGetByte(appFallback, fallBack);
//
//	mapGetByte(appClessModeOff, clessOff);
//
//	MAPGETCARD(traIdleTransaction, IdleTransaction, lblKO);
//	if (IdleTransaction != 0) {
//		returnVal = IdleTransaction;
//		if (IdleTransaction == CAM0) {
//			chipCardPresent = TRUE;
//		} else if (IdleTransaction == SWIPE2) {
//			magCardPresent = TRUE;
//		} else if (IdleTransaction == CLESS) {
//			clessCardPresent = TRUE;
//		} else if (IdleTransaction == KEYBOARD) {
//			kbdPresent = TRUE;
//		}
//	}else{
//		chipCardPresent = FALSE;
//		clessCardPresent = FALSE;
//		magCardPresent = FALSE;
//	}
//	// Open peripherals
//	// ================
//	if (IsISO1() == 1) {
//		hMag13 = Telium_Fopen("SWIPE31", "r*");       // Open mag1 peripheral
//		//		CHECK(hMag13, lblKO);
//	}
//	if (IsISO2() == 1) {
//		hMag2 = Telium_Fopen("SWIPE2", "r*");         // Open mag2 peripheral
//		//		CHECK(hMag2, lblKO);
//	}
//	if (IsISO3() == 1) {
//		hMag3 = Telium_Fopen("SWIPE3", "r*");         // Open mag3 peripheral
//		//		CHECK(hMag3, lblKO);
//	}
//
////	if (clessOff != 1) {
////		if (fallBack != 1) {
////			if (IsCless() == 1) {
////				ClessEmv_DetectCardsStop();
////
////				Cless_ExplicitSelection_GlobalCardDetectionCancel(); //close the cless detecting dll
////
////				// Close driver ( and cless field)
////				ClessEmv_CloseDriver();
////
////				// Open driver (and cless field)
////				ClessEmv_OpenDriver ();
////
////				// Start the asynchronous ISO14443-4 contactless detection
////				ClessEmv_DetectCardsStart(1, CL_TYPE_AB);
////
////				// Launch the card detection
////				Cless_ExplicitSelection_GlobalCardDetection();
////			}
////
////			hCam = Telium_Stdperif("CAM0", NULL);
////			while (hCam != NULL) {
////				Telium_Fclose(hCam);
////				hCam = NULL;
////				hCam = Telium_Stdperif("CAM0", NULL);
////			}
////			// chip handle
////			hCam = Telium_Fopen("CAM0", "rw");
////		}
////	}
//
//	// keyboard handle
////	keyboard = Telium_Stdperif("KEYBOARD", NULL);               // Check "display" peripheral already opened? (done by Manager)
////	if(keyboard == NULL){
////		kbdInternal = 1;
////		keyboard = Telium_Fopen("KEYBOARD", "r*");
////	}
//
////	if (fallBack != 1) {
////		// check for a pre inserted card
////		chipCardPresent = FALSE;
////		if (hCam != NULL) {
////			Telium_Status(hCam, &ucStatus);
////			if ((ucStatus & CAM_PRESENT) != 0) {
////				// A card is present
////				chipCardPresent = TRUE;
////				kbdPresent = FALSE;
////				clessCardPresent = FALSE;
////				magCardPresent = FALSE;
////				returnVal = CAM0;
////			}
////		}
////	}
//
//	// Process magstripe cards
//	while(!magCardPresent){
//		sta = Telium_Ttestall(SWIPE31 | SWIPE2 | SWIPE3, 2);
//		if( (sta & SWIPE31) || (sta & SWIPE2) || (sta & SWIPE3) ){
//			sta |= Telium_Ttestall(sta ^ (SWIPE31 | SWIPE2 | SWIPE3), 10); // Wait for the second event SWIPE2
//			sta |= Telium_Ttestall(sta ^ (SWIPE31 | SWIPE2 | SWIPE3), 10); // Wait for the third event SWIPE3
//			if(sta & SWIPE31) {
//				ret = magTrack1(trk1, src, dst);
//				if((ret >= ISO_OK) && (ret < DEF_SEP)) num++;
//				strncat((char *)buf, trk1, 128);
//			}
//
//			if(sta & SWIPE2) {
//				ret = magTrack2(trk2, src, dst);
//				CHECK(ret >= ISO_OK, lblKO);
//				num++;
//				strncat((char *)&buf[128], trk2, 128);
//			}
//
//			if(sta & SWIPE3) {
//				ret = magTrack3(trk3, src, dst);
//				if((ret < DEF_SEP) && (ret >= ISO_OK)) num++;
//				strncat((char *)&buf[256], trk2, 128);
//			}
//
//			magCardPresent = TRUE;
//			kbdPresent = FALSE;
//			chipCardPresent = FALSE;
//			clessCardPresent = FALSE;
//			returnVal = SWIPE2;
//		}
//	}
//
//	//while(!(((chipCardPresent) || (clessCardPresent)) || (magCardPresent))) {
////	while(!(((chipCardPresent) || (clessCardPresent)) || (magCardPresent) || (kbdPresent))) {
////		if ((fallBack != 1) && (clessOff != 1)) {
////			sta = Telium_Ttestall(CAM0 | SWIPE31 | SWIPE2 | SWIPE3 | CLESS | KEYBOARD, 2);
////
////			///------ process chip cards ------
////			Telium_Status(hCam, &ucStatus);
////			if ((ucStatus & CAM_PRESENT) != 0) {
////				// A card is present
////				chipCardPresent = TRUE;
////				kbdPresent = FALSE;
////				clessCardPresent = FALSE;
////				magCardPresent = FALSE;
////				returnVal = CAM0;
////			}
////		} else {
////			sta = Telium_Ttestall(SWIPE31 | SWIPE2 | SWIPE3 | KEYBOARD, 2);
////		}
////
////		///------ process magstripe cards ------
////		if( (sta & SWIPE31) || (sta & SWIPE2) || (sta & SWIPE3) ){
////			sta |= Telium_Ttestall(sta ^ (SWIPE31 | SWIPE2 | SWIPE3), 10); // Wait for the second event SWIPE2
////			sta |= Telium_Ttestall(sta ^ (SWIPE31 | SWIPE2 | SWIPE3), 10); // Wait for the third event SWIPE3
////
////
////			if(sta & SWIPE31) {
////				ret = magTrack1(trk1, src, dst);
////				if((ret >= ISO_OK) && (ret < DEF_SEP)) {
////					num++;
////				}
////				strncat((char *)buf, trk1, 128);
////			}
////
////			if(sta & SWIPE2) {
////				ret = magTrack2(trk2, src, dst);
////				CHECK(ret >= ISO_OK, lblKO);
////				num++;
////				strncat((char *)&buf[128], trk2, 128);
////			}
////
////			if(sta & SWIPE3) {
////				ret = magTrack3(trk3, src, dst);
////				if((ret < DEF_SEP) && (ret >= ISO_OK)) {
////					num++;
////				}
////				strncat((char *)&buf[256], trk2, 128);
////			}
////
////			magCardPresent = TRUE;
////			kbdPresent = FALSE;
////			chipCardPresent = FALSE;
////			clessCardPresent = FALSE;
////			returnVal = SWIPE2;
////		}
////
////		if (clessOff != 1) {
////
////			///------ process cless cards ------
////			if (sta & CLESS) {
////				clessCardPresent = TRUE;
////				kbdPresent = FALSE;
////				chipCardPresent = FALSE;
////				magCardPresent = FALSE;
////				returnVal = CLESS;
////			}
////
////		}
////
////		///------ process Keyboard event ------
////		MAPGETWORD(traManualPan, ManualPan,lblKO);
////		if (sta & KEYBOARD) {
////			memset(PAN, 0, sizeof(PAN));
////			getKeyPressed(PAN);
////			if (PAN[0] == kbdANN)  {
////				GL_Dialog_Message(hGoal, NULL, "Transaction Cancelled...", GL_ICON_INFORMATION, GL_BUTTON_NONE, 0);
////				kbdPresent = TRUE;
////				clessCardPresent = FALSE;
////				chipCardPresent = FALSE;
////				magCardPresent = FALSE;
////				returnVal = KEYBOARD;
////				strncat((char *)buf, PAN, 1);
////
////			} else {
////				if (ManualPan == 1) {
////					hex2num(&keySelected, PAN, 1);
////
////					switch (keySelected) {
////					case 1: case 2:
////					case 3: case 4:
////					case 5: case 6:
////					case 7: case 8:
////					case 9: case 10:
////
////						MAPPUTSTR(traPan, PAN, lblKO);
////						kbdPresent = TRUE;
////						clessCardPresent = FALSE;
////						chipCardPresent = FALSE;
////						magCardPresent = FALSE;
////						returnVal = KEYBOARD;
////						strncat((char *)buf, PAN, 1);
////
////						break;
////					default:
////						if (PAN[0] == '0' || ((PAN[0] != kbdVAL) && (PAN[0] != kbd00) && (PAN[0] != kbdCOR) && (PAN[0] != kbdINI) && (PAN[0] != kbdR) && (PAN[0] != kbdF1) && (PAN[0] != kbdF2) && (PAN[0] != kbdF3) && (PAN[0] != kbdF4) && (PAN[0] != kbdUP) && (PAN[0] != kbdDN)))  {
////
////							MAPPUTSTR(traPan, PAN, lblKO);
////							kbdPresent = TRUE;
////							clessCardPresent = FALSE;
////							chipCardPresent = FALSE;
////							magCardPresent = FALSE;
////							returnVal = KEYBOARD;
////							strncat((char *)buf, PAN, 1);
////
////						} else {
////							///Do nothing wait for other keys to be pressed or card entry
////						}
////
////						break;
////					}
////				}// End of "if (ManualPan == 1) {"
////
////
////			}
////		} //End of "if (sta & KEYBOARD) {"
////	}
//
//	goto lblEnd;
//
//
//	lblKO:
//	goto lblEnd;
//
//	lblEnd:
//
////	if (hCam != NULL) {
////		Telium_Fclose(hCam);
////		hCam = NULL;
////	}
////
////	if ((keyboard != NULL) && (kbdInternal)) {
////		Telium_Fclose(keyboard);
////		keyboard = NULL;
////	}
//
//	if (hMag2 != NULL) {
//		Telium_Fclose(hMag2);
//		hMag2 = NULL;
//	}
//
//	if (hMag3 != NULL) {
//		Telium_Fclose(hMag3);
//		hMag3 = NULL;
//	}
//
//	if (hMag13 != NULL) {
//		Telium_Fclose(hMag13);
//		hMag13 = NULL;
//	}
//
//	ClessEmv_DetectCardsStop();
//	Cless_ExplicitSelection_GlobalCardDetectionCancel(); //close the cless detecting dll
//	ClessEmv_CloseDriver();
//
//	return returnVal;
//}


//! \brief Wait card insertion before an EMV transaction.
static int Application_Get_Card(byte * buf){
	int magCardPresent = FALSE, ret = 0, num = 0;
	unsigned int sta;
	char trk1[128 + 1],trk2[128 + 1],trk3[128 + 1];
	char *src = 0; //magcard definition
	char *dst = 0; //magcard definition
	int returnVal = 0;

	GL_Dialog_Message(hGoal, NULL, "Swipe card", GL_ICON_INFORMATION, GL_BUTTON_NONE, 0);

	// Open peripherals
	// ================
	if (IsISO1() == 1) {
		hMag13 = Telium_Fopen("SWIPE31", "r*");       // Open mag1 peripheral
		//		CHECK(hMag13, lblKO);
	}
	if (IsISO2() == 1) {
		hMag2 = Telium_Fopen("SWIPE2", "r*");         // Open mag2 peripheral
		//		CHECK(hMag2, lblKO);
	}
	if (IsISO3() == 1) {
		hMag3 = Telium_Fopen("SWIPE3", "r*");         // Open mag3 peripheral
		//		CHECK(hMag3, lblKO);
	}

	// Process magstripe cards
	while(!magCardPresent){
		sta = Telium_Ttestall(SWIPE31 | SWIPE2 | SWIPE3, 2);
		if( (sta & SWIPE31) || (sta & SWIPE2) || (sta & SWIPE3) ){
			sta |= Telium_Ttestall(sta ^ (SWIPE31 | SWIPE2 | SWIPE3), 10); // Wait for the second event SWIPE2
			sta |= Telium_Ttestall(sta ^ (SWIPE31 | SWIPE2 | SWIPE3), 10); // Wait for the third event SWIPE3
			if(sta & SWIPE31) {
				ret = magTrack1(trk1, src, dst);
				if((ret >= ISO_OK) && (ret < DEF_SEP)) num++;
				strncat((char *)buf, trk1, 128);
			}

			if(sta & SWIPE2) {
				ret = magTrack2(trk2, src, dst);
				CHECK(ret >= ISO_OK, lblKO);
				num++;
				strncat((char *)&buf[128], trk2, 128);
			}

			if(sta & SWIPE3) {
				ret = magTrack3(trk3, src, dst);
				if((ret < DEF_SEP) && (ret >= ISO_OK)) num++;
				strncat((char *)&buf[256], trk2, 128);
			}

			magCardPresent = TRUE;
			returnVal = SWIPE2;
		}
	}

	goto lblEnd;

	lblKO:
	goto lblEnd;

	lblEnd:

	if (hMag2 != NULL) {
		Telium_Fclose(hMag2);
		hMag2 = NULL;
	}

	if (hMag3 != NULL) {
		Telium_Fclose(hMag3);
		hMag3 = NULL;
	}

	if (hMag13 != NULL) {
		Telium_Fclose(hMag13);
		hMag13 = NULL;
	}

	return returnVal;
}



//! \brief Wait card insertion before an EMV transaction.
static int Application_Get_Card_Minimal(byte * buf){
	int ret = 0, num = 0;
	Telium_File_t* hCam;
	Telium_File_t* keyboard;
	unsigned char ucStatus;
	unsigned int sta;
	char trk1[128 + 1],trk2[128 + 1],trk3[128 + 1];
	char *src = 0; //magcard definition
	char *dst = 0; //magcard definition
	int returnVal = 0;
	char PAN[lenPan + 1];
	card keySelected = 0;
	byte fallBack = 0;
	byte clessOff = 0;
	word ManualPan = 0;
	int kbdInternal = 0;


	hCam = NULL;
	keyboard = NULL;

	mapGetByte(appFallback, fallBack);

	mapGetByte(appClessModeOff, clessOff);

	// Open peripherals
	// ================
	if (IsISO1() == 1) {
		hMag13 = Telium_Fopen("SWIPE31", "r*");       // Open mag1 peripheral
		//		CHECK(hMag13, lblKO);
	}
	if (IsISO2() == 1) {
		hMag2 = Telium_Fopen("SWIPE2", "r*");         // Open mag2 peripheral
		//		CHECK(hMag2, lblKO);
	}
	if (IsISO3() == 1) {
		hMag3 = Telium_Fopen("SWIPE3", "r*");         // Open mag3 peripheral
		//		CHECK(hMag3, lblKO);
	}

	if (clessOff != 1) {
		if (fallBack != 1) {
			if (IsCless() == 1) {
				ClessEmv_DetectCardsStop();

				Cless_ExplicitSelection_GlobalCardDetectionCancel(); //close the cless detecting dll

				// Close driver ( and cless field)
				ClessEmv_CloseDriver();

				// Open driver (and cless field)
				ClessEmv_OpenDriver ();

				// Start the asynchronous ISO14443-4 contactless detection
				ClessEmv_DetectCardsStart(1, CL_TYPE_AB);

				// Launch the card detection
				Cless_ExplicitSelection_GlobalCardDetection();
			}

			// chip handle
			hCam = Telium_Fopen("CAM0", "rw");
		}
	}

	// keyboard handle
	keyboard = Telium_Stdperif("KEYBOARD", NULL);               // Check "display" peripheral already opened? (done by Manager)
	if(keyboard == NULL){
		kbdInternal = 1;
		keyboard = Telium_Fopen("KEYBOARD", "r*");
	}

	if (fallBack != 1) {
		// check for a pre inserted card
		if (hCam != NULL) {
			Telium_Status(hCam, &ucStatus);
			if ((ucStatus & CAM_PRESENT) != 0) {
				// A card is present
				returnVal = CAM0;
			}
		}
	}

	if ((fallBack != 1) && (clessOff != 1)) {
		sta = Telium_Ttestall(CAM0 | SWIPE31 | SWIPE2 | SWIPE3 | CLESS | KEYBOARD, 100);

		///------ process chip cards ------
		Telium_Status(hCam, &ucStatus);
		if ((ucStatus & CAM_PRESENT) != 0) {
			// A card is present
			returnVal = CAM0;
		}
	} else {
		sta = Telium_Ttestall(SWIPE31 | SWIPE2 | SWIPE3 | KEYBOARD, 100);
	}

	///------ process magstripe cards ------
	if( (sta & SWIPE31) || (sta & SWIPE2) || (sta & SWIPE3) ){
		sta |= Telium_Ttestall(sta ^ (SWIPE31 | SWIPE2 | SWIPE3), 100); // Wait for the second event SWIPE2
		sta |= Telium_Ttestall(sta ^ (SWIPE31 | SWIPE2 | SWIPE3), 100); // Wait for the third event SWIPE3


		if(sta & SWIPE31) {
			ret = magTrack1(trk1, src, dst);
			if((ret >= ISO_OK) && (ret < DEF_SEP)) {
				num++;
			}
			strncat((char *)buf, trk1, 128);
		}

		if(sta & SWIPE2) {
			ret = magTrack2(trk2, src, dst);
			CHECK(ret >= ISO_OK, lblKO);
			num++;
			strncat((char *)&buf[128], trk2, 128);
		}

		if(sta & SWIPE3) {
			ret = magTrack3(trk3, src, dst);
			if((ret < DEF_SEP) && (ret >= ISO_OK)) {
				num++;
			}
			strncat((char *)&buf[256], trk2, 128);
		}

		returnVal = SWIPE2;
	}

	if (clessOff != 1) {

		///------ process cless cards ------
		if (sta & CLESS) {
			returnVal = CLESS;
		}
	}

	MAPGETWORD(traManualPan, ManualPan,lblKO);

	///------ process Keyboard event ------
	if (sta & KEYBOARD) {
		memset(PAN, 0, sizeof(PAN));
		getKeyPressed(PAN);
		if (PAN[0] == kbdANN)  {
			returnVal = KEYBOARD;
			strncat((char *)buf, PAN, 1);
		} else {
			if (ManualPan == 1) {
				hex2num(&keySelected, PAN, 1);

				switch (keySelected) {
				case 1: case 2:
				case 3: case 4:
				case 5: case 6:
				case 7: case 8:
				case 9: case 10:

					MAPPUTSTR(traPan, PAN, lblKO);
					returnVal = KEYBOARD;
					strncat((char *)buf, PAN, 1);

					break;
				default:
					if (PAN[0] == '0' || ((PAN[0] != kbdVAL) && (PAN[0] != kbd00) && (PAN[0] != kbdCOR) && (PAN[0] != kbdINI) && (PAN[0] != kbdR) && (PAN[0] != kbdF1) && (PAN[0] != kbdF2) && (PAN[0] != kbdF3) && (PAN[0] != kbdF4) && (PAN[0] != kbdUP) && (PAN[0] != kbdDN)))  {

						MAPPUTSTR(traPan, PAN, lblKO);
						returnVal = KEYBOARD;
						strncat((char *)buf, PAN, 1);

					} else {
						///Do nothing wait for other keys to be pressed or card entry
					}
					break;
				}
			}// End of "if (ManualPan == 1) {"
		}
	} //End of "if (sta & KEYBOARD) {"


	goto lblEnd;


	lblKO:
	goto lblEnd;

	lblEnd:

	if (hCam != NULL) {
		Telium_Fclose(hCam);
		hCam = NULL;
	}

	if ((keyboard != NULL) && (kbdInternal)){
		Telium_Fclose(keyboard);
		keyboard = NULL;
	}

	if (hMag2 != NULL) {
		Telium_Fclose(hMag2);
		hMag2 = NULL;
	}

	if (hMag3 != NULL) {
		Telium_Fclose(hMag3);
		hMag3 = NULL;
	}

	if (hMag13 != NULL) {
		Telium_Fclose(hMag13);
		hMag13 = NULL;
	}

	ClessEmv_DetectCardsStop();
	Cless_ExplicitSelection_GlobalCardDetectionCancel(); //close the cless detecting dll
	ClessEmv_CloseDriver();

	return returnVal;
}


//! \brief Wait card insertion before an EMV transaction.
int Application_Get_Card_Idle(byte * buf){
	int chipCardPresent = FALSE;
	int clessCardPresent = FALSE;
	int magCardPresent = FALSE;
	int kbdPresent = FALSE;
	Telium_File_t* hCam;
	unsigned char ucStatus;

	int returnVal = 0;
	hCam = NULL;

	// chip handle
	hCam = Telium_Fopen("CAM0", "rw");

	// check for a pre inserted card
	chipCardPresent = FALSE;
	if (hCam != NULL) {
		Telium_Status(hCam, &ucStatus);
		if ((ucStatus & CAM_PRESENT) != 0) {
			// A card is present
			chipCardPresent = TRUE;
			kbdPresent = FALSE;
			clessCardPresent = FALSE;
			magCardPresent = FALSE;
			returnVal = CAM0;
		}
	}

	while(!(((chipCardPresent) || (clessCardPresent)) || (magCardPresent) || (kbdPresent))) {

		Telium_Ttestall(CAM0 | SWIPE31 | SWIPE2 | SWIPE3 | CLESS | KEYBOARD, 2);

		///------ process chip cards ------
		Telium_Status(hCam, &ucStatus);
		if ((ucStatus & CAM_PRESENT) != 0) {
			// A card is present
			chipCardPresent = TRUE;
			kbdPresent = FALSE;
			clessCardPresent = FALSE;
			magCardPresent = FALSE;
			returnVal = CAM0;
		}

	}

	return returnVal;
}


void applicationClessMagMode(void){
	int ret = 0;

	switch (ret=GL_Dialog_Menu(hGoal, "SWITCH CLESS MODE", tzClessModeMenu, 0, GL_BUTTON_ALL, GL_KEY_0, GL_TIME_MINUTE)) {
	case 0:
		MAPPUTBYTE(appClessModeOff, 1, lblKO);
		break; //MSTRIPE MODE active
	case 1:
	default:
		MAPPUTBYTE(appClessModeOff, 0, lblKO);
		break; // Abort/Timeout
	}

	lblKO:;
}

void applicationTraces(void){
	ulong Result = 0;

	Result = GL_Dialog_Menu(hGoal, "TRACE OPTIONS", tzTracesMenu, 0, GL_BUTTON_ALL, GL_KEY_0, GL_TIME_MINUTE);

	switch (Result) {
	case 0:
		__Cless_Menu_TestChooseOutput();
		__Cless_Menu_TestTransactionData();
		__Cless_Menu_TestTransactionDumpDatabase();
		__Cless_Menu_TestTraces();
		break;
	case 1:
	default:
		__Cless_Menu_ParametersDump();
		break;
	}

}

void applicationCVM_Mode(void){
	int ret = 0;
	ulong Result = 0;

	Result = GL_Dialog_Menu(hGoal, "SELECT PIN MODE", tzPinModeMenu, 0, GL_BUTTON_ALL, GL_KEY_0, GL_TIME_MINUTE);

	switch (Result) {
	case 0:
		MAPPUTBYTE(appCVM_ForcePIN, 1, lblKO);
		break; //Force Online PIN
	case 1:
	default:
		MAPPUTBYTE(appCVM_ForcePIN, 0, lblKO);
		break; // Card CVM
	}

	lblKO:;
}

word ApplicationSelectCurrency(void){
	int ret = 0;
	char Title[32];
	char TID[lenTID + 1];
	char MID[lenMid + 1];
	char CurrencyCodeAlpha[3 + 1];
	char CurrencyCodeNumeric[3 + 1];
	char Temp[10 + 1];
	char TempCurrencyName[20 + 1];
	ulong Result = 0;
	word returnWord = 0;

	char MenuCurrContent[40][100];

	memset(TID, 0, sizeof(TID));
	memset(MID, 0, sizeof(MID));
	memset(Temp, 0, sizeof(Temp));
	memset(Title, 0, sizeof(Title));
	memset(TempCurrencyName, 0, sizeof(TempCurrencyName));
	memset(CurrencyCodeAlpha, 0, sizeof(CurrencyCodeAlpha));
	memset(CurrencyCodeNumeric, 0, sizeof(CurrencyCodeNumeric));

	strcpy(Title, "Select Currency");
	strncpy(Temp, "020", 3);

	//Get First Currency
	memset(TempCurrencyName, 0, sizeof(TempCurrencyName));
	memset(MenuCurrContent[0], 0, sizeof(MenuCurrContent[0]));
	MAPGET(appCurrCodeAlpha1, TempCurrencyName,lblKO); //First Currency
	strcpy(MenuCurrContent[0], TempCurrencyName);
	tzCurrencyMenu[0] = MenuCurrContent[0];

	//Get Second Currency
	memset(TempCurrencyName, 0, sizeof(TempCurrencyName));
	memset(MenuCurrContent[1], 0, sizeof(MenuCurrContent[1]));
	MAPGET(appCurrCodeAlpha2, TempCurrencyName, lblKO); //Second Currency
	strcpy(MenuCurrContent[1], TempCurrencyName);
	tzCurrencyMenu[1] = MenuCurrContent[1];


	MAPPUTSTR(traTLS_PositionMarker, "0",lblKO);//Make sure TLS knows this is postion start

	lblRedo:
	Result = GL_Dialog_Menu(hGoal, Title, tzCurrencyMenu, 0, GL_BUTTON_ALL, GL_KEY_0, GL_TIME_MINUTE) ;
	CHECK(Result != GL_KEY_CANCEL, lblKO);
	CHECK(Result != GL_KEY_CORRECTION, lblRedo);

	switch (Result) {
	case 1:
		MAPGET(appCurrCodeAlpha2,CurrencyCodeAlpha,lblKO);
		MAPGET(appCurrCodeNumerc2,CurrencyCodeNumeric,lblKO);
		MAPGET(appTID_2, TID, lblKO);
		MAPGET(appMID_2, MID, lblKO);
		returnWord = appTID_2;
		break; //The second Currency
	case 0:
		MAPGET(appCurrCodeAlpha1,CurrencyCodeAlpha,lblKO);
		MAPGET(appCurrCodeNumerc1,CurrencyCodeNumeric,lblKO);
		MAPGET(appTID_1, TID, lblKO);
		MAPGET(appMID_1, MID, lblKO);
		returnWord = appTID_1;
		//First Currency
		break; // TZS
	default:
		returnWord = 0;
		break; // Abort/Timeout
	}

	strncat(Temp, CurrencyCodeNumeric, 3);
	MAPPUTSTR(appCurrCodeAlpha, CurrencyCodeAlpha,lblKO);
	MAPPUTSTR(traCurrencyLabel, CurrencyCodeAlpha,lblKO);
	MAPPUTSTR(emvTrnCurCod,Temp,lblKO);
	MAPPUTSTR(traCurrencyNum,CurrencyCodeNumeric,lblKO);

	MAPPUTSTR(appTID,TID,lblKO);
	MAPPUTSTR(appMID,MID,lblKO);

	TaskInitiateGPRS();
	goto lblEnd;

	lblKO:
	return 0;

	lblEnd:
	return returnWord;
}



word ApplicationCurrencyFill(char * Currency){
	int ret = 0;
	char Title[32];
	char TID[lenTID + 1];
	char MID[lenMid + 1];
	char CurrencyCodeAlpha[3 + 1];
	char CurrencyCodeNumeric[3 + 1];
	char Temp[10 + 1];
	char TempByte[10];
	card Result = 0;
	word returnWord = 0;

	memset(TID, 0, sizeof(TID));
	memset(MID, 0, sizeof(MID));
	memset(Temp, 0, sizeof(Temp));
	memset(Title, 0, sizeof(Title));
	memset(TempByte, 0, sizeof(TempByte));
	memset(CurrencyCodeAlpha, 0, sizeof(CurrencyCodeAlpha));
	memset(CurrencyCodeNumeric, 0, sizeof(CurrencyCodeNumeric));


	dec2num(&Result, Currency, 0);

	switch (Result) {
	case 840:
		MAPGET(appCurrCodeAlpha2,CurrencyCodeAlpha,lblKO);
		MAPGET(appCurrCodeNumerc2,CurrencyCodeNumeric,lblKO);
		MAPGET(appTID_2, TID, lblKO);
		MAPGET(appMID_2, MID, lblKO);
		returnWord = appTID_2;
		break; //USD
	default:
	case 834:
		MAPGET(appCurrCodeAlpha1,CurrencyCodeAlpha,lblKO);
		MAPGET(appCurrCodeNumerc1,CurrencyCodeNumeric,lblKO);
		MAPGET(appTID_1, TID, lblKO);
		MAPGET(appMID_1, MID, lblKO);
		returnWord = appTID_1;
		//TZS
		break;
	}
	strcpy(TempByte, "02");
	strncat(Temp, CurrencyCodeNumeric, 3);
	fmtPad(Temp, -4, '0');
	strcat(TempByte, Temp);
	MAPPUTSTR(appCurrCodeAlpha, CurrencyCodeAlpha,lblKO);
	MAPPUTSTR(traCurrencyLabel, CurrencyCodeAlpha,lblKO);
	MAPPUTSTR(emvTrnCurCod,TempByte,lblKO);
	MAPPUTSTR(traCurrencyNum,CurrencyCodeNumeric,lblKO);

	MAPPUTSTR(appTID,TID,lblKO);
	MAPPUTSTR(appMID,MID,lblKO);

	TaskInitiateGPRS();
	goto lblEnd;

	lblKO:
	return 0;

	lblEnd:
	return returnWord;
}

word ApplicationCurrencyFillAuto(char * Currency){
	int ret = 0;
	char Title[32];
	char TID[lenTID + 1];
	char MID[lenMid + 1];
	char CurrencyCodeAlpha[3 + 1];
	char CurrencyCodeNumeric[3 + 1];
	char Temp[10 + 1];
	char TempByte[10];
	card Result = 0;
	card TmpCurrency = 0;
	word returnWord = 0;

	memset(TID, 0, sizeof(TID));
	memset(MID, 0, sizeof(MID));
	memset(Temp, 0, sizeof(Temp));
	memset(Title, 0, sizeof(Title));
	memset(TempByte, 0, sizeof(TempByte));
	memset(CurrencyCodeAlpha, 0, sizeof(CurrencyCodeAlpha));
	memset(CurrencyCodeNumeric, 0, sizeof(CurrencyCodeNumeric));

	//Currency from caller
	dec2num(&Result, Currency, 0);

	//Currency from DB
	MAPGET(appCurrCodeNumerc2,CurrencyCodeNumeric,lblKO);
	dec2num(&TmpCurrency, CurrencyCodeNumeric, 0);

	if (Result == TmpCurrency) { //Second Currency
		MAPGET(appCurrCodeAlpha2,CurrencyCodeAlpha,lblKO);
		MAPGET(appCurrCodeNumerc2,CurrencyCodeNumeric,lblKO);
		MAPGET(appTID_2, TID, lblKO);
		MAPGET(appMID_2, MID, lblKO);
		returnWord = appTID_2;
	} else {                       //Other and First currency

		//Currency from DB
		MAPGET(appCurrCodeNumerc1,CurrencyCodeNumeric,lblKO);
		dec2num(&TmpCurrency, CurrencyCodeNumeric, 0);

		if (Result == TmpCurrency) { //First Currency
			MAPGET(appCurrCodeAlpha1,CurrencyCodeAlpha,lblKO);
			MAPGET(appCurrCodeNumerc1,CurrencyCodeNumeric,lblKO);
			MAPGET(appTID_1, TID, lblKO);
			MAPGET(appMID_1, MID, lblKO);
			returnWord = appTID_1;
		} else { //Means this currency doesnt exist
			GL_Dialog_Message(hGoal, NULL, "The Currency of this type is not supported!!", GL_ICON_ERROR, GL_BUTTON_ALL, 4 * GL_TIME_SECOND);
			mapPutStr(traRspCod, "06");
		}
	}

	strcpy(TempByte, "02");
	strncat(Temp, CurrencyCodeNumeric, 3);
	fmtPad(Temp, -4, '0');
	strcat(TempByte, Temp);
	MAPPUTSTR(appCurrCodeAlpha, CurrencyCodeAlpha,lblKO);
	MAPPUTSTR(traCurrencyLabel, CurrencyCodeAlpha,lblKO);
	MAPPUTSTR(emvTrnCurCod,TempByte,lblKO);
	MAPPUTSTR(traCurrencyNum,CurrencyCodeNumeric,lblKO);

	MAPPUTSTR(appTID,TID,lblKO);
	MAPPUTSTR(appMID,MID,lblKO);

	goto lblEnd;

	lblKO:
	return 0;

	lblEnd:
	return returnWord;
}

int Application_Request_Data(char * Narration, word KeySaveLocation, int lenField, char * mask){
	int ret = 0;
	int i = 0;
	char exponent[3 + 1];
	char dataEntered[100];
	char tcMask[256];
	char currencyLabel[3 + 1] ;
	const char entryMask[] = "/d/d/d,/d/d/d,/d/d/D";
	const char entryMask1[] = "/d/d/d,/d/d/d,/d/d/D./D";
	const char entryMask2[] = "/d/d/d,/d/d/d,/d/d/D./D/D";
	const char PercentMask[] = "/d/D./D/D";
	card Exp = 0;

	memset(currencyLabel, 0, sizeof(currencyLabel));
	memset(dataEntered, 0, sizeof(dataEntered));
	memset(exponent, 0, sizeof(exponent));
	memset(tcMask, 0, sizeof(tcMask));

	MAPGET(traCurrencyLabel, currencyLabel, lblKO);

	MAPGET(appExp,exponent,lblKO);
	dec2num(&Exp, exponent,1);

	for (i=0; i<(lenField + 1); i++)
		strcat(tcMask, mask);


	if (KeySaveLocation == appBillerSurcharge) {
		memset(currencyLabel, 0, sizeof(currencyLabel));
		strcpy(currencyLabel,"%");
		ret = GL_Dialog_Amount(hGoal, NULL, Narration, PercentMask, dataEntered, sizeof(dataEntered) - 1, currencyLabel, GL_ALIGN_RIGHT, 2 * GL_TIME_MINUTE);
	} else if ((KeySaveLocation != traAmt) && (KeySaveLocation != traOtherAmt) && (KeySaveLocation != traCashbackAmt)) {
		ret = GL_Dialog_Text(hGoal, "", Narration, tcMask, dataEntered, sizeof(dataEntered), GL_TIME_MINUTE);
	} else {
		switch (Exp) {
		case 1:
			ret = GL_Dialog_Amount(hGoal, NULL, Narration, entryMask1, dataEntered, sizeof(dataEntered) - 1, currencyLabel, GL_ALIGN_RIGHT, 2 * GL_TIME_MINUTE);
			break;
		case 2:
			ret = GL_Dialog_Amount(hGoal, NULL, Narration, entryMask2, dataEntered, sizeof(dataEntered) - 1, currencyLabel, GL_ALIGN_RIGHT, 2 * GL_TIME_MINUTE);
			break;
		default:
			ret = GL_Dialog_Amount(hGoal, NULL, Narration, entryMask, dataEntered, sizeof(dataEntered) - 1, currencyLabel, GL_ALIGN_RIGHT, 2 * GL_TIME_MINUTE);
			break;
		}

		if (!ret) {
			goto lblKO;
		}
	}

	ret = strlen(dataEntered);

	mapPut(KeySaveLocation, dataEntered, ret );

	lblKO:
	return ret;
}

int Application_Completion(void){
	int ret = 0;

	//Reference No
	ret = Application_Request_Data("Enter Ref# :", traReferenceNo, lenRrn, "/d");
	CHECK(ret>0, lblKO);

	//Approval code
	ret = Application_Request_Data("Enter Aut Code# :", traAutCod, lenAutCod, "/c");
	CHECK(ret>0, lblKO);

	//Approval code
	ret = Application_Request_Data("Enter STAN# :", traSTAN, lenAutCod, "/d");
	CHECK(ret>0, lblKO);

	ret = sqlite_Get_LOG_Record(traReferenceNo, traAutCod, 0);

	return ret;
	lblKO:
	ret = -1;
	return ret;
}

//! \brief Wait card insertion before an EMV transaction.
int  ret = 0, num = 0;
int Application_Get_Destination_Card(word KeySaveLocation){
	int magCardPresent = FALSE;
	Telium_File_t* keyboard;
	unsigned int sta;
	char trk1[128 + 1],trk2[128 + 1],trk3[128 + 1];
	char *src = 0; //magcard definition
	char *dst = 0; //magcard definition
	int returnVal = 0;
	char *track2Data;          // Buffer to accept the input
	char Pan[lenPan + 1];
	char buf[(128 * 3) + 4];
	int kbdInternal = 0;

	keyboard = NULL;

	memset(Pan, 0, sizeof(Pan));
	memset(buf, 0, sizeof(buf));
	memset(trk1, 0, sizeof(trk1));
	memset(trk2, 0, sizeof(trk2));
	memset(trk3, 0, sizeof(trk3));

	GL_Dialog_Message(hGoal, NULL, "Swipe\n Destination card", GL_ICON_INFORMATION, GL_BUTTON_NONE, 0);

	// Open peripherals
	// ================
	if (IsISO1() == 1) {
		hMag13 = Telium_Fopen("SWIPE31", "r*");       // Open mag1 peripheral
		//		CHECK(hMag13, lblKO);
	}
	if (IsISO2() == 1) {
		hMag2 = Telium_Fopen("SWIPE2", "r*");         // Open mag2 peripheral
		//		CHECK(hMag2, lblKO);
	}
	if (IsISO3() == 1) {
		hMag3 = Telium_Fopen("SWIPE3", "r*");         // Open mag3 peripheral
		//		CHECK(hMag3, lblKO);
	}

	// keyboard handle
	keyboard = Telium_Stdperif("KEYBOARD", NULL);               // Check "display" peripheral already opened? (done by Manager)
	if(keyboard == NULL){
		kbdInternal = 1;
		keyboard = Telium_Fopen("KEYBOARD", "r");
	}
	while(!magCardPresent) {

		sta = Telium_Ttestall(SWIPE31 | SWIPE2 | SWIPE3 | KEYBOARD, 20);

		///------ process magstripe cards ------
		if( (sta & SWIPE31) || (sta & SWIPE2) || (sta & SWIPE3) ){
			sta |= Telium_Ttestall(sta ^ (SWIPE31 | SWIPE2 | SWIPE3), 10); // Wait for the second event SWIPE2
			sta |= Telium_Ttestall(sta ^ (SWIPE31 | SWIPE2 | SWIPE3), 10); // Wait for the third event SWIPE3


			if(sta & SWIPE31) {
				ret = magTrack1(trk1, src, dst);
				if((ret >= ISO_OK) && (ret < DEF_SEP)) {
					num++;
				}
				strncat(buf, trk1, 128);
			}

			if(sta & SWIPE2) {
				ret = magTrack2(trk2, src, dst);
				CHECK(ret >= ISO_OK, lblKO);
				num++;
				strncat(&buf[128], trk2, 128);
			}

			if(sta & SWIPE3) {
				ret = magTrack3(trk3, src, dst);
				if((ret < DEF_SEP) && (ret >= ISO_OK)) {
					num++;
				}
				strncat(&buf[256], trk2, 128);
			}

			magCardPresent = TRUE;
			returnVal = SWIPE2;
		}


		////I only need track 2
		track2Data = &buf[128];

		ret = fmtTok(0, track2Data, "=");
		CHECK(ret <= lenPan, lblInvalidTrk);  // Search separator '='

		ret = fmtTok(Pan, track2Data, "=");    // Retrieve Pan from track 2
		VERIFY(ret <= lenPan);

		//Save the Trach 2 data only
		MAPPUTSTR(KeySaveLocation, Pan, lblKO);

		lblInvalidTrk:;

		///------ process Keyboard event ------
		if (sta & KEYBOARD) {
			returnVal = KEYBOARD;
		}

	}

	goto lblEnd;

	lblKO:
	goto lblEnd;

	lblEnd:

	if ((keyboard != NULL) && (kbdInternal)){
		Telium_Fclose(keyboard);
		keyboard = NULL;
	}

	if (hMag2 != NULL) {
		Telium_Fclose(hMag2);
		hMag2 = NULL;
	}

	if (hMag3 != NULL) {
		Telium_Fclose(hMag3);
		hMag3 = NULL;
	}

	if (hMag13 != NULL) {
		Telium_Fclose(hMag13);
		hMag13 = NULL;
	}

	return returnVal;
}


int TransactionFlow(void){
	int ret = 1;
	unsigned long amount;
	char EntMod;
	card MnuItem = 0;
	char MnuStr[20];
	char TempAmt[lenAmt + 1];

	memset(MnuStr, 0, sizeof(MnuStr));
	memset(TempAmt, 0, sizeof(TempAmt));

	MAPGET(traMnuItm, MnuStr, lblKO);
	dec2num(&MnuItem, MnuStr,0);

	MAPGETBYTE(traEntMod, EntMod, lblKO);

	//Enter an amount
	ret = EMV_UI_AmountEntry(&amount);
	CHECK(ret > 0, lblKO);

//	switch (MnuItem) {
//	case mnuSaleCB:
//
//		//Cash Back amount
//		ret = Application_Request_Data("Enter CashBack Amt:", traCashbackAmt, lenAmt, "/d");
//		MAPGET(traCashbackAmt, TempAmt, lblKO);
//		mapPut(traOtherAmt, TempAmt, lenAmt);
//
//		break;
//	default:
//		break;
//	}

	switch (MnuItem){
		case mnuDeposit:
			// Amount Deposit
			ret = Application_Request_Data("Enter Amount to deposit", traAmt, lenAmt, "/d");
			MAPGET(traAmt, TempAmt, lblKO);
		break;

		case mnuWithdrawal:
			// Amount Withdrawal
			ret = Application_Request_Data("Enter Amount to withdraw", traAmt, lenAmt, "/d");
			MAPGET(traAmt, TempAmt, lblKO);
		break;

		default:
			break;
	}

	ComputeTotAmt();

	switch (EntMod) {
	case 'c':
	case 'C':
	case 'l':
	case 'L':
		//Nothing to do here
		break;
	default:
		break;
	}

	ret = 1;
	goto lblEnd;

	lblKO:
	ret = -1;

	lblEnd:
	return ret;
}

static int Manual_PAN_Entry(void){
	int ret = 0;
	int i = 0;
	char tcMask[256];
	char tcPAN[lenPan + 1];
	char tcExpDat[lenExpDat + 1];
	char tcCVV2[lenExpDat*2 + 1];
	char CVV2_Val[lenExpDat + 1];
	char Buf[128];
	const char *mnuCVV[] ={
			"Present",
			"Absent",
			"Skip",
			0
	};

	OpenPeripherals();


	//Getting the PAN from the user
	memset(tcPAN, 0, sizeof(tcPAN));
	memset(tcMask, 0, sizeof(tcMask));
	for (i=0; i<lenPan; i++)
		strcat(tcMask, "/d");
	ret = GL_Dialog_Text(hGoal, "PAN Number", "Enter PAN# :", tcMask, tcPAN, sizeof(tcPAN), GL_TIME_MINUTE);
	CHECK((ret != GL_KEY_CANCEL) && (ret != GL_RESULT_INACTIVITY), lblKO); // Exit on cancel/timeout
	MAPPUTSTR(traPan, tcPAN, lblKO);
	MAPPUTSTR(traPanContext, tcPAN, lblKO);
	MaskPan();


	//Getting the Expiry date from the user
	memset(tcMask, 0, sizeof(tcMask));
	memset(tcExpDat, 0, sizeof(tcExpDat));
	for (i=0; i<lenExpDat; i++)
		strcat(tcMask, "/d");
	ret = GL_Dialog_Text(hGoal, "Expiry Date", "Enter Expiry(YYMM) Date :", tcMask, tcExpDat, sizeof(tcExpDat), GL_TIME_MINUTE);
	CHECK((ret != GL_KEY_CANCEL) && (ret != GL_RESULT_INACTIVITY), lblKO); // Exit on cancel/timeout
	MAPPUTSTR(traExpDat, tcExpDat, lblKO);
	MAPPUTSTR(traExpDatContext, tcExpDat, lblKO);

	//Make track 2 data
	memset(Buf, 0, sizeof(Buf));
	strcpy(Buf, tcPAN);
	strcat(Buf, "=");
	strcat(Buf, tcExpDat);
	MAPPUTSTR(traTrk2, Buf, lblKO);
	MAPPUTSTR(traTrk21, Buf, lblKO);
	MAPPUTSTR(traTrk22, Buf, lblKO);


	//Getting the CVV2 from the user
	memset(tcCVV2, 0, sizeof(tcCVV2));
	memset(Buf, 0, sizeof(Buf));
	strcat(tcCVV2,"16");
	do{
		ret = GL_Dialog_Menu(hGoal, "Cardholder Select: ?", mnuCVV, 0, GL_BUTTON_ALL, GL_KEY_0, GL_TIME_MINUTE);
	}while(ret==-1);

	switch(ret){
	case 0:	//cardholder present
		strcat(tcCVV2,"10");
		ret = GL_Dialog_Text(hGoal, "Card CVV2", "Enter CVV2# :", tcMask, CVV2_Val, sizeof(CVV2_Val), GL_TIME_MINUTE);
		CHECK((ret != GL_KEY_CANCEL) && (ret != GL_RESULT_INACTIVITY), lblSkip); // Exit on cancel/timeout

		strcat(tcCVV2, CVV2_Val);
		break;
	case 1:	//cardholder
		strcat(tcCVV2,"00    ");
		break;
	default://skip
		strcat(tcCVV2,"90    ");
		break;
	}

	goto lblOK;
	lblSkip:
	strcat(tcCVV2,"90    ");

	lblOK: //No skipping of CVV2

	MAPPUTSTR(traCVV2, tcCVV2, lblKO);


	goto lblEnd;

	lblKO:
	ret = -1;

	lblEnd:
	ClosePeripherals();
	return ret;
}


void All_AfterTransaction(void){
	int ret = 0;
	card MnuItm = 0;
	char DspData[1024];


	memset(DspData, 0, sizeof(DspData));

	MAPGET(traMnuItm, DspData, lblKO);
	dec2num(&MnuItm, DspData, 0);

	memset(DspData, 0, sizeof(DspData)); //reset used field
	//AB: return code processing and reversal check to be done here
	if(MnuItm != mnuOffline) {
		ret = valRspCod();
		CHECK(ret > 0, lblDeclined);
	}

	ret = logSave();         // Save transaction into log table (Batch)
	CHECK(ret > 0, lblKO);

	// TODO: Customise if required
	// TODO: Reversal, Online Advice, display transaction status, print ticket...

	// (0) Display transaction status
	MAPGET(traAlternateRsp, DspData, lblKO);
	_clrscr();
	if ((strlen(DspData)>1) && (MnuItm != mnuBiller)) {
		GL_Dialog_Message(hGoal, NULL, (char *)DspData, GL_ICON_INFORMATION, GL_BUTTON_NONE, 0);
	} else {
		GL_Dialog_Message(hGoal, NULL, "Please Wait""\n""Printing In Progress", GL_ICON_INFORMATION, GL_BUTTON_NONE, 0);
	}

	// (1) Print receipt
	lblDeclined:

	ret = PrintReceipt();
	CHECK(ret > 0, lblKO);    // Print transaction receipt

	// (2) Manage Advice Messages
	AdviseTransactionManager();

	ret = incCard(appSTAN); //Increment Trace number even if its an Offline Transaction
	CHECK(ret >= 0, lblKO);

	if(MnuItm != mnuVoid) {
		ret = incCard(appInvNum);   //Increment Invoice number / ROC when trx is approved
		CHECK(ret > 0, lblKO);
	}

	// (3) Check and Perform Reversal if required
	revAutoReversal();

	lblKO:;
}


void onlSession(void){
	char DspData[2048];
	char rspCode[10 + 1];
	char MnuStr[32 + 1];
	int ret = 0;
	card tmpRsp, MnuItem = 0;

	memset(rspCode, 0, sizeof(rspCode));
	memset(DspData, 0, sizeof(DspData));
	memset(MnuStr, 0, sizeof(MnuStr));

	strcpy(DspData, "Please Wait....");
	GL_Dialog_Message(hGoal, NULL, (char *)DspData, GL_ICON_INFORMATION, GL_BUTTON_NONE, GL_TIME_SECOND);

	//Send the Transaction online
	performOlineTransaction();

	MAPGET(traMnuItm, MnuStr, lblKO);
	dec2num(&MnuItem, MnuStr,0);

	// (0) Display transaction status
	memset(DspData, 0, sizeof(DspData));
	MAPGET(traAlternateRsp, DspData, lblKO);
	_clrscr();

	if (strlen(DspData)>1) {
		if (MnuItem == mnuBillerInquiry) {

			if(isApproved()){
				strcat(DspData, "\nNo=Cancel / Yes=Valid");
				ret = GL_Dialog_Message(hGoal,  NULL, (char *)DspData, GL_ICON_QUESTION, GL_BUTTON_VALID_CANCEL, 30*1000);

				if ((ret==GL_KEY_CANCEL) && (ret==GL_RESULT_INACTIVITY)) {
					MAPPUTSTR(traRspCod , "98", lblKO);
				}
			} else {
				ret = GL_Dialog_Message(hGoal,  NULL, "Transaction Declined!!", GL_ICON_QUESTION, GL_BUTTON_VALID_CANCEL, 30*1000);
				MAPPUTSTR(traRspCod , "98", lblKO);
			}
		} else if (MnuItem == mnuEchoTest) {
			GL_Dialog_Message(hGoal, NULL, (char *)DspData, GL_ICON_INFORMATION, GL_BUTTON_NONE, GL_TIME_SECOND);
		}
	} else {
		MAPGET(traRspCod,rspCode,lblKO);
		if(!fncIsNumeric(rspCode)){
			getResponse_NonNumeric(DspData,rspCode);
		}else{
			dec2num(&tmpRsp, rspCode, strlen(rspCode));
			getResponse(DspData, tmpRsp);
		}
		GL_Dialog_Message(hGoal, NULL, (char *)DspData, GL_ICON_INFORMATION, GL_BUTTON_VALID_CANCEL, GL_TIME_SECOND);
	}

	lblKO:;
}

void Magnetic_Card_Transaction(void){
	int ret = 0;
	card tmpRsp;
	char EntMode[5];
	char DspData[1024];
	card outputPINLen = 0;
	char rspCode[lenRspCod + 1];

	memset(EntMode, 0, sizeof(EntMode));
	memset(rspCode, 0, sizeof(rspCode));
	memset(DspData, 0, sizeof(DspData));

	//Common Transaction flows
	ret = TransactionFlow();
	CHECK(ret > 0, lblKO);

	//Do Pin request
	MAPGET(traEntMod, EntMode, lblKO);
	if (strncmp(EntMode, "k", 1) != 0) {
		RenterPin:
		//	getOnlinePin(); //3DES FORMAT0 normal
		ret = FUN_PinEntry();
		if (ret < 0) {
			GL_Dialog_Message(hGoal, NULL, "Transaction Cancelled!!!", GL_ICON_ERROR, GL_BUTTON_NONE, 2 * GL_TIME_SECOND);
			goto lblKO;
		}

		mapGetCard(traOnlinePinLen, outputPINLen);
		if (outputPINLen == 0) {
			goto RenterPin;
		}
	}
	_clrscr();

	//Send the Transaction online
	performOlineTransaction();

	// (0) Display transaction status
	memset(DspData, 0, sizeof(DspData));
	MAPGET(traAlternateRsp, DspData, lblKO);
	_clrscr();

	if (strlen(DspData)>1) {
		GL_Dialog_Message(hGoal, NULL, (char *)DspData, GL_ICON_INFORMATION, GL_BUTTON_NONE, GL_TIME_SECOND);
	} else {
		MAPGET(traRspCod,rspCode,lblKO);
		if(!fncIsNumeric(rspCode)){
			getResponse_NonNumeric(DspData,rspCode);
		}else{
			dec2num(&tmpRsp, rspCode, strlen(rspCode));
			getResponse(DspData, tmpRsp);
		}
		GL_Dialog_Message(hGoal, NULL, (char *)DspData, GL_ICON_INFORMATION, GL_BUTTON_VALID_CANCEL, GL_TIME_SECOND);
	}

	mapPutByte(appFallback, 0); //Release Fallback flag

	//After online
	All_AfterTransaction();

	lblKO:;
}

static void prepareEMV_MandatoryData(void){
	char PCODE[(lenPrcCod*2) + 1];
	byte txnType[2];
	card InvNum;
	byte TrnSeqCnt[1 + lenTrnSeqCnt];
	char buf[2 * lenTrnSeqCnt + 1];
	char OthAmt[lenAmt*2 + 1];
	byte AmtNum[1 + lenAmtNum];
	byte AmtBin[1 + lenAmtBin];
	card AmtNat;


	memset(AmtBin, 0, sizeof(AmtBin));
	memset(AmtNum, 0, sizeof(AmtNum));
	memset(OthAmt, 0, sizeof(OthAmt));
	memset(PCODE, 0, sizeof(PCODE));
	memset(buf, 0, sizeof(buf));

	//// ---- Transaction Type
	txnType[0] = 1;
	MAPGET(traRqsProcessingCode, PCODE, lblKO);
	hex2bin(&txnType[1], PCODE,1);
	mapPut(emvTrnTyp, txnType, 2);

	//// ---- Transaction Seq Counter

	MAPGETCARD(appInvNum, InvNum, lblKO);
	TrnSeqCnt[0] = lenTrnSeqCnt;
	VERIFY(TrnSeqCnt[0] == 4);
	num2dec(buf, InvNum, 2 * lenTrnSeqCnt); // ulong -> "NNNNNNNN"
	hex2bin(&TrnSeqCnt[1], buf, lenTrnSeqCnt);  // "NNNNNNNN" -> "\xNN\xNN\xNN\xNN"
	ret = mapPut(emvTrnSeqCnt, TrnSeqCnt, 1 + lenTrnSeqCnt);
	CHECK(ret >= 0, lblKO);

	//// ---- other amount other amount
	MAPGET(traOtherAmt, OthAmt, lblKO);
	fmtPad(OthAmt, -lenAmt, '0');

	AmtNum[0] = lenAmtNum;
	hex2bin(&AmtNum[1], OthAmt, lenAmtNum);
	ret = mapPut(emvAmtOthNum, AmtNum, 1 + lenAmtNum);
	CHECK(ret >= 0, lblKO);

	AmtBin[0] = lenAmtBin;
	dec2num(&AmtNat, OthAmt, 0);
	num2bin(&AmtBin[1], AmtNat, lenAmtBin);
	ret = mapPut(emvAmtOthBin, AmtBin, 1 + lenAmtBin);
	CHECK(ret >= 0, lblKO);

	lblKO:;
}

void dateTimeRefresh(void){
	int iRet = 0;
	char datetime[lenDatTim + 1];

	memset(datetime, 0, sizeof(datetime));

	strcpy(datetime, "20");     //CC
	iRet = getDateTime(datetime + 2);    //CC+YYMMDDhhmmss
	CHECK(iRet > 0, lblKO);

	iRet = mapPut(traDatTim, datetime, lenDatTim);
	CHECK(iRet > 0, lblKO);

	lblKO:;
}

void ApplicationBuildReversalData(void){
	char MTI[(lenMti * 2) + 1];
	char fieldData[100];
	int ret = 0;

	memset(MTI, 0, sizeof(MTI));
	memset(fieldData, 0, sizeof(fieldData));

	//Buildfield 60
	MAPGET(traRqsMTI, MTI, lblKO);
	if (strncmp(MTI,"020000",6) == 0) {
		strcpy(MTI,"020200");
	}
	strncpy(fieldData, &MTI[2],4);
	MAPGET(traSTAN, &fieldData[4], lblKO);
	fmtPad(fieldData, 22, ' '); //Padding according to SPECs RFU (Reserved for Future Use)
	MAPPUTSTR(traRevVoidData, fieldData, lblKO);

	//Buildfield 63
	memset(fieldData, 0, sizeof(fieldData));
	MAPGET(traRqsProcessingCode, fieldData, lblKO); //Padding according to SPECs RFU (Reserved for Future Use)
	if(strlen(fieldData) > 5)
		MAPPUTSTR(traVoid63Data, fieldData, lblKO);

	lblKO:;
}


int ApplicationVoid(char * STAN_Rsp){
	int ret = 0;

	//Approval code
	ret = Application_Request_Data("Enter STAN No# :", traSTAN, lenAutCod, "/d");
	CHECK(ret>0, lblKO);

	MAPGET(traSTAN, STAN_Rsp, lblKO);

	ret = sqlite_Get_LOG_Record(0, 0, traSTAN);
	CHECK(ret>0, lblKO);

	ApplicationBuildReversalData();

	return 1;

	lblKO:
	GL_Dialog_Message(hGoal, NULL, "NO Record Found!!!", GL_ICON_ERROR, GL_BUTTON_NONE, 2 * GL_TIME_SECOND);
	return -1;
}


void Application_Do_Transaction_Idle(void){
	int ret = 0;
	char Buf[128 * 3];
	word retWord = 0;

	memset(Buf, 0, sizeof(Buf));

	retWord = ApplicationSelectCurrency();
	ret = (int)retWord;
	if (retWord < 1) {
		GL_Dialog_Message(hGoal, NULL, "Transaction Cancelled!!", GL_ICON_ERROR, GL_BUTTON_NONE, 100);
		goto lblKO;
	}

	//Initialize date and time functionality
	dateTimeRefresh();

	//	//chip txn
	MAPPUTSTR(traEntMod, "c", lblKO);

	MAPPUTSTR(traRspCod,"100",lblKO);
	MAPPUTSTR(traOtherAmt,"000000000000",lblKO);

	//Common Transaction flows
	ret = TransactionFlow();
	CHECK(ret > 0, lblKO);

	//Prepare some few data
	prepareEMV_MandatoryData();

	// Close standard peripherals
	ClosePeripherals();

	//Chip Transaction flow
	Start_Emv_Transaction();

	mapPutByte(appFallback, 0); //Release Fallback flag
	return;

	lblKO:
	GL_Dialog_Message(hGoal, NULL, "Transaction Cancelled!!", GL_ICON_ERROR, GL_BUTTON_NONE, 2 * GL_TIME_SECOND);
	ret = -1;
	mapPutByte(appFallback, 0); //Release Fallback flag
	return;
}

int Application_Do_Transaction(void){
	int ret = 0;
	int card_Used = 0;
	char Buf[128 * 4];
	char Pan[lenPan + 1];
	char Trk1[128 + 1];
	char CardHolderName[40 + 1];
	char ExpDat[lenExpDat + 1];
	char srvCd[lenSrvCd + 1];
	char *pTrk2;
	byte FallBack = 0;
	int previousUiState;
	int CardResult = 0;

	memset(Buf, 0, sizeof(Buf));

	//Initialize date and time functionality
	dateTimeRefresh();

	//Common Transaction flows
	ret = TransactionFlow();
	CHECK(ret > 0, lblKO);

	RE_ENTER_CARD:

	card_Used = Application_Get_Card((byte *)Buf);

	switch (card_Used) {
	//chip txn
	case CAM0:
		MAPPUTSTR(traEntMod, "c", lblKO);

		MAPPUTSTR(traRspCod,"100",lblKO);

		//Prepare some few data
		prepareEMV_MandatoryData();

		//Chip Transaction flow
		Start_Emv_Transaction();

		mapGetByte(appFallback, FallBack);
		if (FallBack == 1) {
			goto lblContMagStripeFallBack;
		}
		CardResult = 1;
		break;
		//cless txn
	case CLESS:
		mapPutByte(appClessMagMode, 0); //Default transaction as a MCHIP mode

		Cless_ExplicitSelection_ClearGlobalData();

		MAPPUTSTR(traEntMod, "l", lblKO);

		MAPPUTSTR(traRspCod,"100",lblKO);
		MAPPUTSTR(traOtherAmt,"000000000000",lblKO);

		prepareEMV_MandatoryData();

		// CLESS transaction start
		CardResult = CLESS_Explicit_DoTransaction();
		CardResult = 1;

		mapGetByte(appFallback, FallBack);
		if (FallBack == 1) {
			goto lblContMagStripeFallBack;
		}

		break;
		//magnetic txn
	case SWIPE2:

		MAPPUTSTR(traRspCod,"100",lblKO);
		MAPPUTSTR(traOtherAmt,"000000000000",lblKO);

		memset(Pan, 0, sizeof(Pan));
		memset(ExpDat, 0, sizeof(ExpDat));
		memset(srvCd, 0, sizeof(srvCd));
		memset(CardHolderName, 0, sizeof(CardHolderName));
		MAPPUTSTR(traEntMod, "m", lblKO);
		if(*Buf != 0) {
			memcpy(Trk1, Buf, 128);
			ret = fmtTok(0, Trk1, "^");
			CHECK(ret <= sizeof(Trk1), lblInvalidTrk);
			memset(Trk1, 0, sizeof(Trk1));
			memcpy(Trk1, &Buf[ret + 1], 128 - ret);
			ret = fmtTok(CardHolderName, Trk1, "^");  // Retrieve cardholder name from track1
		}

		MAPPUTSTR(traTrk1, CardHolderName, lblKO);

		pTrk2 = &Buf[128];
		MAPPUTSTR(traTrk2, pTrk2, lblKO);
		ret = fmtTok(0, pTrk2, "=");
		CHECK(ret <= lenPan, lblInvalidTrk);  // Search separator '='
		ret = fmtTok(Pan, pTrk2, "=");    // Retrieve Pan from track 2
		VERIFY(ret <= lenPan);
		MAPPUTSTR(traPan, Pan, lblKO);

		pTrk2 += ret;         // Continue to analyse track 2
		CHECK(*pTrk2 == '=', lblInvalidTrk);  // Should be separator
		pTrk2++;
		ret = fmtSbs(ExpDat, pTrk2, 0, lenExpDat);    // Retrieve expiration date from track 2
		VERIFY(ret == 4);
		MAPPUTSTR(traExpDat, ExpDat, lblKO);

		pTrk2++;pTrk2++;pTrk2++;pTrk2++;
		ret = fmtSbs(srvCd, pTrk2, 0, 3);    // Retrieve expiration date from track 2
		VERIFY(ret == 3);
		MAPPUTSTR(traSrvCd,srvCd, lblKO);
		mapGetByte(appFallback, FallBack);

		if(strncmp(srvCd,"2",1)==0 ||strncmp(srvCd,"6",1)==0) {   //first character indicates presence or absence of chip
			if (FallBack != 1) {
				goto lblUseChip;
			}
		}
		//process Magstripe Transaction
		Magnetic_Card_Transaction();

		CardResult = 1;
		break;
	case KEYBOARD:

		if (Buf[0] == kbdANN) {
			goto lblKO; //Transaction was cancelled
		}
		MAPPUTSTR(traEntMod, "k", lblKO);

		//Enter the PAN number
		ret = Manual_PAN_Entry();
		CHECK(ret > 0, lblInvalidTrk);

		// Setup the user interface environment
		previousUiState = EMV_UI_TransactionBegin();

		//process transaction as Magstripe
		Magnetic_Card_Transaction();

		// Restore the user interface environment
		EMV_UI_TransactionEnd(previousUiState);

		CardResult = 1;
		break;
	default:
		CardResult = -1;
		goto lblKO;
		break;
	}

	mapPutByte(appFallback, 0); //Release Fallback flag
	return CardResult;

	///------------------------- Fallback -----------------------
	lblContMagStripeFallBack:
	buzzer(5);buzzer(5);buzzer(5);
	GL_Dialog_Message(hGoal, NULL, "FALLBACK USE MAGSTRIPE", GL_ICON_ERROR, GL_BUTTON_NONE, 2 * GL_TIME_SECOND);
	ret = 1;
	goto RE_ENTER_CARD;

	///--------------------- Use as a chip entry ----------------
	lblUseChip:
	buzzer(5);buzzer(5);buzzer(5);
	GL_Dialog_Message(hGoal, NULL, "USE CHIP", GL_ICON_ERROR, GL_BUTTON_NONE, 2 * GL_TIME_SECOND);
	ret = 2;
	goto RE_ENTER_CARD;

	lblInvalidTrk:
	GL_Dialog_Message(hGoal, NULL, "Invalid Track", GL_ICON_ERROR, GL_BUTTON_NONE, 2 * GL_TIME_SECOND);
	ret = -1;
	return ret;

	lblKO:
	GL_Dialog_Message(hGoal, NULL, "Transaction Cancelled!!", GL_ICON_ERROR, GL_BUTTON_NONE, 2 * GL_TIME_SECOND);
	ret = -1;
	mapPutByte(appFallback, 0); //Release Fallback flag
	return ret;
}

int Application_Card_Transaction(void){
	int ret = 0;
	int card_Used = 0;
	char Buf[128 * 4];
	char Pan[lenPan + 1];
	char Trk1[128 + 1];
	char CardHolderName[40 + 1];
	char ExpDat[lenExpDat + 1];
	char srvCd[lenSrvCd + 1];
	char *pTrk2;
	byte FallBack = 0;

	memset(Buf, 0, sizeof(Buf));

	RE_ENTER_CARD:

	card_Used = Application_Get_Card_Minimal((byte *)Buf);
	switch (card_Used) {
	//chip txn
	case CAM0:

		MAPPUTCARD(traIdleTransaction, CAM0, lblKO);

		MAPPUTSTR(traEntMod, "c", lblKO);

		MAPPUTSTR(traRspCod,"100",lblKO);

		mapGetByte(appFallback, FallBack);
		if (FallBack == 1) {
			goto lblContMagStripeFallBack;
		}
		break;
		//cless txn
	case CLESS:

		MAPPUTCARD(traIdleTransaction, CLESS, lblKO);

		mapPutByte(appClessMagMode, 0); //Default transaction as a MCHIP mode

		Cless_ExplicitSelection_ClearGlobalData();

		MAPPUTSTR(traEntMod, "l", lblKO);

		MAPPUTSTR(traRspCod,"100",lblKO);
		MAPPUTSTR(traOtherAmt,"000000000000",lblKO);

		mapGetByte(appFallback, FallBack);
		if (FallBack == 1) {
			goto lblContMagStripeFallBack;
		}
		Cless_Goal_Open();

		break;
		//magnetic txn
	case SWIPE2:

		MAPPUTCARD(traIdleTransaction, SWIPE2, lblKO);

		MAPPUTSTR(traRspCod,"100",lblKO);
		MAPPUTSTR(traOtherAmt,"000000000000",lblKO);

		memset(Pan, 0, sizeof(Pan));
		memset(ExpDat, 0, sizeof(ExpDat));
		memset(srvCd, 0, sizeof(srvCd));
		memset(CardHolderName, 0, sizeof(CardHolderName));
		MAPPUTSTR(traEntMod, "m", lblKO);
		if(*Buf != 0) {
			memcpy(Trk1, Buf, 128);
			ret = fmtTok(0, Trk1, "^");
			CHECK(ret <= sizeof(Trk1), lblInvalidTrk);
			memset(Trk1, 0, sizeof(Trk1));
			memcpy(Trk1, &Buf[ret + 1], 128 - ret);
			ret = fmtTok(CardHolderName, Trk1, "^");  // Retrieve cardholder name from track1
		}

		MAPPUTSTR(traTrk1, CardHolderName, lblKO);

		pTrk2 = &Buf[128];
		MAPPUTSTR(traTrk2, pTrk2, lblKO);
		ret = fmtTok(0, pTrk2, "=");
		CHECK(ret <= lenPan, lblInvalidTrk);  // Search separator '='
		ret = fmtTok(Pan, pTrk2, "=");    // Retrieve Pan from track 2
		VERIFY(ret <= lenPan);
		MAPPUTSTR(traPan, Pan, lblKO);

		pTrk2 += ret;         // Continue to analyse track 2
		CHECK(*pTrk2 == '=', lblInvalidTrk);  // Should be separator
		pTrk2++;
		ret = fmtSbs(ExpDat, pTrk2, 0, lenExpDat);    // Retrieve expiration date from track 2
		VERIFY(ret == 4);
		MAPPUTSTR(traExpDat, ExpDat, lblKO);

		pTrk2++;pTrk2++;pTrk2++;pTrk2++;
		ret = fmtSbs(srvCd, pTrk2, 0, 3);    // Retrieve expiration date from track 2
		VERIFY(ret == 3);
		MAPPUTSTR(traSrvCd,srvCd, lblKO);
		mapGetByte(appFallback, FallBack);

		if(strncmp(srvCd,"2",1)==0 ||strncmp(srvCd,"6",1)==0) {   //first character indicates presence or absence of chip
			if (FallBack != 1) {
				goto lblUseChip;
			}
		}

		break;
	case KEYBOARD:

		MAPPUTCARD(traIdleTransaction, KEYBOARD, lblKO);

		if (Buf[0] == kbdANN) {
			goto lblKO; //Transaction was cancelled
		}
		MAPPUTSTR(traEntMod, "k", lblKO);

		//Enter the PAN number
		ret = Manual_PAN_Entry();
		CHECK(ret > 0, lblInvalidTrk);

		break;
	default:
		MAPPUTCARD(traIdleTransaction, 0, lblKO);
		goto lblEnd;
		break;
	}

	mapPutByte(appFallback, 0); //Release Fallback flag
	return card_Used;

	///------------------------- Fallback -----------------------
	lblContMagStripeFallBack:
	buzzer(5);buzzer(5);buzzer(5);
	GL_Dialog_Message(hGoal, NULL, "FALLBACK USE MAGSTRIPE", GL_ICON_ERROR, GL_BUTTON_NONE, 2 * GL_TIME_SECOND);
	ret = 1;
	goto RE_ENTER_CARD;

	///--------------------- Use as a chip entry ----------------
	lblUseChip:
	buzzer(5);buzzer(5);buzzer(5);
	GL_Dialog_Message(hGoal, NULL, "USE CHIP", GL_ICON_ERROR, GL_BUTTON_NONE, 2 * GL_TIME_SECOND);
	ret = 2;
	goto RE_ENTER_CARD;

	lblInvalidTrk:
	GL_Dialog_Message(hGoal, NULL, "Invalid Track", GL_ICON_ERROR, GL_BUTTON_NONE, 2 * GL_TIME_SECOND);
	ret = -1;
	return 0;

	lblKO:ret = -1;
	mapPutByte(appFallback, 0); //Release Fallback flag
	return 0;

	lblEnd:
	return card_Used;
}

int Manage_Application_Menu(T_GL_HGRAPHIC_LIB handleGoal){
	int ret = 0, MenuSelected = TRUE, rec = 0, MnuRec = 0;
	int numberOfColumns = 0, LastMenuSelected = 0;
	char menuBuffer[32 * 5 * 80];
	char parentMenuSTR[3];
	card key = 0,keyLog[256];
	int numberOfMenus = 0,keyLogCounter = 0;
	char *Raw_Record;
	char *Menu_Record_Array[100];
	char Menu_Column_Buffer[512];
	char *Menu_Column;
	char *Menu_Column_Array[40];
	char tempBuffer[100];
	char temp[100];
	char MenuContent[40][100];
	char MenuIcon[40][100];
	char MenuSecurity[40][5];
	char MenuSelectedName[100];
	char MenuContent_ID[40][5];
	char MenuSecurityLevel[40][5];
	char TraMenuItem[5];
	char SecuredMenu[2];
	char Amount[lenAmt + 1];
	int MenuProcessingResult = 0;
	int refreshed = 0;


	memset(MenuSelectedName, 0, sizeof(MenuSelectedName));
	memset(Amount, 0, sizeof(Amount));
	memset(keyLog, 0, sizeof(keyLog));

	lblReMENU:
	//check if there is need to do settlement
	fncAutoSettlementChecker();

	strcpy(MenuSelectedName, "Main Menu");
	keyLog[0] = 0;
	MAPPUTWORD(traManualPan,0,lblDB_Corrupt);
	MAPPUTCARD(traIdleTransaction, 0, lblKO);

	clrscr();

	do{
		/// ======== start menu management loop =========

		memset(parentMenuSTR, 0, sizeof(parentMenuSTR));
		memset(menuBuffer, 0, sizeof(menuBuffer));
		num2dec(parentMenuSTR, key,0);

		//menu management
		numberOfMenus = Sqlite_Get_Menu(parentMenuSTR, menuBuffer);
		if(numberOfMenus == 0){// NO CHILDREN MENUS
			//Trying to sort issue when button is pressed and nothing happens
			if ((key == 0) && (numberOfMenus == 0)) {
				fncResetTerminalData();
			}

			//Continue
			LastMenuSelected = GL_KEY_CORRECTION;
			ClosePeripherals();
			MAPPUTSTR(traMnuItm, TraMenuItem, lblKO);
			mapPutStr(traAmt, Amount);
			MenuProcessingResult = MenuProcessingSelect(key); //ProcessMenu
			if (MenuProcessingResult == CLESS_CR_MANAGER_RESTART_DOUBLE_TAP) {
				ret = MenuProcessingResult;
				goto lblEnd;
			}

			goto RE_DSP_MENU;
		}
		numberOfColumns = count_chars(menuBuffer,';') / numberOfMenus;
		/*
		 * menuBuffer{} structure =
		 * tableColumnName1,columnData1;tableColumnName2,columnData2;#tableColumnName1,columnData1;tableColumnName2,columnData2;#
		 */
		//// ----------- split the records into an array ---------------
		Raw_Record = strtok(menuBuffer,"#");//record separator
		Menu_Record_Array[0] = Raw_Record;

		for(rec = 1;rec < numberOfMenus;rec++){
			if(Raw_Record == NULL){
				break;
			}
			Raw_Record = strtok(NULL,"#");
			Menu_Record_Array[rec] = Raw_Record;
		}

		///--------------------- get the menu ID, Menu Name and Icon name -------------------
		for(rec = 0;rec < numberOfMenus; rec++){
			memset(Menu_Column_Buffer, 0, sizeof(Menu_Column_Buffer));
			strcpy(Menu_Column_Buffer, Menu_Record_Array[rec]);

			//++++++++++++ Treat first record ++++++++++++++
			Menu_Column = strtok(Menu_Column_Buffer,";");//record separator
			Menu_Column_Array[0] = Menu_Column;

			memset(tempBuffer, 0, sizeof(tempBuffer));
			ret = fmtTok(tempBuffer, Menu_Column_Array[0], ",");    //extract first token
			Menu_Column_Array[0]+=ret;//skip data
			Menu_Column_Array[0]++;   //skip separator

			if (strncmp(tempBuffer,"MenuId", 6) == 0) { ///Id of menu
				memset(MenuContent_ID[rec], 0, sizeof(MenuContent_ID[rec]));
				memset(temp, 0, sizeof(temp));
				memset(tempBuffer, 0, sizeof(tempBuffer));
				ret = fmtTok(tempBuffer, Menu_Column_Array[0], ",");    //extract second token
				strcpy(temp, tempBuffer);
				strcpy(MenuContent_ID[rec], temp);
			} else if(strncmp(tempBuffer,"MenuName", 8) == 0){ ///Name of menu
				memset(MenuContent[rec], 0, sizeof(MenuContent[rec]));
				memset(temp, 0, sizeof(temp));
				memset(tempBuffer, 0, sizeof(tempBuffer));

				ret = fmtTok(tempBuffer, Menu_Column_Array[0], ",");    //extract second token "Menu Name"
				strcpy(temp, tempBuffer);
				strcpy(MenuContent[rec], temp);

			} else if(strncmp(tempBuffer,"IconPathName", 12) == 0){ ///Icon Menu Name
				memset(MenuIcon[rec], 0, sizeof(MenuIcon[rec]));

				memset(temp, 0, sizeof(temp));
				memset(tempBuffer, 0, sizeof(tempBuffer));
				ret = fmtTok(tempBuffer, Menu_Column_Array[0], ",");
				strcpy(temp, tempBuffer);
				strcpy(MenuIcon[rec], temp);
			} else if(strncmp(tempBuffer,"SecureMenuLevel", 15) == 0){ ///Menu Security Level
				memset(MenuSecurityLevel[rec], 0, sizeof(MenuSecurityLevel[rec]));

				memset(temp, 0, sizeof(temp));
				memset(tempBuffer, 0, sizeof(tempBuffer));
				ret = fmtTok(tempBuffer, Menu_Column_Array[0], ",");
				strcpy(temp, tempBuffer);
				strcpy(MenuSecurityLevel[rec], temp);
			} else if(strncmp(tempBuffer,"SecureMenu", 10) == 0){ ///Menu is Secured
				memset(MenuSecurity[rec], 0, sizeof(MenuSecurity[rec]));

				memset(temp, 0, sizeof(temp));
				memset(tempBuffer, 0, sizeof(tempBuffer));
				ret = fmtTok(tempBuffer, Menu_Column_Array[0], ",");
				strcpy(temp, tempBuffer);
				strcpy(MenuSecurity[rec], temp);
			}


			//+++++++++++++++++  Treat other records +++++++++++++
			for(MnuRec = 1;MnuRec < numberOfColumns;MnuRec++){
				if(Menu_Column == NULL){
					break;
				}
				Menu_Column = strtok(NULL,";");
				Menu_Column_Array[MnuRec] = Menu_Column;

				memset(tempBuffer, 0, sizeof(tempBuffer));
				ret = fmtTok(tempBuffer, Menu_Column_Array[MnuRec], ",");    //extract first token
				Menu_Column_Array[MnuRec]+=ret;//skip data
				Menu_Column_Array[MnuRec]++;   //skip separator

				if (strncmp(tempBuffer,"MenuId", 6) == 0) { ///Id of menu
					memset(MenuContent_ID[rec], 0, sizeof(MenuContent_ID[rec]));
					memset(temp, 0, sizeof(temp));
					memset(tempBuffer, 0, sizeof(tempBuffer));
					ret = fmtTok(tempBuffer, Menu_Column_Array[MnuRec], ",");    //extract second token
					strcpy(temp, tempBuffer);
					strcpy(MenuContent_ID[rec], temp);
				} else if(strncmp(tempBuffer,"MenuName", 8) == 0){ ///Name of menu
					memset(MenuContent[rec], 0, sizeof(MenuContent[rec]));
					memset(temp, 0, sizeof(temp));
					memset(tempBuffer, 0, sizeof(tempBuffer));
					ret = fmtTok(tempBuffer, Menu_Column_Array[MnuRec], ",");    //extract second token
					strcpy(temp, tempBuffer);
					strcpy(MenuContent[rec], temp);
				} else if(strncmp(tempBuffer,"IconPathName", 12) == 0){ ///Name of Icon
					memset(MenuIcon[rec], 0, sizeof(MenuIcon[rec]));
					memset(temp, 0, sizeof(temp));
					memset(tempBuffer, 0, sizeof(tempBuffer));
					ret = fmtTok(tempBuffer, Menu_Column_Array[MnuRec], ",");    //extract second token
					strcpy(temp, tempBuffer);
					strcpy(MenuIcon[rec], temp);
				} else if(strncmp(tempBuffer,"SecureMenuLevel", 15) == 0){ ///Menu security level
					memset(MenuSecurityLevel[rec], 0, sizeof(MenuSecurityLevel[rec]));
					memset(temp, 0, sizeof(temp));
					memset(tempBuffer, 0, sizeof(tempBuffer));
					ret = fmtTok(tempBuffer, Menu_Column_Array[MnuRec], ",");    //extract second token
					strcpy(temp, tempBuffer);
					strcpy(MenuSecurityLevel[rec], temp);
				} else if(strncmp(tempBuffer,"SecureMenu", 10) == 0){ ///Menu security check
					memset(MenuSecurity[rec], 0, sizeof(MenuSecurity[rec]));
					memset(temp, 0, sizeof(temp));
					memset(tempBuffer, 0, sizeof(tempBuffer));
					ret = fmtTok(tempBuffer, Menu_Column_Array[MnuRec], ",");    //extract second token
					strcpy(temp, tempBuffer);
					strcpy(MenuSecurity[rec], temp);
				}
			}
		}
		memset(MenuIcon[numberOfMenus], 0, sizeof(MenuIcon[numberOfMenus]));
		memset(MenuContent[numberOfMenus], 0, sizeof(MenuContent[numberOfMenus]));
		memset(MenuSecurity[numberOfMenus], 0, sizeof(MenuSecurity[numberOfMenus]));
		memset(MenuContent_ID[numberOfMenus], 0, sizeof(MenuContent_ID[numberOfMenus]));
		memset(MenuSecurityLevel[numberOfMenus], 0, sizeof(MenuSecurityLevel[numberOfMenus]));

		///--------------------------------------------------------------------------
		MAPPUTSTR(appAppLoggedName, "----", lblKO);
		////=========== Show The Menu to user ===========
#ifdef ICONMENU
		//--- Icon Menus
		int MenuSync = 0;
		for (MnuRec = 0; MnuRec < (numberOfMenus*2); MnuRec+=2) {
			//menu name
			appMenuContent[MnuRec] = MenuContent[MenuSync];
			//menu icon
			appMenuContent[MnuRec+1] = MenuIcon[MenuSync];

			MenuSync++;
		}
		appMenuContent[(numberOfMenus*2)] = NULL;
		if (key == 0) {
			memset(MenuSelectedName, 0, sizeof(MenuSelectedName));
			strcpy(MenuSelectedName, "Main Menu");
		}

		OpenPeripherals();
		//		LastMenuSelected = GL_Dialog_IconMenu(handleGoal, MenuSelectedName, appMenuContent, 0, GL_BUTTON_ALL, GL_KEY_0, GL_TIME_MINUTE);
		LastMenuSelected = GL_Dialog_IconButtonMenu(handleGoal, MenuSelectedName, appMenuContent, 0, GL_BUTTON_ALL, GL_KEY_0, GL_TIME_MINUTE);

#else
		//--- Normal List menu
		for (MnuRec = 0; MnuRec < numberOfMenus; MnuRec++) {
			appMenuContent[MnuRec] = MenuContent[MnuRec];
		}
		appMenuContent[numberOfMenus] = NULL;

		OpenPeripherals();
		LastMenuSelected = GL_Dialog_Menu(handleGoal, MenuSelectedName, appMenuContent, 0, GL_BUTTON_ALL, GL_KEY_0, GL_TIME_MINUTE);

#endif

		RE_DSP_MENU:
		if ((LastMenuSelected != GL_KEY_CANCEL) && (LastMenuSelected != GL_KEY_CORRECTION) && (LastMenuSelected != GL_RESULT_TIMEOUT) && (LastMenuSelected != GL_RESULT_INACTIVITY)) {

			//Save the Selected menu Item name for next header
			memset(MenuSelectedName, 0, sizeof(MenuSelectedName));
			strcpy(MenuSelectedName, MenuContent[LastMenuSelected]);

			//first save the current menu just in case the user wants to go back to previous menu
			keyLog[keyLogCounter] = key;
			keyLogCounter++;

			//Save the Menu Selected ID
			MAPPUTSTR(traMnuItm, MenuContent_ID[LastMenuSelected], lblKO);
			memset(TraMenuItem, 0, sizeof(TraMenuItem));
			strcpy(TraMenuItem, MenuContent_ID[LastMenuSelected]);
			dec2num(&key, MenuContent_ID[LastMenuSelected],0);
			MenuSelected = TRUE;

			// password checking
			memset(SecuredMenu, 0, sizeof(SecuredMenu));
			strncpy(SecuredMenu, MenuSecurity[LastMenuSelected],1);
			if (strncmp(SecuredMenu,"1", 1) == 0) {
				if(fncSecurityPassword(MenuSecurityLevel[LastMenuSelected]) == 0){
					// Case where the password was wrong
					if(keyLogCounter > 0)
						keyLogCounter--;
					key = keyLog[keyLogCounter];
				}
			}
		} else { ////The transaction is cancelled
			//// -------- Move back to previous menu option ------
			//TODO: (Kev-Code) Put here code to move back to previous menu
			if(LastMenuSelected == GL_KEY_CORRECTION){
				MenuSelected = TRUE;

				if(keyLogCounter > 0)
					keyLogCounter--;
				else
					MenuSelected = FALSE; //Means this was main menu exit the loop
				key = keyLog[keyLogCounter];
			}

			//// ---------- OR break off from the menu levels --------
			if((LastMenuSelected == GL_KEY_CANCEL) || (LastMenuSelected == GL_RESULT_TIMEOUT) || (LastMenuSelected == GL_RESULT_INACTIVITY)){
				MenuSelected = FALSE;
			}
		}

		////=========== End menu loop ==========
	}while(MenuSelected);


	goto lblEnd;
	lblKO:
	//resetting the tra info and the app info
	fncResetTerminalData();

	lblDB_Corrupt:
	if (!refreshed) {
		GL_Dialog_Message(hGoal, NULL, "Please Wait...", GL_ICON_NONE,GL_BUTTON_ALL, GL_TIME_SECOND);
		refreshed = 1;
		RefreshDB();
		goto lblReMENU;
	}

	lblEnd:
	OpenPeripherals();

	//clear dirty buffer
	MemoryManagerClearGarbage();

	return ret;
}
