#ifndef __MENU_MANAGER_H__
#define __MENU_MANAGER_H__

#include <globals.h>

void Generate_Menu_Content(void);
void OldMenu(T_GL_HGRAPHIC_LIB handleGoal);
int Manage_Application_Menu(T_GL_HGRAPHIC_LIB handleGoal);
int MenuProcessingSelect(word MnuItm);
int Application_Do_Transaction(void);
int Application_Card_Transaction(void);
void Application_Do_Transaction_Idle(void);
void onlSession(void);


#define lenMnu_Statement 256


char var_AidTable[lenMnu_Statement];             // INGETRAIN

char var_MnuMainMenu[lenMnu_Statement];             // INGETRAIN
char var_MnuCustomer[lenMnu_Statement];             // CUSTOMER>
char var_MnuSale[lenMnu_Statement];                 // SALE
char var_MnuSaleCB[lenMnu_Statement];               // SALE + CASH BACK
char var_MnuDeposit[lenMnu_Statement];              // DEPOSIT
char var_MnuWithdrawal[lenMnu_Statement];           // WITHDRAWAL
char var_MnuPreaut[lenMnu_Statement];               // PREAUTHORIZATION
char var_MnuCompletion[lenMnu_Statement];           // COMPLETION
char var_MnuBalanceInquiry[lenMnu_Statement];       // BALANCE INQUIRY
char var_MnuMiniSatement[lenMnu_Statement];         // MINI STATEMENT
char var_MnuPayment[lenMnu_Statement];              // PAYMENT TRANSACTION
char var_MnuRefund[lenMnu_Statement];               // REFUND
char var_MnuOffline[lenMnu_Statement];              // OFFLINE
char var_MnuVoid[lenMnu_Statement];                 // VOID
char var_MnuBiller[lenMnu_Statement];               // BILL PAYMENTS
char var_MnuAdjust[lenMnu_Statement];               // ADJUST
char var_MnuReversal[lenMnu_Statement];             // REVERSAL
char var_MnuLogOn[lenMnu_Statement];                // LOG ON
char var_MnuEchoTest[lenMnu_Statement];             // ECHO TEST
char var_MnuCustSettlement[lenMnu_Statement];       // CUSTOMER SETTLEMENT

char var_MnuMerchant[lenMnu_Statement];             // MERCHANT>
char var_MnuSettlement[lenMnu_Statement];           // SETTLEMENT
char var_MnuDetailedRpt[lenMnu_Statement];          // DETAILED REPORT
char var_MnuSummaryRpt[lenMnu_Statement];           // SUMMARY REPORT
char var_MnuDuplicateReceipt[lenMnu_Statement];     // DUPLICATE RECEIPT
char var_MnuReprintReceipt[lenMnu_Statement];       // REPRINT RECEIPT
char var_MnuAdmChgPwd[lenMnu_Statement];            // ADMIN PASSWORD
char var_MnuMrcChgPwd[lenMnu_Statement];            // MERCH PASSWORD
char var_MnuMrcReset[lenMnu_Statement];             // DELETE BATCH
char var_MnuMrcResetRev[lenMnu_Statement];          // DELETE REVERSAL
char var_MnuMngUsers[lenMnu_Statement];             // MANAGE USER
char var_MnuBillerResend[lenMnu_Statement];         // BILLER RESEND ADVICE
char var_MnuAgent[lenMnu_Statement];                // AGENT>
char var_MnuAgencyDeposit[lenMnu_Statement];        // AGENT>

char var_MnuSupervisor[lenMnu_Statement];           // SUPERVISOR>

char var_MnuAdmin[lenMnu_Statement];                // ADMIN>
char var_MnuTerminalPar[lenMnu_Statement];          // Terminal Parameters
char var_MnuBillerConfig[lenMnu_Statement];         // Biller Configure data
char var_MnuTMKey[lenMnu_Statement];    	        // Master key load
char var_MnuGenerateTLSkey[lenMnu_Statement];    	        // Master key load
char var_MnuCmmIS[lenMnu_Statement];                // IP Setup
char var_MnuClessModeOff[lenMnu_Statement];         // Terminal Cless Mode
char var_MnuConnMode[lenMnu_Statement];             // Terminal Connection Mode  Gprs/Ethernet/bluetooth/usb/serial
char var_MnuControlPanel[lenMnu_Statement];         // Terminal Control panel mode enabled
char var_MnuSwapSimSlot [lenMnu_Statement];         // Terminal Sim Slot Management
char var_MnuCvmMode[lenMnu_Statement];              // Terminal Connection Mode  Gprs/Ethernet/bluetooth/usb/serial
char var_MnuTraces[lenMnu_Statement];               // Terminal Traces over USB
char var_MnuTerminalMode[lenMnu_Statement];         // Terminal Mode selection

#endif
