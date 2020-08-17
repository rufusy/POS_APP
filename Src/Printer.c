//****************************************************************************
//       INGENICO                                INGEDEV 7                   
//============================================================================
//       FILE  PRINTER.C                          (Copyright INGENICO 2012)
//============================================================================
//  Created :       10-July-2012     Kassovic
//  Last modified : 10-July-2012     Kassovic
//  Module : TRAINING                                                          
//                                                                          
//  Purpose :
//                       *** Printer management ***
//  This file uses the Graphic Object Advanced Library (GOAL) for printing:
//    # Manager goal must be loaded
//    # Customized mode
//      . By API(s) (used inside this file)
//        Build your document using API(s) widgets
//                                                                            
//  List of routines in file :  
//      PrintDocument : To show the capabilities of the printer when using GOAL.
//                            
//  File history :
//  071012-BK : File created
//                                                                           
//****************************************************************************

//****************************************************************************
//      INCLUDES                                                            
//****************************************************************************
#include <globals.h>
#include "Sqlite.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

//****************************************************************************
//      EXTERN                                                              
//****************************************************************************
extern T_GL_HGRAPHIC_LIB hGoal; // Handle of the graphics object library

//****************************************************************************
//      PRIVATE CONSTANTS                                                   
//****************************************************************************
#ifndef __TELIUM3__
#define FONT_GOAL_LATIN_IT   0xACD5
#define FONT_GOAL_LATIN_BO   0xACD6
#define FONT_GOAL_LATIN_BI   0xACD7
#else
#define FONT_GOAL_LATIN_IT   0x3E7ACD5 // Domain(0x3E7=999) AppliType=ACD5 (Ingenico Font)
#define FONT_GOAL_LATIN_BO   0x3E7ACD6 // Domain(0x3E7=999) AppliType=ACD6 (Ingenico Font)
#define FONT_GOAL_LATIN_BI   0x3E7ACD7 // Domain(0x3E7=999) AppliType=ACD7 (Ingenico Font)
#endif


const char *ISO_Resp[] = {
		"Approved",
		"Refer to Card Issuer",
		"Refer to Issuer's special conditions",
		"Invalid Merchant",
		"Pick Up Card",
		"Do Not Honor",
		"Error",
		"Pick Up Card, Special Conditions",
		"Honor with identification",
		"Request in Progress",
		"Partial Amount Approved",
		"VIP Approval",
		"Invalid Transaction",
		"Invalid Amount",
		"Invalid Card Number",
		"No Such Issuer",
		"Approved, update track 3",
		"Customer Cancellation",
		"Customer Dispute",
		"Re-enter Transaction",
		"Invalid Response",
		"No Action Taken (no match)",
		"Suspected Malfunction",
		"Unacceptable Transaction Fee",
		"File Update not Supported by Receiver",
		"Unable to Locate Record on File",
		"Duplicate File Update Record",
		"File Update Field Edit Error",
		"File Update File Locked Out",
		"File Update not Successful",
		"Format Error",
		"Bank not Supported by Switch",
		"Completed Partially",
		"Expired Card - Pick Up",
		"Suspected Fraud - Pick Up",
		"Contact Acquirer - Pick Up",
		"Restricted Card - Pick Up",
		"Call Acquirer Security - Pick Up",
		"Allowable PIN Tries Exceeded",
		"No CREDIT Account",
		"Requested Function not Supported",
		"Lost Card - Pick Up",
		"No Universal Amount",
		"Stolen Card - Pick Up",
		"No Investment Account",
		"   ",
		"   ",
		"   ",
		"   ",
		"   ",
		"   ",
		"Insufficient Funds",
		"No Cheque Account",
		"No Savings Account",
		"Expired Card",
		"Incorrect PIN",
		"No Card Record",
		"Trans. not Permitted to Cardholder",
		"Transaction not Permitted to Terminal",
		"Suspected Fraud",
		"Card Acceptor Contact Acquirer",
		"Exceeds Withdrawal Amount Limits",
		"Restricted Card",
		"Security Violation",
		"Original Amount Incorrect",
		"Exceeds Withdrawal Frequency Limit",
		"Card Acceptor Call Acquirer Security",
		"Hard Capture - Pick Up Card at ATM",
		"Response Received Too Late",
		"   ",
		"   ",
		"   ",
		"   ",
		"   ",
		"   ",
		"Allowable PIN Tries Exceeded",
		"Previous message not found",
		"Data does not match original message",
		"   ",
		"   ",
		"Invalid Date",
		"Cryptographic failure",
		"Incorrect CVV",
		"Unable to verify PIN",
		"Invalid authorization life cycle",
		"No reason to decline",
		"ATM Malfunction",
		"No Envelope Inserted",
		"Unable to Dispense",
		"Administration Error",
		"Cut-off in Progress",
		"Issuer or Switch is Inoperative",
		"Financial Institution Not Found",
		"Trans Cannot be Completed",
		"Duplicate Transmission",
		"Reconcile Error",
		"System Malfunction",
		"Reconciliation Totals Reset",
		"MAC Error",
		"Reserved for National Use",
		"CALL HELP",
		0
};

//
//const char *ISO_Resp[] = {
//		"Successful approval/completion or that VIP PIN verification is valid",
//		"Refer to card issuer",
//		"Refer to card issuer, special condition",
//		"Invalid merchant or service provider",
//		"Pickup",
//		"Do not honor",
//		"General error",
//		"Pickup card, special condition (other than lost/stolen card)",
//		"Honor with identification",
//		"Request in progress",
//		"Partial approval",
//		"VIP approval",
//		"Invalid transaction",
//		"Invalid amount (currency conversion field overflow) or amount exceeds maximum for card program",
//		"Invalid account number (no such number)",
//		"No such issuer",
//		"Insufficient funds",
//		"Customer cancellation",
//		" ",
//		"Re-enter transaction",
//		"Invalid response",
//		"No action taken (unable to back out prior transaction)",
//		"Suspected Malfunction",
//		" ",
//		" ",
//		"Unable to locate record in file, or account number is missing from the inquiry",
//		" ",
//		" ",
//		"File is temporarily unavailable",
//		" ",
//		"Format error",
//		" ",
//		" ",
//		" ",
//		" ",
//		" ",
//		" ",
//		" ",
//		" ",
//		" ",
//		" ",
//		"Merchant should retain card (card reported lost)",
//		" ",
//		"Merchant should retain card (card reported stolen)",
//		" ",
//		" ",
//		" ",
//		" ",
//		" ",
//		" ",
//		" ",
//		"Insufficient funds",
//		"No checking account",
//		"No savings account",
//		"Expired card",
//		"Incorrect PIN",
//		" ",
//		"Transaction not permitted to cardholder",
//		"Transaction not allowed at terminal",
//		"Suspected fraud",
//		" ",
//		"Activity amount limit exceeded",
//		"Restricted card (for example, in country exclusion table)",
//		"Security violation",
//		" ",
//		"Activity count limit exceeded",
//		" ",
//		" ",
//		"Response received too late",
//		" ",
//		" ",
//		" ",
//		" ",
//		" ",
//		" ",
//		"Allowable number of PIN-entry tries exceeded",
//		"Unable to locate previous message (no match on retrieval reference number)",
//		"Previous message located for a repeat or reversal, but repeat or reversal data are inconsistent with original message",
//		"Blocked, first used —The transaction is from a new cardholder, and the card has not been properly unblocked.",
//		" ",
//		"Visa transactions: credit issuer unavailable. Private label and check acceptance: Invalid date",
//		"PIN cryptographic error found (error found by VIC security module during PIN decryption)",
//		"Negative CAM, dCVV, iCVV, or CVV results",
//		"Unable to verify PIN",
//		" ",
//		"No reason to decline a request for account number verification, address verification, CVV2 verification; or a credit voucher or merchandise return",
//		" ",
//		" ",
//		" ",
//		" ",
//		" ",
//		"Issuer unavailable or switch inoperative (STIP not applicable or available for this transaction)",
//		"Destination cannot be found for routing",
//		"Transaction cannot be completed, violation of law",
//		"Duplicate transmission",
//		"Reconcile error",
//		"System malfunction, System malfunction or certain field error conditions",
//		0
//};


const char *HypercomResp[] = {
		"Approved",
		"REFER TO ISSUER",
		"REFER TO ISSUER SPCL",
		"ERROR-CALL HELP - SN",
		"   ",
		"DO NOT HONOUR",
		"   ",
		"   ",
		"   ",
		"ACCEPTED",
		"APPROVED PARTIAL",
		"   ",
		"INVALID TRANSACTION",
		"INVALID AMOUNT",
		"INVALID CARD READER",
		"   ",
		"   ",
		"   ",
		"   ",
		"RE-ENTER TRANSACTION",
		"   ",
		"   ",
		"   ",
		"   ",
		"   ",
		"UNABLE TO LOCATE RECORD",
		"   ",
		"   ",
		"   ",
		"   ",
		"FORMAT ERROR",
		"NOT SUPPORTED BY SWITCH",
		"   ",
		"   ",
		"   ",
		"   ",
		"   ",
		"   ",
		"   ",
		"   ",
		"   ",
		"PLEASE CALL - LC",
		"   ",
		"PLEASE CALL - CC",
		"   ",
		"   ",
		"   ",
		"   ",
		"   ",
		"   ",
		"DECLINED", //INSUFFICIENT FUNDS
		"   ",
		"   ",
		"EXPIRED CARD",
		"INCORRECT PIN",
		"   ",
		"   ",
		"TRANS. NOT PERMITTED TO TERMINAL",
		"   ",
		"   ",
		"   ",
		"   ",
		"   ",
		"   ",
		"   ",
		"USE CHIP INTERFACE",
		"   ",
		"   ",
		"   ",
		"   ",
		"   ",
		"   ",
		"   ",
		"   ",
		"   ",
		"INVALID PROD. CODE",
		"RECONCILE ERROR",
		"TRANS. NOT FOUND",
		"DECLINED - CVV2",
		"BAD BATCH NUMBER",
		"   ",
		"NO CLOSED SOC SLOTS",
		"NO SUSP. SOC SLOTS",
		"   ",
		"BATCH NOT FOUND",
		"   ",
		"   ",
		"   ",
		"BAD TERMINAL ID",
		"   ",
		"ISSUER INOPERATIVE",
		"   ",
		"   ",
		"DUPLICATE TRANSMISSION",
		"BATCH TRANSFER, WAIT",
		"SYSTEM MALFUNCTION",
		"TIMEOUT ON RESPONSE ",
		"CALL HELP ",
		"CALL HELP ",
		"CALL HELP ",
		0
};


void getResponse_NonNumeric(char * ResponseName, char *rspCode){

	if (strncmp(rspCode, "N0", 2) == 0) {
		strcpy(ResponseName, "Force STIP (VISA)");
	} else if (strncmp(rspCode, "N3", 2) == 0) {
		strcpy(ResponseName, "Cash Service Not Available (VISA)");
	} else if (strncmp(rspCode, "N4", 2) == 0) {
		strcpy(ResponseName, "Cash request exceeds issuer limit (VISA)");
	} else if (strncmp(rspCode, "N7", 2) == 0) {
		strcpy(ResponseName, "Decline for CVV2 failure (VISA)");
	} else if (strncmp(rspCode, "P2", 2) == 0) {
		strcpy(ResponseName, "Invalid biller information (VISA)");
	} else if (strncmp(rspCode, "P5", 2) == 0) {
		strcpy(ResponseName, "PIN Change Unblock Declined (VISA)");
	} else if (strncmp(rspCode, "P6", 2) == 0) {
		strcpy(ResponseName, "Unsafe PIN (VISA)");
	} else if (strncmp(rspCode, "XA", 2) == 0) {
		strcpy(ResponseName, "Forward to issuer");
	} else if (strncmp(rspCode, "XD", 2) == 0) {
		strcpy(ResponseName, "Forward to issuer");
	} else {
		strcpy(ResponseName, "TRANSACTION CANCELLED");
	}

}

void getResponse(char * ResponseName, card rspCode){
	char ResponseData[256];

	memset(ResponseData, 0, sizeof(ResponseData));

	strcpy(ResponseData, ISO_Resp[rspCode]);

	if (strlen(ResponseData)<2) {
		strcpy(ResponseData, HypercomResp[rspCode]);
	}
	strtouppercase(ResponseData);

	strcpy(ResponseName, ResponseData);
}

