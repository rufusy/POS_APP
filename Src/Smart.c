//****************************************************************************
//       INGENICO                                INGEDEV 7               
//============================================================================
//       FILE  SMART.C                            (Copyright INGENICO 2012)
//============================================================================
//  Created :       11-July-2012     Kassovic
//  Last modified : 11-July-2012     Kassovic
//  Module : TRAINING                                                          
//                                                                          
//  Purpose :                                                               
//                       *** Smart card management ***
//  How to send a command to the smart card and wait for an answer (apdu).
//                                                                            
//  List of routines in file :  
//      Smart : Smart card demo.                                            
//                            
//  File history :
//  071112-BK : File created
//                                                                           
//****************************************************************************

//****************************************************************************
//      INCLUDES                                                            
//****************************************************************************
#include <globals.h>

//****************************************************************************
//      EXTERN                                                              
//****************************************************************************
extern T_GL_HGRAPHIC_LIB hGoal; // Handle of the graphics object library

//****************************************************************************
//      PRIVATE CONSTANTS                                                   
//****************************************************************************
    /* */

//****************************************************************************
//      PRIVATE TYPES                                                       
//****************************************************************************
    /* */

//****************************************************************************
//      PRIVATE DATA                                                        
//****************************************************************************
// Properties of the Smart screen (Goal)
// =====================================
static const ST_DSP_LINE txSmart[] =
{
	{ {GL_ALIGN_CENTER, GL_ALIGN_CENTER, GL_COLOR_WHITE, GL_COLOR_BLACK, 0, FALSE, {0, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_WHITE}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_XLARGE}}, // Line0
	  {GL_ALIGN_CENTER, GL_ALIGN_CENTER, FALSE,  0, FALSE, {0, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_XLARGE}} },
	{ {GL_ALIGN_CENTER, GL_ALIGN_CENTER, GL_COLOR_WHITE, GL_COLOR_BLACK, 0, FALSE, {0, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_WHITE}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_XLARGE}}, // Line1
	  {GL_ALIGN_CENTER, GL_ALIGN_CENTER, FALSE,  0, FALSE, {0, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_XLARGE}} },
	{ {GL_ALIGN_CENTER, GL_ALIGN_CENTER, GL_COLOR_WHITE, GL_COLOR_BLACK, 0, FALSE, {0, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_WHITE}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_XLARGE}}, // Line2
	  {GL_ALIGN_CENTER, GL_ALIGN_CENTER, FALSE,  0, FALSE, {0, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_XLARGE}} }
};

// Properties of the Cmd screen (Goal)
// ===================================
static const ST_DSP_LINE txCmd[] =
{
	{ {GL_ALIGN_LEFT,   GL_ALIGN_CENTER, GL_COLOR_WHITE, GL_COLOR_BLACK, 100, FALSE, {1, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_WHITE}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}}, // Line0
	  {GL_ALIGN_LEFT,   GL_ALIGN_CENTER, FALSE,  100, FALSE, {2, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}} },
	{ {GL_ALIGN_LEFT,   GL_ALIGN_CENTER, GL_COLOR_WHITE, GL_COLOR_RED,   100, FALSE, {1, 0, 0, 0}, {1, 1, 1, 1, GL_COLOR_WHITE}, {3, 0, 3, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_SMALL}}, // Line1
	  {GL_ALIGN_LEFT,   GL_ALIGN_CENTER, TRUE,   100, FALSE, {2, 4, 0, 4}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_SMALL}} },
	{ {GL_ALIGN_LEFT,   GL_ALIGN_CENTER, GL_COLOR_WHITE, GL_COLOR_BLACK, 100, FALSE, {1, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_WHITE}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}}, // Line2
	  {GL_ALIGN_LEFT,   GL_ALIGN_CENTER, FALSE,  100, FALSE, {2, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}} },
	{ {GL_ALIGN_LEFT,   GL_ALIGN_CENTER, GL_COLOR_WHITE, GL_COLOR_BLACK, 100, FALSE, {1, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_WHITE}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}}, // Line3
	  {GL_ALIGN_LEFT,   GL_ALIGN_CENTER, FALSE,  100, FALSE, {2, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}} },
	{ {GL_ALIGN_CENTER, GL_ALIGN_CENTER, GL_COLOR_WHITE, GL_COLOR_BLACK,   0, FALSE, {0, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_WHITE}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}}, // Line4
	  {GL_ALIGN_CENTER, GL_ALIGN_CENTER, FALSE,    0, FALSE, {0, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_MEDIUM}} }
};