//****************************************************************************
//      PRIVATE TYPES                                                       
//****************************************************************************
// Properties of the default printer (Goal)
// ========================================
static const ST_PRN_LINE xPrinter =
{
		GL_ALIGN_CENTER, GL_ALIGN_CENTER, FALSE, 0, {0, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {1, 0, 1, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_XXSMALL}
};

static const ST_PADDING xPadding = {4, 0, 4, 0};              // Padding properties
static const ST_BORDER xBorder = {4, 4, 4, 4, GL_COLOR_BLACK}; // Border properties
static const ST_MARGIN xMargin = {1, 10, 1, 10};               // Margin properties

//****************************************************************************
//      PRIVATE DATA                                                        
//****************************************************************************
/* */

//****************************************************************************
//                          void PrintDocument (void)
//  This function shows the capabilities of the printer when using GOAL.
//  The following functions has been developed in customized mode using API(s)
//  widgets (See file Goal.c):
//   - GoalCreateDocument() : To build widgets on a document.
//   - GoalPrnLine() : To add a line on document.
//   - GoalPrnDocument : To print the document.
//   - GoalDestroyDocument() : To destroy all widgets on a document.
//   Support all terminals with an internal printer.
//   GOAL LATIN italic, bold, bold italic fonts may be loaded to get the full
//   options.
//  This function has no parameters.    
//  This function has no return value.                                      
//****************************************************************************

void PrintDocument(void) {
	// Local variables 
	// ***************
	T_GL_HWIDGET xDocument=NULL;
	ST_PRN_LINE xLine;
	byte p; // Printer line index
	bool bFontOk;
	int iRet;

	// Printing in progress
	// ********************
	GL_Dialog_Message(hGoal, NULL, "Please Wait""\n""Printing In Progress", GL_ICON_INFORMATION, GL_BUTTON_NONE, 0);

	xDocument = GoalCreateDocument(hGoal, GL_ENCODING_UTF8);
	CHECK(xDocument!=NULL, lblKO);                  // Create document

	p=0;                                            // *** Font scale capability ***
	xLine = xPrinter;
	xLine.bReverse = TRUE;
	xLine.usSizeWidth = 100;
	xLine.xMargin.usBottom = PIXEL_BOTTOM;
	xLine.xFont.eScale = GL_SCALE_XXLARGE;
	iRet = GoalPrnLine(xDocument, p++, "Font Scale", &xLine);
	CHECK(iRet>=0, lblKO);

	xLine = xPrinter;                               // 'xxsmall' size (see properties)
	iRet = GoalPrnLine(xDocument, p++, "XXsmall", &xLine);
	CHECK(iRet>=0, lblKO);

	xLine.xFont.eScale = GL_SCALE_XSMALL;           // 'xsmall' size (see properties)
	iRet = GoalPrnLine(xDocument, p++, "Xsmall", &xLine);
	CHECK(iRet>=0, lblKO);

	xLine.xFont.eScale = GL_SCALE_SMALL;            // 'small' size  (see properties)
	iRet = GoalPrnLine(xDocument, p++, "Small", &xLine);
	CHECK(iRet>=0, lblKO);

	xLine.xFont.eScale = GL_SCALE_MEDIUM;           // 'medium' size (see properties)
	iRet = GoalPrnLine(xDocument, p++, "Medium", &xLine);
	CHECK(iRet>=0, lblKO);

	xLine.xFont.eScale = GL_SCALE_LARGE;            // 'large' size (see properties)
	iRet = GoalPrnLine(xDocument, p++, "Large", &xLine);
	CHECK(iRet>=0, lblKO);

	xLine.xFont.eScale = GL_SCALE_XLARGE;           // 'xlarge' size (see properties)
	iRet = GoalPrnLine(xDocument, p++, "Xlarge", &xLine);
	CHECK(iRet>=0, lblKO);

	xLine.xFont.eScale = GL_SCALE_XXLARGE;          // 'xxlarge' size (see properties)
	iRet = GoalPrnLine(xDocument, p++, "XXlarge""\n\n", &xLine);
	CHECK(iRet>=0, lblKO);

	xLine = xPrinter;                               // *** Font style capabilities ***
	xLine.bReverse = TRUE;
	xLine.usSizeWidth = 100;
	xLine.xMargin.usBottom = PIXEL_BOTTOM;
	xLine.xFont.eScale = GL_SCALE_XXLARGE;
	iRet = GoalPrnLine(xDocument, p++, "Font Style", &xLine);
	CHECK(iRet>=0, lblKO);

	xLine = xPrinter;                               // 'normal' style (see properties)
	xLine.xFont.eScale = GL_SCALE_XXLARGE;
	iRet = GoalPrnLine(xDocument, p++, "NORMAL", &xLine);
	CHECK(iRet>=0, lblKO);

	xLine.xFont.eStyle = GL_FONT_STYLE_BOLD;        // 'bold' style (see properties)
	bFontOk = IngenicoFontExist(FONT_GOAL_LATIN_BO, "GOAL_LATIN_BO");  // Check if Latin bold font loaded?
	if (bFontOk)
		iRet = GoalPrnLine(xDocument, p++, "BOLD", &xLine);
	else
		iRet = GoalPrnLine(xDocument, p++, "BOLD (x)", &xLine);
	CHECK(iRet>=0, lblKO);

	xLine.xFont.eStyle = GL_FONT_STYLE_ITALIC;      // 'italic' style (see properties)
	bFontOk = IngenicoFontExist(FONT_GOAL_LATIN_IT, "GOAL_LATIN_IT");  // Check if Latin italic font loaded?
	if (bFontOk)
		iRet = GoalPrnLine(xDocument, p++, "ITALIC", &xLine);
	else
		iRet = GoalPrnLine(xDocument, p++, "ITALIC (x)", &xLine);
	CHECK(iRet>=0, lblKO);

	xLine.xFont.eStyle = GL_FONT_STYLE_BOLD_ITALIC; // 'bold italic' style (see properties)
	bFontOk = IngenicoFontExist(FONT_GOAL_LATIN_BI, "GOAL_LATIN_BI");  // Check if Latin bold italic font loaded?
	if (bFontOk)
		iRet = GoalPrnLine(xDocument, p++, "BOLD ITALIC""\n\n", &xLine);
	else
		iRet = GoalPrnLine(xDocument, p++, "BOLD ITALIC (x)""\n\n", &xLine);
	CHECK(iRet>=0, lblKO);

	xLine = xPrinter;                               // *** Font format capabilities ***
	xLine.bReverse = TRUE;
	xLine.usSizeWidth = 100;
	xLine.xMargin.usBottom = PIXEL_BOTTOM;
	xLine.xFont.eScale = GL_SCALE_XXLARGE;
	iRet = GoalPrnLine(xDocument, p++, "Font Format", &xLine);
	CHECK(iRet>=0, lblKO);

	xLine = xPrinter;                               // 'center' adjust (see properties)
	xLine.xFont.eScale = GL_SCALE_XXLARGE;
	iRet = GoalPrnLine(xDocument, p++, "CENTER", &xLine);
	CHECK(iRet>=0, lblKO);

	xLine.eBackAlign = GL_ALIGN_LEFT;               // 'left' adjust (see properties)
	iRet = GoalPrnLine(xDocument, p++, "LEFT", &xLine);
	CHECK(iRet>=0, lblKO);

	xLine.eBackAlign = GL_ALIGN_RIGHT;              // 'right' adjust (see properties)
	iRet = GoalPrnLine(xDocument, p++, "RIGHT", &xLine);
	CHECK(iRet>=0, lblKO);

	xLine.eBackAlign = GL_ALIGN_CENTER;             // 'border center' adjust (see properties)
	xLine.xPadding = xPadding;
	xLine.xBorder = xBorder;
	iRet = GoalPrnLine(xDocument, p++, "BORDER", &xLine);
	CHECK(iRet>=0, lblKO);

	xLine.bReverse = TRUE;                          // 'reverse center' adjust (see properties)
	xLine.xMargin = xMargin;
	iRet = GoalPrnLine(xDocument, p++, "REVERSE", &xLine);
	CHECK(iRet>=0, lblKO);

	xLine.bReverse = FALSE;                         // 'normal center' adjust (see properties)
	xLine.xBorder = xPrinter.xBorder;
	iRet = GoalPrnLine(xDocument, p++, "NORMAL""\n\n", &xLine);
	CHECK(iRet>=0, lblKO);

	xLine = xPrinter;                               // *** Print text capabilities ***
	xLine.bReverse = TRUE;
	xLine.usSizeWidth = 100;
	xLine.xMargin.usBottom = PIXEL_BOTTOM;
	xLine.xFont.eScale = GL_SCALE_XXLARGE;
	iRet = GoalPrnLine(xDocument, p++, "Print Text", &xLine);
	CHECK(iRet>=0, lblKO);

	xLine = xPrinter;                               // Short string
	xLine.eTextAlign = GL_ALIGN_LEFT;
	xLine.usSizeWidth = 100;
	xLine.xFont.eScale = GL_SCALE_LARGE;
	iRet = GoalPrnLine(xDocument, p++, "Short string", &xLine);
	CHECK(iRet>=0, lblKO);                          // Long string
	iRet = GoalPrnLine(xDocument, p++, "Now try a long string:\n"
			"0123456789012345678901234567890123456789""\n\n\n\n\n", &xLine);
	CHECK(iRet>=0, lblKO);

	iRet = GoalPrnDocument(xDocument);              // Print document
	CHECK(iRet>=0, lblKO);

	iRet=1;

	goto lblEnd;

	// Errors treatment 
	// ****************
	lblKO:                                              // None-classified low level error
	GL_Dialog_Message(hGoal, NULL, "Processing Error", GL_ICON_ERROR, GL_BUTTON_VALID, 5*1000);
	goto lblEnd;
	lblEnd:
	if (xDocument)
		GoalDestroyDocument(&xDocument);            // Destroy document
}







/** This function is masking pan before printing.
 * - Format PAN with mask
 */
int MaskPan(void) {
	byte idx = 0;
	char pan[lenPan + 1];
	int ret;

	// Pan with mask
	ret = mapGet(traPan, pan, lenPan + 1);
	fncPanRemove_F(pan);
	CHECK(ret >= 0, lblDBA);
	CHECK(strlen(pan) > 7, lblDBA);

	for (idx = 6; idx < (strlen(pan)-4); idx++){ // Mask almost BIN
		pan[idx] = '*';
	}

	for (idx = 15; idx < (strlen(pan)); idx++){ // Remove F paddings
		if (pan[idx] == 'F') {
			pan[idx] = 0;
		}
	}

	MAPPUTSTR(traMaskPan, pan,lblDBA);

	strFormatDatTim(traDatTim);

	ret = 1;
	goto lblEnd;

	lblDBA:
	goto lblEnd;

	lblEnd:
	return ret;
}



/** This function is masking pan before printing.
 * - Format PAN with mask
 */
int MaskPan_Other(char *Pan, char *MaskedPan ) {
	byte idx = 0;
	char TempPan[lenPan + 5];
	int ret;

	memset(TempPan, 0, sizeof(TempPan));
	// Pan with mask
	strcpy(TempPan, Pan);

	CHECK(strlen(TempPan) > 7, lblDBA);

	for (idx = 6; idx < (strlen(TempPan)-4); idx++){ // Mask almost BIN
		TempPan[idx] = '*';
	}

	for (idx = 15; idx < (strlen(TempPan)); idx++){ // Remove F paddings
		if (TempPan[idx] == 'F') {
			TempPan[idx] = 0;
		}
	}

	strcpy(MaskedPan, TempPan);

	strFormatDatTim(traDatTim);

	ret = 1;
	goto lblEnd;

	lblDBA:
	goto lblEnd;

	lblEnd:
	return ret;
}

static byte printLogo(T_GL_HWIDGET xLayout,byte xline){
	// Local variables
	// ***************
	T_GL_HWIDGET xPicture;
	//	T_GL_HWIDGET xLogoLayout;
	T_GL_HWIDGET xPrint;
	char PrintData[999 + 1];
	int ret = 0;

	xPicture = GL_Picture_Create (xLayout);
	GL_Widget_SetSource        (xPicture, "file://flash/HOST/PRNTLOGO.PNG"); //Set the xPicture to print
	GL_Widget_SetItem          (xPicture, 0, xline++);

	//Merchant  Name
	memset(PrintData, 0, sizeof(PrintData));
	MAPGET(appMerchantName, PrintData,lblKO);
	xPrint = GL_Print_Create    (xLayout);
	GL_Widget_SetWrap      (xPrint, TRUE);
	GL_Widget_SetText      (xPrint, PrintData);
	GL_Widget_SetItem      (xPrint, 0, xline++);
	GL_Widget_SetMargins   (xPrint, 0, 0, 0, 0, GL_UNIT_PIXEL);
	GL_Widget_SetFontScale (xPrint, GL_SCALE_XLARGE);
	GL_Widget_SetBackAlign (xPrint, GL_ALIGN_CENTER);
	GL_Widget_SetFontStyle (xPrint, GL_FONT_STYLE_BOLD);

	//Header 1
	memset(PrintData, 0, sizeof(PrintData));
	MAPGET(appHeader1, PrintData,lblKO);
	if(strlen(PrintData) > 0){
		xPrint = GL_Print_Create    (xLayout);
		GL_Widget_SetText      (xPrint, PrintData);
		GL_Widget_SetItem      (xPrint, 0, xline++);
		GL_Widget_SetMargins   (xPrint, 0, 0, 0, 0, GL_UNIT_PIXEL);
		GL_Widget_SetFontScale (xPrint, GL_SCALE_MEDIUM);
		GL_Widget_SetBackAlign (xPrint, GL_ALIGN_CENTER);
		GL_Widget_SetFontStyle (xPrint, GL_FONT_STYLE_BOLD);
		GL_Widget_SetWrap      (xPrint, TRUE);
	}

	//Header 2
	memset(PrintData, 0, sizeof(PrintData));
	MAPGET(appHeader2, PrintData,lblKO);
	if(strlen(PrintData) > 0){
		xPrint = GL_Print_Create    (xLayout);
		GL_Widget_SetText      (xPrint, PrintData);
		GL_Widget_SetItem      (xPrint, 0, xline++);
		GL_Widget_SetFontScale (xPrint, GL_SCALE_MEDIUM);
		GL_Widget_SetMargins   (xPrint, 0, 0, 0, 0, GL_UNIT_PIXEL);
		GL_Widget_SetBackAlign (xPrint, GL_ALIGN_CENTER);
		GL_Widget_SetFontStyle (xPrint, GL_FONT_STYLE_BOLD);
		GL_Widget_SetWrap      (xPrint, TRUE);
	}

	//Header 3
	memset(PrintData, 0, sizeof(PrintData));
	MAPGET(appHeader3, PrintData,lblKO);
	if(strlen(PrintData) > 0){
		xPrint = GL_Print_Create    (xLayout);
		GL_Widget_SetText      (xPrint, PrintData);
		GL_Widget_SetItem      (xPrint, 0, xline++);
		GL_Widget_SetMargins   (xPrint, 0, 0, 0, 0, GL_UNIT_PIXEL);
		GL_Widget_SetFontScale (xPrint, GL_SCALE_MEDIUM);
		GL_Widget_SetBackAlign (xPrint, GL_ALIGN_CENTER);
		//		GL_Widget_SetFontStyle (xPrint, GL_FONT_STYLE_BOLD);
		GL_Widget_SetWrap      (xPrint, TRUE);
	}

	lblKO:
	return xline;
}



static byte printEMVdetails(T_GL_HWIDGET xLayout,byte xline){
	// Local variables
	// ***************
	T_GL_HWIDGET xPrint;
	char PrintData[64 + 1];
	char tempdata[64 + 1];
	int ret = 0;
	//EMV : TC
	memset(PrintData, 0, sizeof(PrintData));
	memset(tempdata, 0,sizeof(tempdata));
	strcpy(PrintData, "TC  --- : ");
	MAPGET(traIssTrnCrt, tempdata,lblKO); //tag = 9F26
	strcat(PrintData,tempdata);
	if (strlen(tempdata) > 1) {
		xPrint = GL_Print_Create    (xLayout);
		GL_Widget_SetText      (xPrint, PrintData);
		GL_Widget_SetItem      (xPrint, 0, xline++);
		GL_Widget_SetMargins   (xPrint, 0, 0, 8, 0, GL_UNIT_PIXEL);
		GL_Widget_SetFontScale (xPrint, GL_SCALE_MEDIUM);
		GL_Widget_SetBackAlign (xPrint, GL_ALIGN_LEFT);
	}

	//EMV : AID
	memset(PrintData, 0, sizeof(PrintData));
	memset(tempdata, 0,sizeof(tempdata));
	strcpy(PrintData, "AID --- : ");
	MAPGET(traAID, tempdata,lblKO); //tag = 9F26
	strncat(PrintData,tempdata, 18);
	if (strncmp(tempdata, "0000000000", 10) != 0) {
		if (strlen(tempdata) > 1) {
			xPrint = GL_Print_Create    (xLayout);
			GL_Widget_SetText      (xPrint, PrintData);
			GL_Widget_SetItem      (xPrint, 0, xline++);
			GL_Widget_SetMargins   (xPrint, 0, 0, 8, 0, GL_UNIT_PIXEL);
			GL_Widget_SetFontScale (xPrint, GL_SCALE_MEDIUM);
			GL_Widget_SetBackAlign (xPrint, GL_ALIGN_LEFT);
		}
	}

	//EMV : TVR
	memset(PrintData, 0, sizeof(PrintData));
	memset(tempdata, 0,sizeof(tempdata));
	strcpy(PrintData, "TVR --- : ");
	MAPGET(traTVR, tempdata,lblKO); //tag = 95
	strcat(PrintData,tempdata);
	if (strlen(tempdata) > 1) {
		xPrint = GL_Print_Create    (xLayout);
		GL_Widget_SetText      (xPrint, PrintData);
		GL_Widget_SetItem      (xPrint, 0, xline++);
		GL_Widget_SetMargins   (xPrint, 0, 0, 8, 0, GL_UNIT_PIXEL);
		GL_Widget_SetFontScale (xPrint, GL_SCALE_MEDIUM);
		GL_Widget_SetBackAlign (xPrint, GL_ALIGN_LEFT);
	}

	//EMV : TSI
	memset(PrintData, 0, sizeof(PrintData));
	memset(tempdata, 0,sizeof(tempdata));
	strcpy(PrintData, "TSI --- : ");
	MAPGET(traTSI, tempdata,lblKO); //tag = 9B
	strcat(PrintData,tempdata);
	if (strlen(tempdata) > 1) {
		xPrint = GL_Print_Create    (xLayout);
		GL_Widget_SetText      (xPrint, PrintData);
		GL_Widget_SetItem      (xPrint, 0, xline++);
		GL_Widget_SetMargins   (xPrint, 0, 0, 8, 0, GL_UNIT_PIXEL);
		GL_Widget_SetFontScale (xPrint, GL_SCALE_MEDIUM);
		GL_Widget_SetBackAlign (xPrint, GL_ALIGN_LEFT);
	}

	//EMV : CVMR
	memset(PrintData, 0, sizeof(PrintData));
	memset(tempdata, 0,sizeof(tempdata));
	strcpy(PrintData, "CVMR --- : ");
	MAPGET(traCVMR, tempdata,lblKO); //tag = 9B
	strcat(PrintData,tempdata);
	if (strlen(tempdata) > 1) {
		xPrint = GL_Print_Create    (xLayout);
		GL_Widget_SetText      (xPrint, PrintData);
		GL_Widget_SetItem      (xPrint, 0, xline++);
		GL_Widget_SetMargins   (xPrint, 0, 0, 8, 0, GL_UNIT_PIXEL);
		GL_Widget_SetFontScale (xPrint, GL_SCALE_MEDIUM);
		GL_Widget_SetBackAlign (xPrint, GL_ALIGN_LEFT);
	}

	lblKO:
	return xline;
}

static byte printCardDetails(T_GL_HWIDGET xLayout, byte xline){
	// Local variables
	// ***************
	T_GL_HWIDGET xPrint;

	char PrintData[100 + 1];
	char tempdata[76];
	int ret = 0;

	memset(tempdata, 0, sizeof(tempdata));

	//Mask PAN
	memset(PrintData, 0, sizeof(PrintData));
	MAPGET(traMaskPan, PrintData,lblKO);
	xPrint = GL_Print_Create    (xLayout);
	GL_Widget_SetText      (xPrint, PrintData);
	GL_Widget_SetItem      (xPrint, 0, xline++);
	GL_Widget_SetFontScale (xPrint, GL_SCALE_LARGE);
	GL_Widget_SetBackAlign (xPrint, GL_ALIGN_LEFT);
	GL_Widget_SetFontStyle (xPrint, GL_FONT_STYLE_BOLD);

	//	//Expiry Date
	//	memset(PrintData, 0, sizeof(PrintData));
	//	MAPGET(traExpDat, PrintData,lblKO);
	//	xPrint = GL_Print_Create    (xLayout);
	//	GL_Widget_SetText      (xPrint, PrintData);
	//	GL_Widget_SetItem      (xPrint, 0, xline++);
	//	GL_Widget_SetMargins   (xPrint, 0, 0, 8, 0, GL_UNIT_PIXEL);
	//	GL_Widget_SetFontScale (xPrint, GL_SCALE_MEDIUM);
	//	GL_Widget_SetBackAlign (xPrint, GL_ALIGN_RIGHT);
	//	GL_Widget_SetFontStyle (xPrint, GL_FONT_STYLE_BOLD);

	//Issuer Name  -  Application Label
	memset(PrintData, 0, sizeof(PrintData));
	memset(tempdata, 0,sizeof(tempdata));
	strcpy(PrintData, "ISSUER : ");
	MAPGET(traApplicationLabel, tempdata,lblKO);
	Hex2Text(&PrintData[9],tempdata,0);
	xPrint = GL_Print_Create    (xLayout);
	GL_Widget_SetText      (xPrint, PrintData);
	GL_Widget_SetItem      (xPrint, 0, xline++);
	GL_Widget_SetMargins   (xPrint, 0, 0, 8, 0, GL_UNIT_PIXEL);
	GL_Widget_SetFontScale (xPrint, GL_SCALE_MEDIUM);
	GL_Widget_SetBackAlign (xPrint, GL_ALIGN_LEFT);
	GL_Widget_SetFontStyle (xPrint, GL_FONT_STYLE_BOLD);

	//Card Holder name
	memset(PrintData, 0, sizeof(PrintData));
	memset(tempdata, 0,sizeof(tempdata));
	MAPGET(traCardHolderName, tempdata,lblKO);
	strcpy(PrintData, "");
	Hex2Text(&PrintData[0],tempdata,0);
	if (strlen(PrintData) > 1) {
		xPrint = GL_Print_Create    (xLayout);
		GL_Widget_SetText      (xPrint, PrintData);
		GL_Widget_SetItem      (xPrint, 0, xline++);
		GL_Widget_SetMargins   (xPrint, 0, 0, 8, 0, GL_UNIT_PIXEL);
		GL_Widget_SetFontScale (xPrint, GL_SCALE_MEDIUM);
		GL_Widget_SetBackAlign (xPrint, GL_ALIGN_LEFT);
		//	GL_Widget_SetFontStyle (xPrint, GL_FONT_STYLE_BOLD);
	}

	lblKO:
	return xline;
}

static byte printTransactionDateTime(T_GL_HWIDGET xLayout, byte xline){
	// Local variables
	// ***************
	T_GL_HWIDGET xPrint;
	Telium_Date_t xDate;

	char tcDate[32+1];
	char tcTime[32+1];

	memset(tcDate, 0, sizeof(tcDate));
	memset(tcTime, 0, sizeof(tcTime));

	Telium_Read_date (&xDate);                                                       // Read terminal Date&Time

	////======================== Date and Time =============================
	///
	// Read terminal Date&Time
	Telium_Sprintf (tcDate, "DATE %2.2s/%2.2s/20%2.2s  ",                              // Retrieve date
			xDate.day, xDate.month, xDate.year);
	Telium_Sprintf (tcTime, "  %2.2s:%2.2s",                                           // Retrieve time
			xDate.hour, xDate.minute);


	xPrint = GL_Print_Create    (xLayout);
	GL_Widget_SetText      (xPrint, tcDate);
	GL_Widget_SetItem      (xPrint, 0, xline);
	GL_Widget_SetFontScale (xPrint, GL_SCALE_MEDIUM);
	GL_Widget_SetBackAlign (xPrint, GL_ALIGN_LEFT);
	GL_Widget_SetFontStyle (xPrint, GL_FONT_STYLE_BOLD);

	xPrint = GL_Print_Create    (xLayout);
	GL_Widget_SetText      (xPrint, tcTime);
	GL_Widget_SetItem      (xPrint, 0, xline++);
	GL_Widget_SetFontScale (xPrint, GL_SCALE_MEDIUM);
	GL_Widget_SetBackAlign (xPrint, GL_ALIGN_RIGHT);
	GL_Widget_SetFontStyle (xPrint, GL_FONT_STYLE_BOLD);

	return xline;
}

static byte printTransactionStatus(T_GL_HWIDGET xLayout, byte xline){
	// Local variables
	// ***************
	T_GL_HWIDGET xPrint;
	card mnuItem = 0;
	char PrintData[256 + 1];
	char rspCode[5 + 1];
	int ret = 0;
	card tmpRsp;
	char Temp[100];

	memset(Temp, 0, sizeof(Temp));
	memset(rspCode, 0, sizeof(rspCode));
	memset(PrintData, 0, sizeof(PrintData));

	////======================== TRANSACTION STATUS LINE =============================

	MAPGET(traRspCod, rspCode,lblKO);

	MAPGET(traMnuItm,PrintData,lblKO);
	dec2num(&mnuItem, PrintData, 0);

	memset(PrintData, 0, sizeof(PrintData));



	if (strncmp(rspCode,"00",2) == 0) {                              // APPROVED STATUS
		if (mnuItem == mnuRefund) {
			strcpy(PrintData, "COMPLETED SUCCESSFULLY");
		} else {
			strcpy(PrintData, "APPROVED");
		}
	} else if (strncmp(rspCode,"Y1",2) == 0) {                       // OFFLINE APPROVED
		strcpy(PrintData, "OFFLINE APPROVED");
	} else if (strncmp(rspCode,"Z1",2) == 0) {                       // OFFLINE DECLINED
		strcpy(PrintData, "OFFLINE DECLINED");
	} else {                                      // DECLINED

		if (strlen(rspCode) > 0) {
			MAPGET(traRspCod,rspCode,lblKO);
			if(!fncIsNumeric(rspCode)){
				getResponse_NonNumeric(PrintData,rspCode);
			}else{
				dec2num(&tmpRsp, rspCode, strlen(rspCode));

				getResponse(PrintData, tmpRsp);
			}
		} else {
			strcpy(PrintData, "DECLINED");
		}
	}

	xPrint = GL_Print_Create    (xLayout);
	GL_Widget_SetWrap      (xPrint, TRUE);
	GL_Widget_SetText      (xPrint, PrintData);
	GL_Widget_SetItem      (xPrint, 0, xline++);
	GL_Widget_SetFontScale (xPrint, GL_SCALE_LARGE);
	GL_Widget_SetBackAlign (xPrint, GL_ALIGN_CENTER);
	GL_Widget_SetFontStyle (xPrint, GL_FONT_STYLE_BOLD);


	////===== TRANSACTION REF AND APPROVAL / AUTHORIZATION CODE LINE =======
	//APPROVAL / AUTHORIZATION CODE
	memset(PrintData, 0, sizeof(PrintData));
	strcpy(PrintData, "AUTH: ");
	MAPGET(traAutCod,  &PrintData[6],lblKO);
	if (strlen(PrintData) > 7) {
		xPrint = GL_Print_Create    (xLayout);
		GL_Widget_SetText      (xPrint, PrintData);
		GL_Widget_SetItem      (xPrint, 0, xline++);
		GL_Widget_SetFontScale (xPrint, GL_SCALE_XLARGE);
		GL_Widget_SetBackAlign (xPrint, GL_ALIGN_CENTER);
		GL_Widget_SetFontStyle (xPrint, GL_FONT_STYLE_BOLD);
	}

	//RRN
	memset(PrintData, 0, sizeof(PrintData));
	MAPGET(traRrn, Temp,lblKO);
	if (strlen(Temp) > 1) {
		strcpy(PrintData, "REF No.: ");
		strcat(PrintData, Temp);
		xPrint = GL_Print_Create    (xLayout);
		GL_Widget_SetText      (xPrint, PrintData);
		GL_Widget_SetItem      (xPrint, 0, xline++);
		GL_Widget_SetFontScale (xPrint, GL_SCALE_MEDIUM);
		GL_Widget_SetBackAlign (xPrint, GL_ALIGN_LEFT);
	}


	lblKO:
	return xline;
}


static byte printReceiptHeader(T_GL_HWIDGET xLayout,byte xline){
	// Local variables
	// ***************
	T_GL_HWIDGET xPrint;
	char MENU[lenMnu + 1];
	card mnuitem = 0;
	char PrintData[999 + 1];
	int ret = 0;
	card CardVal;

	memset(MENU, 0, sizeof(MENU));
	memset(PrintData, 0, sizeof(PrintData));

	MAPGET(traMnuItm, MENU, lblKO);
	dec2num(&mnuitem, MENU,0);

	////======================== MID and TID line =============================
	xPrint = GL_Print_Create    (xLayout);
	GL_Widget_SetText      (xPrint, "-------------------------------------------");
	GL_Widget_SetItem      (xPrint, 0, xline++);
	GL_Widget_SetFontScale (xPrint, GL_SCALE_LARGE);
	GL_Widget_SetBackAlign (xPrint, GL_ALIGN_CENTER);

	//TID
	memset(PrintData, 0, sizeof(PrintData));
	strcpy(PrintData, "TID:  ");
	MAPGET(appTID, &PrintData[6],lblKO);
	xPrint = GL_Print_Create    (xLayout);
	GL_Widget_SetText      (xPrint, PrintData);
	GL_Widget_SetItem      (xPrint, 0, xline);
	GL_Widget_SetFontScale (xPrint, GL_SCALE_MEDIUM);
	GL_Widget_SetBackAlign (xPrint, GL_ALIGN_LEFT);

	//MID
	memset(PrintData, 0, sizeof(PrintData));
	strcpy(PrintData, "MID:  ");
	MAPGET(appMID,  &PrintData[6],lblKO);
	xPrint = GL_Print_Create    (xLayout);
	GL_Widget_SetText      (xPrint, PrintData);
	GL_Widget_SetItem      (xPrint, 0, xline++);
	GL_Widget_SetFontScale (xPrint, GL_SCALE_MEDIUM);
	GL_Widget_SetBackAlign (xPrint, GL_ALIGN_RIGHT);

	//INVOICE
	memset(PrintData, 0, sizeof(PrintData));
	strcpy(PrintData, "INVOICE: ");
	if ((mnuitem == mnuDuplicateReceipt) || (mnuitem == mnuReprintReceipt)) {
		MAPGET(traInvNum, &PrintData[9], lblKO);
	} else {
		ret = mapGetCard(appInvNum, CardVal);
		num2dec(&PrintData[9], CardVal, lenInvNum);
	}
	xPrint = GL_Print_Create    (xLayout);
	GL_Widget_SetText      (xPrint, PrintData);
	GL_Widget_SetItem      (xPrint, 0, xline);
	GL_Widget_SetMargins   (xPrint, 0, 0, 8, 0, GL_UNIT_PIXEL);
	GL_Widget_SetFontScale (xPrint, GL_SCALE_MEDIUM);
	GL_Widget_SetBackAlign (xPrint, GL_ALIGN_LEFT);


	//STAN
	memset(PrintData, 0, sizeof(PrintData));
	strcpy(PrintData, "STAN: ");
	if ((mnuitem == mnuDuplicateReceipt) || (mnuitem == mnuReprintReceipt)) {
		MAPGET(traSTAN, &PrintData[6], lblKO);
	} else {
		ret = mapGetCard(appSTAN, CardVal);
		num2dec(&PrintData[6], CardVal, lenSTAN);
	}
	xPrint = GL_Print_Create    (xLayout);
	GL_Widget_SetText      (xPrint, PrintData);
	GL_Widget_SetItem      (xPrint, 0, xline++);
	GL_Widget_SetMargins   (xPrint, 8, 0, 0, 0, GL_UNIT_PIXEL);
	GL_Widget_SetFontScale (xPrint, GL_SCALE_MEDIUM);
	GL_Widget_SetBackAlign (xPrint, GL_ALIGN_RIGHT);


	//BATCH
	memset(PrintData, 0, sizeof(PrintData));
	strcpy(PrintData, "BATCH: ");
	MAPGET(appBatchNumber, &PrintData[7],lblKO);
	xPrint = GL_Print_Create    (xLayout);
	GL_Widget_SetText      (xPrint, PrintData);
	GL_Widget_SetItem      (xPrint, 0, xline++);
	GL_Widget_SetMargins   (xPrint, 8, 0, 0, 0, GL_UNIT_PIXEL);
	GL_Widget_SetFontScale (xPrint, GL_SCALE_MEDIUM);
	GL_Widget_SetBackAlign (xPrint, GL_ALIGN_RIGHT);


	//	//Receipt Number
	//	memset(PrintData, 0, sizeof(PrintData));
	//	strcpy(PrintData, "RECEIPT:  ");
	//	//	MAPGET(appReceiptNumber,  &PrintData[6],lblKO);
	//	xPrint = GL_Print_Create    (xLayout);
	//	GL_Widget_SetText      (xPrint, PrintData);
	//	GL_Widget_SetItem      (xPrint, 0, xline++);
	//	GL_Widget_SetMargins   (xPrint, 0, 0, 8, 0, GL_UNIT_PIXEL);
	//	GL_Widget_SetFontScale (xPrint, GL_SCALE_MEDIUM);
	//	GL_Widget_SetBackAlign (xPrint, GL_ALIGN_RIGHT);
	//-------------------------------------------------------------------------
	xline = printTransactionStatus(xLayout ,xline);
	//-------------------------------------------------------------------------
	xline = printTransactionDateTime(xLayout, xline);
	//-------------------------------------------------------------------------
	xline = printCardDetails(xLayout, xline);
	//-------------------------------------------------------------------------

	////========================================================================
	lblKO:
	return xline;
}


static byte printReceiptFooter(T_GL_HWIDGET xLayout,byte xline){

	// Local variables
	// ***************
	T_GL_HWIDGET xPrint;

	char PrintData[999 + 1];
	int ret = 0;

	////======================== DISCLAIMER FOOTER LINES =============================

	//Served by Line
	memset(PrintData, 0, sizeof(PrintData));
	strcpy(PrintData,"\nYou were served by: ");
	MAPGET(appAppLoggedName,  &PrintData[20],lblKO);
	xPrint = GL_Print_Create    (xLayout);
	GL_Widget_SetText      (xPrint, PrintData);
	GL_Widget_SetItem      (xPrint, 0, xline++);
	GL_Widget_SetFontStyle (xPrint, GL_FONT_STYLE_BOLD);
	GL_Widget_SetFontScale (xPrint, GL_SCALE_SMALL);
	GL_Widget_SetBackAlign (xPrint, GL_ALIGN_LEFT);

	//Footer statement
	memset(PrintData, 0, sizeof(PrintData));
	MAPGET(appFooterText, PrintData,lblKO);
	xPrint = GL_Print_Create    (xLayout);
	GL_Widget_SetText      (xPrint, PrintData);
	GL_Widget_SetItem      (xPrint, 0, xline++);
	GL_Widget_SetFontScale (xPrint, GL_SCALE_SMALL);
	GL_Widget_SetFontStyle (xPrint, GL_FONT_STYLE_ITALIC);
	GL_Widget_SetBackAlign (xPrint, GL_ALIGN_CENTER);
	GL_Widget_SetWrap      (xPrint, TRUE);

	//Footer statement
	memset(PrintData, 0, sizeof(PrintData));
	MAPGET(appAppVersion, PrintData,lblKO);
	xPrint = GL_Print_Create    (xLayout);
	GL_Widget_SetText      (xPrint, PrintData);
	GL_Widget_SetItem      (xPrint, 0, xline++);
	GL_Widget_SetFontScale (xPrint, GL_SCALE_SMALL);
	GL_Widget_SetBackAlign (xPrint, GL_ALIGN_CENTER);
	GL_Widget_SetWrap      (xPrint, TRUE);

	//Footer statement
	xPrint = GL_Print_Create    (xLayout);
	GL_Widget_SetText      (xPrint, "-------------------------------------------\n\n\n\n\n");
	GL_Widget_SetItem      (xPrint, 0, xline++);
	GL_Widget_SetFontScale (xPrint, GL_SCALE_LARGE);
	GL_Widget_SetBackAlign (xPrint, GL_ALIGN_CENTER);
	GL_Widget_SetWrap      (xPrint, FALSE);
	memset(PrintData, 0, sizeof(PrintData));



	////========================================================================
	lblKO:
	return xline;
}


static byte printReceiptMerchant(T_GL_HWIDGET xLayout,byte xline){

	// Local variables
	// ***************
	T_GL_HWIDGET xPrint;

	char PrintData[999 + 1];

	////======================== DISCLAIMER FOOTER LINES =============================


	//Footer statement
	xPrint = GL_Print_Create    (xLayout);
	GL_Widget_SetText      (xPrint, "-----  MERCHANT COPY  -----");
	GL_Widget_SetItem      (xPrint, 0, xline++);
	GL_Widget_SetFontScale (xPrint, GL_SCALE_LARGE);
	GL_Widget_SetBackAlign (xPrint, GL_ALIGN_CENTER);
	GL_Widget_SetWrap      (xPrint, FALSE);
	memset(PrintData, 0, sizeof(PrintData));

	xPrint = GL_Print_Create    (xLayout);
	GL_Widget_SetText      (xPrint, "\n\n\n\n\n");
	GL_Widget_SetItem      (xPrint, 0, xline++);
	GL_Widget_SetFontScale (xPrint, GL_SCALE_SMALL);
	GL_Widget_SetBackAlign (xPrint, GL_ALIGN_CENTER);
	GL_Widget_SetWrap      (xPrint, FALSE);


	////========================================================================
	return xline;
}


static byte printReceiptCustomer(T_GL_HWIDGET xLayout,byte xline){

	// Local variables
	// ***************
	T_GL_HWIDGET xPrint;

	char PrintData[999 + 1];

	////======================== DISCLAIMER FOOTER LINES =============================


	//Footer statement
	xPrint = GL_Print_Create    (xLayout);
	GL_Widget_SetText      (xPrint, "-----  CUSTOMER COPY  -----");
	GL_Widget_SetItem      (xPrint, 0, xline++);
	GL_Widget_SetFontScale (xPrint, GL_SCALE_LARGE);
	GL_Widget_SetBackAlign (xPrint, GL_ALIGN_CENTER);
	GL_Widget_SetWrap      (xPrint, FALSE);
	memset(PrintData, 0, sizeof(PrintData));

	xPrint = GL_Print_Create    (xLayout);
	GL_Widget_SetText      (xPrint, "\n\n\n\n\n");
	GL_Widget_SetItem      (xPrint, 0, xline++);
	GL_Widget_SetFontScale (xPrint, GL_SCALE_SMALL);
	GL_Widget_SetBackAlign (xPrint, GL_ALIGN_CENTER);
	GL_Widget_SetWrap      (xPrint, FALSE);


	////========================================================================

	return xline;
}
static byte printReceiptBody(T_GL_HWIDGET xLayout,byte xline){
	// Local variables
	// ***************
	T_GL_HWIDGET xPrint;
	char exponent[2];
	card exp = 0;
	char MENU[lenMnu + 1];
	char MENUCONTEXT[lenMnu + 1];
	card mnuitem = 0, mnuitemContext = 0;
	card wordTemp = 0;
	char PrintData[999 + 1];
	char temp[lenAmt + 1];
	char UPI_Credit_and_QuasiCredit[3 + 1];
	int ret = 0;
	char rspCode[5 + 1];

	memset(temp, 0, sizeof(temp));
	memset(MENU, 0, sizeof(MENU));
	memset(rspCode, 0, sizeof(rspCode));
	memset(exponent, 0, sizeof(exponent));
	memset(MENUCONTEXT, 0, sizeof(MENUCONTEXT));

	MAPGET(appExp,exponent,lblKO);
	dec2num(&exp, exponent,1);

	MAPGET(traMnuItmContext, MENUCONTEXT, lblKO);
	dec2num(&mnuitemContext, MENUCONTEXT,0);

	MAPGET(traMnuItm, MENU, lblKO);
	dec2num(&mnuitem, MENU,0);

	MAPGET(traRspCod, rspCode,lblKO);

	////======================== TRANSACTION NAME and TID FORMATTED AMOUNT =============================

	//TRANSACTION NAME
	memset(PrintData, 0, sizeof(PrintData));
	MAPGET(traCtx, PrintData,lblKO);
	xPrint = GL_Print_Create    (xLayout);
	GL_Widget_SetText      (xPrint, PrintData);
	GL_Widget_SetItem      (xPrint, 0, xline++);
	GL_Widget_SetFontStyle (xPrint, GL_FONT_STYLE_BOLD);
	GL_Widget_SetFontScale (xPrint, GL_SCALE_MEDIUM);
	GL_Widget_SetBackAlign (xPrint, GL_ALIGN_LEFT);

	if ( (mnuitem == mnuBiller) || (mnuitem == mnuBillerPayment) || (mnuitemContext == mnuBillerPayment) || (mnuitemContext == mnuBiller)){

		//REFERENCE NAME
		mapGetCard(appIsRef_ON, wordTemp);
		if (wordTemp) {
			memset(PrintData, 0, sizeof(PrintData));
			MAPGET(traBillerRef, PrintData,lblKO);
			xPrint = GL_Print_Create    (xLayout);
			GL_Widget_SetText      (xPrint, PrintData);
			GL_Widget_SetItem      (xPrint, 0, xline++);
			GL_Widget_SetFontStyle (xPrint, GL_FONT_STYLE_BOLD);
			GL_Widget_SetFontScale (xPrint, GL_SCALE_MEDIUM);
			GL_Widget_SetBackAlign (xPrint, GL_ALIGN_LEFT);
		}

		//Product code payment details
		memset(PrintData, 0, sizeof(PrintData));
		MAPGET(traBillerPaymentDetails, PrintData,lblKO);
		xPrint = GL_Print_Create    (xLayout);
		GL_Widget_SetText      (xPrint, PrintData);
		GL_Widget_SetItem      (xPrint, 0, xline++);
		GL_Widget_SetFontStyle (xPrint, GL_FONT_STYLE_BOLD);
		GL_Widget_SetFontScale (xPrint, GL_SCALE_MEDIUM);
		GL_Widget_SetBackAlign (xPrint, GL_ALIGN_LEFT);

		//Surcharge payment details
		memset(temp, 0, sizeof(temp));
		memset(PrintData, 0, sizeof(PrintData));
		MAPGET(appBillerSurcharge, temp,lblKO);
		fmtAmt(temp,temp,2,".,");
		strcat(temp, "%%");
		strcpy(PrintData, "Convenience : ");
		strcat(PrintData, temp);
		xPrint = GL_Print_Create    (xLayout);
		GL_Widget_SetText      (xPrint, PrintData);
		GL_Widget_SetItem      (xPrint, 0, xline++);
		GL_Widget_SetFontStyle (xPrint, GL_FONT_STYLE_BOLD);
		GL_Widget_SetFontScale (xPrint, GL_SCALE_MEDIUM);
		GL_Widget_SetBackAlign (xPrint, GL_ALIGN_LEFT);
	}

	//FORMATTED AMOUNT
	if (strncmp(rspCode,"00",2) == 0) { //Print Amount Ony For approved transactions
		memset(temp, 0, sizeof(temp));
		MAPGET(traAmt,  temp,lblKO);
		dec2num(&wordTemp, temp, 0); if(mnuitem == mnuBalanceEnquiry)wordTemp = 1;
		if (wordTemp != 0) {//Also Print only if there is an amount to be printed
			memset(PrintData, 0, sizeof(PrintData));
			MAPGET(appCurrCodeAlpha,  PrintData,lblKO);
			strcat(PrintData, " ");
			fmtAmt(&PrintData[4],temp,exp,".,");
			xPrint = GL_Print_Create    (xLayout);
			GL_Widget_SetText      (xPrint, PrintData);
			GL_Widget_SetItem      (xPrint, 0, xline++);
			GL_Widget_SetFontScale (xPrint, GL_SCALE_XLARGE);
			GL_Widget_SetBackAlign (xPrint, GL_ALIGN_RIGHT);
		}
	}
	//	//Narration
	//	if (mnuitem != mnuBalanceEnquiry) { //Some may not need this narration on balance
	xPrint = GL_Print_Create    (xLayout);
	GL_Widget_SetText      (xPrint, "Please DEBIT from my account");
	GL_Widget_SetItem      (xPrint, 0, xline++);
	GL_Widget_SetFontScale (xPrint, GL_SCALE_MEDIUM);
	GL_Widget_SetBackAlign (xPrint, GL_ALIGN_CENTER);
	GL_Widget_SetFontStyle (xPrint, GL_FONT_STYLE_ITALIC);
	//	}

	//CVM Authentication form
	memset(temp, 0, sizeof(temp));
	memset(PrintData, 0, sizeof(PrintData));
	MAPGET(traCVMused,  temp,lblKO);

	memset(UPI_Credit_and_QuasiCredit, 0, sizeof(UPI_Credit_and_QuasiCredit));
	mapGet(traUPI_Credit_QuasiCredit, UPI_Credit_and_QuasiCredit, mapDatLen(traUPI_Credit_QuasiCredit));


	if (strncmp(temp, "OFL", 3) == 0) {
		strcpy(PrintData, "Offline PIN entered");
	} else if (strncmp(temp, "ONL", 3) == 0) {
		strcpy(PrintData, "Online PIN entered");
	} else if (strncmp(temp, "SIG", 3) == 0) {
		if(strncmp(UPI_Credit_and_QuasiCredit, "1", 1) != 0){
			strcpy(PrintData, "Signature Verification");
		}
	} else if (strncmp(temp, "NON", 3) == 0) {
	} else {
		//		if ((strncmp(UPI_Credit_and_QuasiCredit, "1", 1) != 0) &&
		//				(strncmp(UPI_Credit_and_QuasiCredit, "3", 1) != 0)){
		//			strcpy(PrintData, "Signature Verification");
		//		}
	}
	strcat(PrintData, "\n ");

	xPrint = GL_Print_Create    (xLayout);
	GL_Widget_SetText      (xPrint, PrintData);
	GL_Widget_SetItem      (xPrint, 0, xline++);
	GL_Widget_SetFontScale (xPrint, GL_SCALE_MEDIUM);
	GL_Widget_SetBackAlign (xPrint, GL_ALIGN_CENTER);
	GL_Widget_SetFontStyle (xPrint, GL_FONT_STYLE_ITALIC);

	///========= EmvTags =============
	xline = printEMVdetails(xLayout, xline);

	////========================================================================
	lblKO:
	return xline;
}


int PrintReceipt(void){

	// Local variables
	// ***************
	T_GL_HWIDGET xDocument = NULL;
	T_GL_HWIDGET xLayout;
	char MENU[lenMnu + 1];
	card mnuitem = 0;
	byte fallBack = 0;

	byte xline = 0;
	int ret = 0;

	memset(MENU, 0, sizeof(MENU));
	MAPGETBYTE(appFallback, fallBack, lblKO);
	if (fallBack == 1) {
		ret = 1;
		goto lblEnd;
	}

	MaskPan();

	// Display Printing in progress
	// ********************

	MAPGET(traMnuItm, MENU, lblKO);
	dec2num(&mnuitem, MENU,0);

	if(mnuitem == mnuBalanceEnquiry)
		goto CUSTOMER_COPY;
	if(mnuitem == mnuMiniStatement){
		ret = 1;
		goto lblEnd;
	}

	if(!isApproved())
		goto CUSTOMER_COPY;

	xDocument = GoalCreateDocument(hGoal, GL_ENCODING_UTF8);
	CHECK(xDocument!=NULL, lblKO);                  // Create texts document

	///// =====================  MERCHANT RECEIPT  ========================

	xLayout = GL_Layout_Create(xDocument); //set the document layout

	///&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&   Print Logo    &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
	xline = printLogo(xLayout, xline);

	///&&&&&&&&&&&&&&&&&&&&&&&&&&&&  print Header of Receipt &&&&&&&&&&&&&&&&&&&&&&&&&&&
	xline = printReceiptHeader(xLayout, xline);

	///&&&&&&&&&&&&&&&&&&&&&&&&&&&&  print Body of Receipt &&&&&&&&&&&&&&&&&&&&&&&&&&&&&
	xline = printReceiptBody(xLayout, xline);

	///&&&&&&&&&&&&&&&&&&&&&&&&&&&&  print Footer of Receipt &&&&&&&&&&&&&&&&&&&&&&&&&&&
	xline = printReceiptFooter(xLayout, xline);

	/// *************************************************************************
	xline = printReceiptMerchant(xLayout, xline);

	ret = GoalPrnDocument(xDocument);              // Print Text document
	CHECK(ret >= 0, lblKO);

	GoalDestroyDocument(&xDocument);            // Destroy


	GL_Dialog_Message(hGoal, NULL, "Press Green Button for\nCustomer receipt", GL_ICON_NONE, GL_BUTTON_ALL, 20*1000);

	///// =====================  CUSTOMER RECEIPT  ========================
	CUSTOMER_COPY:

	xDocument = GoalCreateDocument(hGoal, GL_ENCODING_UTF8);
	CHECK(xDocument!=NULL, lblKO);                  // Create texts document

	xLayout = GL_Layout_Create(xDocument); //set the document layout

	///&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&   Print Logo    &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
	xline = printLogo(xLayout, xline);

	///&&&&&&&&&&&&&&&&&&&&&&&&&&&&  print Header of Receipt &&&&&&&&&&&&&&&&&&&&&&&&&&&
	xline = printReceiptHeader(xLayout, xline);

	///&&&&&&&&&&&&&&&&&&&&&&&&&&&&  print Body of Receipt &&&&&&&&&&&&&&&&&&&&&&&&&&&&&
	xline = printReceiptBody(xLayout, xline);

	///&&&&&&&&&&&&&&&&&&&&&&&&&&&&  print Footer of Receipt &&&&&&&&&&&&&&&&&&&&&&&&&&&
	xline = printReceiptFooter(xLayout, xline);

	/// *************************************************************************
	xline = printReceiptCustomer(xLayout, xline);

	ret = GoalPrnDocument(xDocument);              // Print Text document
	CHECK(ret >= 0, lblKO);


	ret = 1;

	goto lblEnd;

	// Errors treatment
	// ****************
	lblKO:                                              // None-classified low level error
	GL_Dialog_Message(hGoal, NULL, "Printing Error", GL_ICON_ERROR, GL_BUTTON_VALID, 3*1000);

	goto lblEnd;
	lblEnd:
	if (xDocument)
		GoalDestroyDocument(&xDocument);            // Destroy 

	return ret;
}

static void prnMapTransactionName(card MenuItem, char *NameOfTxn){
	char transactionName[lenTraName + 1];
	char Statement[128 + 1];

	switch (MenuItem) {
	case mnuSale:
		strcpy(NameOfTxn, "Sale    ");
		break;
	case mnuSaleCB:
		strcpy(NameOfTxn, "Sale CB ");
		break;
	case mnuBalanceEnquiry:
		strcpy(NameOfTxn, "Bal Enq ");
		break;
	case mnuPreaut:
		strcpy(NameOfTxn, "Pre Aut ");
		break;
	case mnuCompletion:
		strcpy(NameOfTxn, "Copmletn");
		break;
	default:
		memset(Statement, 0, sizeof(Statement));
		memset(transactionName, 0, sizeof(transactionName));
		Telium_Sprintf (Statement, "SELECT MenuName FROM AppMenus WHERE MenuId = '%d';", MenuItem);
		Sqlite_Run_Statement_MultiRecord(Statement, transactionName);
		strncpy(NameOfTxn, transactionName, 8);
		break;
	}

}

int PrintDetailedLog(void){
	// Local variables
	// ***************
	T_GL_HWIDGET xDocument = NULL;
	T_GL_HWIDGET xLayout;
	T_GL_HWIDGET xPrint;
	char PrintData[256 + 1];
	byte xline = 0;
	int ret = 0;
	int var = 0, rec = 0, var2 = 0;
	char Statement[512];
	char DataResponse[10240];
	int NumOfRecords = 0;
	int NumberOfColumns = 0;
	char *Raw_Columns;
	char *Columns_Array[40];
	char *EntryData;
	char *EntryArray[40];
	char Single_Column_Buffer[256];
	char Column_Name[32];
	char Column_Data[32];
	///----------------
	char amount[20 + 1];
	char date[16 + 1];
	char aprv[8 + 1];
	char CardNumber[lenPan + 1];
	char transactionName[100];
	card mnuItm = 0;
	char CurrencyAlpha[64 + 1];
	char CurrencyNumeric[64 + 1];
	char Dr[lenAmt + 1], Cr[lenAmt + 1];
	char DrCount[lenAmt + 1], CrCount[lenAmt + 1];
	char DrRev[lenAmt + 1], CrRev[lenAmt + 1];
	char DrRevCount[lenAmt + 1], CrRevCount[lenAmt + 1];
	char Totals[lenAmt + 1];
	char tcDate[32+1];
	Telium_Date_t xDate;

	memset(tcDate, 0, sizeof(tcDate));
	memset(Dr, 0, sizeof(Dr));
	memset(Cr, 0, sizeof(Cr));
	memset(DrRev, 0, sizeof(DrRev));
	memset(CrRev, 0, sizeof(CrRev));
	memset(Totals, 0, sizeof(Totals));
	memset(CardNumber, 0, sizeof(CardNumber));
	memset(CurrencyAlpha, 0, sizeof(CurrencyAlpha));
	memset(CurrencyNumeric, 0, sizeof(CurrencyNumeric));
	memset(DataResponse, 0, sizeof(DataResponse));
	memset(Statement, 0, sizeof(Statement));
	memset(transactionName, 0, sizeof(transactionName));
	memset(amount, 0, sizeof(amount));
	memset(date, 0, sizeof(date));
	memset(aprv, 0, sizeof(aprv));

	OpenPeripherals();

	Telium_Read_date (&xDate);                                                       // Read terminal Date&Time

	// Format date Date&Time
	Telium_Sprintf (tcDate, "DATE: %2.2s/%2.2s/20%2.2s", xDate.day, xDate.month, xDate.year);

	// Display Preparing printing data
	// ********************
	GL_Dialog_Message(hGoal, NULL, "Please Wait""\n""Preparing printing data", GL_ICON_INFORMATION, GL_BUTTON_NONE, 0);

	xDocument = GoalCreateDocument(hGoal, GL_ENCODING_UTF8);
	CHECK(xDocument!=NULL, lblKO);                  // Create texts document

	xLayout = GL_Layout_Create(xDocument); //set the document layout

	///&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&   Print Logo    &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
	xline = printLogo(xLayout, xline);

	xPrint = GL_Print_Create    (xLayout);
	GL_Widget_SetText      (xPrint, "\nDETAILED REPORT\n");
	GL_Widget_SetItem      (xPrint, 0, xline++);
	GL_Widget_SetMargins   (xPrint, 1, 1, 1, 1, GL_UNIT_PIXEL);
	GL_Widget_SetFontScale (xPrint, GL_SCALE_MEDIUM);
	GL_Widget_SetBackAlign (xPrint, GL_ALIGN_LEFT);


	strcpy(CurrencyNumeric, "020");

	///Get Currency data 1
	MAPGET(appCurrCodeAlpha1, CurrencyAlpha, lblKO);
	MAPGET(appCurrCodeNumerc1, &CurrencyNumeric[3], lblKO);

	for (var2 = 0; var2 < 2; var2++) { ///Loop to Print the Currencies different in the terminal

		logCalcTot(CurrencyNumeric, Dr,Cr,DrRev,CrRev,DrCount,CrCount,DrRevCount,CrRevCount,Totals);

		xPrint = GL_Print_Create    (xLayout);
		GL_Widget_SetText      (xPrint, tcDate);
		GL_Widget_SetItem      (xPrint, 0, xline);
		GL_Widget_SetMargins   (xPrint, 1, 1, 1, 1, GL_UNIT_PIXEL);
		GL_Widget_SetFontScale (xPrint, GL_SCALE_MEDIUM);
		GL_Widget_SetBackAlign (xPrint, GL_ALIGN_LEFT);

		xPrint = GL_Print_Create    (xLayout);
		GL_Widget_SetText      (xPrint, CurrencyAlpha);
		GL_Widget_SetItem      (xPrint, 0, xline++);
		GL_Widget_SetMargins   (xPrint, 1, 1, 1, 1, GL_UNIT_PIXEL);
		GL_Widget_SetFontScale (xPrint, GL_SCALE_SMALL);
		GL_Widget_SetFontStyle (xPrint, GL_FONT_STYLE_BOLD);
		GL_Widget_SetBackAlign (xPrint, GL_ALIGN_RIGHT);

		///---- format headers ----------
		xPrint = GL_Print_Create    (xLayout);
		GL_Widget_SetText      (xPrint, "TIME     TRANSACTION     APPROVAL           AMOUNT");
		GL_Widget_SetItem      (xPrint, 0, xline++);
		GL_Widget_SetMargins   (xPrint, 1, 1, 1, 1, GL_UNIT_PIXEL);
		GL_Widget_SetFontScale (xPrint, GL_SCALE_SMALL);
		GL_Widget_SetBackAlign (xPrint, GL_ALIGN_LEFT);

		xPrint = GL_Print_Create    (xLayout);
		GL_Widget_SetText      (xPrint, "-------------------------------------------------------------------------");
		GL_Widget_SetItem      (xPrint, 0, xline++);
		GL_Widget_SetMargins   (xPrint, 1, 1, 1, 1, GL_UNIT_PIXEL);
		GL_Widget_SetFontScale (xPrint, GL_SCALE_SMALL);
		GL_Widget_SetBackAlign (xPrint, GL_ALIGN_CENTER);

		///&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

		///get number of records
		strcpy(Statement,"SELECT COUNT(*) FROM log;");
		ret = Sqlite_Run_Statement(Statement,DataResponse);
		CHECK(ret > 0,lblKO);
		NumOfRecords = atoi(DataResponse) + 1;

		for (var = 1; var < NumOfRecords; var++) {
			memset(Statement, 0, sizeof(Statement));

			///------ get record by order of entry -------
			Telium_Sprintf(Statement,"SELECT isoField002, isoField004, isoField007, isoField038, MenuItem FROM log WHERE isoField039 = '00' AND id = '%d' AND isoField049 = '%s' AND MenuItem != '%d';", var, CurrencyNumeric, mnuBalanceEnquiry);
			ret = Sqlite_Run_Statement_MultiRecord(Statement, DataResponse);

			/////------- check if its a valid record ------------
			if (strlen(DataResponse) > 9) {
				//Clear Data for printing Buffer readiness
				memset(transactionName, 0, sizeof(transactionName));
				memset(amount, 0, sizeof(amount));
				memset(date, 0, sizeof(date));
				memset(aprv, 0, sizeof(aprv));

				NumberOfColumns = count_chars(DataResponse,';');

				//// ----------- split the columns into an array ---------------
				Raw_Columns = strtok(DataResponse,";");//column separator
				Columns_Array[0] = Raw_Columns;

				for(rec = 1;rec < NumberOfColumns; rec++){
					if(Raw_Columns == NULL)break;

					Raw_Columns = strtok(NULL,";");
					Columns_Array[rec] = Raw_Columns;
				}

				//look for the column data
				for(rec = 0;rec < NumberOfColumns; rec++){
					memset(Single_Column_Buffer, 0, sizeof(Single_Column_Buffer));
					memset(Column_Name, 0, sizeof(Column_Name));
					memset(Column_Data, 0, sizeof(Column_Data));
					strcpy(Single_Column_Buffer, Columns_Array[rec]);

					EntryData = strtok(Single_Column_Buffer,",");//column data separator
					EntryArray[0] = EntryData;
					strcpy(Column_Name, EntryArray[0]);

					EntryData = strtok(NULL,",");
					if (EntryData != NULL) {
						EntryArray[1] = EntryData;
						strcpy(Column_Data, EntryArray[1]);
					}

					if (strncmp(Column_Name,"isoField004",11) == 0) {////amount
						fmtAmt(&amount[0],Column_Data,2,".,");
					} else if (strncmp(Column_Name,"isoField007",11) == 0) {////transaction date - CCYYMMDDhhmmss
						strncpy(date,&Column_Data[8],2); strcat(date,":");
						strncpy(&date[3],&Column_Data[10],2);
					} else if (strncmp(Column_Name,"isoField038",11) == 0) {
						strcpy(aprv,Column_Data);
					} else if (strncmp(Column_Name,"isoField002",11) == 0) {
						MaskPan_Other(Column_Data, CardNumber);
					} else if (strncmp(Column_Name,"MenuItem",8) == 0) {
						memset(transactionName, 0, sizeof(transactionName));
						memset(Statement, 0, sizeof(Statement));
						dec2num(&mnuItm, Column_Data,0);
						prnMapTransactionName(mnuItm, transactionName);
					}
				}

				//date and transaction name
				memset(PrintData, 0, sizeof(PrintData));
				strcpy(PrintData, date);               strcat(PrintData,"     ");
				strncat(PrintData,transactionName,8);
				xPrint = GL_Print_Create    (xLayout);
				GL_Widget_SetText      (xPrint, PrintData);
				GL_Widget_SetItem      (xPrint, 0, xline);
				GL_Widget_SetMargins   (xPrint, 0, 0, 0, 0, GL_UNIT_PIXEL);
				GL_Widget_SetFontScale (xPrint, GL_SCALE_SMALL);
				GL_Widget_SetBackAlign (xPrint, GL_ALIGN_LEFT);

				//				//card number masked
				//				xPrint = GL_Print_Create    (xLayout);
				//				GL_Widget_SetText      (xPrint, CardNumber);
				//				GL_Widget_SetItem      (xPrint, 0, xline++);
				//				GL_Widget_SetMargins   (xPrint, 0, 0, 3, 0, GL_UNIT_PIXEL);
				//				GL_Widget_SetFontScale (xPrint, GL_SCALE_SMALL);
				//				GL_Widget_SetBackAlign (xPrint, GL_ALIGN_RIGHT);


				memset(PrintData, 0, sizeof(PrintData));
				strcpy(PrintData,aprv);
				xPrint = GL_Print_Create    (xLayout);
				GL_Widget_SetText      (xPrint, PrintData);
				GL_Widget_SetItem      (xPrint, 0, xline);
				GL_Widget_SetMargins   (xPrint, 39, 0, 0, 0, GL_UNIT_PIXEL);
				GL_Widget_SetFontScale (xPrint, GL_SCALE_SMALL);
				GL_Widget_SetBackAlign (xPrint, GL_ALIGN_CENTER);


				memset(PrintData, 0, sizeof(PrintData));
				strcpy(PrintData,amount);
				xPrint = GL_Print_Create    (xLayout);
				GL_Widget_SetText      (xPrint, PrintData);
				GL_Widget_SetItem      (xPrint, 0, xline++);
				GL_Widget_SetMargins   (xPrint, 0, 0, 3, 0, GL_UNIT_PIXEL);
				GL_Widget_SetFontScale (xPrint, GL_SCALE_SMALL);
				GL_Widget_SetBackAlign (xPrint, GL_ALIGN_RIGHT);
			}
			memset(CardNumber, 0, sizeof(CardNumber));
		}

		xPrint = GL_Print_Create    (xLayout);
		GL_Widget_SetText      (xPrint, "-------------------------------------------");
		GL_Widget_SetItem      (xPrint, 0, xline++);
		GL_Widget_SetFontScale (xPrint, GL_SCALE_SMALL);
		GL_Widget_SetBackAlign (xPrint, GL_ALIGN_CENTER);

		xPrint = GL_Print_Create    (xLayout);
		GL_Widget_SetText      (xPrint, "TOTALS");
		GL_Widget_SetItem      (xPrint, 0, xline);
		GL_Widget_SetFontScale (xPrint, GL_SCALE_SMALL);
		GL_Widget_SetFontStyle (xPrint, GL_FONT_STYLE_BOLD);
		GL_Widget_SetBackAlign (xPrint, GL_ALIGN_LEFT);


		memset(amount, 0, sizeof(amount));
		strcpy(amount, CurrencyAlpha); strcat(amount," ");
		fmtAmt(&amount[4],Totals,2,".,");

		xPrint = GL_Print_Create    (xLayout);
		GL_Widget_SetText      (xPrint, amount);
		GL_Widget_SetItem      (xPrint, 0, xline++);
		GL_Widget_SetMargins   (xPrint, 0, 0, 3, 0, GL_UNIT_PIXEL);
		GL_Widget_SetFontScale (xPrint, GL_SCALE_SMALL);
		GL_Widget_SetFontStyle (xPrint, GL_FONT_STYLE_BOLD);
		GL_Widget_SetBackAlign (xPrint, GL_ALIGN_RIGHT);

		xPrint = GL_Print_Create    (xLayout);
		GL_Widget_SetText      (xPrint, "===========================================\n\n");
		GL_Widget_SetItem      (xPrint, 0, xline++);
		GL_Widget_SetFontScale (xPrint, GL_SCALE_SMALL);
		GL_Widget_SetBackAlign (xPrint, GL_ALIGN_CENTER);


		///Get Currency data 1
		memset(CurrencyAlpha, 0, sizeof(CurrencyAlpha));
		memset(CurrencyNumeric, 0, sizeof(CurrencyNumeric));
		strcpy(CurrencyNumeric, "020");
		MAPGET(appCurrCodeAlpha2, CurrencyAlpha, lblKO);
		MAPGET(appCurrCodeNumerc2, &CurrencyNumeric[3], lblKO);

	} //End Of "for (var2 = 0; var2 < 2; var2++) {" for Currency Looping

	///&&&&&&&&&&&&&&&&&&&&&&&&&&&&  print Footer of Receipt &&&&&&&&&&&&&&&&&&&&&&&&&&&
	xline = printReceiptFooter(xLayout, xline);

	// Display Printing in progress
	// ********************
	GL_Dialog_Message(hGoal, NULL, "Please Wait""\n""Printing In Progress", GL_ICON_INFORMATION, GL_BUTTON_NONE, 0);

	ret = GoalPrnDocument(xDocument);              // Print Text document
	CHECK(ret >= 0, lblKO);

	ret = 1;
	goto lblEnd;

	// Errors treatment
	// ****************
	lblKO:                                              // None-classified low level error
	//GL_Dialog_Message(hGoal, NULL, "Printing Error", GL_ICON_ERROR, GL_BUTTON_VALID, 5*1000);

	goto lblEnd;
	lblEnd:
	if (xDocument)
		GoalDestroyDocument(&xDocument);            // Destroy

	ClosePeripherals();
	return ret;
}

int PrintReprintReceipt(void){
	// Local variables
	// ***************
	T_GL_HWIDGET xDocument = NULL;
	T_GL_HWIDGET xLayout;
	char MENU[lenMnu + 1];
	card mnuitem = 0;

	byte xline = 0;
	int ret = 0;

	memset(MENU, 0, sizeof(MENU));

	//Biller Account
	ret = Application_Request_Data("Enter STAN# :", traSTAN, 13, "/d");
	CHECK(ret>0, lblKO);

	// Display Preparing printing data
	// ********************
	GL_Dialog_Message(hGoal, NULL, "Please Wait""\n""Preparing printing data", GL_ICON_INFORMATION, GL_BUTTON_NONE, 0);

	ret = sqlite_Get_LOG_Record(0, 0, traSTAN);
	CHECK(ret>0, lblNorecord);

	MaskPan();

	// Display Printing in progress
	// ********************

	MAPGET(traMnuItm, MENU, lblKO);
	dec2num(&mnuitem, MENU,0);

	// Display Preparing printing data
	// ********************
	GL_Dialog_Message(hGoal, NULL, "Please Wait""\n"" printing...", GL_ICON_INFORMATION, GL_BUTTON_NONE, 0);

	xDocument = GoalCreateDocument(hGoal, GL_ENCODING_UTF8);
	CHECK(xDocument!=NULL, lblKO);                  // Create texts document

	xLayout = GL_Layout_Create(xDocument); //set the document layout

	///&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&   Print Logo    &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
	xline = printLogo(xLayout, xline);

	///&&&&&&&&&&&&&&&&&&&&&&&&&&&&  print Header of Receipt &&&&&&&&&&&&&&&&&&&&&&&&&&&
	xline = printReceiptHeader(xLayout, xline);

	///&&&&&&&&&&&&&&&&&&&&&&&&&&&&  print Body of Receipt &&&&&&&&&&&&&&&&&&&&&&&&&&&&&
	xline = printReceiptBody(xLayout, xline);

	///&&&&&&&&&&&&&&&&&&&&&&&&&&&&  print Footer of Receipt &&&&&&&&&&&&&&&&&&&&&&&&&&&
	xline = printReceiptFooter(xLayout, xline);

	/// *************************************************************************
	xline = printReceiptCustomer(xLayout, xline);

	ret = GoalPrnDocument(xDocument);              // Print Text document
	CHECK(ret >= 0, lblKO);


	ret = 1;

	goto lblEnd;

	// Errors treatment
	// ****************
	lblKO:                                              // None-classified low level error
	GL_Dialog_Message(hGoal, NULL, "Printing Error", GL_ICON_ERROR, GL_BUTTON_VALID, 3*1000);

	goto lblEnd;
	lblEnd:
	if (xDocument)
		GoalDestroyDocument(&xDocument);            // Destroy

	lblNorecord:
	_clrscr();
	return ret;
}


int PrintDuplicateReceipt(void){
	// Local variables
	// ***************
	T_GL_HWIDGET xDocument = NULL;
	T_GL_HWIDGET xLayout;
	char MENU[lenMnu + 1];
	card mnuitem = 0;

	byte xline = 0;
	int ret = 0;

	memset(MENU, 0, sizeof(MENU));

	ret = sqlite_Get_LOG_Record(0, 0, 0);
	CHECK(ret>0, lblNorecord);

	MaskPan();

	// Display Printing in progress
	// ********************

	MAPGET(traMnuItm, MENU, lblKO);
	dec2num(&mnuitem, MENU,0);


	xDocument = GoalCreateDocument(hGoal, GL_ENCODING_UTF8);
	CHECK(xDocument!=NULL, lblKO);                  // Create texts document

	xLayout = GL_Layout_Create(xDocument); //set the document layout

	///&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&   Print Logo    &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
	xline = printLogo(xLayout, xline);

	///&&&&&&&&&&&&&&&&&&&&&&&&&&&&  print Header of Receipt &&&&&&&&&&&&&&&&&&&&&&&&&&&
	xline = printReceiptHeader(xLayout, xline);

	///&&&&&&&&&&&&&&&&&&&&&&&&&&&&  print Body of Receipt &&&&&&&&&&&&&&&&&&&&&&&&&&&&&
	xline = printReceiptBody(xLayout, xline);

	///&&&&&&&&&&&&&&&&&&&&&&&&&&&&  print Footer of Receipt &&&&&&&&&&&&&&&&&&&&&&&&&&&
	xline = printReceiptFooter(xLayout, xline);

	/// *************************************************************************
	xline = printReceiptCustomer(xLayout, xline);

	ret = GoalPrnDocument(xDocument);              // Print Text document
	CHECK(ret >= 0, lblKO);


	ret = 1;

	goto lblEnd;

	// Errors treatment
	// ****************
	lblKO:                                              // None-classified low level error
	GL_Dialog_Message(hGoal, NULL, "Printing Error", GL_ICON_ERROR, GL_BUTTON_VALID, 3*1000);

	goto lblEnd;
	lblEnd:
	if (xDocument)
		GoalDestroyDocument(&xDocument);            // Destroy

	lblNorecord:
	return ret;
}


int PrintSummaryLog(void){
	// Local variables
	// ***************
	T_GL_HWIDGET xDocument = NULL;
	T_GL_HWIDGET xLayout;
	T_GL_HWIDGET xPrint;
	char PrintData[25 + 1];

	byte xline = 0;
	int ret = 0;

	char menuWordArray[100][100];
	char *Raw_Menus;
	card CurrentMenuSelected = 0;
	char CurrentMenuStrSelected[4 + 1];
	char CurrentMenuCount[4 + 1];
	char MenuName[20 + 1];
	int var = 0, var2 = 0;
	char Statement[256];
	char DataResponse[256];
	int NumOfRecords = 0;
	///----------------
	char GroupTotals[20 + 1];
	char Dr[lenAmt + 1], Cr[lenAmt + 1];
	char DrCount[lenAmt + 1], CrCount[lenAmt + 1];
	char DrRev[lenAmt + 1], CrRev[lenAmt + 1];
	char DrRevCount[lenAmt + 1], CrRevCount[lenAmt + 1];
	char Totals[lenAmt + 1];
	char CurrencyAlpha[6 + 1];
	char CurrencyNumeric[6 + 1];

	memset(Dr, 0, sizeof(Dr));
	memset(Cr, 0, sizeof(Cr));
	memset(DrRev, 0, sizeof(DrRev));
	memset(CrRev, 0, sizeof(CrRev));
	memset(Totals, 0, sizeof(Totals));
	memset(CurrencyAlpha, 0, sizeof(CurrencyAlpha));
	memset(CurrencyNumeric, 0, sizeof(CurrencyNumeric));

	OpenPeripherals();

	// Display Preparing printing data
	// ********************
	GL_Dialog_Message(hGoal, NULL, "Please Wait""\n""Preparing printing data", GL_ICON_INFORMATION, GL_BUTTON_NONE, 0);


	xDocument = GoalCreateDocument(hGoal, GL_ENCODING_UTF8);
	CHECK(xDocument!=NULL, lblKO);                  // Create texts document

	xLayout = GL_Layout_Create(xDocument); //set the document layout
	memset(DataResponse, 0, sizeof(DataResponse));

	///&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&   Print Logo    &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
	xline = printLogo(xLayout, xline);


	xPrint = GL_Print_Create    (xLayout);
	GL_Widget_SetText      (xPrint, "\nSUMMARY REPORT");
	GL_Widget_SetItem      (xPrint, 0, xline++);
	GL_Widget_SetMargins   (xPrint, 1, 1, 1, 1, GL_UNIT_PIXEL);
	GL_Widget_SetFontScale (xPrint, GL_SCALE_MEDIUM);
	GL_Widget_SetBackAlign (xPrint, GL_ALIGN_LEFT);

	xPrint = GL_Print_Create    (xLayout);
	GL_Widget_SetText      (xPrint, "--------------------------------------------------------------------------");
	GL_Widget_SetItem      (xPrint, 0, xline++);
	GL_Widget_SetMargins   (xPrint, 0, 0, 0, 0, GL_UNIT_PIXEL);
	GL_Widget_SetFontScale (xPrint, GL_SCALE_SMALL);
	GL_Widget_SetBackAlign (xPrint, GL_ALIGN_LEFT);

	strcpy(CurrencyNumeric, "020");

	///Get Currency data 1
	MAPGET(appCurrCodeAlpha1, CurrencyAlpha, lblKO);
	MAPGET(appCurrCodeNumerc1, &CurrencyNumeric[3], lblKO);


	for (var2 = 0; var2 < 2; var2++) { ///Loop to Print the Currncies different in the terminal
		//Transaction totals by Currency
		logCalcTot(CurrencyNumeric, Dr,Cr,DrRev,CrRev,DrCount,CrCount,DrRevCount,CrRevCount,Totals);

		//Do Printing other things now
		xPrint = GL_Print_Create    (xLayout);
		memset(Statement, 0, sizeof(Statement));
		Telium_Sprintf(Statement, "TRANS NAME     TOTAL TRANS    TOTAL AMT(%s)", CurrencyAlpha);
		GL_Widget_SetText      (xPrint, Statement);
		GL_Widget_SetItem      (xPrint, 0, xline++);
		GL_Widget_SetMargins   (xPrint, 0, 0, 0, 0, GL_UNIT_PIXEL);
		GL_Widget_SetFontScale (xPrint, GL_SCALE_SMALL);
		GL_Widget_SetBackAlign (xPrint, GL_ALIGN_CENTER);


		xPrint = GL_Print_Create    (xLayout);
		GL_Widget_SetText      (xPrint, "--------------------------------------------------------------------------");
		GL_Widget_SetItem      (xPrint, 0, xline++);
		GL_Widget_SetMargins   (xPrint, 0, 0, 0, 0, GL_UNIT_PIXEL);
		GL_Widget_SetFontScale (xPrint, GL_SCALE_SMALL);
		GL_Widget_SetBackAlign (xPrint, GL_ALIGN_LEFT);


		///&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

		///get number of records
		logGetUniqueTransactionRecordMenus(DataResponse);
		NumOfRecords = count_chars(DataResponse,'#') + 1;

		if(strlen(DataResponse)<1)
			NumOfRecords = 0;

		memset(menuWordArray[0], 0, sizeof(menuWordArray[0]));
		Raw_Menus = strtok(DataResponse,"#");//column separator

		if(Raw_Menus != NULL)
			strcpy(menuWordArray[0], Raw_Menus);

		for (var = 1; var < NumOfRecords; var++) {
			memset(menuWordArray[var], 0, sizeof(menuWordArray[var]));
			Raw_Menus = strtok(NULL,"#");//column separator
			strcpy(menuWordArray[var], Raw_Menus);
		}
		memset(menuWordArray[var], 0, sizeof(menuWordArray[var]));

		//=================== start looping for transactions ======================
		for (var = 0; var < NumOfRecords; var++) {
			memset(MenuName, 0, sizeof(MenuName));
			memset(GroupTotals, 0, sizeof(GroupTotals));
			memset(CurrentMenuCount, 0, sizeof(CurrentMenuCount));

			memset(CurrentMenuStrSelected, 0, sizeof(CurrentMenuStrSelected));
			strcpy(CurrentMenuStrSelected, menuWordArray[var]);
			dec2num(&CurrentMenuSelected, menuWordArray[var], 0);

			if (CurrentMenuSelected == mnuBalanceEnquiry) {
				continue;
			}


			///------ get name of selected first group of transactions(Menu) -------
			memset(DataResponse, 0, sizeof(DataResponse));
			memset(Statement, 0, sizeof(Statement));
			Telium_Sprintf(Statement, "SELECT MenuName FROM AppMenus WHERE MenuId = '%d';", CurrentMenuSelected);
			ret = Sqlite_Run_Statement_MultiRecord(Statement,DataResponse);
			strncpy(MenuName, DataResponse, 20);

			xPrint = GL_Print_Create    (xLayout);
			GL_Widget_SetText      (xPrint, MenuName);
			GL_Widget_SetItem      (xPrint, 0, xline);
			GL_Widget_SetMargins   (xPrint, 0, 0, 0, 0, GL_UNIT_PIXEL);
			GL_Widget_SetFontScale (xPrint, GL_SCALE_SMALL);
			GL_Widget_SetBackAlign (xPrint, GL_ALIGN_LEFT);
			GL_Widget_SetWrap(xPrint, FALSE);



			///------ get Count of selected group of transactions(Menu) -------
			memset(DataResponse, 0, sizeof(DataResponse));
			memset(Statement, 0, sizeof(Statement));
			Telium_Sprintf(Statement,"SELECT count(*) FROM log WHERE isoField039 = '00' AND MenuItem = '%s' AND isoField049 = '%s';",CurrentMenuStrSelected, CurrencyNumeric);
			ret = Sqlite_Run_Statement_MultiRecord(Statement,DataResponse);
			strncpy(CurrentMenuCount, DataResponse, strlen(DataResponse));

			xPrint = GL_Print_Create    (xLayout);
			GL_Widget_SetText      (xPrint, CurrentMenuCount);
			GL_Widget_SetItem      (xPrint, 0, xline);
			GL_Widget_SetMargins   (xPrint, 20, 0, 0, 0, GL_UNIT_PIXEL);
			GL_Widget_SetFontScale (xPrint, GL_SCALE_SMALL);
			GL_Widget_SetBackAlign (xPrint, GL_ALIGN_CENTER);
			GL_Widget_SetWrap(xPrint, FALSE);

			///------ get amount of selected group of transactions(Menu) -------
			memset(DataResponse, 0, sizeof(DataResponse));
			memset(Statement, 0, sizeof(Statement));
			Telium_Sprintf(Statement,"SELECT SUM(isoField004) FROM log WHERE isoField039 = '00' AND MenuItem = '%d' AND isoField049 = '%s';",CurrentMenuSelected, CurrencyNumeric);
			ret = Sqlite_Run_Statement_MultiRecord(Statement,DataResponse);

			fmtAmt(GroupTotals,DataResponse,2,".,");
			//		fmtPad(GroupTotals,18,' ');

			memset(PrintData, 0, sizeof(PrintData));
			strcpy(PrintData,GroupTotals);
			xPrint = GL_Print_Create    (xLayout);
			GL_Widget_SetText      (xPrint, PrintData);
			GL_Widget_SetItem      (xPrint, 0, xline++);
			GL_Widget_SetMargins   (xPrint, 0, 0, 10, 0, GL_UNIT_PIXEL);
			GL_Widget_SetFontScale (xPrint, GL_SCALE_SMALL);
			GL_Widget_SetBackAlign (xPrint, GL_ALIGN_RIGHT);
			GL_Widget_SetWrap(xPrint, FALSE);
		}

		xPrint = GL_Print_Create    (xLayout);
		GL_Widget_SetText      (xPrint, "-------------------------------------------");
		GL_Widget_SetItem      (xPrint, 0, xline++);
		GL_Widget_SetMargins   (xPrint, 0, 0, 0, 0, GL_UNIT_PIXEL);
		GL_Widget_SetFontScale (xPrint, GL_SCALE_SMALL);
		GL_Widget_SetBackAlign (xPrint, GL_ALIGN_CENTER);

		//---------- Debits ------------
		xPrint = GL_Print_Create    (xLayout);
		GL_Widget_SetText      (xPrint, "DEBITS");
		GL_Widget_SetItem      (xPrint, 0, xline);
		GL_Widget_SetMargins   (xPrint, 0, 0, 0, 0, GL_UNIT_PIXEL);
		GL_Widget_SetFontScale (xPrint, GL_SCALE_SMALL);
		GL_Widget_SetBackAlign (xPrint, GL_ALIGN_LEFT);

		xPrint = GL_Print_Create    (xLayout);
		GL_Widget_SetText      (xPrint, DrCount);
		GL_Widget_SetItem      (xPrint, 0, xline);
		GL_Widget_SetMargins   (xPrint, 200, 0, 0, 0, GL_UNIT_PIXEL);
		GL_Widget_SetFontScale (xPrint, GL_SCALE_SMALL);
		GL_Widget_SetBackAlign (xPrint, GL_ALIGN_LEFT);

		fmtAmt(Dr,Dr,2,".,");
		xPrint = GL_Print_Create    (xLayout);
		GL_Widget_SetText      (xPrint, Dr);
		GL_Widget_SetItem      (xPrint, 0, xline++);
		GL_Widget_SetMargins   (xPrint, 0, 0, 10, 0, GL_UNIT_PIXEL);
		GL_Widget_SetFontScale (xPrint, GL_SCALE_SMALL);
		GL_Widget_SetBackAlign (xPrint, GL_ALIGN_RIGHT);

		//---------- Debit Reversals ------------
		xPrint = GL_Print_Create    (xLayout);
		GL_Widget_SetText      (xPrint, "DEBIT REVERSALS");
		GL_Widget_SetItem      (xPrint, 0, xline);
		GL_Widget_SetMargins   (xPrint, 0, 0, 0, 0, GL_UNIT_PIXEL);
		GL_Widget_SetFontScale (xPrint, GL_SCALE_SMALL);
		GL_Widget_SetBackAlign (xPrint, GL_ALIGN_LEFT);

		xPrint = GL_Print_Create    (xLayout);
		GL_Widget_SetText      (xPrint, DrRevCount);
		GL_Widget_SetItem      (xPrint, 0, xline);
		GL_Widget_SetMargins   (xPrint, 200, 0, 0, 0, GL_UNIT_PIXEL);
		GL_Widget_SetFontScale (xPrint, GL_SCALE_SMALL);
		GL_Widget_SetBackAlign (xPrint, GL_ALIGN_LEFT);

		fmtAmt(DrRev,DrRev,2,".,");
		xPrint = GL_Print_Create    (xLayout);
		GL_Widget_SetText      (xPrint, DrRev);
		GL_Widget_SetItem      (xPrint, 0, xline++);
		GL_Widget_SetMargins   (xPrint, 0, 0, 10, 0, GL_UNIT_PIXEL);
		GL_Widget_SetFontScale (xPrint, GL_SCALE_SMALL);
		GL_Widget_SetBackAlign (xPrint, GL_ALIGN_RIGHT);

		//---------- Credits ------------
		xPrint = GL_Print_Create    (xLayout);
		GL_Widget_SetText      (xPrint, "CREDITS");
		GL_Widget_SetItem      (xPrint, 0, xline);
		GL_Widget_SetMargins   (xPrint, 0, 0, 0, 0, GL_UNIT_PIXEL);
		GL_Widget_SetFontScale (xPrint, GL_SCALE_SMALL);
		GL_Widget_SetBackAlign (xPrint, GL_ALIGN_LEFT);

		memset(PrintData, 0, sizeof(PrintData));
		xPrint = GL_Print_Create    (xLayout);
		GL_Widget_SetText      (xPrint, CrCount);
		GL_Widget_SetItem      (xPrint, 0, xline);
		GL_Widget_SetMargins   (xPrint, 200, 0, 0, 0, GL_UNIT_PIXEL);
		GL_Widget_SetFontScale (xPrint, GL_SCALE_SMALL);
		GL_Widget_SetBackAlign (xPrint, GL_ALIGN_LEFT);

		fmtAmt(Cr,Cr,2,".,");
		xPrint = GL_Print_Create    (xLayout);
		GL_Widget_SetText      (xPrint, Cr);
		GL_Widget_SetItem      (xPrint, 0, xline++);
		GL_Widget_SetMargins   (xPrint, 0, 0, 10, 0, GL_UNIT_PIXEL);
		GL_Widget_SetFontScale (xPrint, GL_SCALE_SMALL);
		GL_Widget_SetBackAlign (xPrint, GL_ALIGN_RIGHT);

		//---------- Credit Reversals ------------
		xPrint = GL_Print_Create    (xLayout);
		GL_Widget_SetText      (xPrint, "CREDIT REVERSALS");
		GL_Widget_SetItem      (xPrint, 0, xline);
		GL_Widget_SetMargins   (xPrint, 0, 0, 0, 0, GL_UNIT_PIXEL);
		GL_Widget_SetFontScale (xPrint, GL_SCALE_SMALL);
		GL_Widget_SetBackAlign (xPrint, GL_ALIGN_LEFT);

		xPrint = GL_Print_Create    (xLayout);
		GL_Widget_SetText      (xPrint, CrRevCount);
		GL_Widget_SetItem      (xPrint, 0, xline);
		GL_Widget_SetMargins   (xPrint, 200, 0, 0, 0, GL_UNIT_PIXEL);
		GL_Widget_SetFontScale (xPrint, GL_SCALE_SMALL);
		GL_Widget_SetBackAlign (xPrint, GL_ALIGN_LEFT);

		fmtAmt(CrRev,CrRev,2,".,");
		xPrint = GL_Print_Create    (xLayout);
		GL_Widget_SetText      (xPrint, CrRev);
		GL_Widget_SetItem      (xPrint, 0, xline++);
		GL_Widget_SetMargins   (xPrint, 0, 0, 10, 0, GL_UNIT_PIXEL);
		GL_Widget_SetFontScale (xPrint, GL_SCALE_SMALL);
		GL_Widget_SetBackAlign (xPrint, GL_ALIGN_RIGHT);

		//---------- Totals ------------
		xPrint = GL_Print_Create    (xLayout);
		GL_Widget_SetText      (xPrint, "===========================================");
		GL_Widget_SetItem      (xPrint, 0, xline++);
		GL_Widget_SetMargins   (xPrint, 0, 0, 0, 0, GL_UNIT_PIXEL);
		GL_Widget_SetFontScale (xPrint, GL_SCALE_SMALL);
		GL_Widget_SetBackAlign (xPrint, GL_ALIGN_CENTER);

		xPrint = GL_Print_Create    (xLayout);
		GL_Widget_SetText      (xPrint, "TOTALS");
		GL_Widget_SetItem      (xPrint, 0, xline);
		GL_Widget_SetFontStyle (xPrint, GL_FONT_STYLE_BOLD);
		GL_Widget_SetFontScale (xPrint, GL_SCALE_SMALL);
		GL_Widget_SetBackAlign (xPrint, GL_ALIGN_LEFT);

		fmtAmt(Totals,Totals,2,".,");
		xPrint = GL_Print_Create    (xLayout);
		GL_Widget_SetText      (xPrint, Totals);
		GL_Widget_SetItem      (xPrint, 0, xline++);
		GL_Widget_SetMargins   (xPrint, 0, 0, 10, 0, GL_UNIT_PIXEL);
		GL_Widget_SetFontStyle (xPrint, GL_FONT_STYLE_BOLD);
		GL_Widget_SetFontScale (xPrint, GL_SCALE_SMALL);
		GL_Widget_SetBackAlign (xPrint, GL_ALIGN_RIGHT);

		xPrint = GL_Print_Create    (xLayout);
		GL_Widget_SetText      (xPrint, " ");
		GL_Widget_SetItem      (xPrint, 0, xline++);
		GL_Widget_SetFontStyle (xPrint, GL_FONT_STYLE_BOLD);
		GL_Widget_SetFontScale (xPrint, GL_SCALE_SMALL);
		GL_Widget_SetBackAlign (xPrint, GL_ALIGN_LEFT);

		///Get Currency data 1
		memset(Totals, 0, sizeof(Totals));
		memset(CurrencyAlpha, 0, sizeof(CurrencyAlpha));
		memset(CurrencyNumeric, 0, sizeof(CurrencyNumeric));
		strcpy(CurrencyNumeric, "020");
		MAPGET(appCurrCodeAlpha2, CurrencyAlpha, lblKO);
		MAPGET(appCurrCodeNumerc2, &CurrencyNumeric[3], lblKO);

	} //End Of "for (var2 = 0; var2 < 2; var2++) {" for Currency Looping

	///&&&&&&&&&&&&&&&&&&&&&&&&&&&&  print Footer of Receipt &&&&&&&&&&&&&&&&&&&&&&&&&&&
	xline = printReceiptFooter(xLayout, xline);

	// Display Printing in progress
	// ********************
	GL_Dialog_Message(hGoal, NULL, "Please Wait""\n""Printing In Progress", GL_ICON_INFORMATION, GL_BUTTON_NONE, 0);

	ret = GoalPrnDocument(xDocument);              // Print Text document
	CHECK(ret >= 0, lblKO);

	ret = 1;
	goto lblEnd;

	// Errors treatment
	// ****************
	lblKO:                                              // None-classified low level error
	GL_Dialog_Message(hGoal, NULL, "Processing Error", GL_ICON_ERROR, GL_BUTTON_VALID, 5*1000);

	goto lblEnd;
	lblEnd:
	if (xDocument)
		GoalDestroyDocument(&xDocument);            // Destroy

	ClosePeripherals();

	return ret;
}



int LogOn(void){
	int ret = 0;

	return ret;
}