// Properties of the default printer (Goal)
// ========================================
static const ST_PRN_LINE xPrinter =
{
	GL_ALIGN_LEFT, GL_ALIGN_CENTER, FALSE, 100, {0, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {1, 0, 1, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_LARGE}
};

//****************************************************************************
//                          void Smart (void)                            
//  This function sends the application selection command to the card.      
//   - Telium_Fopen() : Open cam0 channel
//   - Telium_Ttestall() : Wait for card insertion from cam0 peripheral
//   - Telium_EMV_power_on() : Retrieve ATR
//   - Telium_EMV_apdu() : Send command for Application Selection
//   - Telium_Ttestall() : Wait for card removal from cam0 peripheral
//   - Telium_Fclose : Close cam0 channel
//  This function has no parameters.    
//  This function has no return value.                                      
//****************************************************************************

void Smart(void) 
{
	// Local variables 
    // ***************
	T_GL_HWIDGET hScreen=NULL;
	T_GL_HWIDGET xDocument=NULL;
	ST_PRN_LINE xLine;
	Telium_File_t *hSmc=NULL;
    HISTORIC xAtr;
	T_APDU xApduC;  // Command APDU
    T_APDU xApduR;  // Response APDU
    char tcDisplay[50+1];
    char tcPrint[1024+1];
    byte tucSnd[262];
    byte tucRsp[262];
	char *pcMsg;
	byte p;  // Printer line index
    int iKey, iRet, iCt, iLen, iSta;

	// Open peripherals
	// ================
    pcMsg = "Processing Error";                               // Error message by default
	hSmc = Telium_Fopen("CAM0", "rw*");                       // Open cam0 peripheral
	CHECK(hSmc!=NULL, lblKO);

    // Prompt Insert a card
    // ********************
	hScreen = GoalCreateScreen(hGoal, txSmart, NUMBER_OF_LINES(txSmart), GL_ENCODING_UTF8);
	CHECK(hScreen!=NULL, lblKO);                              // Create screen and clear it
	iRet = GoalClrScreen(hScreen, GL_COLOR_BLACK, KEY_CANCEL, false);
	CHECK(iRet>=0, lblKO);

	iRet = GoalDspLine(hScreen, 0, "Please Insert", &txSmart[0], 0, false);
	CHECK(iRet>=0, lblKO);                                    // Prompt for a smart card
	iRet = GoalDspLine(hScreen, 1, "Smart Card", &txSmart[1], 0, true);
	CHECK(iRet>=0, lblKO);

    // Wait for smart card
	// *******************
    iRet = TimerStart(0, 30*1000);                            // Timer0 starts to 30s
    CHECK(iRet>=0, lblKO);
	do
	{
		iSta = Telium_Ttestall(CAM0, 10);                     // Wait for the event CAM0
		if (iSta != 0)
			break;                                            // Smart detected
		iKey = GoalGetKey(hScreen, hGoal, true, 100, false);  // Get key pressed/touched (shortcut)
		CHECK(iKey!=GL_KEY_CANCEL, lblEnd);                   // Exit on cancel key
	}while(TimerGet(0) > 0);

	CHECK(TimerGet(0)!=0, lblEnd);                            // Exit on timeout

    // Power on smart card
	// *******************
    iLen=0;
	iRet = Telium_EMV_power_on(hSmc, &xAtr);                  // EMV power on and reset
	if(iRet == 5)                                             // Power once more time on VPP problem detected
		iRet = Telium_EMV_power_on(hSmc, &xAtr);
    switch (iRet)                                             // Card errors
	{                                            
    case 0:                                           break;
	case 1: case 7:         pcMsg = "Card removed";   break;
	case 2: case 4: case 5: pcMsg = "Card Problem";   break;
	case 3:                 pcMsg = "Card mute";      break;
	case 6: default:        pcMsg = "Driver Problem"; break;
    }
	CHECK(iRet==0, lblKO);
	memcpy(tucRsp, xAtr.historic, xAtr.length);               // Retrieve ATR and length
	iLen=xAtr.length;

	GoalDestroyScreen(&hScreen);                              // Destroy screen

	// Display ATR
	// ***********
	hScreen = GoalCreateScreen(hGoal, txCmd, NUMBER_OF_LINES(txCmd), GL_ENCODING_UTF8);
	CHECK(hScreen!=NULL, lblKO);                              // Create new screen and clear it
	iRet = GoalClrScreen(hScreen, GL_COLOR_BLACK, 0, false);
	CHECK(iRet>=0, lblKO);

	iRet = GoalDspLine(hScreen, 0, "ATR:", &txCmd[0], 0, false);
	CHECK(iRet>=0, lblKO);
	for (iCt=0; iCt<iLen; iCt++)                              // Show historic
		Telium_Sprintf (&tcDisplay[2*iCt], "%02X", tucRsp[iCt]);
	iRet = GoalDspLine(hScreen, 1, tcDisplay, &txCmd[1], 0, false);
	CHECK(iRet>=0, lblKO);

	// Print ATR
	// *********
	p=0;
	xDocument = GoalCreateDocument(hGoal, GL_ENCODING_UTF8);
	CHECK(xDocument!=NULL, lblKO);                            // Create document

	xLine = xPrinter;                                         // Build document (Demo, ATR info, Application selection command)
	xLine.eTextAlign = GL_ALIGN_CENTER;
	xLine.bReverse = TRUE;
	xLine.xMargin.usBottom = PIXEL_BOTTOM;
	xLine.xFont.eScale = GL_SCALE_XXLARGE;
	iRet = GoalPrnLine(xDocument, p++, "Smart Demo", &xLine);
	CHECK(iRet>=0, lblKO);

	xLine = xPrinter;
	iRet = GoalPrnLine(xDocument, p++, "ATR:", &xLine);       // ATR info
	CHECK(iRet>=0, lblKO);
	for (iCt=0; iCt<iLen; iCt++)
		Telium_Sprintf(&tcPrint[3*iCt], "%02X ", tucRsp[iCt]);
	iRet = GoalPrnLine(xDocument, p++, tcPrint, &xLine);
	CHECK(iRet>=0, lblKO);

    // Send command to the card for application selection
	// **************************************************
	xApduC.length = 5+14;                                     // APDU length
	memcpy (tucSnd,                                           // APDU request data cla/ins/P1/P2/Le/data
		    "\x00\xA4\x04\x00\x0E"
			"1PAY.SYS.DDF01", 
			xApduC.length); 
    xApduC.data = tucSnd;                                     // Sending buffer
    xApduR.data = tucRsp;                                     // Receiving buffer

    iRet = Telium_EMV_apdu(hSmc, &xApduC, &xApduR);           // Call APDU communication routine
    if(iRet == 0) 
	{
		if(tucRsp[0] == 0x61)                                 // Retrieve APDU response data (PSE found)
		{                                                          
			memcpy(tucSnd, "\x00\xC0\x00\x00", 4);            // APDU request data cla/ins/P1/P2/Le
            tucSnd[4] = tucRsp[1];
            xApduC.length = 5;                                // APDU length
            iRet = Telium_EMV_apdu(hSmc, &xApduC, &xApduR);   // Call APDU communication routine
            if(iRet == 0)
                iLen = xApduR.length;
            else
                iRet = -iRet;
		}
    } 
    else
    	iRet = -iRet;

	// Print and display card response
	// *******************************
	iLen = xApduR.length;
    if(iRet < 0) 
    {
		switch (-iRet)                                        // Card errors
		{                                            
		case 1: case 7:         pcMsg = "Card removed";   break;
        case 2: case 4: case 5: pcMsg = "Card Problem";   break;
        case 3:                 pcMsg = "Card mute";      break;
        case 6: default:        pcMsg = "Driver Problem"; break;
        }
		CHECK(iRet==0, lblKO);
    }
	else
	{
		iRet = GoalPrnLine(xDocument, p++, "APPLICATION SELECTION:", &xLine);
		CHECK(iRet>=0, lblKO);
        if ((tucRsp[iLen-2]==0x6A) && (tucRsp[iLen-1]==0x82)) // PSE not found, List of AIDs Method
        {
    		iRet = GoalPrnLine(xDocument, p++, "No PSE, List AIDs Method", &xLine);
    		CHECK(iRet>=0, lblKO);
			iRet = GoalDspLine(hScreen, 2, "No PSE", &txCmd[2], 0, false);
			CHECK(iRet>=0, lblKO);
			iRet = GoalDspLine(hScreen, 3, "List AIDs Method", &txCmd[3], 0, false);
			CHECK(iRet>=0, lblKO);
        }
		if ((tucRsp[iLen-2]==0x90) && (tucRsp[iLen-1]==0x00)) // PSE found, Directory selection Method
		{
    		iRet = GoalPrnLine(xDocument, p++, "PSE, Directory Selection", &xLine);
    		CHECK(iRet>=0, lblKO);
			iRet = GoalDspLine(hScreen, 2, "PSE found", &txCmd[2], 0, false);
			CHECK(iRet>=0, lblKO);
			iRet = GoalDspLine(hScreen, 3, "Directory Selection", &txCmd[3], 0, false);
			CHECK(iRet>=0, lblKO);
		}
	    for (iCt=0; iCt<iLen; iCt++)                          // Application selection data
	    	Telium_Sprintf(&tcPrint[3*iCt], "%02X ", tucRsp[iCt]);
		iRet = GoalPrnLine(xDocument, p++, tcPrint, &xLine);
		CHECK(iRet>=0, lblKO);
	    //iRet = GoalPrnMultiLine(p++, 33, tcPrint, strlen(tcPrint), &xLine);
		//CHECK(iRet>=0, lblKO);
		iRet = GoalPrnLine(xDocument, p++, "\n\n\n\n", &xLine);
		CHECK(iRet>=0, lblKO);
    }

    iRet=0;
    goto lblEnd;

	// Errors treatment 
    // ****************
lblKO:                                                        // None-classified low level error
	iRet=-1;
	goto lblEnd;
lblEnd:
	TimerStop(0);                                             // Stop Timer0
	// Remove smart card
	// =================
    if (hSmc) {
		Telium_Status (hSmc, (byte*)&iSta);
		switch (iSta)
		{
		case CAM_PRESENT:                                     // *** Card present ***
			if (iRet==0) {
				GoalDspLine(hScreen, 4, "Remove Card", &txCmd[4], 0, true);
				GoalPrnDocument(xDocument);                   // Print document
			} else {
				Telium_Sprintf(tcDisplay, "%s\n%s", pcMsg, "Remove Card");
				GL_Dialog_Message(hGoal, NULL, tcDisplay, GL_ICON_ERROR, GL_BUTTON_NONE, 0);
			}
		    Telium_Power_down(hSmc);                          // Card power off
			Telium_Ttestall(CAM0, 0);                         // Wait for removal
		    ResetPeripherals(KEYBOARD | TSCREEN);             // Reset keyboard/touch FIFO for unexpected keys
			break;	
		default:                                              // *** Card absent ***
			break;
		}
		Telium_Fclose(hSmc);                                  // Close cam0 peripheral
	}
	if (hScreen)
		GoalDestroyScreen(&hScreen);                          // Destroy screen
    if (xDocument)
    	GoalDestroyDocument(&xDocument);                      // Destroy document
}
